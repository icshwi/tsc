/*=========================< begin file & file header >=======================
 *  References
 *  
 *    filename : ioctllib.h
 *    author   : JFG
 *    company  : IOxOS
 *    creation : july 30,2015
 *    version  : 0.0.1
 *
 *----------------------------------------------------------------------------
 *  Description
 *
 *    This file contain the declarations of all exported functions define in
 *    ioctllib.c
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


#ifndef _H_IOCTLLIB
#define _H_IOCTLLIB

int  ioctl_csr(struct tsc_device *ifc, unsigned int cmd, unsigned long arg);
int  ioctl_map(struct tsc_device *ifc, unsigned int cmd, unsigned long arg);
int  ioctl_rdwr(struct tsc_device *ifc, unsigned int cmd, unsigned long arg);
int  ioctl_dma(struct tsc_device *ifc, unsigned int cmd, unsigned long arg);
int  ioctl_kbuf(struct tsc_device *ifc, unsigned int cmd, unsigned long arg);
int  ioctl_sflash(struct tsc_device *ifc, unsigned int cmd, unsigned long arg);
int  ioctl_timer(struct tsc_device *ifc, unsigned int cmd, unsigned long arg);
int  ioctl_fifo(struct tsc_device *ifc, unsigned int cmd, unsigned long arg);
int  ioctl_i2c(struct tsc_device *ifc, unsigned int cmd, unsigned long arg);
int  ioctl_semaphore(struct tsc_device *ifc, unsigned int cmd, unsigned long arg);

#endif /*  _H_IOCTLLIB */

/*================================< end file >================================*/
