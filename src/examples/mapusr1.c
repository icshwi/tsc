/*=========================< begin file & file header >=======================
 *  References
 *
 *    filename : mapusr1.c
 *    author   : JFG, XP
 *    company  : IOxOS
 *    creation : june 30,2008
 *
 *----------------------------------------------------------------------------
 *  Description
 *
 *    Map and read / write USER1 space.
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
#include <pty.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <sys/mman.h>
#include "../../include/tscioctl.h"
#include "../../include/tsculib.h"

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * Function name : main
 * Prototype     : void
 * Parameters    : arc, argv
 * Return        : void
 *----------------------------------------------------------------------------
 * Description   : test usr mapping
 *
 *++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

int main( int argc, char *argv[]){
	int *mybuffer    = calloc(1024*1024, 1);
	int status        = 0;
	int addr          = 0;
	int PP_OFFSET     = 0x100000;
	int i             = 0;
	int size          = 0x1000;

	// Init TSC
	tsc_init();

	// Use this structure just to configure the MODE (taken from TscMon code)
	struct tsc_ioctl_rdwr tsc_read_s;

	tsc_read_s.m.ads   = (char) RDWR_MODE_SET_DS(0x44, RDWR_SIZE_SHORT);
	tsc_read_s.m.space = RDWR_SPACE_USR1;
	tsc_read_s.m.swap  = RDWR_SWAP_QS;
	tsc_read_s.m.am    = 0x0;

	// Set src_addr with the desired address value (PP_OFFSET is 0x100000 and addr comes from upstream calls)
	ulong src_addr = PP_OFFSET + addr;

	// Execute the read operation and fill the previous allocated “mybuffer” with data
	status = tsc_read_blk(src_addr, (char*) mybuffer, size, tsc_read_s.mode);

	// Check status
	fprintf(stderr, "tsc_read_blk returned %d \n", status);
	if (status) {
	    printf("tsc_blk_read() returned %d\n", status);
	    return status;
	}

	// Print buffer
	for (i = 0; i < 32; i++){
		printf("mybuffer %x \n",  mybuffer[i]);
	}

	// Exit TSC
	tsc_exit();
}
