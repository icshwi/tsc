/*=========================< begin file & file header >=======================
 *  References
 *
 *    filename : tst_0x.c
 *    author   : JFG, XP
 *    company  : IOxOS
 *    creation : May 12,2016
 *
 *----------------------------------------------------------------------------
 *  Description
 *
 *	  Memory tests.
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

#include <debug.h>
#include <sys/time.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <aio.h>
#include <errno.h>
#include <ctype.h>
#include <time.h>
#include <sys/ioctl.h>

#include <cli.h>
#include <tscioctl.h>
#include <tsculib.h>
#include <tscmasioctl.h>
#include <tscslvioctl.h>
#include "tsctst.h"
#include "tstlib.h"

extern int tst_check_cmd_tstop(void);
char *ident="        ";

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * Function name : tst_config
 * Prototype     : int
 * Parameters    : test control structure, test id
 * Return        : error/success
 *
 *----------------------------------------------------------------------------
 * Description   : get driver information
 *
 *++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

int tst_config(struct tst_ctl *tc, char *tst_id){
	time_t tm;
	char *ct;
	int d0     = 0;
	int retval = 0;

	tm = time(0);
	ct = ctime(&tm);

	TST_LOG( tc, (logline, "\n%s->Entering:%s", tst_id, ct));
	TST_LOG( tc, (logline, "%s->Checking %s configuration", tst_id, tsc_get_drv_name()));
	TST_LOG( tc, (logline, "\n%sDriver         = %s                           -> OK", ident, tsc_get_drv_version()));
	TST_LOG( tc, (logline, "\n%sLibrary        = %s                           -> OK", ident, tsc_get_lib_version()));
	TST_LOG( tc, (logline, "\n%sVendor         = %x                           -> OK", ident, tsc_get_vendor_id()));
	TST_LOG( tc, (logline, "\n%sDevice         = %x                           -> OK", ident, tsc_get_device_id()));

	TST_LOG( tc, (logline, "\nFPGA idenfitiers\n"));
	tsc_csr_read(TSC_CSR_ILOC_EFUSE_USR, &d0);
	TST_LOG( tc, (logline, "FPGA  eFUSE          : 0x%08x\n", d0));
	tsc_csr_read(TSC_CSR_ILOC_SIGN, &d0);
	TST_LOG( tc, (logline, "FPGA  Signature      : 0x%08x\n", d0));
	tsc_csr_read(TSC_CSR_ILOC_GENCTL, &d0);
	TST_LOG(tc, (logline, "FPGA  Version        : 0x%08x\n", d0));
	tsc_csr_read(TSC_CSR_ILOC_TOSCA2_SIGN, &d0);
	TST_LOG(tc, (logline, "TOSCA Signature      : 0x%08x\n", d0));

	tm = time(0);
	ct = ctime(&tm);

	TST_LOG( tc, (logline, "\n%s->Exiting:%s", tst_id, ct));

	return( retval | TST_STS_DONE);
}

int tst_01(struct tst_ctl *tc){
	return(tst_config(tc, "Tst:01"));
}

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * Function name : tst_shm_calibration
 * Prototype     : int
 * Parameters    : test control structure, test id
 * Return        : error/success
 *----------------------------------------------------------------------------
 * Description   : calibrate ddr smem1 and smem2 and test it
 *
 *++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

int tst_shm_calibration(struct tst_ctl *tc, char *tst_id){
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

	// IDEL adjustment register for both DDR3 memory
	unsigned int SMEM_DDR3_IFSTA[2] = {0x808, 0xc08};
	// IDEL control register for both DDR3 memory
	unsigned int SMEM_DDR3_IDEL[2] = {0x80c, 0xc0c};
	// SMEM control & status register
	unsigned int SMEM_DDR3_CSR[2] = {0x800, 0xc00};

    time_t tm;
    char *ct                        = NULL;
	struct tsc_ioctl_map_win map_win;
	int             DQ_NOK[16];
	int             DQ_OK[16];
	int             ppc             = 0;
	float           f0, f1, f2		= 0.0;
    int             retval          = 0;
    int             tsc_fd          = -1;
	unsigned int    *buf_ddr 		= NULL;	    // Buffer mapped directly in DDR3 area
    unsigned int    *buf_tx  		= NULL;	    // Locally buffer to send data to DDR3
    unsigned int    *buf_rx  		= NULL;	    // Locally buffer to receive data from DDR3
    unsigned int    *buf_tx_start   = NULL;
    unsigned int    size   		    = 0x40;
    unsigned int    offset 		    = 0x100000; // DDR3 offset memory
    int    d0, d1, d2	    = 0;
    int    data            = 0;
    int    cnt_value       = 0;
    unsigned int    memOrg          = 0;
    unsigned int    mem             = 0x12;
    unsigned int    init_delay_1[16]          = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
    unsigned int    init_delay_2[16]          = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
    unsigned int    temp_cnt_value_store[16]  = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
    unsigned int    final_cnt_value_store[16] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
    int    dq_path         = 0;
    unsigned int    r, rr	        = 0;
    unsigned int    best            = 0;
    unsigned int    worst           = 0;
    unsigned int    location        = 0;
    int    vtc_read        = 0;
    int    vtc_set         = 0;
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

	tsc_fd = tc->fd;

	tm = time(0);
	ct = ctime(&tm);

	TST_LOG( tc, (logline, "%s->Entering:%s\n", tst_id, ct));
	TST_LOG( tc, (logline, "%s->Executing SMEM1 & SMEM2 calibration n", tst_id));

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

    /* Reset memory controller */
    /***************************************************************************/
	// Only on the first memory due to the fact that reset impact both memory
	// Calibration need to be done in the order : 1 -> 2
    // Don't modify the bit 7
	if(mem == 1){
		tsc_csr_read(SMEM_DDR3_CSR[mem - 1], &data);
		data = 0x8000 | (data & (1 << 7));
		tsc_csr_write(SMEM_DDR3_CSR[mem - 1], &data);

		usleep(20000);

		tsc_csr_read(SMEM_DDR3_CSR[mem - 1], &data);
		data = 0x2000 | (data & (1 << 7));
		tsc_csr_write(SMEM_DDR3_CSR[mem - 1], &data);

		usleep(20000);
	}

    /* INITIAL READ */
    /***************************************************************************/

    // Loop on 2 SMEM
    for (r = 0; r < rr; r++){
		// Reset calibration register
		data = 0;
		tsc_csr_write(SMEM_DDR3_IFSTA[mem - 1], &data);
		tsc_csr_write(SMEM_DDR3_IDEL[mem - 1], &data);

		TST_LOG( tc, (logline, "Initial value for MEM%x : \n", mem));

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
				TST_LOG( tc, (logline, "DQ[%02i] IFSTA register 0x%08x -> Initial delay 0x%03x \n", j, cnt_value, init_delay_1[j]));
			}
			// MEM2
			else if (r == 1){
				init_delay_2[j]         = cnt_value & 0x1ff;
				TST_LOG( tc, (logline, "DQ[%02i] IFSTA register 0x%08x -> Initial delay 0x%03x \n", j, cnt_value, init_delay_2[j]));
			}
		}
		mem++;
    }
    TST_LOG( tc, (logline, "\n"));

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
			TST_LOG( tc, (logline, "Calibration of the SMEM1\n"));
		}
		else if((mem - 1) == 1){
			map_win.req.mode.space = MAP_SPACE_SHM2; // SHM #2
			TST_LOG( tc, (logline, "Calibration of the SMEM2\n"));
		}

		map_win.req.mode.flags = 0;
		retval = tsc_map_alloc(&map_win);
		if(retval < 0){
			TST_LOG( tc, (logline, "Error in mapping SHM"));
			return (-1);
		}

		buf_ddr = mmap(NULL, map_win.req.size, PROT_READ | PROT_WRITE, MAP_SHARED, tsc_fd, map_win.req.loc_addr);
		if(buf_ddr == MAP_FAILED){
			TST_LOG( tc, (logline, "Error MAP FAILED \n"));
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
		TST_LOG( tc, (logline, "   FPGA System Monitoring\n"));
		tsc_smon_read(0x00, &d0);
		f0 = (((double)(d0 >> 6) * 503.975) / 1024.) - (double)273.15;
		tsc_smon_read(0x20, &d1);
		f1 = (((double)(d1 >> 6) * 503.975) / 1024.) - 273.15;
		tsc_smon_read(0x24, &d2);
		f2 = (((double)(d2 >> 6) * 503.975) / 1024.) - 273.15;
		TST_LOG( tc, (logline, "      Temperature          : %.2f [%.2f - %.2f]\n", f0, f1, f2));
		tsc_smon_read(0x01, &d0);
		f0 = (((double)(d0 >> 6) * 3.0) / 1024.);
		tsc_smon_read(0x21, &d1);
		f1 = (((double)(d1 >> 6) * 3.0) / 1024.);
		tsc_smon_read(0x25, &d2);
		f2 = (((double)(d2 >> 6) * 3.0) / 1024.);
		TST_LOG( tc, (logline, "      VCCint               : %.2f [%.2f - %.2f]\n", f0, f1, f2));
		tsc_smon_read(0x02, &d0);
		f0 = (((double)(d0 >> 6) * 3.0) / 1024.);
		tsc_smon_read(0x22, &d1);
		f1 = (((double)(d1 >> 6) * 3.0) / 1024.);
		tsc_smon_read(0x26, &d2);
		f2 = (((double)(d2 >> 6) * 3.0) / 1024.);
		TST_LOG( tc, (logline, "      VCCaux               : %.2f [%.2f - %.2f]\n", f0, f1, f2));
		d0 = 3;
		tsc_smon_write(0x40, &d0);

		TST_LOG( tc, (logline, "\n"));
		TST_LOG( tc, (logline, "Calibration pattern   : 0101 1001 0011 0100 1011 0101 1001 0011 \n"));
		TST_LOG( tc, (logline, "Default INC           : %d \n", CURRENT_STEP));
		TST_LOG( tc, (logline, "Default CNT           : %02x \n", DEFAULT_DELAY));
		TST_LOG( tc, (logline, "\n"));

		// Reset calibration register
		data = 0;
		tsc_csr_write(SMEM_DDR3_IFSTA[mem - 1], &data);
		tsc_csr_write(SMEM_DDR3_IDEL[mem - 1], &data);

		// Pass the entire possible delay taps
		TST_LOG( tc, (logline, "+--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------+ \n"));
		TST_LOG( tc, (logline, " Default delay     :  "));
		for(j = DEFAULT_DELAY; j < MAX; j++){
			if(j == (CURRENT_DLY / CURRENT_STEP)){
				printf("*");
				break;
			}
			else{
				printf("   ");
			}
		}
		TST_LOG( tc, (logline, "\n"));
		TST_LOG( tc, (logline, " Delay value [MSB] : 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 01 01 01 01 01 01 01 01 01 01 01 01 01 01 01 01 01 01 01 01 01 01 01 01 01 01 01 01 01 01 01 01 01 01 01 01 01 01 01 01 01 01 01 01 01 01 01 01 01 01 01 01 01 01 01 01 01 01 01 01 01 01 01 01 \n"));
		TST_LOG( tc, (logline, " Delay value [LSB] : 40 44 48 4C 50 54 58 5C 60 64 68 6C 70 74 78 7C 80 84 88 8C 90 94 98 9C A0 A4 A8 AC B0 B4 B8 BC C0 C4 C8 CC D0 D4 D8 DC E0 E4 E8 EC F0 F4 F8 FC 00 04 08 0C 10 14 18 1C 20 24 28 2C 30 34 38 3F 40 44 48 4C 50 54 58 5C 60 64 68 6C 70 74 78 7C 80 84 88 8C 90 94 98 9C A0 A4 A8 AC B0 B4 B8 BC C0 C4 C8 CC D0 D4 D8 DC E0 E4 E8 EC F0 F4 F8 FC \n"));
		TST_LOG( tc, (logline, "+--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------+ \n"));

		// Set IDEL to 0
		data = 0;
		tsc_csr_write(SMEM_DDR3_IDEL[mem - 1], &data);

		// Loop on 16 DQ
		for(j = 0; j < 16; j++){

			// Store initial value of count of the IFSTA register
			dq_path = (j << 12);
			tsc_csr_write(SMEM_DDR3_IFSTA[mem - 1], &dq_path);
			tsc_csr_read(SMEM_DDR3_IDEL[mem - 1], &vtc_read); 				// Acquire current value of the register
			vtc_set = (vtc_read | (1 << 28));								// Set value to disable VTC
			tsc_csr_write(SMEM_DDR3_IDEL[mem - 1], &vtc_set); 				// Disable VTC
			tsc_csr_read(SMEM_DDR3_IFSTA[mem - 1], &cnt_value); 			// Read initial value of IFSTA register
			tsc_csr_write(SMEM_DDR3_IDEL[mem - 1], &vtc_read); 				// Re-active active VTC
			temp_cnt_value_store[j] = DEFAULT_DELAY;

			if(j < 8){
				TST_LOG( tc, (logline, " DQ[%02d] test >>>>> :", j + 8));
			}
			else{
				TST_LOG( tc, (logline, " DQ[%02d] test >>>>> :", j - 8));
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
					TST_LOG( tc, (logline, "  Y"));
					end   = k;
					NOK   = 0;
					ok++;
				}
				else{
					TST_LOG( tc, (logline, "  -"));
				}

				// Increment only the tap delay when we are < MAX tap
				if(k < (MAX - CURRENT_STEP)) {

if (ppc == 1) {
					if(j < 8){
						// Compute new count value and write IFSTA
						data = (temp_cnt_value_store[j] + CURRENT_STEP) << 16;
						tsc_csr_write(SMEM_DDR3_IFSTA[mem - 1], &data);

						// Load new count value
						data = (1 << 31) | (0x1 << (j + 8));
						tsc_csr_write(SMEM_DDR3_IDEL[mem - 1], &data);

						// Update new value of count
						temp_cnt_value_store[j] = temp_cnt_value_store[j] + CURRENT_STEP;
					}
					else {
						// Compute new count value and write IFSTA
						data = (temp_cnt_value_store[j] + CURRENT_STEP) << 16;
						tsc_csr_write(SMEM_DDR3_IFSTA[mem - 1], &data);

						// Load new count value
						data = (1 << 31) | (0x1 << (j - 8));
						tsc_csr_write(SMEM_DDR3_IDEL[mem - 1 ], &data);


						// Update new value of count
						temp_cnt_value_store[j] = temp_cnt_value_store[j] + CURRENT_STEP;
					}
}
else {
					// Compute new count value and write IFSTA
					data = (temp_cnt_value_store[j] + CURRENT_STEP) << 16;
					tsc_csr_write(SMEM_DDR3_IFSTA[mem - 1], &data);

					// Load new count value
					data = (1 << 31) | (0x1 << (j));
					tsc_csr_write(SMEM_DDR3_IDEL[mem - 1], &data);

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
				TST_LOG( tc, (logline, "\n"));
				// MEM1
				if(r == 0) {
					TST_LOG( tc, (logline, " Init  delay 0x%03x :\n", init_delay_1[j + 8]));
				}
				// MEM 2
				else if (r == 1){
					TST_LOG( tc, (logline, " Init  delay 0x%03x :\n", init_delay_2[j + 8]));
				}
				TST_LOG( tc, (logline, " Final delay 0x%03x :", marker));
				for(n = DEFAULT_DELAY ; n < marker; n = n + CURRENT_STEP){
					TST_LOG( tc, (logline, "   "));
				}
				TST_LOG( tc, (logline, "  *"));
				TST_LOG( tc, (logline, "\n"));
			}
			else{
				// Trace new delay
				TST_LOG( tc, (logline, "\n"));
				// MEM1
				if(r == 0) {
					TST_LOG( tc, (logline, " Init  delay 0x%03x :\n", init_delay_1[j - 8]));
				}
				// MEM 2
				else if (r == 1){
					TST_LOG( tc, (logline, " Init  delay 0x%03x :\n", init_delay_2[j - 8]));
				}
				TST_LOG( tc, (logline, " Final delay 0x%03x :", marker));
				for(n = DEFAULT_DELAY ; n < marker; n = n + CURRENT_STEP){
					printf("   ");
				}
				TST_LOG( tc, (logline, "  *"));
				TST_LOG( tc, (logline, "\n"));
			}

if (ppc == 1) {
			if(j < 8){
				// Compute new count value and write IFSTA
				data = final_cnt_value_store[j] << 16;
				tsc_csr_write(SMEM_DDR3_IFSTA[mem - 1], &data);

				// Load new count value
				data = (1 << 31) | (0x1 << (j + 8));
				tsc_csr_write(SMEM_DDR3_IDEL[mem - 1], &data);
			}
			else {
				// Compute new count value and write IFSTA
				data = final_cnt_value_store[j] << 16;
				tsc_csr_write(SMEM_DDR3_IFSTA[mem - 1], &data);

				// Load new count value
				data = (1 << 31) | (0x1 << (j - 8));
				tsc_csr_write(SMEM_DDR3_IDEL[mem - 1 ], &data);
			}
}
else {
			// Compute new count value and write IFSTA
			data = final_cnt_value_store[j] << 16;
			tsc_csr_write(SMEM_DDR3_IFSTA[mem - 1], &data);

			// Load new count value
			data = (1 << 31) | (0x1 << (j));
			tsc_csr_write(SMEM_DDR3_IDEL[mem - 1], &data);
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

			TST_LOG( tc, (logline, "+--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------+ \n"));

			// Set IDEL and IFSTA to 0
			data = 0;
			tsc_csr_write(SMEM_DDR3_IDEL[mem - 1], &data);
			tsc_csr_write(SMEM_DDR3_IFSTA[mem - 1], &data);

		}

		// Execution is finished OK or NOK
		TST_LOG( tc, (logline, "\n"));
		if (NOK == 1){
			TST_LOG( tc, (logline, "Calibration is not possible, error on line(s) : \n"));
			for (m = 0; m < 16; m++){
				if (DQ_NOK[m] == 1){
					TST_LOG( tc, (logline, "DQ[%i] \n", m));
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
		    TST_LOG( tc, (logline, "Best calibration window size is %i for DQ[%02i] \n", best, location));

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
		    TST_LOG( tc, (logline, "Worst calibration windows size is %i for DQ[%02i] \n", worst, location));
		}

		// Print initial and final

		TST_LOG( tc, (logline, "\n"));
		if (r == 0){ // MEM1
			for (m = 0 ;m < 16 ;m++) {
				if(m < 8){
					TST_LOG( tc, (logline, "DQ[%02d] - Initial delay 0x%03x - Final delay 0x%03x \n", m, init_delay_1[m], final_cnt_value_store[m + 8]));
				}
				else{
					TST_LOG( tc, (logline, "DQ[%02d] - Initial delay 0x%03x - Final delay 0x%03x \n", m, init_delay_1[m], final_cnt_value_store[m - 8]));
				}
			}
		}
		else if (r == 1){ // MEM2
			for (m = 0 ;m < 16 ;m++) {
				if(m < 8){
					TST_LOG( tc, (logline, "DQ[%02d] - Initial delay 0x%03x - Final delay 0x%03x \n", m, init_delay_2[m], final_cnt_value_store[m + 8]));
				}
				else{
					TST_LOG( tc, (logline, "DQ[%02d] - Initial delay 0x%03x - Final delay 0x%03x \n", m, init_delay_2[m], final_cnt_value_store[m - 8]));
				}
			}
		}

		TST_LOG( tc, (logline, "\n"));
		TST_LOG( tc, (logline, "Calibration finished ! \n"));
		TST_LOG( tc, (logline, "\n"));

		// Set IDEL and IFSTA to 0
		data = 0;
		tsc_csr_write(SMEM_DDR3_IDEL[mem - 1], &data);
		tsc_csr_write(SMEM_DDR3_IFSTA[mem - 1], &data);

		// Unmap DDR3 memory
		munmap(buf_ddr, map_win.req.size);
		tsc_map_free(&map_win);

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

    	TST_LOG( tc, (logline, "Final value for MEM%x : \n", mem));

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
   				TST_LOG( tc, (logline, "DQ[%02i] Initial delay 0x%03x - IFSTA register 0x%08x -> Final delay 0x%03x \n", j, init_delay_1[j], cnt_value, cnt_value & 0x1ff));
   			}
   			// MEM2
   			else if (r == 1){
   				TST_LOG( tc, (logline, "DQ[%02i] Initial delay 0x%03x - IFSTA register 0x%08x -> Final delay 0x%03x \n", j, init_delay_2[j], cnt_value, cnt_value & 0x1ff));
   			}
   		}

    	// Set IDEL and IFSTA to 0
    	data = 0;
    	tsc_csr_write(SMEM_DDR3_IDEL[mem - 1], &data);
    	tsc_csr_write(SMEM_DDR3_IFSTA[mem - 1], &data);

   		mem++;

    }
    TST_LOG( tc, (logline, "\n"));

	tm = time(0);
	ct = ctime(&tm);
	TST_LOG( tc, (logline, "\n%s->Exiting :%s", tst_id, ct));

	return(retval | TST_STS_DONE);
}

int tst_02(struct tst_ctl *tc){
	return(tst_shm_calibration(tc, "Tst:02"));
}

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * Function name : tst_shm_addr
 * Prototype     : int
 * Parameters    : test control structure, test id
 * Return        : error/success
 *----------------------------------------------------------------------------
 * Description   : test all shm address bits : read write shm
 *
 *++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

int tst_shm_addr(struct tst_ctl *tc, char *tst_id){
	time_t tm;
	char *ct           = NULL;
	char *eaddr        = NULL;
	char *data_buf     = NULL; // Consistent data
	char *check_buf    = NULL; // Resulting data for check
	int i              = 0;
	int j              = 0;
	int retval         = 0;
	int size_ref       = 0x4;
	int shm_addr       = 0x4000;
	int shm_addr_ref   = 0x4000;
	int ref_pattern    = 0xdeadface;

	tm = time(0);
	ct = ctime(&tm);

	TST_LOG( tc, (logline, "%s->Entering:%s\n", tst_id, ct));
	TST_LOG( tc, (logline, "%s->Executing Read / Write SHM and test bit address \n", tst_id));

	// Prepare buffers
	data_buf  = (char *)malloc(size_ref);
	check_buf = (char *)malloc(size_ref);

	// Fill 1 data  with test pattern
	tst_cpu_fill(data_buf, size_ref, 0, 0xdeadface, 0);

	// Loop on 2 DDR3 memory
	for(j = 1; j < 3; j++){

		shm_addr = shm_addr_ref;

		// Generate test with sliding SHM address bit for low address bit
		for(i = 0; i < 13; i++){

			// Fill check buffer with 0
			tst_cpu_fill(check_buf, size_ref, 0, 0, 0);

			TST_LOG(tc, (logline, "\nTesting SHM#%x address low  bit: %08x ", j, shm_addr));

			// Write local SHM with 0
			tsc_shm_write(shm_addr, check_buf, size_ref, 4, 0, j);

			usleep(1000);

			// Write local SHM with consistent data
			tsc_shm_write(shm_addr, data_buf, size_ref, 4, 0, j);

			usleep(1000);

			// Read local SHM with to check data
			tsc_shm_read(shm_addr, check_buf, size_ref, 4, 0, j);

			usleep(1000);

			// Check for errors in consistent pattern area
			eaddr = tst_cpu_check(check_buf, size_ref, 0, ref_pattern, 0);
			if(eaddr){
				TST_LOG( tc, (logline, "->Error in consistent pattern area at address %x", shm_addr));
				retval = TST_STS_ERR;
				break;
			}

			TST_LOG( tc, (logline, "              -> OK\r"));

			// Change SHM bit address
			shm_addr = shm_addr_ref | (1 << i);
		}

		shm_addr = shm_addr_ref;

		// Generate test with sliding SHM address bit
		for(i = 0; i < 15; i++){

			// Fill check buffer with 0
			tst_cpu_fill(check_buf, size_ref, 0, 0, 0);

			TST_LOG(tc, (logline, "\nTesting SHM#%x address high bit: %08x ", j, shm_addr));

			// Write local SHM with 0
			tsc_shm_write(shm_addr, check_buf, size_ref, 4, 0, j);

			usleep(1000);

			// Write local SHM with consistent data
			tsc_shm_write(shm_addr, data_buf, size_ref, 4, 0, j);

			usleep(1000);

			// Read local SHM with to check data
			tsc_shm_read(shm_addr, check_buf, size_ref, 4, 0, j);

			usleep(1000);

			// Check for errors in consistent pattern area
			eaddr = tst_cpu_check(check_buf, size_ref, 0, ref_pattern, 0);
			if(eaddr){
				TST_LOG( tc, (logline, "->Error in consistent pattern area at address %x", shm_addr));
				retval = TST_STS_ERR;
				break;
			}

			TST_LOG( tc, (logline, "              -> OK\r"));

			// Change SHM bit address
			shm_addr = shm_addr << 1;
		}
	}

	tm = time(0);
	ct = ctime(&tm);
	TST_LOG( tc, (logline, "\n%s->Exiting :%s", tst_id, ct));

	free(data_buf);
	free(check_buf);

	return( retval | TST_STS_DONE);
}

int tst_03(struct tst_ctl *tc){
	return(tst_shm_addr(tc, "Tst:03"));
}

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * Function name : tst_sram_shm_mem_pmem
 * Prototype     : int
 * Parameters    : test control structure, test id, mode
 * Return        : error/success
 *----------------------------------------------------------------------------
 * Description   : read write sram1, sram2, shm1, shm2 from cpu with mem | pmem
 *
 *++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

int tst_sram_shm_mem_pmem(struct tst_ctl *tc, char *tst_id){
	struct tsc_ioctl_map_win map_loc_win;
	time_t tm;
	char *ct            = NULL;
	char * uaddr        = NULL;
	char * eaddr        = NULL;
	int fd              = 0;
	int retval          = 0;
	int size_ref        = 0;
	int offset          = 0;
	int sub_offset      = 0x1000; // Sliding sub_offset
	int sub_offset_ref  = 0x1000;
	int size            = tc->at->shm_size_0;
	int sub_size        = tc->at->shm_size_0 - 0x2000; // Sliding size
	int sub_size_ref    = tc->at->shm_size_0 - 0x2000;
	int ref_pattern     = 0xdeadface;
	int win_size        = 0x10000;
	int i               = 0;
	int j               = 0;
	int k               = 0;

	tm = time(0);
	ct = ctime(&tm);

	TST_LOG( tc, (logline, "%s->Entering:%s\n", tst_id, ct));

	// Loop on SRAM and DDR
	for (k = 0; k < 2; k++){
		if (k == 0){
			TST_LOG( tc, (logline, "\n"));
			TST_LOG( tc, (logline, "%s->Executing Read / Write SRAM memory: \n", tst_id));
		}
		else if (k == 1){
			TST_LOG( tc, (logline, "\n"));
			TST_LOG( tc, (logline, "%s->Executing Read / Write DDR3 memory: \n", tst_id));
		}
		// Loop on device 1 and device 2 with MEM and PMEM mapping
		for (j = 0; j < 4; j++){
			if(j == 0){
				TST_LOG( tc, (logline, "#1 MEM  \n"));
			}
			else if (j == 1){
				TST_LOG( tc, (logline, "#1 PMEM  \n"));
			}
			else if(j == 2){
				TST_LOG( tc, (logline, "#2 MEM  \n"));
			}
			else if(j == 3){
				TST_LOG( tc, (logline, "#2 PMEM  \n"));
			}

			fd = tc->fd;

			size_ref = tc->at->shm_size_0;

			// SRAM
			if (k == 0){
				offset   = tc->at->sram_offset_0;
			}
			// DDR
			else if (k == 1){
				offset   = tc->at->shm_offset_0;
			}

			// Map local SHH
			memset(&map_loc_win, sizeof(map_loc_win), 0);

			map_loc_win.req.rem_addr   = offset;
			map_loc_win.req.loc_addr   = 0;
			map_loc_win.req.size       = win_size;
			if(j == 0){
				map_loc_win.req.mode.sg_id = MAP_ID_MAS_PCIE_MEM;
				map_loc_win.req.mode.space = MAP_SPACE_SHM1;
			}
			else if(j == 1){
				map_loc_win.req.mode.sg_id = MAP_ID_MAS_PCIE_PMEM;
				map_loc_win.req.mode.space = MAP_SPACE_SHM1;
			}
			else if(j == 2){
				map_loc_win.req.mode.sg_id = MAP_ID_MAS_PCIE_MEM;
				map_loc_win.req.mode.space = MAP_SPACE_SHM2;
			}
			else if (j == 3){
				map_loc_win.req.mode.sg_id = MAP_ID_MAS_PCIE_PMEM;
				map_loc_win.req.mode.space = MAP_SPACE_SHM2;
			}
			map_loc_win.req.mode.flags = 0;

			retval = tsc_map_alloc(&map_loc_win);
			if(retval < 0){
				TST_LOG( tc, (logline, "->Error in mapping memory \n"));
				retval = TST_STS_ERR;
			}

			uaddr = mmap(NULL, map_loc_win.req.size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, map_loc_win.req.loc_addr);
			if(uaddr == MAP_FAILED){
				TST_LOG( tc, (logline, "->Error MAP FAILED \n"));

				retval = munmap(uaddr, size);
				if(retval < 0){
					TST_LOG( tc, (logline, "Error tsc unmap uaddr ! \n"));
				}

				retval = tsc_map_free(&map_loc_win);
				if(retval < 0){
					TST_LOG( tc, (logline, "Error tsc map local free! \n"));
				}

				tm = time(0);
				ct = ctime(&tm);
				TST_LOG( tc, (logline, "\n%s->Exiting :%s", tst_id, ct));

				retval = TST_STS_ERR;

				return(retval | TST_STS_DONE);
			}

			// Generate test with sliding size and offset
			for(i = 0; i < 0x1000; i++){
				TST_LOG(tc, (logline, "%s->Executing: iteration:%4d size:%05x offset:%05x", tst_id, i++, sub_size, sub_offset));

				// Initialize whole memory area with reference pattern 0xdeadface
				// Write into local SHM0
				tst_cpu_fill(uaddr, size_ref, 0, ref_pattern, 0);

				// Check for errors
				eaddr = tst_cpu_check(uaddr, size_ref, 0, ref_pattern, 0);
				if(eaddr){
					TST_LOG( tc, (logline, "->Error reference pattern at offset %x", (uint)(eaddr - uaddr) + offset));
					retval = TST_STS_ERR;
					break;
				}

				// Initialize consistent sub memory area with consistent data
				// Write into SHM0
				tst_cpu_fill(uaddr + sub_offset, sub_size, 1, offset + sub_offset, 4);

				// Check for errors before consistent pattern
				eaddr = tst_cpu_check(uaddr, sub_offset, 0, 0xdeadface, 0);
				if(eaddr){
					TST_LOG( tc, (logline, "->Error before consistent pattern at offset %x", (uint)(eaddr - uaddr) + offset));
					retval = TST_STS_ERR;
					break;
				}

				// Check for errors in consistent pattern area
				eaddr = tst_cpu_check(uaddr + sub_offset, sub_size, 1, offset + sub_offset, 4);
				if(eaddr){
					TST_LOG( tc, (logline, "->Error in consistent pattern area at offset %x", (uint)(eaddr - uaddr) + offset));
					retval = TST_STS_ERR;
					break;
				}

				// Check for errors after consistent pattern area
				eaddr = tst_cpu_check(uaddr + sub_offset + sub_size, size_ref - sub_size - sub_offset, 0, 0xdeadface, 0);
				if(eaddr){
					TST_LOG( tc, (logline, "->Error after consistent pattern at offset %x", (uint)(eaddr - uaddr) + offset));
					retval = TST_STS_ERR;
					break;
				}

				TST_LOG( tc, (logline, "                -> OK\r"));


				// Slide size
				sub_size   = sub_size_ref + ((i & 0xf00) >> 5);
				sub_offset = sub_offset_ref + ((i & 0xf0)  >> 1);

			    if(tc->exec_mode & TST_EXEC_FAST){
			    	if(i > 0x80){
			    		break;
			    	}
			    }
			}
			TST_LOG( tc, (logline, "\n"));

			retval = munmap(uaddr, size);
			if(retval < 0){
				TST_LOG( tc, (logline, "Error tsc unmap uaddr ! \n"));
			}

			retval = tsc_map_free(&map_loc_win);
			if(retval < 0){
				TST_LOG( tc, (logline, "Error tsc map local free! \n"));
			}
		}
	}
	tm = time(0);
	ct = ctime(&tm);
	TST_LOG( tc, (logline, "\n%s->Exiting :%s", tst_id, ct));

	return(retval | TST_STS_DONE);
}

int tst_04(struct tst_ctl *tc){
	return(tst_sram_shm_mem_pmem(tc, "Tst:04"));
}

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * Function name : tst_usr
 * Prototype     : int
 * Parameters    : test control structure, test id, mode
 * Return        : error/success
 *----------------------------------------------------------------------------
 * Description   : read write usr1 and usr2 over mem and pmem
 *
 *++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

int tst_usr(struct tst_ctl *tc, char *tst_id){
	struct tsc_ioctl_map_win map_loc_win;
	time_t tm;
	char *ct            = NULL;
	char * uaddr        = NULL;
	char * eaddr        = NULL;
	int fd              = 0;
	int retval          = 0;
	int size_ref        = 0;
	int offset          = 0;
	int sub_offset      = 0x1000; // Sliding sub_offset
	int sub_offset_ref  = 0x1000;
	int size            = tc->at->usr_size_0;
	int sub_size        = tc->at->usr_size_0 - 0x2000; // Sliding size
	int sub_size_ref    = tc->at->usr_size_0 - 0x2000;
	int ref_pattern     = 0xdeadface;
	int win_size        = 0x10000;
	int i               = 0;
	int j               = 0;

	tm = time(0);
	ct = ctime(&tm);

	TST_LOG( tc, (logline, "%s->Entering:%s\n", tst_id, ct));
	TST_LOG( tc, (logline, "%s->Executing Read / Write USR memory: \n", tst_id));

	// Loop on device 1 and device 2 with MEM and PMEM mapping
	for (j = 0; j < 4; j++){
		if(j == 0){
			TST_LOG( tc, (logline, "#1 MEM  \n"));
		}
		else if (j == 1){
			TST_LOG( tc, (logline, "#1 PMEM  \n"));
		}
		else if(j == 2){
			TST_LOG( tc, (logline, "#2 MEM  \n"));
		}
		else if(j == 3){
			TST_LOG( tc, (logline, "#2 PMEM  \n"));
		}

		fd = tc->fd;

		size_ref = tc->at->usr_size_0;
		offset   = tc->at->usr_offset_0;

		// Map local USR
		memset(&map_loc_win, sizeof(map_loc_win), 0);

		map_loc_win.req.rem_addr   = offset;
		map_loc_win.req.loc_addr   = 0;
		map_loc_win.req.size       = win_size;
		if(j == 0){
			map_loc_win.req.mode.sg_id = MAP_ID_MAS_PCIE_MEM;
			map_loc_win.req.mode.space = MAP_SPACE_USR1;
		}
		else if(j == 1){
			map_loc_win.req.mode.sg_id = MAP_ID_MAS_PCIE_PMEM;
			map_loc_win.req.mode.space = MAP_SPACE_USR1;
		}
		else if(j == 2){
			map_loc_win.req.mode.sg_id = MAP_ID_MAS_PCIE_MEM;
			map_loc_win.req.mode.space = MAP_SPACE_USR2;
		}
		else if (j == 3){
			map_loc_win.req.mode.sg_id = MAP_ID_MAS_PCIE_PMEM;
			map_loc_win.req.mode.space = MAP_SPACE_USR2;
		}
		map_loc_win.req.mode.flags = 0;

		retval = tsc_map_alloc(&map_loc_win);
		if(retval < 0){
			TST_LOG( tc, (logline, "->Error in mapping memory \n"));
			retval = TST_STS_ERR;
		}

		uaddr = mmap(NULL, map_loc_win.req.size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, map_loc_win.req.loc_addr);
		if(uaddr == MAP_FAILED){
			TST_LOG( tc, (logline, "->Error MAP FAILED \n"));

			retval = munmap(uaddr, size);
			if(retval < 0){
				TST_LOG( tc, (logline, "Error tsc unmap uaddr ! \n"));
			}

			retval = tsc_map_free(&map_loc_win);
			if(retval < 0){
				TST_LOG( tc, (logline, "Error tsc map local free! \n"));
			}

			tm = time(0);
			ct = ctime(&tm);
			TST_LOG( tc, (logline, "\n%s->Exiting :%s", tst_id, ct));

			retval = TST_STS_ERR;

			return(retval | TST_STS_DONE);
		}

		// Generate test with sliding size and offset
		for(i = 0; i < 0x1000; i++){
			TST_LOG(tc, (logline, "%s->Executing: iteration:%4d size:%05x offset:%05x", tst_id, i++, sub_size, sub_offset));

			// Initialize whole memory area with reference pattern 0xdeadface
			// Write into local USR
			tst_cpu_fill(uaddr, size_ref, 0, ref_pattern, 0);

			// Check for errors
			eaddr = tst_cpu_check(uaddr, size_ref, 0, ref_pattern, 0);
			if(eaddr){
				TST_LOG( tc, (logline, "->Error reference pattern at offset %x", (uint)(eaddr - uaddr) + offset));
				retval = TST_STS_ERR;
				break;
			}

			// Initialize consistent sub memory area with consistent data
			// Write into USR
			tst_cpu_fill(uaddr + sub_offset, sub_size, 1, offset + sub_offset, 4);

			// Check for errors before consistent pattern
			eaddr = tst_cpu_check(uaddr, sub_offset, 0, 0xdeadface, 0);
			if(eaddr){
				TST_LOG( tc, (logline, "->Error before consistent pattern at offset %x", (uint)(eaddr - uaddr) + offset));
				retval = TST_STS_ERR;
				break;
			}

			// Check for errors in consistent pattern area
			eaddr = tst_cpu_check(uaddr + sub_offset, sub_size, 1, offset + sub_offset, 4);
			if(eaddr){
				TST_LOG( tc, (logline, "->Error in consistent pattern area at offset %x", (uint)(eaddr - uaddr) + offset));
				retval = TST_STS_ERR;
				break;
			}

			// Check for errors after consistent pattern area
			eaddr = tst_cpu_check(uaddr + sub_offset + sub_size, size_ref - sub_size - sub_offset, 0, 0xdeadface, 0);
			if(eaddr){
				TST_LOG( tc, (logline, "->Error after consistent pattern at offset %x", (uint)(eaddr - uaddr) + offset));
				retval = TST_STS_ERR;
				break;
			}

			TST_LOG( tc, (logline, "                -> OK\r"));


			// Slide size
			sub_size   = sub_size_ref + ((i & 0xf00) >> 5);
			sub_offset = sub_offset_ref + ((i & 0xf0)  >> 1);

		    if(tc->exec_mode & TST_EXEC_FAST){
		    	if(i > 0x80){
		    		break;
		    	}
		    }
		}
		TST_LOG( tc, (logline, "\n"));

		retval = munmap(uaddr, size);
		if(retval < 0){
			TST_LOG( tc, (logline, "Error tsc unmap uaddr ! \n"));
		}

		retval = tsc_map_free(&map_loc_win);
		if(retval < 0){
			TST_LOG( tc, (logline, "Error tsc map local free! \n"));
		}
	}
	tm = time(0);
	ct = ctime(&tm);
	TST_LOG( tc, (logline, "\n%s->Exiting :%s", tst_id, ct));

	return(retval | TST_STS_DONE);
}

int tst_05(struct tst_ctl *tc){
	return(tst_usr(tc, "Tst:05"));
}

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * Function name : tst_kbuf
 * Prototype     : int
 * Parameters    : test control structure, test id
 * Return        : error/success
 *
 *----------------------------------------------------------------------------
 * Description   : read write kbuf0 from cpu
 *
 *++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

int tst_kbuf(struct tst_ctl *tc, char *tst_id){
	struct tsc_ioctl_kbuf_req buf_p;
	time_t tm;
	char *ct           = NULL;
	char *eaddr        = NULL;
	char *ref_buf      = NULL; 						  // 0xdeadface
	char *data_buf     = NULL; 					      // Consistent data
	char *check_buf    = NULL; 						  // Resulting data for check
	int i              = 0;
	int retval         = 0;
	int size_ref       = 0;
	int offset  	   = 0;
	int sub_offset     = 0x1000; 					  // Sliding sub_offset
	int sub_offset_ref = 0x1000;
	int sub_size       = tc->at->shm_size_0 - 0x2000; // Sliding size
	int sub_size_ref   = tc->at->shm_size_0 - 0x2000;
	int ref_pattern    = 0xdeadface;

	tm = time(0);
	ct = ctime(&tm);

	TST_LOG( tc, (logline, "%s->Entering:%s\n", tst_id, ct));
	TST_LOG( tc, (logline, "%s->Executing Read / Write KBUF0\n", tst_id));

	// Compute board offset in decoding window
	size_ref = tc->at->shm_size_0;
	offset   = tc->at->shm_offset_0;

	// Allocate kernel buffer suitable for transfer
	buf_p.size = 0x10000;
	retval = tsc_kbuf_alloc(&buf_p);
	if(retval != 0){
	    TST_LOG( tc, (logline, "->Error allocationt kernel buffer\n"));

		tm = time(0);
		ct = ctime(&tm);
		TST_LOG( tc, (logline, "\n%s->Exiting :%s", tst_id, ct));

		free(ref_buf);
		free(data_buf);
		free(check_buf);

		tsc_kbuf_free(&buf_p);

		retval = TST_STS_ERR;

		return( retval | TST_STS_DONE);
	}

	// Prepare buffers
	ref_buf   = (char *)malloc(size_ref);
	data_buf  = (char *)malloc(size_ref);
	check_buf = (char *)malloc(size_ref);

	// Fill reference buffer with pattern 0xdeadface
	tst_cpu_fill(ref_buf, size_ref, 0, ref_pattern, 0);

	// Fill data buffer with pattern 0 4 8 C ...
	tst_cpu_fill(data_buf, size_ref, 1, offset, 4);

	// Fill check buffer with 0
	tst_cpu_fill(check_buf, size_ref, 0, 0, 0);

	// Generate test with sliding size and offset
	for(i = 0; i < 0x1000; i++){
		TST_LOG(tc, (logline, "%s->Executing: iteration:%4d size:%05x offset:%05x", tst_id, i++, sub_size, sub_offset));

		// Fill check buffer with 0
		tst_cpu_fill(check_buf, size_ref, 0, 0, 0);

		// Write local KBUF0 with reference pattern
		tsc_kbuf_write(buf_p.k_base , ref_buf, size_ref);

		// Write local KBUF0 with consistent data
		tsc_kbuf_write(buf_p.k_base + sub_offset, data_buf + sub_offset, sub_size);

		// Read local KBUF0 with to check data
		tsc_kbuf_read(buf_p.k_base, check_buf, size_ref);

		// Check for errors before consistent pattern
		eaddr = tst_cpu_check(check_buf, sub_offset, 0, 0xdeadface, 0);
		if(eaddr){
			TST_LOG( tc, (logline, "->Error before consistent pattern at offset %x", (uint)(eaddr - check_buf)));
			retval = TST_STS_ERR;
			break;
		}

		// Check for errors in consistent pattern area
		eaddr = tst_cpu_check(check_buf + sub_offset, sub_size, 1, offset + sub_offset, 4);
		if(eaddr){
			TST_LOG( tc, (logline, "->Error in consistent pattern area at offset %x", (uint)(eaddr - check_buf)));
			retval = TST_STS_ERR;
			break;
		}

		// Check for errors after consistent pattern area
		eaddr = tst_cpu_check(check_buf + sub_offset + sub_size, size_ref - sub_size - sub_offset, 0, 0xdeadface, 0);
		if(eaddr){
			TST_LOG( tc, (logline, "->Error after consistent pattern at offset %x", (uint)(eaddr - check_buf)));
			retval = TST_STS_ERR;
			break;
		}

		TST_LOG( tc, (logline, "                -> OK\r"));

		// Slide size
	    sub_size   = sub_size_ref + ((i & 0xf00) >> 5);
	    sub_offset = sub_offset_ref + ((i & 0xf0)  >> 1);

	    if(tc->exec_mode & TST_EXEC_FAST){
	       	if(i > 0x80){
	       		break;
	       	}
	    }
	}

	tm = time(0);
	ct = ctime(&tm);
	TST_LOG( tc, (logline, "\n%s->Exiting :%s", tst_id, ct));

	free(ref_buf);
	free(data_buf);
	free(check_buf);

	tsc_kbuf_free(&buf_p);

	return( retval | TST_STS_DONE);
}

int tst_06(struct tst_ctl *tc){
	return(tst_kbuf(tc, "Tst:06"));
}
