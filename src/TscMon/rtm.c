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

#ifndef lint
static char *rcsid = "$Id: rtm.c,v 1.0 2017/10/18 08:26:51 ioxos Exp $";
#endif

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

char *
rtm_rcsid()
{
  return( rcsid);
}

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

  set_mtca4_rtm_led_state(led_id, led_function, led_on_duration);

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

int 
tsc_rtm( struct cli_cmd_para *c)
{
  int cnt  = 0;
  int i    = 0;
  int data = 0;

  // Check if the board is a IFC1410
  tsc_pon_read(0x0, &data);
  if (data != 0x73571410) {
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
  }

  tsc_print_usage( c);
  return( CLI_ERR);
}
