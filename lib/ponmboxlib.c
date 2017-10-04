/*=========================< begin file & file header >=======================
 *  References
 *  
 *    filename : ponmboxlib.c
 *    author   : RH
 *    company  : IOxOS
 *    creation : 
 *    version  : 0.0.1
 *
 *----------------------------------------------------------------------------
 *  Description
 *
 *     That library contains a set of function to access the PON mailbox.
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

#ifndef lint
static char rcsid[] = "$Id: ponmboxlib.c,v 1.00 2017/10/05 11:39:32 ioxos Exp $";
#endif

#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include "ponmboxlib.h"

#define MAGIC_BYTE_VALUE_VALID                           0x1F
#define MAGIC_BYTE_VALUE_TEMPORARY                       0xAA

#define DESCRIPTOR_TYPE_NONE                             0x00
#define DESCRIPTOR_TYPE_FIRMWARE_INFO                    0x01
#define DESCRIPTOR_TYPE_BOARD_INFO                       0x02
#define DESCRIPTOR_TYPE_PRODUCT_INFO                     0x03
#define DESCRIPTOR_TYPE_SENSOR_DATA_VALUE                0x04
#define DESCRIPTOR_TYPE_SERVICE_REQUEST                  0x05

#define DESCRIPTOR_FORMAT_VERSION_FIRMWARE_INFO          0x01
#define DESCRIPTOR_FORMAT_VERSION_BOARD_INFO             0x01
#define DESCRIPTOR_FORMAT_VERSION_PRODUCT_INFO           0x01
#define DESCRIPTOR_FORMAT_VERSION_SENSOR_DATA_VALUE      0x01
#define DESCRIPTOR_FORMAT_VERSION_SERVICE_REQUEST        0x01

#define SENSOR_DATA_VALUE_SIZE_8BIT                      0x01
#define SENSOR_DATA_VALUE_SIZE_16BIT                     0x02
#define SENSOR_DATA_VALUE_SIZE_32BIT                     0x04

#define SENSOR_VALUE_STATUS_NOT_INITIALIZED                             0x01
#define SENSOR_VALUE_STATUS_UPDATE_IN_PROGRESS                          0x02
#define SENSOR_VALUE_STATUS_VALID                                       0x03
#define SENSOR_VALUE_STATUS_ERROR_UNABLE_TO_UPDATE                      0x04


/*
 * Private functions prototypes
 */

mbox_info_t *alloc_mbox_info(void);
int get_mbox_byte(int byte_offset, unsigned char *destination);
unsigned char copy_mbox_string(int offset, unsigned char **destination);

/*
 * Public functions
 */

mbox_info_t *get_mbox_info(void)
{
  int i;
  int offset = 0;
  int continue_parsing = 1;
  unsigned char magic_byte;
  unsigned char descriptor_type;
  unsigned char descriptor_format_version;
  mbox_sensor_data_value_t *sensor = NULL;

  if (get_mbox_byte(offset++, &magic_byte)) return NULL;
  if (magic_byte != MAGIC_BYTE_VALUE_VALID) return NULL;

  mbox_info_t *info = alloc_mbox_info();
  if (!info) return NULL;

  while (continue_parsing)
  {
    get_mbox_byte(offset++, &descriptor_type);
    get_mbox_byte(offset++, &descriptor_format_version);

    switch (descriptor_type)
    {
    case DESCRIPTOR_TYPE_NONE:
      continue_parsing = 0;
      break;

    case DESCRIPTOR_TYPE_FIRMWARE_INFO:
      get_mbox_byte(offset++, &info->firmware_revision_major);
      get_mbox_byte(offset++, &info->firmware_revision_minor);
      break;

    case DESCRIPTOR_TYPE_BOARD_INFO:
      get_mbox_byte(offset++, &info->amc_slot_number);
      offset += copy_mbox_string(offset, &info->board_name);
      offset += copy_mbox_string(offset, &info->board_revision);
      offset += copy_mbox_string(offset, &info->board_serial_number);
      break;

    case DESCRIPTOR_TYPE_PRODUCT_INFO:
      offset += copy_mbox_string(offset, &info->product_name);
      offset += copy_mbox_string(offset, &info->product_revision);
      offset += copy_mbox_string(offset, &info->product_serial_number);
      break;

    case DESCRIPTOR_TYPE_SENSOR_DATA_VALUE:
      sensor = malloc(sizeof(mbox_sensor_data_value_t));
      if (!sensor) return info;
      sensor->status_offset = offset;
      offset += 1;
      sensor->timestamp_offset = offset;
      offset += 4;
      get_mbox_byte(offset++, &sensor->value_size);
      sensor->value_offset = offset;
      offset += sensor->value_size;
      offset += copy_mbox_string(offset, &sensor->name);
      sensor->next = info->sensors_values;
      info->sensors_values = sensor;
      break;

    default:
      continue_parsing = 0;
    }
  }

  return info;
}


