/*=========================< begin file & file header >=======================
 *  References
 *  
 *    filename : ponmboxlib.h
 *    author   : RH
 *    company  : IOxOS
 *    creation : october 4,2017
 *
 *----------------------------------------------------------------------------
 *  Description
 *
 *    This file contains the declarations of all exported functions define in
 *    ponmboxlib.c
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

#ifndef _H_PONMBOXLIB
#define _H_PONMBOXLIB

/*
 * Descriptors definitions
 */

#define MAGIC_BYTE_VALUE_VALID               					        0x1F
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
#define SENSOR_DATA_VALUE_SIZE_32BIT                                    0x04

#define SENSOR_VALUE_STATUS_NOT_INITIALIZED                             0x01
#define SENSOR_VALUE_STATUS_UPDATE_IN_PROGRESS                          0x02
#define SENSOR_VALUE_STATUS_VALID                                       0x03
#define SENSOR_VALUE_STATUS_ERROR_UNABLE_TO_UPDATE                      0x04

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

#define RTM_STATUS_ABSENT                              0
#define RTM_STATUS_INCOMPATIBLE                        1
#define RTM_STATUS_COMPATIBLE_NO_PAYLOAD_POWER         2
#define RTM_STATUS_COMPATIBLE_HAS_PAYLOAD_POWER        3

typedef struct mbox_sensor_data_value
{
  struct mbox_sensor_data_value *next;
  unsigned char *name;
  int status_offset;
  int timestamp_offset;
  unsigned char value_size;
  int value_offset;
  unsigned char status;
  int timestamp;
  int value;
} mbox_sensor_data_value_t;

typedef struct
{
  unsigned char major;
  unsigned char minor;
  unsigned char maintenance;
  unsigned int build_id;
} mbox_firmware_revision_t;

typedef struct
{
  unsigned short management_service_requests_offset;
  unsigned short payload_descriptors_offset;
  unsigned short payload_service_requests_offset;
  mbox_firmware_revision_t firmware_revision;
  unsigned char firmware_revision_major;
  unsigned char firmware_revision_minor;
  unsigned char amc_slot_number;
  unsigned char *board_name;
  unsigned char *board_revision;
  unsigned char *board_serial_number;
  unsigned char *product_name;
  unsigned char *product_revision;
  unsigned char *product_serial_number;
  unsigned char rtm_status;
  unsigned int rtm_manufacturer_id;
  unsigned int rtm_zone3_interface_designator;
  mbox_sensor_data_value_t *sensors_values;
} mbox_info_t;

mbox_info_t *alloc_mbox_info(void);
mbox_info_t *get_mbox_info(void);

int get_mbox_byte(int offset, unsigned char *destination);
int pop_mbox_byte(int *offset, unsigned char *destination);
int pop_mbox_short(int *offset, unsigned short *destination);
int pop_mbox_tribyte(int *offset, unsigned int *destination);
int pop_mbox_int(int *offset, unsigned int *destination);
int push_mbox_byte(int *offset, unsigned char byte);
int get_mbox_sensor_value(mbox_info_t *info, unsigned char *name, int *value, int *timestamp);
void enable_service_request(mbox_info_t *info, int offset);
void wait_for_service_request_completion(mbox_info_t *info, int offset);
void free_mbox_info(mbox_info_t *info);
unsigned char send_mbox_service_request(mbox_info_t *info, unsigned char command, unsigned char argc, ...);
unsigned char pop_mbox_string(int *offset, unsigned char **destination);

#endif
