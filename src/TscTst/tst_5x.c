/*=========================< begin file & file header >=======================
 *  References
 *
 *    filename : tst_5x.c
 *    author   : XP
 *    company  : IOxOS
 *    creation : May 12,2016
 *    version  : 0.0.1
 *
 *----------------------------------------------------------------------------
 *  Description
 *
 *	  Various tests
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
 *=============================< end file header >============================*/

#include <debug.h>
#include <sys/types.h>
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

// FMC register base
#define FMC1 0x1200
#define FMC2 0x1300

// FMC#1 base register : 0x1200 - 0x12FF
// FMC#2 base register : 0x1300 - 0x13FF

// Add offset
	// +0x00 : SIGNATURE (0x31FF0101 = FMC#1 - 0x31FF0201 = FMC#2)

	// OUT
	// +0x04 : FMC_LA[31:0]
	// +0x08 : FMC_HB[5:0] - FMC_HA[23:0] - FMC_LA[33:32]
	// +0x0C : FMC_HB[21:6]

	// IN
	// +0x10 : FMC_LA[31:0]
	// +0x14 : FMC_HB[5:0] - FMC_HA[23:0] - FMC_LA[33:32]
	// +0x18 : FMC_HB[21:6]

	// DIRECTON: 0 = IN, 1 = OUT
	// +0x1C : FMC_LA[31:0]
	// +0x20 : FMC_HB[5:0] - FMC_HA[23:0] - FMC_LA[33:32]
	// +0x24 : FMC_HB[21:6]
       	   	   // [27] FMC_NUMBER
       	   	   // [25] FMC_PG_M2C
       	   	   // [24] FMC_PRESENT_N
/*
LA01 <-> LA06	HA00 <-> HA01	HB00 <-> HB01
LA02 <-> LA00	HA02 <-> HA03	HB02 <-> HB03
LA04 <-> LA03	HA04 <-> HA05	HB04 <-> HB05
LA05 <-> LA10	HA06 <-> HA07	HB06 <-> HB07
LA07 <-> LA08	HA08 <-> HA09	HB08 <-> HB09
LA09 <-> LA13	HA10 <-> HA11	HB10 <-> HB11
LA11 <-> LA12	HA12 <-> HA13	HB12 <-> HB13
LA15 <-> LA16	HA15 <-> HA16	HB14 <-> HB15
LA17 <-> LA14	HA17 <-> HA14	HB16 <-> HB19
LA19 <-> LA20	HA19 <-> HA20	HB17 <-> HB18
LA21 <-> LA22	HA21 <-> HA18	HB20 <-> HB21
LA23 <-> LA18	HA23 <-> HA22
LA24 <-> LA25
LA26 <-> LA27
LA28 <-> LA29
LA30 <-> LA31
LA32 <-> LA33

          0,  1,  2,  3,  4,  5,  6,  7,  8,  9, 10, 11, 12, 13, 14, 15, 16,
         17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 33
FMC_LA=[  2,  6,  0,  4,  3, 10,  1,  8,  7, 13,  5, 12, 11,  9, 17, 16, 15,
         14, 23, 20, 19, 22, 21, 18, 25, 24, 27, 26, 29, 28, 31, 30, 33, 32 ]

          0,  1,  2,  3,  4,  5,  6,  7,  8,  9, 10, 11,
         12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23
FMC_HA=[  1,  0,  3,  2,  5,  4,  7,  6,  9,  8, 11, 10,
         13, 12, 17, 16, 15, 14, 21, 20, 19, 18, 23, 22 ]

          0,  1,  2,  3,  4,  5,  6,  7,  8,  9, 10,
         11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21
FMC_HB=[  1,  0,  3,  2,  5,  4,  7,  6,  9,  8, 11,
         10, 13, 12, 15, 14, 19, 18, 17, 16, 21, 20 ]
*/


/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * Function name : tst_fmc
 * Prototype     : int
 * Parameters    : test id and control structure
 * Return        : Done or Error
 *
 *----------------------------------------------------------------------------
 * Description   : test FMC#1 and FMC#2 on IFC14xx boards
 *
 *++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
