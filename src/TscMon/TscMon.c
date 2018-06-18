/*=========================< begin file & file header >=======================
 *  References
 *  
 *    filename : TscMon.c
 *    author   : JFG, XP
 *    company  : IOxOS
 *    creation : Sept 14,2015
 *
 *----------------------------------------------------------------------------
 *  Description
 *
 *    Main program for TscMon application.
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

#include <termios.h>
#include <stdlib.h>
#include <stdio.h>
#include <pty.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <aio.h>
#include <errno.h>
#include <time.h>

#include <tscioctl.h>
#include <cli.h>
#include <tsculib.h>
#include "cmdlist.h"
#include "tdma.h"

#include <sys/time.h>
#include <ctype.h>
#include <sys/stat.h>
#include <sys/mman.h>

char TscMon_version[] = "2.00";
char TscMon_official_release[] = "1.4.7";

int tsc_cmd_exec( struct cli_cmd_list *, struct cli_cmd_para *);

struct termios termios_old;
struct termios termios_new;
char cli_prompt[16];
struct cli_cmd_para cmd_para;
struct cli_cmd_history cmd_history;
struct aiocb aiocb;
char aio_buf[256];
char *cmdline;
int script_exit = 0;
int tsc_sign;
int tsc_date;

static char *month[16] ={ NULL,"Jan","Feb","Mar","Apr","May","Jun","Jul","Aug","Sep","Oct","Nov","Dec",NULL,NULL};

int tsc_fd;

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * Function name : tsc_ddr_idel_calib_start
 * Prototype     : int
 * Parameters    : memory (shm1 or shm2)
 * Return        : success/error
 *----------------------------------------------------------------------------
 * Description   : calibration of the ddr memory
 *
 *++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

int tsc_ddr_idel_calib_start(int quiet){
	struct tsc_ioctl_map_win map_win;
	char para_buf[32];
	int  DQ_NOK[16];
	int  DQ_OK[16];
	int  ppc                        = 0;
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
    unsigned int    init            = 0;
    unsigned int    cnt_value  = 0;
    unsigned int    init_cnt_value_store[16]  = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
    unsigned int    temp_cnt_value_store[16]  = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
    unsigned int    final_cnt_value_store[16] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
    unsigned int    dq_path         = 0;
    unsigned int    r, rr	        = 0;
    unsigned int    best            = 0;
    unsigned int    worst           = 0;
    unsigned int    location        = 0;

    ////////////////////////////////////////////////////////////////////////////////////
    // ADJUST DEFAULT VALUE FOR CURRENT_DLY ACCORDING TO HARDWARE IMPLEMENTATION      //
    unsigned int    CURRENT_DLY     = 256;	   // Current delay value                 //
    // ADJUST DEFAULT STEP VALUE FOR INC / DEC VALUE                                  //
    unsigned int    CURRENT_STEP    = 4 /* 8 */;	   // Current step value          //
    ////////////////////////////////////////////////////////////////////////////////////

    unsigned int    MAX             = 0x1ff/*64*/;      // Max delay tap value
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

    int mem = 0x12;

    // SMEM control & status register
    unsigned int SMEM_DDR3_CSR[2] = {0x800, 0xc00};

    // IDEL adjustment register for both DDR3 memory
    unsigned int SMEM_DDR3_IFSTA[2] = {0x808, 0xc08};

    // IDEL control register for both DDR3 memory
    unsigned int SMEM_DDR3_IDEL[2] = {0x80c, 0xc0c};

    ppc = CheckByteOrder(); // Check endianness: 1-> ppc, 0-> x86

    // If calibration of both ddr, loop twice
    if(mem == 0x12){
    	rr  = 2;
    	mem = 1;
    }
    // else loop only on the SMEM1 or SMEM2
    else {
    	rr = 1;
    }

    // Global loop for DDR calibration
    for (r = 0; r < rr; r++){

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
			if (quiet == 0) {
				printf("SMEM1 calibration\n");
			}
		}
		else if((mem - 1) == 1){
			map_win.req.mode.space = MAP_SPACE_SHM2; // SHM #2
			if (quiet == 0) {
				printf("SMEM2 calibration\n");
			}
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

		//Reset memory calibration
		// Only on the first memory due to the fact that reset impact both memory
		// Calibration need to be done in the order : 1 -> 2
		if(mem == 1){
			data = 0x8000;
			tsc_csr_write(tsc_fd, SMEM_DDR3_CSR[mem - 1], &data);
			data = 0x2080;
			tsc_csr_write(tsc_fd, SMEM_DDR3_CSR[mem - 1], &data);
		}

		// Reset calibration register
		data = 0;
		tsc_csr_write(tsc_fd, SMEM_DDR3_IFSTA[mem - 1], &data);
		tsc_csr_write(tsc_fd, SMEM_DDR3_IDEL[mem - 1], &data);

		// Set IDEL to 0
		data = 0;
		tsc_csr_write(tsc_fd, SMEM_DDR3_IDEL[mem - 1], &data);

		// Loop on 16 DQ
		for(j = 0; j < 16; j++){

			// Store initial value of count of the IFSTA register
			dq_path = j << 12;
			tsc_csr_write(tsc_fd, SMEM_DDR3_IFSTA[mem - 1], &dq_path);
			tsc_csr_read(tsc_fd, SMEM_DDR3_IFSTA[mem - 1], &cnt_value);

			init_cnt_value_store[j] = 0;/* cnt_value & 0xff;*/
			temp_cnt_value_store[j] = init_cnt_value_store[j];
/*
			if(j < 8){
				if (quiet == 0) {
					printf(" DQ[%02d] ->", j + 8);
				}
			}
			else{
				if (quiet == 0) {
					printf(" DQ[%02d] ->", j - 8);
				}
			}
*/
			// Reset avg_x, start index, number of test passed "ok" and end value for each DQ
			avg_x = 0;
			start = 0;
			end   = 0;
			ok    = 0;

			// Add steps by steps for current DQ from initial count value to max
			for(k = init_cnt_value_store[j]; k < MAX ; k = k + CURRENT_STEP){
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
					end   = k;
					NOK   = 0;
					ok++;
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
				marker = init_cnt_value_store[j];
			}
			// Update the new count value with the median value
			else {
				// Compute the start window
				start = (end - (ok * CURRENT_STEP)) + CURRENT_STEP/*1*/;

				// Compute the average of the window
				avg_x = (ok * CURRENT_STEP) / 2;

				// Compute the new delay marker to apply
				marker = start + avg_x;
			}

			// Update the array with the new count value
			final_cnt_value_store[j] = marker;

			// Trace new delay
/*
			if (quiet == 0) {
				printf(" Delay 0x%x, %i steps", marker, ok);
			}
			for(n = init_cnt_value_store[j] ; n < marker; n = n + CURRENT_STEP){
				printf("   ");
			}
			if (quiet == 0) {
				printf("\n");
			}
*/

if (ppc == 1){
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
else{
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

			// Set IDEL and IFSTA to 0
			data = 0;
			tsc_csr_write(tsc_fd, SMEM_DDR3_IDEL[mem - 1], &data);
			tsc_csr_write(tsc_fd, SMEM_DDR3_IFSTA[mem - 1], &data);

		}

		// Execution is finished OK or NOK
		if (NOK == 1){
			printf("Calibration is not possible, error on line(s) : \n");
			for (m = 0; m < 16; m++){
				if (DQ_NOK[m] == 1){
					printf("DQ[%i] \n", m);
				}
			}
		}
		else {
			if (quiet == 0) {
				printf("Done !  -> ");
			}

			// Search best case
		    best = DQ_OK[0];

		    for (m = 1 ;m < 16 ;m++) {
		    	if ( DQ_OK[m] > best ) {
		    		best = DQ_OK[m];
		    		if (m < 8){
		    			location = m + 8;
		    		}
		    		else {
		    			location = m - 8;
		    		}
		        }
		    }
		    if (quiet == 0) {
		    	printf("best case DQ[%i], %i steps | ", location, best);
		    }

			// Search worst case
		    worst = DQ_OK[0];

		    for (m = 1 ;m < 16 ;m++) {
		    	if ( DQ_OK[m] < worst ) {
		    		worst = DQ_OK[m];
		    		if (m < 8){
		    			location = m + 8;
		    		}
		    		else {
		    			location = m - 8;
		    		}
		        }
		    }
		    if (quiet == 0) {
		    	printf("worst case DQ[%i], %i steps\n", location, worst);
		    }
		}

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
/*
		if (quiet == 0) {
			printf("\n");
		}
*/
    }
    return 0;
}

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * Function name : main
 * Prototype     : int
 * Parameters    : argument count
 *                 pointer to argument list
 * Return        : 0
 *----------------------------------------------------------------------------
 * Description   : main entry for tscmon application
 *
 *++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

int main(int argc, char *argv[]){
	struct cli_cmd_history *h;
	struct winsize winsize;
	int iex     = 0;
	int retval  = 0;
	int cmd_cnt = 0;
	int mm      = 0;
	int dd      = 0;
	int yy      = 0;
	int hh      = 0;
	int mn      = 0;
	int ss      = 0;
	int data    = 0;
	int ret     = 0;
	int quiet   = 0;

	// Test if TscMon is started in quiet mode
	if (argc > 1){
		if (argv[1][0] == 'q') {
			quiet = 1;
		}
		else {
			quiet = 0;
		}
	}

	/* TscMon is only aware of first IFC */
	tsc_fd = tsc_init(0);
	if(tsc_fd < 0){
		printf("Cannot find interface\n");
		exit( -1);
	}

	retval = tsc_csr_read(tsc_fd, 0x18, &tsc_sign);
	if( retval < 0){
		printf("  ERROR -> cannot access ILOC_SIGN register !!\n");
	}
	tsc_csr_read(tsc_fd, 0x28, &tsc_date);
	ss = tsc_date & 0x3f;
	mn = (tsc_date>>6) & 0x3f;
	hh = (tsc_date>>12) & 0x1f;
	yy = (tsc_date>>17) & 0x3f;
	mm = (tsc_date>>23) & 0xf;
	dd = (tsc_date>>27) & 0x1f;

	/* configure the terminal in canonical mode with echo */
	ioctl( 0, TIOCGWINSZ, &winsize);
	tcgetattr( 0, &termios_old);
	memcpy( &termios_new, &termios_old, sizeof( struct termios));
	termios_new.c_lflag &=  (uint)(~(ECHOCTL | ECHO | ICANON));
	tcsetattr( 0, TCSANOW, &termios_new);

	/* initialize command list history */
	h = cli_history_init( &cmd_history);

	rdwr_init();
	tst_init();
	dma_init();
	tdma_init(quiet);

	// Launch automatically DDR3 calibration
	tsc_ddr_idel_calib_start(quiet);

	// Test how many arguments exist and their position
	if(argc > 2){
		struct cli_cmd_para script_para;

		/* check for script execution */
		if(argv[2][0] == '@') {
			cli_cmd_parse( &argv[2][1], &script_para);
			iex = tsc_script( &argv[2][1], &script_para);
			if( iex == 2){
				goto TscMon_exit;
			}
		}
	}
	else if(argc > 1){
		struct cli_cmd_para script_para;

		/* check for script execution */
		if(argv[1][0] == '@') {
			cli_cmd_parse( &argv[1][1], &script_para);
			iex = tsc_script( &argv[1][1], &script_para);
			if( iex == 2){
				goto TscMon_exit;
			}
		}
	}

	if (quiet == 0){


//		printf("          _______       __  __             \n");
//		printf("         |__   __|     |  \\/  |            \n");
//		printf("            | |___  ___| \\  / | ___  _ __  \n");
//		printf("            | / __|/ __| |\\/| |/ _ \\| '_ \\ \n");
//		printf("            | \\__ \\ (__| |  | | (_) | | | |\n");
//		printf("            |_|___/\\___|_|  |_|\\___/|_| |_|\n");
		printf("    _______ _______ _______ _______  _____  __   _ \n");
		printf("       |    |______ |       |  |  | |     | | \\  | \n");
		printf("       |    ______| |_____  |  |  | |_____| |  \\_| \n");
		printf("\n");
		printf("     +------------------------------------------+\n");
		printf("     |  IOxOS Technologies Copyright 2015-2018  |\n");
		if(tsc_get_device_id() == 0x1000){
			printf("     |  TscMon - %s %04x diagnostic tool         |\n", "IO", tsc_get_device_id());
		}
		else if(tsc_get_device_id() == 0x1001){
			printf("     |  TscMon - %s %04x diagnostic tool   |\n", "CENTRAL", tsc_get_device_id());
		}
		printf("     |  Version %s - %s %s     |\n", TscMon_version, __DATE__, __TIME__);
		printf("     |  FPGA Built %s %02d 20%02d %02d:%02d:%02d         |\n", month[mm], dd, yy, hh, mn, ss);
		printf("     |  FPGA Sign  %08x                     |\n", tsc_sign);

		tsc_pon_read(tsc_fd, 0x0, &data);
		if (data == 0x73571211) {
			printf("     |  Driver IFC1211 Version %s             |\n", tsc_get_drv_version());
		}
		else if (data == 0x73571410){
			printf("     |  Driver IFC1410 Version %s             |\n", tsc_get_drv_version());
		}
		else if (data == 0x73571411){
			printf("     |  Driver IFC1411 Version %s             |\n", tsc_get_drv_version());
		}
		printf("     |  ******* Official release %s *******  |\n", TscMon_official_release);
		printf("     +------------------------------------------+\n");
	}
	printf("\n");

	cmd_cnt = 0;
	while(1){
		sprintf(cli_prompt, "%d:TscMon>", ++cmd_cnt);
		cmdline = cli_get_cmd( h, cli_prompt);

		if( cmdline[0] == 'q'){
			break;
		}
		// Check for script execution
		if( cmdline[0] == '@'){
			struct cli_cmd_para script_para;

			cli_cmd_parse(&cmdline[1], &script_para);
			iex = tsc_script( &cmdline[1], &script_para);
			if( iex == 2){
				break;
			}
			continue;
		}
		// Loop command
		if( cmdline[0] == '&'){
			printf("Entering loop mode [enter any character to stop loop]...\n");
			if( aio_error( &aiocb) != EINPROGRESS){
				retval = aio_read( &aiocb);
				if( retval < 0){
					perror("aio_read");
					goto TscMon_end_loop;
				}
			}
			while( 1){
				// Execute script
				if( cmdline[1] == '@'){
					struct cli_cmd_para script_para;

					cli_cmd_parse(  &cmdline[2], &script_para);
					iex = tsc_script( &cmdline[2], &script_para);
					if(iex == 2){
						break;
					}
				}
				else{
					cli_cmd_parse( &cmdline[1], &cmd_para);
					if( tsc_cmd_exec( &cmd_list[0], &cmd_para) < 0){
						if( aio_error( &aiocb) == EINPROGRESS){
							retval = aio_cancel( aiocb.aio_fildes, &aiocb);
						}
						goto TscMon_end_loop;
					}
				}
				if(aio_error( &aiocb) != EINPROGRESS){
					aio_return( &aiocb);
					goto TscMon_end_loop;
				}
			}
TscMon_end_loop:
      	  continue;
		}
		cli_cmd_parse( cmdline, &cmd_para);
		if( cmdline[0] == '?'){
			tsc_func_help( &cmd_para);
			continue;
		}
		// Call specific command according to the history
		if( cmdline[0] == '!'){
			char *new_cmd;
			int idx;

			cmd_para.cmd = NULL;
			if( sscanf( &cmdline[1], "%d", &idx) == 1){
				new_cmd = cli_history_find_idx( &cmd_history, idx);
				if( new_cmd){
					printf("%s\n", new_cmd);
					cli_cmd_parse( new_cmd, &cmd_para);
				}
			}
			else {
				new_cmd = cli_history_find_str( &cmd_history, &cmdline[1]);
				if( new_cmd){
					printf("%s\n", new_cmd);
					cli_cmd_parse( new_cmd, &cmd_para);
				}
			}
		}
		// Call alias
		if( cmdline[0] == '+'){
			char *new_cmd;

			new_cmd = alias_find( &cmdline[1]);
			cmd_para.cmd = NULL;
			if( new_cmd){
				printf("%s\n", new_cmd);
				cli_cmd_parse( new_cmd, &cmd_para);
			}
		}
		// Execute OS command
		if( cmdline[0] == '$'){
			char new_cmd[1024];

			strcpy(new_cmd, &cmdline[1]);
			printf("OS command: <%s>\n", new_cmd);

			ret =  system(new_cmd);
			printf("return code: %i\n", ret);

		}
		else if( cmd_para.cmd){
			tsc_cmd_exec( &cmd_list[0], &cmd_para);
		}

	}

TscMon_exit:
	kbuf_free( NULL);
  	tst_exit();
  	rdwr_exit();

  	/* restore previous terminal setting */
  	tcsetattr( 0, TCSANOW, &termios_old);
	tsc_exit(tsc_fd);
  	exit(0);
}

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * Function name : tsc_cmd_exec
 * Prototype     : int
 * Parameters    : command parameter, command list
 * Return        : status
 *----------------------------------------------------------------------------
 * Description   : execute command
 *
 *++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

int tsc_cmd_exec( struct cli_cmd_list *l, struct cli_cmd_para *c){
	long i;

	i = 0;
	if(strlen(c->cmd)) {
		while(1) {
			if( !l->cmd) {// Command doesn't exist, break
				break;
			}
			// Check that the user enter the correct function name !
			if(!strncmp(l->cmd, c->cmd, strlen(l->cmd))){
				c->idx = i;
				return( l->func( c));
			}
			i++; l++;
		}
		printf("%s -> Invalid command name ", c->cmd);
		printf("(\'help\' or \'?\' displays a list of valid commands)\n");
	}
	return(-1);
}

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * Function name : tsc_print_usage
 * Prototype     : int
 * Parameters    : command parameter
 * Return        : 0
 *----------------------------------------------------------------------------
 * Description   : print usage
 *
 *++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

int tsc_print_usage( struct cli_cmd_para *c){
	long i;

	i = 0;
	while( cmd_list[c->idx].msg[i]){
		printf("%s\n", cmd_list[c->idx].msg[i]);
		i++;
	}
	return(0);
}

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * Function name : tsc_func_help
 * Prototype     : int
 * Parameters    : command paramaeter structure
 * Return        : error/success
 *----------------------------------------------------------------------------
 * Description   : help
 *
 *++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

int tsc_func_help( struct cli_cmd_para *c){
	char *cmd;
	long i, j;

	i = 0;
	if( c->cnt > 0){
		while(1){
			cmd = cmd_list[i].cmd;
			if( cmd){
				if( !strcmp( c->para[0], cmd)){
					long j;

					j = 0;
					while( cmd_list[i].msg[j]){
						printf("%s\n", cmd_list[i].msg[j]);
						j++;
					}
					return(0);
				}
				i++;
			}
			else{
				printf("%s -> Invalid command name ", c->para[0]);
				printf("(\'help\' or \'?\' displays a list of valid commands)\n");
				return(-1);
			}
		}
	}

	while(1){
		cmd = cmd_list[i++].cmd;
		if( cmd){
			printf("%s", cmd);
			if( i&3){
				j = 10 - (int)strlen( cmd);
				while(j--){
					putchar(' ');
				}
			}
			else{
				putchar('\n');
			}
		}
		else{
			putchar('\n');
			break;
		}
	}
	return(0);
}

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * Function name : tsc_func_history
 * Prototype     : int
 * Parameters    : command parameter structure
 * Return        : 0
 *----------------------------------------------------------------------------
 * Description   : history function
 *
 *++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

int tsc_func_history( struct cli_cmd_para *c){
	cli_history_print( &cmd_history);
	return(0);
}

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * Function name : tsc_wait
 * Prototype     : int
 * Parameters    : command parameter
 * Return        : status
 *----------------------------------------------------------------------------
 * Description   : wait command
 *
 *++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

int tsc_wait(struct cli_cmd_para *c){
	int retval, ret;
	int tmo;
	char ch;

	printf("Continue [y/n] -> ");
	fflush( stdout);
	retval = 0;
	ch = 'y';
	if( aio_error( &aiocb) != EINPROGRESS){
		ret = aio_read( &aiocb);
		if( ret < 0){
			perror("aio_read");
			goto tsc_wait_exit;
		}
	}
	tmo = -1;
	if( c->cnt > 0){
		sscanf( c->para[0], "%d", &tmo);
		tmo = tmo * 10;
	}
	while( tmo--){
		if( aio_error( &aiocb) != EINPROGRESS){
			retval = 0;
			ch = *(char *)aiocb.aio_buf;
			if( ch == 'n'){
				retval = -1;
			}
			aio_return( &aiocb);
			goto tsc_wait_exit;
		}
		usleep(100000);
	}

	if( aio_error( &aiocb) == EINPROGRESS){
		ret = aio_cancel( aiocb.aio_fildes, &aiocb);
	}

tsc_wait_exit:

	printf("%c\n", ch);
	return( retval);
}
