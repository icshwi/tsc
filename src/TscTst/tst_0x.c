/*=========================< begin file & file header >=======================
 *  References
 *
 *    filename : tst_0x.c
 *    author   : JFG, XP
 *    company  : IOxOS
 *    creation : May 12,2016
 *    version  : 0.0.1
 *
 *----------------------------------------------------------------------------
 *  Description
 *
 *	  Transfers test with local board
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
char *ident="        ";

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * Function name : tst_config
 * Prototype     : int
 * Parameters    : test control structure, test ID
 * Return        : Done or Error
 *
 *----------------------------------------------------------------------------
 * Description   : Get driver information
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
 * Function name : tst_shm_mem_pmem
 * Prototype     : int
 * Parameters    : test control structure, test ID, mode
 * Return        : Done or Error
 *
 *----------------------------------------------------------------------------
 * Description   : Read write SHM0 from CPU with MEM | PMEM
 *
 *++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
int tst_shm_mem_pmem(struct tst_ctl *tc, int mode, char *tst_id){
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
	int i               = 0;

	tm = time(0);
	ct = ctime(&tm);

	if(mode == 0){
		TST_LOG( tc, (logline, "%s->Entering:%s\n", tst_id, ct));
		TST_LOG( tc, (logline, "%s->Executing Read / Write SHM0 MEM\n", tst_id));
	}
	else if (mode == 1){
		TST_LOG( tc, (logline, "%s->Entering:%s\n", tst_id, ct));
		TST_LOG( tc, (logline, "%s->Executing Read / Write SHM0 PMEM\n", tst_id));
	}

	fd = tc->fd;

	size_ref = tc->at->shm_size_0;
	offset   = tc->at->shm_offset_0;

	// Map local SHH
	memset(&map_loc_win, sizeof(map_loc_win), 0);

	map_loc_win.req.rem_addr   = 0;
	map_loc_win.req.loc_addr   = 0;
	if(mode == 0){
		map_loc_win.req.size       = 0x10000;
		map_loc_win.req.mode.sg_id = MAP_ID_MAS_PCIE_MEM;
	} else if(mode == 1){
		map_loc_win.req.size       = 0x100000;
		map_loc_win.req.mode.sg_id = MAP_ID_MAS_PCIE_PMEM;
	}
	map_loc_win.req.mode.space = MAP_SPACE_SHM;
	map_loc_win.req.mode.flags = 0;

	retval = tsc_map_alloc(&map_loc_win);
	if(retval < 0){
		TST_LOG( tc, (logline, "->Error in mapping SHM"));
		retval = TST_STS_ERR;
	}

	uaddr = mmap(NULL, map_loc_win.req.size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, map_loc_win.req.loc_addr + offset);
	if(uaddr == MAP_FAILED){
	    TST_LOG( tc, (logline, "->Error MAP FAILED\n"));
	    retval = TST_STS_ERR;
	    goto ERROR;
	}

	// Generate test with sliding size and offset
	for(i = 0; i < 0x1000; i++){
		TST_LOG(tc, (logline, "%s->Executing: iteration:%4d size:%05x offset:%05x", tst_id, i++, sub_size, sub_offset));

		// Initialize whole memory area with reference pattern 0xdeadface
		// Write into local SHM0
		tst_cpu_fill(uaddr, size_ref, 0, ref_pattern, 0);

		usleep(1000);

		// Check for errors
		eaddr = tst_cpu_check(uaddr, size_ref, 0, ref_pattern, 0);
		if(eaddr){
			TST_LOG( tc, (logline, "->Error reference patternt at offset %x", (uint)(eaddr - uaddr) + offset));
			retval = TST_STS_ERR;
			break;
		}

		// Initialize consistent sub memory area with consistent data
		// Write into SHM0
		tst_cpu_fill(uaddr + sub_offset, sub_size, 1, offset + sub_offset, 4);

		usleep(1000);

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
	}

ERROR:

	retval = munmap(uaddr, size);
	if(retval < 0){
		printf("Error tsc unmap uaddr ! \n");
	}

	retval = tsc_map_free(&map_loc_win);
	if(retval < 0){
		printf("Error tsc map local free! \n");
	}

	tm = time(0);
	ct = ctime(&tm);
	TST_LOG( tc, (logline, "\n%s->Exiting :%s", tst_id, ct));

	return(retval | TST_STS_DONE);
}

int tst_02(struct tst_ctl *tc){
	return(tst_shm_mem_pmem(tc, 0, "Tst:02"));
}

int tst_03(struct tst_ctl *tc){
	return(tst_shm_mem_pmem(tc, 1, "Tst:03"));
}

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * Function name : tst_kbuf
 * Prototype     : int
 * Parameters    : test control structure, test ID
 * Return        : Done or Error
 *
 *----------------------------------------------------------------------------
 * Description   : Read write KBUF0 from CPU
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
	    TST_LOG( tc, (logline, "->Error allocatint kernel buffer\n"));
	    retval = TST_STS_ERR;
	    goto ERROR;
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

		usleep(1000);

		// Write local KBUF0 with consistent data
		tsc_kbuf_write(buf_p.k_base + sub_offset, data_buf + sub_offset, sub_size);

		usleep(1000);

		// Read local KBUF0 with to check data
		tsc_kbuf_read(buf_p.k_base, check_buf, size_ref);

		usleep(1000);

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
	}

	tm = time(0);
	ct = ctime(&tm);
	TST_LOG( tc, (logline, "\n%s->Exiting :%s", tst_id, ct));

ERROR:

	free(ref_buf);
	free(data_buf);
	free(check_buf);

	tsc_kbuf_free(&buf_p);

	return( retval | TST_STS_DONE);
}

int tst_05(struct tst_ctl *tc){
	return(tst_kbuf(tc, "Tst:05"));
}
