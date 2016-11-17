/*=========================< begin file & file header >=======================
 *  References
 *  
 *    filename : pev791xlib.c
 *    author   : JFG
 *    company  : IOxOS
 *    creation : 
 *    version  : 0.0.1
 *
 *----------------------------------------------------------------------------
 *  Description
 *
 *     That library contains a set of function to access the IDT PCI Express
 *     switch
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

float
pev791x_bmr_conv_11bit_u( unsigned short val)
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

float
pev791x_bmr_conv_11bit_s( unsigned short val)
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

float
pev791x_bmr_conv_16bit_u( unsigned short val)
{
  return(((float)val/(float)(1 << 13)));
}

int
pev791x_bmr_read( uint bmr,
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
      device |= 0x53;
      break;
    }
    case 1:
    {
      device |= 0x63;
      break;
    }
    default:
    {
      return(-1);
    }
  }
  device = I2C_DEV( device, 2, IFC1211_I2C_CTL_CMDSIZ(1)|IFC1211_I2C_CTL_DATSIZ(cnt), IFC1211_I2C_CTL_SPEED_100k);
  device |= IFC1211_I2C_CTL_STR_REPEAT;

  return( tsc_i2c_read( device, reg, data));
}

int
pev791x_bmr_write( uint bmr,
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
      device = 0x53;
      break;
    }
    case 1:
    {
      device = 0x5b;
      break;
    }
    default:
    {
      return(-1);
    }
  }
  device = I2C_DEV( device, 2, IFC1211_I2C_CTL_CMDSIZ(1)|IFC1211_I2C_CTL_DATSIZ(2), IFC1211_I2C_CTL_SPEED_100k);
  device |= IFC1211_I2C_CTL_STR_REPEAT;

  return( tsc_i2c_write( device, reg, data));
}
