/*=========================< begin file & file header >=======================
 *  References
 *  
 *    filename : rcc1466lib.c
 *    author   : XP, RH
 *    company  : IOxOS
 *    creation : July 15,2019
 *
 *----------------------------------------------------------------------------
 *  Description
 *
 *    That library contains a set of function to handle the RCC_1466 mTCA.4.1 RTM.
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
#include "../include/ponmboxlib.h"
#include "../include/tsculib.h"
#include "../include/rcc1466lib.h"


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
| U120   | PCA9539BS  | I/O      | 0x76    | direct                           |
|--------+------------+----------+---------+----------------------------------|
| U121   | PCA9539BS  | I/O      | 0x77    | direct                           |
 -----------------------------------------------------------------------------

U111 I2C switch channels assignation
--------------------------------------------------------------------------------

  * channel 0: PCB SFP5
  * channel 1: no device
  * channel 2: no device
  * channel 3: PCB SFP1
  * channel 4: PCB SFP2
  * channel 5: PCB SFP3
  * channel 6: PCB SFP4
  * channel 7: mezzanine board

U107 I/O expander I/O assignation
 -----------------------------------------------------------------------------
| port | pin | direction | usage                                              |
|------+-----+-----------+----------+-----------------------------------------|
| [0]  | [0] | output    | FP_LED2B_K                                         |
|      | [1] | output    | ADF_SD1_EXTCLK_FSEL0                               |
|      | [2] | output    | ADF_SD1_EXTCLK_FSEL1                               |
|      | [3] | input     | EXT presence: 0 = present, 1 = absent              |
|      | [4] | n/a       | no connect                                         |
|      | [5] | n/a       | no connect                                         |
|      | [6] | n/a       | no connect                                         |
|      | [7] | n/a       | no connect                                         |
|------+-----+-----------+----------+-----------------------------------------|
| [1]  | [0] | output    | SFP5 TX disable: 0 = enable, 1 = disable           |
|      | [1] | output    | SFP5 rate select 0                                 |
|      | [2] | output    | SFP5 rate select 1                                 |
|      | [3] | input     | SFP5 presence: 0 = present, 1 = absent             |
|      | [4] | input     | SFP5 TX fault: 0 = ok, 1 = fault                   |
|      | [5] | input     | SFP5 LOS (loss of signal): 0 = ok, 1 = fault       |
|      | [6] | n/a       | no connect                                         |
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
 * Function name : rcc1466_presence
 * Prototype     : int
 * Parameters    : int *present
 * Return        : status of operation
 *----------------------------------------------------------------------------
 * Description   : RCC1466 board support function, check presence
 *
 *++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

#define RCC_1466_ZONE3_INTERFACE_DESIGNATOR    0x00011466

int rcc1466_presence(int fd) {
	int retval = 0;

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
	    info->rtm_zone3_interface_designator != RCC_1466_ZONE3_INTERFACE_DESIGNATOR)
	{
		printf("RTM present but not a supported RCC_1466:\n");
		printf("  manufacturer id: 0x%06x\n", info->rtm_manufacturer_id);
		printf("  zone3 interface designator:0x%08x\n", info->rtm_zone3_interface_designator);
		free_mbox_info(info);
		return (-1);
	}

	free_mbox_info(info);
	return retval;
}

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * Function name : rcc1466_init
 * Prototype     : int
 * Parameters    : void
 * Return        : status of operation
 *----------------------------------------------------------------------------
 * Description   : Initialization function
 *
 *++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

int rcc1466_init(int fd){
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

	//check on-board devices presence
	//set I/O expanders pins direction and default state
	// Blink MMC LED
	//set_mtca4_rtm_led_state(......);
	return retval;
}

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * Function name : rcc1466_extension_presence
 * Prototype     : int
 * Parameters    : int *present
 * Return        : status of operation
 *----------------------------------------------------------------------------
 * Description   : Extension board support function, check presence
 *
 *++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

int rcc1466_extension_presence(int fd, int *present) {
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
 * Function name : rcc1466_led_turn_on
 * Prototype     : int
 * Parameters    : rcc1466_led_t led_id
 * Return        : status of operation
 *----------------------------------------------------------------------------
 * Description   : On-board LEDs support functions ON
 *
 *++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

int rcc1466_led_turn_on(int fd, rcc1466_led_t led_id) {
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
		// PORT#1 RCC1466_LED123_GREEN[0] & RCC1466_LED123_RED[1]
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
		// PORT#0 RCC1466_LED124_GREEN[2] & RCC1466_LED124_RED[3]
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
		// PORT#1 RCC1466_LED125_GREEN[4] & RCC1466_LED125_RED[5]
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
		// PORT#0 SP1461_LED126_GREEN[6] & RCC1466_LED126_RED[7]
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
 * Function name : rcc1466_led_turn_off
 * Prototype     : int
 * Parameters    : rcc1466_led_t led_id
 * Return        : status of operation
 *----------------------------------------------------------------------------
 * Description   : On-board LEDs support functions OFF
 *
 *++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

int rcc1466_led_turn_off(int fd, rcc1466_led_t led_id) {
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
		// PORT#1 RCC1466_LED123_GREEN[0] & RCC1466_LED123_RED[1]
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
		// PORT#0 RCC1466_LED124_GREEN[2] & RCC1466_LED124_RED[3]
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
		// PORT#1 RCC1466_LED125_GREEN[4] & RCC1466_LED125_RED[5]
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
		// PORT#0 SP1461_LED126_GREEN[6] & RCC1466_LED126_RED[7]
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
 * Function name : rcc1466_sfp_status
 * Prototype     : int
 * Parameters    : rcc1466_sfp_id_t id, rcc1466_sfp_status_t *status
 * Return        : status of operation
 *----------------------------------------------------------------------------
 * Description   : SFP slots support functions status
 *
 *++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

int rcc1466_sfp_status(int fd, rcc1466_sfp_id_t id, uint8_t *status){
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

	default :
		printf("Bad ID ! \n");
		return(-1);
	}
return retval;
}

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * Function name : rcc1466_sfp_control
 * Prototype     : int
 * Parameters    : rcc1466_sfp_id_t id, rcc1466_sfp_status_t control
 * Return        : status of operation
 *----------------------------------------------------------------------------
 * Description   : SFP slots support functions control
 *
 *++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

int rcc1466_sfp_control(int fd, rcc1466_sfp_id_t id, int sfp_enable, int sfp_rate){
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

	default :
		printf("Bad index ! \n");
		printf("Available id is 0 to 6 \n");
		return(-1);
	}
	return retval;
}
