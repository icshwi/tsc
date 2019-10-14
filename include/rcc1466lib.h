/*=========================< begin file & file header >=======================
 *  References
 *  
 *    filename : rcc1466lib.h
 *    author   : XP, RH
 *    company  : IOxOS
 *    creation : July 15,2019
 *
 *----------------------------------------------------------------------------
 *  Description
 *
 *    This file contain the declarations of all exported functions define in
 *    rcc1466lib.c
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

#ifndef _H_RCC1466LIB
#define _H_RCC1466LIB

typedef enum {
	RCC1466_EXT_PIN_LOW,
	RCC1466_EXT_PIN_HIGH,
	RCC1466_EXT_PIN_Z
	} rcc1466_ext_pin_state_t;

typedef enum {
	RCC1466_LED123_GREEN,	// 0
	RCC1466_LED123_RED,		// 1
	RCC1466_LED124_GREEN,	// 2
	RCC1466_LED124_RED,		// 3
	RCC1466_LED125_GREEN,	// 4
	RCC1466_LED125_RED,		// 5
	RCC1466_LED126_GREEN,	// 6
	RCC1466_LED126_RED,		// 7
	} rcc1466_led_t;

typedef enum {
	RCC1466_SFP_FPGA_LANE_0,
	RCC1466_SFP_FPGA_LANE_1,
	RCC1466_SFP_FPGA_LANE_2,
	RCC1466_SFP_FPGA_LANE_3,
	RCC1466_SFP_CPU_SGMII
	} rcc1466_sfp_id_t;

typedef enum {
	SFP_PRESENT        = 0x08,
	SFP_TX_FAULT       = 0x10,
	SFP_LOSS_OF_SIGNAL = 0x20
	} rcc1466_sfp_status_t;

typedef enum {
	SFP_TX_DISABLE     = 0x01,
	SFP_RX_HIGH_RATE   = 0x02,
	SFP_TX_HIGH_RATE   = 0x04
	} rcc1466_sfp_control_t;

int rcc1466_init(int fd);
int rcc1466_presence(int fd);
int rcc1466_extension_presence(int fd, int *present);
int rcc1466_led_turn_on(int fd, rcc1466_led_t led_id);
int rcc1466_led_turn_off(int fd, rcc1466_led_t led_id);
int rcc1466_sfp_status(int fd, rcc1466_sfp_id_t id, uint8_t *status);
int rcc1466_sfp_control(int fd, rcc1466_sfp_id_t id, int sfp_enable, int sfp_rate);
#endif /*  _H_RCC1466LIB */
