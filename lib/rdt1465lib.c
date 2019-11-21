/*=========================< begin file & file header >=======================
 *  References
 *  
 *    filename : rdt1465lib.c
 *    author   : XP, RH
 *    company  : IOxOS
 *    creation : october 18,2017
 *
 *----------------------------------------------------------------------------
 *  Description
 *
 *    That library contains a set of function to handle the RDT_1465 mTCA.4.1 RTM.
 *
 *----------------------------------------------------------------------------
 *
 *  Copyright (C) IOxOS Technologies SA <ioxos@ioxos.ch>
 *  Copyright (C) 2019  European Spallation Source ERIC
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
#include "../include/rdt1465lib.h"
#include "../include/pca9539lib.h"

/*
User I2C devices
 -----------------------------------------------------------------------------
| PCB    | part       | device   | 7-bit   |                                  |
| refdes | name       | function | address | I2C access                       |
|--------+------------+----------+---------+----------------------------------|
| U112   | PCA9539BS  | I/O      | 0x74    | direct                           |
 -----------------------------------------------------------------------------

U107 I/O expander I/O assignation
 -----------------------------------------------------------------------------
| port | pin | direction | usage                                              |
|------+-----+-----------+----------------------------------------------------|
| [0]  | [0] | output    | relays enable: 0 = disable, 1 = enable             |
|      | [1] | input     | EXT presence: 0 = present, 1 = absent              |
|      | [1] | output    | SMA GPIO output enable: 0 = enable, 1 = disable    |
|      | [2] | in/out/Z  | EXT I/O 11 (application specific)                  |
|      | [4] | in/out/Z  | EXT I/O 10 (application specific)                  |
|      | [5] | in/out/Z  | EXT I/O 9 (application specific)                   |
|      | [6] | in/out/Z  | EXT I/O 8 (application specific)                   |
|      | [7] | in/out/Z  | EXT I/O 7 (application specific)                   |
|------+-----+-----------+----------------------------------------------------|
| [1]  | [0] | in/out/Z  | EXT I/O 6 (application specific)                   |
|      | [1] | in/out/Z  | EXT I/O 5 (application specific)                   |
|      | [2] | in/out/Z  | EXT I/O 4 (application specific)                   |
|      | [3] | in/out/Z  | EXT I/O 3 (application specific)                   |
|      | [4] | in/out/Z  | EXT I/O 2 (application specific)                   |
|      | [5] | in/out/Z  | EXT I/O 1 (application specific)                   |
|      | [6] | in/out/Z  | EXT I/O 0 (application specific)                   |
|      | [7] | output    | Front-panel LED2B: 0 = on, 1 = off                 |
 -----------------------------------------------------------------------------
*/

#define CF_I2C_BUS 4
#define U112_I2C_ADDRESS 0x74
#define U112_PORT_0_INITIAL_STATE 0xff
#define U112_PORT_1_INITIAL_STATE 0xff

#define U112_PORT_0_DIRECTIONS (PCA9539_PIN_0_OUT | \
                                PCA9539_PIN_1_IN  | \
                                PCA9539_PIN_2_OUT | \
                                PCA9539_PIN_3_IN  | \
                                PCA9539_PIN_4_IN  | \
                                PCA9539_PIN_5_IN  | \
                                PCA9539_PIN_6_IN  | \
                                PCA9539_PIN_7_IN)

#define U112_PORT_1_DIRECTIONS (PCA9539_PIN_0_IN  | \
                                PCA9539_PIN_1_IN  | \
                                PCA9539_PIN_2_IN  | \
                                PCA9539_PIN_3_IN  | \
                                PCA9539_PIN_4_IN  | \
                                PCA9539_PIN_5_IN  | \
                                PCA9539_PIN_6_IN  | \
                                PCA9539_PIN_7_OUT)

#define PORT_0_PIN_RS_RELAY_EN   0
#define PORT_0_PIN_EXT_PRESENT_N 1
#define PORT_0_PIN_SMA_GPIO_OE_N 2

#define PORT_1_PIN_FP_LED2B_K    7

uint ext_pin_to_gpio_index[] = { 6, 5, 4, 3, 2, 1, 0, 7, 6, 5, 4, 3 };


