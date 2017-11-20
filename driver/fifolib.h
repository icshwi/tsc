/*=========================< begin file & file header >=======================
 *  References
 *  
 *    filename : fifolib.h
 *    author   : JFG
 *    company  : IOxOS
 *    creation : july 30,2015
 *    version  : 0.0.1
 *
 *----------------------------------------------------------------------------
 *  Description
 *
 *    This file contain the declarations of all exported functions define in
 *    fifolib.c
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
 *=============================< end file header >============================*/


#ifndef _H_FIFOLIB
#define _H_FIFOLIB

int tsc_fifo_init(struct tsc_device *ifc, int idx, int mode);
int tsc_fifo_wait_ef(struct tsc_device *ifc, struct tsc_ioctl_fifo *fifo);
int tsc_fifo_wait_ff(struct tsc_device *ifc, struct tsc_ioctl_fifo *fifo);
int tsc_fifo_status(struct tsc_device *ifc, int idx, int *sts);
int tsc_fifo_clear(struct tsc_device *ifc, int idx);
int tsc_fifo_read(struct tsc_device *ifc, int idx, int *data, int cnt, int *sts);
int tsc_fifo_write(struct tsc_device *ifc, int idx, int *data, int cnt, int *sts);

#endif /*  _H_FIFOLIB */

/*================================< end file >================================*/
