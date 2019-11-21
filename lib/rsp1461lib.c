/*=========================< begin file & file header >=======================
 *  References
 *  
 *    filename : rsp1461lib.c
 *    author   : XP
 *    company  : IOxOS
 *    creation : october 18,2017
 *
 *----------------------------------------------------------------------------
 *  Description
 *
 *    That library contains a set of function to handle the RSP_1461 mTCA.4.1 RTM.
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
#include "../include/rsp1461lib.h"

/*
User I2C devices
 -----------------------------------------------------------------------------
| PCB    | part       | device   | 7-bit   |                                  |
| refdes | name       | function | address | I2C access                       |
|--------+------------+----------+---------+----------------------------------|
| U111   | TCA9548A   | switch   | 0x70    | direct                           |
|--------+------------+----------+---------+----------------------------------|
| U107   | PCA9539BS  | I/O      | 0x74    | direct                           |
|--------+------------+----------+---------+----------------------------------|
| U108   | PCA9539BS  | I/O      | 0x75    | direct                           |
|--------+------------+----------+---------+----------------------------------|
| U120   | PCA9539BS  | I/O      | 0x76    | direct                           |
|--------+------------+----------+---------+----------------------------------|
| U121   | PCA9539BS  | I/O      | 0x77    | direct                           |
|--------+------------+----------+---------+----------------------------------|
| U100   | DS110DF111 | CDR      | 0x18    | switch channel 1 (PCB SFP7)      |
|--------+------------+----------+---------+----------------------------------|
| U102   | DS110DF111 | CDR      | 0x18    | switch channel 2 (PCB SFP6)      |
 -----------------------------------------------------------------------------

U111 I2C switch channels assignation
--------------------------------------------------------------------------------

  * channel 0: PCB SFP5
  * channel 1: PCB SFP7
  * channel 2: PCB SFP6
  * channel 3: PCB SFP1
  * channel 4: PCB SFP2
  * channel 5: PCB SFP3
  * channel 6: PCB SFP4
  * channel 7: extension board (EXT)

U107 I/O expander I/O assignation
 -----------------------------------------------------------------------------
| port | pin | direction | usage                                              |
|------+-----+-----------+----------+-----------------------------------------|
| [0]  | [0] | n/a       | no connect                                         |
|      | [1] | n/a       | no connect                                         |
|      | [2] | n/a       | no connect                                         |
|      | [3] | input     | EXT presence: 0 = present, 1 = absent              |
|      | [4] | n/a       | no connect                                         |
|      | [5] | n/a       | no connect                                         |
|      | [6] | in/out/Z  | EXT I/O 5 (application specific)                   |
|      | [7] | in/out/Z  | EXT I/O 6 (application specific)                   |
|------+-----+-----------+----------+-----------------------------------------|
| [1]  | [0] | output    | SFP5 TX disable: 0 = enable, 1 = disable           |
|      | [1] | output    | SFP5 rate select 0                                 |
|      | [2] | output    | SFP5 rate select 1                                 |
|      | [3] | input     | SFP5 presence: 0 = present, 1 = absent             |
|      | [4] | input     | SFP5 TX fault: 0 = ok, 1 = fault                   |
|      | [5] | input     | SFP5 LOS (loss of signal): 0 = ok, 1 = fault       |
|      | [6] | in/out/Z  | EXT I/O 3 (application specific)                   |
|      | [7] | in/out/Z  | EXT I/O 4 (application specific)                   |
 -----------------------------------------------------------------------------

U108 I/O expander I/O assignation
 -----------------------------------------------------------------------------
| port | pin | direction | usage                                              |
|------+-----+-----------+----------+-----------------------------------------|
| [0]  | [0] | output    | SFP7 TX disable: 0 = enable, 1 = disable           |
|      | [1] | output    | SFP7 rate select 0                                 |
|      | [2] | output    | SFP7 rate select 1                                 |
|      | [3] | input     | SFP7 presence: 0 = present, 1 = absent             |
|      | [4] | input     | SFP7 TX fault: 0 = ok, 1 = fault                   |
|      | [5] | input     | SFP7 LOS (loss of signal): 0 = ok, 1 = fault       |
|      | [6] | in/out/Z  | EXT I/O 0 (application specific)                   |
|      | [7] | in/out/Z  | EXT I/O 1 (application specific)                   |
|------+-----+-----------+----------+-----------------------------------------|
| [1]  | [0] | output    | SFP6 TX disable: 0 = enable, 1 = disable           |
|      | [1] | output    | SFP6 rate select 0                                 |
|      | [2] | output    | SFP6 rate select 1                                 |
|      | [3] | input     | SFP6 presence: 0 = present, 1 = absent             |
|      | [4] | input     | SFP6 TX fault: 0 = ok, 1 = fault                   |
|      | [5] | input     | SFP6 LOS (loss of signal): 0 = ok, 1 = fault       |
|      | [6] | in/out/Z  | EXT I/O 2 (application specific)                   |
|      | [7] | n/a       | no connect                                         |
 -----------------------------------------------------------------------------

U120 I/O expander I/O assignation
 -----------------------------------------------------------------------------
| port | pin | direction | usage                                              |
|------+-----+-----------+----------+-----------------------------------------|
| [0]  | [0] | output    | SFP1 TX disable: 0 = enable, 1 = disable           |
|      | [1] | output    | SFP1 rate select 0                                 |
|      | [2] | output    | SFP1 rate select 1                                 |
|      | [3] | input     | SFP1 presence: 0 = present, 1 = absent             |
|      | [4] | input     | SFP1 TX fault: 0 = ok, 1 = fault                   |
|      | [5] | input     | SFP1 LOS (loss of signal): 0 = ok, 1 = fault       |
|      | [6] | output    | LED126 green: 0 = off, 1 = on                      |
|      | [7] | output    | LED126 red: 0 = off, 1 = on                        |
|------+-----+-----------+----------+-----------------------------------------|
| [1]  | [0] | output    | SFP2 TX disable: 0 = enable, 1 = disable           |
|      | [1] | output    | SFP2 rate select 0                                 |
|      | [2] | output    | SFP2 rate select 1                                 |
|      | [3] | input     | SFP2 presence: 0 = present, 1 = absent             |
|      | [4] | input     | SFP2 TX fault: 0 = ok, 1 = fault                   |
|      | [5] | input     | SFP2 LOS (loss of signal): 0 = ok, 1 = fault       |
|      | [6] | output    | LED125 green: 0 = off, 1 = on                      |
|      | [7] | output    | LED125 red: 0 = off, 1 = on                        |
 -----------------------------------------------------------------------------

U121 I/O expander I/O assignation
 -----------------------------------------------------------------------------
| port | pin | direction | usage                                              |
|------+-----+-----------+----------+-----------------------------------------|
| [0]  | [0] | output    | SFP3 TX disable: 0 = enable, 1 = disable           |
|      | [1] | output    | SFP3 rate select 0                                 |
|      | [2] | output    | SFP3 rate select 1                                 |
|      | [3] | input     | SFP3 presence: 0 = present, 1 = absent             |
|      | [4] | input     | SFP3 TX fault: 0 = ok, 1 = fault                   |
|      | [5] | input     | SFP3 LOS (loss of signal): 0 = ok, 1 = fault       |
|      | [6] | output    | LED124 green: 0 = off, 1 = on                      |
|      | [7] | output    | LED124 red: 0 = off, 1 = on                        |
|------+-----+-----------+----------+-----------------------------------------|
| [1]  | [0] | output    | SFP4 TX disable: 0 = enable, 1 = disable           |
|      | [1] | output    | SFP4 rate select 0                                 |
|      | [2] | output    | SFP4 rate select 1                                 |
|      | [3] | input     | SFP4 presence: 0 = present, 1 = absent             |
|      | [4] | input     | SFP4 TX fault: 0 = ok, 1 = fault                   |
|      | [5] | input     | SFP4 LOS (loss of signal): 0 = ok, 1 = fault       |
|      | [6] | output    | LED123 green: 0 = off, 1 = on                      |
|      | [7] | output    | LED123 red: 0 = off, 1 = on                        |
 -----------------------------------------------------------------------------
*/

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * Function name : rsp1461_presence
 * Prototype     : int
 * Parameters    : int *present
 * Return        : status of operation
 *----------------------------------------------------------------------------
 * Description   : RSP1461 board support function, check presence
 *
 *++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

