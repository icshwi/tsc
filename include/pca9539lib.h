/*=========================< begin file & file header >=======================
 *  References
 *  
 *    filename : rdt1465lib.h
 *    author   : RH
 *    company  : IOxOS
 *    creation : august 09,2019
 *
 *----------------------------------------------------------------------------
 *  Description
 *
 *    This file contain the declarations of all exported functions define in
 *    pca9539lib.c
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

#ifndef _H_PCA9539LIB
#define _H_PCA9539LIB

#include <sys/types.h>

typedef enum {
  PCA9539_PORT_0 = 0,
  PCA9539_PORT_1 = 1
  } pca9539_port_t;

typedef enum {
  PCA9539_OUTPUT_PIN = 0,
  PCA9539_INPUT_PIN = 1
  } pca9539_pin_direction_t;

typedef enum {
  PCA9539_PIN_0_OUT = 0x00,
  PCA9539_PIN_0_IN  = 0x01,
  PCA9539_PIN_1_OUT = 0x00,
  PCA9539_PIN_1_IN  = 0x02,
  PCA9539_PIN_2_OUT = 0x00,
  PCA9539_PIN_2_IN  = 0x04,
  PCA9539_PIN_3_OUT = 0x00,
  PCA9539_PIN_3_IN  = 0x08,
  PCA9539_PIN_4_OUT = 0x00,
  PCA9539_PIN_4_IN  = 0x10,
  PCA9539_PIN_5_OUT = 0x00,
  PCA9539_PIN_5_IN  = 0x20,
  PCA9539_PIN_6_OUT = 0x00,
  PCA9539_PIN_6_IN  = 0x40,
  PCA9539_PIN_7_OUT = 0x00,
  PCA9539_PIN_7_IN  = 0x80
  } pca9539_port_direction_t;


int pca9539_set_port_direction(
  int fd,
  uint i2c_bus,
  uint device_address,
  pca9539_port_t port,
  pca9539_port_direction_t direction);

int pca9539_get_port_direction(
  int fd, 
  uint i2c_bus,
  uint device_address,
  pca9539_port_t port,
  pca9539_port_direction_t *direction);

int pca9539_set_port_state(
  int fd,
  uint i2c_bus,
  uint device_address,
  pca9539_port_t port,
  uint state);

int pca9539_get_port_state(
  int fd,
  uint i2c_bus,
  uint device_address,
  pca9539_port_t port,
  uint *state);

int pca9539_set_pin_direction(
  int fd,
  uint i2c_bus,
  uint device_address,
  pca9539_port_t port,
  uint pin,
  pca9539_pin_direction_t direction);

int pca9539_get_pin_direction(
  int fd,
  uint i2c_bus,
  uint device_address,
  pca9539_port_t port,
  uint pin,
  pca9539_pin_direction_t *direction);

int pca9539_set_pin_state(
  int fd,
  uint i2c_bus,
  uint device_address,
  pca9539_port_t port,
  uint pin,
  uint state);

int pca9539_get_pin_state(
  int fd,
  uint i2c_bus,
  uint device_address,
  pca9539_port_t port,
  uint pin,
  uint *state);


int pca9539_read(int fd, uint i2c_bus, uint device_address, uint reg, uint *data);


#endif /*  _H_PCA9539LIB */
