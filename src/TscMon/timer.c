/*=========================< begin file & file header >=======================
 *  References
 *  
 *    filename : timer.c
 *    author   : JFG, XP
 *    company  : IOxOS
 *    creation : Sept 15,2015
 *
 *----------------------------------------------------------------------------
 *  Description
 *
 *     That file contains a set of function called by TscMon to handle the
 *     TSC TIMER interface.
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

#ifndef lint
static char *rcsid = "$Id: timer.c,v 1.1 2015/12/18 13:12:01 ioxos Exp $";
#endif

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

extern int tsc_fd;

char *
timer_rcsid()
{
  return( rcsid);
}

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * Function name : tsc_timer
 * Prototype     : int
 * Parameters    : pointer to command parameter list
 * Return        : ok  if command executed
 *                 nok if error
 *----------------------------------------------------------------------------
 * Description   : perform timer operation
 *
 *++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

int tsc_timer( struct cli_cmd_para *c){
	int cnt = c->cnt;

	if(cnt--) {
		// Start timer
		if( !strcmp( "start", c->para[0])) {
			tsc_timer_start(tsc_fd, TIMER_BASE_1000 | TIMER_100MHZ, 0);
			return( TSC_OK);
		}
		// Restart timer
		else if( !strcmp( "restart", c->para[0])) {
			tsc_timer_restart(tsc_fd);
			return( TSC_OK);
		}
		// Stop timer
		else if( !strcmp( "stop", c->para[0])) {
			tsc_timer_stop(tsc_fd);
			return( TSC_OK);
		}
		// Read timer
		else if( !strcmp( "read", c->para[0])) {
			struct tsc_time tm;

			tsc_timer_read(tsc_fd, &tm);
			printf("current timer value : %d.%06d msec\n", tm.msec, (tm.usec&TIMER_UTIME_MASK)*10);
			return( TSC_OK);
		}
		// Read timer in date format
		else if( !strcmp( "date", c->para[0])) {
			struct tsc_time tm;
			int hh,mm,ss, ms;
			int sec;

			tsc_timer_read(tsc_fd, &tm);

			hh  = tm.msec/3600;
			mm  = tm.msec/60;
			ss  = tm.msec/1000;
			ms  = tm.msec%1000;
			sec = tm.msec/1000;
			hh  = sec/3600;
			sec = sec - (hh*3600);
			mm  = sec/60;
			ss  = sec - (mm*60);
			printf("current timer value : %02dh:%02dm:%02ds:%03dms\n", hh, mm, ss, ms);
			return( TSC_OK);
		}
		else{
			printf("Not enough arguments -> usage:\n");
			tsc_print_usage(c);
			return( TSC_ERR);
		}
	}
    printf("Not enough arguments -> usage:\n");
    tsc_print_usage(c);
	return( TSC_ERR);
}