#define RSP_1461_ZONE3_INTERFACE_DESIGNATOR    0x00011461

int rsp1461_presence(int fd) {
	int retval = 0;
	/*int offset = 0;*/
	/*unsigned char magic_byte;*/

	// Get mbox info for RSP presence
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
	    info->rtm_zone3_interface_designator != RSP_1461_ZONE3_INTERFACE_DESIGNATOR)
	{
		printf("RTM present but not a supported RSP_1461:\n");
		printf("  manufacturer id: 0x%06x\n", info->rtm_manufacturer_id);
		printf("  zone3 interface designator:0x%08x\n", info->rtm_zone3_interface_designator);
		free_mbox_info(info);
		return (-1);
	}

	free_mbox_info(info);
	return retval;
}

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * Function name : rsp1461_init
 * Prototype     : int
 * Parameters    : void
 * Return        : status of operation
 *----------------------------------------------------------------------------
 * Description   : Initialization function
 *
 *++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

int rsp1461_init(int fd){
	int retval = 0;
	int addr   = 0;
	int bus    = 4;
	int reg    = 0;
	int rs     = 1;
	int ds     = 1;
	int device = 0;
	int data   = 0;

// ----- Init U120 -----
	addr = 0x76;
	data = 0x38; // 0011'1000 out->0, in->1
	reg  = 6; // Port[0]
	device = (bus & 7) << 29; device |= addr & 0x7f; device |= ((rs - 1) & 3) << 16; device |= ((ds - 1) & 3) << 18;
	retval = tsc_i2c_write(fd, device, reg, data);
	reg  = 7; // Port[1]
	data = 0x38; // 0011'1000 out->0, in->1
	retval = tsc_i2c_write(fd, device, reg, data);

// ----- Init U121 -----
	addr = 0x77;
	reg  = 6; // Port[0]
	data = 0x38; // 0011'1000 out->0, in->1
	device = (bus & 7) << 29; device |= addr & 0x7f; device |= ((rs - 1) & 3) << 16; device |= ((ds - 1) & 3) << 18;
	retval = tsc_i2c_write(fd, device, reg, data);
	reg  = 7; // Port[1]
	data = 0x38; // 0011'1000 out->0, in->1
	retval = tsc_i2c_write(fd, device, reg, data);

// ----- Init U107 -----
	addr = 0x74;
	reg  = 6; // Port[0]
	data = 0xff; // 1111'1111 out->0, in->1
	device = (bus & 7) << 29; device |= addr & 0x7f; device |= ((rs - 1) & 3) << 16; device |= ((ds - 1) & 3) << 18;
	retval = tsc_i2c_write(fd, device, reg, data);
	reg  = 7; // Port[1]
	data = 0xf8; // 1111'1000 out->0, in->1
	retval = tsc_i2c_write(fd, device, reg, data);

// ----- Init U108 -----
	addr = 0x75;
	reg  = 6; // Port[0]
	data = 0xf8; // 1111'1000 out->0, in->1
	device = (bus & 7) << 29; device |= addr & 0x7f; device |= ((rs - 1) & 3) << 16; device |= ((ds - 1) & 3) << 18;
	retval = tsc_i2c_write(fd, device, reg, data);
	reg  = 7; // Port[1]
	data = 0xf8; // 1111'1000 out->0, in->1
	retval = tsc_i2c_write(fd, device, reg, data);

	//check on-board devices presence
	//set I/O expanders pins direction and default state
	// Blink MMC LED
	//set_mtca4_rtm_led_state(......);
	return retval;
}

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * Function name : rsp1461_extension_presence
 * Prototype     : int
 * Parameters    : int *present
 * Return        : status of operation
 *----------------------------------------------------------------------------
 * Description   : Extension board support function, check presence
 *
 *++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

