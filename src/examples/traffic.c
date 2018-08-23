/*=========================< begin file & file header >=======================
 *  References
 *
 *    filename : traffic.c
 *    author   : XP
 *    company  : IOxOS
 *    creation : August 23,2018
 *
 *----------------------------------------------------------------------------
 *  Description
 *
 *    Traffic generator
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

/*
 * Mapping
 *
 * SRAM1&2 ---|0x4000   - 0x8000|---   @0x4000
 * DDR1&2  ---|0x100000 - 0x200000|--- @0x100000
 * USR1&2  ---|0x100000 - 0x200000|--- @0x10000
 */


/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * Function name : main
 * Prototype     : void
 * Parameters    : arc, argv
 * Return        : void
 *----------------------------------------------------------------------------
 * Description   : test shm mapping
 *
 *++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

int main(int argc, char *argv[]){
	struct tsc_ioctl_map_win map_loc_win;
	char * uaddr        = NULL;
	char * eaddr        = NULL;
	int fd              = -1;
	int retval          = 0;
	int offset          = 0;
	int size            = 0;
	int win_size        = 0;
	int i               = 0;
	int agent           = 0;
	int loop            = 0;

	if (argc != 3) {
		printf("Not enough parameters..\n");
		printf("Usage: traffic <number of iteration> <agent>      \n");
		printf("   <number of iteration> : from 1 to 2147483646   \n");
		printf("   <agent>               : 0 SRAM1                \n");
		printf("                           1 SRAM2                \n");
		printf("                           2 DDR1                 \n");
		printf("                           3 DDR2                 \n");
		printf("                           4 USR1                 \n");
		printf("                           5 USR2                 \n");
		return -1;
	}

	if (sscanf(argv[1], "%i", &loop) != 1) {
		printf("Bad value..\n");
		printf("Usage: traffic <number of iteration> <agent>      \n");
		printf("   <number of iteration> : from 1 to 2147483646   \n");
		printf("   <agent>               : 0 SRAM1                \n");
		printf("                           1 SRAM2                \n");
		printf("                           2 DDR1                 \n");
		printf("                           3 DDR2                 \n");
		printf("                           4 USR1                 \n");
		printf("                           5 USR2                 \n");
		return -1;
	}

	if (sscanf(argv[2], "%i", &agent) != 1) {
		printf("Bad value..\n");
		printf("Usage: traffic <number of iteration> <agent>      \n");
		printf("   <number of iteration> : from 1 to 2147483646   \n");
		printf("   <agent>               : 0 SRAM1                \n");
		printf("                           1 SRAM2                \n");
		printf("                           2 DDR1                 \n");
		printf("                           3 DDR2                 \n");
		printf("                           4 USR1                 \n");
		printf("                           5 USR2                 \n");
		return -1;
	}

	printf("\n");
	printf("----------------- \n");
	printf("Traffic Generator \n");
	printf("----------------- \n");
	printf("\n");
	printf("MApping \n");
	printf("SRAM1&2 ---|0x4000   - 0x8000|---   @0x4000 \n");
	printf("DDR1&2  ---|0x100000 - 0x200000|--- @0x100000 \n");
	printf("USR1&2  ---|0x100000 - 0x200000|--- @0x10000 \n ");
	printf("\n");

	// Init TSC
	fd = tsc_init();
	if (fd == -1){
		printf("Driver init failed");
		return -1;
	}

	if (agent == 0) {
		printf("iteration is %i traffic on agent SRAM1 \n", loop);

		// Map SRAM1
		memset(&map_loc_win, sizeof(map_loc_win), 0);

		offset   = 0x4000;
		size     = 0x4000;
		win_size = 0x10000;

		map_loc_win.req.rem_addr   = offset;
		map_loc_win.req.loc_addr   = 0;
		map_loc_win.req.size       = win_size;
		map_loc_win.req.mode.sg_id = MAP_ID_MAS_PCIE_MEM;
		map_loc_win.req.mode.space = MAP_SPACE_SHM1;
		map_loc_win.req.mode.flags = 0;

		retval = tsc_map_alloc(&map_loc_win);
		if(retval < 0){
			printf("Error in mapping memory \n");
			tsc_exit();
			return -1;
		}
	}
	else if (agent == 1){
		printf("iteration is %i traffic on agent SRAM2 \n", loop);

		// Map SRAM2
		memset(&map_loc_win, sizeof(map_loc_win), 0);

		offset   = 0x4000;
		size     = 0x4000;
		win_size = 0x10000;

		map_loc_win.req.rem_addr   = offset;
		map_loc_win.req.loc_addr   = 0;
		map_loc_win.req.size       = win_size;
		map_loc_win.req.mode.sg_id = MAP_ID_MAS_PCIE_MEM;
		map_loc_win.req.mode.space = MAP_SPACE_SHM2;
		map_loc_win.req.mode.flags = 0;

		retval = tsc_map_alloc(&map_loc_win);
		if(retval < 0){
			printf("Error in mapping memory \n");
			tsc_exit();
			return -1;
		}
	}
	else if (agent == 2){
		printf("iteration is %i traffic on agent DDR1 \n", loop);

		// Map SHM1
		memset(&map_loc_win, sizeof(map_loc_win), 0);

		offset   = 0x100000;
		size     = 0x10000;
		win_size = 0x100000;

		map_loc_win.req.rem_addr   = offset;
		map_loc_win.req.loc_addr   = 0;
		map_loc_win.req.size       = win_size;
		map_loc_win.req.mode.sg_id = MAP_ID_MAS_PCIE_MEM;
		map_loc_win.req.mode.space = MAP_SPACE_SHM1;
		map_loc_win.req.mode.flags = 0;

		retval = tsc_map_alloc(&map_loc_win);
		if(retval < 0){
			printf("Error in mapping memory \n");
			tsc_exit();
			return -1;
		}
	}
	else if (agent == 3){
		printf("iteration is %i traffic on agent DDR2 \n", loop);

		// Map SHM2
		memset(&map_loc_win, sizeof(map_loc_win), 0);

		offset   = 0x100000;
		size     = 0x10000;
		win_size = 0x100000;

		map_loc_win.req.rem_addr   = offset;
		map_loc_win.req.loc_addr   = 0;
		map_loc_win.req.size       = win_size;
		map_loc_win.req.mode.sg_id = MAP_ID_MAS_PCIE_MEM;
		map_loc_win.req.mode.space = MAP_SPACE_SHM2;
		map_loc_win.req.mode.flags = 0;

		retval = tsc_map_alloc(&map_loc_win);
		if(retval < 0){
			printf("Error in mapping memory \n");
			tsc_exit();
			return -1;
		}
	}
	else if (agent == 4){
		printf("iteration is %i traffic on agent USR1 \n", loop);

		// Map USR1
		memset(&map_loc_win, sizeof(map_loc_win), 0);

		offset = 0x100000;
		size   = 0x10000;
		win_size = 0x100000;

		map_loc_win.req.rem_addr   = offset;
		map_loc_win.req.loc_addr   = 0;
		map_loc_win.req.size       = win_size;
		map_loc_win.req.mode.sg_id = MAP_ID_MAS_PCIE_MEM;
		map_loc_win.req.mode.space = MAP_SPACE_USR1;
		map_loc_win.req.mode.flags = 0;

		retval = tsc_map_alloc(&map_loc_win);
		if(retval < 0){
			printf("Error in mapping memory \n");
			tsc_exit();
			return -1;
		}
	}
	else if (agent == 5){
		printf("iteration is %i traffic on agent USR2 \n", loop);

		// Map USR2
		memset(&map_loc_win, sizeof(map_loc_win), 0);

		offset = 0x100000;
		size   = 0x10000;
		win_size = 0x100000;

		map_loc_win.req.rem_addr   = offset;
		map_loc_win.req.loc_addr   = 0;
		map_loc_win.req.size       = win_size;
		map_loc_win.req.mode.sg_id = MAP_ID_MAS_PCIE_MEM;
		map_loc_win.req.mode.space = MAP_SPACE_USR2;
		map_loc_win.req.mode.flags = 0;

		retval = tsc_map_alloc(&map_loc_win);
		if(retval < 0){
			printf("Error in mapping memory \n");
			tsc_exit();
			return -1;
		}
	}
	else {
		printf("Bad value..\n");
		printf("Usage: traffic <number of iteration> <agent>      \n");
		printf("   <number of iteration> : from 1 to 2147483646   \n");
		printf("   <agent>               : 0 SRAM1                \n");
		printf("                           1 SRAM2                \n");
		printf("                           2 DDR1                 \n");
		printf("                           3 DDR2                 \n");
		printf("                           4 USR1                 \n");
		printf("                           5 USR2                 \n");
		tsc_exit();
		return -1;
	}

	uaddr = mmap(NULL, map_loc_win.req.size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, map_loc_win.req.loc_addr);
	if(uaddr == MAP_FAILED){
		printf("Error MAP FAILED \n");
		retval = munmap(uaddr, size);
		if(retval < 0){
			printf("Error tsc unmap uaddr ! \n");
		}

		retval = tsc_map_free(&map_loc_win);
		if(retval < 0){
			printf("Error tsc map local free! \n");
		}
		tsc_exit();
		return(retval);
	}

	// Generate test with sliding size and offset
	for(i = 0 ; i < loop; i++){

		// Write into agent : 0..4..8..c
		tst_cpu_fill(uaddr, size, 1, offset, 4);

		// Check for errors in pattern
		eaddr = tst_cpu_check(uaddr, size, 1, offset, 4);
		if(eaddr){
			printf("->Error in pattern at iteration %i \n", i);
			retval = -1;
			break;
		}
		else {
			printf("#%i OK \n", i);
		}
	}

	retval = munmap(uaddr, size);
	if(retval < 0){
		printf("Error tsc unmap uaddr ! \n");
	}

	retval = tsc_map_free(&map_loc_win);
	if(retval < 0){
		printf("Error tsc map local free! \n");
	}

	tsc_exit();

	return 0;
}
