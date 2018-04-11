/*=========================< begin file & file header >=======================
 *  References
 *  
 *    filename : ponmboxlib.c
 *    author   : RH
 *    company  : IOxOS
 *    creation : october 18,2017
 *
 *----------------------------------------------------------------------------
 *  Description
 *
 *    That library contains a set of function to access the PON mailbox.
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
static char rcsid[] = "$Id: ponmboxlib.c,v 1.00 2017/10/05 11:39:32 ioxos Exp $";
#endif

#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <errno.h>
#include <unistd.h>
#include "ponmboxlib.h"
#include "tsculib.h"

//#define DEBUG

#ifdef DEBUG
#include <stdio.h>
#define printd(fmt,arg...) printf(fmt, ## arg)
#else
#define printd(fmt,arg...)
#endif

/*
 * Descriptors definitions
 */

#define MAGIC_BYTE_VALUE_VALID                                          0x1F
#define MAGIC_BYTE_VALUE_TEMPORARY                                      0xAA

#define DESCRIPTOR_TYPE_NONE                                            0x00
#define DESCRIPTOR_TYPE_MAILBOX_INFO                                    0x01
#define DESCRIPTOR_TYPE_FIRMWARE_INFO                                   0x02
#define DESCRIPTOR_TYPE_BOARD_INFO                                      0x03
#define DESCRIPTOR_TYPE_PRODUCT_INFO                                    0x04
#define DESCRIPTOR_TYPE_SENSOR_DATA_VALUE                               0x05
#define DESCRIPTOR_TYPE_SERVICE_REQUEST                                 0x06
#define DESCRIPTOR_TYPE_RTM_INFO                                        0x07

#define DESCRIPTOR_FORMAT_VERSION_FIRMWARE_INFO                         0x01
#define DESCRIPTOR_FORMAT_VERSION_BOARD_INFO                            0x01
#define DESCRIPTOR_FORMAT_VERSION_PRODUCT_INFO                          0x01
#define DESCRIPTOR_FORMAT_VERSION_SENSOR_DATA_VALUE                     0x01
#define DESCRIPTOR_FORMAT_VERSION_SERVICE_REQUEST                       0x01
#define DESCRIPTOR_FORMAT_VERSION_RTM_INFO                              0x01

#define SENSOR_DATA_VALUE_SIZE_8BIT                                     0x01
#define SENSOR_DATA_VALUE_SIZE_16BIT                                    0x02
#define SENSOR_DATA_VALUE_SIZE_24BIT                                    0x03
#define SENSOR_DATA_VALUE_SIZE_32BIT                                    0x04

#define SENSOR_VALUE_STATUS_NOT_INITIALIZED                             0x01
#define SENSOR_VALUE_STATUS_UPDATE_IN_PROGRESS                          0x02
#define SENSOR_VALUE_STATUS_VALID                                       0x03
#define SENSOR_VALUE_STATUS_ERROR_UNABLE_TO_UPDATE                      0x04

#define SENSOR_NAME_MAX_LENGTH                                          0x10

/*
 * Service requests definitions
 */

#define SERVICE_REQUEST_REQUESTER_ID_OFFSET                             0x00
#define SERVICE_REQUEST_PROVIDER_ID_OFFSET                              0x01
#define SERVICE_REQUEST_STATUS_OFFSET                                   0x02
#define SERVICE_REQUEST_COMPLETION_CODE_OFFSET                          0x03
#define SERVICE_REQUEST_COMMAND_OFFSET                                  0x04
#define SERVICE_REQUEST_ARGUMENTS_LENGTH_OFFSET                         0x05
#define SERVICE_REQUEST_ARGUMENTS_OFFSET                                0x06

#define SERVICE_REQUEST_MMC_ID                                          0x01
#define SERVICE_REQUEST_CPU_ID                                          0x10
#define SERVICE_REQUEST_PON_FPGA_ID                                     0x20
#define SERVICE_REQUEST_CENTRAL_FPGA_ID                                 0x30
#define SERVICE_REQUEST_RTM_ID                                          0x40

#define SERVICE_REQUEST_STATUS_SUSPENDED                                   1
#define SERVICE_REQUEST_STATUS_PENDING                                     2
#define SERVICE_REQUEST_STATUS_PROCESSING                                  3
#define SERVICE_REQUEST_STATUS_COMPLETED                                   4

