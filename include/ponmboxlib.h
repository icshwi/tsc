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
 *  Copyright (C) IOxOS Technologies SA <ioxos@ioxos.ch>
 *  Copyright (C) 2019  European Spallation Source ERIC
 *
 *    THIS SOFTWARE IS PROVIDED BY THE AUTHOR AND CONTRIBUTORS ``AS IS'' AND
 *    ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 *    IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 *    ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR OR CONTRIBUTORS BE LIABLE
 *    FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 *    DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 *    OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 *    HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 *    LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 *    OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 *    SUCH DAMAGE.
 *
 *    GPL license :
 *    This program is free software; you can redistribute it and/or
 *    modify it under the terms of the GNU General Public License
 *    as published by the Free Software Foundation; either version 2
 *    of the License, or (at your option) any later version.
 *
 *    This program is distributed in the hope that it will be useful,
 *    but WITHOUT ANY WARRANTY; without even the implied warranty of
 *    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *    GNU General Public License for more details.
 *
 *    You should have received a copy of the GNU General Public License
 *    along with this program; if not, write to the Free Software
 *    Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
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


mbox_info_t *get_mbox_info(int fd);
void free_mbox_info(mbox_info_t *info);
int get_mbox_sensor_value(int fd, mbox_info_t *info, unsigned char *name, int *value, int *timestamp);
unsigned char send_mbox_service_request(int fd, mbox_info_t *info, unsigned char command, unsigned char argc, ...);

typedef struct payload_sensor_handle_s payload_sensor_handle_t;

typedef struct payload_sensor_handle_s
{
  payload_sensor_handle_t *next;
  char *sensor_name;
  int descriptor_offset;
  int value_size;
} payload_sensor_handle_t;

unsigned char create_payload_sensor(int fd, mbox_info_t *info, unsigned char *name, int value_size, payload_sensor_handle_t **handle);
unsigned char get_payload_sensor_value(int fd, payload_sensor_handle_t *handle, int *value);
unsigned char set_payload_sensor_value(int fd, payload_sensor_handle_t *handle, int value);
unsigned char get_payload_sensors(int fd, mbox_info_t *info, payload_sensor_handle_t **handle);
payload_sensor_handle_t *find_payload_sensor(payload_sensor_handle_t *handle, char *name);
void free_payload_sensors(payload_sensor_handle_t *handle);

#endif
