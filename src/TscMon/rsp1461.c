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

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * Function name : tsc_rsp1461
 * Prototype     : int
 * Parameters    : command line parameter structure
 * Return        : error/success
 *----------------------------------------------------------------------------
 * Description   : manipulation of rsp1461
 *
 *++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

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
	int present       = 0;
	int data          = 0;
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

	if(cnt--) {
// --- INIT ---
		if(((!strcmp("init", c->para[0]))) && (c->cnt == 1)){
			retval = rsp1461_init(); // Check return function
			return retval;
		}

// --- EXTENSION ---
		else if(!strcmp("extension", c->para[0])){

			// Present
			if((!strcmp("present", c->para[1])) && (c->cnt == 2)){
				retval = rsp1461_extension_presence(&present); // Check function return
				if (present){
					printf("rsp1461 extension presence: YES \n");
				}
				else {
					printf("rsp1461 extension presence: NO \n");
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
					printf("Status is : %x \n", ext_state);
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
					return retval;
				}
				return(0);
			}
		}

// --- LED ---
		else if((!strcmp("led", c->para[0]))  && (c->cnt == 3)){
			// ID
			led_id = strtoul(c->para[2], &p, 16);
			if((led_id < 0) || ( led_id > 7)){
				printf("Bad LED id ! \n");
				printf("Available id is 0 to 7 \n");
			    tsc_print_usage(c);
				return(-1);
			}
			// On
			else if(!strcmp("on", c->para[1])){
				retval = rsp1461_led_turn_on(led_id); // Check function return
				return retval;
			}
			// Off
			else if(!strcmp("off", c->para[1])){
				retval = rsp1461_led_turn_off(led_id); // Check function return
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
