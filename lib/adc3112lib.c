/*=========================< begin file & file header >=======================
 *  References
 *  
 *    filename : adc3112lib.c
 *    author   : JFG, XP
 *    company  : IOxOS
 *    creation : october 18,2018
 *
 *----------------------------------------------------------------------------
 *  Description
 *
 *    That library contains a set of function to access the TOSCA II XUSER
 *    FASTSCOPE ADC3112 data acquisition logic.
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

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <tsculib.h>
#include <tscextlib.h>
#include <adc3112lib.h>

int adc3112_verbose_flag = 0;

int 
adc3112_set_verbose(int vf)
{
  adc3112_verbose_flag = vf;
  return(adc3112_verbose_flag);
}

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * Function name : adc3112_XXX
 * Prototype     : int
 * Parameters    : void
 * Return        : 
 *----------------------------------------------------------------------------
 * Description   : 
 *
 *++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

int 
adc3112_XXX(void)
{

  return( 0);
}

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * Function name : adc3112_csr_rd
 * Prototype     : int
 * Parameters    : register index
 * Return        : register content
 *----------------------------------------------------------------------------
 * Description   : 
 *
 *++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

int 
adc3112_csr_rd(int fd, int fmc,
		int csr)
{
  int addr;
  int data;

  addr = ADC3112_CSR_ADDR( csr);
  if( fmc == ADC3112_FMC2) addr +=  ADC3112_CSR_OFF_FMC2;
  tsc_csr_read(fd, addr, &data);
  return( data);
}

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * Function name : adc3112_csr_wr
 * Prototype     : vois
 * Parameters    : register index
 * Return        : register content
 *----------------------------------------------------------------------------
 * Description   : 
 *
 *++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

void 
adc3112_csr_wr(int fd, int fmc,
		int csr,
	        int data)
{
  int addr;

  addr = ADC3112_CSR_ADDR( csr);
  if( fmc == ADC3112_FMC2) addr +=  ADC3112_CSR_OFF_FMC2;
  tsc_csr_write(fd, addr, &data);

  return;
}

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * Function name : adc3112_identify
 * Prototype     : int
 * Parameters    : none
 * Return        : fail/success
 *----------------------------------------------------------------------------
 * Description   : 
 *
 *++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

int 
adc3112_identify(int fd, int fmc)
{
  int id;

  id = adc3112_csr_rd(fd, fmc,  ADC3112_CSR_SIGN);

  return( id);
}

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * Function name : adc3112_spi_read
 * Prototype     : int
 * Parameters    : fmc number (1 or 2)
 *                 SPI device address
 *                 register index
 * Return        : data
 *----------------------------------------------------------------------------
 * Description   : 
 *
 *++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

int
adc3112_spi_read(int fd, int fmc,
		  int cmd,
	 	  int reg)
{
  int tmo, data;

  cmd |=  0x80000000 | reg;
  tmo = 1000;
  //printf("adc3112_spi_read( %x, %x, %x)\n", fmc, cmd, reg);
  adc3112_csr_wr(fd, fmc, ADC3112_CSR_SERIAL, cmd);
  while( --tmo)
  {
    if( !(adc3112_csr_rd(fd, fmc, ADC3112_CSR_SERIAL) & 0x80000000)) break;
  }
  if( !tmo)
  {
    printf("adc3112_spi_read() : cmd = %08x -> timeout...\n", cmd);
    return(-1);
  }
  data = adc3112_csr_rd(fd, fmc, ADC3112_CSR_SERIAL+1);
  //printf("cmd = %08x - data = %08x\n", cmd, data);

  return( data);
}

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * Function name : adc3112_spi_write
 * Prototype     : int
 * Parameters    : fmc number (1 or 2)
 *                 SPI device address
 *                 register index
 *                 data
 * Return        : fail/success
 *----------------------------------------------------------------------------
 * Description   : 
 *
 *++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

int
adc3112_spi_write(int fd, int fmc,
		   int cmd,
	 	   int reg,
		   int data)
{
  int tmo;

  cmd |=  0xc0000000 | reg;
  tmo = 1000;
  //printf("adc3112_spi_read( %x, %x, %x)\n", fmc, cmd, reg);
  adc3112_csr_wr(fd, fmc, ADC3112_CSR_SERIAL+1, data);
  data = adc3112_csr_rd(fd, fmc, ADC3112_CSR_SERIAL+1);
  adc3112_csr_wr(fd, fmc, ADC3112_CSR_SERIAL, cmd);
  while( --tmo)
  {
    if( !(adc3112_csr_rd(fd, fmc, ADC3112_CSR_SERIAL) & 0x80000000)) break;
  }
  if( !tmo)
  {
    printf("adc3112_spi_read() : cmd = %08x -> timeout...\n", cmd);
    return(-1);
  }

  return( 0);
}

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * Function name : adc3112_i2c_read
 * Prototype     : int
 * Parameters    : fmc number (1 or 2)
 *                 I2C device address
 *                 register index
 * Return        : data
 *----------------------------------------------------------------------------
 * Description   : 
 *
 *++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

uint
adc3112_i2c_read(int fd, int fmc,
		  uint device,
	 	  uint reg)
{
  int status;
  uint data;

  if( fmc == ADC3112_FMC2)
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
 * Function name : adc3112_i2c_write
 * Prototype     : int
 * Parameters    : fmc number (1 or 2)
 *                 I2C device address
 *                 register index
 * Return        : data
 *----------------------------------------------------------------------------
 * Description   : 
 *
 *++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

int
adc3112_i2c_write(int fd, int fmc,
		   uint device,
	 	   uint reg,
		   uint data)
{
  int status;

  if( fmc == ADC3112_FMC2)
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
 * Function name : adc3112_eeprom_read
 * Prototype     : int
 * Parameters    : fmc number (1 or 2)
 *                 pointer to data buffer
 *                 eeprom offset
 *                 size
 * Return        : data
 *----------------------------------------------------------------------------
 * Description   : 
 *
 *++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

int
adc3112_eeprom_read(int fd, int fmc,
		     char *buf,
	 	     int offset,
		     int size)
{
  int i;

  for( i = 0; i < size; i++)
  {
    buf[i] = (char)adc3112_i2c_eeprom_read(fd, fmc, tsc_swap_16(offset+i));
  }
  return(size);
}

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * Function name : adc3112_eeprom_write
 * Prototype     : int
 * Parameters    : fmc number (1 or 2)
 *                 pointer to data buffer
 *                 eeprom offset
 *                 size
 * Return        : data
 *----------------------------------------------------------------------------
 * Description   : 
 *
 *++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

int
adc3112_eeprom_write(int fd, int fmc,
		      char *buf,
	 	      int offset,
		      int size)
{
  int i;
  uint data;

  for( i = 0; i < size; i++)
  {
    data = (uint)buf[i];
    adc3112_i2c_eeprom_write(fd, fmc, tsc_swap_16(offset+i), data);
  }
  return(size);
}

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * Function name : adc3112_reset
 * Prototype     : int
 * Parameters    : fmc number (1 or 2)
 * Return        : fail/success
 *----------------------------------------------------------------------------
 * Description   : 
 *
 *++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

void
adc3112_reset(int fd, int fmc)
{

  adc3112_csr_wr(fd, fmc, ADC3112_CSR_CTL, 0x100);
  usleep( 20000);
  adc3112_csr_wr(fd, fmc, ADC3112_CSR_CTL, 0x0);
  usleep( 50000);

  return;
}

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * Function name : adc3112_xra1404_init
 * Prototype     : int
 * Parameters    : fmc number (1 or 2)
 * Return        : fail/success
 *----------------------------------------------------------------------------
 * Description   : 
 *
 *++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

void
adc3112_xra1404_init(int fd, int fmc)
{
  adc3112_spi_xratrig_write(fd, fmc, ADC3112_XRA_GCR, 0x00); /* XRA1404 GCR Initialisation -> All outputs       */
  adc3112_spi_xratrig_write(fd, fmc, ADC3112_XRA_PUR, 0xb8); /* XRA1404 PUR Initialisation -> Pull-up 3.3V      */
  adc3112_spi_xratrig_write(fd, fmc, ADC3112_XRA_TSCR, 0x07); /* XRA1404 TSCR Initialisation -> High impedance   */
  adc3112_spi_xratrig_write(fd, fmc, 0x2, 0x20); /* XRA1404 OCR ADC_ENABLE = 0  -> ADS5409 ENABLE = '0'  */

  adc3112_spi_xra01_write(fd, fmc, ADC3112_XRA_GCR, 0x00);   /* XRA1404 GCR Initialisation -> All outputs   */
  adc3112_spi_xra01_write(fd, fmc, ADC3112_XRA_PUR, 0xff);   /* XRA1404 PUR Initialisation -> Pull-up 3.3V   */
  adc3112_spi_xra01_write(fd, fmc, ADC3112_XRA_TSCR, 0x00);   /* XRA1404 TSCR Initialisation -> All totem-pole   */

  adc3112_spi_xra23_write(fd, fmc, ADC3112_XRA_GCR, 0x00);   /* XRA1404 GCR Initialisation -> All outputs   */
  adc3112_spi_xra23_write(fd, fmc, ADC3112_XRA_PUR, 0xff);   /* XRA1404 PUR Initialisation -> Pull-up 3.3V   */
  adc3112_spi_xra23_write(fd, fmc, ADC3112_XRA_TSCR, 0x00);   /* XRA1404 TSCR Initialisation -> All totem-pole   */

  adc3112_spi_dac_write(fd, fmc, 0x38, 0x0001);   /*  Enable Internal Reference & reset DACs to gain = 2  */
  adc3112_spi_dac_write(fd, fmc, 0x02, 0x0003);   /*  Gain DAC-B = 1 and Gain DAC-A = 1 */
  adc3112_spi_dac_write(fd, fmc, 0x18, 0x3fff);   /*  Write to DAC-A input register and update DAC-A minimum level (max threshold for GPIO, min for channels!)  */
  adc3112_spi_dac_write(fd, fmc, 0x19, 0x7fff);   /*  Write to DAC-B input register and update DAC-B; mid-range (0 delay)  */

  return;

}
/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * Function name : adc3112_xratrig_enable
 * Prototype     : int
 * Parameters    : fmc number (1 or 2)
 * Return        : fail/success
 *----------------------------------------------------------------------------
 * Description   : 
 *
 *++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

