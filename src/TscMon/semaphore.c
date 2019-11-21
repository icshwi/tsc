/*=========================< begin file & file header >=======================
 *  References
 *
 *    filename : semaphore.c
 *    author   : XP
 *    company  : IOxOS
 *    creation : Sept 4,2016
 *
 *----------------------------------------------------------------------------
 *  Description
 *
 *    That file contains a set of function called by TscMon to perform
 *    semaphore operation on the board.
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
 * Function name : tsc_semaphore
 * Prototype     : int
 * Parameters    : cli_cmd_para structure
 * Return        : error/success
 *----------------------------------------------------------------------------
 * Description   : main command to drive semaphore
 *
 *++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

int tsc_semaphore(struct cli_cmd_para *c){
	int cnt  = 0;
	int idx  = 0;
	cnt      = c->cnt;
	uint sts = 0;
	uint tag = 0;
	char *p;

	// Select sub command and check syntax
	if(cnt--) {
		if((!strcmp("status", c->para[0])) && (c->cnt == 1)) {
			tsc_semaphore_status(tsc_fd, &sts);
			printf("SEMAPHORE global status = 0x%08x\n", sts);
			printf("SEMAPHORE#0             : %01x\n", (sts & 0x00000001));
			printf("SEMAPHORE#1             : %01x\n", (sts & 0x00000002) >> 1);
			printf("SEMAPHORE#2             : %01x\n", (sts & 0x00000004) >> 2);
			printf("SEMAPHORE#3             : %01x\n", (sts & 0x00000008) >> 3);
			printf("SEMAPHORE#4             : %01x\n", (sts & 0x00000010) >> 4);
			printf("SEMAPHORE#5             : %01x\n", (sts & 0x00000020) >> 5);
			printf("SEMAPHORE#6             : %01x\n", (sts & 0x00000040) >> 6);
			printf("SEMAPHORE#7             : %01x\n", (sts & 0x00000080) >> 7);
			printf("SEMAPHORE#8             : %01x\n", (sts & 0x00000100) >> 8);
			printf("SEMAPHORE#9             : %01x\n", (sts & 0x00000200) >> 9);
			printf("SEMAPHORE#A             : %01x\n", (sts & 0x00000400) >> 10);
			printf("SEMAPHORE#B             : %01x\n", (sts & 0x00000800) >> 11);
			printf("SEMAPHORE#C             : %01x\n", (sts & 0x00001000) >> 12);
			printf("SEMAPHORE#D             : %01x\n", (sts & 0x00002000) >> 13);
			printf("SEMAPHORE#E             : %01x\n", (sts & 0x00004000) >> 14);
			printf("SEMAPHORE#F             : %01x\n", (sts & 0x00008000) >> 15);
			return(0);
		}
		else if((!strcmp("release", c->para[0])) && (c->cnt == 2)){
			if(!cnt){
			    printf("Not enough arguments -> usage:\n");
			    tsc_print_usage(c);
				return(-1);
			}
			idx = strtoul( c->para[1], &p, 16);
			if((idx < 0) || (idx > 15)){
				printf("Bad semaphore index : %x \n", idx);
				printf("Available index is 0 to F \n");
				return(-1);
			}
			else{
				tsc_semaphore_release(tsc_fd, idx, 0);
				printf("SEMAPHORE#%d released \n", idx);
				return(0);
			}
		}
		else if((!strcmp("get", c->para[0])) && (c->cnt == 3)){
			if(!cnt){
			    printf("Not enough arguments -> usage:\n");
			    tsc_print_usage(c);
				return(-1);
			}
			idx = strtoul(c->para[1], &p, 16);
			if((idx < 0) || (idx > 15)){
				printf("Bad semaphore index : %x \n", idx);
				printf("Available index is 0 to F \n");
				return(-1);
			}
			else{
				tag = strtoul(c->para[2], &p, 16);
				if(tsc_semaphore_get(tsc_fd, idx, tag) == 3){
					printf("Semaphore not available !\n");
				}
				else{
					printf("Semaphore acquired ! \n");
				}
				return(0);
			}
		}
		else {
		    printf("Not enough arguments -> usage:\n");
		    tsc_print_usage(c);
			return(-1);
		}
	}
    printf("Not enough arguments -> usage:\n");
    tsc_print_usage(c);
	return 0;
}
