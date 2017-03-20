/*=========================< begin file & file header >=======================
 *  References
 *
 *    filename : ddr.c
 *    author   : Xavier Paillard
 *    company  : IOxOS
 *    creation : February 2017
 *    version  : 0.0.1
 *
 *----------------------------------------------------------------------------
 *  Description
 *
 *    That file contains a set of function called by TscMon to perform
 *    specifics command on DDR memory
 *
 *----------------------------------------------------------------------------
 *  Copyright Notice
 *
 *    Copyright and all other rights in this document are reserved by
 *    IOxOS Technologies SA. This documents contains proprietary information
 *    and is supplied on express condition that it may not be disclosed,
 *    reproduced in whole or in part, or used for any other purpose other
 *    than that for which it is supplies, without the written consent of
 *    IOxOS Technologies SA
 *
 *----------------------------------------------------------------------------
 *  Change History
 *
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

#include "ddr.h"

// IDEL control register for both DDR3 memory
unsigned int SMEM_DDR3_IDEL[2] = {0x80c, 0xc0c};

extern int tsc_fd;

// Print hex to bin
// ----------------------------------------------------------------------------------
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

// Althea ddr reset
// Reset to default DDR3 memory delay
// ----------------------------------------------------------------------------------
int althea_ddr_idel_reset(int mem){
	unsigned int data = 0;

	printf("Loading Althea DDR3 #%d default IDELAY ... \n", mem);
	data = 0x8000ffff;
	tsc_csr_write(SMEM_DDR3_IDEL[mem - 1], &data); // Load default delay
	usleep(1000);
	data = 0x00000000;
	tsc_csr_write(SMEM_DDR3_IDEL[mem - 1], &data); // Unselect all lines
	printf("DDR3 #%d done ... \n", mem);

	return 0;
}

// Althea ddr set IDELAY
// Set the IDELAY +/- for selected lane
// |----- x -----|
// Step is 1 to 16
// ----------------------------------------------------------------------------------
int althea_ddr_idel_set(int mem, unsigned int dq, unsigned int step, char* pm){
	unsigned int data = 0;

	// -- Decrement the delay --
	if (*pm == '-') {
		data = (dq & 0x0000ffff) | ((step & 0x00000000f) << 20) | ((1 & 0x00000001) << 29);
		tsc_csr_write(SMEM_DDR3_IDEL[mem - 1], &data);
	}
	// -- Increment the delay --
	else if(*pm == '+') {
		data = (dq & 0x0000ffff) | ((step & 0x00000000f) << 20) | ((1 & 0x00000001) << 30);
		tsc_csr_write(SMEM_DDR3_IDEL[mem - 1], &data);
	}
	printf("DDR3 #%d IDELAY set done ...\n", mem);

	return 0;
}

// Show the current status of DDR DQ line
// ----------------------------------------------------------------------------------
int althea_ddr_idel_status(int mem){
	unsigned int d0   = 0;
	unsigned int temp = 0;
	unsigned int mask = 0;


	printf("Althea DDR3 #%d IDELAY status\n", mem);
	printf("-------------------------------- \n");
	tsc_csr_read(SMEM_DDR3_IDEL[mem - 1], &d0);
	// -- DQ[15:0] (data bus) --
	mask = 0x0000ffff;
	temp = d0 & mask;
	printf("DQ[15:0] : ");
	bin(temp,16);
	printf(" [0x%04x]", temp);
	printf("\n");
	printf("-------------------------------- \n");

    return 0;
}

// Althea ddr align
// Align the DDR memory delay
// When adjust delay, R/W is needed to apply delay
// There are 32 delays taps 0..31
// Delay register is wrap-around
// MAthing DQ <-> DATA
// DQ[15..8] -> DATA[7..0]
// DQ[07..0] -> DATA[15..8]
// ----------------------------------------------------------------------------------
int althea_ddr_idel_calib(int mem){
	struct tsc_ioctl_map_win map_win;
	float   f0, f1, f2	    		= 0.0;
    int             retval          = 0;
	unsigned int    *buf_ddr 		= NULL;	    // Buffer mapped directly in DDR3 area
    unsigned int    *buf_tx  		= NULL;	    // Locally buffer to send data to DDR3
    unsigned int    *buf_rx  		= NULL;	    // Locally buffer to receive data from DDR3
    unsigned int    *buf_tx_start   = NULL;
    unsigned int    size   		    = 0x40;
    unsigned int    offset 		    = 0x100000; // DDR3 offset memory
    unsigned int    d0, d1, d2	    = 0;
    unsigned int    data            = 0;

    ////////////////////////////////////////////////////////////////////////////////////
    // ADJUST DEFAULT VALUE FOR CURRENT ACCORDING TO HARDWARE IMPLEMENTATION          //
    unsigned int    CURRENT         = 19;	   // Current delay value                         //
    ////////////////////////////////////////////////////////////////////////////////////

    unsigned int    MAX             = 32;      // Max delay tap value
    unsigned int    j, k, m, n      = 0;
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

    // Buffer definition
	buf_tx       = malloc(size);
	buf_rx       = malloc(size);
	buf_tx_start = buf_tx;

	// Map DDR3 memory region --
    buf_ddr = NULL;
    memset(&map_win, sizeof(map_win), 0);

    map_win.req.rem_addr   = offset;
    map_win.req.loc_addr   = 0;
	map_win.req.size       = size;
	map_win.req.mode.sg_id = MAP_ID_MAS_PCIE_MEM;
	if ((mem - 1) == 0){
		map_win.req.mode.space = MAP_SPACE_SHM1; // SHM #1
	}
	else if((mem - 1) == 1){
		map_win.req.mode.space = MAP_SPACE_SHM2; // SHM #2
	}

	map_win.req.mode.flags = 0;

	retval = tsc_map_alloc(&map_win);
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
    tsc_smon_write(0x41, &d0);
    printf("   Artix7 System Monitoring\n");
    tsc_smon_read(0x00, &d0);
    f0 = (((double)(d0 >> 6) * 503.975) / 1024.) - (double)273.15;
    tsc_smon_read(0x20, &d1);
    f1 = (((double)(d1 >> 6) * 503.975) / 1024.) - 273.15;
    tsc_smon_read(0x24, &d2);
    f2 = (((double)(d2 >> 6) * 503.975) / 1024.) - 273.15;
    printf("      Temperature          : %.2f [%.2f - %.2f]\n", f0, f1, f2);
    tsc_smon_read(0x01, &d0);
    f0 = (((double)(d0 >> 6) * 3.0) / 1024.);
    tsc_smon_read(0x21, &d1);
    f1 = (((double)(d1 >> 6) * 3.0) / 1024.);
    tsc_smon_read(0x25, &d2);
    f2 = (((double)(d2 >> 6) * 3.0) / 1024.);
    printf("      VCCint               : %.2f [%.2f - %.2f]\n", f0, f1, f2);
    tsc_smon_read(0x02, &d0);
    f0 = (((double)(d0 >> 6) * 3.0) / 1024.);
    tsc_smon_read(0x22, &d1);
    f1 = (((double)(d1 >> 6) * 3.0) / 1024.);
    tsc_smon_read(0x26, &d2);
    f2 = (((double)(d2 >> 6) * 3.0) / 1024.);
    printf("      VCCaux               : %.2f [%.2f - %.2f]\n", f0, f1, f2);
    d0 = 3;
    tsc_smon_write(0x40, &d0);

	printf("\n");
	printf("Enter hardware default DQ delay (default value: %i): ", CURRENT);
	if (scanf(" %d", &CURRENT) != 1) {
	  printf("ERROR ! \n");
	}
	printf(" %i", CURRENT);
	printf("\n");
    printf("Calibration pattern   : 0101 1001 0011 0100 1011 0101 1001 0011 \n");
    printf("Default absolute IDEL : DQ = %i \n", CURRENT);
    printf("\n");

    // Pass the entire possible delay taps
    printf("+---------------------------------------------------------------------------------------------------------------+ \n");
    printf(" Default delay :  ");
    for(j = 0; j < MAX; j++){
    	if(j == CURRENT){
    		printf("*");
    		break;
    	}
    	else{
    		printf("   ");
    	}
    }
    printf("\n");
    printf(" Delay value   : 00 01 02 03 04 05 06 07 08 09 10 11 12 13 14 15 16 17 18 19 20 21 22 23 24 25 26 27 28 29 30 31  \n");
    printf("+---------------------------------------------------------------------------------------------------------------+ \n");

	// Reset all
    data = 0x8000ffff;
    tsc_csr_write(SMEM_DDR3_IDEL[mem - 1], &data); // Load initial DQ
    data = 0x00000000;
    tsc_csr_write(SMEM_DDR3_IDEL[mem - 1], &data); // Unselect all lines (DQ)

	// Loop on 16 DQ
    for(j = 0; j < 16; j++){
		printf(" DQ[%02d] test   :", j);

		// Reset avg_x, start index, number of test passed "ok" and end value for each DQ
		avg_x = 0;
		start = 0;
		end   = 0;
		ok    = 0;

		// Substract maximal steps for current DQ
		for(k = CURRENT; k-- > 0 ; ){
#ifdef PPC
			if(j < 8){
				data = (0x20000000 | (0x1 << (j + 8)));
				tsc_csr_write(SMEM_DDR3_IDEL[mem - 1], &data); // -
			}
			else {
				data = (0x20000000 | (0x1 << (j - 8)));
				tsc_csr_write(SMEM_DDR3_IDEL[mem - 1], &data); // -
			}
#else
			data = (0x20000000 | (0x1 << (j)));
			tsc_csr_write(SMEM_DDR3_IDEL[mem - 1 ], &data); // -
#endif

			// Fill DDR3 with test pattern
			memcpy(buf_ddr, buf_tx, size);
			// Get data from DDR3
			memcpy(buf_rx, buf_ddr, size);
		}

		// Fill DDR3 with test pattern
		memcpy(buf_ddr, buf_tx, size);
		// Get data from DDR3
		memcpy(buf_rx, buf_ddr, size);

		// Add steps by steps for current DQ
		for(k = 0; k < MAX ; k++){
			// Fill DDR3 with test pattern
			memcpy(buf_ddr, buf_tx, size);
			// Get data from DDR3
			memcpy(buf_rx, buf_ddr, size);

			// Acquire test results in result array corresponding to the current DQ
			for(m = 0; m < 16; m++){
				pattern[m * 2]       =  (buf_rx[m] & (0x1 << j)) >>  j;
				pattern[(m * 2) + 1] =  (buf_rx[m] & (0x1 << (16 + j))) >> (16 + j);
			}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////
// DEBUG
// Print pattern
/*
			if (k != 0){
				printf("            :");
			}

			for(m = 0; m < 32; m++){
				printf(" %x ",pattern[m]);
			}
			printf("\n");

			printf("               :");
			for(m = 0; m < 32; m++){
				printf(" %x ",ref_pattern[m]);
			}
			printf("\n");

			// Print pattern
			for(m = 0; m < 16; m++){
				printf("buf [%i] %x \n", m, buf_rx[m]);
				printf("buf lsb [%i] %x \n", m, (buf_rx[m] & (0x00000001 << j)) >>  j);
				printf("buf msb [%i] %x \n", m, (buf_rx[m] & (0x1 << (16 + j))) >> (16 + j));
			}
*/
/////////////////////////////////////////////////////////////////////////////////////////////////////////////

			// Check data received with reference pattern
			if (!memcmp(pattern, ref_pattern, 32 * sizeof(int))){
				printf("  Y");
				end   = k;
				NOK   = 0;
				ok++;
			}
			else{
				printf("  N");
			}

			// Increment only the tap delay when we are < 31 tap
			if(k < (MAX -1)) {
#ifdef PPC
				if(j < 8){
					data = (0x40000000 | (0x1 << (j + 8)));
					tsc_csr_write(SMEM_DDR3_IDEL[mem - 1], &data); // +
				}
				else {
					data = (0x40000000 | (0x1 << (j - 8)));
					tsc_csr_write(SMEM_DDR3_IDEL[mem - 1 ], &data); // +
				}
#else
				data = (0x40000000 | (0x1 << (j)));
				tsc_csr_write(SMEM_DDR3_IDEL[mem - 1], &data); // +
#endif
			}

			// Do transfer
			// Fill DDR3 with test pattern
			memcpy(buf_ddr, buf_tx, size);
			// -- Get data from DDR3
			memcpy(buf_rx, buf_ddr, size);

		}

		// Compute the start window
		start = (end - ok) + 1;

		// Compute the average of the window
		avg_x = ok / 2;

		// Compute the new delay marker to apply
		marker = start + avg_x;

		// Trace new delay
		printf("\n");
		printf(" Final delay %i:", marker);
		for(n = 0; n < marker; n++){
			printf("   ");
		}
		printf("  *");
		printf("\n");
		printf("+---------------------------------------------------------------------------------------------------------------+ \n");

		// Place delay cursor on the left
		for(k = 0; k < MAX ; k++){
#ifdef PPC
			if(j < 8){
				data = (0x20000000 | (0x1 << (j + 8)));
				tsc_csr_write(SMEM_DDR3_IDEL[mem - 1], &data); // -
			}
			else {
				data = (0x20000000 | (0x1 << (j - 8)));
				tsc_csr_write(SMEM_DDR3_IDEL[mem - 1], &data); // -
			}
#else
			data = (0x20000000 | (0x1 << (j)));
			tsc_csr_write(SMEM_DDR3_IDEL[mem - 1], &data); // -
#endif

			// Fill DDR3 with test pattern
			memcpy(buf_ddr, buf_tx, size);
			// Get data from DDR3
			memcpy(buf_rx, buf_ddr, size);
		}

		// Fill DDR3 with test pattern
		memcpy(buf_ddr, buf_tx, size);
		// Get data from DDR3
		memcpy(buf_rx, buf_ddr, size);

		// Place delay cursor on the good delay according to the marker value for current DQ
		for(k = 0; k < marker ; k++){
#ifdef PPC
			if(j < 8){
				data = (0x40000000 | (0x1 << (j + 8)));
				tsc_csr_write(SMEM_DDR3_IDEL[mem - 1], &data); // +
			}
			else {
				data = (0x40000000 | (0x1 << (j - 8)));
				tsc_csr_write(SMEM_DDR3_IDEL[mem - 1], &data); // +
			}
#else
			data = (0x40000000 | (0x1 << (j)));
			tsc_csr_write(SMEM_DDR3_IDEL[mem - 1], &data); // +
#endif

			// Fill DDR3 with test pattern
			memcpy(buf_ddr, buf_tx, size);
			// Get data from DDR3
			memcpy(buf_rx, buf_ddr, size);
		}

		// Fill DDR3 with test pattern
		memcpy(buf_ddr, buf_tx, size);
		// Get data from DDR3
		memcpy(buf_rx, buf_ddr, size);

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// DEBUG
/*
		printf("start: %i, end: %i, ok: %i, avg: %i, marker: %i \n", start, end, ok, avg_x, marker);
		printf("\n");
*/
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

		data = 0x00000000;
		tsc_csr_write(SMEM_DDR3_IDEL[mem - 1], &data); // Unselect all lines
    }

    printf("\n");
	// Execution is finished OK or NOK
	if (NOK == 1){
	 	printf("Calibration is not possible, error ! \n");
	}
	else {
		printf("Calibration done ! \n");
	}

	data = 0x00000000;
	tsc_csr_write(SMEM_DDR3_IDEL[mem - 1], &data); // Unselect all lines

	munmap(buf_ddr, map_win.req.size); // Unmap DDR3 memory
	tsc_map_free(&map_win);

    free(buf_tx);
    free(buf_rx);
	return 0;
}