void
adc3112_xratrig_enable(int fd, int fmc,
			int src,
			int level)
{
  int ocr, gcr;

  gcr = ADC3112_XRA_TRIG_DIS;
  if( src > 0)
  {
    ocr = ADC3112_XRA_TRIG_RISE;
  }
  else 
  {
    ocr = 0;
    src = -src;
  }
  switch( src)
  {
    case ADC3112_XRA_TRIG_GPIO:
    {
      gcr = ADC3112_XRA_TRIG_DIS;
      ocr |= ADC3112_XRA_TRIG_GPIO;
      break;
    }
    case ADC3112_XRA_TRIG_ADC0:
    {
      gcr = ADC3112_XRA_TRIG_ADC0;
      ocr |= ADC3112_XRA_TRIG_ADC;
      break;
    }
    case ADC3112_XRA_TRIG_ADC1:
    {
      gcr = ADC3112_XRA_TRIG_ADC1;
      ocr |= ADC3112_XRA_TRIG_ADC;
      break;
    }
    case ADC3112_XRA_TRIG_ADC2:
    {
      gcr = ADC3112_XRA_TRIG_ADC2;
      ocr |= ADC3112_XRA_TRIG_ADC;
      break;
    }
    case ADC3112_XRA_TRIG_ADC3:
    {
      gcr = ADC3112_XRA_TRIG_ADC3;
      ocr |= ADC3112_XRA_TRIG_ADC;
      break;
    }
    default:
    {
      return;
    }
  }
  adc3112_spi_xratrig_write(fd, fmc, ADC3112_XRA_GCR, ADC3112_XRA_TRIG_HIZ);
  adc3112_spi_xratrig_write(fd, fmc, ADC3112_XRA_OCR, ocr);
  adc3112_spi_xratrig_write(fd, fmc, ADC3112_XRA_GCR, gcr);
  adc3112_spi_dac_write(fd, fmc, 0x18, level);
  return;                
}                        

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * Function name : adc3112_xratrig_disable
 * Prototype     : int
 * Parameters    : fmc number (1 or 2)
 * Return        : fail/success
 *----------------------------------------------------------------------------
 * Description   : 
 *
 *++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

void
adc3112_xratrig_disable(int fd, int fmc)
{
  adc3112_spi_xratrig_write(fd, fmc, 0x2, 0x7); /* XRA1404 OCR    ADC_ENABLE = 0  -> ADS5409 ENABLE = '0'   */
  adc3112_spi_xratrig_write(fd, fmc, ADC3112_XRA_GCR, ADC3112_XRA_TRIG_DIS);
  adc3112_spi_xratrig_write(fd, fmc, ADC3112_XRA_OCR, 0x20);
  return;
}

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * Function name : adc3112_lmk_init
 * Prototype     : int
 * Parameters    : fmc number (1 or 2)
 * Return        : fail/success
 *----------------------------------------------------------------------------
 * Description   : 
 *
 *++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

