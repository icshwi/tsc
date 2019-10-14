/*=========================< begin file & file header >=======================
 *  References
 *  
 *    filename : rdt1465lib.h
 *    author   : XP
 *    company  : IOxOS
 *    creation : october 30,2017
 *
 *----------------------------------------------------------------------------
 *  Description
 *
 *    This file contain the declarations of all exported functions define in
 *    rdt1465lib.c
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

#ifndef _H_RDT1465LIB
#define _H_RDT1465LIB

typedef enum {
	RDT1465_EXT_PIN_LOW,
	RDT1465_EXT_PIN_HIGH,
	RDT1465_EXT_PIN_Z
	} rdt1465_ext_pin_state_t;

int rdt1465_init(void);
void rdt1465_dbg(void);
int rdt1465_presence(int fd);
int rdt1465_analog_enable(uint enable);
int rdt1465_gpio_output_enable(uint enable);
int rdt1465_red_led_enable(uint enable);
int rdt1465_extension_presence(uint *present);
int rdt1465_extension_set_pin_state(uint ext_pin_index, rdt1465_ext_pin_state_t state);
int rdt1465_extension_get_pin_state(uint ext_pin_index, uint *state, uint *direction);
#endif /*  _H_RDT1465LIB */