#define SERVICE_REQUEST_NORMAL_COMPLETION_CODE                          0x00

#define SERVICE_REQUEST_MAX_ARGUMENTS_LENGTH                               8

/*
 * Payload sensors definitions
 */

#define PAYLOAD_SENSOR_DESCRIPTOR_STATUS_OFFSET                            2
#define PAYLOAD_SENSOR_DESCRIPTOR_TIMESTAMP_OFFSET                         3
#define PAYLOAD_SENSOR_DESCRIPTOR_VALUE_SIZE_OFFSET                        7
#define PAYLOAD_SENSOR_DESCRIPTOR_VALUE_OFFSET                             8
#define PAYLOAD_SENSOR_DESCRIPTOR_8BIT_VALUE_NAME_OFFSET                   9
#define PAYLOAD_SENSOR_DESCRIPTOR_16BIT_VALUE_NAME_OFFSET                 10
#define PAYLOAD_SENSOR_DESCRIPTOR_24BIT_VALUE_NAME_OFFSET                 11
#define PAYLOAD_SENSOR_DESCRIPTOR_32BIT_VALUE_NAME_OFFSET                 12


/*
 * Private functions prototypes
 */

mbox_info_t *alloc_mbox_info(void);
int get_mbox_byte(int fd, int offset, unsigned char *destination);
int pop_mbox_data(int fd, int *offset, unsigned int *destination, int data_size);
int pop_mbox_byte(int fd, int *offset, unsigned char *destination);
int pop_mbox_short(int fd, int *offset, unsigned short *destination);
int pop_mbox_tribyte(int fd, int *offset, unsigned int *destination);
int pop_mbox_int(int fd, int *offset, unsigned int *destination);
unsigned char pop_mbox_string(int fd, int *offset, unsigned char **destination);
int set_mbox_byte(int fd, int offset, unsigned char byte);
int push_mbox_data(int fd, int *offset, unsigned int byte, int data_size);
int push_mbox_byte(int fd, int *offset, unsigned char byte);
int push_mbox_short(int fd, int *offset, unsigned short data);
int push_mbox_int(int fd, int *offset, unsigned int data);
void enable_service_request(int fd, mbox_info_t *info, int offset);
void wait_for_service_request_completion(int fd, mbox_info_t *info, int offset);
unsigned char skip_c_string(int fd, int *offset);
unsigned char skip_existing_mbox_sensors(int fd, mbox_info_t *info, int *offset);
int descriptor_name_offset_for_value_size(int value_size);
payload_sensor_handle_t *create_payload_sensor_handle(int offset, int value_size, char *name);


/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * Function name : get_mbox_info
 * Prototype     : mbox_info_t*
 * Parameters    : void
 * Return        : info
 *----------------------------------------------------------------------------
 * Description   : get mailbox information
 *
 *++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

