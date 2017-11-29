/*=========================< begin file & file header >=======================
 *  References
 *
 *    filename : tst_2x.c
 *    author   : XP
 *    company  : IOxOS
 *    creation : Nov, 29 2017
 *    version  : 0.0.1
 *
 *----------------------------------------------------------------------------
 *  Description
 *
 *	  DMA tests
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

struct tsc_ioctl_dma_req req_p; // dma request structure

			                     //SRAM1    SRAM2    DDR1    DDR2    USR1    USR2    KBUF
int source[7]                  = { 2,       3,       2,      3,      4,      5,      0};
const char * source_txt[]      = {"SRAM1", "SRAM2", "DDR1", "DDR2", "USR1", "USR2", "KBUF"};

int destination[7]             = { 2,       3,       2,      3,      4,      5,      0};
const char * destination_txt[] = {"SRAM1", "SRAM2", "DDR1", "DDR2", "USR1", "USR2", "KBUF"};

/* ->    * SRAM1 * SRAM2 * DDR1 * DDR2 * USR1 * USR2 * KBUF * DEST
 * **********************************************************
 * SRAM1 * ----- *       * ---- *      *      *      *      *
 * SRAM2 *       * ----- *      * ---- *      *      *      *
 * DDR1  * ----- *       * ---- *      *      *      *      *
 * DDR2  *       * ----- *      * ---- *      *      *      *
 * USR1  *       *       *      *      * ---- *      *      *
 * USR2  *       *       *      *      *      * ---- *      *
 * KBUF  *       *       *      *      *      *      * ---- *
 * **********************************************************
 * SRC   *
 */

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * Function name : dma_configure
 * Prototype     : void
 * Parameters    : dma channel, source, destination, size, space src, space dest
 * Return        : Done or Error
 *
 *----------------------------------------------------------------------------
 * Description   : Dma configure [channel, src, des, size, space_src, space_des]
 *
 *++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
void dma_configure(int channel, int src, int des, int size, int space_src, int space_des){
	req_p.des_addr   = des;
	req_p.des_space  = space_des;
	req_p.des_mode   = 0;
	req_p.src_addr   = src;
	req_p.src_space  = space_src;
	req_p.src_mode   = 0;
	req_p.size       = size;

	// User dma channel 0, 1 for ENGINE-SHM1
	if (channel == 0){
		req_p.start_mode = (char)DMA_START_CHAN(DMA_CHAN_0);
	}
	else if (channel == 1){
		req_p.start_mode = (char)DMA_START_CHAN(DMA_CHAN_1);
	}
	// User dma channel 2, 3 for ENGINE-SHM2
	else if (channel == 2){
		req_p.start_mode = (char)DMA_START_CHAN(DMA_CHAN_2);
	}
	else if (channel == 3){
		req_p.start_mode = (char)DMA_START_CHAN(DMA_CHAN_3);
	}
	req_p.end_mode   = 0;
	req_p.intr_mode  = DMA_INTR_ENA;
	req_p.wait_mode  = DMA_WAIT_INTR | DMA_WAIT_1S | (5 << 4);
}

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * Function name : tst_dma_
 * Prototype     : int
 * Parameters    : test control structure, test ID, direction of transfer
 * Return        : Done or Error
 *
 *----------------------------------------------------------------------------
 * Description   : DMA test between all agents (SRAM1, SRAM2, SHM1, SHM2, USR1,
 *                 USR2, KBUF) on all channels (0, 1, 2, 3) in both direction
 *
 *++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
