/*=========================< begin file & file header >=======================
 *  References
 *  
 *    filename : rsp1461lib.h
 *    author   : XP
 *    company  : IOxOS
 *    creation : october 30,2017
 *
 *----------------------------------------------------------------------------
 *  Description
 *
 *    This file contain the declarations of all exported functions define in
 *    rsp1461lib.c
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

#ifndef _H_RSP1461LIB
#define _H_RSP1461LIB

typedef enum {
	RSP1461_EXT_PIN_LOW,
	RSP1461_EXT_PIN_HIGH,
	RSP1461_EXT_PIN_Z
	} rsp1461_ext_pin_state_t;

typedef enum {
	RSP1461_LED123_GREEN,	// 0
	RSP1461_LED123_RED,		// 1
	RSP1461_LED124_GREEN,	// 2
	RSP1461_LED124_RED,		// 3
	RSP1461_LED125_GREEN,	// 4
	RSP1461_LED125_RED,		// 5
	RSP1461_LED126_GREEN,	// 6
	RSP1461_LED126_RED,		// 7
	} rsp1461_led_t;

typedef enum {
	RSP1461_SFP_FPGA_LANE_0,
	RSP1461_SFP_FPGA_LANE_1,
	RSP1461_SFP_FPGA_LANE_2,
	RSP1461_SFP_FPGA_LANE_3,
	RSP1461_SFP_CPU_SGMII,
	RSP1461_SFP_CPU_XFI_LANE_0,
	RSP1461_SFP_CPU_XFI_LANE_1
	} rsp1461_sfp_id_t;

typedef enum {
	SFP_PRESENT        = 0x08,
	SFP_TX_FAULT       = 0x10,
	SFP_LOSS_OF_SIGNAL = 0x20
	} rsp1461_sfp_status_t;

typedef enum {
	SFP_TX_DISABLE     = 0x01,
	SFP_RX_HIGH_RATE   = 0x02,
	SFP_TX_HIGH_RATE   = 0x04
	} rsp1461_sfp_control_t;

int rsp1461_init(int tsc_fd);
int rsp1461_presence(int tsc_fd);
int rsp1461_extension_presence(int tsc_fd, int *present);
int rsp1461_extension_set_pin_state(int tsc_fd,int index, rsp1461_ext_pin_state_t state);
int rsp1461_extension_get_pin_state(int tsc_fd,int index, int *state, int *direction);
int rsp1461_led_turn_on(int tsc_fd,rsp1461_led_t led_id);
int rsp1461_led_turn_off(int tsc_fd,rsp1461_led_t led_id);
int rsp1461_sfp_status(int tsc_fd,rsp1461_sfp_id_t id, uint8_t *status);
int rsp1461_sfp_control(int tsc_fd,rsp1461_sfp_id_t id, int sfp_enable, int sfp_rate);
#endif /*  _H_RSP1461LIB */
