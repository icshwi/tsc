/*=========================< begin file & file header >=======================
 *  References
 *
 *    filename : semaphorelib.h
 *    author   : XP
 *    company  : IOxOS
 *    creation : Sept 4,2016
 *    version  : 0.0.1
 *
 *----------------------------------------------------------------------------
 *  Description
 *
 *    This file contain the declarations of all exported functions define in
 *    semaphorelib.c
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

#ifndef _H_SEMAPHORELIB
#define _H_SEMAPHORELIB

int semaphore_status(struct ifc1211_device *ifc, struct tsc_ioctl_semaphore  *semaphore);
int semaphore_release(uint idx, void *base_shm_ptr);
int semaphore_get(uint idx, void *base_shm_ptr, uint *tag);

#endif /*  _H_SEMAPHORELIB */
