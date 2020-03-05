/*=========================< begin file & file header >=======================
 *  References
 *  
 *    filename : fbi1482lib.c
 *    author   : RH
 *    company  : IOxOS
 *    creation : December 11,2019
 *
 *----------------------------------------------------------------------------
 *  Description
 *
 *    That library contains a set of functions to handle the FBI_1482
 *    mezzanine card.
 *
 *----------------------------------------------------------------------------
 *
 *  Copyright (C) IOxOS Technologies SA <ioxos@ioxos.ch>
 *
 *    THIS SOFTWARE IS PROVIDED BY THE AUTHOR AND CONTRIBUTORS ``AS IS'' AND
 *    ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 *    IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 *    ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR OR CONTRIBUTORS BE LIABLE
 *    FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 *    DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 *    OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 *    HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 *    LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 *    OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 *    SUCH DAMAGE.
 *
 *    GPL license :
 *    This program is free software; you can redistribute it and/or
 *    modify it under the terms of the GNU General Public License
 *    as published by the Free Software Foundation; either version 2
 *    of the License, or (at your option) any later version.
 *
 *    This program is distributed in the hope that it will be useful,
 *    but WITHOUT ANY WARRANTY; without even the implied warranty of
 *    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *    GNU General Public License for more details.
 *
 *    You should have received a copy of the GNU General Public License
 *    along with this program; if not, write to the Free Software
 *    Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 *
 *=============================< end file header >============================*/

#include <stdlib.h>
#include <stdio.h>
#include "../include/ponmboxlib.h"
#include "../include/tsculib.h"
#include "../include/rcc1466lib.h"
#include "../include/fbi1482lib.h"
#include "../include/pca9539lib.h"
#include "../include/i2ceepromlib.h"

/*
User I2C devices
 ---------------------------------------------------------------------------------
| PCB    | part       | device       | 7-bit   |                                  |
| refdes | name       | function     | address | I2C access                       |
|--------+------------+--------------+---------+----------------------------------|
| U110   | TMP102A    | T° sensor    | 0x48    | Depends on the carrier board     |
|--------+------------+--------------+---------+----------------------------------|
| U112   | 24AA025UID | UID EEPROM   | 0x50    | Depends on the carrier board     |
|--------+------------+--------------+---------+----------------------------------|
| U111   | PCA9539BS  | I/O expander | 0x75    | Depends on the carrier board     |
 ---------------------------------------------------------------------------------


U111 I/O expander I/O assignation
 -----------------------------------------------------------------------------
| port | pin | direction | usage                                              |
|------+-----+-----------+----------------------------------------------------|
| [0]  | [0] | output    | RS485_TXEN0                                        |
|      | [1] | output    | RS485_TXEN1                                        |
|      | [2] | output    | RS485_TXEN2                                        |
|      | [3] | output    | RS485_TXEN3                                        |
|      | [4] | output    | RS485_RXEN_N0                                      |
|      | [5] | output    | RS485_RXEN_N1                                      |
|      | [6] | output    | RS485_RXEN_N2                                      |
|      | [7] | output    | RS485_RXEN_N3                                      |
|------+-----+-----------+----------------------------------------------------|
| [1]  | [0] | output    | FP_RS485_LED_GREEN_A                               |
|      | [1] | output    | FP_RS485_LED_RED_A                                 |
|      | [2] | output    | DBG_LED_GREEN1_A                                   |
|      | [3] | output    | DBG_LED_GREEN2_A                                   |
|      | [4] | output    | DBG_LED_RED1_A                                     |
|      | [5] | output    | DBG_LED_RED2_A                                     |
|      | [6] | n/a       | n/a                                                |
|      | [7] | input     | POWER_GOOD                                         |
 -----------------------------------------------------------------------------
*/

#define U110_I2C_ADDRESS 0x48

#define U111_I2C_ADDRESS 0x75
#define U111_PORT_0_INITIAL_STATE 0xf0
#define U111_PORT_1_INITIAL_STATE 0x00

#define U111_PORT_0_DIRECTIONS (PCA9539_PIN_0_OUT | \
                                PCA9539_PIN_1_OUT | \
                                PCA9539_PIN_2_OUT | \
                                PCA9539_PIN_3_OUT | \
                                PCA9539_PIN_4_OUT | \
                                PCA9539_PIN_5_OUT | \
                                PCA9539_PIN_6_OUT | \
                                PCA9539_PIN_7_OUT)

#define U111_PORT_1_DIRECTIONS (PCA9539_PIN_0_OUT | \
                                PCA9539_PIN_1_OUT | \
                                PCA9539_PIN_2_OUT | \
                                PCA9539_PIN_3_OUT | \
                                PCA9539_PIN_4_OUT | \
                                PCA9539_PIN_5_OUT | \
                                PCA9539_PIN_6_IN  | \
                                PCA9539_PIN_7_IN)

#define U111_PORT_0_PIN_RS485_TXEN0           0
#define U111_PORT_0_PIN_RS485_TXEN1           1
#define U111_PORT_0_PIN_RS485_TXEN2           2
#define U111_PORT_0_PIN_RS485_TXEN3           3
#define U111_PORT_0_PIN_RS485_RXEN_N0         4
#define U111_PORT_0_PIN_RS485_RXEN_N1         5
#define U111_PORT_0_PIN_RS485_RXEN_N2         6
#define U111_PORT_0_PIN_RS485_RXEN_N3         7

