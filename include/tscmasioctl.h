/*=========================< begin file & file header >=======================
 *  References
 *  
 *    filename : tscmasioctl.h
 *    author   : JFG, XP
 *    company  : IOxOS
 *    creation : Sept 30,2015
 *
 *----------------------------------------------------------------------------
 *  Description
 *
 *    This file contains the declarations for the ioctl commands supported
 *    by the TSC master driver.
 *
 *----------------------------------------------------------------------------
 *
 *  Copyright Notice
 *  
 *    Copyright and all other rights in this document are reserved by 
 *    IOxOS Technologies SA. This documents contains proprietary information    
 *    and is supplied on express condition that it may not be disclosed, 
 *    reproduced in whole or in part, or used for any other purpose other
 *    than that for which it is supplies, without the written consent of  
 *    IOxOS Technologies SA                                                        
 *  
 *=============================< end file header >============================*/

#ifndef _H_TSCMASIOCTL
#define _H_TSCMASIOCTL

#define TSC_IOCTL_MAS            0x00030000
#define TSC_IOCTL_MAS_MAP_SET    (TSC_IOCTL_MAS | 0x1)
#define TSC_IOCTL_MAS_MAP_GET    (TSC_IOCTL_MAS | 0x2)
#define TSC_IOCTL_MAS_IRQ_SET    (TSC_IOCTL_MAS | 0x3)
#define TSC_IOCTL_MAS_IRQ_GET    (TSC_IOCTL_MAS | 0x4)

struct tsc_ioctl_mas_map
{
  ulong rem_addr;
  ulong loc_addr;
  uint size;
  uint mode;
  uint pg_idx;
  uint ivec;
};

#define MAS_MAP_MODE_SWAP_AUTO      0x100
#define MAS_MAP_MODE_SWAP_DW        0x200
#define MAS_MAP_MODE_SWAP_QW        0x300
#define MAS_MAP_MODE_SWAP_MASK      0x300

#define MAS_MAP_MODE_SHM             0x20000
#define MAS_MAP_MODE_USR             0x30000
#define MAS_MAP_MODE_SPACE           0xf0000

#define MAS_MAP_NO_IVEC             -1
#define MAS_MAP_IVEC_MIN             0
#define MAS_MAP_IVEC_MAX          0xff

#define MAS_MAP_IDX_INV             -1

#endif /*  _H_TSCMASIOCTL */

