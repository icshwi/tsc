/*=========================< begin file & file header >=======================
 *  References
 *
 *    filename : rsp1461.c
 *    author   : XP
 *    company  : IOxOS
 *    creation : Mars 08,2018
 *
 *----------------------------------------------------------------------------
 *  Description
 *
 *    That file contains a set of function called by TscMon to configure
 *    the RSP1461 interface.
 *
 *----------------------------------------------------------------------------
 *
 *  Copyright Notice
 *
 *    Copyright and all other rights in this document are reserved by
 *    IOxOS Technologies SA. This documents contains proprietary information
 *    and is supplied on express condition that it may not be disclosed,
 *    reproduced in whole or in part, or used for any other purpose other
 *    than that for which it is supplies, without the written consent of
 *    IOxOS Technologies SA
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
#include "rsp1461.h"
#include "TscMon.h"

/*
User I2C devices
 -----------------------------------------------------------------------------
| PCB    | part       | device   | 7-bit   |                                  |
| refdes | name       | function | address | I2C access                       |
|--------+------------+----------+---------+----------------------------------|
| U111   | TCA9548A   | switch   | 0x70    | direct                           |
|--------+------------+----------+---------+----------------------------------|
| U107   | PCA9539BS  | I/O      | 0x74    | direct                           |
|--------+------------+----------+---------+----------------------------------|
| U108   | PCA9539BS  | I/O      | 0x75    | direct                           |
|--------+------------+----------+---------+----------------------------------|
| U120   | PCA9539BS  | I/O      | 0x76    | direct                           |
|--------+------------+----------+---------+----------------------------------|
| U121   | PCA9539BS  | I/O      | 0x77    | direct                           |
|--------+------------+----------+---------+----------------------------------|
| U100   | DS110DF111 | CDR      | 0x18    | switch channel 1 (PCB SFP7)      |
|--------+------------+----------+---------+----------------------------------|
| U102   | DS110DF111 | CDR      | 0x18    | switch channel 2 (PCB SFP6)      |
 -----------------------------------------------------------------------------

U111 I2C switch channels assignation
--------------------------------------------------------------------------------

  * channel 0: PCB SFP5
  * channel 1: PCB SFP7
  * channel 2: PCB SFP6
  * channel 3: PCB SFP1
  * channel 4: PCB SFP2
  * channel 5: PCB SFP3
  * channel 6: PCB SFP4
  * channel 7: extension board (EXT)

U107 I/O expander I/O assignation
 -----------------------------------------------------------------------------
| port | pin | direction | usage                                              |
|------+-----+-----------+----------+-----------------------------------------|
| [0]  | [0] | n/a       | no connect                                         |
|      | [1] | n/a       | no connect                                         |
|      | [2] | n/a       | no connect                                         |
|      | [3] | input     | EXT presence: 0 = present, 1 = absent              |
|      | [4] | n/a       | no connect                                         |
|      | [5] | n/a       | no connect                                         |
|      | [6] | in/out/Z  | EXT I/O 5 (application specific)                   |
|      | [7] | in/out/Z  | EXT I/O 6 (application specific)                   |
|------+-----+-----------+----------+-----------------------------------------|
| [1]  | [0] | output    | SFP5 TX disable: 0 = enable, 1 = disable           |
|      | [1] | output    | SFP5 rate select 0                                 |
|      | [2] | output    | SFP5 rate select 1                                 |
|      | [3] | input     | SFP5 presence: 0 = present, 1 = absent             |
|      | [4] | input     | SFP5 TX fault: 0 = ok, 1 = fault                   |
|      | [5] | input     | SFP5 LOS (loss of signal): 0 = ok, 1 = fault       |
|      | [6] | in/out/Z  | EXT I/O 3 (application specific)                   |
|      | [7] | in/out/Z  | EXT I/O 4 (application specific)                   |
 -----------------------------------------------------------------------------

U108 I/O expander I/O assignation
 -----------------------------------------------------------------------------
| port | pin | direction | usage                                              |
|------+-----+-----------+----------+-----------------------------------------|
| [0]  | [0] | output    | SFP7 TX disable: 0 = enable, 1 = disable           |
|      | [1] | output    | SFP7 rate select 0                                 |
|      | [2] | output    | SFP7 rate select 1                                 |
|      | [3] | input     | SFP7 presence: 0 = present, 1 = absent             |
|      | [4] | input     | SFP7 TX fault: 0 = ok, 1 = fault                   |
|      | [5] | input     | SFP7 LOS (loss of signal): 0 = ok, 1 = fault       |
|      | [6] | in/out/Z  | EXT I/O 0 (application specific)                   |
|      | [7] | in/out/Z  | EXT I/O 1 (application specific)                   |
|------+-----+-----------+----------+-----------------------------------------|
| [1]  | [0] | output    | SFP6 TX disable: 0 = enable, 1 = disable           |
|      | [1] | output    | SFP6 rate select 0                                 |
|      | [2] | output    | SFP6 rate select 1                                 |
|      | [3] | input     | SFP6 presence: 0 = present, 1 = absent             |
|      | [4] | input     | SFP6 TX fault: 0 = ok, 1 = fault                   |
|      | [5] | input     | SFP6 LOS (loss of signal): 0 = ok, 1 = fault       |
|      | [6] | in/out/Z  | EXT I/O 2 (application specific)                   |
|      | [7] | n/a       | no connect                                         |
 -----------------------------------------------------------------------------

U120 I/O expander I/O assignation
 -----------------------------------------------------------------------------
| port | pin | direction | usage                                              |
|------+-----+-----------+----------+-----------------------------------------|
| [0]  | [0] | output    | SFP1 TX disable: 0 = enable, 1 = disable           |
|      | [1] | output    | SFP1 rate select 0                                 |
|      | [2] | output    | SFP1 rate select 1                                 |
|      | [3] | input     | SFP1 presence: 0 = present, 1 = absent             |
|      | [4] | input     | SFP1 TX fault: 0 = ok, 1 = fault                   |
|      | [5] | input     | SFP1 LOS (loss of signal): 0 = ok, 1 = fault       |
|      | [6] | output    | LED126 green: 0 = off, 1 = on                      |
|      | [7] | output    | LED126 red: 0 = off, 1 = on                        |
|------+-----+-----------+----------+-----------------------------------------|
| [1]  | [0] | output    | SFP2 TX disable: 0 = enable, 1 = disable           |
|      | [1] | output    | SFP2 rate select 0                                 |
|      | [2] | output    | SFP2 rate select 1                                 |
|      | [3] | input     | SFP2 presence: 0 = present, 1 = absent             |
|      | [4] | input     | SFP2 TX fault: 0 = ok, 1 = fault                   |
|      | [5] | input     | SFP2 LOS (loss of signal): 0 = ok, 1 = fault       |
|      | [6] | output    | LED125 green: 0 = off, 1 = on                      |
|      | [7] | output    | LED125 red: 0 = off, 1 = on                        |
 -----------------------------------------------------------------------------

U121 I/O expander I/O assignation
 -----------------------------------------------------------------------------
| port | pin | direction | usage                                              |
|------+-----+-----------+----------+-----------------------------------------|
| [0]  | [0] | output    | SFP3 TX disable: 0 = enable, 1 = disable           |
|      | [1] | output    | SFP3 rate select 0                                 |
|      | [2] | output    | SFP3 rate select 1                                 |
|      | [3] | input     | SFP3 presence: 0 = present, 1 = absent             |
|      | [4] | input     | SFP3 TX fault: 0 = ok, 1 = fault                   |
|      | [5] | input     | SFP3 LOS (loss of signal): 0 = ok, 1 = fault       |
|      | [6] | output    | LED124 green: 0 = off, 1 = on                      |
|      | [7] | output    | LED124 red: 0 = off, 1 = on                        |
|------+-----+-----------+----------+-----------------------------------------|
| [1]  | [0] | output    | SFP4 TX disable: 0 = enable, 1 = disable           |
|      | [1] | output    | SFP4 rate select 0                                 |
|      | [2] | output    | SFP4 rate select 1                                 |
|      | [3] | input     | SFP4 presence: 0 = present, 1 = absent             |
|      | [4] | input     | SFP4 TX fault: 0 = ok, 1 = fault                   |
|      | [5] | input     | SFP4 LOS (loss of signal): 0 = ok, 1 = fault       |
|      | [6] | output    | LED123 green: 0 = off, 1 = on                      |
|      | [7] | output    | LED123 red: 0 = off, 1 = on                        |
 -----------------------------------------------------------------------------
*/

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * Function name : tsc_rsp1461
 * Prototype     : int
 * Parameters    : command line parameter structure
 * Return        : error/success
 *----------------------------------------------------------------------------
 * Description   : manipulation of rsp1461
 *
 *++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