void
adc3112_lmk_init(int fd, int fmc,
		  int lmk_reg[])
{
  int reg;

  adc3112_spi_lmk_write(fd, fmc, 0x0, 0x00020000); /* LMK04803B_R00 Generate a programmable RESET to the LMK04803B   */
  usleep( 50000);
  adc3112_spi_lmk_write(fd, fmc, 0xb, lmk_reg[0xb]);
  usleep( 50000);
  for( reg = 0; reg <= 0x10; reg++)
  {
    //printf("lmk reg %02d : %08x..", reg, adc3112_spi_lmk_read( fmc, reg));
    if( reg != 0xb)
    {
      adc3112_spi_lmk_write(fd, fmc, reg, lmk_reg[ reg]);
    }
    //printf("%08x\n", adc3112_spi_lmk_read( fmc, reg));
  }
  for( reg = 0x11; reg < 0x18; reg++)
  {
    //printf("lmk reg %02d : %08x\n", reg, adc3112_spi_lmk_read( fmc, reg));
  }
  for( reg = 0x18; reg < 0x1f; reg++)
  {
    //printf("lmk reg %02d : %08x..", reg, adc3112_spi_lmk_read( fmc, reg));
    adc3112_spi_lmk_write(fd, fmc, reg, lmk_reg[ reg]);
    //printf("%08x\n", adc3112_spi_lmk_read( fmc, reg));
  }
  /* Synchronisation forced */
  adc3112_spi_lmk_write(fd, fmc, 0xb, lmk_reg[ 0xb] & 0xfffeffff); /* clk_OUT0/clk_OUT6 running */
  adc3112_spi_lmk_write(fd, fmc, 0x1f, lmk_reg[ 0x1f]);

  /* --------------------------------------------*/
  /* Enable On-board 100 MHz clock from +OSC575  */
  /* --------------------------------------------*/
  adc3112_csr_wr(fd, fmc, ADC3112_CSR_GPIO, 0x00000070);  /* OFCT -> LMK04803B CLKout5                  */
  adc3112_csr_wr(fd, fmc, ADC3112_CSR_LED, 0x80000003);   /* FP Led flashing + CCHD575-100MHz  Power-on */
  usleep( 20000);
  adc3112_spi_lmk_write(fd, fmc, 0x1e, lmk_reg[ 0x1e]);             /*  LMK04803B_R30 PLL2 P/N Recallibration */
  adc3112_spi_lmk_write(fd, fmc, 0x0c, lmk_reg[ 0x0c] | 0xc00000);  /*  LMK04803B_R12 LD pin PLL1&PLL2 DLD / force a SYNC event  */
  usleep( 20000);

  return;
}
void
adc3112_lmk_dump(int fd, int fmc)
{
  int reg, data;

  printf("\nDump LMK registers:");
  for( reg = 0; reg < 32; reg += 4)
  {
    int i;

    printf("\n%2x : ", reg);
    for( i = 0; i <  4; i++)
    {
      data = adc3112_spi_lmk_read(fd, fmc, reg+i);
      printf("%08x ", data);
    }
  }
  printf("\n");

  return;
}

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * Function name : adc3112_ads5409_reset
 * Prototype     : int
 * Parameters    : fmc number (1 or 2)
 * Return        : fail/success
 *----------------------------------------------------------------------------
 * Description   : 
 *
 *++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

void
adc3112_ads5409_reset(int fd, int fmc,
		       int filter)
{

  /* *****************************************************************/
  /* ADS5409 Channel 01 U737                                         */      
  /* *****************************************************************/
  adc3112_spi_ads01_write(fd, fmc, 0x2c, 0xD2F0); /* ADS5409_R2c 4 SW RESET  !!!NEW JB20140626   */
  usleep( 50000);
  if( filter == ADC3112_ADS_FILTER_LOW)
  {
    adc3112_spi_ads01_write(fd, fmc, 0x0, 0xc000); /* ADS5409_R00 4 Wire SPI + decimation filter low  */
  }
  else if( filter == ADC3112_ADS_FILTER_HIGH)
  {
    adc3112_spi_ads01_write(fd, fmc, 0x0, 0xd200); /* ADS5409_R00 4 Wire SPI + decimation filter high  */
  }
  else 
  {
    adc3112_spi_ads01_write(fd, fmc, 0x0, 0x8000); /* ADS5409_R00 4 Wire SPI + no filter  */
  }
  /* *****************************************************************/
  /* ADS5409 Channel 23 U747                                         */      
  /* *****************************************************************/
  adc3112_spi_ads23_write(fd, fmc, 0x2c, 0xD2F0); /* ADS5409_R2c 4 SW RESET  !!!NEW JB20140626   */
  usleep( 50000);
  if( filter == ADC3112_ADS_FILTER_LOW)
  {
    adc3112_spi_ads23_write(fd, fmc, 0x0, 0xc000); /* ADS5409_R00 4 Wire SPI + decimation filter low  */
  }
  else if( filter == ADC3112_ADS_FILTER_HIGH)
  {
    adc3112_spi_ads23_write(fd, fmc, 0x0, 0xd200); /* ADS5409_R00 4 Wire SPI + decimation filter high  */
  }
  else 
  {
    adc3112_spi_ads23_write(fd, fmc, 0x0, 0x8000); /* ADS5409_R00 4 Wire SPI + no filter  */
  }
  return;
}


/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * Function name : adc3112_ads5409_init
 * Prototype     : int
 * Parameters    : fmc number (1 or 2)
 * Return        : fail/success
 *----------------------------------------------------------------------------
 * Description   : 
 *
 *++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

