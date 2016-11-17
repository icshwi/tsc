/*=========================< begin file & file header >=======================
 *  References
 *
 *    filename : semaphore.c
 *    author   : XP
 *    company  : IOxOS
 *    creation : Sept 4,2016
 *    version  : 0.0.1
 *
 *----------------------------------------------------------------------------
 *  Description
 *
 *     That file contains a set of function called by TscMon to perform
 *     semaphore operation on IFC1211 board
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

#define DEBUGno
#include <debug.h>
#include <sys/types.h>
#include <sys/time.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <cli.h>
#include <tscioctl.h>
#include <tsculib.h>
#include <aio.h>
#include <errno.h>
#include <ctype.h>
#include <time.h>
#include "TscMon.h"

// Main function for IFC1211 command
// ----------------------------------------------------------------------------------
int tsc_semaphore(struct cli_cmd_para *c){
	int cnt  = 0;
	int idx  = 0;
	cnt      = c->cnt;
	uint sts = 0;
	uint tag = 0;
	char *p;

	// Select sub command and check syntax
	if(cnt--) {
		if((!strcmp("status", c->para[0])) && (c->cnt == 1)) {
			tsc_semaphore_status(&sts);
			printf("SEMAPHORE global status = 0x%08x\n", sts);
			printf("SEMAPHORE#0             : %01x\n", (sts & 0x00000001));
			printf("SEMAPHORE#1             : %01x\n", (sts & 0x00000002) >> 1);
			printf("SEMAPHORE#2             : %01x\n", (sts & 0x00000004) >> 2);
			printf("SEMAPHORE#3             : %01x\n", (sts & 0x00000008) >> 3);
			printf("SEMAPHORE#4             : %01x\n", (sts & 0x00000010) >> 4);
			printf("SEMAPHORE#5             : %01x\n", (sts & 0x00000020) >> 5);
			printf("SEMAPHORE#6             : %01x\n", (sts & 0x00000040) >> 6);
			printf("SEMAPHORE#7             : %01x\n", (sts & 0x00000080) >> 7);
			printf("SEMAPHORE#8             : %01x\n", (sts & 0x00000100) >> 8);
			printf("SEMAPHORE#9             : %01x\n", (sts & 0x00000200) >> 9);
			printf("SEMAPHORE#A             : %01x\n", (sts & 0x00000400) >> 10);
			printf("SEMAPHORE#B             : %01x\n", (sts & 0x00000800) >> 11);
			printf("SEMAPHORE#C             : %01x\n", (sts & 0x00001000) >> 12);
			printf("SEMAPHORE#D             : %01x\n", (sts & 0x00002000) >> 13);
			printf("SEMAPHORE#E             : %01x\n", (sts & 0x00004000) >> 14);
			printf("SEMAPHORE#F             : %01x\n", (sts & 0x00008000) >> 15);
			return(0);
		}
		else if((!strcmp("release", c->para[0])) && (c->cnt == 2)){
			if(!cnt){
				printf("Bad parameter! Type \"? semaphore\" for help \n");
				return(-1);
			}
			idx = strtoul( c->para[1], &p, 16);
			if((idx < 0) || (idx > 15)){
				printf("Bad semaphore index : %x \n", idx);
				printf("Available index is 0 to F \n");
				return(-1);
			}
			else{
				tsc_semaphore_release(idx);
				printf("SEMAPHORE#%d released \n", idx);
				return(0);
			}
		}
		else if((!strcmp("get", c->para[0])) && (c->cnt == 3)){
			if(!cnt){
				printf("Bad parameter! Type \"? semaphore\" for help \n");
				return(-1);
			}
			idx = strtoul(c->para[1], &p, 16);
			if((idx < 0) || (idx > 15)){
				printf("Bad semaphore index : %x \n", idx);
				printf("Available index is 0 to F \n");
				return(-1);
			}
			else{
				tag = strtoul(c->para[2], &p, 16);
				if(tsc_semaphore_get(idx, &tag) == 3){
					printf("Semaphore not available !\n");
				}
				else{
					printf("Semaphore acquired ! \n");
				}
				return(0);
			}
		}
		else {
			printf("Bad parameter! Type \"? semaphore\" for help \n");
			return(-1);
		}
	}
	return 0;
}