void free_mbox_info(mbox_info_t *info)
{
  if (!info) return;
  if (info->board_name)            free(info->board_name);
  if (info->board_revision)        free(info->board_revision);
  if (info->board_serial_number)   free(info->board_serial_number);
  if (info->product_name)          free(info->product_name);
  if (info->product_revision)      free(info->product_revision);
  if (info->product_serial_number) free(info->product_serial_number);

  mbox_sensor_data_value_t *record = info->sensors_values;
  mbox_sensor_data_value_t *next_record;
  while (record)
  {
    next_record = record->next;
    if (record->name) free(record->name);
    free(record);
    record = next_record;
  }

  free(info);
}


int get_mbox_sensor_value(mbox_info_t *info, unsigned char *name, int *value, int *timestamp)
{
  int i;
  int offset;
  unsigned char data;
  mbox_sensor_data_value_t *sensor = info->sensors_values;
  while (sensor)
  {
    if (!strcmp(sensor->name, name))
    {
      offset = sensor->status_offset;
      get_mbox_byte(offset, &sensor->status);
      switch (sensor->status)
      {
      case SENSOR_VALUE_STATUS_NOT_INITIALIZED:
        return EAGAIN;

      case SENSOR_VALUE_STATUS_UPDATE_IN_PROGRESS:
        return EBUSY;

      case SENSOR_VALUE_STATUS_VALID:
        break;

      case SENSOR_VALUE_STATUS_ERROR_UNABLE_TO_UPDATE:
        return EREMOTEIO;

      default:
        return EPROTO;
      }

      offset = sensor->timestamp_offset;
      (*timestamp) = 0;
      for (i = 0 ; i < 4 ; i++)
      {
        get_mbox_byte(offset++, &data);
        (*timestamp) = ((*timestamp) << 8) | data;
      }

      offset = sensor->value_offset;
      (*value) = 0;
      for (i = 0 ; i < sensor->value_size ; i++)
      {
        get_mbox_byte(offset++, &data);
        (*value) = ((*value) << 8) | data;
      }

      return 0;
    }

    sensor = sensor->next;
  }

  return ENXIO;
}


/*
 * Private functions
 */

mbox_info_t *alloc_mbox_info(void)
{
  mbox_info_t *info = malloc(sizeof(mbox_info_t));
  if (!info) return NULL;

  info->board_name = NULL;
  info->board_revision = NULL;
  info->board_serial_number = NULL;
  info->product_name = NULL;
  info->product_revision = NULL;
  info->product_serial_number = NULL;
  info->sensors_values = NULL;
  return info;
}


int get_mbox_byte(int byte_offset, unsigned char *destination)
{
  int retval;
  int word_offset = byte_offset >> 2;
  int data = 0;

  if (!destination) return EFAULT;

  retval = tsc_pon_write( 0xd0, &word_offset);
  if( retval < 0)
  {
    return EIO;
  }
  retval = tsc_pon_read( 0xd4, &data);
  if( retval < 0)
  {
    return EIO;
  }

  byte_offset = (byte_offset & 0x03) * 8;
  *destination = (data >> byte_offset) & 0xFF;
  return 0;
}


unsigned char copy_mbox_string(int offset, unsigned char **destination)
{
  int size = 0;
  unsigned char c;
  unsigned char buffer[256];

  while (1)
  {
    get_mbox_byte(offset++, &c);
    buffer[size++] = c;
    if (!c) break;
  }

  (*destination) = malloc(size);
  strncpy((*destination), buffer, size);

  return size;
}


