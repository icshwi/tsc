/*=========================< begin file & file header >=======================
 *  References
 *  
 *    filename : mapmaslib.h
 *    author   : JFG
 *    company  : IOxOS
 *    creation : july 30,2015
 *    version  : 0.0.1
 *
 *----------------------------------------------------------------------------
 *  Description
 *
 *    This file contain the declarations of all exported functions define in
 *    mapmaslib.c
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


#ifndef _H_MAPMASLIB
#define _H_MAPMASLIB

short tsc_map_mas_set_mode( struct ifc1211_device *ifc, struct tsc_ioctl_map_mode *m);
int tsc_map_mas_set_sg( struct ifc1211_device *ifc, struct map_ctl *map_ctl_p, int offset);
int tsc_map_mas_clear_sg( struct ifc1211_device *ifc, struct map_ctl *map_ctl_p, int offset, int npg);
int tsc_map_mas_alloc( struct ifc1211_device *ifc, struct tsc_ioctl_map_win *w);
int tsc_map_mas_modify( struct ifc1211_device *ifc, struct tsc_ioctl_map_win *w, ulong *rem_base);
int tsc_map_mas_free( struct ifc1211_device *ifc, int sg_id, uint offset);
int tsc_map_mas_get( struct ifc1211_device *ifc, struct tsc_ioctl_map_win *w);

#endif /*  _H_MAPMASLIB */

/*================================< end file >================================*/
