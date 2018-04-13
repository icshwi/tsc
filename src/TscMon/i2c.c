/*=========================< begin file & file header >=======================
 *  References
 *  
 *    filename : i2c.c
 *    author   : JFG, XP
 *    company  : IOxOS
 *    creation : Sept 15,2015
 *
 *----------------------------------------------------------------------------
 *  Description
 *
 *    This file contains functions do drive I2C.
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

#define I2C_OK   0
#define I2C_ERR -1

#define I2C_CTL_CMDSIZ_1   0x00000000
#define I2C_CTL_CMDSIZ_2   0x00010000
#define I2C_CTL_CMDSIZ_3   0x00020000
#define I2C_CTL_CMDSIZ_4   0x00030000
#define I2C_CTL_DATSIZ_1   0x00000000
#define I2C_CTL_DATSIZ_2   0x00040000
#define I2C_CTL_DATSIZ_3   0x00080000
#define I2C_CTL_DATSIZ_4   0x000c0000

#define I2C_IFC_DEV_LM95255   I2C_DEV( 0x4c, 0, (I2C_CTL_CMDSIZ_1 | I2C_CTL_DATSIZ_1))
#define I2C_IFC_DEV_DS1339    I2C_DEV( 0x68, 0, (I2C_CTL_CMDSIZ_1 | I2C_CTL_DATSIZ_1))
#define I2C_IFC_DEV_MAX5970   I2C_DEV( 0x30, 1, (I2C_CTL_CMDSIZ_1 | I2C_CTL_DATSIZ_1))
#define I2C_IFC_DEV_BMR463_0  I2C_DEV( 0x24, 1, (I2C_CTL_CMDSIZ_1 | I2C_CTL_DATSIZ_2))
#define I2C_IFC_DEV_BMR463_1  I2C_DEV( 0x53, 1, (I2C_CTL_CMDSIZ_1 | I2C_CTL_DATSIZ_2))
#define I2C_IFC_DEV_BMR463_2  I2C_DEV( 0x63, 1, (I2C_CTL_CMDSIZ_1 | I2C_CTL_DATSIZ_2))
#define I2C_IFC_DEV_PES32NT   I2C_DEV( 0x75, 1, (I2C_CTL_CMDSIZ_4 | I2C_CTL_DATSIZ_4))

struct tsc_i2c_devices i2c_devices_ifc[] =
{
  { "lm95255",  I2C_IFC_DEV_LM95255},
  { "ds1339"   ,I2C_IFC_DEV_DS1339},
  { "max5970",  I2C_IFC_DEV_MAX5970},
  { "bmr463_0", I2C_IFC_DEV_BMR463_0},
  { "bmr463_1", I2C_IFC_DEV_BMR463_1},
  { "bmr463_2", I2C_IFC_DEV_BMR463_2},
  { "pes32nt",  I2C_IFC_DEV_PES32NT},
  { NULL,       0x00000000}
};

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * Function name : tsc_i2c
 * Prototype     : int
 * Parameters    : pointer to command parameter list
 * Return        : ok  if command executed
 *                 nok if error
 *----------------------------------------------------------------------------
 * Description   : execute i2c commands
 *
 *++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

// Original version
#ifdef JFG
int 
tsc_i2c( struct cli_cmd_para *c)
{
  struct tsc_i2c_devices *i2d;
  int idx, reg, data, device;
  int de, rs;


  idx = 0;
  if( c->ext)
  {
    if( c->ext[0] == '1') idx = 1;
  }
  if( c->cnt < 3)
  {
    printf("Not enough arguments -> usage:\n");
    tsc_print_usage( c);
    return( I2C_ERR);
  }
  i2d = &i2c_devices_ifc[0];
  device = 0;
  while( i2d->name)
  {
    if( !strcmp( i2d->name, c->para[0]))
    {
      device = i2d->id;
      break;
    }
    i2d++;
  }
  if( !device)
  {
    printf("Bad device name [%s] -> usage:\n", c->para[0]);
    tsc_print_usage( c);
    return( I2C_ERR);
  }
  if( sscanf( c->para[2],"%x", &reg) != 1)
  {
    printf("Bad register argument [%s] -> usage:\n", c->para[1]);
    tsc_print_usage( c);
    return( I2C_ERR);
  }
  if(!strcmp( "read", c->para[1]))
  {
    printf("i2c read: %x %x\n", device, reg);
    tsc_i2c_read( device, reg, &data);
    printf("data = %x\n", data);
    return( I2C_OK);
  }
  if(!strcmp( "write", c->para[1]))
  {
    if( c->cnt < 4)
    {
      printf("Not enough arguments -> usage:\n");
      tsc_print_usage( c);
      return( I2C_ERR);
    }
    if( sscanf( c->para[2],"%x", &data) != 1)
    {
      printf("Bad data argument [%s] -> usage:\n", c->para[2]);
      tsc_print_usage( c);
      return( I2C_ERR);
    }
    printf("i2c write: %x %x\n", device, reg);
    return( I2C_OK);
  }
  return( I2C_ERR);
}
#endif

// New version
int 
tsc_i2c( struct cli_cmd_para *c)
{
  int bus, reg, data, addr, device;
  int rs, ds;
  int retval;

  bus = 0;
  if( c->ext)
  {
    bus = *(char *)c->ext - '0';
  }
  if( c->cnt < 3)
  {
    printf("Not enough arguments -> usage:\n");
    tsc_print_usage( c);
    return( I2C_ERR);
  }
  device = (bus&7)<<29;
  if( sscanf( c->para[0],"%x", &addr) != 1)
  {
    printf("Bad device argument [%s] -> usage:\n", c->para[0]);
    tsc_print_usage( c);
    return( I2C_ERR);
  }
  device |= addr & 0x7f;
  rs = 1;
  retval = sscanf( c->para[2],"%x.%d", &reg, &rs);
  if( retval < 1)
  {
    printf("Bad register argument [%s] -> usage:\n", c->para[2]);
    tsc_print_usage( c);
    return( I2C_ERR);
  }
  device |= ((rs-1)&3)<<16; 
  if(!strncmp( "read", c->para[1], 2))
  {
    ds = 1;
    sscanf( c->para[1],"read.%d", &ds);
    device |= ((ds-1)&3)<<18;
    printf("i2c read: %08x %x\n", device, reg);
    tsc_i2c_read( device, reg, &data);
    printf("data = %x\n", data);
    return( I2C_OK);
  }
  if(!strncmp( "write", c->para[1], 2))
  {
    ds = 1;
    sscanf( c->para[1],"write.%d", &ds);
    device |= ((ds-1)&3)<<18; 
    retval = sscanf( c->para[3],"%x", &data);
    if( retval < 1)
    {
      printf("Bad data argument [%s] -> usage:\n", c->para[3]);
      tsc_print_usage( c);
      return( I2C_ERR);
    }
    printf("i2c write: %08x %x\n", device, reg, data);
    tsc_i2c_write( device, reg, data);
    return( I2C_OK);
  }
  return( I2C_ERR);
}