int tst_fmc(struct tst_ctl *tc, char *tst_id){
	time_t tm;
	char *ct     = NULL;
	int data     = 0;
	int retval   = 0;
	int i        = 0;
	int j        = 0;
	int present1 = 0;
	int present2 = 0;
	int start    = 0;
	int end      = 0;

    // Index          0, 1, 2, 3, 4,  5, 6, 7, 8,  9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 33
	int fmc_la[34] = {2, 6, 0, 4, 3, 10, 1, 8, 7, 13,  5, 12, 11,  9, 17, 16, 15, 14, 23, 20, 19, 22, 21, 18, 25, 24, 27, 26, 29, 28, 31, 30, 33, 32};

	// Index          0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23
	int fmc_ha[24] = {1, 0, 3, 2, 5, 4, 7, 6, 9, 8, 11, 10, 13, 12, 17, 16, 15, 14, 21, 20, 19, 18, 23, 22};

	// Index          0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21
	int fmc_hb[22] = {1, 0, 3, 2, 5, 4, 7, 6, 9, 8, 11, 10, 13, 12, 15, 14, 19, 18, 17, 16, 21, 20};

	// FMC register
	int fmc[2]     = {FMC1, FMC2};

	tm = time(0);
	ct = ctime(&tm);

	TST_LOG( tc, (logline, "%s->Entering:%s\n", tst_id, ct));
	TST_LOG( tc, (logline, "%s->Executing test FMC#1 & FMC#2 on IFC14xx boards\n", tst_id));

	// Enable FMC in PON
	data = 0xc0000000;
	tsc_pon_read(0xc, &data);

	// Check if the board is a IFC1410 -------------------------------------------------------
	tsc_pon_read(0x0, &data);
	if (data != 0x73571410) {
		TST_LOG( tc, (logline, "-> The board is not a IFC14xx abort test ! \n"));
		retval = TST_STS_ERR;
		tm = time(0);
		ct = ctime(&tm);
		TST_LOG( tc, (logline, "\n%s->Exiting :%s", tst_id, ct));

		return( retval | TST_STS_DONE);
	}

	// Check if the FMC#1 is present ---------------------------------------------------------
	tsc_csr_read(FMC1 + 0x24 , &data);
	if (((data & (1 << 24)) == 0)) {
		TST_LOG( tc, (logline, "-> FMC#1 present number : %x \n", (1 + ((data & (1 << 27)) >> 27))));
		TST_LOG( tc, (logline, "-> FMC#1 FMC_PG_M2C     : %x \n", (data & (1 << 25)) >> 25));
		present1 = 1;
	}
	else { TST_LOG( tc, (logline, "-> FMC#1                : not present \n")); }

	// Check if the FMC#2 is present ---------------------------------------------------------
	tsc_csr_read(FMC2 + 0x24 , &data);
	if (((data & (1 << 24)) == 0)) {
		TST_LOG( tc, (logline, "-> FMC#2 present number : %x \n", (1 + ((data & (1 << 27)) >> 27))));
		TST_LOG( tc, (logline, "-> FMC#2 FMC_PG_M2C     : %x \n", (data & (1 << 25))>> 25));
		present2 = 1;
	}
	else { TST_LOG( tc, (logline, "-> FMC#2                : not present \n")); }

	// Check if at least one FMC is mounted on the board to continue the test ----------------
	if ((present1 == 0) & (present2 == 0)){
		TST_LOG( tc, (logline, "-> No FMC connected to the board abort test ! \n"));
		retval = TST_STS_ERR;
		tm = time(0);
		ct = ctime(&tm);
		TST_LOG( tc, (logline, "\n%s->Exiting :%s", tst_id, ct));

		return( retval | TST_STS_DONE);
	}

	// Check FPGA FMC signatures ------------------------------------------------------------
	tsc_csr_read(FMC1, &data);
	TST_LOG( tc, (logline, "-> FPGA FMC#1 signature : 0x%08x\n", data));
	tsc_csr_read(FMC2, &data);
	TST_LOG( tc, (logline, "-> FPGA FMC#2 signature : 0x%08x\n", data));

	// Check which one FMC is connected on the board, or both FMC
	if ((present1 == 1) & (present2 == 1)) {
		start = 0;
		end   = 2;
	}
	else if (present1) {
		start = 0;
		end   = 1;
	}
	else if (present2){
		start = 1;
		end   = 2;
	}

	// Loop on FMC number
	for (j = start; j < end; j++) {
		// Current FMC tested
		if (j == 0){
			TST_LOG( tc, (logline, "\n"));
			TST_LOG( tc, (logline, "********** TESTING FMC#1 ********** \n"));
			TST_LOG( tc, (logline, "\n"));
		}
		else if (j == 1){
			TST_LOG( tc, (logline, "\n"));
			TST_LOG( tc, (logline, "********** TESTING FMC#2 ********** \n"));
			TST_LOG( tc, (logline, "\n"));
		}

		TST_LOG( tc, (logline, "\n"));
		TST_LOG( tc, (logline, "---------- LA ----------\n"));
		TST_LOG( tc, (logline, "\n"));

		// FMC test LA[31:0] --------------------------------------------------------------------
		for (i = 0; i < 32; i++){

			// ---------- Drive '1'

			// Set all bit in IN direction
			data = 0;
			tsc_csr_write(fmc[j] + 0x1c, &data);

			usleep(100);

			// Set bit per bit in OUT direction
			data = 1 << i;
			tsc_csr_write(fmc[j] + 0x1c , &data);

			usleep(100);

			// Write '1' for OUT value
			data = 1 << i;
			tsc_csr_write(fmc[j] + 0x04 , &data);

			usleep(100);

			// Check that IN value is well '1'
			tsc_csr_read(fmc[j] + 0x10, &data);
			if ((data & (1 << i)) != (1 << i)) {
				TST_LOG( tc, (logline, "-> Error in local LA input value : expected 0x%08x read 0x%08x \n", (1 << i), (data & (1 << i))));
				retval = TST_STS_ERR;
			}
			else { TST_LOG( tc, (logline, "-> LA[%02i] <-> LA[%02i] ", i, fmc_la[i])); }

			// Check on other side that value is '1' on IN

			// Check corresponding value
			tsc_csr_read(fmc[j] + 0x10, &data);

			if ((1 << fmc_la[i]) != (data & (1 << fmc_la[i]))){
				TST_LOG( tc, (logline, "-> Error in remote LA input value : expected 0x%08x read 0x%08x \n", (1 << fmc_la[i]), (data & (1 << fmc_la[i]))));
				retval = TST_STS_ERR;
			}
			else { TST_LOG( tc, (logline, "-> Drive '1' -> OK \n")); }

			// ---------- Drive '0'

			// Set all bit in IN direction
			data = 0;
			tsc_csr_write(fmc[j] + 0x1c, &data);

			usleep(100);

			// Set bit per bit in OUT direction
			data = 1 << i;
			tsc_csr_write(fmc[j] + 0x1c , &data);

			usleep(100);

			// Write '0' for OUT value
			data = 0 << i;
			tsc_csr_write(fmc[j] + 0x04 , &data);

			usleep(100);

			// Check that IN value is well '0'
			tsc_csr_read(fmc[j] + 0x10, &data);
			if ((data & (0 << i)) != (0 << i)) {
				TST_LOG( tc, (logline, "-> Error in local LA input value : expected 0x%08x read 0x%08x \n", (0 << i), (data & (0 << i))));
				retval = TST_STS_ERR;
			}
			else { TST_LOG( tc, (logline, "-> LA[%02i] <-> LA[%02i] ", i, fmc_la[i])); }

			// Check on other side that value is '0' on IN

			// Check corresponding value
			tsc_csr_read(fmc[j] + 0x10, &data);

			if ((0 << fmc_la[i]) != (data & (0 << fmc_la[i]))){
				TST_LOG( tc, (logline, "-> Error in remote LA input value : expected 0x%08x read 0x%08x \n", (0 << fmc_la[i]), (data & (0 << fmc_la[i]))));
				retval = TST_STS_ERR;
			}
			else { TST_LOG( tc, (logline, "-> Drive '0' -> OK \n")); }
		}

		// FMC test LA[33:32] -----------------------------------------------------------------
		for (i = 0; i < 2; i++){

			// ---------- Drive '1'

			// Set all bit in IN direction
			data = 0;
			tsc_csr_write(fmc[j] + 0x20, &data);

			usleep(100);

			// Set bit per bit in OUT direction
			data = 1 << i;
			tsc_csr_write(fmc[j] + 0x20 , &data);

			usleep(100);

			// Write '1' for OUT value
			data = 1 << i;
			tsc_csr_write(fmc[j] + 0x08 , &data);

			usleep(100);

			// Check that IN value is well '1'
			tsc_csr_read(fmc[j] + 0x14, &data);
			if ((data & (1 << i)) != (1 << i)) {
				TST_LOG( tc, (logline, "-> Error in local LA input value : expected 0x%08x read 0x%08x \n", (1 << i), (data & (1 << i))));
				retval = TST_STS_ERR;
			}
			else { TST_LOG( tc, (logline, "-> LA[%02i] <-> LA[%02i] ", i + 32, fmc_la[i + 32])); }

			// Check on other side that value is '1' on IN

			// Check corresponding value
			tsc_csr_read(fmc[j] + 0x14, &data);

			if ((1 << fmc_la[i + 32]) != (data & (1 << fmc_la[i + 32]))){
				TST_LOG( tc, (logline, "-> Error in remote LA input value : expected 0x%08x read 0x%08x \n", (1 << fmc_la[i + 32]), (data & (1 << fmc_la[i + 32]))));
				retval = TST_STS_ERR;
			}
			else { TST_LOG( tc, (logline, "-> Drive '1' -> OK \n")); }

			// ---------- Drive '0'

			// Set all bit in IN direction
			data = 0;
			tsc_csr_write(fmc[j] + 0x20, &data);

			usleep(100);

			// Set bit per bit in OUT direction
			data = 1 << i;
			tsc_csr_write(fmc[j] + 0x20 , &data);

			usleep(100);

			// Write '0' for OUT value
			data = 0 << i;
			tsc_csr_write(fmc[j] + 0x08 , &data);

			usleep(100);

			// Check that IN value is well '0'
			tsc_csr_read(fmc[j] + 0x14, &data);
			if ((data & (0 << i)) != (0 << i)) {
				TST_LOG( tc, (logline, "-> Error in local LA input value : expected 0x%08x read 0x%08x \n", (0 << i), (data & (0 << i))));
				retval = TST_STS_ERR;
			}
			else { TST_LOG( tc, (logline, "-> LA[%02i] <-> LA[%02i] ", i + 32, fmc_la[i + 32])); }

			// Check on other side that value is '0' on IN

			// Check corresponding value
			tsc_csr_read(fmc[j] + 0x14, &data);

			if ((0 << fmc_la[i + 32]) != (data & (0 << fmc_la[i + 32]))){
				TST_LOG( tc, (logline, "-> Error in remote LA input value : expected 0x%08x read 0x%08x \n", (0 << fmc_la[i + 32]), (data & (0 << fmc_la[i + 32]))));
				retval = TST_STS_ERR;
			}
			else { TST_LOG( tc, (logline, "-> Drive '0' -> OK \n")); }
		}

		TST_LOG( tc, (logline, "\n"));
		TST_LOG( tc, (logline, "---------- HA ----------\n"));
		TST_LOG( tc, (logline, "\n"));

		// FMC test HA[23:0] ---------------------------------------------------------------------
		for (i = 0; i < 24; i++){

			// ---------- Drive '1'

			// Set all bit in IN direction
			data = 0;
			tsc_csr_write(fmc[j] + 0x20, &data);

			usleep(100);

			// Set bit per bit in OUT direction
			data = 1 << (i + 2);
			tsc_csr_write(fmc[j] + 0x20 , &data);

			usleep(100);

			// Write '1' for OUT value
			data = 1 << (i + 2);
			tsc_csr_write(fmc[j] + 0x08 , &data);

			usleep(100);

			// Check that IN value is well '1'
			tsc_csr_read(fmc[j] + 0x14, &data);
			if ((data & (1 << (i + 2))) != (1 << (i + 2))) {
				TST_LOG( tc, (logline, "-> Error in local HA input value : expected 0x%08x read 0x%08x \n", (1 << (i + 2)), (data & (1 << (i + 2)))));
				retval = TST_STS_ERR;
			}
			else { TST_LOG( tc, (logline, "-> HA[%02i] <-> HA[%02i] ", i , fmc_ha[i])); }

			// Check on other side that value is '1' on IN

			// Check corresponding value
			tsc_csr_read(fmc[j] + 0x14, &data);

			if (((1 << fmc_ha[i]) << 2) != (data & ((1 << fmc_ha[i]) << 2))){
				TST_LOG( tc, (logline, "-> Error in remote HA input value : expected 0x%08x read 0x%08x \n", ((1 << fmc_ha[i]) << 2), (data & ((1 << fmc_ha[i]) << 2))));
				retval = TST_STS_ERR;
			}
			else { TST_LOG( tc, (logline, "-> Drive '1' -> OK \n")); }

			// ---------- Drive '0'

			// Set all bit in IN direction
			data = 0;
			tsc_csr_write(fmc[j] + 0x20, &data);

			usleep(1000);

			// Set bit per bit in OUT direction
			data = 1 << (i + 2);
			tsc_csr_write(fmc[j] + 0x20 , &data);

			usleep(100);

			// Write '0' for OUT value
			data = 0 << (i + 2);
			tsc_csr_write(fmc[j] + 0x08 , &data);

			usleep(100);

			// Check that IN value is well '0'
			tsc_csr_read(fmc[j] + 0x14, &data);
			if ((data & (0 << (i + 2))) != (0 << (i + 2))) {
				TST_LOG( tc, (logline, "-> Error in local HA input value : expected 0x%08x read 0x%08x \n", (0 << (i + 2)), (data & (0 << (i + 2)))));
				retval = TST_STS_ERR;
			}
			else { TST_LOG( tc, (logline, "-> HA[%02i] <-> HA[%02i] ", i , fmc_ha[i])); }

			// Check on other side that value is '0' on IN

			// Check corresponding value
			tsc_csr_read(fmc[j] + 0x14, &data);

			if (((0 << fmc_ha[i]) << 2) != (data & ((0 << fmc_ha[i]) << 2))){
				TST_LOG( tc, (logline, "-> Error in remote HA input value : expected 0x%08x read 0x%08x \n", ((0 << fmc_ha[i]) << 2), (data & ((0 << fmc_ha[i]) << 2))));
				retval = TST_STS_ERR;
			}
			else { TST_LOG( tc, (logline, "-> Drive '0' -> OK \n")); }
		}

		TST_LOG( tc, (logline, "\n"));
		TST_LOG( tc, (logline, "---------- HB ----------\n"));
		TST_LOG( tc, (logline, "\n"));

		// FMC test HB[5:0] ----------------------------------------------------------------------
		for (i = 0; i < 6; i++){

			// ---------- Drive '1'

			// Set all bit in IN direction
			data = 0;
			tsc_csr_write(fmc[j] + 0x20, &data);

			usleep(100);

			// Set bit per bit in OUT direction
			data = 1 << (i + 26);
			tsc_csr_write(fmc[j] + 0x20 , &data);

			usleep(100);

			// Write '1' for OUT value
			data = 1 << (i + 26);
			tsc_csr_write(fmc[j] + 0x08 , &data);

			usleep(100);

			// Check that IN value is well '1'
			tsc_csr_read(fmc[j] + 0x14, &data);
			if ((data & (1 << (i + 26))) != (1 << (i + 26))) {
				TST_LOG( tc, (logline, "-> Error in local HB input value : expected 0x%08x read 0x%08x \n", (1 << (i + 26)), (data & (1 << (i + 26)))));
				retval = TST_STS_ERR;
			}
			else { TST_LOG( tc, (logline, "-> HB[%02i] <-> HB[%02i] ", i , fmc_hb[i])); }

			// Check on other side that value is '1' on IN

			// Check corresponding value
			tsc_csr_read(fmc[j] + 0x14, &data);

			if (((1 << fmc_hb[i]) << 26) != (data & ((1 << fmc_hb[i]) << 26))){
				TST_LOG( tc, (logline, "-> Error in remote HB input value : expected 0x%08x read 0x%08x \n", ((1 << fmc_hb[i]) << 26), (data & ((1 << fmc_hb[i]) << 26))));
				retval = TST_STS_ERR;
			}
			else { TST_LOG( tc, (logline, "-> Drive '1' -> OK \n")); }

			// ---------- Drive '0'

			// Set all bit in IN direction
			data = 0;
			tsc_csr_write(fmc[j] + 0x20, &data);

			usleep(100);

			// Set bit per bit in OUT direction
			data = 1 << (i + 26);
			tsc_csr_write(fmc[j] + 0x20 , &data);

			usleep(100);

			// Write '0' for OUT value
			data = 0 << (i + 26);
			tsc_csr_write(fmc[j] + 0x08 , &data);

			usleep(100);

			// Check that IN value is well '0'
			tsc_csr_read(fmc[j] + 0x14, &data);
			if ((data & (0 << (i + 26))) != (0 << (i + 26))) {
				TST_LOG( tc, (logline, "-> Error in local HB input value : expected 0x%08x read 0x%08x \n", (0 << (i + 26)), (data & (0 << (i + 26)))));
				retval = TST_STS_ERR;
			}
			else { TST_LOG( tc, (logline, "-> HB[%02i] <-> HB[%02i] ", i , fmc_hb[i])); }

			// Check on other side that value is '0' on IN

			// Check corresponding value
			tsc_csr_read(fmc[j] + 0x14, &data);

			if (((0 << fmc_hb[i]) << 26) != (data & ((0 << fmc_hb[i]) << 26))){
				TST_LOG( tc, (logline, "-> Error in remote HB input value : expected 0x%08x read 0x%08x \n", ((0 << fmc_hb[i]) << 26), (data & ((0 << fmc_hb[i]) << 26))));
				retval = TST_STS_ERR;
			}
			else { TST_LOG( tc, (logline, "-> Drive '0' -> OK \n")); }
		}

		// FMC test HB[21:6] ----------------------------------------------------------------------
		for (i = 0; i < 16; i++){

			// ---------- Drive '1'

			// Set all bit in IN direction
			data = 0;
			tsc_csr_write(fmc[j] + 0x24, &data);

			usleep(100);

			// Set bit per bit in OUT direction
			data = 1 << i;
			tsc_csr_write(fmc[j] + 0x24 , &data);

			usleep(100);

			// Write '1' for OUT value
			data = 1 << i;
			tsc_csr_write(fmc[j] + 0x0c , &data);

			usleep(100);

			// Check that IN value is well '1'
			tsc_csr_read(fmc[j] + 0x18, &data);
			if ((data & (1 << i)) != (1 << i)) {
				TST_LOG( tc, (logline, "-> Error in local HB input value : expected 0x%08x read 0x%08x \n", (1 << i), (data & (1 << i))));
				retval = TST_STS_ERR;
			}
			else { TST_LOG( tc, (logline, "-> HB[%02i] <-> HB[%02i] ", i + 6 , fmc_hb[i + 6])); }

			// Check on other side that value is '1' on IN

			// Check corresponding value
			tsc_csr_read(fmc[j] + 0x18, &data);

			if (((1 << (fmc_hb[i + 6] - 6))) != (data & ((1 << (fmc_hb[i + 6] - 6))))){
				TST_LOG( tc, (logline, "-> Error in remote HB input value : expected 0x%08x read 0x%08x \n", ((1 << (fmc_hb[i + 6] - 6))), (data & ((1 << (fmc_hb[i + 6] - 6))))));
				retval = TST_STS_ERR;
			}
			else { TST_LOG( tc, (logline, "-> Drive '1' -> OK \n")); }

			// ---------- Drive '0'

			// Set all bit in IN direction
			data = 0;
			tsc_csr_write(fmc[j] + 0x24, &data);

			usleep(100);

			// Set bit per bit in OUT direction
			data = 1 << i;
			tsc_csr_write(fmc[j] + 0x24 , &data);

			usleep(100);

			// Write '0' for OUT value
			data = 0 << i;
			tsc_csr_write(fmc[j] + 0x0c , &data);

			usleep(100);

			// Check that IN value is well '0'
			tsc_csr_read(fmc[j] + 0x18, &data);
			if ((data & (0 << i)) != (0 << i)) {
				TST_LOG( tc, (logline, "-> Error in local HB input value : expected 0x%08x read 0x%08x \n", (0 << i), (data & (0 << i))));
				retval = TST_STS_ERR;
			}
			else { TST_LOG( tc, (logline, "-> HB[%02i] <-> HB[%02i] ", i + 6 , fmc_hb[i + 6])); }

			// Check on other side that value is '0' on IN

			// Check corresponding value
			tsc_csr_read(fmc[j] + 0x18, &data);

			if (((0 << (fmc_hb[i + 6] - 6))) != (data & ((0 << (fmc_hb[i + 6] - 6))))){
				TST_LOG( tc, (logline, "-> Error in remote HB input value : expected 0x%08x read 0x%08x \n", ((0 << (fmc_hb[i + 6] - 6))), (data & ((0 << (fmc_hb[i + 6] - 6))))));
				retval = TST_STS_ERR;
			}
			else { TST_LOG( tc, (logline, "-> Drive '0' -> OK \n")); }
		}
	}

	if(retval == TST_STS_ERR){ TST_LOG( tc, (logline, "                             -> TEST NOK\r")); }
	else { TST_LOG( tc, (logline, "                             -> TEST OK\r")); }

	tm = time(0);
	ct = ctime(&tm);
	TST_LOG( tc, (logline, "\n%s->Exiting :%s", tst_id, ct));

	return( retval | TST_STS_DONE);
}

int tst_50(struct tst_ctl *tc){
	return(tst_fmc(tc, "Tst:50"));
}
