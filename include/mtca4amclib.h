/*=========================< begin file & file header >=======================
 *  References
 *  
 *    filename : mtca4amclib.h
 *    author   : RH
 *    company  : IOxOS
 *    creation : September 13,2019
 *
 *----------------------------------------------------------------------------
 *  Description
 *
 *    This file contains the declarations of all exported functions define in
 *    mtca4amclib.c
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

#ifndef _H_MTCA4AMCLIB
#define _H_MTCA4AMCLIB

typedef enum {
  MTCA4_RTM_CLK_IN_ENABLED = 0,
  MTCA4_RTM_CLK_IN_DISABLED
} mtca4_rtm_clk_in_enable_t;

typedef enum {
  MTCA4_RTM_TCLK_IN_ENABLED = 0,
  MTCA4_RTM_TCLK_IN_DISABLED
} mtca4_rtm_tclk_in_enable_t;

typedef enum {
  MTCA4_RTM_CLK_OUT_ENABLED = 0,
  MTCA4_RTM_CLK_OUT_DISABLED
} mtca4_rtm_clk_out_enable_t;


unsigned char set_mtca4_rtm_clk_in_enable(
  mtca4_rtm_clk_in_enable_t state);

unsigned char set_mtca4_rtm_tclk_in_enable(
  mtca4_rtm_tclk_in_enable_t state);

unsigned char set_mtca4_rtm_clk_out_enable(
  mtca4_rtm_clk_out_enable_t state);

#endif


