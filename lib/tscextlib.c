/*=========================< begin file & file header >=======================
 *  References
 *  
 *    filename : tscextlib.c
 *    author   : JFG, XP
 *    company  : IOxOS
 *    creation : october 21,2016
 *
 *----------------------------------------------------------------------------
 *  Description
 *
 *    That library contains a set of function to access device.
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

#include <stdlib.h>
#include <stdio.h>
#include <pty.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <errno.h>
#include <sys/mman.h>

#include "../include/tscioctl.h"
#include "../include/tsculib.h"
#include "../include/tscextlib.h"

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * Function name : tscext_bmr_conv_11bit_u
 * Prototype     : float
 * Parameters    : value
 * Return        : value converted
 *----------------------------------------------------------------------------
 * Description   : convert value on 11 bits unsigned
 *
 *++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

float
tscext_bmr_conv_11bit_u( unsigned short val)
{
  unsigned short l;
  short h;

  l = val & (unsigned short)0x7ff;
  h = (short)(val >> 11);
  h |= (short)0xffe0;
  h =  (short)~h;
  h++;
  return(((float)l/(float)(1 << h)));
}

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * Function name : tscext_bmr_conv_11bit_s
 * Prototype     : float
 * Parameters    : value
 * Return        : value converted
 *----------------------------------------------------------------------------
 * Description   : convert value on 11 bits signed
 *
 *++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

float
tscext_bmr_conv_11bit_s( unsigned short val)
{
  short h,l;

  l = (short)(val & (unsigned short)0x7ff);
  if( l & 0x400) l |= (short)0xf800;
  h = (short)(val >> 11);
  h |= (short)0xffe0;
  h =  (short)~h;
  h++;
  return(((float)l/(float)(1 << h)));
}

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * Function name : tscext_bmr_conv_16bit_u
 * Prototype     : float
 * Parameters    : value to convert
 * Return        : value converted
 *----------------------------------------------------------------------------
 * Description   : convert a 16 bits value unsigned
 *
 *++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

float
tscext_bmr_conv_16bit_u( unsigned short val)
{
  return(((float)val/(float)(1 << 13)));
}

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * Function name : tscext_bmr_read
 * Prototype     : int
 * Parameters    : bmr, register, data read, count
 * Return        : status of operation
 *----------------------------------------------------------------------------
 * Description   : read from bmr
 *
 *++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

int
tscext_bmr_read(int fd, uint bmr,
	      uint reg,
	      uint *data,
	      uint cnt)
{
  int device;

  device = 0;
  if( cnt > 4) return( -1);
  switch( bmr)
  {
    case 0:
    {
      device |= 0x24;
      break;
    }
    case 1:
    {
      device |= 0x53;
      break;
    }
    case 2:
    {
      device |= 0x63;
      break;
    }
    default:
    {
      return(-1);
    }
  }
  device = I2C_DEV( device, 1, TSC_I2C_CTL_CMDSIZ(1)|TSC_I2C_CTL_DATSIZ(cnt));

  return( tsc_i2c_read(fd, device, reg, data));
}

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * Function name : tscext_bmr_write
 * Prototype     : int
 * Parameters    : bmr, register, data, count
 * Return        : status of operation
 *----------------------------------------------------------------------------
 * Description   : write into bmr
 *
 *++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

int
tscext_bmr_write(int fd, uint bmr,
	       uint reg,
	       uint data,
	       uint cnt)
{
  int device;

  device = 0;
  if( cnt > 4) return( -1);
  switch( bmr)
  {
    case 0:
    {
      device = 0x24;
      break;
    }
    case 1:
    {
      device = 0x53;
      break;
    }
    case 2:
    {
      device = 0x63;
      break;
    }
    default:
    {
      return(-1);
    }
  }
  device = I2C_DEV( device, 1, TSC_I2C_CTL_CMDSIZ(1)|TSC_I2C_CTL_DATSIZ(2));

  return( tsc_i2c_write(fd, device, reg, data));
}

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * Function name : tscext_csr_rd
 * Prototype     : int
 * Parameters    : register
 * Return        : data read
 *----------------------------------------------------------------------------
 * Description   : read a data in csr
 *
 *++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

int
tscext_csr_rd(int fd, int reg)
{
  int data;
  tsc_csr_read(fd, reg, &data);
  return( data);
}

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * Function name : tscext_csr_wr
 * Prototype     : void
 * Parameters    : register and data
 * Return        : void
 *----------------------------------------------------------------------------
 * Description   : write in csr space
 *
 *++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

void
tscext_csr_wr(int fd, int reg,
	    int data)
{
  tsc_csr_write(fd, reg, &data);
}