mbox_info_t *get_mbox_info(int fd)
{
  int i;
  unsigned char data;
  int offset = 0;
  int continue_parsing = 1;
  unsigned char magic_byte;
  unsigned char descriptor_type;
  unsigned char descriptor_format_version;
  mbox_sensor_data_value_t *sensor = NULL;

  if (pop_mbox_byte(fd, &offset, &magic_byte)) return NULL;
  if (magic_byte != MAGIC_BYTE_VALUE_VALID) return NULL;

  mbox_info_t *info = alloc_mbox_info();
  if (!info) return NULL;

  while (continue_parsing)
  {
    pop_mbox_byte(fd, &offset, &descriptor_type);
    pop_mbox_byte(fd, &offset, &descriptor_format_version);
    printd("%s() descriptor type = %d, format = %d\n", __func__, descriptor_type, descriptor_format_version);

    switch (descriptor_type)
    {
    case DESCRIPTOR_TYPE_NONE:
      continue_parsing = 0;
      break;

    case DESCRIPTOR_TYPE_MAILBOX_INFO:
      pop_mbox_short(fd, &offset, &info->management_service_requests_offset);
      pop_mbox_short(fd, &offset, &info->payload_descriptors_offset);
      pop_mbox_short(fd, &offset, &info->payload_service_requests_offset);
      printd("%s() management service requests at 0x%04x\n", __func__, info->management_service_requests_offset);
      printd("%s() payload descriptors at 0x%04x\n", __func__, info->payload_descriptors_offset);
      printd("%s() payload service requests at 0x%04x\n", __func__, info->payload_service_requests_offset);
      break;

    case DESCRIPTOR_TYPE_FIRMWARE_INFO:
      pop_mbox_byte(fd, &offset, &info->firmware_revision.major);
      pop_mbox_byte(fd, &offset, &info->firmware_revision.minor);
      pop_mbox_byte(fd, &offset, &info->firmware_revision.maintenance);
      pop_mbox_int(fd, &offset, &info->firmware_revision.build_id);
      break;

    case DESCRIPTOR_TYPE_BOARD_INFO:
      pop_mbox_byte(fd, &offset, &info->amc_slot_number);
      pop_mbox_string(fd, &offset, &info->board_name);
      pop_mbox_string(fd, &offset, &info->board_revision);
      pop_mbox_string(fd, &offset, &info->board_serial_number);
      break;

    case DESCRIPTOR_TYPE_PRODUCT_INFO:
      pop_mbox_string(fd, &offset, &info->product_name);
      pop_mbox_string(fd, &offset, &info->product_revision);
      pop_mbox_string(fd, &offset, &info->product_serial_number);
      break;

    case DESCRIPTOR_TYPE_RTM_INFO:
      pop_mbox_byte(fd, &offset, &info->rtm_status);
      pop_mbox_tribyte(fd, &offset, &info->rtm_manufacturer_id);
      pop_mbox_int(fd, &offset, &info->rtm_zone3_interface_designator);
      break;

    case DESCRIPTOR_TYPE_SENSOR_DATA_VALUE:
      sensor = malloc(sizeof(mbox_sensor_data_value_t));
      if (!sensor) return info;
      sensor->status_offset = offset;
      offset += 1;
      sensor->timestamp_offset = offset;
      offset += 4;
      pop_mbox_byte(fd, &offset, &sensor->value_size);
      sensor->value_offset = offset;
      offset += sensor->value_size;
      pop_mbox_string(fd, &offset, &sensor->name);
      sensor->next = info->sensors_values;
      info->sensors_values = sensor;
      break;

    default:
      continue_parsing = 0;
    }
  }

  return info;
}

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * Function name : free_mbox_info
 * Prototype     : void
 * Parameters    : info
 * Return        : void
 *----------------------------------------------------------------------------
 * Description   : free mailbox info
 *
 *++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

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

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * Function name : get_mbox_sensor_value
 * Prototype     : int
 * Parameters    : info, name, value, timestamp
 * Return        : status
 *----------------------------------------------------------------------------
 * Description   : get mailbox sensor value
 *
 *++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

int get_mbox_sensor_value(int fd, mbox_info_t *info, unsigned char *name, int *value, int *timestamp)
{
  if (!info || !name || !value || !timestamp) return EFAULT;

  int i;
  int offset;
  unsigned char data;
  mbox_sensor_data_value_t *sensor = info->sensors_values;
  while (sensor)
  {
    if (!strcmp(sensor->name, name))
    {
      offset = sensor->status_offset;
      pop_mbox_byte(fd, &offset, &sensor->status);
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
        pop_mbox_byte(fd, &offset, &data);
        (*timestamp) = ((*timestamp) << 8) | data;
      }

      offset = sensor->value_offset;
      (*value) = 0;
      for (i = 0 ; i < sensor->value_size ; i++)
      {
        pop_mbox_byte(fd, &offset, &data);
        (*value) = ((*value) << 8) | data;
      }

      return 0;
    }

    sensor = sensor->next;
  }

  return ENXIO;
}

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * Function name : send_mbox_service_request
 * Prototype     : unsigned char
 * Parameters    : info, command, argc
 * Return        : status
 *----------------------------------------------------------------------------
 * Description   : send mailbox service request
 *
 *++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

