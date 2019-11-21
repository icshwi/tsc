/*=========================< begin file & file header >=======================
 *  References
 *  
 *    filename : mtca4rtmlib.h
 *    author   : RH
 *    company  : IOxOS
 *    creation : october 18,2017
 *
 *----------------------------------------------------------------------------
 *  Description
 *
 *    This file contains the declarations of all exported functions define in
 *    mtca4rtmlib.c
 *
 *----------------------------------------------------------------------------
 *
 *  Copyright (C) IOxOS Technologies SA <ioxos@ioxos.ch>
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

#ifndef _H_MTCA4RTMLIB
#define _H_MTCA4RTMLIB

typedef enum {
  MTCA4_RTM_BLUE_LED = 0,
  MTCA4_RTM_RED_LED,
  MTCA4_RTM_GREEN_LED,
  MTCA4_RTM_NUM_LEDS
} mtca4_rtm_led_id_t;

// as defined in PICMG 3.0, table 3-31
typedef enum {
  MTCA4_RTM_LED_OFF             = 0x00,
  MTCA4_RTM_LED_BLINK_100MS_OFF = 0x0A,
  MTCA4_RTM_LED_BLINK_500MS_OFF = 0x32,
  MTCA4_RTM_LED_LAMP_TEST       = 0xFB,
  MTCA4_RTM_LED_RESTORE         = 0xFC,
  MTCA4_RTM_LED_RESERVED1       = 0xFD,
  MTCA4_RTM_LED_RESERVED2       = 0xFE,
  MTCA4_RTM_LED_ON              = 0xFF
} mtca4_rtm_led_function_t;

typedef enum {
  MTCA4_RTM_LED_BLINK_100MS_ON = 0x0A,
  MTCA4_RTM_LED_BLINK_500MS_ON = 0x32,
} mtca4_rtm_led_on_duration_t;

typedef enum {
  MTCA4_RTM_EEPROM_WRITE_ENABLED = 0,
  MTCA4_RTM_EEPROM_WRITE_DISABLED
} mtca4_rtm_eeprom_wp_t;

typedef enum {
  MTCA4_RTM_RESET_ASSERTED = 0,
  MTCA4_RTM_RESET_DEASSERTED
} mtca4_rtm_reset_t;

typedef enum {
  MTCA4_RTM_ZONE3_ENABLED = 0,
  MTCA4_RTM_ZONE3_DISABLED
} mtca4_rtm_zone3_enable_t;

unsigned char set_mtca4_rtm_led_state(
  int fd,
  mtca4_rtm_led_id_t id,
  mtca4_rtm_led_function_t function,
  mtca4_rtm_led_on_duration_t on_duration);

unsigned char set_mtca4_rtm_eeprom_wp(
  int fd,
  mtca4_rtm_eeprom_wp_t state);

unsigned char set_mtca4_rtm_reset(
  int fd,
  mtca4_rtm_reset_t state);

unsigned char set_mtca4_rtm_zone3_enable(
  int fd,
  mtca4_rtm_zone3_enable_t state);

#endif


