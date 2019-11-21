/*=========================< begin file & file header >=======================
 *  References
 *  
 *    filename : rdt1465lib.h
 *    author   : RH
 *    company  : IOxOS
 *    creation : august 09,2019
 *
 *----------------------------------------------------------------------------
 *  Description
 *
 *    This file contain the declarations of all exported functions define in
 *    pca9539lib.c
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

#ifndef _H_PCA9539LIB
#define _H_PCA9539LIB

#include <sys/types.h>

typedef enum {
  PCA9539_PORT_0 = 0,
  PCA9539_PORT_1 = 1
  } pca9539_port_t;

typedef enum {
  PCA9539_OUTPUT_PIN = 0,
  PCA9539_INPUT_PIN = 1
  } pca9539_pin_direction_t;

typedef enum {
  PCA9539_PIN_0_OUT = 0x00,
  PCA9539_PIN_0_IN  = 0x01,
  PCA9539_PIN_1_OUT = 0x00,
  PCA9539_PIN_1_IN  = 0x02,
  PCA9539_PIN_2_OUT = 0x00,
  PCA9539_PIN_2_IN  = 0x04,
  PCA9539_PIN_3_OUT = 0x00,
  PCA9539_PIN_3_IN  = 0x08,
  PCA9539_PIN_4_OUT = 0x00,
  PCA9539_PIN_4_IN  = 0x10,
  PCA9539_PIN_5_OUT = 0x00,
  PCA9539_PIN_5_IN  = 0x20,
  PCA9539_PIN_6_OUT = 0x00,
  PCA9539_PIN_6_IN  = 0x40,
  PCA9539_PIN_7_OUT = 0x00,
  PCA9539_PIN_7_IN  = 0x80
  } pca9539_port_direction_t;


int pca9539_set_port_direction(
  int fd,
  uint i2c_bus,
  uint device_address,
  pca9539_port_t port,
  pca9539_port_direction_t direction);

int pca9539_get_port_direction(
  int fd, 
  uint i2c_bus,
  uint device_address,
  pca9539_port_t port,
  pca9539_port_direction_t *direction);

int pca9539_set_port_state(
  int fd,
  uint i2c_bus,
  uint device_address,
  pca9539_port_t port,
  uint state);

int pca9539_get_port_state(
  int fd,
  uint i2c_bus,
  uint device_address,
  pca9539_port_t port,
  uint *state);

int pca9539_set_pin_direction(
  int fd,
  uint i2c_bus,
  uint device_address,
  pca9539_port_t port,
  uint pin,
  pca9539_pin_direction_t direction);

int pca9539_get_pin_direction(
  int fd,
  uint i2c_bus,
  uint device_address,
  pca9539_port_t port,
  uint pin,
  pca9539_pin_direction_t *direction);

int pca9539_set_pin_state(
  int fd,
  uint i2c_bus,
  uint device_address,
  pca9539_port_t port,
  uint pin,
  uint state);

int pca9539_get_pin_state(
  int fd,
  uint i2c_bus,
  uint device_address,
  pca9539_port_t port,
  uint pin,
  uint *state);


int pca9539_read(int fd, uint i2c_bus, uint device_address, uint reg, uint *data);


#endif /*  _H_PCA9539LIB */
