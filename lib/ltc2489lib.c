/*=========================< begin file & file header >=======================
 *  References
 *
 *    filename : ltc2489lib.c
 *    author   : CG
 *    company  : IOxOS
 *    creation : June 23,2020
 *
 *----------------------------------------------------------------------------
 *  Description
 *
 *    That library contains a set of function to handle LTC2489 I2C devices.
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
#include <stdint.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/ioctl.h>

#include "../include/tsculib.h"
#include "ltc2489lib.h"

#include <linux/i2c-dev.h>
#ifndef I2C_M_RD
#include <linux/i2c.h>
#endif


/* LTC2489 I2C Adresses     -> 0x14, 0x15, 0x16, 0x17, 0x24, 0x25, 0x26, 0x27, 0x34 */

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * Function name : ltc2489_i2c_read
 * Prototype     : int
 * Parameters    : i2c_bus, i2c_add, len, data ptr
 * Return        : status of read operation
 *----------------------------------------------------------------------------
 * Description   : do an i2c read (this a workaround)
 *
 *++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

static int ltc2489_i2c_read(int i2c_bus, int i2c_add, int len, uint *data)
{
  int i2c_fd, rc;
  uint tmp;
  char i2c_dev[64];
  uint8_t buf[4];

  snprintf(i2c_dev, sizeof(i2c_dev), "/dev/i2c-%d", i2c_bus);

  i2c_fd = open(i2c_dev, O_RDWR);
  if (i2c_fd < 0) return(-1);

  rc = ioctl(i2c_fd, I2C_SLAVE, (i2c_add & 0x7f));
  if (rc<0)
  {
    close(i2c_fd);
    return (-1);
  }

  struct i2c_rdwr_ioctl_data msgset;
  struct i2c_msg msg;

  msg.addr = i2c_add;
  msg.flags = I2C_M_RD;
  msg.len = ((len<4) ? len : 4);
  msg.buf = buf;

  msgset.msgs = &msg;
  msgset.nmsgs = 1;

  rc = ioctl(i2c_fd, I2C_RDWR, &msgset);
  if (rc < 0)
  {
    close(i2c_fd);
    return(-1);
  }
  close(i2c_fd);

  if (data != NULL)
  {
    if (len > 0)
      tmp = buf[0];

    if (len > 1)
      tmp = ((tmp<<8) | buf[1]);

    if (len > 2)
      tmp = ((tmp<<8) | buf[2]);

    if (len > 3)
      tmp = ((tmp<<8) | buf[3]);

    (*data) = tmp;
  }

  return(1);
}

#if 0
/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * Function name : ltc2489_i2c_acquire
 * Prototype     : int
 * Parameters    : i2c_bus, i2c_add, len, data ptr
 * Return        : status of read operation
 *----------------------------------------------------------------------------
 * Description   : do an i2c read (this a workaround)
 *
 *++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

static int ltc2489_i2c_acquire(int i2c_bus, int i2c_add, int cmd, uint *data)
{
  int fd, rc;
  uint tmp;
  char i2c_dev[64];
  uint8_t buf[4];

  snprintf(i2c_dev, sizeof(i2c_dev), "/dev/i2c-%d", i2c_bus);

  fd = open(i2c_dev, O_RDWR);
  if (fd < 0) return(-1);

  rc = ioctl(fd, I2C_SLAVE, (i2c_add & 0x7f));
  if (rc<0)
  {
    close(fd);
    return (-1);
  }

  struct i2c_rdwr_ioctl_data msgset;
  struct i2c_msg msg;

  buf[0] = cmd;

  rc = write(fd, &buf, 1);
  if (rc < 0)
  {
     close(fd);
     return(rc);
  }

  /* wait 150 ms for conversion to complete */
  usleep(150000);

  msg.addr = i2c_add;
  msg.flags = I2C_M_RD;
  msg.len = 3;
  msg.buf = buf;

  msgset.msgs = &msg;
  msgset.nmsgs = 1;

  rc = ioctl(fd, I2C_RDWR, &msgset);
  if (rc < 0)
  {
    close(fd);
    return(rc);
  }
  close(fd);

  if (data != NULL)
  {
    tmp = buf[0];
    tmp = ((tmp<<8) | buf[1]);
    tmp = ((tmp<<8) | buf[2]);

    (*data) = tmp;
  }

  return(1);
}
#endif

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * Function name : ltc2489_device
 * Prototype     : int
 * Parameters    : i2c_bus, device_address
 * Return        : i2c command
 *----------------------------------------------------------------------------
 * Description   : Build I2C command
 *
 *++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

static int ltc2489_device(int i2c_bus, int device_address, int reg_size, int data_size)
{
  int device = 0;

  device  = TSC_I2C_CTL_PORT(i2c_bus);
  device |= TSC_I2C_CTL_ADD(device_address);
  if (reg_size > 0)
    device |= TSC_I2C_CTL_CMDSIZ(reg_size);
  if (data_size > 0)
    device |= TSC_I2C_CTL_DATSIZ(data_size);

  return device;
}

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * Function name : ltc2489_read
 * Prototype     : int
 * Parameters    : i2c_bus, device_address, cmd, data
 * Return        : status of operation
 *----------------------------------------------------------------------------
 * Description   : read data from an LTC2489
 *
 *++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

int ltc2489_read(int fd, int i2c_bus, int device_address, int cmd, int *data)
{
  int rc, device;
  uint tmp = 0, range, value;

  device = ltc2489_device(i2c_bus, device_address, 1, 0);

  /* start a conversion */
  rc = tsc_i2c_cmd(fd, device, cmd);

  /* may be a conversion generate by a GENERAL CALL (i2c add 0x77) ! */
  if (rc != 0 && (rc & TSC_I2C_CTL_EXEC_MASK) != TSC_I2C_CTL_EXEC_DONE)
  {
    usleep(150000);
    rc = tsc_i2c_cmd(fd, device, cmd);
  }

  if (rc != 0 && (rc & TSC_I2C_CTL_EXEC_MASK) != TSC_I2C_CTL_EXEC_DONE)
    return(-1);

  /* wait 150 ms for conversion to complete */
  usleep(150000);

  rc = ltc2489_i2c_read(i2c_bus, device_address, 3, &tmp);

  if (rc < 0)
    return (rc);

  if (data != NULL)
  {
    /* input range */
    range = ((tmp >> 22)&3);

    /* Vin < -FS */
    if (range == 0)
    {
      value = tmp;
      rc = -2;
    }
    /* Vin >= FS */
    else if (range == 3)
    {
      value = tmp;
      rc = -3;
    }
    else
    {
      /* data is 17-bit */
      value = ((tmp >> 6)&0x1ffff);

      /* sign extend when sign is negative */
      if ((tmp & (1<<23))==0)
        value |= 0xffff0000;
    }
    (*data) = (int)value;
  }

  return(rc);
}

