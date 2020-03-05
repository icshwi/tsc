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
#include <adclib.h>

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * Function name : adc_csr_rd
 * Prototype     : int
 * Parameters    : fmc	fmc index (1 or 2)
 *                 csr	register index
 * Return        : register content
 *----------------------------------------------------------------------------
 * Description   : Read ADC CSR register referred by csr
 *
 *++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

int 
adc_csr_rd(int fd, int fmc, int csr)
{
  int addr;
  int data;

  addr = ADC_CSR_ADDR( csr);
  if( fmc == ADC_FMC2) addr +=  ADC_CSR_OFF_FMC2;
  tsc_csr_read(fd,  addr, &data);
  return( data);
}

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * Function name : adc_csr_wr
 * Prototype     : void
 * Parameters    : fmc  fmc index (1 or 2)
 *                 csr  register index
 *                 data  data to be written in register
 * Return        : void
 *----------------------------------------------------------------------------
 * Description   : Write ADC CSR register referred by csr with data
 *
 *++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

void 
adc_csr_wr(int fd, int fmc, int csr, int data)
{
  int addr;

  addr = ADC_CSR_ADDR( csr);
  if( fmc == ADC_FMC2) addr +=  ADC_CSR_OFF_FMC2;
  tsc_csr_write(fd, addr, &data);

  return;
}

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * Function name : adc_identify
 * Prototype     : int
 * Parameters    : fmc FMC identifier (1 or 2)
 * Return        : ADC signature (expect ADC_SIGN_ID)
 *----------------------------------------------------------------------------
 * Description   : returns the content of ADC signature register (index 0x80)
 *
 *++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

int 
adc_identify(int fd, int fmc)
{
  int id;

  id = adc_csr_rd(fd, fmc,  ADC_CSR_SIGN);

  return( id);
}
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

int
adc_spi_read(int fd, int fmc, int cmd, int reg)
{
  int tmo, data;

  cmd |=  0x80000000 | reg;
  tmo = 1000;
  //printf("adc_spi_read( %x, %x, %x)\n", fmc, cmd, reg);
  adc_csr_wr(fd, fmc, ADC_CSR_SERIAL, cmd);
  while( --tmo)
  {
    if( !(adc_csr_rd(fd, fmc, ADC_CSR_SERIAL) & 0x80000000)) break;
  }
  if( !tmo)
  {
    printf("adc_spi_read() : cmd = %08x -> timeout...\n", cmd);
    return(-1);
  }
  data = adc_csr_rd(fd, fmc, ADC_CSR_SERIAL+1);
  //printf("cmd = %08x - data = %08x\n", cmd, data);

  return( data);
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

int
adc_spi_write(int fd, int fmc, int cmd, int reg, int data)
{
  int tmo;

  cmd |=  0xc0000000 | reg;
  tmo = 1000;
  //printf("adc_spi_read( %x, %x, %x)\n", fmc, cmd, reg);
  adc_csr_wr(fd, fmc, ADC_CSR_SERIAL+1, data);
  data = adc_csr_rd(fd, fmc, ADC_CSR_SERIAL+1);
  adc_csr_wr(fd, fmc, ADC_CSR_SERIAL, cmd);
  while( --tmo)
  {
    if( !(adc_csr_rd(fd, fmc, ADC_CSR_SERIAL) & 0x80000000)) break;
  }
  if( !tmo)
  {
    printf("adc_spi_read() : cmd = %08x -> timeout...\n", cmd);
    return(-1);
  }

  return( 0);
}

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * Function name : adc_i2c_read
 * Prototype     : int
 * Parameters    : fmc  FMC identifier (1 or 2)
 *                 dev  I2C device address
 *                 reg  register index
 * Return        : content of register
 *----------------------------------------------------------------------------
 * Description   : returns the content of register reg located in the resource
 * identified by dev.
 *
 *++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

uint
adc_i2c_read(int fd, int fmc, uint device, uint reg)
{
  int status;
  uint data;

  if( fmc == ADC_FMC2)
  {
    device |= 0xa0000000;
  }
  else
  {
    device |= 0x80000000;
  }
  status = tsc_i2c_read(fd, device, reg, &data);
  if( status < 0)
  {
    return(-1);
  }
  return( data);
}    

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * Function name : adc_i2c_write
 * Prototype     : int
 * Parameters    : fmc  fmc number (1 or 2)
 *                 dev  I2C device address
 *                 reg  register index
 *                 data  data to be written in register
 * Return        : 0  if I2C cycle OK
 * 				  -1  if error
 *----------------------------------------------------------------------------
 * Description   : write data in the register reg located in the resource 
 * identified by dev
 *
 *++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

int
adc_i2c_write(int fd, int fmc, uint device, uint reg, uint data)
{
  int status;

  if( fmc == ADC_FMC2)
  {
    device |= 0xa0000000;
  }
  else
  {
    device |= 0x80000000;
  }
  status = tsc_i2c_write(fd, device, reg, data);
  if( status < 0)
  {
    return(-1);
  }
  return( 0);
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


int
adc_gpio_trig(int fd, int fmc)
{
  adc_csr_wr(fd, fmc, ADC_CSR_GPIO, 0x10); /* set output low */
  adc_csr_wr(fd, fmc, ADC_CSR_GPIO, 0x20); /* set output high */
  adc_csr_wr(fd, fmc, ADC_CSR_GPIO, 0x10); /* set output low */
  return(0);
}