void
adc3112_ads5409_init(int fd, int fmc,
		      int corr)
{
  /* *****************************************************************/
  /* ADS5409 Channel 01 U737                                         */      
  /* *****************************************************************/

  /* Initialization sequence according to data-sheet */
  if( corr)
  {
    //adc3112_spi_ads01_write( fmc, 0x1, 0x8202);  /* ADS5409_R01 CHA&B Corr EN + Two's complement + HP Mode 1     */
    adc3112_spi_ads01_write(fd, fmc, 0x1, 0x820a);  /* ADS5409_R01 CHA&B Corr EN + Offset Binary + HP Mode 1     */
  }
  else 
  {
    //adc3112_spi_ads01_write( fmc, 0x1, 0x0002);  /* ADS5409_R01 CHA&B Corr DIS + Two's complement + HP Mode 1     */
    adc3112_spi_ads01_write(fd, fmc, 0x1, 0x000a);  /* ADS5409_R01 CHA&B Corr DIS + Offset Binary + HP Mode 1     */
  }
  adc3112_spi_ads01_write(fd, fmc, 0x3, 0x4B18);  /* ADS5409_R03 Clear CHA accumulator   */
  adc3112_spi_ads01_write(fd, fmc, 0x1a, 0x4B18); /* ADS5409_R03 Clear CHB accumulator   */
  usleep( 10000);
  adc3112_spi_ads01_write(fd, fmc, 0x3, 0x0B18);  /* ADS5409_R03 Start DC and gain auto correction loop   */
  adc3112_spi_ads01_write(fd, fmc, 0x1a, 0x0B18); /* ADS5409_R1A Start DC and gain auto correction loop   */
  usleep( 10000);

  adc3112_spi_ads01_write(fd, fmc, 0x38, 0xFFFF); /* ADS5409_R38 Bias EN + SYNC EN + Input buf EN                  */
  adc3112_spi_ads01_write(fd, fmc, 0x02, 0x0780); /* ADS5409_R02 Over-range threshold = F                          */
  adc3112_spi_ads01_write(fd, fmc, 0x0e, 0xAAA8); /* ADS5409_R0E Sync from SYNC pins                               */
  adc3112_spi_ads01_write(fd, fmc, 0x0f, 0xA000); /* ADS5409_R0F Sync from SYNC pins + VREF = 1.0 V                */
  adc3112_spi_ads01_write(fd, fmc, 0x37, 0xd400); /* ADS5409_R37 Sleep mode                                        */
  adc3112_spi_ads01_write(fd, fmc, 0x3a, 0x4e18); /* ADS5409_R3A LDVS = 3.5 mA + term 100 Ohm + EN DACLK & DBCLK   */
  //adc3112_spi_ads01_write( fmc, 0x3a, 0x4818); /* ADS5409_R3A LDVS = 3.5 mA + term 100 Ohm + EN DACLK & DBCLK  JOEL  */
  adc3112_spi_ads01_write(fd, fmc, 0x66, 0x0FFF); /* ADS5409_R66 LDVS OUTA bus EN                                  */
  adc3112_spi_ads01_write(fd, fmc, 0x67, 0x0FFF); /* ADS5409_R67 LDVS OUTB bus EN                                  */
  //adc3112_spi_ads01_write( fmc, 0x3a, 0xfc18); /* ADS5409_R3A LDVS = 2.5 mA + term 200 Ohm + EN DACLK & DBCLK [JB]  */
  //adc3112_spi_ads01_write( fmc, 0x66, 0x2FFF); /* ADS5409_R66 LDVS OUTA bus EN                                  */
  //adc3112_spi_ads01_write( fmc, 0x67, 0x2FFF); /* ADS5409_R67 LDVS OUTB bus EN                                  */

  /* *****************************************************************/
  /* ADS5409 Channel 23 U747                                         */      
  /* *****************************************************************/

  /* Initialization sequence according to data-sheet */
  if( corr)
  {
    //adc3112_spi_ads23_write( fmc, 0x1, 0x8202);  /* ADS5409_R01 CHA&B Corr EN + Two's complement + HP Mode 1     */
    adc3112_spi_ads23_write(fd, fmc, 0x1, 0x820a);  /* ADS5409_R01 CHA&B Corr EN + Offset Binary + HP Mode 1     */
  }
  else 
  {
    //adc3112_spi_ads23_write( fmc, 0x1, 0x0002);  /* ADS5409_R01 CHA&B Corr DIS + Two's complement + HP Mode 1     */
    adc3112_spi_ads23_write(fd, fmc, 0x1, 0x000a);  /* ADS5409_R01 CHA&B Corr DIS + Offset Binary + HP Mode 1     */
  }
  adc3112_spi_ads23_write(fd, fmc, 0x3, 0x4B18);  /* ADS5409_R03 Clear CHA accumulator   */
  adc3112_spi_ads23_write(fd, fmc, 0x1a, 0x4B18); /* ADS5409_R03 Clear CHB accumulator   */
  usleep( 10000);
  adc3112_spi_ads23_write(fd, fmc, 0x3, 0x0B18);  /* ADS5409_R03 Start DC and gain auto correction loop   */
  adc3112_spi_ads23_write(fd, fmc, 0x1a, 0x0B18); /* ADS5409_R1A Start DC and gain auto correction loop   */
  usleep( 10000);

  adc3112_spi_ads23_write(fd, fmc, 0x38, 0xFFFF); /* ADS5409_R38 Bias EN + SYNC EN + Input buf EN                  */
  adc3112_spi_ads23_write(fd, fmc, 0x02, 0x0780); /* ADS5409_R02 Over-range threshold = F                          */
  adc3112_spi_ads23_write(fd, fmc, 0x0e, 0xAAA8); /* ADS5409_R0E Sync from SYNC pins                               */
  adc3112_spi_ads23_write(fd, fmc, 0x0f, 0xA000); /* ADS5409_R0F Sync from SYNC pins + VREF = 1.0 V                */
  adc3112_spi_ads23_write(fd, fmc, 0x37, 0xd400); /* ADS5409_R37 Sleep mode                                        */
  adc3112_spi_ads23_write(fd, fmc, 0x3a, 0x4e18); /* ADS5409_R3A LDVS = 3.5 mA + term 100 Ohm + EN DACLK & DBCLK   */
  //adc3112_spi_ads23_write( fmc, 0x3a, 0x4818); /* ADS5409_R3A LDVS = 3.5 mA + term 100 Ohm + EN DACLK & DBCLK JOEL  */
  adc3112_spi_ads23_write(fd, fmc, 0x66, 0x0FFF); /* ADS5409_R66 LDVS OUTA bus EN                                  */
  adc3112_spi_ads23_write(fd, fmc, 0x67, 0x0FFF); /* ADS5409_R67 LDVS OUTB bus EN                                  */
  //adc3112_spi_ads01_write( fmc, 0x3a, 0xfc18); /* ADS5409_R3A LDVS = 2.5 mA + term 200 Ohm + EN DACLK & DBCLK [JB] */
  //adc3112_spi_ads01_write( fmc, 0x66, 0x2FFF); /* ADS5409_R66 LDVS OUTA bus EN                                  */
  //adc3112_spi_ads01_write( fmc, 0x67, 0x2FFF); /* ADS5409_R67 LDVS OUTB bus EN                                  */

  return;
}


void
adc3112_ads5409_dump(int fd, int fmc)
{
  int reg, data;

  printf("\nDump ADS5409 Channel 01:");
  for( reg = 0; reg < 256; reg += 8)
  {
    int i;

    printf("\n%2x : ", reg);
    for( i = 0; i <  8; i++)
    {
      data = adc3112_spi_ads01_read(fd, fmc, reg+i);
      printf("%04x ", (unsigned short)data);
    }
  }
  printf("\n");

  printf("\nDump ADS5409 Channel 23:");
  for( reg = 0; reg < 256; reg += 8)
  {
    int i;

    printf("\n%2x : ", reg);
    for( i = 0; i <  8; i++)
    {
      data = adc3112_spi_ads23_read(fd, fmc, reg+i);
      printf("%04x ", (unsigned short)data);
    }
  }
  printf("\n");
  return;
}
/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * Function name : adc3112_ads5409_enable
 * Prototype     : int
 * Parameters    : fmc number (1 or 2)
 * Return        : fail/success
 *----------------------------------------------------------------------------
 * Description   : 
 *
 *++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