unsigned char send_mbox_service_request(int fd, mbox_info_t *info, unsigned char command, unsigned char argc, ...)
{
  if (!info) return EFAULT;
  if (argc > SERVICE_REQUEST_MAX_ARGUMENTS_LENGTH) return E2BIG;

  int offset = info->payload_service_requests_offset;
  push_mbox_byte(fd, &offset, SERVICE_REQUEST_CPU_ID);
  push_mbox_byte(fd, &offset, SERVICE_REQUEST_MMC_ID);
  push_mbox_byte(fd, &offset, SERVICE_REQUEST_STATUS_SUSPENDED);
  push_mbox_byte(fd, &offset, SERVICE_REQUEST_NORMAL_COMPLETION_CODE);
  push_mbox_byte(fd, &offset, command);
  push_mbox_byte(fd, &offset, argc);

  va_list vl;
  va_start(vl, argc);
  while (argc--)
  {
    push_mbox_byte(fd, &offset, (unsigned char)va_arg(vl, int));
  }
  va_end(vl);


  enable_service_request(fd, info, info->payload_service_requests_offset);
  wait_for_service_request_completion(fd, info, info->payload_service_requests_offset);
  return 0;
}

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * Function name : alloc_mbox_info
 * Prototype     : mbox_info_t *
 * Parameters    : void
 * Return        : info
 *----------------------------------------------------------------------------
 * Description   : allocate mailbox information
 *
 *++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

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
  info->rtm_status = RTM_STATUS_ABSENT;
  info->rtm_manufacturer_id = 0;
  info->rtm_zone3_interface_designator = 0;
  info->sensors_values = NULL;
  return info;
}

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * Function name : get_mbox_byte
 * Prototype     : int
 * Parameters    : offset, destination
 * Return        : status
 *----------------------------------------------------------------------------
 * Description   : get mailbox byte
 *
 *++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

int get_mbox_byte(int fd, int offset, unsigned char *destination)
{
  int retval;
  int byte_offset = offset;
  int word_offset = byte_offset >> 2;
  int data = 0;

  if (!destination) return EFAULT;

  retval = tsc_pon_write(fd, 0xd0, &word_offset);
  if( retval < 0)
  {
    return EIO;
  }
  retval = tsc_pon_read(fd, 0xd4, &data);
  if( retval < 0)
  {
    return EIO;
  }

  byte_offset = (byte_offset & 0x03) * 8;
  *destination = (data >> byte_offset) & 0xFF;
  return 0;
}

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * Function name : pop_mbox_data
 * Prototype     : int
 * Parameters    : offset, destination, data size
 * Return        : status
 *----------------------------------------------------------------------------
 * Description   : pop mailbox data of given size
 *
 *++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

int pop_mbox_data(int fd, int *offset, unsigned int *destination, int data_size)
{
  if (data_size > sizeof(unsigned int)) return EINVAL;

  int retval;
  unsigned char byte;

  (*destination) = 0;
  while (data_size--)
  {
    retval = get_mbox_byte(fd, (*offset), &byte);
    if (retval) return retval;
    (*offset)++;
    (*destination) = ((*destination) << 8) | byte;
  }

  return 0;
}

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * Function name : pop_mbox_byte
 * Prototype     : int
 * Parameters    : offset, destination
 * Return        : status
 *----------------------------------------------------------------------------
 * Description   : pop mailbox byte
 *
 *++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

int pop_mbox_byte(int fd, int *offset, unsigned char *destination)
{
  unsigned int value;
  int retval = pop_mbox_data(fd, offset, &value, 1);
  (*destination) = (unsigned char)(value & 0xFF);
  return retval;
}

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * Function name : pop_mbox_short
 * Prototype     : int
 * Parameters    : offset, destination
 * Return        : status
 *----------------------------------------------------------------------------
 * Description   : pop mailbox short
 *
 *++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

int pop_mbox_short(int fd, int *offset, unsigned short *destination)
{
  unsigned int value;
  int retval = pop_mbox_data(fd, offset, &value, 2);
  (*destination) = (unsigned short)(value & 0xFFFF);
  return retval;
}

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * Function name : pop_mbox_tribyte
 * Prototype     : int
 * Parameters    : offset, destination
 * Return        : status
 *----------------------------------------------------------------------------
 * Description   : pop mailbox 24-bit word
 *
 *++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

int pop_mbox_tribyte(int fd, int *offset, unsigned int *destination)
{
  return pop_mbox_data(fd, offset, destination, 3);
}

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * Function name : pop_mbox_int
 * Prototype     : int
 * Parameters    : offset, destination
 * Return        : status
 *----------------------------------------------------------------------------
 * Description   : pop mailbox (integer)
 *
 *++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

int pop_mbox_int(int fd, int *offset, unsigned int *destination)
{
  return pop_mbox_data(fd, offset, destination, 4);
}

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * Function name : pop_mbox_string
 * Prototype     : unsigned char
 * Parameters    : offset, destination
 * Return        : size
 *----------------------------------------------------------------------------
 * Description   : pop mailbox (string)
 *
 *++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

