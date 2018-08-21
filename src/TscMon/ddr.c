/*=========================< begin file & file header >=======================
 *  References
 *
 *    filename : ddr.c
 *    author   : XP
 *    company  : IOxOS
 *    creation : February 2017
 *
 *----------------------------------------------------------------------------
 *  Description
 *
 *    That file contains a set of function called by TscMon to perform
 *    specifics command on DDR memory
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

#define DEBno
#include <debug.h>
#include <sys/types.h>
#include <sys/time.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <ctype.h>
#include <pty.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/mman.h>

#include <cli.h>
#include <tscioctl.h>
#include <tsculib.h>
#include "TscMon.h"

#include "ddr.h"

// SMEM control & status register
unsigned int SMEM_DDR3_CSR[2] = {0x800, 0xc00};

// IDEL adjustment register for both DDR3 memory
unsigned int SMEM_DDR3_IFSTA[2] = {0x808, 0xc08};

// IDEL control register for both DDR3 memory
unsigned int SMEM_DDR3_IDEL[2] = {0x80c, 0xc0c};

extern int tsc_fd;

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * Function name : bin
 * Prototype     : void
 * Parameters    : n, bit_size
 * Return        : void
 *----------------------------------------------------------------------------
 * Description   : trancode binary
 *
 *++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

void bin(unsigned n, unsigned int bit_size){
	unsigned int i;
	unsigned int j = 1;

    for (i = 1 << (bit_size - 1); i > 0; i = i / 2) {
    	(n & i)? printf("1"): printf("0");
    	if (!(j % 4)){
    		printf("'");
    	}
    	j++;
    }
}

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * Function name : tsc_ddr_idel_reset
 * Prototype     : int
 * Parameters    : memory
 * Return        : 0
 *----------------------------------------------------------------------------
 * Description   : reset idelay value
 *
 *++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*
int tsc_ddr_idel_reset(int mem){
	unsigned int data = 0;
	unsigned int init = 0;

	printf("Loading tsc DDR3 #%d default IDELAY ... \n", mem);

	//tsc_csr_read(SMEM_DDR3_IDEL[mem - 1], &data);
	//data = (data | (1 << 31) | (1 << 20) | (0xffff << 0)) & (~(0x7 << 21));
	//tsc_csr_write(SMEM_DDR3_IDEL[mem - 1], &data); // Load default delay for all lines with step at 1
	tsc_csr_read(tsc_fd, SMEM_DDR3_IDEL[mem - 1], &init); 			// Acquire initial value of the register
	data = (init | (1 << 31) | (3 << 16) | (0xffff << 0));	// Set value to reset delay
	tsc_csr_write(tsc_fd, SMEM_DDR3_IDEL[mem - 1], &data); 			// Load default delay for all lines with step at 1

	usleep(1000);

	//tsc_csr_read(SMEM_DDR3_IDEL[mem - 1], &data);
	//data = (data | (1 << 20)) & (~(0xffff << 0)) & (~(0x7 << 21));
	//tsc_csr_write(SMEM_DDR3_IDEL[mem - 1], &data); // Step at 1, unselect all lines
	data = 0;
	tsc_csr_write(tsc_fd, SMEM_DDR3_IDEL[mem - 1], &data); // Full reset the register
	tsc_csr_write(tsc_fd, SMEM_DDR3_IDEL[mem - 1], &init); // Load initial value in the register

	printf("DDR3 #%d done ... \n", mem);

	return 0;
}
*/

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * Function name : tsc_ddr_idel_set
 * Prototype     : int
 * Parameters    : memory, dq, step, plus/minus increment
 * Return        : 0
 *----------------------------------------------------------------------------
 * Description   : set idelay for each dq
 *
 *++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

