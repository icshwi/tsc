/*=========================< begin file & file header >=======================
 *  References
 *  
 *    filename : rdt1465lib.h
 *    author   : XP
 *    company  : IOxOS
 *    creation : october 30,2017
 *
 *----------------------------------------------------------------------------
 *  Description
 *
 *    This file contain the declarations of all exported functions define in
 *    rdt1465lib.c
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

#ifndef _H_RDT1465LIB
#define _H_RDT1465LIB

typedef enum {
	RDT1465_EXT_PIN_LOW,
	RDT1465_EXT_PIN_HIGH,
	RDT1465_EXT_PIN_Z
	} rdt1465_ext_pin_state_t;

int rdt1465_init(int fd);
void rdt1465_dbg(int fd);
int rdt1465_presence(int fd);
int rdt1465_analog_enable(int fd, uint enable);
int rdt1465_gpio_output_enable(int fd, uint enable);
int rdt1465_red_led_enable(int fd, uint enable);
int rdt1465_extension_presence(int fd, uint *present);
int rdt1465_extension_set_pin_state(int fd, uint ext_pin_index, rdt1465_ext_pin_state_t state);
int rdt1465_extension_get_pin_state(int fd, uint ext_pin_index, uint *state, uint *direction);
#endif /*  _H_RDT1465LIB */