int rsp1461_extension_presence(int fd, int *present) {
	int retval = 0;
	int addr   = 0x74; //U107
	int bus    = 4;
	int reg    = 0;
	int rs     = 1;
	int ds     = 1;
	int device = 0;
	unsigned int data   = 0;

    // Check U107 EXT presence bit at port 0
	device = (bus&7)<<29; device |= addr & 0x7f;device |= ((rs-1)&3)<<16; device |= ((ds-1)&3)<<18;
	retval = tsc_i2c_read(fd, device, reg, &data);
	*present = (data & 0x8) >> 3;

	return retval;
}

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * Function name : rsp1461_extension_set_pin_state
 * Prototype     : int
 * Parameters    : int index, rsp1461_ext_pin_state_t state
 * Return        : status of operation
 *----------------------------------------------------------------------------
 * Description   : Extension board set pin state
 *
 *++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

int rsp1461_extension_set_pin_state(int fd, int index, rsp1461_ext_pin_state_t state) {
	int retval = 0;
	int addr     = 0;
	int bus      = 4;
	int reg      = 0;
	int rs       = 1;
	int ds       = 1;
	int device   = 0;
	unsigned int data     = 0;
	/*int data_org = 0;*/

	switch(index){
		// U108 PORT#0
	    case 0:
	    case 1:
	    	addr = 0x75;
	    	device = (bus & 7) << 29; device |= addr & 0x7f; device |= ((rs - 1) & 3) << 16; device |= ((ds - 1) & 3) << 18;
	    	if(index == 0){
	    		if((state == RSP1461_EXT_PIN_LOW) || (state == RSP1461_EXT_PIN_HIGH)) {
	    			// Write to the output the value
	    			reg = 2;
	    			// Read pin value
	    			retval = tsc_i2c_read(fd, device, reg, &data);
	    			data ^= (-state ^ data) & (1 << 6); // Bit n will be set if x is 1, and cleared if x is 0. // number ^= (-x ^ number) & (1 << n);
	    			retval = tsc_i2c_write(fd, device, reg, data);
	    			// Configure pin as an output
	    	    	reg = 6;
	    	    	// Read pin direction
	    	    	retval = tsc_i2c_read(fd, device, reg, &data);
	    	    	// Set specific pin as an output direction (clear bit)
	    	    	data &= ~(1 << 6);
	    	    	retval = tsc_i2c_write(fd, device, reg, data);
	    		}
	    		else if (state == RSP1461_EXT_PIN_Z){
	    			// Configure pin as an input
	    	    	reg = 6;
	    	    	// Read pin direction
	    	    	retval = tsc_i2c_read(fd, device, reg, &data);
	    	    	// Set specific pin as an input direction (set bit)
	    	    	data |= 1 << 6;
	    	    	retval = tsc_i2c_write(fd, device, reg, data);
	    		}
	    	}
	    	else if (index == 1){
	    		if((state == RSP1461_EXT_PIN_LOW) || (state == RSP1461_EXT_PIN_HIGH)) {
	    			// Write to the output the value
	    			reg = 2;
	    			// Read pin value
	    			retval = tsc_i2c_read(fd, device, reg, &data);
	    			data ^= (-state ^ data) & (1 << 7); // Bit n will be set if x is 1, and cleared if x is 0. // number ^= (-x ^ number) & (1 << n);
	    			retval = tsc_i2c_write(fd, device, reg, data);
	    			// Configure pin as an output
	    	    	reg = 6;
	    	    	// Read pin direction
	    	    	retval = tsc_i2c_read(fd, device, reg, &data);
	    	    	// Set specific pin as an output direction (clear bit)
	    	    	data &= ~(1 << 7);
	    	    	retval = tsc_i2c_write(fd, device, reg, data);
	    		}
	    		else if (state == RSP1461_EXT_PIN_Z){
	    			// Configure pin as an input
	    	    	reg = 6;
	    	    	// Read pin direction
	    	    	retval = tsc_i2c_read(fd, device, reg, &data);
	    	    	// Set specific pin as an input direction (set bit)
	    	    	data |= 1 << 7;
	    	    	retval = tsc_i2c_write(fd, device, reg, data);
	    		}
	    	}
	    	break;
	    // U108 PORT#1
	    case 2:
	    	addr = 0x75;
	    	device = (bus & 7) << 29; device |= addr & 0x7f; device |= ((rs - 1) & 3) << 16; device |= ((ds - 1) & 3) << 18;
	    	if((state == RSP1461_EXT_PIN_LOW) || (state == RSP1461_EXT_PIN_HIGH)) {
	    		// Write to the output the value
	    		reg = 3;
	    		// Read pin value
	    		retval = tsc_i2c_read(fd, device, reg, &data);
	    		data ^= (-state ^ data) & (1 << 6); // Bit n will be set if x is 1, and cleared if x is 0. // number ^= (-x ^ number) & (1 << n);
	    		retval = tsc_i2c_write(fd, device, reg, data);
	    		// Configure pin as an output
	    	   	reg = 7;
	    	   	// Read pin direction
	    	   	retval = tsc_i2c_read(fd, device, reg, &data);
	    	   	// Set specific pin as an output direction (clear bit)
	    	   	data &= ~(1 << 6);
	    	   	retval = tsc_i2c_write(fd, device, reg, data);
	    	}
	    	else if (state == RSP1461_EXT_PIN_Z){
	    		// Configure pin as an input
	    	   	reg = 7;
	    	   	// Read pin direction
	    	   	retval = tsc_i2c_read(fd, device, reg, &data);
	    	   	// Set specific pin as an input direction (set bit)
	    	   	data |= 1 << 6;
	    	   	retval = tsc_i2c_write(fd, device, reg, data);
	    	}
	        break;
	    // U107 PORT#1
	    case 3:
	    case 4:
	    	addr = 0x74;
	    	device = (bus & 7) << 29; device |= addr & 0x7f; device |= ((rs - 1) & 3) << 16; device |= ((ds - 1) & 3) << 18;
	    	if(index == 3){
	    		if((state == RSP1461_EXT_PIN_LOW) || (state == RSP1461_EXT_PIN_HIGH)) {
	    			// Write to the output the value
	    			reg = 3;
	    			// Read pin value
	    			retval = tsc_i2c_read(fd, device, reg, &data);
	    			data ^= (-state ^ data) & (1 << 6); // Bit n will be set if x is 1, and cleared if x is 0. // number ^= (-x ^ number) & (1 << n);
	    			retval = tsc_i2c_write(fd, device, reg, data);
	    			// Configure pin as an output
	    			reg = 7;
	    			// Read pin direction
	    			retval = tsc_i2c_read(fd, device, reg, &data);
	    			// Set specific pin as an output direction (clear bit)
	    			data &= ~(1 << 6);
	    			retval = tsc_i2c_write(fd, device, reg, data);
	    		}
	    		else if (state == RSP1461_EXT_PIN_Z){
	    			// Configure pin as an input
	    			reg = 7;
	    			// Read pin direction
	    			retval = tsc_i2c_read(fd, device, reg, &data);
	    			// Set specific pin as an input direction (set bit)
	    			data |= 1 << 6;
	    			retval = tsc_i2c_write(fd, device, reg, data);
	    		}
	    	}
	    	else if (index == 4){
	    		if((state == RSP1461_EXT_PIN_LOW) || (state == RSP1461_EXT_PIN_HIGH)) {
	    			// Write to the output the value
	    			reg = 3;
	    			// Read pin value
	    			retval = tsc_i2c_read(fd, device, reg, &data);
	    			data ^= (-state ^ data) & (1 << 7); // Bit n will be set if x is 1, and cleared if x is 0. // number ^= (-x ^ number) & (1 << n);
	    			retval = tsc_i2c_write(fd, device, reg, data);
	    			// Configure pin as an output
	    			reg = 7;
	    			// Read pin direction
	    			retval = tsc_i2c_read(fd, device, reg, &data);
	    			// Set specific pin as an output direction (clear bit)
	    			data &= ~(1 << 7);
	    			retval = tsc_i2c_write(fd, device, reg, data);
	    		}
	    		else if (state == RSP1461_EXT_PIN_Z){
	    			// Configure pin as an input
	    			reg = 7;
	    			// Read pin direction
	    			retval = tsc_i2c_read(fd, device, reg, &data);
	    			// Set specific pin as an input direction (set bit)
	    			data |= 1 << 7;
	    			retval = tsc_i2c_write(fd, device, reg, data);
	    		}
	    	}
	    	break;
	    // U107 PORT#0
	    case 5:
	    case 6:
	    	addr = 0x74;
	    	device = (bus & 7) << 29; device |= addr & 0x7f; device |= ((rs - 1) & 3) << 16; device |= ((ds - 1) & 3) << 18;
	    	if(index == 5){
	    		if((state == RSP1461_EXT_PIN_LOW) || (state == RSP1461_EXT_PIN_HIGH)) {
	    			// Write to the output the value
	    			reg = 2;
	    			data ^= (-state ^ data) & (1 << 6); // Bit n will be set if x is 1, and cleared if x is 0. // number ^= (-x ^ number) & (1 << n);
	    			retval = tsc_i2c_write(fd, device, reg, data);
	    			// Configure pin as an output
	    			reg = 6;
	    			// Read pin direction
	    			retval = tsc_i2c_read(fd, device, reg, &data);
	    			// Set specific pin as an output direction (clear bit)
	    			data &= ~(1 << 6);
	    			retval = tsc_i2c_write(fd, device, reg, data);
	    		}
	    		else if (state == RSP1461_EXT_PIN_Z){
	    			// Configure pin as an input
	    			reg = 6;
	    			// Read pin direction
	    			retval = tsc_i2c_read(fd, device, reg, &data);
	    			// Set specific pin as an input direction (set bit)
	    			data |= 1 << 6;
	    			retval = tsc_i2c_write(fd, device, reg, data);
	    		}
	    	}
	    	else if (index == 6){
	    		if((state == RSP1461_EXT_PIN_LOW) || (state == RSP1461_EXT_PIN_HIGH)) {
	    			// Write to the output the value
	    			reg = 2;
	    			data ^= (-state ^ data) & (1 << 7); // Bit n will be set if x is 1, and cleared if x is 0. // number ^= (-x ^ number) & (1 << n);
	    			retval = tsc_i2c_write(fd, device, reg, data);
	    			// Configure pin as an output
	    			reg = 6;
	    			// Read pin direction
	    			retval = tsc_i2c_read(fd, device, reg, &data);
	    			// Set specific pin as an output direction (clear bit)
	    			data &= ~(1 << 7);
	    			retval = tsc_i2c_write(fd, device, reg, data);
	    		}
	    		else if (state == RSP1461_EXT_PIN_Z){
	    			// Configure pin as an input
	    			reg = 6;
	    			// Read pin direction
	    			retval = tsc_i2c_read(fd, device, reg, &data);
	    			// Set specific pin as an input direction (set bit)
	    			data |= 1 << 7;
	    			retval = tsc_i2c_write(fd, device, reg, data);
	    		}
	    	}
	        break;
	    default :
			printf("Bad pin index ! \n");
			printf("Available id is 0 to 6 \n");
			return(-1);
		}

	return retval;
}

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * Function name : rsp1461_extension_get_pin_state
 * Prototype     : int
 * Parameters    : int index, int *state, int *direction
 * Return        : status of operation
 *----------------------------------------------------------------------------
 * Description   : Extension board get pin state
 *
 *++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

