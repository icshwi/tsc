/*=========================< begin file & file header >=======================
 *  References
 *  
 *    filename : irqlib.h
 *    author   : JFG
 *    company  : IOxOS
 *    creation : july 30,2015
 *    version  : 0.0.1
 *
 *----------------------------------------------------------------------------
 *  Description
 *
 *    This file contain the declarations of all exported functions define in
 *    irqlib.c
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

#ifndef _H_IRQLIB
#define _H_IRQLIB

int tsc_irq_register( struct tsc_device *ifc, int src, void (* func)( struct tsc_device *, int, void *), void *arg);
void tsc_irq_unregister( struct tsc_device *ifc, int src);
int tsc_irq_check_busy( struct tsc_device *ifc, int src);
void tsc_irq_spurious( struct tsc_device *p, int src, void *arg);
int tsc_irq_mask( struct tsc_device *ifc, int op, int src);

#endif /*  _H_IRQLIB */

/*================================< end file >================================*/
