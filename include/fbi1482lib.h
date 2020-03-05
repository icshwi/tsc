/*=========================< begin file & file header >=======================
 *  References
 *  
 *    filename : fbi1482lib.h
 *    author   : RH
 *    company  : IOxOS
 *    creation : December 11,2019
 *
 *----------------------------------------------------------------------------
 *  Description
 *
 *    This file contain the declarations of all exported functions define in
 *    fbi1482lib.c
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

#ifndef _H_FBI1482LIB
#define _H_FBI1482LIB

typedef enum {
  FBI1482_RS485_ACTIVITY = 0,
  FBI1482_RS485_FAULT = 1
} fbi1482_rs485_indicator_t;

typedef enum {
  FBI1482_RS485_TXEN_CHANNEL_0 = 0,
  FBI1482_RS485_TXEN_CHANNEL_1 = 1,
  FBI1482_RS485_TXEN_CHANNEL_2 = 2,
  FBI1482_RS485_TXEN_CHANNEL_3 = 3
} fbi1482_rs485_txen_t;

int fbi1482_presence(uint i2c_bus);
int fbi1482_init(int fd, uint i2c_bus);
int fbi1482_get_temp(int fd, uint i2c_bus, int *temp);
int fbi1482_get_uid(int fd, uint i2c_bus, unsigned char *uid);
int fbi1482_rs485_set_indicator(int fd, uint i2c_bus, fbi1482_rs485_indicator_t indicator, int state);
int fbi1482_rs485_txen(int fd, uint i2c_bus, fbi1482_rs485_txen_t channel, int state);
#endif /*  _H_FBI1482LIB */