int
adc3112_ads5409_enable(int fd, int fmc,
			int m01,
			int m23)
{
  int data;

   /* *****************************************************************/
  /* ISERDES ADS5409 Initialisation                                  */      
  /* *****************************************************************/

  adc3112_spi_xratrig_write(fd, fmc, 0x2, 0x60); /* XRA1404 OCR    ADC_ENABLE = 1  -> ADS5409 ENABLE = '1'   */
  usleep( 20000);
  adc3112_csr_wr(fd, fmc, ADC3112_CSR_CTL, 0x4000);
  usleep( 20000);
  /*  Check MMCM is un-locked (MCMM RESET) */
  data = adc3112_csr_rd(fd, fmc, ADC3112_CSR_CTL);
  if( data & 0x8000)
  {
    printf(" MMCM is not un-locked after RESET [%08x]\n", data);
    return(-1);
  }
  adc3112_csr_wr(fd, fmc, ADC3112_CSR_CTL, 0x0000);
  usleep( 20000);
  data = adc3112_csr_rd(fd, fmc, ADC3112_CSR_CTL);
  if( !(data & 0x8000))
  {
    printf(" MMCM is not locked [%08x]\n", data);
    return(-1);
  }
  adc3112_csr_wr(fd, fmc, ADC3112_CSR_CTL, 0x100);
  usleep( 20000);
  adc3112_csr_wr(fd, fmc, ADC3112_CSR_CTL, 0x0000);
  usleep( 20000);

  if( (m01 & ADC3112_ADS_FILTER_MASK) == ADC3112_ADS_FILTER_LOW)
  {
    adc3112_spi_ads01_write(fd, fmc, 0x0, 0xc000); /* ADS5409_R00 4 Wire SPI + decimation filter low  */
  }
  else if( (m01 & ADC3112_ADS_FILTER_MASK) == ADC3112_ADS_FILTER_HIGH)
  {
    adc3112_spi_ads01_write(fd, fmc, 0x0, 0xd200); /* ADS5409_R00 4 Wire SPI + decimation filter high  */
  }
  else 
  {
    adc3112_spi_ads01_write(fd, fmc, 0x0, 0x8000); /* ADS5409_R00 4 Wire SPI + no filter  */
  }

  if( (m23 & ADC3112_ADS_FILTER_MASK) == ADC3112_ADS_FILTER_LOW)
  {
    adc3112_spi_ads23_write(fd, fmc, 0x0, 0xc000); /* ADS5409_R00 4 Wire SPI + decimation filter low  */
  }
  else if( (m23 & ADC3112_ADS_FILTER_MASK) == ADC3112_ADS_FILTER_HIGH)
  {
    adc3112_spi_ads23_write(fd, fmc, 0x0, 0xd200); /* ADS5409_R00 4 Wire SPI + decimation filter high  */
  }
  else 
  {
    adc3112_spi_ads23_write(fd, fmc, 0x0, 0x8000); /* ADS5409_R00 4 Wire SPI + no filter  */
  }
  usleep( 20000);

  /* *****************************************************************/
  /* Initialize Front-end INPUT                                      */      
  /* *****************************************************************/
  adc3112_spi_xra01_write(fd, fmc, 0x02, m01 & ADC3112_XRA_FE_MASK);   /* XRA1404 OCR    Grounded ADC inputs */
  adc3112_spi_xra23_write(fd, fmc, 0x02, m23 & ADC3112_XRA_FE_MASK);   /* XRA1404 OCR    Grounded ADC inputs */

  return(0);
}

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * Function name : adc3112_gpio_reset
 * Prototype     : int
 * Parameters    : fmc number (1 or 2)
 * Return        : fail/success
 *----------------------------------------------------------------------------
 * Description   : 
 *
 *++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

int
adc3112_gpio_reset(int fd, int fmc)
{
  adc3112_csr_wr(fd, fmc, ADC3112_CSR_GPIO, 0x0);
  return(0);
}

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * Function name : adc3112_gpio_trig
 * Prototype     : int
 * Parameters    : fmc number (1 or 2)
 * Return        : fail/success
 *----------------------------------------------------------------------------
 * Description   : 
 *
 *++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

int
adc3112_gpio_trig(int fd, int fmc)
{
  adc3112_csr_wr(fd, fmc, ADC3112_CSR_GPIO, 0x10); /* set output low */
  adc3112_csr_wr(fd, fmc, ADC3112_CSR_GPIO, 0x20); /* set output high */
  adc3112_csr_wr(fd, fmc, ADC3112_CSR_GPIO, 0x10); /* set output low */
  return(0);
}

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * Function name : adc3112_ttrig_arm
 * Prototype     : int
 * Parameters    : fmc number (1 or 2)
 * Return        : fail/success
 *----------------------------------------------------------------------------
 * Description   : 
 *
 *++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

int
adc3112_ttrig_arm(int fd, int fmc)
{
  int csr;

  adc3112_csr_wr(fd, fmc, ADC3112_CSR_TTRIG, ADC3112_TTRIG_RESET);
  csr = adc3112_csr_rd(fd, fmc, ADC3112_CSR_TTRIG);
  adc3112_csr_wr(fd, fmc, ADC3112_CSR_TTRIG, ADC3112_TTRIG_ENA|ADC3112_TTRIG_ARM);
  csr = adc3112_csr_rd(fd, fmc, ADC3112_CSR_TTRIG);
  return(csr);
}

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * Function name : adc3112_dac_set_level
 * Prototype     : int
 * Parameters    : fmc number (1 or 2)
 * Return        : fail/success
 *----------------------------------------------------------------------------
 * Description   : 
 *
 *++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

int
adc3112_dac_set_level(int fd, int fmc,
		       int chan,
		       int level)
{
  switch( chan)
  {
    case ADC3112_DAC_A:
    {
      adc3112_spi_dac_write(fd, fmc, 0x18, level);
      return(0);
    }
    case ADC3112_DAC_B:
    {
      adc3112_spi_dac_write(fd, fmc, 0x19, level);
      return(0);
    }
    case ADC3112_DAC_AB:
    {
      adc3112_spi_dac_write(fd, fmc, 0x1f, level);
      return(0);
    }
  }
  return(-1);
}

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * Function name : adc3112_ttim_init
 * Prototype     : int
 * Parameters    : fmc number (1 or 2)
 * Return        : fail/success
 *----------------------------------------------------------------------------
 * Description   : 
 *
 *++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

int
adc3112_ttim_init(int fd, int fmc)
{
  int data;

  /* ------------------------------------------------------*/
  /* ISERDES TTIM Initialisation                           */
  /* ------------------------------------------------------*/
  adc3112_csr_wr(fd, fmc, ADC3112_CSR_TTIM, 0x50F00000); /* IODELAY default prog value TTIM data   */

  adc3112_csr_wr(fd, fmc, ADC3112_CSR_TTIM, 0x10000000); /*  IODELAY Read-out pointer */
  data = adc3112_csr_rd(fd, fmc, ADC3112_CSR_TTIM);      /*  IODELAY TAP value verification */
  if( (data & 0xf00) != 0x400) return(-1);
  adc3112_csr_wr(fd, fmc, ADC3112_CSR_TTIM, 0x10100000); /*  IODELAY Read-out pointer */
  data = adc3112_csr_rd(fd, fmc, ADC3112_CSR_TTIM);      /*  IODELAY TAP value verification */
  if( (data & 0xf00) != 0x700) return(-1);
  adc3112_csr_wr(fd, fmc, ADC3112_CSR_TTIM, 0x10200000); /*  IODELAY Read-out pointer */
  data = adc3112_csr_rd(fd, fmc, ADC3112_CSR_TTIM);      /*  IODELAY TAP value verification */
  if( (data & 0xf00) != 0xa00) return(-1);
  adc3112_csr_wr(fd, fmc, ADC3112_CSR_TTIM, 0x10300000); /*  IODELAY Read-out pointer */
  data = adc3112_csr_rd(fd, fmc, ADC3112_CSR_TTIM);      /*  IODELAY TAP value verification */
  if( (data & 0xf00) != 0xd00) return(-1);

  /* ------------------------------------------------------*/
  /* Enable TTIM Initialisation                            */
  /* ------------------------------------------------------*/

  /* ------------------------------------------------------*/
  /* Control TTIM Logic                                    */
  /* ------------------------------------------------------*/

  return(0);
}


/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * Function name : adc3112_calib
 * Prototype     : int
 * Parameters    : fmc number (1 or 2)
 *                 channe number (0 to 3)
 * Return        : fail/success
 *----------------------------------------------------------------------------
 * Description   : 
 *
 *++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

