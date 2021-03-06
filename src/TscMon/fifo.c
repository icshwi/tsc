/*=========================< begin file & file header >=======================
 *  References
 *
 *    filename : fifo.c
 *    author   : XP
 *    company  : IOxOS
 *    creation : sept 5,2016
 *
 *----------------------------------------------------------------------------
 *  Description
 *
 *     That file contains a set of function to control fifo.
 *
 *----------------------------------------------------------------------------
 *
 *  Copyright (C) IOxOS Technologies SA <ioxos@ioxos.ch>
 *  Copyright (C) 2019  European Spallation Source ERIC
 *
 *    THIS SOFTWARE IS PROVIDED BY THE AUTHOR AND CONTRIBUTORS ``AS IS'' AND
 *    ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 *    IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 *    ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR OR CONTRIBUTORS BE LIABLE
 *    FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 *    DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 *    OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 *    HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 *    LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 *    OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 *    SUCH DAMAGE.
 *
 *    GPL license :
 *    This program is free software; you can redistribute it and/or
 *    modify it under the terms of the GNU General Public License
 *    as published by the Free Software Foundation; either version 2
 *    of the License, or (at your option) any later version.
 *
 *    This program is distributed in the hope that it will be useful,
 *    but WITHOUT ANY WARRANTY; without even the implied warranty of
 *    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *    GNU General Public License for more details.
 *
 *    You should have received a copy of the GNU General Public License
 *    along with this program; if not, write to the Free Software
 *    Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 *
 *=============================< end file header >============================*/

#define DEBUGno
#include <debug.h>
#include <sys/types.h>
#include <sys/time.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <cli.h>
#include <tscioctl.h>
#include <tsculib.h>
#include <aio.h>
#include <errno.h>
#include <ctype.h>
#include <time.h>
#include "TscMon.h"

extern int tsc_fd;

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * Function name : tsc_fifo
 * Prototype     : int
 * Parameters    : command line parameter structure
 * Return        : error/success
 *----------------------------------------------------------------------------
 * Description   : manipulation of fifos
 *
 *++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

int tsc_fifo(struct cli_cmd_para *c) {
	int retval = 0;
	char *p;
	uint sts   = 0;
	uint data  = 0;
	uint tmo   = 0;
	uint mode  = 0;
	retval     = -1;
	int cnt    = c->cnt;
	int i      = 0;
	int idx    = 0;

	// Check FIFO index ----------
	if(c->ext) {
		idx = strtoul( c->ext, &p, 16);
		if((idx < 0) || ( idx > 7)){
			printf("Bad FIFO index : %d\n", idx);
			printf("Available index is 0 to 7 \n");
			return( -1);
		}
	}

	if(cnt--) {

		// Init ----------
		if(((!strcmp( "init", c->para[i]))) && (c->cnt == 2)){
			mode = strtoul( c->para[i+1], &p, 16);
			if (mode == 0) {
				printf("FIFO#%x is initialized in FIFO mode ...\n", idx);
				tsc_fifo_init(tsc_fd, idx, mode);
			}
			else if (mode == 1){
				printf("FIFO#%x is initialized in MAILBOX mode ...\n", idx);
				tsc_fifo_init(tsc_fd, idx, mode);
			}
			else {
			    printf("Not enough arguments -> usage:\n");
			    tsc_print_usage(c);
			}
			return(0);
		}

		// Status ----------
		else if((!strcmp( "status", c->para[i])) && (c->cnt == 1)){
			tsc_fifo_status(tsc_fd, idx, &sts);
			printf("FIFO#%d status                     = 0x%08x\n", idx, sts);
			printf("FIFO#%d current word counter       : 0x%02x\n", idx, (sts & 0x000000ff));
			printf("FIFO#%d current SRAM write pointer : 0x%02x\n", idx, (sts & 0x0000ff00) >> 8);
			printf("FIFO#%d current SRAM read pointer  : 0x%02x\n", idx, (sts & 0x00ff0000) >> 16);
			printf("FIFO#%d not empty                  : %01x\n",   idx, (sts & 0x01000000) >> 24);
			printf("FIFO#%d full                       : %01x\n",   idx, (sts & 0x02000000) >> 25);
			printf("FIFO#%d used as MailBox            : %01x\n",   idx, (sts & 0x04000000) >> 26);
			printf("FIFO#%d error                      : %01x\n",   idx, (sts & 0x08000000) >> 27);
			printf("FIFO#%d reset                      : %01x\n",   idx, (sts & 0x10000000) >> 28);
			printf("FIFO#%d TOSCA memory read enable   : %01x\n",   idx, (sts & 0x20000000) >> 29);
			printf("FIFO#%d TOSCA memory write enable  : %01x\n",   idx, (sts & 0x40000000) >> 30);
			printf("FIFO#%d FIFO enable                : %01x\n",   idx, (sts & 0x80000000) >> 31);
			return(0);
		}

		// Clear ----------
		else if((!strcmp("clear", c->para[i]))  && (c->cnt == 1)){
			tsc_fifo_clear(tsc_fd, idx, &sts);
			printf("FIFO#%d cleared \n", idx);
			return(0);
		}

		// Read ----------
		else if((!strcmp( "read", c->para[i])) && (c->cnt == 1)){
			if(idx != -1){
				if(tsc_fifo_read(tsc_fd, idx, &data, 1, &sts) > 0){
					printf("FIFO#%d data = %08x [%08x]\n", idx, data, sts);
				}
				else{
					printf("FIFO#%d is empty\n", idx);
				}
			}
			return(0);
		}

		// Write ----------
		else if((!strcmp( "write", c->para[i])) && (c->cnt == 2)){
			if(idx != -1){
				if(!cnt){
					printf("Bad parameter! Type \"? fifo\" for help \n");
				}
				data = strtoul( c->para[i+1], &p, 16);
				if(!tsc_fifo_write(tsc_fd, idx, &data, 1, &sts)){
					printf("FIFO#%d is full\n", idx);
				}
			}
			return(0);
		}

		// Wait ----------
		else if((!strcmp( "wait", c->para[i])) && (c->cnt == 2)){
			if(idx != -1){
				tmo = 0;
				if( cnt--){
					tmo = strtoul( c->para[i+1], &p, 10);
					if( p ==  c->para[i+1]){
						printf("%s : Bad timeout value\n", c->para[i+1]);
						return(-1);
					}
				}
				printf("FIFO#%d waiting [%d msec] ...\n", idx, tmo);
				retval = tsc_fifo_wait_ef(tsc_fd, idx, &sts, tmo);		// Check not_empty flag with IRQ
				if( retval == -1){
					printf("Timeout : ");
				}
				printf("Status = %08x\n", sts);
			}
			return 0;
		}
		else {
		    printf("Not enough arguments -> usage:\n");
		    tsc_print_usage(c);
			return( -1);
		}
	}
	else {
	    printf("Not enough arguments -> usage:\n");
	    tsc_print_usage(c);
		return( -1);
	}
    printf("Not enough arguments -> usage:\n");
    tsc_print_usage(c);
	return( -1);
}