// Main function for Althea command
// ----------------------------------------------------------------------------------
int tsc_ddr(struct cli_cmd_para *c){
	int cnt = 0;
	int idx = 0;
	cnt = c->cnt;
	unsigned int dq   = 0;
	unsigned int sts  = 0;
	unsigned int tag  = 0;
	unsigned int mem  = 0;
	unsigned int step = 0;
	char *p;

// Select sub command and check syntax
	if(cnt--) {

// DDR command
		if((!strcmp("calib", c->para[0])) && (c->cnt == 2)) {
			// Acquire and transform parameter
			sscanf(c->para[1], "%x", &mem);
			if ((mem < 1) | (mem > 2)){
				printf("Bad value! Type \"? althea\" for help \n");
			}
			else {
				althea_ddr_idel_calib(mem);
			}
		}
		else if((!strcmp("reset", c->para[0])) && (c->cnt == 2)) {
			sscanf(c->para[1], "%x", &mem);
			if ((mem < 1) | (mem > 2)){
				printf("Bad value! Type \"? althea\" for help \n");
			}
			else {
				althea_ddr_idel_reset(mem);
			}
		}
		else if((!strcmp("status", c->para[0])) && (c->cnt == 2)) {
			sscanf(c->para[1], "%x", &mem);
			if ((mem < 1) | (mem > 2)){
				printf("Bad value! Type \"? althea\" for help \n");
			}
			else {
				althea_ddr_idel_status(mem);
			}
		}
		else if((!strcmp("set", c->para[0])) && (c->cnt == 5)) {
			// Acquire and transform parameter to unsigned int
			sscanf(c->para[1], "%x", &mem);
			sscanf(c->para[2], "%x", &dq);
			sscanf(c->para[3], "%x", &step);

			if((mem < 1) | (mem > 2) | (dq > 0xffff) | (step > 16) | (step < 1)) {
				printf("Bad value! Type \"? althea\" for help \n");
			}
			else {
				althea_ddr_idel_set(mem, dq, step, c->para[4]);
			}
		}
		else {
			printf("Bad parameter! Type \"? ddr\" for help \n");
			return(-1);
		}
	}
// Bad command
	else {
		printf("Bad parameter! Type \"? ddr\" for help \n");
		return(-1);
	}
return 0;
}
