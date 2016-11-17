/*=========================< begin file & file header >=======================
 *  References
 *  
 *    filename : tscslvioctl.h
 *    author   : JFG
 *    company  : IOxOS
 *    creation : Sept 30,2015
 *    version  : 0.0.1
 *
 *----------------------------------------------------------------------------
 *  Description
 *
 *    This file contain the declarations for the ioctl commands supported
 *    by the IFC1211 slave driver.
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


#ifndef _H_TSCSLVIOCTL
#define _H_TSCSLVIOCTL

#define TSC_IOCTL_SLV            0x00040000
#define TSC_IOCTL_SLV_MAP_SET    (TSC_IOCTL_SLV | 0x1)
#define TSC_IOCTL_SLV_MAP_GET    (TSC_IOCTL_SLV | 0x2)


#endif /*  _H_TSCSLVIOCTL */

/*================================< end file >================================*/