int tst_dma(struct tst_ctl *tc, char *tst_id){
	struct tsc_ioctl_kbuf_req buf_p;
	time_t tm;
	char *ct           = NULL;
	char *ref_buf      = NULL; // 0xdeadface
	char *data_buf     = NULL; // Consistent data
	char *check_buf    = NULL; // Resulting data for check
	char *eaddr        = NULL;
	int i              = 0;
	int retval         = 0;
	int size_ref       = 0;
	int offset  	   = 0;
	int sub_offset     = 0x1000; // Sliding sub_offset
	int sub_offset_ref = 0x1000;
	int sub_size       = tc->at->shm_size_0 - 0x2000; // Sliding size
	int sub_size_ref   = tc->at->shm_size_0 - 0x2000;
	int ref_pattern    = 0xdeadface;
	int src            = 0;
	int dest           = 0;
	int channel        = 0;

	tm = time(0);
	ct = ctime(&tm);

	TST_LOG( tc, (logline, "%s->Entering:%s\n", tst_id, ct));

	// Reference size for DMA test (SRAM, DDDR, USR)
	size_ref = tc->at->shm_size_0;

	// Compute board offset in decoding window //////////////////////////////////////////////////////////// !!!!!!!!!!!!!!!!!!!!!!!!!
	offset   = tc->at->shm_offset_0;
	//offset   = tc->at->sram_offset_0;
	//offset   = tc->at->usr_offset_0;

	// Prepare buffers
	ref_buf   = (char *)malloc(size_ref);
	data_buf  = (char *)malloc(size_ref);
	check_buf = (char *)malloc(size_ref);

	// Allocate a Kernel buffer
	buf_p.size = 0x100000;
	retval = tsc_kbuf_alloc(&buf_p);
	if(retval != 0){
	    TST_LOG( tc, (logline, "->Error allocatint kernel buffer\n"));
	    retval = TST_STS_ERR;
		// Free data buffers
		free(ref_buf);
		free(data_buf);
		free(check_buf);
		// Free kernel buffer
		tsc_kbuf_free(&buf_p);

		tm = time(0);
		ct = ctime(&tm);
		TST_LOG( tc, (logline, "\n%s->Exiting :%s", tst_id, ct));
		return( retval | TST_STS_DONE);
	}

	// Fill reference buffer with pattern 0xdeadface
	tst_cpu_fill(ref_buf, size_ref, 0, ref_pattern, 0);

	// Fill data buffer with pattern 0 4 8 C ...
	tst_cpu_fill(data_buf, size_ref, 1, offset, 4);

	// Fill check buffer with 0
	tst_cpu_fill(check_buf, size_ref, 0, 0, 0);

// Loop on four DMA channels (0,1 engines SHM1, 1,2 engines SHM2) -----------------------------------------
	for (channel = 0; channel < 4; channel++) {

// Allocate DMA channel -----------------------------------------------------------------------------------
		if(tsc_dma_alloc(channel)){
			TST_LOG( tc, (logline, "->DMA channel %x is busy \n", channel));
			retval = TST_STS_ERR;
			// Free DMA engine
			tsc_dma_free(0);
			tsc_dma_free(1);
			// Free data buffers
			free(ref_buf);
			free(data_buf);
			free(check_buf);
			// Free kernel buffer
			tsc_kbuf_free(&buf_p);

			tm = time(0);
			ct = ctime(&tm);
			TST_LOG( tc, (logline, "\n%s->Exiting :%s", tst_id, ct));

			return( retval | TST_STS_DONE);
		}

// Loop on all DMA sources possible ----------------------------------------------------------------------
		for (src = 0; src < 7; src++) {

// Initialize source with consistent data ----------------------------------------------------------------
			if (source[src] == 0){									// PCIe - KBUF
				tsc_kbuf_write(buf_p.k_base, data_buf, size_ref);
			}
			else if (source[src] == 2){								// SRAM1 - DDR1
				tsc_shm_write(offset, data_buf, size_ref, 4, 0, 1);
			}
			else if (source[src] == 3){								// SRAM2 - DDR2
				tsc_shm_write(offset, data_buf, size_ref, 4, 0, 2);
			}
			else if (source[src] == 4){								// USR1
				tsc_usr_write(offset, data_buf, size_ref, 4, 0, 1);
			}
			else if (source[src] == 5){								// USR2
				tsc_usr_write(offset, data_buf, size_ref, 4, 0, 2);
			}

// Loop on all DMA destination possible ------------------------------------------------------------------
			for (dest = 0; dest < 7; dest++){

				// Exclude DMA transfer on same agent
				if (source[src] != destination[dest]){
					TST_LOG( tc, (logline, "%s->Executing DMA channel#%x from %s to %s            \n", tst_id, channel, source_txt[src], destination_txt[dest]));

					// Reset offset and size
					sub_size   = sub_size_ref;
					sub_offset = sub_offset_ref;

// Generate test with sliding size and offset ------------------------------------------------------------
					for(i = 0; i < 0x1000; i++){
						TST_LOG(tc, (logline, "%s->Executing: iteration:%4d size:%05x offset:%05x", tst_id, i++, sub_size, sub_offset));

						// Fill check buffer with 0
						tst_cpu_fill(check_buf, size_ref, 0, 0, 0);

// Prepare data destination with reference pattern -------------------------------------------------------
						if (destination[dest] == 0){							// PCIe - KBUF
							tsc_kbuf_write(buf_p.k_base, ref_buf, size_ref);
						}
						else if (destination[dest] == 2){						// SRAM1 - DDR1
							tsc_shm_write(offset, ref_buf, size_ref, 4, 0, 1);
						}
						else if (destination[dest] == 3){						// SRAM2 - DDR2
							tsc_shm_write(offset, ref_buf, size_ref, 4, 0, 2);
						}
						else if (destination[dest] == 4){						// USR1
							tsc_usr_write(offset, ref_buf, size_ref, 4, 0, 1);
						}
						else if (destination[dest] == 5){						// USR2
							tsc_usr_write(offset, ref_buf, size_ref, 4, 0, 2);
						}

						usleep(1000);

// Preapare DMA transfer ----------------------------------------------------------------------------------
						// Source is KBUF
						if (source[src] == 0){
							dma_configure(channel, buf_p.b_base + sub_offset, offset + sub_offset, sub_size, source[src], destination[dest]);
						}
						// Destination is KBUF
						else if (destination[dest] == 0){
							dma_configure(channel, offset + sub_offset, buf_p.b_base + sub_offset, sub_size, source[src], destination[dest]);
						}
						// All others cases (SRAM, SHM, USR)
						else {
							dma_configure(channel, offset + sub_offset, offset + sub_offset, sub_size, source[src], destination[dest]);
						}

// Do DMA transfer ----------------------------------------------------------------------------------------
						retval = tsc_dma_move(&req_p);
						if(retval < 0){
							TST_LOG(tc, (logline, "->DMA move error \n"));
							retval = TST_STS_ERR;
							// Free DMA engine
							tsc_dma_free(channel);
							// Free working buffers
							free(ref_buf);
							free(data_buf);
							free(check_buf);
							// Free kernel buffer
							tsc_kbuf_free(&buf_p);

							tm = time(0);
							ct = ctime(&tm);
							TST_LOG( tc, (logline, "\n%s->Exiting :%s", tst_id, ct));

							return( retval | TST_STS_DONE);
						}

// Acquire data -------------------------------------------------------------------------------------------
						if (destination[dest] == 0){							// PCIe - KBUF
							tsc_kbuf_read(buf_p.k_base, check_buf, size_ref);
						}
						else if (destination[dest] == 2){						// SRAM1 - DDR1
							tsc_shm_read(offset, check_buf, size_ref, 4, 0, 1);
						}
						else if (destination[dest] == 3){						// SRAM2 - DDR2
							tsc_shm_read(offset, check_buf, size_ref, 4, 0, 2);
						}
						else if (destination[dest] == 4){						// USR1
							tsc_usr_read(offset, check_buf, size_ref, 4, 0, 1);
						}
						else if (destination[dest] == 5){						// USR2
							tsc_usr_read(offset, check_buf, size_ref, 4, 0, 2);
						}

// Check data before DMA area -----------------------------------------------------------------------------
						eaddr = tst_cpu_check(check_buf, sub_offset, 0, 0xdeadface, 0);
						if(eaddr){
							TST_LOG(tc, (logline, "->Error before consistent pattern at offset %x", (uint)(eaddr - check_buf) + offset));
							retval = TST_STS_ERR;
							// Free DMA engine
							tsc_dma_free(channel);
							// Free data buffers
							free(ref_buf);
							free(data_buf);
							free(check_buf);
							// Free kernel buffer
							tsc_kbuf_free(&buf_p);

							tm = time(0);
							ct = ctime(&tm);
							TST_LOG( tc, (logline, "\n%s->Exiting :%s", tst_id, ct));

							return( retval | TST_STS_DONE);
						}

// Check data in DMA area ---------------------------------------------------------------------------------
						eaddr = tst_cpu_check(check_buf + sub_offset, sub_size, 1, offset + sub_offset, 4);
						if(eaddr){
							TST_LOG(tc, (logline, "->Error in consistent pattern area at offset %x", (uint)(eaddr - check_buf) + offset));
							retval = TST_STS_ERR;
							// Free DMA engine
							tsc_dma_free(channel);
							// Free data buffers
							free(ref_buf);
							free(data_buf);
							free(check_buf);
							// Free kernel buffer
							tsc_kbuf_free(&buf_p);

							tm = time(0);
							ct = ctime(&tm);
							TST_LOG( tc, (logline, "\n%s->Exiting :%s", tst_id, ct));

							return( retval | TST_STS_DONE);
						}

// Check data after DMA area ------------------------------------------------------------------------------
						eaddr = tst_cpu_check(check_buf + sub_offset + sub_size, size_ref - sub_size - sub_offset, 0, 0xdeadface, 0);
						if(eaddr){
							TST_LOG(tc, (logline, "->Error after consistent pattern at offset %x", (uint)(eaddr - check_buf) + offset));
							retval = TST_STS_ERR;
							// Free DMA engine
							tsc_dma_free(channel);
							// Free data buffers
							free(ref_buf);
							free(data_buf);
							free(check_buf);
							// Free kernel buffer
							tsc_kbuf_free(&buf_p);

							tm = time(0);
							ct = ctime(&tm);
							TST_LOG( tc, (logline, "\n%s->Exiting :%s", tst_id, ct));

							return( retval | TST_STS_DONE);
						}

						TST_LOG(tc, (logline, "                -> OK\r"));

						// Slide size
						sub_size = sub_size_ref + ((i & 0xf00) >> 5);
						// Slide offset
						sub_offset = sub_offset_ref + ((i & 0xf0)  >> 1);
					}
				}
			}
		}
		// Free DMA engine
		tsc_dma_free(channel);
	}

	// Free kernel buffer
	tsc_kbuf_free(&buf_p);
	// Free data buffer
	free(ref_buf);
	free(data_buf);
	free(check_buf);

	tm = time(0);
	ct = ctime(&tm);
	TST_LOG( tc, (logline, "\n%s->Exiting :%s", tst_id, ct));

	return( retval | TST_STS_DONE);
}

// DMA test
int tst_20(struct tst_ctl *tc){
	return(tst_dma(tc, "Tst:20"));
}
