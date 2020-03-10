/*=========================< begin file & file header >=======================
 *  References
 *  
 *    filename : i2ceepromlib.c
 *    author   : RH
 *    company  : IOxOS
 *    creation : December 11,2019
 *
 *----------------------------------------------------------------------------
 *  Description
 *
 *    That library contains a set of function to handle I2C EEPROM devices.
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

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <errno.h>
#include "../include/tsculib.h"
#include "../include/i2ceepromlib.h"


/* private functions prototypes */

int i2ceeprom_device(uint i2c_bus, uint device_address, uint memory_address_bit_width);


/**********************************************************************
 * private functions
 **********************************************************************/

int i2ceeprom_device(uint i2c_bus, uint device_address, uint memory_address_bit_width)
{
  int rs     = (memory_address_bit_width >> 3) & 0x03;
  int ds     = 1;
  int device = 0;

  device  = (i2c_bus & 7) << 29;
  device |= device_address & 0x7f;
  device |= ((rs - 1) & 3) << 16;
  device |= ((ds - 1) & 3) << 18;

  return device;
}

/**********************************************************************
 * public functions
 **********************************************************************/

int i2ceeprom_read(int fd, uint i2c_bus, uint device_address, uint memory_address, uint memory_address_bit_width,
                   uint data_byte_count, unsigned char *buffer)
{
  int retval = 0;
  unsigned int data = 0;
  int device = i2ceeprom_device(i2c_bus, device_address, memory_address_bit_width);

  if (!buffer) return EINVAL;
  if (memory_address_bit_width != 8 && memory_address_bit_width != 16) return EINVAL;

  while (!retval && data_byte_count--)
  {
    retval = tsc_i2c_read(fd, device, memory_address++, &data);
    // tsc_i2c_read() is broken -> ignore return value
    retval = 0;
    *(buffer++) = (char)data;
  }
  return retval;
}


int i2ceeprom_write(int fd, uint i2c_bus, uint device_address, uint memory_address, uint memory_address_bit_width,
                    uint data_byte_count, unsigned char *buffer)
{
  int retval = 0;
  unsigned int data = 0;
  int device = i2ceeprom_device(i2c_bus, device_address, memory_address_bit_width);

  if (!buffer) return EINVAL;
  if (memory_address_bit_width != 8 && memory_address_bit_width != 16) return EINVAL;

  while (!retval && data_byte_count--)
  {
    data = *(buffer++);
    retval = tsc_i2c_write(fd, device, memory_address++, data);
    // tsc_i2c_read() is broken -> ignore return value
    retval = 0;
    usleep(20000);
  }
  return retval;
}