#define U111_PORT_1_PIN_FP_RS485_LED_GREEN_A  0
#define U111_PORT_1_PIN_FP_RS485_LED_RED_A    1
#define U111_PORT_1_PIN_DBG_LED_GREEN1_A      2
#define U111_PORT_1_PIN_DBG_LED_GREEN2_A      3
#define U111_PORT_1_PIN_DBG_LED_RED1_A        4
#define U111_PORT_1_PIN_DBG_LED_RED2_A        5
#define U111_PORT_1_PIN_POWER_GOOD            7

#define U112_I2C_ADDRESS 0x50
#define U112_I2C_ADDRESS_BIT_WIDTH 8


/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * Function name : fbi1482_presence
 * Prototype     : int
 * Parameters    : int *present
 * Return        : status of operation
 *----------------------------------------------------------------------------
 * Description   : FBI1482 board support function, check presence
 *
 *++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

int fbi1482_presence(uint i2c_bus)
{
  int retval = 0;

// TODO: check presence of FBI_1482 mezzanine

  return retval;
}

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * Function name : fbi1482_init
 * Prototype     : int
 * Parameters    : void
 * Return        : status of operation
 *----------------------------------------------------------------------------
 * Description   : Initialization function
 *
 *++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

int fbi1482_init_u111(int fd, uint i2c_bus)
{
  int retval = 0;
  retval  = pca9539_set_port_state(fd, i2c_bus, U111_I2C_ADDRESS, PCA9539_PORT_0, U111_PORT_0_INITIAL_STATE);
  retval |= pca9539_set_port_state(fd, i2c_bus, U111_I2C_ADDRESS, PCA9539_PORT_1, U111_PORT_1_INITIAL_STATE);
  retval |= pca9539_set_port_direction(fd, i2c_bus, U111_I2C_ADDRESS, PCA9539_PORT_0, U111_PORT_0_DIRECTIONS);
  retval |= pca9539_set_port_direction(fd, i2c_bus, U111_I2C_ADDRESS, PCA9539_PORT_1, U111_PORT_1_DIRECTIONS);
  // tsc_i2c_write() is broken -> ignore return value
  retval = 0;
  return retval;
}


int fbi1482_init(int fd, uint i2c_bus)
{
  return fbi1482_init_u111(fd, i2c_bus);
}

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * Function name : fbi1482_get_uid
 * Prototype     : int
 * Parameters    : void
 * Return        : status of operation
 *----------------------------------------------------------------------------
 * Description   : read unique id from UID EEPROM
 *
 *++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

int fbi1482_get_uid(int fd, uint i2c_bus, unsigned char *uid)
{
  int retval  = 0;

  retval = i2ceeprom_read(fd, i2c_bus, U112_I2C_ADDRESS, 0xfc, U112_I2C_ADDRESS_BIT_WIDTH, 4, uid);
  if (retval)
  {
    puts("Failed to read I2C EEPROM");
    return retval;
  }
  
  return retval;
}

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * Function name : fbi1482_get_temp
 * Prototype     : int
 * Parameters    : void
 * Return        : status of operation
 *----------------------------------------------------------------------------
 * Description   : read on-board temperature sensor
 *
 *++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

int fbi1482_get_temp(int fd, uint i2c_bus, int *temp)
{
  int retval = 0;
  int device = 0;
  int addr   = U110_I2C_ADDRESS;
  int reg    = 0;
  int rs     = 1;
  int ds     = 1;
  uint data;
  signed char byte_temp;

  device = (i2c_bus & 7) << 29; device |= addr & 0x7f; device |= ((rs - 1) & 3) << 16; device |= ((ds - 1) & 3) << 18;
  retval = tsc_i2c_read(fd, device, reg, &data);
  byte_temp = (signed char)(data & 0xFF);
  *temp = (signed int)(byte_temp);
  // tsc_i2c_read() is broken -> ignore return value
  retval = 0;
  return retval;
}

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * Function name : fbi1482_rs485_set_indicator
 * Prototype     : int
 * Parameters    : void
 * Return        : status of operation
 *----------------------------------------------------------------------------
 * Description   : Set state of RS485 indicator
 *
 *++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

int fbi1482_rs485_set_indicator(int fd, uint i2c_bus, fbi1482_rs485_indicator_t indicator, int state)
{
  return pca9539_set_pin_state(fd, i2c_bus, U111_I2C_ADDRESS, PCA9539_PORT_1, indicator, state);
}

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * Function name : fbi1482_rs485_txen
 * Prototype     : int
 * Parameters    : void
 * Return        : status of operation
 *----------------------------------------------------------------------------
 * Description   : Enable/disable RS485 transceiver
 *
 *++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

int fbi1482_rs485_txen(int fd, uint i2c_bus, fbi1482_rs485_txen_t channel, int state)
{
  return pca9539_set_pin_state(fd, i2c_bus, U111_I2C_ADDRESS, PCA9539_PORT_0, channel, state);
}

