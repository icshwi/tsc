/*=========================< begin file & file header >=======================
 *  References
 *
 *    filename : mapshm1.c
 *    author   : JFG, XP
 *    company  : IOxOS
 *    creation : june 30,2008
 *
 *----------------------------------------------------------------------------
 *  Description
 *
 *    Map and read / write SHM1 space.
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
 * Description   : test shm mapping
 *
 *++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

main(int argc, char *argv[]){
	struct tsc_ioctl_map_win map_win;
	int             retval          = 0;
	unsigned int    *buf_ddr 		= NULL;	    // Buffer mapped directly in DDR3 area
    unsigned int    fd              = 0;

	// Init TSC
	fd = tsc_init();

	// Map DDR3 memory region --
	buf_ddr = NULL;

	// Fill mapping structure
	memset(&map_win, sizeof(map_win), 0);
	map_win.req.rem_addr   = 0x100000;				// Address in SHM1
	map_win.req.loc_addr   = 0;						// Local address
	map_win.req.size       = 0x40;					// Mapping size
	map_win.req.mode.sg_id = MAP_ID_MAS_PCIE_MEM;	// Memory space
	map_win.req.mode.space = MAP_SPACE_SHM1; 		// SHM #1
	map_win.req.mode.flags = 0;

	// Allocate a mapping
	retval = tsc_map_alloc(&map_win);
	if(retval < 0){
		printf("Error in mapping SHM1 \n");
	}

	// Map in user space ressource
	buf_ddr = mmap(NULL, map_win.req.size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, map_win.req.loc_addr);
	if(buf_ddr == MAP_FAILED){
		printf("Error MAP FAILED \n");
	}

	// Write / read SHM1 (DDR3 memory)
	*buf_ddr = 0xdeadface;
	printf("Word written : %x \n", buf_ddr[0]);

	// TSC exit
	tsc_exit();
}
