/*=========================< begin file & file header >=======================
 *  References
 *  
 *    filename : timer.c
 *    author   : JFG
 *    company  : IOxOS
 *    creation : Sept 15,2015
 *    version  : 0.0.1
 *
 *----------------------------------------------------------------------------
 *  Description
 *
 *     That file contains a set of function called by TscMon to handle the
 *     IFC1211 TIMER interface.
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

char *
timer_rcsid()
{
  return( rcsid);
}

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * Function name : tsc_timer
 * Prototype     : int
 * Parameters    : pointer to command parameter list
 * Return        : TSC_OK  if command executed
 *                 TSC_ERR if error
 *----------------------------------------------------------------------------
 * Description   : perform TIMER operation
 *
 *++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
int 
tsc_timer( struct cli_cmd_para *c)
{
  if( !strcmp( "start", c->para[0])) 
  {
    tsc_timer_start( TIMER_BASE_1000 | TIMER_100MHZ, 0);
    return( TSC_OK);
  }
  else if( !strcmp( "restart", c->para[0])) 
  {
    tsc_timer_restart();
    return( TSC_OK);
  }
  else if( !strcmp( "stop", c->para[0])) 
  {
    tsc_timer_stop();
    return( TSC_OK);
  }
  else if( !strcmp( "read", c->para[0])) 
  {
    struct tsc_time tm;

    tsc_timer_read( &tm);
    printf("current timer value : %d.%06d msec\n", tm.msec, (tm.usec&TIMER_UTIME_MASK)*10);
    return( TSC_OK);
  }
  else if( !strcmp( "date", c->para[0])) 
  {
    struct tsc_time tm;
    int hh,mm,ss, ms;
    int sec;

    tsc_timer_read( &tm);
    hh =  tm.msec/3600;
    mm = tm.msec/60;
    ss = tm.msec/1000;
    ms = tm.msec%1000;
    sec = tm.msec/1000;
    hh =  sec/3600;
    sec = sec - (hh*3600);
    mm = sec/60;
    ss = sec - (mm*60);
    printf("current timer value : %02d:%02d:%02d.%03d\n", hh, mm, ss, ms);
    return( TSC_OK);
  }
  else
  {
    return( TSC_ERR);
  }
}