unsigned char pop_mbox_string(int fd, int *offset, unsigned char **destination)
{
  int size = 0;
  char c;
  do
  {
    get_mbox_byte(fd, (*offset) + size++, &c);
  }
  while (c);

  char *buffer = malloc(size);
  if (!buffer) return 0;

  (*destination) = buffer;
  do
  {
    pop_mbox_byte(fd, offset, &c);
    (*(buffer++)) = c;
  }
  while (c);

  return size;
}

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * Function name : set_mbox_byte
 * Prototype     : int
 * Parameters    : offset, destination
 * Return        : status
 *----------------------------------------------------------------------------
 * Description   : set mailbox byte
 *
 *++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

int set_mbox_byte(int fd, int offset, unsigned char byte)
{
  int retval;
  int byte_offset = offset;
  int word_offset = byte_offset >> 2;
  int data;
  int mask;

  retval = tsc_pon_write(fd, 0xd0, &word_offset);
  if( retval < 0)
  {
    return EIO;
  }
  retval = tsc_pon_read(fd, 0xd4, &data);
  if( retval < 0)
  {
    return EIO;
  }

  byte_offset = (byte_offset & 0x03) * 8;
  mask = 0xFF << byte_offset;
  data &= ~mask;
  data |= byte << byte_offset;

  retval = tsc_pon_write(fd, 0xd4, &data);
  if( retval < 0)
  {
    return EIO;
  }

  return 0;
}

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * Function name : push_mbox_data
 * Prototype     : int
 * Parameters    : offset, byte
 * Return        : status
 *----------------------------------------------------------------------------
 * Description   : push mailbox (variable sized data)
 *
 *++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

int push_mbox_data(int fd, int *offset, unsigned int data, int data_size)
{
  if (data_size > sizeof(unsigned int)) return EINVAL;

  unsigned char bytes[data_size];
  int loop = data_size;
  while (loop--)
  {
    bytes[loop] = data & 0xFF;
    data >>= 8;
  }

  int retval;
  loop = 0;
  while (data_size--)
  {
    retval = set_mbox_byte(fd, *offset, bytes[loop++]);
    if (retval) return retval;
    (*offset)++;
  }

  return 0;
}

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * Function name : push_mbox_byte
 * Prototype     : int
 * Parameters    : offset, data
 * Return        : status
 *----------------------------------------------------------------------------
 * Description   : push mailbox (byte)
 *
 *++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

int push_mbox_byte(int fd, int *offset, unsigned char data)
{
  return push_mbox_data(fd, offset, (unsigned int)data, 1);
}

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * Function name : push_mbox_short
 * Prototype     : int
 * Parameters    : offset, data
 * Return        : status
 *----------------------------------------------------------------------------
 * Description   : push mailbox (short)
 *
 *++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

int push_mbox_short(int fd, int *offset, unsigned short data)
{
  return push_mbox_data(fd, offset, (unsigned int)data, 2);
}

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * Function name : push_mbox_int
 * Prototype     : int
 * Parameters    : offset, data
 * Return        : status
 *----------------------------------------------------------------------------
 * Description   : push mailbox (int)
 *
 *++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

int push_mbox_int(int fd, int *offset, unsigned int data)
{
  return push_mbox_data(fd, offset, data, 4);
}

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * Function name : push_mbox_string
 * Prototype     : int
 * Parameters    : offset, string
 * Return        : status
 *----------------------------------------------------------------------------
 * Description   : push mailbox (C string)
 *
 *++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

int push_mbox_string(int fd, int *offset, unsigned char *string)
{
  int retval;

  do
  {
    retval = push_mbox_byte(fd, offset, *string);
    if (retval) return retval;
  }
  while(*(string++));

  return 0;
}

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * Function name : enable_service_request
 * Prototype     : void
 * Parameters    : info, offset
 * Return        : void
 *----------------------------------------------------------------------------
 * Description   : enable service request
 *
 *++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

void enable_service_request(int fd, mbox_info_t *info, int offset)
{
  offset += SERVICE_REQUEST_STATUS_OFFSET; 
  push_mbox_byte(fd, &offset, SERVICE_REQUEST_STATUS_PENDING);
}

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * Function name : wait_for_service_request_completion
 * Prototype     : void
 * Parameters    : info, offset
 * Return        : status
 *----------------------------------------------------------------------------
 * Description   : wait for service request completion
 *
 *++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

