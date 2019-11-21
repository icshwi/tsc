/*=========================< begin file & file header >=======================
 *  References
 *
 *    filename : rdt1465.c
 *    author   : XP
 *    company  : IOxOS
 *    creation : Mars 08,2018
 *
 *----------------------------------------------------------------------------
 *  Description
 *
 *    That file contains a set of function called by TscMon to configure
 *    the RDT1465 interface.
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
#include <rdt1465lib.h>
#include <aio.h>
#include <errno.h>
#include <ctype.h>
#include <time.h>
#include "rdt1465.h"
#include "TscMon.h"

extern int tsc_fd;

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++n
 * Function name : tsc_rdt1465
 * Prototype     : int
 * Parameters    : command line parameter structure
 * Return        : error/success
 *----------------------------------------------------------------------------
 * Description   : manipulation of rdt1465
 *
 *++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

int tsc_rdt1465(struct cli_cmd_para *c) {
	char *p;
	int retval        = -1;
	int cnt           = c->cnt;
	int i             = 0;
	int ext_id        = 0;
	uint ext_state    = 0;
	uint present      = 1;	// Init to "absent"
	int data          = 0;
	uint direction    = 0;
	rdt1465_ext_pin_state_t ext_pin_state_enum;

	// Check if the board is a IFC14xx
	tsc_pon_read(tsc_fd, 0x0, &data);
	if (((data & 0xffffff00) >> 8) != 0x735714) {
		printf("The board is not compatible...\n");
		return -1;
	}

	// Check if card is present
	if (rdt1465_presence(tsc_fd)) {
		return -1;
	}

	if(cnt--) {
// --- INIT ---
		if(((!strcmp("init", c->para[0]))) && (c->cnt == 1)){
			retval = rdt1465_init(tsc_fd);
			printf("Initialization done... \n");
			return retval;
		}

// --- DBG ---
		if(((!strcmp("dbg", c->para[0]))) && (c->cnt == 1)){
			rdt1465_dbg(tsc_fd);
			return 0;
		}

// --- ANALOG ---
		else if(((!strcmp("analog", c->para[0]))) && (c->cnt == 2)){
			if(!strcmp("on", c->para[1])){
				return rdt1465_analog_enable(tsc_fd, 1);
			}
			else if(!strcmp("off", c->para[1])){
				return rdt1465_analog_enable(tsc_fd, 0);
			}
			else{
				tsc_print_usage(c);
				return(-1);
			}
		}

// --- GPIO ---
		else if(((!strcmp("gpio_out", c->para[0]))) && (c->cnt == 2)){
			if(!strcmp("on", c->para[1])){
				return rdt1465_gpio_output_enable(tsc_fd, 1);
			}
			else if(!strcmp("off", c->para[1])){
				return rdt1465_gpio_output_enable(tsc_fd, 0);
			}
			else{
				tsc_print_usage(c);
				return(-1);
			}
		}

// --- RED LED ---
		else if(((!strcmp("red_led", c->para[0]))) && (c->cnt == 2)){
			if(!strcmp("on", c->para[1])){
				return rdt1465_red_led_enable(tsc_fd, 1);
			}
			else if(!strcmp("off", c->para[1])){
				return rdt1465_red_led_enable(tsc_fd, 0);
			}
			else{
				tsc_print_usage(c);
				return(-1);
			}
		}

// --- EXTENSION ---
		else if(!strcmp("extension", c->para[0])){

			// Present
			if((!strcmp("present", c->para[1])) && (c->cnt == 2)){
				retval = rdt1465_extension_presence(tsc_fd, &present);
				if (present){
					printf("NO \n");
				}
				else {
					printf("YES \n");
				}
				return retval;
			}
			// Status
			if((!strcmp("status", c->para[1])) && (c->cnt == 2)){
				// Check if extension board is present
				retval = rdt1465_extension_presence(tsc_fd, &present);
				if (present){
					printf("No extension board \n");
					return -1;
				}
				else {
					// Get all status
					printf(" --------------------------  \n");
					printf("| I/O  | direction | state | \n");
					for (i = 0; i < 12; i++){
						retval = rdt1465_extension_get_pin_state(tsc_fd, i, &ext_state, &direction);
						printf("|------+-----------+-------| \n");
						if(direction == 1){
							printf("| [%2d] | in        | %x     | \n", i, ext_state);
						}
						else if (direction == 0){
							printf("| [%2d] | out       | %x     | \n", i, ext_state);
						}
					}
						printf(" --------------------------  \n");
				}
				return retval;
			}

			if (c->cnt == 3) {
				// ID
				ext_id = strtoul(c->para[2], &p, 16);
				if((ext_id < 0) || ( ext_id > 11)){
					printf("Bad extension pin id !\n");
					printf("Available id is 0 to 11 \n");
					tsc_print_usage(c);
					return(-1);
				}
				// Get
				if(!strcmp("get", c->para[1])){
					retval = rdt1465_extension_get_pin_state(tsc_fd, ext_id, &ext_state, &direction);
					printf("%x \n", ext_state);
					return retval;
				}
				// Control
				else {
					if(!strncmp("z", c->para[1], 1)){
						ext_pin_state_enum = RDT1465_EXT_PIN_Z;
					}
					else if(!strncmp("0", c->para[1], 1)){
						ext_pin_state_enum = RDT1465_EXT_PIN_LOW;
					}
					else if(!strncmp("1", c->para[1], 1)){
						ext_pin_state_enum = RDT1465_EXT_PIN_HIGH;
					}
					else {
					    printf("Bad arguments -> usage:\n");
					    tsc_print_usage(c);
						return(-1);
					}
				}
				retval = rdt1465_extension_set_pin_state(tsc_fd, ext_id, ext_pin_state_enum);
				return retval;
			}
			return(0);
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
