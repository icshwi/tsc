/*=========================< begin file & file header >=======================
 *  References
 *  
 *    filename : amc.c
 *    author   : RH
 *    company  : IOxOS
 *    creation : September 13,2019
 *
 *----------------------------------------------------------------------------
 *  Description
 *
 *    That file contains the functions to act on a AMC.
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
#include "rtm.h"
#include "ponmboxlib.h"
#include "mtca4amclib.h"
#include "TscMon.h"

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * Function name : amc_rtm_clk_in
 * Prototype     : int
 * Parameters    : pointer to command parameter list
 * Return        : int
 *
 *----------------------------------------------------------------------------
 * Description   : rtm clk_in control
 *
 *++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

int amc_rtm_clk_in(struct cli_cmd_para *c){
	mtca4_rtm_clk_in_enable_t state;

	if ((c->cnt != 2)){
		tsc_print_usage(c);
		return(CLI_ERR);
	}

	if (!strcasecmp(c->para[1], "on")){
		state = MTCA4_RTM_CLK_IN_ENABLED;
	}
	else if (!strcasecmp(c->para[1], "off")){
		state = MTCA4_RTM_CLK_IN_DISABLED;
	}
	else{
		tsc_print_usage(c);
		return(CLI_ERR);
	}

	set_mtca4_rtm_clk_in_enable(state);

	return CLI_OK;
}

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * Function name : amc_rtm_tclk_in
 * Prototype     : int
 * Parameters    : pointer to command parameter list
 * Return        : int
 *
 *----------------------------------------------------------------------------
 * Description   : rtm tclk_in control
 *
 *++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

int amc_rtm_tclk_in(struct cli_cmd_para *c){
	mtca4_rtm_tclk_in_enable_t state;

	if ((c->cnt != 2)){
		tsc_print_usage(c);
		return(CLI_ERR);
	}

	if (!strcasecmp(c->para[1], "on")){
		state = MTCA4_RTM_TCLK_IN_ENABLED;
	}
	else if (!strcasecmp(c->para[1], "off")){
		state = MTCA4_RTM_TCLK_IN_DISABLED;
	}
	else{
		tsc_print_usage(c);
		return(CLI_ERR);
	}

	set_mtca4_rtm_tclk_in_enable(state);

	return CLI_OK;
}

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * Function name : amc_rtm_clk_out
 * Prototype     : int
 * Parameters    : pointer to command parameter list
 * Return        : int
 *
 *----------------------------------------------------------------------------
 * Description   : rtm clk_out control
 *
 *++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

int amc_rtm_clk_out(struct cli_cmd_para *c){
	mtca4_rtm_clk_out_enable_t state;

	if ((c->cnt != 2)){
		tsc_print_usage(c);
		return(CLI_ERR);
	}

	if (!strcasecmp(c->para[1], "on")){
		state = MTCA4_RTM_CLK_OUT_ENABLED;
	}
	else if (!strcasecmp(c->para[1], "off")){
		state = MTCA4_RTM_CLK_OUT_DISABLED;
	}
	else{
		tsc_print_usage(c);
		return(CLI_ERR);
	}

	set_mtca4_rtm_clk_out_enable(state);

	return CLI_OK;
}

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * Function name : tsc_amc
 * Prototype     : int
 * Parameters    : pointer to command parameter list
 * Return        : int
 *
 *----------------------------------------------------------------------------
 * Description   : amc operations
 *
 *++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

int tsc_amc(int fd, struct cli_cmd_para *c){
	int cnt  = 0;
	int i    = 0;
	int data = 0;

	// Check if the board is a IFC14XX
	tsc_pon_read(fd, 0x0, &data);
	data &= 0xFFFFFF00;
	if (data != 0x73571400) {
		printf("Command available only on IFC14xx board\n");
		return (CLI_ERR);
	}

	cnt = c->cnt;
	i = 0;
	if(cnt--){
		if(!strcmp("rtm_clk_in", c->para[i])){
			return(amc_rtm_clk_in(c));
		}
		else if(!strcmp("rtm_tclk_in", c->para[i])){
			return(amc_rtm_tclk_in(c));
		}
		else if(!strcmp("rtm_clk_out", c->para[i])){
			return(amc_rtm_clk_out(c));
		}
	}

	tsc_print_usage(c);
	return(CLI_ERR);
}