void wait_for_service_request_completion(int fd, mbox_info_t *info, int offset)
{
  offset += SERVICE_REQUEST_STATUS_OFFSET; 
  unsigned char status = SERVICE_REQUEST_STATUS_PENDING;
  do
  {
    usleep(10000);
    get_mbox_byte(fd, offset, &status);
  }
  while (status != SERVICE_REQUEST_STATUS_COMPLETED);
}


/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * Function name : create_payload_sensor
 * Prototype     : unsigned char
 * Parameters    : info, value_size, name
 * Return        : status
 *----------------------------------------------------------------------------
 * Description   : create a new payload-side sensor descriptor
 *
 *++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

unsigned char create_payload_sensor(int fd, mbox_info_t *info, unsigned char *name, int value_size, payload_sensor_handle_t **handle)
{
  if (!info) return EFAULT;
  if (!name) return EFAULT;
  if (!handle) return EFAULT;

  int name_length = strlen(name) + 1;
  if (strlen(name) > SENSOR_NAME_MAX_LENGTH) return ENAMETOOLONG;

  int offset;
  if (skip_existing_mbox_sensors(fd, info, &offset)) return EFAULT;
  (*handle) = create_payload_sensor_handle(offset, value_size, name);

  push_mbox_byte(fd, &offset, DESCRIPTOR_TYPE_SENSOR_DATA_VALUE);
  push_mbox_byte(fd, &offset, DESCRIPTOR_FORMAT_VERSION_SENSOR_DATA_VALUE);
  push_mbox_byte(fd, &offset, SENSOR_VALUE_STATUS_NOT_INITIALIZED);
  push_mbox_int(fd, &offset, 0x00); // timestamp
  push_mbox_byte(fd, &offset, value_size);
  push_mbox_data(fd, &offset, 0x00, value_size); // value
  push_mbox_string(fd, &offset, name);
  push_mbox_byte(fd, &offset, DESCRIPTOR_TYPE_NONE); // end of descriptor

  return 0;
}


unsigned char get_payload_sensor_value(int fd, payload_sensor_handle_t *handle, int *value)
{
  if (!handle) return EFAULT;
  if (!value) return EFAULT;

  int value_offset = handle->descriptor_offset + PAYLOAD_SENSOR_DESCRIPTOR_VALUE_OFFSET;
  return pop_mbox_data(fd, &value_offset, value, handle->value_size);
}


unsigned char set_payload_sensor_value(int fd, payload_sensor_handle_t *handle, int value)
{
  if (!handle) return EFAULT;

  unsigned char retval;
  retval = set_mbox_byte(
             fd,
             handle->descriptor_offset + PAYLOAD_SENSOR_DESCRIPTOR_STATUS_OFFSET,
             SENSOR_VALUE_STATUS_UPDATE_IN_PROGRESS);
  if (retval) return retval;

  int value_offset = handle->descriptor_offset + PAYLOAD_SENSOR_DESCRIPTOR_VALUE_OFFSET;
  retval = push_mbox_data(fd, &value_offset, value, handle->value_size);
  if (retval) return retval;

  return set_mbox_byte(
           fd,
           handle->descriptor_offset + PAYLOAD_SENSOR_DESCRIPTOR_STATUS_OFFSET,
           SENSOR_VALUE_STATUS_VALID);
}


unsigned char get_payload_sensors(int fd, mbox_info_t *info, payload_sensor_handle_t **handle)
{
  if (!info) return EFAULT;
  if (!handle) return EFAULT;

  int offset = info->payload_descriptors_offset;
  int continue_parsing = 1;
  unsigned char descriptor_type;
  unsigned char descriptor_format_version;
  int name_offset;
  unsigned char *name;
  unsigned char value_size;
  payload_sensor_handle_t *last_handle = NULL;
  payload_sensor_handle_t *new_handle;

  (*handle) = NULL;

  while (continue_parsing)
  {
    pop_mbox_byte(fd, &offset, &descriptor_type);
    pop_mbox_byte(fd, &offset, &descriptor_format_version);

    switch (descriptor_type)
    {
    case DESCRIPTOR_TYPE_NONE:
      continue_parsing = 0;
      break;

    case DESCRIPTOR_TYPE_SENSOR_DATA_VALUE:
      get_mbox_byte(fd, offset - 2 + PAYLOAD_SENSOR_DESCRIPTOR_VALUE_SIZE_OFFSET, &value_size);
      name_offset = offset - 2 + descriptor_name_offset_for_value_size(value_size);
      pop_mbox_string(fd, &name_offset, &name);
      new_handle = create_payload_sensor_handle(offset-2, value_size, name);
      free(name);
      if (!(*handle)) (*handle) = new_handle;
      if (last_handle) last_handle->next = new_handle;
      last_handle = new_handle;
      offset = name_offset;
      break;

    default:
      free_payload_sensors(*handle);
      return EFAULT;
    }
  }

  if (*handle) return 0;
  return ENODATA;
}


