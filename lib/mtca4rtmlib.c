/*=========================< begin file & file header >=======================
 *  References
 *
 *    filename : mtca4rtmlib.c
 *    author   : RH
 *    company  : IOxOS
 *    creation : october 18,2017
 *
 *----------------------------------------------------------------------------
 *  Description
 *
 *    This file contains a set of function to act on a MTCA.4 RTM.
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

#include "mtca4rtmlib.h"
#include "ponmboxlib.h"

/*
 * Service requests commands
 */
#define CMD_SET_RTM_LED_STATE                     0x01
#define CMD_GET_RTM_LED_STATE                     0x02
#define CMD_SET_RTM_RESET_STATE                   0x03
#define CMD_SET_RTM_EEPROM_WP_STATE               0x04
#define CMD_SET_RTM_ZONE3_ENABLE                  0x05

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * Function name : set_mtca4_rtm_led_state
 * Prototype     : unsigned char
 * Parameters    : id, function, on_duration
 * Return        : status of operation
 *----------------------------------------------------------------------------
 * Description   : set led state function
 *
 *++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

unsigned char set_mtca4_rtm_led_state(
  int fd,
  mtca4_rtm_led_id_t id,
  mtca4_rtm_led_function_t function,
  mtca4_rtm_led_on_duration_t on_duration)
{
  mbox_info_t *mbox = get_mbox_info(fd);
  unsigned char status = send_mbox_service_request(
                           fd,
                           mbox,
                           CMD_SET_RTM_LED_STATE,
                           3,
                           id,
                           function,
                           on_duration);
  free_mbox_info(mbox);
  return status;
}

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * Function name : set_mtca4_rtm_reset
 * Prototype     : unsigned char
 * Parameters    : state
 * Return        : status of operation
 *----------------------------------------------------------------------------
 * Description   : set the state of the RTM_RESET_N signal
 *
 *++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

unsigned char set_mtca4_rtm_reset(
  int fd,
  mtca4_rtm_reset_t state)
{
  mbox_info_t *mbox = get_mbox_info(fd);
  unsigned char status = send_mbox_service_request(
                           fd,
                           mbox,
                           CMD_SET_RTM_RESET_STATE,
                           1,
                           state);
  free_mbox_info(mbox);
  return status;
}

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * Function name : set_mtca4_rtm_reset
 * Prototype     : unsigned char
 * Parameters    : state
 * Return        : status of operation
 *----------------------------------------------------------------------------
 * Description   : set the state of the RTM_RESET_N signal
 *
 *++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

unsigned char set_mtca4_rtm_eeprom_wp(
  int fd,
  mtca4_rtm_eeprom_wp_t state)
{
  mbox_info_t *mbox = get_mbox_info(fd);
  unsigned char status = send_mbox_service_request(
                           fd,
                           mbox,
                           CMD_SET_RTM_EEPROM_WP_STATE,
                           1,
                           state);
  free_mbox_info(mbox);
  return status;
}


/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * Function name : set_mtca4_rtm_zone3_enable
 * Prototype     : unsigned char
 * Parameters    : state
 * Return        : status of operation
 *----------------------------------------------------------------------------
 * Description   : enable/disable zone3 I/Os
 *
 *++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

unsigned char set_mtca4_rtm_zone3_enable(
  int fd,
  mtca4_rtm_zone3_enable_t state)
{
  mbox_info_t *mbox = get_mbox_info(fd);
  unsigned char status = send_mbox_service_request(
                           fd,
                           mbox,
                           CMD_SET_RTM_ZONE3_ENABLE,
                           1,
                           state);
  free_mbox_info(mbox);
  return status;
}