/*int tsc_ddr_idel_set(int mem, unsigned int dq, unsigned int step, char* pm){
	int data = 0;

	// -- Decrement the delay --
	if (*pm == '-') {
		tsc_csr_read(tsc_fd, SMEM_DDR3_IDEL[mem - 1], &data);
		data = data & (~(0xf << 20)); // Clear field STEP
		data = data | (dq & 0x0000ffff) | ((step & 0x00000000f) << 20) | ((1 & 0x00000001) << 29);
		tsc_csr_write(tsc_fd, SMEM_DDR3_IDEL[mem - 1], &data);
	}
	// -- Increment the delay --
	else if(*pm == '+') {
		tsc_csr_read(tsc_fd, SMEM_DDR3_IDEL[mem - 1], &data);
		data = data & (~(0xf << 20)); // Clear field STEP
		data = data | (dq & 0x0000ffff) | ((step & 0x00000000f) << 20) | ((1 & 0x00000001) << 30);
		tsc_csr_write(tsc_fd, SMEM_DDR3_IDEL[mem - 1], &data);
	}

	tsc_csr_read(tsc_fd, SMEM_DDR3_IDEL[mem - 1], &data);
	data = (data | (1 << 20)) & (~(0xffff << 0)) & (~(0x7 << 21));
	tsc_csr_write(tsc_fd, SMEM_DDR3_IDEL[mem - 1], &data); // unselect all lines, step at 1

	printf("DDR3 #%d IDELAY set done ...\n", mem);

	return 0;
}
*/

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * Function name : tsc_ddr_idel_status
 * Prototype     : int
 * Parameters    : mem
 * Return        : 0
 *----------------------------------------------------------------------------
 * Description   : status of calibration registers
 *
 *++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

/*int tsc_ddr_idel_status(int mem){
	int d0   = 0;
	unsigned int temp = 0;
	unsigned int mask = 0;

	printf("tsc DDR3 #%d IDELAY status\n", mem);
	printf("-------------------------------- \n");
	tsc_csr_read(tsc_fd, SMEM_DDR3_IDEL[mem - 1], &d0);
	// -- DQ[15:0] (data bus) --
	mask = 0x0000ffff;
	temp = d0 & mask;
	printf("DQ[15:0]       : ");
	bin(temp,16);
	printf(" [0x%04x]", temp);
	printf("\n");
	// STEP
	tsc_csr_read(tsc_fd, SMEM_DDR3_IDEL[mem - 1], &d0);
	mask = 0x00f00000;
	temp = (d0 & mask) >> 20;
	printf("INC / DEC STEP : %x \n", temp);

	printf("-------------------------------- \n");

    return 0;
}
*/

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * Function name : tsc_ddr_idel_calib
 * Prototype     : int
 * Parameters    : memory (shm1 or shm2)
 * Return        : success/error
 *----------------------------------------------------------------------------
 * Description   : calibration of the ddr memory
 *
 *++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