int rsp1461_extension_get_pin_state(int fd, int index, int *state, int *direction) {
	int retval   = 0;
	int addr     = 0;
	int bus      = 4;
	int reg      = 0;
	int rs       = 1;
	int ds       = 1;
	int device   = 0;
	unsigned int data     = 0;
	unsigned int data_org = 0;

switch(index){
	// U108 PORT#0
    case 0:
    case 1:
    	addr = 0x75;
    	reg = 6;
    	device = (bus & 7) << 29; device |= addr & 0x7f; device |= ((rs - 1) & 3) << 16; device |= ((ds - 1) & 3) << 18;
    	// Read pin direction
    	retval = tsc_i2c_read(fd, device, reg, &data_org);
    	// Set specific pin as an input direction
    	data = data_org;
    	if(index == 0){
        	*direction = (data_org & (1 << 6)) >> 6;
    		data |= 1 << 6;
    	}
    	else if (index == 1){
    		*direction = (data_org & (1 << 7)) >> 7;
    		data |= 1 << 7;
    	}
    	retval = tsc_i2c_write(fd, device, reg, data);
    	// Get pin value
    	reg = 0;
    	retval = tsc_i2c_read(fd, device, reg, &data);
    	if(index == 0){
    		*state = (data & 0x40) >> 6;
    	}
    	else if (index == 1){
    		*state = (data & 0x80) >> 7;
    	}
    	// Re-set pin direction as original
    	reg = 6;
    	retval = tsc_i2c_write(fd, device, reg, data_org);
        break;
    // U108 PORT#1
    case 2:
    	addr = 0x75;
    	reg = 7;
    	device = (bus & 7) << 29; device |= addr & 0x7f; device |= ((rs - 1) & 3) << 16; device |= ((ds - 1) & 3) << 18;
    	// Read pin direction
    	retval = tsc_i2c_read(fd, device, reg, &data_org);
    	// Set specific pin as an input direction
    	data = data_org;
    	*direction = (data_org & (1 << 6)) >> 6;
    	data |= 1 << 6;
    	retval = tsc_i2c_write(fd, device, reg, data);
    	// Get pin value
    	reg = 1;
    	retval = tsc_i2c_read(fd, device, reg, &data);
    	*state = (data & 0x40) >> 6;
    	// Re-set pin direction as original
    	reg = 7;
    	retval = tsc_i2c_write(fd, device, reg, data_org);
        break;
    // U107 PORT#1
    case 3:
    case 4:
    	addr = 0x74;
    	reg = 7;
    	device = (bus & 7) << 29; device |= addr & 0x7f; device |= ((rs - 1) & 3) << 16; device |= ((ds - 1) & 3) << 18;
    	// Read pin direction
    	retval = tsc_i2c_read(fd, device, reg, &data_org);
    	// Set specific pin as an input direction
    	data = data_org;
    	if(index == 3){
    		*direction = (data_org & (1 << 6)) >> 6;
    		data |= 1 << 6;
    	}
    	else if (index == 4){
    		*direction = (data_org & (1 << 7)) >> 7;
    		data |= 1 << 7;
    	}
    	retval = tsc_i2c_write(fd, device, reg, data);
    	// Get pin value
    	reg = 1;
    	retval = tsc_i2c_read(fd, device, reg, &data);
    	if(index == 3){
    		*state = (data & 0x40) >> 6;
    	}
    	else if (index == 4){
    		*state = (data & 0x80) >> 7;
    	}
    	// Re-set pin direction as original
    	reg = 7;
    	retval = tsc_i2c_write(fd, device, reg, data_org);
        break;
    // U107 PORT#0
    case 5:
    case 6:
    	addr = 0x74;
    	reg = 6;
    	device = (bus & 7) << 29; device |= addr & 0x7f; device |= ((rs - 1) & 3) << 16; device |= ((ds - 1) & 3) << 18;
    	// Read pin direction
    	retval = tsc_i2c_read(fd, device, reg, &data_org);
    	// Set specific pin as an input direction
    	data = data_org;
    	if(index == 5){
    		*direction = (data_org & (1 << 6)) >> 6;
    		data |= 1 << 6;
    	}
    	else if (index == 6){
    		*direction = (data_org & (1 << 7)) >> 7;
    		data |= 1 << 7;
    	}
    	retval = tsc_i2c_write(fd, device, reg, data);
    	// Get pin value
    	reg = 0;
    	retval = tsc_i2c_read(fd, device, reg, &data);
    	if(index == 5){
    		*state = (data & 0x40) >> 6;
    	}
    	else if (index == 6){
    		*state = (data & 0x80) >> 7;
    	}
    	// Re-set pin direction as original
    	reg = 6;
    	retval = tsc_i2c_write(fd, device, reg, data_org);
        break;
    default :
		printf("Bad pin index ! \n");
		printf("Available id is 0 to 6 \n");
		return(-1);
	}
	return retval;
}

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * Function name : rsp1461_led_turn_on
 * Prototype     : int
 * Parameters    : rsp1461_led_t led_id
 * Return        : status of operation
 *----------------------------------------------------------------------------
 * Description   : On-board LEDs support functions ON
 *
 *++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

