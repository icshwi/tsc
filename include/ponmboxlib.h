/*=========================< begin file & file header >=======================
 *  References
 *  
 *    filename : tsculib.h
 *    author   : JFG
 *    company  : IOxOS
 *    creation : june 30,2008
 *    version  : 0.0.1
 *
 *----------------------------------------------------------------------------
 *  Description
 *
 *    This file contain the declarations of all exported functions define in
 *    tsculib.c
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

#ifndef _H_PONMBOXLIB
#define _H_PONMBOXLIB


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
  unsigned char firmware_revision_major;
  unsigned char firmware_revision_minor;
  unsigned char amc_slot_number;
  unsigned char *board_name;
  unsigned char *board_revision;
  unsigned char *board_serial_number;
  unsigned char *product_name;
  unsigned char *product_revision;
  unsigned char *product_serial_number;
  mbox_sensor_data_value_t *sensors_values;
} mbox_info_t;


mbox_info_t *get_mbox_info(void);
void free_mbox_info(mbox_info_t *info);
int get_mbox_sensor_value(mbox_info_t *info, unsigned char *name, int *value, int *timestamp);


#endif