/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * Function name : rdt1465_presence
 * Prototype     : int
 * Parameters    : int *present
 * Return        : status of operation
 *----------------------------------------------------------------------------
 * Description   : RDT1465 board support function, check presence
 *
 *++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

#define RDT_1465_ZONE3_INTERFACE_DESIGNATOR    0x00011465

int rdt1465_presence(int fd) {
	int retval = 0;
	/*int offset = 0;*/
	/*unsigned char magic_byte;*/

	// Get mbox info for RDT presence
	mbox_info_t *info = get_mbox_info(fd);
	if (!info){
		printf("Allocation of mbox info structure failed \n");
		return (-1);
	}

	switch (info->rtm_status)
	{
	case RTM_STATUS_ABSENT:
		printf("No RTM\n");
		free_mbox_info(info);
		return (-1);

	case RTM_STATUS_INCOMPATIBLE:
		printf("Incompatible RTM\n");
		free_mbox_info(info);
		return (-1);

	case RTM_STATUS_COMPATIBLE_NO_PAYLOAD_POWER:
		printf("No RTM payload power\n");
		free_mbox_info(info);
		return (-1);

	case RTM_STATUS_COMPATIBLE_HAS_PAYLOAD_POWER:
		break;

	default:
		printf("Unknown RTM status: %d\n", info->rtm_status);
		free_mbox_info(info);
		return (-1);
	}

	if (info->rtm_manufacturer_id != IOXOS_MANUFACTURER_ID ||
	    info->rtm_zone3_interface_designator != RDT_1465_ZONE3_INTERFACE_DESIGNATOR)
	{
		printf("RTM present but not a supported RDT_1465:\n");
		printf("  manufacturer id: 0x%06x\n", info->rtm_manufacturer_id);
		printf("  zone3 interface designator:0x%08x\n", info->rtm_zone3_interface_designator);
		free_mbox_info(info);
		return (-1);
	}

	free_mbox_info(info);
	return retval;
}

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * Function name : rdt1465_init
 * Prototype     : int
 * Parameters    : void
 * Return        : status of operation
 *----------------------------------------------------------------------------
 * Description   : Initialization function
 *
 *++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

int rdt1465_init_u112(int fd)
{
	int retval = 0;
	retval  = pca9539_set_port_state(fd, CF_I2C_BUS, U112_I2C_ADDRESS, PCA9539_PORT_0, 0xff);
	retval |= pca9539_set_port_state(fd, CF_I2C_BUS, U112_I2C_ADDRESS, PCA9539_PORT_1, 0xff);
	retval |= pca9539_set_port_direction(fd, CF_I2C_BUS, U112_I2C_ADDRESS, PCA9539_PORT_0, U112_PORT_0_DIRECTIONS);
	retval |= pca9539_set_port_direction(fd, CF_I2C_BUS, U112_I2C_ADDRESS, PCA9539_PORT_1, U112_PORT_1_DIRECTIONS);
	return retval;
}


int rdt1465_init(int fd){
	return rdt1465_init_u112(fd);
}


/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * Function name : rdt1465_dbg
 * Prototype     : void
 * Parameters    : void
 * Return        : nothing
 *----------------------------------------------------------------------------
 * Description   : display configuration of I/O expander, for debug purpose
 *
 *++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

void rdt1465_dbg(int fd)
{
        uint data;
        pca9539_read(fd, CF_I2C_BUS, U112_I2C_ADDRESS, 0, &data);
        printf("reg 0 (in 0):  0x%02x\n", data);
        pca9539_read(fd, CF_I2C_BUS, U112_I2C_ADDRESS, 1, &data);
        printf("reg 1 (in 1):  0x%02x\n", data);
        pca9539_read(fd, CF_I2C_BUS, U112_I2C_ADDRESS, 6, &data);
        printf("reg 6 (dir 0): 0x%02x\n", data);
        pca9539_read(fd, CF_I2C_BUS, U112_I2C_ADDRESS, 7, &data);
        printf("reg 7 (dir 1): 0x%02x\n", data);
}

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * Function name : rdt1465_analog_enable
 * Prototype     : int
 * Parameters    : uint enable
 * Return        : status of operation
 *----------------------------------------------------------------------------
 * Description   : Enable/disable analog section (ADC/DAC)
 *
 *++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

