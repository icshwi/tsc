/*=========================< begin file & file header >=======================
 *  References
 *  
 *    filename : mtca4rtmlib.h
 *    author   : RH
 *    company  : IOxOS
 *    creation : october 18,2017
 *
 *----------------------------------------------------------------------------
 *  Description
 *
 *    This file contains the declarations of all exported functions define in
 *    mtca4rtmlib.c
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

#ifndef _H_MTCA4RTMLIB
#define _H_MTCA4RTMLIB

typedef enum {
  MTCA4_RTM_BLUE_LED = 0,
  MTCA4_RTM_RED_LED,
  MTCA4_RTM_GREEN_LED,
  MTCA4_RTM_NUM_LEDS
} mtca4_rtm_led_id_t;

// as defined in PICMG 3.0, table 3-31
typedef enum {
  MTCA4_RTM_LED_OFF             = 0x00,
  MTCA4_RTM_LED_BLINK_100MS_OFF = 0x0A,
  MTCA4_RTM_LED_BLINK_500MS_OFF = 0x32,
  MTCA4_RTM_LED_LAMP_TEST       = 0xFB,
  MTCA4_RTM_LED_RESTORE         = 0xFC,
  MTCA4_RTM_LED_RESERVED1       = 0xFD,
  MTCA4_RTM_LED_RESERVED2       = 0xFE,
  MTCA4_RTM_LED_ON              = 0xFF
} mtca4_rtm_led_function_t;

typedef enum {
  MTCA4_RTM_LED_BLINK_100MS_ON = 0x0A,
  MTCA4_RTM_LED_BLINK_500MS_ON = 0x32,
} mtca4_rtm_led_on_duration_t;

typedef enum {
  MTCA4_RTM_EEPROM_WRITE_ENABLED = 0,
  MTCA4_RTM_EEPROM_WRITE_DISABLED
} mtca4_rtm_eeprom_wp_t;

typedef enum {
  MTCA4_RTM_RESET_ASSERTED = 0,
  MTCA4_RTM_RESET_DEASSERTED
} mtca4_rtm_reset_t;

typedef enum {
  MTCA4_RTM_ZONE3_ENABLED = 0,
  MTCA4_RTM_ZONE3_DISABLED
} mtca4_rtm_zone3_enable_t;


unsigned char set_mtca4_rtm_led_state(
  mtca4_rtm_led_id_t id,
  mtca4_rtm_led_function_t function,
  mtca4_rtm_led_on_duration_t on_duration);

unsigned char set_mtca4_rtm_eeprom_wp(
  mtca4_rtm_eeprom_wp_t state);

unsigned char set_mtca4_rtm_reset(
  mtca4_rtm_reset_t state);

unsigned char set_mtca4_rtm_zone3_enable(
  mtca4_rtm_zone3_enable_t state);

#endif


