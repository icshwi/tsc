/*=========================< begin file & file header >=======================
 *  References
 *  
 *    filename : tscklib.h
 *    author   : JFG
 *    company  : IOxOS
 *    creation : june 30,2008
 *    version  : 0.0.1
 *
 *----------------------------------------------------------------------------
 *  Description
 *
 *    This file contain the declarations of all exported functions define in
 *    tscklib.c
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


#ifndef _H_TSCKLIB
#define _H_TSCKLIB

unsigned int tsc_msb32( unsigned int x);
int tsc_dev_init( struct ifc1211_device *ifc);
void tsc_dev_exit( struct ifc1211_device *ifc);
int tsc_irq_init( struct ifc1211_device *ifc);
void tsc_irq_exit( struct ifc1211_device *ifc);
int tsc_map_mas_init( struct ifc1211_device *ifc);
void tsc_map_mas_exit( struct ifc1211_device *ifc);
int tsc_map_slv_init( struct ifc1211_device *ifc);
void tsc_map_slv_exit( struct ifc1211_device *ifc);
int tsc_map_read( struct ifc1211_device *ifc, struct tsc_ioctl_map_ctl *m);
int tsc_map_clear( struct ifc1211_device *ifct, struct tsc_ioctl_map_ctl *m);
int tsc_csr_op( struct ifc1211_device *ifc, struct tsc_ioctl_csr_op *csr_op);
int tsc_shm_init( struct ifc1211_device *ifc, int idx);
void tsc_shm_exit( struct ifc1211_device *ifc, int idx);
int tsc_kbuf_alloc( struct ifc1211_device *ifc, struct tsc_ioctl_kbuf_req *r);
int tsc_kbuf_free( struct ifc1211_device *ifc, struct tsc_ioctl_kbuf_req *r);
int tsc_sflash_init( struct ifc1211_device *ifc);
int tsc_dma_init( struct ifc1211_device *ifc);
void tsc_dma_exit( struct ifc1211_device *ifc);
int tsc_timer_init( struct ifc1211_device *ifc);
int tsc_i2c_init( struct ifc1211_device *ifc);
void tsc_i2c_exit( struct ifc1211_device *ifc);
int tsc_semaphore_release(struct ifc1211_device *ifc, struct tsc_ioctl_semaphore *semaphore);
int tsc_semaphore_get(struct ifc1211_device *ifc, struct tsc_ioctl_semaphore *semaphore);

#include "irqlib.h"
#include "maplib.h"
#include "mapmaslib.h"
#include "mapslvlib.h"
#include "rdwrlib.h"
#include "sflashlib.h"
#include "dmalib.h"
#include "timerlib.h"
#include "fifolib.h"
#include "i2clib.h"
#include "semaphorelib.h"

#endif /*  _H_TSCKLIB */

/*================================< end file >================================*/