int rsp1461_led_turn_on(int fd, rsp1461_led_t led_id) {
	int retval = 0;
	int addr   = 0;
	int bus    = 4;
	int reg    = 0;
	int rs     = 1;
	int ds     = 1;
	int device = 0;
	unsigned int data   = 0;

// ----- U121 -----
	if ((int)led_id < 4){
		addr = 0x77;
		device = (bus & 7) << 29; device |= addr & 0x7f; device |= ((rs - 1) & 3) << 16; device |= ((ds - 1) & 3) << 18;
		// PORT#1 RSP1461_LED123_GREEN[0] & RSP1461_LED123_RED[1]
		if ((int)led_id < 2){
			reg = 1;
			retval = tsc_i2c_read(fd, device, reg, &data);

			if((int)led_id == 0){
				data |= 1 << 6;
			}
			else if ((int)led_id == 1){
				data |= 1 << 7;
			}
			reg = 3;
			retval = tsc_i2c_write(fd, device, reg, data);
		}
		// PORT#0 RSP1461_LED124_GREEN[2] & RSP1461_LED124_RED[3]
		else if ((int)led_id > 1){
			reg = 0;
			retval = tsc_i2c_read(fd, device, reg, &data);

			if((int)led_id == 2){
				data |= 1 << 6;
			}
			else if ((int)led_id == 3){
				data |= 1 << 7;
			}
			reg = 2;
			retval = tsc_i2c_write(fd, device, reg, data);
		}
	}
// ----- U120 -----
	else if ((int)led_id > 3){
		addr = 0x76;
		device = (bus & 7) << 29; device |= addr & 0x7f; device |= ((rs - 1) & 3) << 16; device |= ((ds - 1) & 3) << 18;
		// PORT#1 RSP1461_LED125_GREEN[4] & RSP1461_LED125_RED[5]
		if ((int)led_id < 6){
			reg = 1;
			retval = tsc_i2c_read(fd, device, reg, &data);

			if((int)led_id == 4){
				data |= 1 << 6;
			}
			else if ((int)led_id == 5){
				data |= 1 << 7;
			}
			reg = 3;
			retval = tsc_i2c_write(fd, device, reg, data);
		}
		// PORT#0 SP1461_LED126_GREEN[6] & RSP1461_LED126_RED[7]
		else if ((int)led_id > 5){
			reg = 0;
			retval = tsc_i2c_read(fd, device, reg, &data);

			if((int)led_id == 6){
				data |= 1 << 6;
			}
			else if ((int)led_id == 7){
				data |= 1 << 7;
			}
			reg = 2;
			retval = tsc_i2c_write(fd, device, reg, data);
		}
	}
	return retval;
}

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * Function name : rsp1461_led_turn_off
 * Prototype     : int
 * Parameters    : rsp1461_led_t led_id
 * Return        : status of operation
 *----------------------------------------------------------------------------
 * Description   : On-board LEDs support functions OFF
 *
 *++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

