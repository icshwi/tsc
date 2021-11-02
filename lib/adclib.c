/*=========================< begin file & file header >=======================
 *  References
 *
 *    filename : adclib.c
 *    author   : JFG, XP
 *    company  : IOxOS
 *    creation : october 18,2018
 *
 *----------------------------------------------------------------------------
 *  Description
 *
 *    That library contains a set of function to access the TOSCA II XUSER
 *    FASTSCOPE ADC data acquisition logic.
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

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <tsculib.h>
#include <tscextlib.h>
#include <fmclib.h>
#include <adclib.h>


/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * Function name : adc_spi_read
 * Prototype     : int
 * Parameters    : fmc  fmc number (1 or 2)
 *                 cmd  SPI device address
 *                 reg  register index
 * Return        : content of register
 *----------------------------------------------------------------------------
 * Description   : returns the content of register reg located in the resource
 * identified by cmd
 *
 *++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

int adc_spi_read(int fd, int fmc, int cmd, int reg, int *data)
{
  int to, tmp, ret;

  to = 1000;
  cmd |=  ((1<<31) | (reg&0xffff));
  //printf("adc_spi_read( %x, %x, %x)\n", fmc, cmd, reg);

  ret = fmc_csr_write(fd, fmc, ADC_CSR_SERIAL, cmd);
  if (ret < 0)
    return(ret);

  while (--to)
  {
    ret = fmc_csr_read(fd, fmc, ADC_CSR_SERIAL, &tmp);
    if (ret < 0)
      return (ret);

    if (!(tmp & (1<<31)))
      break;
  }

  if (!to)
  {
    fprintf(stderr, "adc_spi_read() : cmd = %08x -> timeout...\n", cmd);
    return(-1);
  }

  ret = fmc_csr_read(fd, fmc, ADC_CSR_SERIAL+1, &tmp);
  if (ret < 0)
    return (ret);

  //printf("cmd = %08x - data = %08x\n", cmd, data);

  if (data != NULL)
  {
    *data = tmp;
  }

  return(0);
}

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * Function name : adc_spi_write
 * Prototype     : int
 * Parameters    : fmc  fmc number (1 or 2)
 *                 cmd  SPI device address
 *                 reg  register index
 *                 data  data to be written in register
 * Return        : 0  if SPI command OK
 * 				  -1  in case of timeout
 *----------------------------------------------------------------------------
 * Description   : writes data in the register reg located in the resource
 * identified by cmd
 *
 *++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

int adc_spi_write(int fd, int fmc, int cmd, int reg, int data)
{
  int to, tmp, ret;

  to = 1000;
  cmd |=  ((1<<31)|(1<<30)|(reg&0xffff));

  //printf("adc_spi_read( %x, %x, %x)\n", fmc, cmd, reg);

  ret = fmc_csr_write(fd, fmc, ADC_CSR_SERIAL+1, data);
  if (ret<0)
    return (ret);

  ret = fmc_csr_write(fd, fmc, ADC_CSR_SERIAL, cmd);
  if (ret<0)
    return (ret);

  while(--to)
  {
    ret = fmc_csr_read(fd, fmc, ADC_CSR_SERIAL, &tmp);
    if (ret<0)
      return(ret);

    if (!(tmp & (1<<31)))
      return(0);
  }

  fprintf(stderr, "adc_spi_read() : cmd = %08x -> timeout...\n", cmd);
  return(-1);
}


/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * Function name : adc_gpio_trig
 * Prototype     : int
 * Parameters    : fmc number (1 or 2)
 * Return        : fail/success
 *----------------------------------------------------------------------------
 * Description   :
 *
 *++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

int adc_gpio_trig(int fd, int fmc)
{
  fmc_csr_write(fd, fmc, ADC_CSR_GPIO, 0x10); /* set output low */
  fmc_csr_write(fd, fmc, ADC_CSR_GPIO, 0x20); /* set output high */
  fmc_csr_write(fd, fmc, ADC_CSR_GPIO, 0x10); /* set output low */
  return(0);
}

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * Function name : adc_read_tmp102
 * Prototype     : int
 * Parameters    : fmc number (1 or 2)
 *                 i2c device address
 * Return        : fail/success
 *----------------------------------------------------------------------------
 * Description   : read temperature from TMP102 I2C device
 *
 *++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

