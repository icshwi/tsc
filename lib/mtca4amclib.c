/*=========================< begin file & file header >=======================
 *  References
 *
 *    filename : mtca4amclib.c
 *    author   : RH
 *    company  : IOxOS
 *    creation : September 13,2019
 *
 *----------------------------------------------------------------------------
 *  Description
 *
 *    This file contains a set of function to act on a MTCA.4 AMC.
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

#include "mtca4amclib.h"
#include "ponmboxlib.h"

/*
 * Service requests commands
 */
#define CMD_SET_RTM_CLK_IN_ENABLE                 0x06
#define CMD_SET_RTM_TCLK_IN_ENABLE                0x07
#define CMD_SET_RTM_CLK_OUT_ENABLE                0x08

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * Function name : set_mtca4_rtm_clk_in_enable
 * Prototype     : mtca4_rtm_clk_in_enable_t
 * Parameters    : state
 * Return        : status of operation
 *----------------------------------------------------------------------------
 * Description   : enable/disable CLK_IN output to RTM
 *
 *++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

unsigned char set_mtca4_rtm_clk_in_enable(
  int fd,
  mtca4_rtm_clk_in_enable_t state)
{
  mbox_info_t *mbox = get_mbox_info(fd);
  unsigned char status = send_mbox_service_request(
                           fd,
                           mbox,
                           CMD_SET_RTM_CLK_IN_ENABLE,
                           1,
                           state);
  free_mbox_info(mbox);
  return status;
}

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * Function name : set_mtca4_rtm_tclk_in_enable
 * Prototype     : mtca4_rtm_tclk_in_enable_t
 * Parameters    : state
 * Return        : status of operation
 *----------------------------------------------------------------------------
 * Description   : enable/disable TCLK_IN output to RTM
 *
 *++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

unsigned char set_mtca4_rtm_tclk_in_enable(
  int fd,
  mtca4_rtm_tclk_in_enable_t state)
{
  mbox_info_t *mbox = get_mbox_info(fd);
  unsigned char status = send_mbox_service_request(
                           fd,
                           mbox,
                           CMD_SET_RTM_TCLK_IN_ENABLE,
                           1,
                           state);
  free_mbox_info(mbox);
  return status;
}

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * Function name : set_mtca4_rtm_clk_out_enable
 * Prototype     : mtca4_rtm_clk_out_enable_t
 * Parameters    : state
 * Return        : status of operation
 *----------------------------------------------------------------------------
 * Description   : enable/disable CLK_OUT input from RTM
 *
 *++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

unsigned char set_mtca4_rtm_clk_out_enable(
  int fd,
  mtca4_rtm_clk_out_enable_t state)
{
  mbox_info_t *mbox = get_mbox_info(fd);
  unsigned char status = send_mbox_service_request(
                           fd,
                           mbox,
                           CMD_SET_RTM_CLK_OUT_ENABLE,
                           1,
                           state);
  free_mbox_info(mbox);
  return status;
}

