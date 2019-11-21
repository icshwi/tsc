/*=========================< begin file & file header >=======================
 *  References
 *
 *    filename : rcc1466.c
 *    author   : XP, RH
 *    company  : IOxOS
 *    creation : July 15,2019
 *
 *----------------------------------------------------------------------------
 *  Description
 *
 *    That file contains a set of function called by TscMon to configure
 *    the RCC1466 interface.
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
#include <rcc1466lib.h>
#include <aio.h>
#include <errno.h>
#include <ctype.h>
#include <time.h>
#include "rcc1466.h"
#include "TscMon.h"

extern int tsc_fd;

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * Function name : tsc_rcc1466
 * Prototype     : int
 * Parameters    : command line parameter structure
 * Return        : error/success
 *----------------------------------------------------------------------------
 * Description   : manipulation of rcc1466
 *
 *++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

int tsc_rcc1466(struct cli_cmd_para *c) {
	char *p;
	int retval        = -1;
	int cnt           = c->cnt;
	int led_id        = 0;
	int i             = 0;
	int sfp_id        = 0;
	int sfp_enable    = 0;
	int sfp_rate      = 0;
	int sfp_tx_rate   = 0;
	int sfp_rx_rate   = 0;
	int present       = 1;	// Init to "absent"
	int data          = 0;
	rcc1466_led_t           led;
	uint8_t                 sfp_status;
	char aa[] = "   ";
	char bb[] = "   ";
	char cc[] = "   ";
	char *strs[7] = {"fpga0", "fpga1", "fpga2", "fpga3", "eth0", "eth1", "eth2"};

	// Check if the board is a IFC14xx
	tsc_pon_read(tsc_fd, 0x0, &data);
	if (((data & 0xffffff00) >> 8) != 0x735714) {
		printf("The board is not compatible...\n");
		return -1;
	}

	// Check if card is present
	if (rcc1466_presence(tsc_fd)) {
		return -1;
	}

	if(cnt--) {
// --- INIT ---
		if(((!strcmp("init", c->para[0]))) && (c->cnt == 1)){
			retval = rcc1466_init(tsc_fd);
			printf("Initialization done... \n");
			return retval;
		}

// --- EXTENSION ---
		else if(!strcmp("extension", c->para[0])){

			// Present
			if((!strcmp("present", c->para[1])) && (c->cnt == 2)){
				retval = rcc1466_extension_presence(tsc_fd, &present);
				if (present){
					printf("NO \n");
				}
				else {
					printf("YES \n");
				}
				return retval;
			}
			return(0);
		}

// --- LED ---
		else if((!strcmp("led", c->para[0]))  && (c->cnt == 3)){
			// Parse LED ID
			led_id = strtoul(c->para[2], &p, 16);
			switch(led_id){
			    case 0:
			        led = RCC1466_LED123_GREEN;
			        break;
			    case 1:
			        led = RCC1466_LED123_RED;
			        break;
			    case 2:
			    	led = RCC1466_LED124_GREEN;
			        break;
			    case 3:
			        led = RCC1466_LED124_RED;
			        break;
			    case 4:
			        led = RCC1466_LED125_GREEN;
			        break;
			    case 5:
			        led = RCC1466_LED125_RED;
			        break;
			    case 6:
			        led = RCC1466_LED126_GREEN;
			        break;
			    case 7:
			        led = RCC1466_LED126_RED;
			        break;
			    default :
					printf("Bad LED id ! \n");
					printf("Available id is 0 to 7 \n");
				    tsc_print_usage(c);
					return(-1);
			}
			// On
			if(!strcmp("on", c->para[1])){
				retval = rcc1466_led_turn_on(tsc_fd, led);
				return retval;
			}
			// Off
			else if(!strcmp("off", c->para[1])){
				retval = rcc1466_led_turn_off(tsc_fd, led);
				return retval;
			}
			else {
			    printf("Bad arguments -> usage:\n");
			    tsc_print_usage(c);
				return(-1);
			}
			return(0);
		}

// --- SFP ---
		else if((!strcmp("sfp", c->para[0]))){
			if (c->cnt == 3){
				// ID
				if(!strncmp("fpga0", c->para[2], 5)){
					sfp_id = 0;
				}
				else if(!strncmp("fpga1", c->para[2], 5)){
					sfp_id = 1;
				}
				else if(!strncmp("fpga2", c->para[2], 5)){
					sfp_id = 2;
				}
				else if(!strncmp("fpga3", c->para[2], 5)){
					sfp_id = 3;
				}
				else if(!strncmp("eth0", c->para[2], 4)){
					sfp_id = 4;
				}
				else if(!strncmp("all", c->para[2], 4)){
					sfp_id = 7;
				}
				else {
					printf("Bad SFP id !\n");
					tsc_print_usage(c);
					return -1;
				}

				// Status
				if(!strcmp("status", c->para[1])){
					if (sfp_id == 7){
						printf(" ------------------------------------------------  \n");
						printf("|       | Present | TX fault | RX loss of signal | \n");
						for (i = 0; i < 5; i++){
							retval = rcc1466_sfp_status(tsc_fd, i, &sfp_status);
							if(sfp_status & (SFP_PRESENT)){
								strcpy(aa, "no ");
								strcpy(bb, "n/a");
								strcpy(cc, "n/a");
							}
							else {
								strcpy(aa, "yes");
								if(sfp_status & (SFP_TX_FAULT)){
									strcpy(bb, "yes");
								}
								else{
									strcpy(bb, "no ");
								}
								if(sfp_status & (SFP_LOSS_OF_SIGNAL)){
									strcpy(cc, "yes");
								}
								else{
									strcpy(cc, "no ");
								}
							}
							printf("|-------+---------+----------+-------------------| \n");
							if (i < 4){
								printf("| %s | %s     | %s      | %s               | \n", strs[i], aa, bb, cc);
							}
							else{
								printf("| %s  | %s     | %s      | %s               | \n", strs[i], aa, bb, cc);
							}
						}
						 printf(" ------------------------------------------------ \n");
					}
					else {
						retval = rcc1466_sfp_status(tsc_fd, sfp_id, &sfp_status);
						printf("sfp status: \n");
						if(sfp_status & (SFP_PRESENT)){
							printf("   Present:           no \n");
							printf("   TX fault:          n/a \n");
							printf("   RX loss of signal: n/a \n");
						}
						else {
							printf("   Present:           yes \n");
							if(sfp_status & (SFP_TX_FAULT)){
								printf("   TX fault:          yes \n");
							}
							else{
								printf("   TX fault:          no \n");
							}
							if(sfp_status & (SFP_LOSS_OF_SIGNAL)){
								printf("   RX loss of signal: yes \n");
							}
							else{
								printf("   RX loss of signal: no \n");
							}
						}
					}
					return retval;
				}
				else {
				    printf("Bad parameter -> usage:\n");
				    tsc_print_usage(c);
					return( -1);
				}
			}
			else if ((c->cnt == 6)){
				// ID
				if(!strncmp("fpga0", c->para[5], 5)){
					sfp_id = 0;
				}
				else if(!strncmp("fpga1", c->para[5], 5)){
					sfp_id = 1;
				}
				else if(!strncmp("fpga2", c->para[5], 5)){
					sfp_id = 2;
				}
				else if(!strncmp("fpga3", c->para[5], 5)){
					sfp_id = 3;
				}
				else if(!strncmp("eth0", c->para[5], 4)){
					sfp_id = 4;
				}
				else {
					printf("Bad SFP id !\n");
					tsc_print_usage(c);
					return -1;
				}
				// Control
				if(!strcmp("control", c->para[1])){
					if(!strncmp("enable", c->para[2], 6)){
						sfp_enable = 0;
					}
					else if(!strncmp("disable", c->para[2], 7)){
						sfp_enable = 1;
					}
					else {
						printf("Bad SFP enable value !\n");
						tsc_print_usage(c);
						return(-1);

					}
					if(!strncmp("low", c->para[3], 3)){
						sfp_rx_rate = 0;
					}
					else if(!strncmp("high", c->para[3], 4)){
						sfp_rx_rate = 1;
					}
					else {
						printf("Bad SFP rx_rate value !\n");
						tsc_print_usage(c);
						return(-1);
					}
					if(!strncmp("low", c->para[4], 3)){
						sfp_tx_rate = 0;
					}
					else if(!strncmp("high", c->para[4], 4)){
						sfp_tx_rate = 1;
					}
					else {
						printf("Bad SFP tx_rate value !\n");
						tsc_print_usage(c);
						return(-1);
					}
					if ((sfp_tx_rate == 0) && (sfp_rx_rate == 0)){
						sfp_rate = 0;
					}
					else if ((sfp_tx_rate == 0) && (sfp_rx_rate == 1)){
						sfp_rate = 1;
					}
					else if ((sfp_tx_rate == 1) && (sfp_rx_rate == 0)){
						sfp_rate = 2;
					}
					else if ((sfp_tx_rate == 1) && (sfp_rx_rate == 1)){
						sfp_rate = 3;
					}
					retval = rcc1466_sfp_control(tsc_fd, sfp_id, sfp_enable, sfp_rate);
					return retval;
				}
				else {
				    printf("Bad parameter -> usage:\n");
				    tsc_print_usage(c);
					return( -1);
				}
			}
			else {
			    printf("Not enough arguments -> usage:\n");
			    tsc_print_usage(c);
				return( -1);
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