int tsc_ddr_idel_calib(int mem){

    ////////////////////////////////////////////////////////////////////////////////////
    // ADJUST DEFAULT VALUE FOR CURRENT_DLY ACCORDING TO HARDWARE IMPLEMENTATION      //
    unsigned int    CURRENT_DLY     = 256;	                                          //
    // ADJUST DEFAULT STEP VALUE FOR INC / DEC VALUE                                  //
    unsigned int    CURRENT_STEP    = 4;			                                  //
    // ADJUSTE DEFAULT INIT DELAY													  //
    unsigned int    DEFAULT_DELAY   = 0x40;                                           //
    // MAX DELAY VALUE                                                                //
    unsigned int    MAX             = 0x1ff;                                          //
    ////////////////////////////////////////////////////////////////////////////////////

	struct tsc_ioctl_map_win map_win;
	int             DQ_NOK[16];
	int             DQ_OK[16];
	int             ppc             = 0;
	float           f0, f1, f2		= 0.0;
    int             retval          = 0;
	unsigned int    *buf_ddr 		= NULL;	    // Buffer mapped directly in DDR3 area
    unsigned int    *buf_tx  		= NULL;	    // Locally buffer to send data to DDR3
    unsigned int    *buf_rx  		= NULL;	    // Locally buffer to receive data from DDR3
    unsigned int    *buf_tx_start   = NULL;
    unsigned int    size   		    = 0x40;
    unsigned int    offset 		    = 0x100000; // DDR3 offset memory
    int             d0, d1, d2	    = 0;
    int             data            = 0;
    int             cnt_value       = 0;
    unsigned int    memOrg          = 0;
    unsigned int    init_delay_1[16]          = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
    unsigned int    init_delay_2[16]          = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
    unsigned int    temp_cnt_value_store[16]  = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
    unsigned int    final_cnt_value_store[16] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
    int             dq_path         = 0;
    unsigned int    r, rr	        = 0;
    unsigned int    best            = 0;
    unsigned int    worst           = 0;
    unsigned int    location        = 0;
    int             vtc_read        = 0;
    int             vtc_set         = 0;
    unsigned int    j, k, m, n      = 0;	   // Loop increment
    unsigned int    start           = 0; 	   // Save the start index
    unsigned int    end	            = 0; 	   // Save the end index
    unsigned int    ok              = 0;	   // Count the number of passed test "1"
    unsigned int    avg_x           = 0;       // Horizontal DQ average delay for 16 lines of DQ
    unsigned int    marker          = 0;	   // Final delay marker value per line
    unsigned int    NOK             = 1;	   // Calibration is done or not
    unsigned int    pattern[32]     = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
    unsigned int    ref_pattern[32] = {0, 1, 0, 1, 1, 0, 0, 1, 0, 0, 1, 1, 0, 1, 0, 0, 1, 0, 1, 1, 0, 1, 0, 1, 1, 0, 0, 1, 0, 0, 1, 1};

    // 32 bits reference pattern for each DQ : 0101 1001 0011 0100 1011 0101 1001 0011

    unsigned int word0  = 0xffff0000; // 1111 1111 1111 1111 0000 0000 0000 0000
    unsigned int word1  = 0xffff0000; // 1111 1111 1111 1111 0000 0000 0000 0000
    unsigned int word2  = 0x0000ffff; // 0000 0000 0000 0000 1111 1111 1111 1111
    unsigned int word3  = 0xffff0000; // 1111 1111 1111 1111 0000 0000 0000 0000
    unsigned int word4  = 0x00000000; // 0000 0000 0000 0000 0000 0000 0000 0000
    unsigned int word5  = 0xffffffff; // 1111 1111 1111 1111 1111 1111 1111 1111
    unsigned int word6  = 0xffff0000; // 1111 1111 1111 1111 0000 0000 0000 0000
    unsigned int word7  = 0x00000000; // 0000 0000 0000 0000 0000 0000 0000 0000
    unsigned int word8  = 0x0000ffff; // 0000 0000 0000 0000 1111 1111 1111 1111
    unsigned int word9  = 0xffffffff; // 1111 1111 1111 1111 1111 1111 1111 1111
    unsigned int word10 = 0xffff0000; // 1111 1111 1111 1111 0000 0000 0000 0000
    unsigned int word11 = 0xffff0000; // 1111 1111 1111 1111 0000 0000 0000 0000
    unsigned int word12 = 0x0000ffff; // 0000 0000 0000 0000 1111 1111 1111 1111
    unsigned int word13 = 0xffff0000; // 1111 1111 1111 1111 0000 0000 0000 0000
    unsigned int word14 = 0x00000000; // 0000 0000 0000 0000 0000 0000 0000 0000
    unsigned int word15 = 0xffffffff; // 1111 1111 1111 1111 1111 1111 1111 1111

    ppc = CheckByteOrder(); // Check endianness: 1-> ppc, 0-> x86

    // Check if calibration is needed for mem1, mem2 or mem1 & mem2
    if(mem == 0x1){
    	mem = 1;
    	memOrg = mem;
    	rr = 1;
    }
    else if(mem == 0x2){
    	mem = 2;
    	memOrg = mem;
    	rr = 1;
    }
    else if(mem == 0x12){
    	mem = 1;
    	memOrg = mem;
    	rr = 2;
    }

    /* INITIAL READ */
    /***************************************************************************/

    // Loop on 2 SMEM
    for (r = 0; r < rr; r++){
		// Reset calibration register
		data = 0;
		tsc_csr_write(SMEM_DDR3_IFSTA[mem - 1], &data);
		tsc_csr_write(SMEM_DDR3_IDEL[mem - 1], &data);

		printf("Initial value for MEM%x : \n", mem);

		// Loop on 16 DQ
		for(j = 0; j < 16; j++){
			// Store initial value of count of the IFSTA register
			dq_path = (j << 12);
			tsc_csr_write(SMEM_DDR3_IFSTA[mem - 1], &dq_path);
			tsc_csr_read(SMEM_DDR3_IDEL[mem - 1], &vtc_read); 			// Acquire current value of the register
			vtc_set = (vtc_read | (1 << 28));							// Set value to disable VTC
			tsc_csr_write(SMEM_DDR3_IDEL[mem - 1], &vtc_set); 			// Disable VTC
			tsc_csr_read(SMEM_DDR3_IFSTA[mem - 1], &cnt_value); 		// Read initial value of IFSTA register
			tsc_csr_write(SMEM_DDR3_IDEL[mem - 1], &vtc_read); 			// Re-active active VTC

			// MEM1
			if(r == 0) {
				init_delay_1[j]         = cnt_value & 0x1ff;
				printf("DQ[%02i] IFSTA register 0x%08x -> Initial delay 0x%03x \n", j, cnt_value, init_delay_1[j]);
			}
			// MEM2
			else if (r == 1){
				init_delay_2[j]         = cnt_value & 0x1ff;
				printf("DQ[%02i] IFSTA register 0x%08x -> Initial delay 0x%03x \n", j, cnt_value, init_delay_2[j]);
			}
		}
		mem++;
    }
    printf("\n");

    /* CALIBRATION */
    /***************************************************************************/

    mem = memOrg;

    // Global loop for DDR calibration
    for (r = 0; r < rr; r++){

		// Buffer definition
		buf_tx       = malloc(size);
		buf_rx       = malloc(size);
		buf_tx_start = buf_tx;

		// Map DDR3 memory region
		buf_ddr = NULL;
		memset(&map_win, sizeof(map_win), 0);
		map_win.req.rem_addr   = offset;
		map_win.req.loc_addr   = 0;
		map_win.req.size       = size;
		map_win.req.mode.sg_id = MAP_ID_MAS_PCIE_MEM;

		if ((mem - 1) == 0){
			map_win.req.mode.space = MAP_SPACE_SHM1; // SHM #1
			printf("Calibration of the SMEM1\n");
		}
		else if((mem - 1) == 1){
			map_win.req.mode.space = MAP_SPACE_SHM2; // SHM #2
			printf("Calibration of the SMEM2\n");
		}

		map_win.req.mode.flags = 0;
		retval = tsc_map_alloc(tsc_fd, &map_win);
		if(retval < 0){
			printf("Error in mapping SHM");
			return (-1);
		}

		buf_ddr = mmap(NULL, map_win.req.size, PROT_READ | PROT_WRITE, MAP_SHARED, tsc_fd, map_win.req.loc_addr);
		if(buf_ddr == MAP_FAILED){
			printf("Error MAP FAILED \n");
			return (-1);
		}

		// Init local tx buffer with specific pattern
		*buf_tx = word0;
		buf_tx++;
		*buf_tx = word1;
		buf_tx++;
		*buf_tx = word2;
		buf_tx++;
		*buf_tx = word3;
		buf_tx++;
		*buf_tx = word4;
		buf_tx++;
		*buf_tx = word5;
		buf_tx++;
		*buf_tx = word6;
		buf_tx++;
		*buf_tx = word7;
		buf_tx++;
		*buf_tx = word8;
		buf_tx++;
		*buf_tx = word9;
		buf_tx++;
		*buf_tx = word10;
		buf_tx++;
		*buf_tx = word11;
		buf_tx++;
		*buf_tx = word12;
		buf_tx++;
		*buf_tx = word13;
		buf_tx++;
		*buf_tx = word14;
		buf_tx++;
		*buf_tx = word15;

		buf_tx = buf_tx_start;

		// Acquire temperature and voltage of current system
		d0 = 0x3000;
		tsc_smon_write(tsc_fd, 0x41, &d0);
		printf("   FPGA System Monitoring\n");
		tsc_smon_read(tsc_fd, 0x00, &d0);
		f0 = (((double)(d0 >> 6) * 503.975) / 1024.) - (double)273.15;
		tsc_smon_read(tsc_fd, 0x20, &d1);
		f1 = (((double)(d1 >> 6) * 503.975) / 1024.) - 273.15;
		tsc_smon_read(tsc_fd, 0x24, &d2);
		f2 = (((double)(d2 >> 6) * 503.975) / 1024.) - 273.15;
		printf("      Temperature          : %.2f [%.2f - %.2f]\n", f0, f1, f2);
		tsc_smon_read(tsc_fd, 0x01, &d0);
		f0 = (((double)(d0 >> 6) * 3.0) / 1024.);
		tsc_smon_read(tsc_fd, 0x21, &d1);
		f1 = (((double)(d1 >> 6) * 3.0) / 1024.);
		tsc_smon_read(tsc_fd, 0x25, &d2);
		f2 = (((double)(d2 >> 6) * 3.0) / 1024.);
		printf("      VCCint               : %.2f [%.2f - %.2f]\n", f0, f1, f2);
		tsc_smon_read(tsc_fd, 0x02, &d0);
		f0 = (((double)(d0 >> 6) * 3.0) / 1024.);
		tsc_smon_read(tsc_fd, 0x22, &d1);
		f1 = (((double)(d1 >> 6) * 3.0) / 1024.);
		tsc_smon_read(tsc_fd, 0x26, &d2);
		f2 = (((double)(d2 >> 6) * 3.0) / 1024.);
		printf("      VCCaux               : %.2f [%.2f - %.2f]\n", f0, f1, f2);
		d0 = 3;
		tsc_smon_write(tsc_fd, 0x40, &d0);

		printf("\n");
		printf("Calibration pattern   : 0101 1001 0011 0100 1011 0101 1001 0011 \n");
		printf("Default INC           : %d \n", CURRENT_STEP);
		printf("Default CNT           : %02x \n", DEFAULT_DELAY);
		printf("\n");

		// Reset calibration register
		data = 0;
		tsc_csr_write(tsc_fd, SMEM_DDR3_IFSTA[mem - 1], &data);
		tsc_csr_write(tsc_fd, SMEM_DDR3_IDEL[mem - 1], &data);

		// Pass the entire possible delay taps
		printf("+--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------+ \n");
		printf(" Default delay     :  ");
		for(j = DEFAULT_DELAY; j < MAX; j++){
			if(j == (CURRENT_DLY / CURRENT_STEP)){
				printf("*");
				break;
			}
			else{
				printf("   ");
			}
		}
		printf("\n");
		printf(" Delay value [MSB] : 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 01 01 01 01 01 01 01 01 01 01 01 01 01 01 01 01 01 01 01 01 01 01 01 01 01 01 01 01 01 01 01 01 01 01 01 01 01 01 01 01 01 01 01 01 01 01 01 01 01 01 01 01 01 01 01 01 01 01 01 01 01 01 01 01 \n");
		printf(" Delay value [LSB] : 40 44 48 4C 50 54 58 5C 60 64 68 6C 70 74 78 7C 80 84 88 8C 90 94 98 9C A0 A4 A8 AC B0 B4 B8 BC C0 C4 C8 CC D0 D4 D8 DC E0 E4 E8 EC F0 F4 F8 FC 00 04 08 0C 10 14 18 1C 20 24 28 2C 30 34 38 3F 40 44 48 4C 50 54 58 5C 60 64 68 6C 70 74 78 7C 80 84 88 8C 90 94 98 9C A0 A4 A8 AC B0 B4 B8 BC C0 C4 C8 CC D0 D4 D8 DC E0 E4 E8 EC F0 F4 F8 FC \n");
		printf("+--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------+ \n");

		// Set IDEL to 0
		data = 0;
		tsc_csr_write(tsc_fd, SMEM_DDR3_IDEL[mem - 1], &data);

		// Loop on 16 DQ
		for(j = 0; j < 16; j++){

			// Store initial value of count of the IFSTA register
			dq_path = (j << 12);
			tsc_csr_write(tsc_fd, SMEM_DDR3_IFSTA[mem - 1], &dq_path);
			tsc_csr_read(tsc_fd, SMEM_DDR3_IDEL[mem - 1], &vtc_read); 				// Acquire current value of the register
			vtc_set = (vtc_read | (1 << 28));								// Set value to disable VTC
			tsc_csr_write(tsc_fd, SMEM_DDR3_IDEL[mem - 1], &vtc_set); 				// Disable VTC
			tsc_csr_read(tsc_fd, SMEM_DDR3_IFSTA[mem - 1], &cnt_value); 			// Read initial value of IFSTA register
			tsc_csr_write(tsc_fd, SMEM_DDR3_IDEL[mem - 1], &vtc_read); 				// Re-active active VTC
			temp_cnt_value_store[j] = DEFAULT_DELAY;

			if(j < 8){
				printf(" DQ[%02d] test >>>>> :", j + 8);
			}
			else{
				printf(" DQ[%02d] test >>>>> :", j - 8);
			}

			// Reset avg_x, start index, number of test passed "ok" and end value for each DQ
			avg_x = 0;
			start = 0;
			end   = 0;
			ok    = 0;

			// Add steps by steps for current DQ from initial count value to max
			for(k = DEFAULT_DELAY; k < MAX ; k = k + CURRENT_STEP){
				// Fill DDR3 with test pattern
				memcpy(buf_ddr, buf_tx, size);
				// Get data from DDR3
				memcpy(buf_rx, buf_ddr, size);
				// Acquire test results in result array corresponding to the current DQ
				for(m = 0; m < 16; m++){
					pattern[m * 2]       =  (buf_rx[m] & (0x1 << j)) >>  j;
					pattern[(m * 2) + 1] =  (buf_rx[m] & (0x1 << (16 + j))) >> (16 + j);
				}

				// Check data received with reference pattern
				if (!memcmp(pattern, ref_pattern, 32 * sizeof(int))){
					printf("  Y");
					end   = k;
					NOK   = 0;
					ok++;
				}
				else{
					printf("  -");
				}

				// Increment only the tap delay when we are < MAX tap
				if(k < (MAX - CURRENT_STEP)) {

if (ppc == 1) {
					if(j < 8){
						// Compute new count value and write IFSTA
						data = (temp_cnt_value_store[j] + CURRENT_STEP) << 16;
						tsc_csr_write(tsc_fd, SMEM_DDR3_IFSTA[mem - 1], &data);

						// Load new count value
						data = (1 << 31) | (0x1 << (j + 8));
						tsc_csr_write(tsc_fd, SMEM_DDR3_IDEL[mem - 1], &data);

						// Update new value of count
						temp_cnt_value_store[j] = temp_cnt_value_store[j] + CURRENT_STEP;
					}
					else {
						// Compute new count value and write IFSTA
						data = (temp_cnt_value_store[j] + CURRENT_STEP) << 16;
						tsc_csr_write(tsc_fd, SMEM_DDR3_IFSTA[mem - 1], &data);

						// Load new count value
						data = (1 << 31) | (0x1 << (j - 8));
						tsc_csr_write(tsc_fd, SMEM_DDR3_IDEL[mem - 1 ], &data);


						// Update new value of count
						temp_cnt_value_store[j] = temp_cnt_value_store[j] + CURRENT_STEP;
					}
}
else {
					// Compute new count value and write IFSTA
					data = (temp_cnt_value_store[j] + CURRENT_STEP) << 16;
					tsc_csr_write(tsc_fd, SMEM_DDR3_IFSTA[mem - 1], &data);

					// Load new count value
					data = (1 << 31) | (0x1 << (j));
					tsc_csr_write(tsc_fd, SMEM_DDR3_IDEL[mem - 1], &data);

					// Update new value of count
					temp_cnt_value_store[j] = temp_cnt_value_store[j] + CURRENT_STEP;
}
				}
			}

			// If calibration failed set the default count value
			if (ok == 0){
				marker = DEFAULT_DELAY;
			}
			// Update the new count value with the median value
			else {
				// Compute the start window
				start = (end - (ok * CURRENT_STEP)) + CURRENT_STEP;

				// Compute the average of the window
				avg_x = (ok * CURRENT_STEP) / 2;

				// Compute the new delay marker to apply
				marker = start + avg_x;
			}

			// Update the array with the new count value
			final_cnt_value_store[j] = marker;

			if(j < 8){
				// Trace new delay
				printf("\n");
				// MEM1
				if(r == 0) {
					printf(" Init  delay 0x%03x :\n", init_delay_1[j + 8]);
				}
				// MEM 2
				else if (r == 1){
					printf(" Init  delay 0x%03x :\n", init_delay_2[j + 8]);
				}
				printf(" Final delay 0x%03x :", marker);
				for(n = DEFAULT_DELAY ; n < marker; n = n + CURRENT_STEP){
					printf("   ");
				}
				printf("  *");
				printf("\n");
			}
			else{
				// Trace new delay
				printf("\n");
				// MEM1
				if(r == 0) {
					printf(" Init  delay 0x%03x :\n", init_delay_1[j - 8]);
				}
				// MEM 2
				else if (r == 1){
					printf(" Init  delay 0x%03x :\n", init_delay_2[j - 8]);
				}
				printf(" Final delay 0x%03x :", marker);
				for(n = DEFAULT_DELAY ; n < marker; n = n + CURRENT_STEP){
					printf("   ");
				}
				printf("  *");
				printf("\n");
			}

if (ppc == 1) {
			if(j < 8){
				// Compute new count value and write IFSTA
				data = final_cnt_value_store[j] << 16;
				tsc_csr_write(tsc_fd, SMEM_DDR3_IFSTA[mem - 1], &data);

				// Load new count value
				data = (1 << 31) | (0x1 << (j + 8));
				tsc_csr_write(tsc_fd, SMEM_DDR3_IDEL[mem - 1], &data);
			}
			else {
				// Compute new count value and write IFSTA
				data = final_cnt_value_store[j] << 16;
				tsc_csr_write(tsc_fd, SMEM_DDR3_IFSTA[mem - 1], &data);

				// Load new count value
				data = (1 << 31) | (0x1 << (j - 8));
				tsc_csr_write(tsc_fd, SMEM_DDR3_IDEL[mem - 1 ], &data);
			}
}
else {
			// Compute new count value and write IFSTA
			data = final_cnt_value_store[j] << 16;
			tsc_csr_write(tsc_fd, SMEM_DDR3_IFSTA[mem - 1], &data);

			// Load new count value
			data = (1 << 31) | (0x1 << (j));
			tsc_csr_write(tsc_fd, SMEM_DDR3_IDEL[mem - 1], &data);
}

			// Check status
			if (ok == 0){
				NOK = 1;
				DQ_NOK[j] = 1;
			}
			else {
				DQ_NOK[j] = 0;
			}

			DQ_OK[j] = ok;

			printf("+--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------+ \n");

			// Set IDEL and IFSTA to 0
			data = 0;
			tsc_csr_write(tsc_fd, SMEM_DDR3_IDEL[mem - 1], &data);
			tsc_csr_write(tsc_fd, SMEM_DDR3_IFSTA[mem - 1], &data);

		}

		// Execution is finished OK or NOK
		printf("\n");
		if (NOK == 1){
			printf("Calibration is not possible, error on line(s) : \n");
			for (m = 0; m < 16; m++){
				if (DQ_NOK[m] == 1){
					printf("DQ[%i] \n", m);
				}
			}
		}
		else {
			// Search best case
		    best = DQ_OK[0];

		    for (m = 1 ;m < 16 ;m++) {
		    	if ( DQ_OK[m] > best ) {
		    		//best = DQ_OK[m];
		    		if (m < 8){
		    			best = DQ_OK[m + 8];
		    			location = m + 8;
		    		}
		    		else {
		    			best = DQ_OK[m - 8];
		    			location = m - 8;
		    		}
		        }
		    }
		    printf("Best calibration window size is %i for DQ[%02i] \n", best, location);

			// Search worst case
		    worst = DQ_OK[0];

		    for (m = 1 ;m < 16 ;m++) {
		    	if ( DQ_OK[m] < worst ) {
		    		worst = DQ_OK[m];
		    		if (m < 8){
		    			best = DQ_OK[m + 8];
		    			location = m + 8;
		    		}
		    		else {
		    			best = DQ_OK[m - 8];
		    			location = m - 8;
		    		}
		        }
		    }
		    printf("Worst calibration windows size is %i for DQ[%02i] \n", worst, location);
		}

		// Print initial and final

		printf("\n");
		if (r == 0){ // MEM1
			for (m = 0 ;m < 16 ;m++) {
				if(m < 8){
					printf("DQ[%02d] - Initial delay 0x%03x - Final delay 0x%03x \n", m, init_delay_1[m], final_cnt_value_store[m + 8]);
				}
				else{
					printf("DQ[%02d] - Initial delay 0x%03x - Final delay 0x%03x \n", m, init_delay_1[m], final_cnt_value_store[m - 8]);
				}
			}
		}
		else if (r == 1){ // MEM2
			for (m = 0 ;m < 16 ;m++) {
				if(m < 8){
					printf("DQ[%02d] - Initial delay 0x%03x - Final delay 0x%03x \n", m, init_delay_2[m], final_cnt_value_store[m + 8]);
				}
				else{
					printf("DQ[%02d] - Initial delay 0x%03x - Final delay 0x%03x \n", m, init_delay_2[m], final_cnt_value_store[m - 8]);
				}
			}
		}

		printf("\n");
		printf("Calibration finished ! \n");
		printf("\n");

		// Set IDEL and IFSTA to 0
		data = 0;
		tsc_csr_write(tsc_fd, SMEM_DDR3_IDEL[mem - 1], &data);
		tsc_csr_write(tsc_fd, SMEM_DDR3_IFSTA[mem - 1], &data);

		// Unmap DDR3 memory
		munmap(buf_ddr, map_win.req.size);
		tsc_map_free(tsc_fd, &map_win);

		// Free buffer
		free(buf_tx);
		free(buf_rx);

		mem++;
    }

    /* FINAL READ */
    /***************************************************************************/

    // Check if calibration is needed for mem1, mem2 or mem1 & mem2
    mem = memOrg;

    // Loop on SMEM
    for (r = 0; r < rr; r++){
    	// Reset calibration register
    	data = 0;
    	tsc_csr_write(SMEM_DDR3_IFSTA[mem - 1], &data);
    	tsc_csr_write(SMEM_DDR3_IDEL[mem - 1], &data);

    	printf("Final value for MEM%x : \n", mem);

    	// Loop on 16 DQ
    	for(j = 0; j < 16; j++){
    		// Store initial value of count of the IFSTA register
    		dq_path = (j << 12);
    		tsc_csr_write(SMEM_DDR3_IFSTA[mem - 1], &dq_path);
    		tsc_csr_read(SMEM_DDR3_IDEL[mem - 1], &vtc_read); 			// Acquire current value of the register
    		vtc_set = (vtc_read | (1 << 28));							// Set value to disable VTC
    		tsc_csr_write(SMEM_DDR3_IDEL[mem - 1], &vtc_set); 			// Disable VTC
    		tsc_csr_read(SMEM_DDR3_IFSTA[mem - 1], &cnt_value); 		// Read initial value of IFSTA register
    		tsc_csr_write(SMEM_DDR3_IDEL[mem - 1], &vtc_read); 			// Re-active active VTC

    		// MEM1
   			if(r == 0) {
   				printf("DQ[%02i] Initial delay 0x%03x - IFSTA register 0x%08x -> Final delay 0x%03x \n", j, init_delay_1[j], cnt_value, cnt_value & 0x1ff);
   			}
   			// MEM2
   			else if (r == 1){
   				printf("DQ[%02i] Initial delay 0x%03x - IFSTA register 0x%08x -> Final delay 0x%03x \n", j, init_delay_2[j], cnt_value, cnt_value & 0x1ff);
   			}
   		}

    	// Set IDEL and IFSTA to 0
    	data = 0;
    	tsc_csr_write(SMEM_DDR3_IDEL[mem - 1], &data);
    	tsc_csr_write(SMEM_DDR3_IFSTA[mem - 1], &data);

   		mem++;

    }
    printf("\n");


    return 0;
}

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * Function name : tsc_ddr
 * Prototype     : int
 * Parameters    : command line parameter structure
 * Return        : error/success
 *----------------------------------------------------------------------------
 * Description   : main command to drive ddr calibration
 *
 *++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

