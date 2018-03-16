/*=========================< begin file & file header >=======================
 *  References
 *  
 *    filename : tscextlib.c
 *    author   : JFG, XP
 *    company  : IOxOS
 *    creation : october 21,2016
 *
 *----------------------------------------------------------------------------
 *  Description
 *
 *    That library contains a set of function to access device.
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

#include <stdlib.h>
#include <stdio.h>
#include <pty.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <errno.h>
#include <sys/mman.h>

#include "../include/tscioctl.h"
#include "../include/tsculib.h"
#include "../include/tscextlib.h"

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * Function name : tscext_bmr_conv_11bit_u
 * Prototype     : float
 * Parameters    : value
 * Return        : value converted
 *----------------------------------------------------------------------------
 * Description   : convert value on 11 bits unsigned
 *
 *++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

float
tscext_bmr_conv_11bit_u( unsigned short val)
{
  unsigned short l;
  short h;

  l = val & (unsigned short)0x7ff;
  h = (short)(val >> 11);
  h |= (short)0xffe0;
  h =  (short)~h;
  h++;
  return(((float)l/(float)(1 << h)));
}

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * Function name : tscext_bmr_conv_11bit_s
 * Prototype     : float
 * Parameters    : value
 * Return        : value converted
 *----------------------------------------------------------------------------
 * Description   : convert value on 11 bits signed
 *
 *++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

float
tscext_bmr_conv_11bit_s( unsigned short val)
{
  short h,l;

  l = (short)(val & (unsigned short)0x7ff);
  if( l & 0x400) l |= (short)0xf800;
  h = (short)(val >> 11);
  h |= (short)0xffe0;
  h =  (short)~h;
  h++;
  return(((float)l/(float)(1 << h)));
}

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * Function name : tscext_bmr_conv_16bit_u
 * Prototype     : float
 * Parameters    : value to convert
 * Return        : value converted
 *----------------------------------------------------------------------------
 * Description   : convert a 16 bits value unsigned
 *
 *++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

float
tscext_bmr_conv_16bit_u( unsigned short val)
{
  return(((float)val/(float)(1 << 13)));
}

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * Function name : tscext_bmr_read
 * Prototype     : int
 * Parameters    : bmr, register, data read, count
 * Return        : status of operation
 *----------------------------------------------------------------------------
 * Description   : read from bmr
 *
 *++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

int
tscext_bmr_read( uint bmr,
	      uint reg,
	      uint *data,
	      uint cnt)
{
  int device;

  device = 0;
  if( cnt > 4) return( -1);
  switch( bmr)
  {
    case 0:
    {
      device |= 0x24;
      break;
    }
    case 1:
    {
      device |= 0x53;
      break;
    }
    case 2:
    {
      device |= 0x63;
      break;
    }
    default:
    {
      return(-1);
    }
  }
  device = I2C_DEV( device, 1, TSC_I2C_CTL_CMDSIZ(1)|TSC_I2C_CTL_DATSIZ(cnt));

  return( tsc_i2c_read( device, reg, data));
}

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * Function name : tscext_bmr_write
 * Prototype     : int
 * Parameters    : bmr, register, data, count
 * Return        : status of operation
 *----------------------------------------------------------------------------
 * Description   : write into bmr
 *
 *++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

int
tscext_bmr_write( uint bmr,
	       uint reg,
	       uint data,
	       uint cnt)
{
  int device;

  device = 0;
  if( cnt > 4) return( -1);
  switch( bmr)
  {
    case 0:
    {
      device = 0x24;
      break;
    }
    case 1:
    {
      device = 0x53;
      break;
    }
    case 2:
    {
      device = 0x63;
      break;
    }
    default:
    {
      return(-1);
    }
  }
  device = I2C_DEV( device, 1, TSC_I2C_CTL_CMDSIZ(1)|TSC_I2C_CTL_DATSIZ(2));

  return( tsc_i2c_write( device, reg, data));
}

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * Function name : tscext_csr_rd
 * Prototype     : int
 * Parameters    : register
 * Return        : data read
 *----------------------------------------------------------------------------
 * Description   : read a data in csr
 *
 *++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

int
tscext_csr_rd( int reg)
{
  int data;
  tsc_csr_read( reg, &data);
  return( data);
}

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * Function name : tscext_csr_wr
 * Prototype     : void
 * Parameters    : register and data
 * Return        : void
 *----------------------------------------------------------------------------
 * Description   : write in csr space
 *
 *++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

void
tscext_csr_wr( int reg,
	    int data)
{
  tsc_csr_write( reg, &data);
}
