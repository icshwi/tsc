/*=========================< begin file & file header >=======================
 *  References
 *  
 *    filename : timerlib.h
 *    author   : JFG
 *    company  : IOxOS
 *    creation : Nov 30,2015
 *    version  : 0.0.1
 *
 *----------------------------------------------------------------------------
 *  Description
 *
 *    This file contain the declarations of all exported functions define in
 *    timerlib.c
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


#ifndef _H_TIMERLIB
#define _H_TIMERLIB
int tsc_timer_irq( struct ifc1211_device *ifc, int src, void *arg);
int timer_init( struct ifc1211_device *ifc);
int tsc_timer_start( struct ifc1211_device *ifc, struct tsc_ioctl_timer *tmr);
int tsc_timer_restart( struct ifc1211_device *ifc, struct tsc_time *tm);
int tsc_timer_stop( struct ifc1211_device *ifc);
int tsc_timer_read( struct ifc1211_device *ifc, struct tsc_time *tm);

#endif /*  _H_TIMERLIB */

/*================================< end file >================================*/
