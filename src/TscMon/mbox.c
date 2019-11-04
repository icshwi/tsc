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

extern int tsc_fd;

void print_out_rtm_info(mbox_info_t *info);
void print_out_payload_sensor_info(mbox_info_t *mbox_info, char *sensor_name);
void print_out_payload_sensor_details(payload_sensor_handle_t *handle);

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
    retval = tsc_pon_write(tsc_fd, 0xd0, &offset);
    if( retval < 0)
    {
      puts("cannot write to mailbox address register");
      return( CLI_ERR);
    }
    retval = tsc_pon_read(tsc_fd, 0xd4, &data);
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
  retval = tsc_pon_write(tsc_fd, 0xd0, &offset);
  if( retval < 0)
  {
    puts("cannot write to mailbox address register");
    return( CLI_ERR);
  }
  retval = tsc_pon_write(tsc_fd, 0xd4, &data);
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
  /* static unsigned char sensors_installed = 0; */

  if (c->cnt > 1)
  {
    tsc_print_usage( c);
    return( CLI_ERR);
  }

  mbox_info_t *info = get_mbox_info(tsc_fd);
  if (!info)
  {
    puts("failed to get mbox info");
    return( CLI_ERR);
  }

  puts("--------------------------------------------------------");
  puts("  AMC Info");
  puts("--------------------------------------------------------");
  printf("  MMC firmware revision:           %d.%d.%d (%08x)\n",
         info->firmware_revision.major,
         info->firmware_revision.minor,
         info->firmware_revision.maintenance,
         info->firmware_revision.build_id);
  printf("  AMC slot number [0-11]:          %d\n", info->amc_slot_number);
  printf("  Board name:                      %s\n", info->board_name);
  printf("  Board revision:                  %s\n", info->board_revision);
  printf("  Board serial number:             %s\n", info->board_serial_number);
  printf("  Product name:                    %s\n", info->product_name);
  printf("  Product revision:                %s\n", info->product_revision);
  printf("  Product serial number:           %s\n", info->product_serial_number);

  puts("--------------------------------------------------------");
  puts("  RTM Info");
  puts("--------------------------------------------------------");
  switch (info->rtm_status)
  {
  case RTM_STATUS_ABSENT:
    puts("  No RTM board");
    break;

  case RTM_STATUS_INCOMPATIBLE:
    puts("  RTM present but not compatible");
    print_out_rtm_info(info);
    break;

  case RTM_STATUS_COMPATIBLE_NO_PAYLOAD_POWER:
    print_out_rtm_info(info);
    puts("  Power state:                     RTM has no payload power");
    break;

  case RTM_STATUS_COMPATIBLE_HAS_PAYLOAD_POWER:
    print_out_rtm_info(info);
    puts("  Power state:                     RTM has payload power");
    break;

  default:
    printf("  Unknown RTM status: %d\n", info->rtm_status);
  }

  puts("--------------------------------------------------------");
  puts("  Management-side sensors");
  puts("--------------------------------------------------------");
  mbox_sensor_data_value_t *sensor = info->sensors_values;
  int value = 0;
  int retval;
  int timestamp = 0;
  
  while (sensor)
  {
    printf("  Sensor \"%s\":", sensor->name);
    retval = get_mbox_sensor_value(tsc_fd, info, sensor->name, &value, &timestamp);
    if (retval) {
      printf("   failed to get data of sensor: %s\n", strerror(retval));
    }
    else {
      puts("");
    }
    printf("    status = (%d) %s\n", sensor->status, mbox_sensor_status[sensor->status]);
    printf("    value = %d\n", value);
    printf("    timestamp = %d\n", timestamp);
    sensor = sensor->next;
  }

  puts("--------------------------------------------------------");
  puts("  Payload-side sensors");
  puts("--------------------------------------------------------");
  print_out_payload_sensor_info(info, NULL);

  puts("--------------------------------------------------------");
  free_mbox_info(info);
  return CLI_OK;
}


/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * Function name : print_out_rtm_info
 * Prototype     : void
 * Parameters    : info
 * Return        : void
 *
 *----------------------------------------------------------------------------
 * Description   : print out rtm info
 *
 *++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

void
print_out_rtm_info(mbox_info_t *info)
{
  printf("  RTM manufacturer id:             %06x\n", info->rtm_manufacturer_id);
  printf("  RTM zone 3 interface designator: %08x\n", info->rtm_zone3_interface_designator);
}

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * Function name : mbox_payload_sensor_show
 * Prototype     : int
 * Parameters    : pointer to command parameter list
 * Return        : status
 *
 *----------------------------------------------------------------------------
 * Description   : mbox payload sensor show
 *
 *++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

int mbox_payload_sensor_show( struct cli_cmd_para *c)
{
  char *sensor_name = NULL;

  if (c->cnt == 2)
  {
    sensor_name = NULL;
  }
  else if (c->cnt == 3)
  {
    sensor_name = c->para[2];
  }
  else
  {
    tsc_print_usage( c);
    return( CLI_ERR);
  }

  mbox_info_t *info = get_mbox_info(tsc_fd);
  if (!info)
  {
    puts("failed to get mbox info");
    return( CLI_ERR);
  }

  print_out_payload_sensor_info(info, sensor_name);

  free_mbox_info(info);
  return CLI_OK;
}

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * Function name : print_out_payload_sensor_info
 * Prototype     : void
 * Parameters    : mbox_info, sensor_name
 * Return        : void
 *
 *----------------------------------------------------------------------------
 * Description   : print out payload sensor info
 *
 *++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