int rsp1461_led_turn_off(int fd, rsp1461_led_t led_id) {
	int retval = 0;
	int addr   = 0;
	int bus    = 4;
	int reg    = 0;
	int rs     = 1;
	int ds     = 1;
	int device = 0;
	unsigned int data   = 0;

// ----- U121 -----
	if ((int)led_id < 4){
		addr = 0x77;
		device = (bus & 7) << 29; device |= addr & 0x7f; device |= ((rs - 1) & 3) << 16; device |= ((ds - 1) & 3) << 18;
		// PORT#1 RSP1461_LED123_GREEN[0] & RSP1461_LED123_RED[1]
		if ((int)led_id < 2){
			reg = 1;
			retval = tsc_i2c_read(fd, device, reg, &data);
			if((int)led_id == 0){
				data &= ~(1 << 6);
			}
			else if ((int)led_id == 1){
				data &= ~(1 << 7);
			}
			reg = 3;
			retval = tsc_i2c_write(fd, device, reg, data);
		}
		// PORT#0 RSP1461_LED124_GREEN[2] & RSP1461_LED124_RED[3]
		else if ((int)led_id > 1){
			reg = 0;
			retval = tsc_i2c_read(fd, device, reg, &data);
			if((int)led_id == 2){
				data &= ~(1 << 6);
			}
			else if ((int)led_id == 3){
				data &= ~(1 << 7);
			}
			reg = 2;
			retval = tsc_i2c_write(fd, device, reg, data);
		}
	}
// ----- U120 -----
	else if ((int)led_id > 3){
		addr = 0x76;
		device = (bus & 7) << 29; device |= addr & 0x7f; device |= ((rs - 1) & 3) << 16; device |= ((ds - 1) & 3) << 18;
		// PORT#1 RSP1461_LED125_GREEN[4] & RSP1461_LED125_RED[5]
		if ((int)led_id < 6){
			reg = 1;
			retval = tsc_i2c_read(fd, device, reg, &data);
			if((int)led_id == 4){
				data &= ~(1 << 6);
			}
			else if ((int)led_id == 5){
				data &= ~(1 << 7);
			}
			reg = 3;
			retval = tsc_i2c_write(fd, device, reg, data);
		}
		// PORT#0 SP1461_LED126_GREEN[6] & RSP1461_LED126_RED[7]
		else if ((int)led_id > 5){
			reg = 0;
			retval = tsc_i2c_read(fd, device, reg, &data);

			if((int)led_id == 6){
				data &= ~(1 << 6);
			}
			else if ((int)led_id == 7){
				data &= ~(1 << 7);
			}
			reg = 2;
			retval = tsc_i2c_write(fd, device, reg, data);
		}
	}
	return retval;
}

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * Function name : rsp1461_sfp_status
 * Prototype     : int
 * Parameters    : rsp1461_sfp_id_t id, rsp1461_sfp_status_t *status
 * Return        : status of operation
 *----------------------------------------------------------------------------
 * Description   : SFP slots support functions status
 *
 *++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