int rdt1465_analog_enable(int fd, uint enable)
{
        enable = enable ? 0 : 1;
        return pca9539_set_pin_state(fd, CF_I2C_BUS, U112_I2C_ADDRESS, PCA9539_PORT_0, PORT_0_PIN_RS_RELAY_EN, enable);
}

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * Function name : rdt1465_gpio_output_enable
 * Prototype     : int
 * Parameters    : uint enable
 * Return        : status of operation
 *----------------------------------------------------------------------------
 * Description   : Enable/disable the GPIO output
 *
 *++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

int rdt1465_gpio_output_enable(int fd, uint enable)
{
        enable = enable ? 0 : 1;
        return pca9539_set_pin_state(fd, CF_I2C_BUS, U112_I2C_ADDRESS, PCA9539_PORT_0, PORT_0_PIN_SMA_GPIO_OE_N, enable);
}

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * Function name : rdt1465_red_led_enable
 * Prototype     : int
 * Parameters    : uint enable
 * Return        : status of operation
 *----------------------------------------------------------------------------
 * Description   : Turn on/off LED2B (red)
 *
 *++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

int rdt1465_red_led_enable(int fd, uint enable)
{
        enable = enable ? 0 : 1;
        return pca9539_set_pin_state(fd, CF_I2C_BUS, U112_I2C_ADDRESS, PCA9539_PORT_1, PORT_1_PIN_FP_LED2B_K, enable);
}

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * Function name : rdt1465_extension_presence
 * Prototype     : int
 * Parameters    : int *present
 * Return        : status of operation
 *----------------------------------------------------------------------------
 * Description   : Extension board support function, check presence
 *
 *++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

int rdt1465_extension_presence(int fd, uint *present) {
        return pca9539_get_pin_state(fd, CF_I2C_BUS, U112_I2C_ADDRESS, PCA9539_PORT_0, PORT_0_PIN_EXT_PRESENT_N, present);
}

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * Function name : rdt1465_extension_set_pin_state
 * Prototype     : int
 * Parameters    : int index, rdt1465_ext_pin_state_t state
 * Return        : status of operation
 *----------------------------------------------------------------------------
 * Description   : Extension board set pin state
 *
 *++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

int rdt1465_extension_set_pin_state(int fd, uint ext_pin_index, rdt1465_ext_pin_state_t state)
{
  uint gpio_index = 0;
  uint port = 0;
  int retval = 0;

  if (ext_pin_index > 11) return 1;
  port = (ext_pin_index > 6) ? 0 : 1;
  gpio_index = ext_pin_to_gpio_index[ext_pin_index];

  if (state == RDT1465_EXT_PIN_Z)
  {
    retval = pca9539_set_pin_direction(fd, CF_I2C_BUS, U112_I2C_ADDRESS, port, gpio_index, PCA9539_INPUT_PIN);
  }
  else
  {
    retval  = pca9539_set_pin_state(fd, CF_I2C_BUS, U112_I2C_ADDRESS, port, gpio_index, state);
    retval |= pca9539_set_pin_direction(fd, CF_I2C_BUS, U112_I2C_ADDRESS, port, gpio_index, PCA9539_OUTPUT_PIN);
  }

  return retval;
}

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * Function name : rdt1465_extension_get_pin_state
 * Prototype     : int
 * Parameters    : int index, int *state, int *direction
 * Return        : status of operation
 *----------------------------------------------------------------------------
 * Description   : Extension board get pin state
 *
 *++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

int rdt1465_extension_get_pin_state(int fd, uint ext_pin_index, uint *state, uint *direction) {
  uint gpio_index = 0;
  uint port = 0;
  int retval = 0;

  if (ext_pin_index > 11) return 1;
  port = (ext_pin_index > 6) ? 0 : 1;
  gpio_index = ext_pin_to_gpio_index[ext_pin_index];

  retval  = pca9539_get_pin_state(fd, CF_I2C_BUS, U112_I2C_ADDRESS, port, gpio_index, state);
  retval |= pca9539_get_pin_direction(fd, CF_I2C_BUS, U112_I2C_ADDRESS, port, gpio_index, direction);
  return retval;
}