void print_out_payload_sensor_info(mbox_info_t *mbox_info, char *sensor_name)
{
  payload_sensor_handle_t *all_sensors;
  payload_sensor_handle_t *current_handle;
  if (!get_payload_sensors(tsc_fd, mbox_info, &all_sensors))
  {
    if (sensor_name == NULL)
    {
      payload_sensor_handle_t *current_handle = all_sensors;
      while (current_handle)
      {
        print_out_payload_sensor_details(current_handle);
        current_handle = current_handle->next;
      }
    }
    else
    {
      current_handle = find_payload_sensor(all_sensors, sensor_name);
      if (current_handle)
      {
        print_out_payload_sensor_details(current_handle);
      }
      else
      {
        printf("no such sensor \"%s\"", sensor_name);
      }
    }

    free_payload_sensors(all_sensors);
  }
  else
  {
    puts("No payload sensors");
  }
}

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * Function name : print_out_payload_sensor_details
 * Prototype     : void
 * Parameters    : handle
 * Return        : void
 *
 *----------------------------------------------------------------------------
 * Description   : print out payload sensor details
 *
 *++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

void print_out_payload_sensor_details(payload_sensor_handle_t *handle)
{
  if (!handle) return;
  printf("  Sensor \"%s\":\n", handle->sensor_name);
  printf("    descriptor offset = 0x%04x\n", handle->descriptor_offset);
  printf("    value size = %d\n", handle->value_size);

  int value = 0;
  get_payload_sensor_value(tsc_fd, handle, &value);
  printf("    value = %d (0x%08x)\n", value, value);
}

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * Function name : mbox_payload_sensor_create
 * Prototype     : int
 * Parameters    : cli_cmd_para
 * Return        : status
 *
 *----------------------------------------------------------------------------
 * Description   : mbox payload sensor create
 *
 *++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

int mbox_payload_sensor_create( struct cli_cmd_para *c)
{
  if (c->cnt != 4)
  {
    tsc_print_usage( c);
    return( CLI_ERR);
  }

  unsigned char *sensor_name = (unsigned char*)c->para[2];
  int value_size = atoi(c->para[3]);
  if (value_size < 1 || value_size > 4)
  {
    tsc_print_usage( c);
    return( CLI_ERR);
  }

  mbox_info_t *info = get_mbox_info(tsc_fd);
  if (!info)
  {
    puts("failed to get mbox info");
    return( CLI_ERR);
  }

  payload_sensor_handle_t *handle;
  /* sensor name is signed char, and this code puts it into unsigned char in 3rd arg */
  /* It is better to check how sensor_name is formed carefully */
  /* han.lee@esss.se Thursday, October 17 23:02:20 CEST 2019 */
  if (!create_payload_sensor(tsc_fd, info, sensor_name, value_size, &handle))
  {
    print_out_payload_sensor_details(handle);
    free_payload_sensors(handle);
  }

  free_mbox_info(info);
  return CLI_OK;
}

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * Function name : mbox_payload_sensor_set
 * Prototype     : int
 * Parameters    : cli_cmd_para
 * Return        : status
 *
 *----------------------------------------------------------------------------
 * Description   : mbox payload sensor set
 *
 *++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

int mbox_payload_sensor_set( struct cli_cmd_para *c)
{
  if (c->cnt != 4)
  {
    tsc_print_usage( c);
    return( CLI_ERR);
  }

  char *sensor_name = c->para[2];
  int value = strtol(c->para[3], NULL, 16);

  mbox_info_t *info = get_mbox_info(tsc_fd);
  if (!info)
  {
    puts("failed to get mbox info");
    return( CLI_ERR);
  }

  payload_sensor_handle_t *all_sensors;
  payload_sensor_handle_t *current_handle;
  if (!get_payload_sensors(tsc_fd, info, &all_sensors))
  {
    current_handle = find_payload_sensor(all_sensors, sensor_name);
    if (current_handle)
    {
      set_payload_sensor_value(tsc_fd, current_handle, value);
    }
    else
    {
      printf("No such sensor \"%s\"", sensor_name);
    }

    free_payload_sensors(all_sensors);
  }
  else
  {
    puts("No payload sensors");
  }

  free_mbox_info(info);
  return CLI_OK;
}

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * Function name : mbox_payload_sensor
 * Prototype     : int
 * Parameters    : pointer to command parameter list
 * Return        : int
 *
 *----------------------------------------------------------------------------
 * Description   : perform payload sensor-related operations
 *
 *++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

int mbox_payload_sensor( struct cli_cmd_para *c)
{
  if (c->cnt < 2)
  {
    tsc_print_usage( c);
    return( CLI_ERR);
  }

  if( !strcmp( "show", c->para[1]))
  {
    return( mbox_payload_sensor_show( c));
  }
  else if( !strcmp( "create", c->para[1]))
  {
    return( mbox_payload_sensor_create( c));
  }
  else if( !strcmp( "set", c->para[1]))
  {
    return( mbox_payload_sensor_set( c));
  }
  tsc_print_usage( c);
  return( CLI_ERR);
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
  tsc_pon_read(tsc_fd, 0x0, &data);
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
    else if( !strcmp( "write", c->para[i]))
    {
      return( mbox_write( c));
    }
    else if( !strcmp( "info", c->para[i]))
    {
      return( mbox_info( c));
    }
    else if( !strcmp( "payload_sensor", c->para[i]))
    {
      return( mbox_payload_sensor( c));
    }
  }
  tsc_print_usage( c);
  return( CLI_ERR);
}