int rsp1461_sfp_status(int fd, rsp1461_sfp_id_t id, uint8_t *status){
	int retval   = 0;
	int addr     = 0;
	int bus      = 4;
	int reg      = 0;
	int rs       = 1;
	int ds       = 1;
	int device   = 0;
	unsigned int data     = 0;

	switch(id){

	// U120 PORT#0 & PORT#1
	case 0:
	case 1:
    	addr = 0x76;
    	device = (bus & 7) << 29; device |= addr & 0x7f; device |= ((rs - 1) & 3) << 16; device |= ((ds - 1) & 3) << 18;
    	if (id == 0){
    		reg = 0; // PORT#0
    	}
    	else if (id == 1){
    		reg = 1; // PORT#1
    	}
    	retval = tsc_i2c_read(fd, device, reg, &data);
    	*status = (data & 0x3f);
		break;

	// U121 PORT#0 & PORT#1
	case 2:
	case 3:
    	addr = 0x77;
    	device = (bus & 7) << 29; device |= addr & 0x7f; device |= ((rs - 1) & 3) << 16; device |= ((ds - 1) & 3) << 18;
    	if (id == 2){
    		reg = 0; // PORT#0
    	}
    	else if (id == 3){
    		reg = 1; // PORT#1
    	}
    	retval = tsc_i2c_read(fd, device, reg, &data);
    	*status = (data & 0x3f);
		break;

	// U107 PORT#1
	case 4:
    	addr = 0x74;
    	device = (bus & 7) << 29; device |= addr & 0x7f; device |= ((rs - 1) & 3) << 16; device |= ((ds - 1) & 3) << 18;
    	reg = 1; // PORT#1
    	retval = tsc_i2c_read(fd, device, reg, &data);
    	*status = (data & 0x3f);
		break;

	// U108 PORT#1 & PORT#0
	case 5:
	case 6:
    	addr = 0x75;
    	device = (bus & 7) << 29; device |= addr & 0x7f; device |= ((rs - 1) & 3) << 16; device |= ((ds - 1) & 3) << 18;
    	if (id == 5){
    		reg = 1; // PORT#1
    	}
    	else if (id == 6){
    		reg = 0; // PORT#0
    	}
    	retval = tsc_i2c_read(fd, device, reg, &data);
    	*status = (data & 0x3f);
		break;

	default :
		printf("Bad ID ! \n");
		return(-1);
	}
return retval;
}

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * Function name : rsp1461_sfp_control
 * Prototype     : int
 * Parameters    : rsp1461_sfp_id_t id, rsp1461_sfp_status_t control
 * Return        : status of operation
 *----------------------------------------------------------------------------
 * Description   : SFP slots support functions control
 *
 *++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