payload_sensor_handle_t *find_payload_sensor(payload_sensor_handle_t *handle, char *name)
{
  if (!handle) return NULL;
  if (!name) return NULL;

  do
  {
    if (!strcmp(handle->sensor_name, name)) return handle;
    handle = handle->next;
  }
  while (handle);

  return NULL;
}


void free_payload_sensors(payload_sensor_handle_t *handle)
{
  if (!handle) return;

  payload_sensor_handle_t *next_handle;
  do
  {
    next_handle = handle->next;
    free(handle->sensor_name);
    free(handle);
    handle = next_handle;
  }
  while (handle);
}


// private

unsigned char skip_c_string(int fd, int *offset)
{
  char c;
  do
  {
    pop_mbox_byte(fd, offset, &c);
  }
  while (c);
  return 0;
}


unsigned char skip_existing_mbox_sensors(int fd, mbox_info_t *info, int *offset)
{
  if (!info) return EFAULT;
  if (!offset) return EFAULT;

  int running_offset = info->payload_descriptors_offset;
  int continue_parsing = 1;
  unsigned char descriptor_type;
  unsigned char descriptor_format_version;
  unsigned char c;
  unsigned char value_size;

  while (continue_parsing)
  {
    pop_mbox_byte(fd, &running_offset, &descriptor_type);
    pop_mbox_byte(fd, &running_offset, &descriptor_format_version);
    //printf("%s() descriptor type = %d, format = %d\n", __func__, descriptor_type, descriptor_format_version);

    switch (descriptor_type)
    {
    case DESCRIPTOR_TYPE_NONE:
      continue_parsing = 0;
      (*offset) = running_offset - 2;
      break;

    case DESCRIPTOR_TYPE_SENSOR_DATA_VALUE:
      get_mbox_byte(fd, running_offset - 2 + PAYLOAD_SENSOR_DESCRIPTOR_VALUE_SIZE_OFFSET, &value_size);
      running_offset = running_offset - 2 + descriptor_name_offset_for_value_size(value_size);
      skip_c_string(fd, &running_offset);
      break;

    default:
      return EFAULT;
    }
  }

  return 0;
}


int descriptor_name_offset_for_value_size(int value_size)
{
  switch (value_size)
  {
  case SENSOR_DATA_VALUE_SIZE_8BIT:
    return PAYLOAD_SENSOR_DESCRIPTOR_8BIT_VALUE_NAME_OFFSET;

  case SENSOR_DATA_VALUE_SIZE_16BIT:
    return PAYLOAD_SENSOR_DESCRIPTOR_16BIT_VALUE_NAME_OFFSET;

  case SENSOR_DATA_VALUE_SIZE_24BIT:
    return PAYLOAD_SENSOR_DESCRIPTOR_24BIT_VALUE_NAME_OFFSET;

  case SENSOR_DATA_VALUE_SIZE_32BIT:
    return PAYLOAD_SENSOR_DESCRIPTOR_32BIT_VALUE_NAME_OFFSET;
  }
  return 0;
}


payload_sensor_handle_t *create_payload_sensor_handle(int offset, int value_size, char *name)
{
  if (!name) return NULL;

  payload_sensor_handle_t *handle = malloc(sizeof(payload_sensor_handle_t));
  if (!handle) return NULL;

  handle->sensor_name = malloc(strlen(name));
  if (!handle->sensor_name)
  {
    free(handle);
    return NULL;
  }
  strcpy(handle->sensor_name, name);

  handle->next = NULL;
  handle->descriptor_offset = offset;
  handle->value_size = value_size;
}