int adc_read_tmp102(int fd, int fmc, uint dev, uint *temp, uint *temp_lo, uint *temp_hi)
{
  int ret;
  uint ctl, loc_temp, loc_temp_lo, loc_temp_hi;

  ret = fmc_i2c_read(fd, fmc, dev, 1, &ctl);
  if (ret < 0)
    return(ret);

  ret = fmc_i2c_read(fd, fmc, dev, 0, &loc_temp);
  if (ret < 0)
    return(ret);

  ret = fmc_i2c_read(fd, fmc, dev, 2, &loc_temp_lo);
  if (ret < 0)
    return(ret);

  ret = fmc_i2c_read(fd, fmc, dev, 3, &loc_temp_hi);
  if (ret < 0)
    return(ret);

  if (loc_temp & 0x100)
  {
    loc_temp    = (((loc_temp    << 5) & 0x1fe0) | ((loc_temp    >> 11) & 0x1f));
    loc_temp_lo = (((loc_temp_lo << 5) & 0x1fe0) | ((loc_temp_lo >> 11) & 0x1f));
    loc_temp_hi = (((loc_temp_hi << 5) & 0x1fe0) | ((loc_temp_hi >> 11) & 0x1f));
  }
  else
  {
    loc_temp    = (((loc_temp    << 4) & 0xff0) | ((loc_temp    >> 12) & 0xf));
    loc_temp_lo = (((loc_temp_lo << 4) & 0xff0) | ((loc_temp_lo >> 12) & 0xf));
    loc_temp_hi = (((loc_temp_hi << 4) & 0xff0) | ((loc_temp_hi >> 12) & 0xf));
  }

  if (temp != NULL)
    (*temp) = loc_temp;

  if (temp_lo != NULL)
    (*temp_lo) = loc_temp_lo;

  if (temp_hi != NULL)
    (*temp_hi) = loc_temp_hi;

  return(1);
}


/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * Function name : adc_set_tmp102
 * Prototype     : int
 * Parameters    : fmc number (1 or 2)
 *                 i2c device address
 * Return        : fail/success
 *----------------------------------------------------------------------------
 * Description   : set temperature limit T_low and T_high from
 *                 TMP102 I2C device
 *
 *++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

int adc_set_tmp102(int fd, int fmc, uint dev, uint *temp_lo, uint *temp_hi)
{
  uint temp, lo, hi, t;
  int ret;

  ret = fmc_i2c_read(fd, fmc, dev, 0, &temp);
  if (ret < 0)
    return(ret);

  if (temp_lo != NULL)
  {
    lo = (*temp_lo);

    /* Extended Mode */
    if (temp & 0x100)
    {
      t = (((lo>>5) & 0xff) | ((lo<<11) & 0xf800));
    }
    /* Normal Mode */
    else
    {
      t = (((lo>>4) & 0xff) | ((lo<<12) & 0xf000));
    }
    ret = fmc_i2c_write(fd, fmc, dev, 2, t);
    if (ret < 0)
      return(ret);
  }

  if (temp_hi != NULL)
  {
    hi = (*temp_hi);

    /* Extended Mode */
    if (temp & 0x100)
    {
      t = (((hi>>5) & 0xff) | ((hi<<11) & 0xf800));
    }
    /* Normal Mode */
    else
    {
      t= (((hi>>4) & 0xff) | ((hi<<12) & 0xf000));
    }
    ret = fmc_i2c_write(fd, fmc, dev, 3, t);
    if (ret < 0)
      return(ret);
  }

  return (1);
}
