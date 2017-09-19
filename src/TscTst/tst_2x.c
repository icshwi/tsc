/*=========================< begin file & file header >=======================
 *  References
 *
 *    filename : tst_3x.c
 *    author   : JFG, XP
 *    company  : IOxOS
 *    creation : May 12,2016
 *    version  : 0.0.1
 *
 *----------------------------------------------------------------------------
 *  Description
 *
 *	  Local DMA tests
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

struct tsc_ioctl_dma_req req_p;   // dma request structure

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * Function name : dma_configure_local
 * Prototype     : void
 * Parameters    : dma channel, source, destination, size, space src, space dest
 * Return        : Done or Error
 *
 *----------------------------------------------------------------------------
 * Description   : Dma configure [channel, src, des, size, space_src, space_des]
 *
 *++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
void dma_configure_local(int channel, int src, int des, int size, int space_src, int space_des){
	req_p.des_addr   = des;
	req_p.des_space  = space_des;
	req_p.des_mode   = 0;
	req_p.src_addr   = src;
	req_p.src_space  = space_src;
	req_p.src_mode   = 0;
	req_p.size       = size;
	// User dma channel 0 and 1
	if (channel == 0){
		req_p.start_mode = (char)DMA_START_CHAN(DMA_CHAN_0);
	}
	else if (channel == 1){
		req_p.start_mode = (char)DMA_START_CHAN(DMA_CHAN_1);
	}
	req_p.end_mode   = 0;
	req_p.intr_mode  = DMA_INTR_ENA;
	req_p.wait_mode  = DMA_WAIT_INTR | DMA_WAIT_1S | (5<<4);
}

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * Function name : tst_dma_kbuf_shm_local
 * Prototype     : int
 * Parameters    : test control structure, test ID, direction of transfer
 * Return        : Done or Error
 *
 *----------------------------------------------------------------------------
 * Description   : DMA test between KBUF0 to SHM0
 *                 direction: 0 = kbuf0 -> shm0, 1 = shm0 -> kbuf0
 *
 *++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
int tst_dma_kbuf_shm_local(struct tst_ctl *tc, char *tst_id, int direction){
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
	int dma_channel    = 0; // 2 DMAs channels : 0 an 1

	tm = time(0);
	ct = ctime(&tm);

	TST_LOG( tc, (logline, "%s->Entering:%s\n", tst_id, ct));

	if(direction == 0){
		TST_LOG( tc, (logline, "%s->Executing DMA from KBUF0 to SHM0\n", tst_id));
	}
	else if(direction == 1){
		TST_LOG( tc, (logline, "%s->Executing DMA from SHM0 to KBUF0\n", tst_id));
	}

	// Allocate kernel buffer suitable for DMA transfer
	buf_p.size = 0x100000; // Allocate 1 MB kernel buffer
	retval = tsc_kbuf_alloc(&buf_p);
	if(retval != 0){
	    TST_LOG( tc, (logline, "->Error allocatint kernel buffer\n"));
	    retval = TST_STS_ERR;
	    goto ERROR;
	}

	// Compute VME board offset in decoding window
	size_ref = tc->at->shm_size_0;
	offset   = tc->at->shm_offset_0;

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

	// Choose direction: 0 = kbuf0 -> shm0, 1 = shm0 -> kbuf0
	if(direction == 0){
		// Write local KBUF0 with consistent data
		tsc_kbuf_write(buf_p.k_base, data_buf, size_ref);

	}
	else if(direction == 1){
		// Write local SHM0 with consistent data
		tsc_shm_write(offset, data_buf, size_ref, 4, 0);

	}

	// Allocate DMA
	if(tsc_dma_alloc(dma_channel)){
		TST_LOG( tc, (logline, "->DMA channel %x is busy \n", dma_channel));
		retval = TST_STS_ERR;
		goto ERROR;
	}
	if(tsc_dma_alloc(dma_channel + 1)){
		TST_LOG( tc, (logline, "->DMA channel %x is busy \n", dma_channel + 1));
		retval = TST_STS_ERR;
		goto ERROR;
		}

	// Test both DMA channel
	for (dma_channel = 0; dma_channel < 2; dma_channel++){

		// Reset offset and size
		sub_size = sub_size_ref;
		sub_offset = sub_offset_ref;;

		TST_LOG( tc, (logline, "\nTesting DMA Channel #%x \n", dma_channel));
		// Generate test with sliding size and offset
		for(i = 0; i < 0x1000; i++){
			TST_LOG(tc, (logline, "%s->Executing: iteration:%4d size:%05x offset:%05x", tst_id, i++, sub_size, sub_offset));

			// Fill check buffer with 0
			tst_cpu_fill(check_buf, size_ref, 0, 0, 0);

			// Choose direction: 0 = kbuf0 -> shm0, 1 = shm0 -> kbuf0
			if(direction == 0){
				// Write SHM0 with reference pattern 0xdeadface
				tsc_shm_write(offset, ref_buf, size_ref, 4, 0);

				usleep(1000);

				// Prepare DMA [channel, src, des, size, space_src, space_des]
				dma_configure_local(dma_channel, buf_p.b_base + sub_offset, offset + sub_offset, sub_size, DMA_SPACE_PCIE, DMA_SPACE_SHM);
			}
			else if (direction == 1){
				// Write local KBUF0 with reference pattern 0xdeadface
				tsc_kbuf_write(buf_p.k_base, ref_buf, size_ref);

				usleep(1000);

				// Prepare DMA [channel, src, des, size, space_src, space_des]
				dma_configure_local(dma_channel, offset + sub_offset, buf_p.b_base + sub_offset, sub_size, DMA_SPACE_SHM, DMA_SPACE_PCIE);
			}

			// Do DMA transfer
			retval = tsc_dma_move(&req_p);

			if(retval < 0){
				TST_LOG(tc, (logline, "->DMA move error \n"));
				retval = TST_STS_ERR;
				goto ERROR;
			}

			// Choose direction: 0 = kbuf0 -> shm0, 1 = shm0 -> kbuf0
			if(direction == 0){
				// Acquire data from SHM0
				tsc_shm_read(offset, check_buf, size_ref, 4, 0);

			}
			else if(direction == 1){
				// Acquire data from KBUF0
				tsc_kbuf_read(buf_p.k_base, check_buf, size_ref);
			}

			// Check data before DMA area
			eaddr = tst_cpu_check(check_buf, sub_offset, 0, 0xdeadface, 0);
			if(eaddr){
				TST_LOG(tc, (logline, "->Error before consistent pattern at offset %x", (uint)(eaddr - check_buf) + offset));
				retval = TST_STS_ERR;
				goto ERROR;
			}

			// Check data in DMA area
			eaddr = tst_cpu_check(check_buf + sub_offset, sub_size, 1, offset + sub_offset, 4);
			if(eaddr){
				TST_LOG(tc, (logline, "->Error in consistent pattern area at offset %x", (uint)(eaddr - check_buf) + offset));
				retval = TST_STS_ERR;
				goto ERROR;
			}

			// Check data after DMA area
			eaddr = tst_cpu_check(check_buf + sub_offset + sub_size, size_ref - sub_size - sub_offset, 0, 0xdeadface, 0);
			if(eaddr){
				TST_LOG(tc, (logline, "->Error after consistent pattern at offset %x", (uint)(eaddr - check_buf) + offset));
				retval = TST_STS_ERR;
				goto ERROR;
			}

			TST_LOG(tc, (logline, "                -> OK\r"));

			// Slide size
			sub_size = sub_size_ref + ((i & 0xf00) >> 5);
			// Slide offset
			sub_offset = sub_offset_ref + ((i & 0xf0)  >> 1);
		}
	}

ERROR:

	// Free DMA engine
	tsc_dma_free(0);
	tsc_dma_free(1);

	free(ref_buf);
	free(data_buf);
	free(check_buf);

	tsc_kbuf_free(&buf_p);

	tm = time(0);
	ct = ctime(&tm);
	TST_LOG( tc, (logline, "\n%s->Exiting :%s", tst_id, ct));

	return( retval | TST_STS_DONE);
}

// DMA test : kbuf0 -> shm0
int tst_20(struct tst_ctl *tc){
	return(tst_dma_kbuf_shm_local(tc, "Tst:20", 0));
}

// DMA test : shm0 -> kbuf0
int tst_21(struct tst_ctl *tc){
	return(tst_dma_kbuf_shm_local(tc, "Tst:21", 1));
}

