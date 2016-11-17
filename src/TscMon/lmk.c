/*=========================< begin file & file header >=======================
 *  References
 *  
 *    filename : lmk.c
 *    author   : JFG
 *    company  : IOxOS
 *    creation : Sept 15,2015
 *    version  : 0.0.1
 *
 *----------------------------------------------------------------------------
 *  Description
 *
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
#include <debug.h>
#include <sys/types.h>
#include <sys/time.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <cli.h>
#include <tscioctl.h>
#include <tsculib.h>
#include <aio.h>
#include <errno.h>
#include <ctype.h>
#include <time.h>
#include "TscMon.h"

#define LMK_OK   0
#define LMK_ERR -1
#define IFC_CSR_LMKCTL        0x48
#define IFC_CSR_LMKDAT        0x4c
#define IFC_LMKCTL_READ  0x80000000
#define IFC_LMKCTL_WRITE 0xc0000000
#define IFC_LMKCTL_LMK0  0x00000000
#define IFC_LMKCTL_LMK1  0x00010000
#define IFC_LMKCTL_ADD   0x0000001f

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * Function name : tsc_lmk
 * Prototype     : int
 * Parameters    : pointer to command parameter list
 * Return        : RDWR_OK  if command executed
 *                 RDWR_ERR if error
 *----------------------------------------------------------------------------
 * Description   : Compare IFC1211 CSR registers with expected content
 *
 *++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
int 
tsc_lmk( struct cli_cmd_para *c)
{
  int idx, reg, data;
  int tmp;


  idx = 0;
  if( c->ext)
  {
    if( c->ext[0] == '1') idx = 1;
  }
  if( c->cnt < 2)
  {
    printf("Not enough arguments -> usage:\n");
    tsc_print_usage( c);
    return( LMK_ERR);
  }
  if( sscanf( c->para[1],"%x", &reg) != 1)
  {
    printf("Bad register argument [%s] -> usage:\n", c->para[1]);
    tsc_print_usage( c);
    return( LMK_ERR);
  }
  reg &= IFC_LMKCTL_ADD;
  reg |= idx << 16;
  if(!strcmp( "read", c->para[0]))
  {
    int tmo;

    tsc_pon_read( IFC_CSR_LMKCTL, &tmp);
    reg |= tmp & ~(IFC_LMKCTL_WRITE | IFC_LMKCTL_LMK1 | IFC_LMKCTL_ADD);
    reg |= IFC_LMKCTL_READ;
    tsc_pon_write( IFC_CSR_LMKCTL, &reg);
    tmo = 100;
    while( tmo--)
    {
      tsc_pon_read( IFC_CSR_LMKCTL, &tmp);
      if( !(tmp & IFC_LMKCTL_READ))
      {
	break;
      }
    }
    tsc_pon_read( IFC_CSR_LMKDAT, &data);
    printf("LMK%d reg:%02x = %08x\n", idx, reg&IFC_LMKCTL_ADD, data);
    return( LMK_OK);
  }
  if(!strcmp( "write", c->para[0]))
  {
    if( c->cnt < 3)
    {
      printf("Not enough arguments -> usage:\n");
      tsc_print_usage( c);
      return( LMK_ERR);
    }
    if( sscanf( c->para[2],"%x", &data) != 1)
    {
      printf("Bad data argument [%s] -> usage:\n", c->para[2]);
      tsc_print_usage( c);
      return( LMK_ERR);
    }
    tsc_pon_write( IFC_CSR_LMKDAT, &data);
    tsc_pon_read( IFC_CSR_LMKCTL, &tmp);
    reg |= tmp & ~(IFC_LMKCTL_WRITE | IFC_LMKCTL_LMK1 | IFC_LMKCTL_ADD);
    reg |= IFC_LMKCTL_WRITE;
    tsc_pon_write( IFC_CSR_LMKCTL, &reg);
    return( LMK_OK);
  }
  return( LMK_ERR);
}
