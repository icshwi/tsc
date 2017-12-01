/*=========================< begin file & file header >=======================
 *  References
 *  
 *    filename : pev791xlib.h
 *    author   : JFG, XP
 *    company  : IOxOS
 *    creation : june 30,2008
 *
 *----------------------------------------------------------------------------
 *  Description
 *
 *    This file contains the declarations of all exported functions define in
 *    pev791xlib.c
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

#ifndef _H_PEV791XLIB
#define _H_PEV791XLIB

struct pev_ioctl_map_pg
{
  uint size;                            /* mapping size required by user            */
  char flag; char sg_id; ushort mode;   /* mapping mode                             */
  ulong rem_addr;                       /* remote address to be mapped              */
  ulong loc_addr;                       /* local address returned by mapper         */
  uint offset;                          /* offset of page containing local address  */
  uint win_size;                        /* size actually mapped                     */
  ulong rem_base;                       /* remote address of window actually mapped */
  ulong loc_base;                       /* local address of window actually mapped  */
  void *usr_addr;                       /* user address pointing to local address   */
  ulong pci_base;                       /* pci base address of SG window            */
};

float pev791x_bmr_conv_11bit_u( unsigned short);
float pev791x_bmr_conv_11bit_s( unsigned short);
float pev791x_bmr_conv_16bit_u( unsigned short);
int pev791x_pex_write( uint, uint);
int pev791x_bmr_read( uint, uint, uint *, uint);
int pev791x_bmr_write( uint, uint, uint, uint);
int pev_csr_rd( int);
void pev_csr_wr( int, int);
int pev_map_alloc( struct pev_ioctl_map_pg *);
int pev_map_free( struct pev_ioctl_map_pg *);
void *pev_mmap( struct pev_ioctl_map_pg *);
int pev_munmap( struct pev_ioctl_map_pg *);

#endif /*  _H_PEV791XLIB */