void
adc3112_calib_init(int fd, struct adc3112_calib_ctl *cc,
		    int fmc)

{
  int bit, step, chan;

  cc->fmc = fmc;
  for( chan = 0; chan < ADC3112_CHAN_NUM; chan++)
  {
    cc->chan[chan].chan = chan;
    cc->chan[chan].delay = 0;
    for( bit = 0; bit < CAL_BIT_NUM; bit++)
    {
      cc->chan[chan].ttim[bit] = 0;
      for( step = 0; step < CAL_STEP_NUM; step++)
      {
        cc->chan[chan].err_cnt[bit][step] = 0;
      }
    }

    /* reset ADC channel gain to default value */
    adc3112_csr_wr(fd, fmc, ADC3112_CSR_GAIN + (4*chan), 0x4000);
  }
  return;
}

void
adc3112_calib_config(int fd, int fmc,
		      uint mode)
 {
  int data;

  /* ADS5409_01 DAQ calibration with 0x555/0xAAA */
  adc3112_spi_ads01_write(fd, fmc, 0xe, 0x0000);
  adc3112_spi_ads01_write(fd, fmc, 0xf, 0x0000);
  adc3112_spi_ads01_write(fd, fmc, 0x1, 0x0000);


  data = 0x8000 | ((mode & 0xfff00000) >> 18);
  adc3112_spi_ads01_write(fd, fmc, 0x3c, data);
  data = ((mode & 0xfff0) >> 2);
  adc3112_spi_ads01_write(fd, fmc, 0x3d, data);
  data = ((mode & 0xfff00000) >> 18);
  adc3112_spi_ads01_write(fd, fmc, 0x3e, data);

  /* ADS5409_23 DAQ calibration with 0x555/0xAAA */
  adc3112_spi_ads23_write(fd, fmc, 0xe, 0x0000);
  adc3112_spi_ads23_write(fd, fmc, 0xf, 0x0000);
  adc3112_spi_ads23_write(fd, fmc, 0x1, 0x0000);


  data = 0x8000 | ((mode & 0xfff00000) >> 18);
  adc3112_spi_ads23_write(fd, fmc, 0x3c, data);
  data = ((mode & 0xfff0) >> 2);
  adc3112_spi_ads23_write(fd, fmc, 0x3d, data);
  data = ((mode & 0xfff00000) >> 18);
  adc3112_spi_ads23_write(fd, fmc, 0x3e, data);

  return;
}

void
adc3112_calib_get_ttim(int fd, struct adc3112_calib_ctl *cc,
			 int chan)
{
  int bit, data;

  for( bit = 0; bit < 12; bit++)
  {
    data = (bit<<20) | (chan << 26);
    adc3112_csr_wr(fd, cc->fmc, ADC3112_CSR_TTIM, data);
    cc->chan[chan].ttim[bit+4] =  adc3112_csr_rd(fd, cc->fmc, ADC3112_CSR_TTIM);

  }
  data = (0xc00000) | (chan << 26);
  adc3112_csr_wr(fd, cc->fmc, ADC3112_CSR_TTIM, data);
  cc->chan[chan].ttim[0] =  adc3112_csr_rd(fd, cc->fmc, ADC3112_CSR_TTIM);
  return;
}

void
adc3112_calib_set_default(int fd, struct adc3112_calib_ctl *cc,
			   int chan)
{
  int data;

  /* load default delay */
  data = 0x40000000 | (chan << 26);
  adc3112_csr_wr(fd, cc->fmc, ADC3112_CSR_TTIM, data); 
  adc3112_csr_rd(fd, cc->fmc, ADC3112_CSR_TTIM);
  cc->chan[chan].delay = 0;
  /* update ttim array */
  adc3112_calib_get_ttim(fd, cc, chan);

  return;
}

void
adc3112_calib_restore(int fd, int fmc)
{
  /* ADS5409_01 exit calibration mode */
  adc3112_spi_ads01_write(fd, fmc, 0xe, 0xaaa8);
  adc3112_spi_ads01_write(fd, fmc, 0xf, 0xa000);
  //adc3112_spi_ads01_write( fmc, 0x1, 0x0002);
  adc3112_spi_ads01_write(fd, fmc, 0x1, 0x8206);
  adc3112_spi_ads01_write(fd, fmc, 0x3a, 0x4a18);
  adc3112_spi_ads01_write(fd, fmc, 0x3c, 0);
  adc3112_spi_ads01_write(fd, fmc, 0x3d, 0);
  adc3112_spi_ads01_write(fd, fmc, 0x3e, 0);

  /* ADS5409_23 exit calibration mode */
  adc3112_spi_ads23_write(fd, fmc, 0xe, 0xaaa8);
  adc3112_spi_ads23_write(fd, fmc, 0xf, 0xa000);
  //adc3112_spi_ads23_write( fmc, 0x1, 0x0002);
  adc3112_spi_ads23_write(fd, fmc, 0x1, 0x8206);
  adc3112_spi_ads23_write(fd, fmc, 0x3a, 0x4a18);
  adc3112_spi_ads23_write(fd, fmc, 0x3c, 0);
  adc3112_spi_ads23_write(fd, fmc, 0x3d, 0);
  adc3112_spi_ads23_write(fd, fmc, 0x3e, 0);

  /* ------------------------------------------------------*/
  /* Patch ADC5409 SYNC_OUT work around                    */
  /* ------------------------------------------------------*/
  /*
   adc3112_spi_lmk_write( fmc, 0xb, 0x07610000);
   usleep(10000);
   adc3112_spi_lmk_write( fmc, 0, 0x00140800);
   adc3112_spi_lmk_write( fmc, 1, 0x00140040);
   adc3112_spi_lmk_write( fmc, 2, 0x00140040);
   adc3112_spi_lmk_write( fmc, 3, 0x00180800);
   adc3112_spi_lmk_write( fmc, 4, 0x00140040);
   adc3112_spi_lmk_write( fmc, 5, 0x00180040);
   adc3112_spi_lmk_write( fmc, 0xb, 0x07600000);
   usleep(10000);
  */
  return;
}

void
adc3112_calib_reset_delta(struct adc3112_calib_ctl *cc)
{
  int bit, chan;

  for( chan = 0; chan < ADC3112_CHAN_NUM; chan++)
  {
    for( bit = 0; bit < CAL_BIT_NUM; bit++)
    {
      cc->chan[chan].delta[bit] = 0;
    }
  }
  return;
}

