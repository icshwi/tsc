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


#define RTM_STATUS_ABSENT                              0
#define RTM_STATUS_INCOMPATIBLE                        1
#define RTM_STATUS_COMPATIBLE_NO_PAYLOAD_POWER         2
#define RTM_STATUS_COMPATIBLE_HAS_PAYLOAD_POWER        3

#define IOXOS_MANUFACTURER_ID                          0xbe5e

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


mbox_info_t *get_mbox_info(void);
void free_mbox_info(mbox_info_t *info);
int get_mbox_sensor_value(mbox_info_t *info, unsigned char *name, int *value, int *timestamp);
unsigned char send_mbox_service_request(mbox_info_t *info, unsigned char command, unsigned char argc, ...);

#endif
