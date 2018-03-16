/*=========================< begin file & file header >=======================
 *  References
 *  
 *    filename : mbox.c
 *    author   : RH
 *    company  : IOxOS
 *    creation : Sept 28,2017
 *
 *----------------------------------------------------------------------------
 *  Description
 *
 *    That file contains a set of function to drive mailbox.
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

#ifndef lint
static char *rcsid = "$Id: mbox.c,v 1.0 2017/10/18 08:26:51 ioxos Exp $";
#endif

#define DEBUGno
#include <debug.h>
#include <sys/types.h>
#include <sys/time.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <cli.h>
#include <tscioctl.h>
#include <tsculib.h>
#include <aio.h>
#include <errno.h>
#include <ctype.h>
#include <time.h>
#include "mbox.h"
#include "ponmboxlib.h"
#include "TscMon.h"

void print_out_rtm_info(mbox_info_t *info);

char *
mbox_rcsid()
{
  return( rcsid);
}

char *mbox_sensor_status[] =
{
  "(reserved)",
  "not initialized",
  "update in progress",
  "valid",
  "error: unable to update"
};

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * Function name : mbox_read
 * Prototype     : int
 * Parameters    : pointer to command parameter list
 * Return        : int
 *
 *----------------------------------------------------------------------------
 * Description   : perform mbox read operation
 *
 *++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

int 
mbox_read( struct cli_cmd_para *c)
{
  int retval;
  int offset = 0;
  int count = 1;
  int data = 0;

  if ((c->cnt < 2) || (c->cnt > 3))
  {
    tsc_print_usage( c);
    return( CLI_ERR);
  }
  if( sscanf( c->para[1], "%x", &offset) != 1)
  {
    printf("Bad offset argument [%s] -> usage:\n", c->para[0]);
    tsc_print_usage( c);
    return( CLI_ERR);
  }
  if( (c->cnt == 3) && sscanf( c->para[2], "%x", &count) != 1)
  {
    printf("Bad offset argument [%s] -> usage:\n", c->para[0]);
    tsc_print_usage( c);
    return( CLI_ERR);
  }

  offset >>= 2;
  while( count--)
  {
    retval = tsc_pon_write( 0xd0, &offset);
    if( retval < 0)
    {
      puts("cannot write to mailbox address register");
      return( CLI_ERR);
    }
    retval = tsc_pon_read( 0xd4, &data);
    if( retval < 0)
    {
      puts("cannot read from mailbox data register");
      return( CLI_ERR);
    }
    printf("0x%04x : 0x%08x\n", offset << 2, data);
    offset++;
  }

  return CLI_OK;
}

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * Function name : mbox_write
 * Prototype     : int
 * Parameters    : pointer to command parameter list
 * Return        : int
 *
 *----------------------------------------------------------------------------
 * Description   : perform mbox write operation
 *
 *++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

int 
mbox_write( struct cli_cmd_para *c)
{
  int retval;
  int offset = 0;
  int data = 0;

  if (c->cnt != 3)
  {
    tsc_print_usage( c);
    return( CLI_ERR);
  }
  else if( sscanf( c->para[1], "%x", &offset) != 1)
  {
    printf("Bad offset argument [%s] -> usage:\n", c->para[0]);
    tsc_print_usage( c);
    return( CLI_ERR);
  }
  else if( sscanf( c->para[2], "%x", &data) != 1)
  {
    printf("Bad data argument [%s] -> usage:\n", c->para[0]);
    tsc_print_usage( c);
    return( CLI_ERR);
  }

  offset >>= 2;
  retval = tsc_pon_write( 0xd0, &offset);
  if( retval < 0)
  {
    puts("cannot write to mailbox address register");
    return( CLI_ERR);
  }
  retval = tsc_pon_write( 0xd4, &data);
  if( retval < 0)
  {
    puts("cannot write to mailbox data register");
    return( CLI_ERR);
  }

  return CLI_OK;
}

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * Function name : mbox_info
 * Prototype     : int
 * Parameters    : pointer to command parameter list
 * Return        : int
 *
 *----------------------------------------------------------------------------
 * Description   : perform mbox info operation
 *
 *++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

int 
mbox_info( struct cli_cmd_para *c)
{

  if (c->cnt > 1)
  {
    tsc_print_usage( c);
    return( CLI_ERR);
  }

  mbox_info_t *info = get_mbox_info();
  if (!info)
  {
    puts("failed to get mbox info");
    return( CLI_ERR);
  }

  printf("Firmware revision:               %d.%d.%d (%08x)\n",
         info->firmware_revision.major,
         info->firmware_revision.minor,
         info->firmware_revision.maintenance,
         info->firmware_revision.build_id);
  printf("AMC slot number:                 %d\n", info->amc_slot_number);
  printf("Board name:                      %s\n", info->board_name);
  printf("Board revision:                  %s\n", info->board_revision);
  printf("Board serial number:             %s\n", info->board_serial_number);
  printf("Product name:                    %s\n", info->product_name);
  printf("Product revision:                %s\n", info->product_revision);
  printf("Product serial number:           %s\n", info->product_serial_number);
  switch (info->rtm_status)
  {
  case RTM_STATUS_ABSENT:
    puts("No RTM board");
    break;

  case RTM_STATUS_INCOMPATIBLE:
    puts("RTM present but not compatible");
    print_out_rtm_info(info);
    break;

  case RTM_STATUS_COMPATIBLE_NO_PAYLOAD_POWER:
    print_out_rtm_info(info);
    puts("RTM has no payload power");
    break;

  case RTM_STATUS_COMPATIBLE_HAS_PAYLOAD_POWER:
    print_out_rtm_info(info);
    puts("RTM has payload power");
    break;

  default:
    printf("Unknown RTM status: %d\n", info->rtm_status);
  }

  mbox_sensor_data_value_t *sensor = info->sensors_values;
  int value = 0;
  int retval;
  int timestamp = 0;
  while (sensor)
  {
    printf("Sensor %s:", sensor->name);
    if (retval = get_mbox_sensor_value(info, sensor->name, &value, &timestamp))
    {
      printf(" failed to get data of sensor: %s\n", strerror(retval));
    }
    else puts("");
    printf("  status = (%d) %s\n", sensor->status, mbox_sensor_status[sensor->status]);
    printf("  value = %d\n", value);
    printf("  timestamp = %d\n", timestamp);
    sensor = sensor->next;
  }

  free_mbox_info(info);

  return CLI_OK;
}


void
print_out_rtm_info(mbox_info_t *info)
{
  printf("RTM manufacturer id:             %06x\n", info->rtm_manufacturer_id);
  printf("RTM zone 3 interface designator: %08x\n", info->rtm_zone3_interface_designator);
}


/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * Function name : tsc_mbox
 * Prototype     : int
 * Parameters    : pointer to command parameter list
 * Return        : int
 *
 *----------------------------------------------------------------------------
 * Description   : perform mbox operation
 *
 *++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

int 
tsc_mbox( struct cli_cmd_para *c)
{
  int cnt  = 0;
  int i    = 0;;
  int data = 0;

  // Check if the board is a IFC14xx
  tsc_pon_read(0x0, &data);
  if (((data & 0xffffff00) >> 8) != 0x735714) {
	printf("Command available only on IFC14xx board\n");
	return (CLI_ERR);
  }

  cnt = c->cnt;
  i = 0;
  if( cnt--)
  {
    if( !strcmp( "read", c->para[i]))
    {
      return( mbox_read( c));
    }
    if( !strcmp( "write", c->para[i]))
    {
      return( mbox_write( c));
    }
    if( !strcmp( "info", c->para[i]))
    {
      return( mbox_info( c));
    }
  }
  tsc_print_usage( c);
  return( CLI_ERR);
}