int
adc3112_calib_inc_delay(int fd, struct adc3112_calib_ctl *cc,
			 int set,
			 int chan)
{
  int bit, data;

  if( cc->chan[chan].delay >= (CAL_STEP_NUM/2))
  {
    return(-1);
  }
  if( set & CAL_ALL_BIT)
  {
    cc->chan[chan].delay += 1;
    for( bit = 0; bit < 12; bit++)
    {
      data = 0xa0000000 | (bit<<20) | (chan << 26);
      adc3112_csr_wr(fd, cc->fmc, ADC3112_CSR_TTIM, data); 
      cc->chan[chan].ttim[bit+4] =  adc3112_csr_rd(fd, cc->fmc, ADC3112_CSR_TTIM);
    }
    data = 0xa0c00000 | (chan << 26);
    adc3112_csr_wr(fd, cc->fmc, ADC3112_CSR_TTIM, data);
    cc->chan[chan].ttim[0] =  adc3112_csr_rd(fd, cc->fmc, ADC3112_CSR_TTIM);
  }
  else
  {
    //printf("in adc3112_calib_dec_delay( %x, %d)\n", set, chan);
    for( bit = 0; bit < 12; bit++)
    {
      if( set & (1<<bit))
      {
	//printf("increment bit %d\n", bit);
	data = 0xa0000000 | (bit<<20) | (chan << 26);
        adc3112_csr_wr(fd, cc->fmc, ADC3112_CSR_TTIM, data); 
	cc->chan[chan].ttim[bit+4] =  adc3112_csr_rd(fd, cc->fmc, ADC3112_CSR_TTIM);
      }
    }
    if( set & 0x1000)
    {
      //printf("decrement bit 12\n");
      data = 0xa0c00000 | (chan << 26);
      adc3112_csr_wr(fd, cc->fmc, ADC3112_CSR_TTIM, data); 
      cc->chan[chan].ttim[0] =  adc3112_csr_rd(fd, cc->fmc, ADC3112_CSR_TTIM);
    }
  }
  return(0);
}

int
adc3112_calib_dec_delay(int fd, struct adc3112_calib_ctl *cc,
			 int set,
			 int chan)
{
  int bit, data;

  if( cc->chan[chan].delay <= -(CAL_STEP_NUM/2))
  {
    return(-1);
  }
  if( set & CAL_ALL_BIT)
  {
    cc->chan[chan].delay -= 1;
    for( bit = 0; bit < 12; bit++)
    {
      data = 0x80000000 | (bit<<20) | (chan << 26);
      adc3112_csr_wr(fd, cc->fmc, ADC3112_CSR_TTIM, data); 
      cc->chan[chan].ttim[bit+4] =  adc3112_csr_rd(fd, cc->fmc, ADC3112_CSR_TTIM);
    }
    data = 0x80c00000 | (chan << 26);
    adc3112_csr_wr(fd, cc->fmc, ADC3112_CSR_TTIM, data); 
    cc->chan[chan].ttim[0] =  adc3112_csr_rd(fd, cc->fmc, ADC3112_CSR_TTIM);
  }
  else
  {
    //printf("in adc3112_calib_dec_delay( %x, %d)\n", set, chan);
    for( bit = 0; bit < 12; bit++)
    {
      if( set & (1<<bit))
      {
	//printf("decrement bit %d\n", bit);
	data = 0x80000000 | (bit<<20) | (chan << 26);
        adc3112_csr_wr(fd, cc->fmc, ADC3112_CSR_TTIM, data); 
	cc->chan[chan].ttim[bit+4] =  adc3112_csr_rd(fd, cc->fmc, ADC3112_CSR_TTIM);
      }
    }
    if( set & 0x1000)
    {
      //printf("decrement bit 12\n");
      data = 0x80c00000 | (chan << 26);
      adc3112_csr_wr(fd, cc->fmc, ADC3112_CSR_TTIM, data); 
      cc->chan[chan].ttim[0] =  adc3112_csr_rd(fd, cc->fmc, ADC3112_CSR_TTIM);
    }
  }
  return(0);
}

void
adc3112_calib_adjust_delay(int fd, struct adc3112_calib_ctl *cc,
			    int chan)
{
  int step, bit;

  if( adc3112_verbose_flag) printf("IODelay #%d :", chan);
  for( bit = 0; bit < 12; bit++)
  {
    if( adc3112_verbose_flag) printf(" %3d",  (cc->chan[chan].ttim[15-bit] >> 8) & 0xfff);
  }
  if( adc3112_verbose_flag) printf(" %3d\n",  (cc->chan[chan].ttim[0] >> 8) & 0xfff);
  if( adc3112_verbose_flag) printf("           :");
  for( bit = 0; bit < 12; bit++)
  {
    if( adc3112_verbose_flag) printf(" %3d",  cc->chan[chan].delta[15-bit]);
    if( cc->chan[chan].delta[bit+4] < 0)
    {
      for( step = 0; step < -cc->chan[chan].delta[bit+4]; step++)
      {
	adc3112_calib_dec_delay(fd, cc, 1<<bit, chan);
      }
    }
    if( cc->chan[chan].delta[bit+4] > 0)
    {
      for( step = 0; step < cc->chan[chan].delta[bit+4]; step++)
      {
	adc3112_calib_inc_delay(fd, cc, 1<<bit, chan);
      }
    }
  }
  if( adc3112_verbose_flag) printf(" %3d\n", cc->chan[chan].delta[0]);
  if( cc->chan[chan].delta[0] < 0)
  {
    for( step = 0; step < -cc->chan[chan].delta[0]; step++)
    {
      adc3112_calib_dec_delay(fd, cc, 1<<12, chan);
    }
  }
  if( cc->chan[chan].delta[0] > 0)
  {
    for( step = 0; step < cc->chan[chan].delta[0]; step++)
    {
      adc3112_calib_inc_delay(fd, cc, 1<<12, chan);
    }
  }

  if( adc3112_verbose_flag) printf("           :");
  for( bit = 0; bit < 12; bit++)
  {
    if( adc3112_verbose_flag) printf(" %3d",  (cc->chan[chan].ttim[15-bit] >> 8) & 0xfff);
  }
  if( adc3112_verbose_flag) printf(" %3d\n",  (cc->chan[chan].ttim[0] >> 8) & 0xfff);

  return;
}

unsigned short
adc3112_calib_data_check(struct adc3112_calib_ctl *cc,
			  int chan,
			  unsigned short cmp0,
			  unsigned short cmp1,
			  char *adc_buf)
{
  unsigned short res, tmp, dat0, dat1;
  int i, bit;
  char *p;

  res = 0;
  if( !(chan & 1)) /* hardware add 1 to data for chan 0 and 2 */
  {
    cmp0 += 0x10;
    cmp1 += 0x10;
  }
  for( bit = 0; bit < CAL_BIT_NUM; bit++)
  {
    cc->chan[chan].err_cnt[bit][(CAL_STEP_NUM/2) +  cc->chan[chan].delay] = 0;
  }
  p = adc_buf;
  for( i = 0; i < (ADC_NUM_SAMPLES*ADC_SAMPLE_SIZE); i += sizeof(int))
  {
    dat0 = (unsigned short)tsc_swap_16(*(short *)&p[i+2]);
    dat0 = (dat0&0xfff0) | (~dat0&1); /* JFG ==> flip sync bit */
    dat1 = (unsigned short)tsc_swap_16(*(short *)&p[i]);
    dat1 = (dat1&0xfff0) | (~dat1&1); /* JFG ==> flip sync bit */
    tmp = cmp0 ^ dat0;
    res |= tmp;
    for( bit = 0; bit < CAL_BIT_NUM; bit++)
    {
      if( tmp & (1<<bit))
      {
        cc->chan[chan].err_cnt[bit][(CAL_STEP_NUM/2) +  cc->chan[chan].delay] += 1;
      }
    }
    tmp = cmp1 ^ dat1;
    res |= tmp;
    for( bit = 0; bit < CAL_BIT_NUM; bit++)
    {
      if( tmp & (1<<bit))
      {
        cc->chan[chan].err_cnt[bit][(CAL_STEP_NUM/2) +  cc->chan[chan].delay] += 1;
      }
    }
  }
  return( res);
}


