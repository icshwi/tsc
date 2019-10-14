/*=========================< begin file & file header >=======================
 *  References
 *  
 *    filename : rdt1465lib.c
 *    author   : RH
 *    company  : IOxOS
 *    creation : august 09,2019
 *
 *----------------------------------------------------------------------------
 *  Description
 *
 *    That library contains a set of function to handle PCA9539 I2C devices.
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

#include <stdlib.h>
#include <stdio.h>
#include "../include/tsculib.h"
#include "../include/pca9539lib.h"


/* definitions */

#define PCA9539_REG_INPUT_PORT_0              0
#define PCA9539_REG_INPUT_PORT_1              1
#define PCA9539_REG_OUTPUT_PORT_0             2
#define PCA9539_REG_OUTPUT_PORT_1             3
#define PCA9539_REG_POLARITY_INVERSION_PORT_0 4
#define PCA9539_REG_POLARITY_INVERSION_PORT_1 5
#define PCA9539_REG_CONFIGURATION_PORT_0      6
#define PCA9539_REG_CONFIGURATION_PORT_1      7 


/* private functions prototypes */

int pca9539_device(uint i2c_bus, uint device_address);
int pca9539_write(uint i2c_bus, uint device_address, uint reg, uint data);
uint pca9539_set_bit(uint value, uint bit_index, uint bit_state);

extern int tsc_fd;

/**********************************************************************/

int pca9539_device(uint i2c_bus, uint device_address)
{
  int rs     = 1;
  int ds     = 1;
  int device = 0;

  device  = (i2c_bus & 7) << 29;
  device |= device_address & 0x7f;
  device |= ((rs - 1) & 3) << 16;
  device |= ((ds - 1) & 3) << 18;

  return device;
}


int pca9539_read(uint i2c_bus, uint device_address, uint reg, uint *data)
{
  int device = pca9539_device(i2c_bus, device_address);
  return tsc_i2c_read(tsc_fd, device, reg, data);
}


int pca9539_write(uint i2c_bus, uint device_address, uint reg, uint data)
{
  int device = pca9539_device(i2c_bus, device_address);
  return tsc_i2c_write(tsc_fd, device, reg, data);
}


int pca9539_set_port_direction(
      uint i2c_bus,
      uint device_address,
      pca9539_port_t port,
      pca9539_port_direction_t direction)
{
  return pca9539_write(i2c_bus, device_address, PCA9539_REG_CONFIGURATION_PORT_0 + port, direction);
}

int pca9539_get_port_direction(
      uint i2c_bus,
      uint device_address,
      pca9539_port_t port,
      pca9539_port_direction_t *direction)
{
  return pca9539_read(i2c_bus, device_address, PCA9539_REG_CONFIGURATION_PORT_0 + port, direction);
}

int pca9539_set_port_state(
      uint i2c_bus,
      uint device_address,
      pca9539_port_t port,
      uint state)
{
  return pca9539_write(i2c_bus, device_address, PCA9539_REG_OUTPUT_PORT_0 + port, state);
}


int pca9539_get_port_state(
      uint i2c_bus,
      uint device_address,
      pca9539_port_t port,
      uint *state)
{
  return pca9539_read(i2c_bus, device_address, PCA9539_REG_INPUT_PORT_0 + port, state);
}


uint pca9539_set_bit(uint value, uint bit_index, uint bit_state)
{
  bit_state = (bit_state) ? 1 : 0;
  value &= ~(1 << bit_index);
  value |= bit_state << bit_index;
  return value;
}


int pca9539_set_pin_direction(
  uint i2c_bus,
  uint device_address,
  pca9539_port_t port,
  uint pin,
  pca9539_pin_direction_t direction)
{
  int retval = 0;
  uint port_direction;

  if (pin > 7) return 1;

  retval = pca9539_get_port_direction(i2c_bus, device_address, port, &port_direction);
  port_direction = pca9539_set_bit(port_direction, pin, direction);
  retval = pca9539_set_port_direction(i2c_bus, device_address, port, port_direction);
  return retval;
}


int pca9539_get_pin_direction(
  uint i2c_bus,
  uint device_address,
  pca9539_port_t port,
  uint pin,
  pca9539_pin_direction_t *direction)
{
  int retval = 0;
  uint port_direction;

  if (pin > 7) return 1;

  retval = pca9539_get_port_direction(i2c_bus, device_address, port, &port_direction);
  (*direction) = (port_direction >> pin) & 0x01;
  return retval;
}


int pca9539_set_pin_state(
  uint i2c_bus,
  uint device_address,
  pca9539_port_t port,
  uint pin,
  uint state)
{
  int retval = 0;
  uint port_state;

  if (pin > 7) return 1;

  retval = pca9539_get_port_state(i2c_bus, device_address, port, &port_state);
  port_state = pca9539_set_bit(port_state, pin, state);
  retval = pca9539_set_port_state(i2c_bus, device_address, port, port_state);
  return retval;
}


int pca9539_get_pin_state(
  uint i2c_bus,
  uint device_address,
  pca9539_port_t port,
  uint pin,
  uint *state)
{
  int retval = 0;
  uint port_state;

  if (pin > 7) return 1;

  retval = pca9539_get_port_state(i2c_bus, device_address, port, &port_state);
  (*state) = (port_state >> pin) & 0x01;
  return retval;
}
