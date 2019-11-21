/*=========================< begin file & file header >=======================
 *  References
 *  
 *    filename : rcc1466lib.h
 *    author   : XP, RH
 *    company  : IOxOS
 *    creation : July 15,2019
 *
 *----------------------------------------------------------------------------
 *  Description
 *
 *    This file contain the declarations of all exported functions define in
 *    rcc1466lib.c
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

#ifndef _H_RCC1466LIB
#define _H_RCC1466LIB

typedef enum {
	RCC1466_EXT_PIN_LOW,
	RCC1466_EXT_PIN_HIGH,
	RCC1466_EXT_PIN_Z
	} rcc1466_ext_pin_state_t;

typedef enum {
	RCC1466_LED123_GREEN,	// 0
	RCC1466_LED123_RED,		// 1
	RCC1466_LED124_GREEN,	// 2
	RCC1466_LED124_RED,		// 3
	RCC1466_LED125_GREEN,	// 4
	RCC1466_LED125_RED,		// 5
	RCC1466_LED126_GREEN,	// 6
	RCC1466_LED126_RED,		// 7
	} rcc1466_led_t;

typedef enum {
	RCC1466_SFP_FPGA_LANE_0,
	RCC1466_SFP_FPGA_LANE_1,
	RCC1466_SFP_FPGA_LANE_2,
	RCC1466_SFP_FPGA_LANE_3,
	RCC1466_SFP_CPU_SGMII
	} rcc1466_sfp_id_t;

typedef enum {
	SFP_PRESENT        = 0x08,
	SFP_TX_FAULT       = 0x10,
	SFP_LOSS_OF_SIGNAL = 0x20
	} rcc1466_sfp_status_t;

typedef enum {
	SFP_TX_DISABLE     = 0x01,
	SFP_RX_HIGH_RATE   = 0x02,
	SFP_TX_HIGH_RATE   = 0x04
	} rcc1466_sfp_control_t;

int rcc1466_init(int fd);
int rcc1466_presence(int fd);
int rcc1466_extension_presence(int fd, int *present);
int rcc1466_led_turn_on(int fd, rcc1466_led_t led_id);
int rcc1466_led_turn_off(int fd, rcc1466_led_t led_id);
int rcc1466_sfp_status(int fd, rcc1466_sfp_id_t id, uint8_t *status);
int rcc1466_sfp_control(int fd, rcc1466_sfp_id_t id, int sfp_enable, int sfp_rate);
#endif /*  _H_RCC1466LIB */
