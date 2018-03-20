/*=========================< begin file & file header >=======================
 *  References
 *  
 *    filename : tscextlib.h
 *    author   : JFG, XP
 *    company  : IOxOS
 *    creation : june 30,2008
 *
 *----------------------------------------------------------------------------
 *  Description
 *
 *    This file contains the declarations of all exported functions define in
 *    tscextlib.c
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

#ifndef _H_TSCEXTLIB
#define _H_TSCEXTLIB

#include <tscioctl.h>

struct tscext_ioctl_map_pg
{
  uint size;                            /* mapping size required by user            */
  char flag; char sg_id; ushort mode;   /* mapping mode                             */
  uint64_t rem_addr;                       /* remote address to be mapped              */
  uint64_t loc_addr;                       /* local address returned by mapper         */
  uint offset;                          /* offset of page containing local address  */
  uint win_size;                        /* size actually mapped                     */
  uint64_t rem_base;                       /* remote address of window actually mapped */
  uint64_t loc_base;                       /* local address of window actually mapped  */
  void *usr_addr;                       /* user address pointing to local address   */
  uint64_t pci_base;                       /* pci base address of SG window            */
};

float tscext_bmr_conv_11bit_u( unsigned short);
float tscext_bmr_conv_11bit_s( unsigned short);
float tscext_bmr_conv_16bit_u( unsigned short);
int tscext_pex_write( uint, uint);
int tscext_bmr_read( uint, uint, uint *, uint);
int tscext_bmr_write( uint, uint, uint, uint);
int tscext_csr_rd( int);
void tscext_csr_wr( int, int);
int tscext_map_alloc( struct tscext_ioctl_map_pg *);
int tscext_map_free( struct tscext_ioctl_map_pg *);
void *tscext_mmap( struct tscext_ioctl_map_pg *);
int tscext_munmap( struct tscext_ioctl_map_pg *);

#endif /*  _H_TSCEXTLIB */