int rsp1461_sfp_control(int fd, rsp1461_sfp_id_t id, int sfp_enable, int sfp_rate){
	int retval   = 0;
	int addr     = 0;
	int bus      = 4;
	int reg      = 0;
	int rs       = 1;
	int ds       = 1;
	int device   = 0;
	unsigned int data     = 0;

	switch(id){

	// U120 PORT#0 & PORT#1
	case 0:
	case 1:
	   	addr = 0x76;
	   	device = (bus & 7) << 29; device |= addr & 0x7f; device |= ((rs - 1) & 3) << 16; device |= ((ds - 1) & 3) << 18;
	   	if (id == 0){
	   		reg = 0; // PORT#0
	   		retval = tsc_i2c_read(fd, device, reg, &data); // Get current state
	   		// Set enable
	   		data ^= (-sfp_enable ^ data) & (1 << 0); // Bit n will be set if x is 1, and cleared if x is 0. // number ^= (-x ^ number) & (1 << n);
	   		switch(sfp_rate) {
				// Set rate[00]
				case 0 :
					data ^= (-(0) ^ data) & (1 << 1);
					data ^= (-(0) ^ data) & (1 << 2);
					break;
				// Set rate[01]
				case 1  :
					data ^= (-(1) ^ data) & (1 << 1);
					data ^= (-(0) ^ data) & (1 << 2);
					break;
				// Set rate[10]
				case 2  :
					data ^= (-(0) ^ data) & (1 << 1);
					data ^= (-(1) ^ data) & (1 << 2);
					break;
				// Set rate[11]
				case 3  :
					data ^= (-(1) ^ data) & (1 << 1);
					data ^= (-(1) ^ data) & (1 << 2);
					break;
				default :
					NULL;
	   		}
	   		reg = 2;
	   			retval = tsc_i2c_write(fd, device, reg, data);	// Set state
	   	}
	   	else if (id == 1){
	   		reg = 1; // PORT#1
	   		retval = tsc_i2c_read(fd, device, reg, &data); // Get current state
	   		// Set enable
	   		data ^= (-sfp_enable ^ data) & (1 << 0); // Bit n will be set if x is 1, and cleared if x is 0. // number ^= (-x ^ number) & (1 << n);
	   		switch(sfp_rate) {
				// Set rate[00]
				case 0 :
					data ^= (-(0) ^ data) & (1 << 1);
					data ^= (-(0) ^ data) & (1 << 2);
					break;
				// Set rate[01]
				case 1  :
					data ^= (-(1) ^ data) & (1 << 1);
					data ^= (-(0) ^ data) & (1 << 2);
					break;
				// Set rate[10]
				case 2  :
					data ^= (-(0) ^ data) & (1 << 1);
					data ^= (-(1) ^ data) & (1 << 2);
					break;
				// Set rate[11]
				case 3  :
					data ^= (-(1) ^ data) & (1 << 1);
					data ^= (-(1) ^ data) & (1 << 2);
					break;
				default :
					NULL;
	   		}
	   		reg = 3;
	   		retval = tsc_i2c_write(fd, device, reg, data);	// Set state
	   	}
		break;

	// U121 PORT#0 & PORT#1
	case 2:
	case 3:
	   	addr = 0x77;
		device = (bus & 7) << 29; device |= addr & 0x7f; device |= ((rs - 1) & 3) << 16; device |= ((ds - 1) & 3) << 18;
	   	if (id == 2){
	   		reg = 0; // PORT#0
	   		retval = tsc_i2c_read(fd, device, reg, &data); // Get current state
	   		// Set enable
	   		data ^= (-sfp_enable ^ data) & (1 << 0); // Bit n will be set if x is 1, and cleared if x is 0. // number ^= (-x ^ number) & (1 << n);
	   		switch(sfp_rate) {
				// Set rate[00]
				case 0 :
					data ^= (-(0) ^ data) & (1 << 1);
					data ^= (-(0) ^ data) & (1 << 2);
					break;
				// Set rate[01]
				case 1  :
					data ^= (-(1) ^ data) & (1 << 1);
					data ^= (-(0) ^ data) & (1 << 2);
					break;
				// Set rate[10]
				case 2  :
					data ^= (-(0) ^ data) & (1 << 1);
					data ^= (-(1) ^ data) & (1 << 2);
					break;
				// Set rate[11]
				case 3  :
					data ^= (-(1) ^ data) & (1 << 1);
					data ^= (-(1) ^ data) & (1 << 2);
					break;
				default :
					NULL;
	   		}
	   		reg = 2;
	   		retval = tsc_i2c_write(fd, device, reg, data);	// Set state
	   	}
	   	else if (id == 3){
	   		reg = 1; // PORT#1
	   		retval = tsc_i2c_read(fd, device, reg, &data); // Get current state
	   		// Set enable
	   		data ^= (-sfp_enable ^ data) & (1 << 0); // Bit n will be set if x is 1, and cleared if x is 0. // number ^= (-x ^ number) & (1 << n);
	   		switch(sfp_rate) {
				// Set rate[00]
				case 0 :
					data ^= (-(0) ^ data) & (1 << 1);
					data ^= (-(0) ^ data) & (1 << 2);
					break;
				// Set rate[01]
				case 1  :
					data ^= (-(1) ^ data) & (1 << 1);
					data ^= (-(0) ^ data) & (1 << 2);
					break;
				// Set rate[10]
				case 2  :
					data ^= (-(0) ^ data) & (1 << 1);
					data ^= (-(1) ^ data) & (1 << 2);
					break;
				// Set rate[11]
				case 3  :
					data ^= (-(1) ^ data) & (1 << 1);
					data ^= (-(1) ^ data) & (1 << 2);
					break;
				default :
					NULL;
	   		}
	   		reg = 3;
	   		retval = tsc_i2c_write(fd, device, reg, data);	// Set state
	   	}
		break;

	// U107 PORT#1
	case 4:
	   	addr = 0x74;
	   	device = (bus & 7) << 29; device |= addr & 0x7f; device |= ((rs - 1) & 3) << 16; device |= ((ds - 1) & 3) << 18;
	   	reg = 1; // PORT#1
	   	retval = tsc_i2c_read(fd, device, reg, &data); // Get current state
   		// Set enable
   		data ^= (-sfp_enable ^ data) & (1 << 0); // Bit n will be set if x is 1, and cleared if x is 0. // number ^= (-x ^ number) & (1 << n);
   		switch(sfp_rate) {
			// Set rate[00]
			case 0 :
				data ^= (-(0) ^ data) & (1 << 1);
				data ^= (-(0) ^ data) & (1 << 2);
				break;
			// Set rate[01]
			case 1  :
				data ^= (-(1) ^ data) & (1 << 1);
				data ^= (-(0) ^ data) & (1 << 2);
				break;
			// Set rate[10]
			case 2  :
				data ^= (-(0) ^ data) & (1 << 1);
				data ^= (-(1) ^ data) & (1 << 2);
				break;
			// Set rate[11]
			case 3  :
				data ^= (-(1) ^ data) & (1 << 1);
				data ^= (-(1) ^ data) & (1 << 2);
				break;
			default :
				NULL;
   		}
   		reg = 3;
   		retval = tsc_i2c_write(fd, device, reg, data);	// Set state
		break;

	// U108 PORT#1 & PORT#0
	case 5:
	case 6:
	   	addr = 0x75;
	   	device = (bus & 7) << 29; device |= addr & 0x7f; device |= ((rs - 1) & 3) << 16; device |= ((ds - 1) & 3) << 18;
	   	if (id == 5){
	   		reg = 1; // PORT#1
	   		retval = tsc_i2c_read(fd, device, reg, &data); // Get current state
	   		// Set enable
	   		data ^= (-sfp_enable ^ data) & (1 << 0); // Bit n will be set if x is 1, and cleared if x is 0. // number ^= (-x ^ number) & (1 << n);
	   		switch(sfp_rate) {
				// Set rate[00]
				case 0 :
					data ^= (-(0) ^ data) & (1 << 1);
					data ^= (-(0) ^ data) & (1 << 2);
					break;
				// Set rate[01]
				case 1  :
					data ^= (-(1) ^ data) & (1 << 1);
					data ^= (-(0) ^ data) & (1 << 2);
					break;
				// Set rate[10]
				case 2  :
					data ^= (-(0) ^ data) & (1 << 1);
					data ^= (-(1) ^ data) & (1 << 2);
					break;
				// Set rate[11]
				case 3  :
					data ^= (-(1) ^ data) & (1 << 1);
					data ^= (-(1) ^ data) & (1 << 2);
					break;
				default :
					NULL;
	   		}
	   		reg = 3;
	   		retval = tsc_i2c_write(fd, device, reg, data);	// Set state
	   	}
	   	else if (id == 6){
	   		reg = 0; // PORT#0
	   		retval = tsc_i2c_read(fd, device, reg, &data); // Get current state
	   		// Set enable
	   		data ^= (-sfp_enable ^ data) & (1 << 0); // Bit n will be set if x is 1, and cleared if x is 0. // number ^= (-x ^ number) & (1 << n);
	   		switch(sfp_rate) {
				// Set rate[00]
				case 0 :
					data ^= (-(0) ^ data) & (1 << 1);
					data ^= (-(0) ^ data) & (1 << 2);
					break;
				// Set rate[01]
				case 1  :
					data ^= (-(1) ^ data) & (1 << 1);
					data ^= (-(0) ^ data) & (1 << 2);
					break;
				// Set rate[10]
				case 2  :
					data ^= (-(0) ^ data) & (1 << 1);
					data ^= (-(1) ^ data) & (1 << 2);
					break;
				// Set rate[11]
				case 3  :
					data ^= (-(1) ^ data) & (1 << 1);
					data ^= (-(1) ^ data) & (1 << 2);
					break;
				default :
					NULL;
	   		}
	   		reg = 2;
	   		retval = tsc_i2c_write(fd, device, reg, data);	// Set state
	   	}
		break;

	default :
		printf("Bad index ! \n");
		printf("Available id is 0 to 6 \n");
		return(-1);
	}
	return retval;
}
