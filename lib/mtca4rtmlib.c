/*=========================< begin file & file header >=======================
 *  References
 *  
 *    filename : mtca4rtmlib.c
 *    author   : RH
 *    company  : IOxOS
 *    creation : october 18,2017
 *
 *----------------------------------------------------------------------------
 *  Description
 *
 *    This file contains a set of function to act on a MTCA.4 RTM.
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

#include "mtca4rtmlib.h"
#include "ponmboxlib.h"

/*
 * Service requests commands
 */
#define CMD_SET_RTM_LED_STATE                     0x01
#define CMD_GET_RTM_LED_STATE                     0x02

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * Function name : set_mtca4_rtm_led_state
 * Prototype     : unsigned char
 * Parameters    : id, function, on_duration
 * Return        : status of operation
 *----------------------------------------------------------------------------
 * Description   : set led state function
 *
 *++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

unsigned char set_mtca4_rtm_led_state(
  mtca4_rtm_led_id_t id,
  mtca4_rtm_led_function_t function,
  mtca4_rtm_led_on_duration_t on_duration)
{
  mbox_info_t *mbox = get_mbox_info();
  return send_mbox_service_request(mbox, CMD_SET_RTM_LED_STATE, 3, id, function, on_duration);
}