int
adc3112_calib_show_res(struct adc3112_calib_ctl *cc)
{
  int step;

  if( adc3112_verbose_flag) printf("delay   CH0  CH1  CH2  CH3\n");
  for( step = 0; step < (CAL_STEP_NUM+1); step++)
  {
    if( adc3112_verbose_flag) printf("%5d : %04x %04x %04x %04x\n", step - (CAL_STEP_NUM/2), 
	   cc->chan[0].cal_res[step], cc->chan[1].cal_res[step], cc->chan[2].cal_res[step], cc->chan[3].cal_res[step]);
  }
  return(0);
}

int
adc3112_calib_show_err_cnt(struct adc3112_calib_ctl *cc,
			    int chan)
{
  int step;
  int bit;
  int min[CAL_BIT_NUM], max[CAL_BIT_NUM];

  for( bit = 0; bit < CAL_BIT_NUM; bit++)
  {
    min[bit] = -1;
    max[bit] = -1;
  }
  if( adc3112_verbose_flag) printf("\n Delay   D11  D10  D9   D8   D7   D6   D5   D4   D3   D2   D1   D0  SYNC\n");
  for( step = 0; step < (CAL_STEP_NUM+1); step++)
  {
   if( adc3112_verbose_flag)  printf("%5d :", step - (CAL_STEP_NUM/2)); 
    for( bit = 0; bit < 12; bit++)
    {
      if( (cc->chan[chan].err_cnt[15-bit][step] == 0) && (min[15-bit] == -1))
      {
	min[15-bit] = step;
      }
      if( (cc->chan[chan].err_cnt[15-bit][step] != 0) && (min[15-bit] != -1) && (max[15-bit] == -1))
      {
	max[15-bit] = step - 1;;
      }
      if( cc->chan[chan].err_cnt[15-bit][step] == ADC_NUM_SAMPLES)
      {
        if( adc3112_verbose_flag) printf(" XXXX");
      } 
      else
      {
        if( adc3112_verbose_flag) printf(" %04x", cc->chan[chan].err_cnt[15-bit][step]);
      }
    }
    if( cc->chan[chan].err_cnt[0][step] == ADC_NUM_SAMPLES)
    {
      if( adc3112_verbose_flag) printf(" XXXX\n");
    }
    else 
    { 
      if( adc3112_verbose_flag) printf(" %04x\n", cc->chan[chan].err_cnt[0][step]);
    }
  }
  return(0);
}

int
adc3112_calib_show_min_max(struct adc3112_calib_ctl *cc,
			    int chan)
{
  int step;
  int bit;
  int min[CAL_BIT_NUM], max[CAL_BIT_NUM], first[CAL_BIT_NUM];

  for( bit = 0; bit < CAL_BIT_NUM; bit++)
  {
    min[bit] = -32;
    max[bit] = 32;
    first[bit] = 0;
  }

  for( bit = 0; bit < 12; bit++)
  {
    for( step = 0; step < (CAL_STEP_NUM/2); step++)
    {
      if( cc->chan[chan].err_cnt[15-bit][(CAL_STEP_NUM/2) + step] == 0)
      {
	first[15-bit] = step;
	break;
      }
      if( cc->chan[chan].err_cnt[15-bit][(CAL_STEP_NUM/2) - step] == 0)
      {
	first[15-bit] = -step;
	break;
      }
    }
  }
  for( step = 0; step < (CAL_STEP_NUM/2); step++)
  {
    if( cc->chan[chan].err_cnt[0][(CAL_STEP_NUM/2) + step] == 0)
    {
      first[0] = step;
      break;
    }
    if( cc->chan[chan].err_cnt[0][(CAL_STEP_NUM/2) - step] == 0)
    {
      first[0] = -step;
      break;
    }
  }
  for( bit = 0; bit < 12; bit++)
  {
    for( step = first[15-bit]; step < (CAL_STEP_NUM/2); step++)
    {
      if( (cc->chan[chan].err_cnt[15-bit][(CAL_STEP_NUM/2) + step] != 0) && (max[15-bit] == 32))
      {
	max[15-bit] = step;
      }
    }
  }
  for( step = first[0]; step < (CAL_STEP_NUM/2); step++)
  {
    if( (cc->chan[chan].err_cnt[0][(CAL_STEP_NUM/2) + step] != 0) && (max[0] == 32))
    {
      max[0] = step;
    }
  }
  for( bit = 0; bit < 12; bit++)
  {
    for( step = first[15-bit]; step > -(CAL_STEP_NUM/2); step--)
    {
      if( (cc->chan[chan].err_cnt[15-bit][(CAL_STEP_NUM/2) + step] != 0) && (min[15-bit] == -32))
      {
	min[15-bit] = step;
      }
    }
  }
  for( step = first[0]; step > -(CAL_STEP_NUM/2); step--)
  {
    if( (cc->chan[chan].err_cnt[0][(CAL_STEP_NUM/2) + step] != 0) && (min[0] == -32))
    {
      min[0] = step;
    }
  }

  if( adc3112_verbose_flag) printf(" MAX  :");
  cc->chan[chan].hold_time =  max[15];
  for( bit = 0; bit < 12; bit++)
  {
    if( max[15-bit] < cc->chan[chan].hold_time) cc->chan[chan].hold_time = max[15-bit]; 
    if( adc3112_verbose_flag) printf(" %3d ", max[15-bit]);
  }
  if( adc3112_verbose_flag) printf(" %3d\n", max[0]);
  //if( max[0] < cc->chan[chan].hold_time) cc->chan[chan].hold_time = max[0]; 
  cc->chan[chan].hold_time =   (cc->chan[chan].hold_time - 1)*CAL_STEP_WIDTH;

  if( adc3112_verbose_flag) printf(" MEAN :");
  for( bit = 0; bit < 12; bit++)
  {
    cc->chan[chan].delta[15-bit] =  (max[15-bit] + min[15-bit])/2;
    if( adc3112_verbose_flag) printf(" %3d ", cc->chan[chan].delta[15-bit]);
  }
  cc->chan[chan].delta[0] = (max[0] + min[0])/2; 
  if( adc3112_verbose_flag) printf(" %3d\n", cc->chan[chan].delta[0]);

  if( adc3112_verbose_flag) printf(" MIN  :");
  cc->chan[chan].set_time =  min[15];
  for( bit = 0; bit < 12; bit++)
  {
    if( min[15-bit] > cc->chan[chan].set_time) cc->chan[chan].set_time = min[15-bit]; 
    if( adc3112_verbose_flag) printf(" %3d ", min[15-bit]);
  }
  if( adc3112_verbose_flag) printf(" %3d\n", min[0]);
  //if( min[0] > cc->chan[chan].set_time) cc->chan[chan].set_time = min[0];
  cc->chan[chan].set_time =   (cc->chan[chan].set_time - 1)*CAL_STEP_WIDTH;

  return(0);
}

