/*=========================< begin file & file header >=======================
 *  References
 *  
 *    filename : i2clib.h
 *    author   : JFG
 *    company  : IOxOS
 *    creation : july 30,2015
 *    version  : 0.0.1
 *
 *----------------------------------------------------------------------------
 *  Description
 *
 *    This file contain the declarations of all exported functions define in
 *    i2clib.c
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

#ifndef _H_I2CLIB
#define _H_I2CLIB

struct i2c_ctl
{
  struct mutex i2c_lock;        /* mutex to lock I2C access                     */
  struct semaphore sem;         /* semaphore to synchronize with I2C interrput  */
};

void tsc_i2c_irq( struct tsc_device *ifc, int src, void *arg);
int tsc_i2c_reset( struct tsc_device *ifc);
int tsc_i2c_wait( struct tsc_device *ifc, int irq);
int tsc_i2c_cmd( struct tsc_device *ifc, struct tsc_ioctl_i2c *i);
int tsc_i2c_read( struct tsc_device *ifc, struct tsc_ioctl_i2c *i);
int tsc_i2c_write( struct tsc_device *ifc, struct tsc_ioctl_i2c *i);

#endif /*  _H_I2CLIB */

/*================================< end file >================================*/