int tsc_ddr(struct cli_cmd_para *c){
	int cnt = 0;
	//int idx = 0;
	cnt = c->cnt;
	//unsigned int dq   = 0;
	//unsigned int sts  = 0;
	//unsigned int tag  = 0;
	unsigned int mem  = 0;
	//unsigned int step = 0;
	//char *p;

// Select sub command and check syntax
	if(cnt--) {

// DDR command
		if((!strcmp("calib", c->para[1]))) {
			// Acquire and transform parameter
			sscanf(c->para[2], "%x", &mem);
			// SMEM1 or SMEM2 or both SMEM directly
			if ((mem == 1) | (mem == 2) | (mem == 0x12)){
				tsc_ddr_idel_calib(mem);
			}
			else {
				printf("Bad value! Type \"? smem\" for help \n");
			}
		}
		/*else if((!strcmp("reset", c->para[1])) && (c->cnt == 3)) {
			sscanf(c->para[2], "%x", &mem);
			if ((mem < 1) | (mem > 2)){
				printf("Bad value! Type \"? smem\" for help \n");
			}
			else {
				tsc_ddr_idel_reset(mem);
			}
		}
		else if((!strcmp("status", c->para[1])) && (c->cnt == 3)) {
			sscanf(c->para[2], "%x", &mem);
			if ((mem < 1) | (mem > 2)){
				printf("Bad value! Type \"? smem\" for help \n");
			}
			else {
				tsc_ddr_idel_status(mem);
			}
		}
		else if((!strcmp("set", c->para[1])) && (c->cnt == 6)) {
			// Acquire and transform parameter to unsigned int
			sscanf(c->para[2], "%x", &mem);
			sscanf(c->para[3], "%x", &dq);
			sscanf(c->para[4], "%x", &step);

			if((mem < 1) | (mem > 2) | (dq > 0xffff) | (step > 16) | (step < 1)) {
				printf("Bad value! Type \"? smem\" for help \n");
			}
			else {
				tsc_ddr_idel_set(mem, dq, step, c->para[4]);
			}
		}*/
		else {
		    printf("Not enough arguments -> usage:\n");
		    tsc_print_usage(c);
			return(-1);
		}
	}
	// Bad command
	else {
	    printf("Not enough arguments -> usage:\n");
	    tsc_print_usage(c);
		return(-1);
	}

return 0;
}
