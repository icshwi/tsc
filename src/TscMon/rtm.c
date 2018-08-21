/*=========================< begin file & file header >=======================
 *  References
 *  
 *    filename : rtm.c
 *    author   : RH
 *    company  : IOxOS
 *    creation : Oct 18,2017
 *
 *----------------------------------------------------------------------------
 *  Description
 *
 *    That file contains the functions to drive RTM.
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
#include "mtca4rtmlib.h"
#include "TscMon.h"

<<<<<<< HEAD
extern int tsc_fd;

char *
rtm_rcsid()
{
  return( rcsid);
}

=======
>>>>>>> 97f16a8... Cosmetic, add -Wall compilation flag to clean [XP]
/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * Function name : rtm_led
 * Prototype     : int
 * Parameters    : pointer to command parameter list
 * Return        : int
 *
 *----------------------------------------------------------------------------
 * Description   : rtm led
 *
 *++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

<<<<<<< HEAD
int 
rtm_led( struct cli_cmd_para *c)
{
  int retval;
  mtca4_rtm_led_id_t led_id;
  mtca4_rtm_led_function_t led_function;
  mtca4_rtm_led_on_duration_t led_on_duration;

  if ((c->cnt != 3))
  {
    tsc_print_usage( c);
    return( CLI_ERR);
  }

  if (!strcasecmp(c->para[1], "red"))
  {
    led_id = MTCA4_RTM_RED_LED;
  }
  else if (!strcasecmp(c->para[1], "green"))
  {
    led_id = MTCA4_RTM_GREEN_LED;
  }
  else
  {
    tsc_print_usage( c);
    return( CLI_ERR);
  }

  if (!strcasecmp(c->para[2], "on"))
  {
    led_function = MTCA4_RTM_LED_ON;
  }
  else if (!strcasecmp(c->para[2], "off"))
  {
    led_function = MTCA4_RTM_LED_OFF;
  }
  else if (!strcasecmp(c->para[2], "slow_blink"))
  {
    led_function = MTCA4_RTM_LED_BLINK_500MS_OFF;
    led_on_duration = MTCA4_RTM_LED_BLINK_500MS_ON;
  }
  else if (!strcasecmp(c->para[2], "fast_blink"))
  {
    led_function = MTCA4_RTM_LED_BLINK_100MS_OFF;
    led_on_duration = MTCA4_RTM_LED_BLINK_100MS_ON;
  }
  else
  {
    tsc_print_usage( c);
    return( CLI_ERR);
  }

  set_mtca4_rtm_led_state(tsc_fd, led_id, led_function, led_on_duration);

  return CLI_OK;
=======
int rtm_led(struct cli_cmd_para *c){
	mtca4_rtm_led_id_t led_id;
	mtca4_rtm_led_function_t led_function;
	mtca4_rtm_led_on_duration_t led_on_duration;

	if ((c->cnt != 3)){
		tsc_print_usage(c);
		return(CLI_ERR);
	}

	if (!strcasecmp(c->para[1], "red")){
		led_id = MTCA4_RTM_RED_LED;
	}
	else if (!strcasecmp(c->para[1], "green")){
		led_id = MTCA4_RTM_GREEN_LED;
	}
	else{
		tsc_print_usage(c);
		return(CLI_ERR);
	}

	if (!strcasecmp(c->para[2], "on")){
		led_function = MTCA4_RTM_LED_ON;
	}
	else if (!strcasecmp(c->para[2], "off")){
		led_function = MTCA4_RTM_LED_OFF;
	}
	else if (!strcasecmp(c->para[2], "slow_blink")){
		led_function = MTCA4_RTM_LED_BLINK_500MS_OFF;
		led_on_duration = MTCA4_RTM_LED_BLINK_500MS_ON;
	}
	else if (!strcasecmp(c->para[2], "fast_blink")){
		led_function = MTCA4_RTM_LED_BLINK_100MS_OFF;
		led_on_duration = MTCA4_RTM_LED_BLINK_100MS_ON;
	}
	else{
		tsc_print_usage(c);
		return(CLI_ERR);
	}

	set_mtca4_rtm_led_state(led_id, led_function, led_on_duration);

	return CLI_OK;
>>>>>>> 97f16a8... Cosmetic, add -Wall compilation flag to clean [XP]
}

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * Function name : rtm_eeprom
 * Prototype     : int
 * Parameters    : pointer to command parameter list
 * Return        : int
 *
 *----------------------------------------------------------------------------
 * Description   : rtm eeprom write-protect control
 *
 *++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

int rtm_eeprom(struct cli_cmd_para *c){
	mtca4_rtm_eeprom_wp_t state;

	if ((c->cnt != 2)){
		tsc_print_usage(c);
		return(CLI_ERR);
	}

	if (!strcasecmp(c->para[1], "we")){
		state = MTCA4_RTM_EEPROM_WRITE_ENABLED;
	}
	else if (!strcasecmp(c->para[1], "wp")){
		state = MTCA4_RTM_EEPROM_WRITE_DISABLED;
	}
	else{
		tsc_print_usage(c);
		return(CLI_ERR);
	}

<<<<<<< HEAD
  set_mtca4_rtm_eeprom_wp(tsc_fd, state);
=======
	set_mtca4_rtm_eeprom_wp(state);
>>>>>>> 97f16a8... Cosmetic, add -Wall compilation flag to clean [XP]

	return CLI_OK;
}

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * Function name : rtm_reset
 * Prototype     : int
 * Parameters    : pointer to command parameter list
 * Return        : int
 *
 *----------------------------------------------------------------------------
 * Description   : rtm reset control
 *
 *++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

int rtm_reset(struct cli_cmd_para *c){
	mtca4_rtm_reset_t state;

	if ((c->cnt != 2)){
		tsc_print_usage(c);
		return(CLI_ERR);
	}

	if (!strcasecmp(c->para[1], "on")){
		state = MTCA4_RTM_RESET_ASSERTED;
	}
	else if (!strcasecmp(c->para[1], "off")){
		state = MTCA4_RTM_RESET_DEASSERTED;
	}
	else{
		tsc_print_usage(c);
		return(CLI_ERR);
	}

	set_mtca4_rtm_reset(tsc_fd, state);

	return CLI_OK;
}

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * Function name : rtm_zone3
 * Prototype     : int
 * Parameters    : pointer to command parameter list
 * Return        : int
 *
 *----------------------------------------------------------------------------
 * Description   : rtm zone3 control
 *
 *++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

int rtm_zone3(struct cli_cmd_para *c){
	mtca4_rtm_eeprom_wp_t state;

	if ((c->cnt != 2)){
		tsc_print_usage(c);
		return(CLI_ERR);
	}

	if (!strcasecmp(c->para[1], "on")){
		state = MTCA4_RTM_ZONE3_ENABLED;
	}
	else if (!strcasecmp(c->para[1], "off")){
		state = MTCA4_RTM_ZONE3_DISABLED;
	}
	else{
		tsc_print_usage(c);
		return(CLI_ERR);
	}

<<<<<<< HEAD
  set_mtca4_rtm_zone3_enable(tsc_fd, state);
=======
	set_mtca4_rtm_zone3_enable(state);
>>>>>>> 97f16a8... Cosmetic, add -Wall compilation flag to clean [XP]

	return CLI_OK;
}

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * Function name : tsc_rtm
 * Prototype     : int
 * Parameters    : pointer to command parameter list
 * Return        : int
 *
 *----------------------------------------------------------------------------
 * Description   : rtm operation
 *
 *++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

<<<<<<< HEAD
int 
tsc_rtm( struct cli_cmd_para *c)
{
  int cnt  = 0;
  int i    = 0;
  int data = 0;

  // Check if the board is a IFC14XX
  tsc_pon_read(tsc_fd, 0x0, &data);
  data &= 0xFFFFFF00;
  if (data != 0x73571400) {
	printf("Command available only on IFC14xx board\n");
	return (CLI_ERR);
  }

  cnt = c->cnt;
  i = 0;
  if( cnt--)
  {
    if( !strcmp( "led", c->para[i]))
    {
      return( rtm_led( c));
    }
    else if( !strcmp( "reset", c->para[i]))
    {
      return( rtm_reset( c));
    }
    else if( !strcmp( "eeprom", c->para[i]))
    {
      return( rtm_eeprom( c));
    }
    else if( !strcmp( "zone3", c->para[i]))
    {
      return( rtm_zone3( c));
    }
  }

  tsc_print_usage( c);
  return( CLI_ERR);
=======
int tsc_rtm(struct cli_cmd_para *c){
	int cnt  = 0;
	int i    = 0;
	int data = 0;

	// Check if the board is a IFC14XX
	tsc_pon_read(0x0, &data);
	data &= 0xFFFFFF00;
	if (data != 0x73571400) {
		printf("Command available only on IFC14xx board\n");
		return (CLI_ERR);
	}

	cnt = c->cnt;
	i = 0;
	if(cnt--){
		if(!strcmp("led", c->para[i])){
			return(rtm_led(c));
		}
		else if(!strcmp("reset", c->para[i])){
			return(rtm_reset(c));
		}
		else if(!strcmp("eeprom", c->para[i])){
			return(rtm_eeprom(c));
		}
		else if(!strcmp("zone3", c->para[i])){
			return(rtm_zone3(c));
		}
	}

	tsc_print_usage(c);
	return(CLI_ERR);
>>>>>>> 97f16a8... Cosmetic, add -Wall compilation flag to clean [XP]
}