static int init_rsp = 0;

int tsc_rsp1461(struct cli_cmd_para *c) {
	char *p;
	int retval        = -1;
	int cnt           = c->cnt;
	int led_id        = 0;

	int sfp_id        = 0;
	int sfp_control   = 0;
	int ext_id        = 0;
	int ext_state     = 0;
	int ext_pin_state = 0;
	int present       = 1;	// Init to "absent"
	int data          = 0;
	rsp1461_led_t           led;
	rsp1461_sfp_control_t   sfp_control_enum;
	rsp1461_ext_pin_state_t ext_pin_state_enum;
	rsp1461_sfp_status_t    sfp_status;

	// Check if the board is a IFC14xx
	tsc_pon_read(0x0, &data);
	if (((data & 0xffffff00) >> 8) != 0x735714) {
		printf("The board is not compatible...\n");
		return -1;
	}

	// Check if card is present
	rsp1461_presence();

	// Execute init automatically one time
	if (init_rsp == 0){
		init_rsp = 1;
		retval = rsp1461_init(); // Check return function
		printf("Initialization done... \n");
	}

	if(cnt--) {
// --- INIT ---
		if(((!strcmp("init", c->para[0]))) && (c->cnt == 1)){
			retval = rsp1461_init(); // Check return function
			printf("Initialization done... \n");
			return retval;
		}

// --- EXTENSION ---
		else if(!strcmp("extension", c->para[0])){

			// Present
			if((!strcmp("present", c->para[1])) && (c->cnt == 2)){
				retval = rsp1461_extension_presence(&present); // Check function return
				if (present){
					printf("rsp1461 extension presence: NO \n");
				}
				else {
					printf("rsp1461 extension presence: YES \n");
				}
				return retval;
			}
			if (c->cnt == 3) {
				// ID
				ext_id = strtoul(c->para[2], &p, 16);
				if((ext_id < 0) || ( ext_id > 6)){
					printf("Bad extension pin id !\n");
					printf("Available id is 0 to 6 \n");
				    tsc_print_usage(c);
					return(-1);
				}
				// Get
				if(!strcmp("get", c->para[1])){
					retval = rsp1461_extension_get_pin_state(ext_id, &ext_state); // Check function return
					printf("Status of pin#%x is : %x \n", ext_id, ext_state);
					return retval;
				}
				// Control
				else {
					ext_pin_state = strtoul(c->para[1], &p, 16);
					if (ext_pin_state == 0){
						ext_pin_state_enum = RSP1461_EXT_PIN_LOW;
					}
					else if (ext_pin_state == 1) {
						ext_pin_state_enum = RSP1461_EXT_PIN_HIGH;
					}
					else if (ext_pin_state == 2) {
						ext_pin_state_enum = RSP1461_EXT_PIN_Z;
					}
					else {
					    printf("Bad arguments -> usage:\n");
					    tsc_print_usage(c);
						return(-1);
					}
					retval = rsp1461_extension_set_pin_state(ext_id, ext_pin_state_enum); // Check function return
					printf("Pin#%x set \n", ext_id);
					return retval;
				}
				return(0);
			}
		}

// --- LED ---
		else if((!strcmp("led", c->para[0]))  && (c->cnt == 3)){
			// Parse LED ID
			led_id = strtoul(c->para[2], &p, 16);
			switch(led_id){
			    case 0:
			        led = RSP1461_LED123_GREEN;
			        break;
			    case 1:
			        led = RSP1461_LED123_RED;
			        break;
			    case 2:
			    	led = RSP1461_LED124_GREEN;
			        break;
			    case 3:
			        led = RSP1461_LED124_RED;
			        break;
			    case 4:
			        led = RSP1461_LED125_GREEN;
			        break;
			    case 5:
			        led = RSP1461_LED125_RED;
			        break;
			    case 6:
			        led = RSP1461_LED126_GREEN;
			        break;
			    case 7:
			        led = RSP1461_LED126_RED;
			        break;
			    default :
					printf("Bad LED id ! \n");
					printf("Available id is 0 to 7 \n");
				    tsc_print_usage(c);
					return(-1);
			}
			// On
			if(!strcmp("on", c->para[1])){
				retval = rsp1461_led_turn_on(led); // Check function return
				return retval;
			}
			// Off
			else if(!strcmp("off", c->para[1])){
				retval = rsp1461_led_turn_off(led); // Check function return
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
		else if((!strcmp("sfp", c->para[0])) && (c->cnt == 3)){
			// ID
			sfp_id = strtoul(c->para[2], &p, 16);
			if((sfp_id < 0) || ( led_id > 6)){
				printf("Bad SFP id !");
				printf("Available id is 0 to 6 \n");
			    tsc_print_usage(c);
				return(-1);
			}
			// Status
			if(!strcmp("status", c->para[1])){
				retval = rsp1461_sfp_status(sfp_id, &sfp_status); // Check function return
				if(sfp_status == SFP_PRESENT){
					printf("SFP_PRESENT");
				}
				else if (sfp_status == SFP_TX_FAULT){
					printf("SFP_TX_FAULT");
				}
				else if (sfp_status == SFP_LOSS_OF_SIGNAL) {
					printf("SFP_LOSS_OF_SIGNAL");
				}
				else {
					printf("Error status ! \n");
				}
				return retval;
			}
			// Control
			else {
				printf("rsp1461 sfp control");
				sfp_control = strtoul(c->para[1], &p, 16);
				if (sfp_control == 0x01){
					sfp_control_enum = SFP_TX_DISABLE;
				}
				else if (sfp_control == 0x02){
					sfp_control_enum = SFP_RX_HIGH_RATE;
				}
				else if (sfp_control == 0x04){
					sfp_control_enum = SFP_TX_HIGH_RATE;
				}
				else {
					printf("Bad arguments -> usage:\n");
				    tsc_print_usage(c);
					return(-1);
				}
				retval = rsp1461_sfp_control(sfp_id, sfp_control_enum); // Check function return
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
