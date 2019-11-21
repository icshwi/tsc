/*=========================< begin file & file header >=======================
 *  References
 *  
 *    filename : adc3110lib.c
 *    author   : JFG, XP
 *    company  : IOxOS
 *    creation : october 18,2018
 *
 *----------------------------------------------------------------------------
 *  Description
 *
 *    That library contains a set of function to access the TOSCA II XUSER
 *    FASTSCOPE ADC3110 data acquisition logic.
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
#include <adc3110lib.h>

int adc3110_verbose_flag = 0;
int adc3110_spi_ads[ADC3110_CHAN_NUM] =
{
  ADC3110_SPI_ADS01,
  ADC3110_SPI_ADS01,
  ADC3110_SPI_ADS23,
  ADC3110_SPI_ADS23,
  ADC3110_SPI_ADS45,
  ADC3110_SPI_ADS45,
  ADC3110_SPI_ADS67,
  ADC3110_SPI_ADS67
};


int 
adc3110_set_verbose( int vf)
{
  adc3110_verbose_flag = vf;
  return(adc3110_verbose_flag);
}

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * Function name : adc3110_XXX
 * Prototype     : int
 * Parameters    : void
 * Return        : 
 *----------------------------------------------------------------------------
 * Description   : 
 *
 *++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

int 
adc3110_XXX( void)
{

  return( 0);
}
/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * Function name : adc3110_csr_rd
 * Prototype     : int
 * Parameters    : fmc	fmc index (1 or 2)
 *                 csr	register index
 * Return        : register content
 *----------------------------------------------------------------------------
 * Description   : Read ADC3110 CSR register referred by csr
 *
 *++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

int 
adc3110_csr_rd(int fd, int fmc,
		int csr)
{
  int addr;
  int data;

  addr = ADC3110_CSR_ADDR( csr);
  if( fmc == ADC3110_FMC2) addr +=  ADC3110_CSR_OFF_FMC2;
  tsc_csr_read(fd, addr, &data);
  return( data);
}

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * Function name : adc3110_csr_wr
 * Prototype     : void
 * Parameters    : fmc  fmc index (1 or 2)
 *                 csr  register index
 *                 data  data to be written in register
 * Return        : void
 *----------------------------------------------------------------------------
 * Description   : Write ADC3110 CSR register referred by csr with data
 *
 *++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

void 
adc3110_csr_wr(int fd, int fmc,
		int csr,
	        int data)
{
  int addr;

  addr = ADC3110_CSR_ADDR( csr);
  if( fmc == ADC3110_FMC2) addr +=  ADC3110_CSR_OFF_FMC2;
  tsc_csr_write(fd, addr, &data);

  return;
}

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * Function name : adc3110_identify
 * Prototype     : int
 * Parameters    : fmc FMC identifier (1 or 2)
 * Return        : ADC3110 signature (expect ADC3110_SIGN_ID)
 *----------------------------------------------------------------------------
 * Description   : returns the content of ADC3110 signature register (index 0x80)
 *
 *++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

int 
adc3110_identify(int fd, int fmc)
{
  int id;

  id = adc3110_csr_rd(fd, fmc,  ADC3110_CSR_SIGN);

  return( id);
}
/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * Function name : adc3110_spi_read
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
adc3110_spi_read(int fd, int fmc,
		  int cmd,
	 	  int reg)
{
  int tmo, data;

  cmd |=  0x80000000 | reg;
  tmo = 1000;
  //printf("adc3110_spi_read( %x, %x, %x)\n", fmc, cmd, reg);
  adc3110_csr_wr(fd, fmc, ADC3110_CSR_SERIAL, cmd);
  while( --tmo)
  {
    if( !(adc3110_csr_rd(fd, fmc, ADC3110_CSR_SERIAL) & 0x80000000)) break;
  }
  if( !tmo)
  {
    printf("adc3110_spi_read() : cmd = %08x -> timeout...\n", cmd);
    return(-1);
  }
  data = adc3110_csr_rd(fd, fmc, ADC3110_CSR_SERIAL+1);
  //printf("cmd = %08x - data = %08x\n", cmd, data);

  return( data);
}

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * Function name : adc3110_spi_write
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
adc3110_spi_write(int fd, int fmc,
		   int cmd,
	 	   int reg,
		   int data)
{
  int tmo;

  cmd |=  0xc0000000 | reg;
  tmo = 1000;
  //printf("adc3110_spi_read( %x, %x, %x)\n", fmc, cmd, reg);
  adc3110_csr_wr(fd, fmc, ADC3110_CSR_SERIAL+1, data);
  data = adc3110_csr_rd(fd, fmc, ADC3110_CSR_SERIAL+1);
  adc3110_csr_wr(fd, fmc, ADC3110_CSR_SERIAL, cmd);
  while( --tmo)
  {
    if( !(adc3110_csr_rd(fd, fmc, ADC3110_CSR_SERIAL) & 0x80000000)) break;
  }
  if( !tmo)
  {
    printf("adc3110_spi_read() : cmd = %08x -> timeout...\n", cmd);
    return(-1);
  }

  return( 0);
}

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * Function name : adc3110_i2c_read
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
adc3110_i2c_read(int fd, int fmc,
		  uint device,
	 	  uint reg)
{
  int status;
  uint data;

  if( fmc == ADC3110_FMC2)
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
 * Function name : adc3110_i2c_write
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
adc3110_i2c_write(int fd, int fmc,
		   uint device,
	 	   uint reg,
		   uint data)
{
  int status;

  if( fmc == ADC3110_FMC2)
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
 * Function name : adc3110_reset
 * Prototype     : void
 * Parameters    : fmc FMC identifier (1 or 2)
 * Return        : void
 *----------------------------------------------------------------------------
 * Description   : perform a reset of the ADC3110 FMC by setting and re-setting
 * bit 8 of the control register (ADC3110_CSR_CTL)
 *
 *++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

void
adc3110_reset(int fd, int fmc)
{
  int sign;
  int pon;

  pon = 0xc0000000;
  tsc_pon_write(fd, 0xc, &pon);
  usleep( 20000);
  sign = adc3110_csr_rd(fd, fmc, ADC3110_CSR_SIGN);
  adc3110_csr_wr(fd, fmc, ADC3110_CSR_SIGN, sign);
  adc3110_csr_wr(fd, fmc, ADC3110_CSR_CTL, 0x1c00);
  usleep( 50000);
  adc3110_csr_wr(fd, fmc, ADC3110_CSR_CTL, 0x4000);
  usleep( 50000);
  adc3110_csr_wr(fd, fmc, ADC3110_CSR_CTL, 0x00);

  return;
}

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * Function name : adc3110_lmk_init
 * Prototype     : void
 * Parameters    : fmc  FMC identifier (1 or 2)
 * 				   lmk_reg[]  pointer to a table holding the init value of the 
 * 				   			  32 registers
 * Return        : void
 *----------------------------------------------------------------------------
 * Description   : initialize the LMK04803 clock controller registers according
 * to the table pointed by lmk_reg[]
 *
 *++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

void
adc3110_lmk_init(int fd, int fmc,
		  int lmk_reg[])
{
  int reg;

  if(adc3110_verbose_flag) printf("Initialisation LMK04906\n");
  adc3110_spi_lmk_write(fd, fmc, 0x0, 0x00020000); /* LMK04803B_R00 Generate a programmable RESET to the LMK04803B   */
  usleep( 50000);
  adc3110_spi_lmk_write(fd, fmc, 0xb, lmk_reg[0xb]);
  usleep( 50000);
  for( reg = 0; reg <= 0x10; reg++)
  {
    //printf("lmk reg %02d : %08x..", reg, adc3110_spi_lmk_read( fmc, reg));
    if( reg != 0xb)
    {
      adc3110_spi_lmk_write(fd, fmc, reg, lmk_reg[ reg]);
    }
    //printf("%08x\n", adc3110_spi_lmk_read( fmc, reg));
  }
  for( reg = 0x11; reg < 0x18; reg++)
  {
    //printf("lmk reg %02d : %08x\n", reg, adc3110_spi_lmk_read( fmc, reg));
  }
  for( reg = 0x18; reg < 0x1f; reg++)
  {
    //printf("lmk reg %02d : %08x..", reg, adc3110_spi_lmk_read( fmc, reg));
    adc3110_spi_lmk_write(fd, fmc, reg, lmk_reg[ reg]);
    //printf("%08x\n", adc3110_spi_lmk_read( fmc, reg));
  }

  /* --------------------------------------------*/
  /* Enable On-board 100 MHz clock from +OSC575  */
  /* --------------------------------------------*/
  adc3110_csr_wr(fd, fmc, ADC3110_CSR_LED, 0x80000003);   /* FP Led flashing + CCHD575-100MHz  Power-on */
  usleep( 20000);
  adc3110_spi_lmk_write(fd, fmc, 0x1e, lmk_reg[ 0x1e]);             /*  LMK04803B_R30 PLL2 P/N Recallibration */
  adc3110_spi_lmk_write(fd, fmc, 0x0c, lmk_reg[ 0x0c] | 0x800000);  /*  LMK04906_R12 LD pin programmable  */
  usleep( 20000);
  /* --------------------------------------------*/
  /* Check if manual SYNC to be performed        */
  /* --------------------------------------------*/
  if( !(lmk_reg[ 0x0b]&0x8000))
  {
    printf("Perform manual synchronisation of clock outputs\n");
    adc3110_spi_lmk_write(fd, fmc, 0x1e, lmk_reg[ 0x0b] | 0x10000); /* force manual SYNC */
    usleep( 20000);
    adc3110_spi_lmk_write(fd, fmc, 0x1e, lmk_reg[ 0x0b]);
  }

  return;
}

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * Function name : adc3110_lmk_dump
 * Prototype     : void
 * Parameters    : fmc  FMC identifier (1 or 2)
 * Return        : void
 *----------------------------------------------------------------------------
 * Description   : displays the content of the 32 registers
 *
 *++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

void
adc3110_lmk_dump(int fd, int fmc)
{
  int reg, data;

  printf("\nDump LMK registers:");
  for( reg = 0; reg < 32; reg += 4)
  {
    int i;

    printf("\n%2x : ", reg);
    for( i = 0; i <  4; i++)
    {
      data = adc3110_spi_lmk_read(fd, fmc, reg+i);
      printf("%08x ", data);
    }
  }
  printf("\n");

  return;
}

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * Function name : adc3110_ads42lb69_init
 * Prototype     : void
 * Parameters    : fmc  FMC identifier (1 or 2)
 * Return        : void
 *----------------------------------------------------------------------------
 * Description   : initializes the four ads42lb69 dual analog-to-digital 
 * converters present on the ADC_3110 FMC
 *
 *++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

void
adc3110_ads42lb69_init(int fd, int fmc,
			int chan_set)
{
  if( chan_set & (ADC3110_CHAN_SET_0|ADC3110_CHAN_SET_1))
  {
    /* *****************************************************************/
    /* Initialisation ADC_3110 ads42lb69 01                            */      
    /* *****************************************************************/
    if(adc3110_verbose_flag) printf("Initialisation ADC_3110 ads42lb69 01\n");
    adc3110_spi_ads01_write(fd, fmc, 0x8, 0x19);  /* ADS42LB69_Reg 0x08 RESET device   */
    usleep( 10000);
    adc3110_spi_ads01_write(fd, fmc, 0x4, 0x00);  /* ADS42LB69_Reg 0x08 RESET device   */
    adc3110_spi_ads01_write(fd, fmc, 0x5, 0x00);  /* ADS42LB69_Reg 0x08 RESET device   */
    adc3110_spi_ads01_write(fd, fmc, 0x6, 0x00);  /* ADS42LB69_Reg 0x06 LVDS CLKDIV=0 : Bypassed   */
    adc3110_spi_ads01_write(fd, fmc, 0x7, 0x00);  /* ADS42LB69_Reg 0x07 SYNC_IN delay = 0 ps   */
    adc3110_spi_ads01_write(fd, fmc, 0x8, 0x0c);  /* ADS42LB69_Reg 0x07 SYNC_IN delay = 0 ps   */
    adc3110_spi_ads01_write(fd, fmc, 0xb, 0x00);  /* ADS42LB69_Reg 0x0B Channel A(even) Gain disabled 0dB /No FLIP   */
    adc3110_spi_ads01_write(fd, fmc, 0xc, 0x00);  /* ADS42LB69_Reg 0x0C Channel B(Odd) Gain disabled 0dB /No OVR   */
    adc3110_spi_ads01_write(fd, fmc, 0xd, 0x00);  /* ADS42LB69_Reg 0x0D OVR pin normal   */
    //adc3110_spi_ads01_write( fmc, 0xe, 0x90);  /* High frequency  JFG  */
    adc3110_spi_ads01_write(fd, fmc, 0xf, 0x00);  /* ADS42LB69_Reg 0x0F Normal operation    */
    //adc3110_spi_ads01_write( fmc, 0xf, 0x44);  /* ADS42LB69_Reg 0x0F Test operation (ramp)    */
    adc3110_spi_ads01_write(fd, fmc, 0x10, 0x00);  /* ADS42LB69_Reg 0x10 Custom Pattern MSB    */
    adc3110_spi_ads01_write(fd, fmc, 0x11, 0x01);  /* ADS42LB69_Reg 0x11 Custom Pattern LSB   */
    adc3110_spi_ads01_write(fd, fmc, 0x14, 0x0c);  /* ADS42LB69_Reg 0x14 LVDS strenght (Default DATA/CLK))   */
    adc3110_spi_ads01_write(fd, fmc, 0x15, 0x01);  /* ADS42LB69_Reg 0x15 DDR Mode enabled   */
    adc3110_spi_ads01_write(fd, fmc, 0x16, 0x00);  /* ADS42LB69_Reg 0x16 DDR Timing 0ps(default)   */
    adc3110_spi_ads01_write(fd, fmc, 0x17, 0x00);  /* ADS42LB69_Reg 0x17 QDR CLKOUT delay   */
    adc3110_spi_ads01_write(fd, fmc, 0x18, 0x00);  /* ADS42LB69_Reg 0x18 QDR CLKOUT timing   */
    adc3110_spi_ads01_write(fd, fmc, 0x1f, 0xff);  /* ADS42LB69_Reg 0x1F Fast OVR   */
    adc3110_spi_ads01_write(fd, fmc, 0x30, 0x00);  /* ADS42LB69_Reg 0x30 SYNC IN disabled   */
  }

  if( chan_set & (ADC3110_CHAN_SET_2|ADC3110_CHAN_SET_3))
  {
    if(adc3110_verbose_flag) printf("Initialisation ADC_3110 ads42lb69 23\n");
    /* *****************************************************************/
    /* Initialisation ADC_3110 ads42lb69 23                            */      
    /* *****************************************************************/
    adc3110_spi_ads23_write(fd, fmc, 0x8, 0x19);  /* ADS42LB69_Reg 0x08 RESET device   */
    usleep( 10000);
    adc3110_spi_ads23_write(fd, fmc, 0x4, 0x00);  /* ADS42LB69_Reg 0x08 RESET device   */
    adc3110_spi_ads23_write(fd, fmc, 0x5, 0x00);  /* ADS42LB69_Reg 0x08 RESET device   */
    adc3110_spi_ads23_write(fd, fmc, 0x6, 0x00);  /* ADS42LB69_Reg 0x06 LVDS CLKDIV=0 : Bypassed   */
    adc3110_spi_ads23_write(fd, fmc, 0x7, 0x00);  /* ADS42LB69_Reg 0x07 SYNC_IN delay = 0 ps   */
    adc3110_spi_ads23_write(fd, fmc, 0x8, 0x0c);  /* ADS42LB69_Reg 0x07 SYNC_IN delay = 0 ps   */
    adc3110_spi_ads23_write(fd, fmc, 0xb, 0x00);  /* ADS42LB69_Reg 0x0B Channel A(even) Gain disabled 0dB /No FLIP   */
    adc3110_spi_ads23_write(fd, fmc, 0xc, 0x00);  /* ADS42LB69_Reg 0x0C Channel B(Odd) Gain disabled 0dB /No OVR   */
    adc3110_spi_ads23_write(fd, fmc, 0xd, 0x00);  /* ADS42LB69_Reg 0x0D OVR pin normal   */
    //adc3110_spi_ads23_write( fmc, 0xe, 0x90);  /* High frequency  JFG  */
    adc3110_spi_ads23_write(fd, fmc, 0xf, 0x00);  /* ADS42LB69_Reg 0x0F Normal operation    */
    //adc3110_spi_ads23_write( fmc, 0xf, 0x44);  /* ADS42LB69_Reg 0x0F Test operation (ramp)    */
    adc3110_spi_ads23_write(fd, fmc, 0x10, 0x00);  /* ADS42LB69_Reg 0x10 Custom Pattern MSB    */
    adc3110_spi_ads23_write(fd, fmc, 0x11, 0x01);  /* ADS42LB69_Reg 0x11 Custom Pattern LSB   */
    adc3110_spi_ads23_write(fd, fmc, 0x14, 0x0c);  /* ADS42LB69_Reg 0x14 LVDS strenght (Default DATA/CLK))   */
    adc3110_spi_ads23_write(fd, fmc, 0x15, 0x01);  /* ADS42LB69_Reg 0x15 DDR Mode enabled   */
    adc3110_spi_ads23_write(fd, fmc, 0x16, 0x00);  /* ADS42LB69_Reg 0x16 DDR Timing 0ps(default)   */
    adc3110_spi_ads23_write(fd, fmc, 0x17, 0x00);  /* ADS42LB69_Reg 0x17 QDR CLKOUT delay   */
    adc3110_spi_ads23_write(fd, fmc, 0x18, 0x00);  /* ADS42LB69_Reg 0x18 QDR CLKOUT timing   */
    adc3110_spi_ads23_write(fd, fmc, 0x1f, 0xff);  /* ADS42LB69_Reg 0x1F Fast OVR   */
    adc3110_spi_ads23_write(fd, fmc, 0x30, 0x00);  /* ADS42LB69_Reg 0x30 SYNC IN disabled   */
  }

  if( chan_set & (ADC3110_CHAN_SET_4|ADC3110_CHAN_SET_5))
  {
    /* *****************************************************************/
    /* Initialisation ADC_3110 ads42lb69 45                            */      
    /* *****************************************************************/
    if(adc3110_verbose_flag) printf("Initialisation ADC_3110 ads42lb69 45\n");
    adc3110_spi_ads45_write(fd, fmc, 0x8, 0x19);  /* ADS42LB69_Reg 0x08 RESET device   */
    usleep( 10000);
    adc3110_spi_ads45_write(fd, fmc, 0x4, 0x00);  /* ADS42LB69_Reg 0x08 RESET device   */
    adc3110_spi_ads45_write(fd, fmc, 0x5, 0x00);  /* ADS42LB69_Reg 0x08 RESET device   */
    adc3110_spi_ads45_write(fd, fmc, 0x6, 0x00);  /* ADS42LB69_Reg 0x06 LVDS CLKDIV=0 : Bypassed   */
    adc3110_spi_ads45_write(fd, fmc, 0x7, 0x00);  /* ADS42LB69_Reg 0x07 SYNC_IN delay = 0 ps   */
    adc3110_spi_ads45_write(fd, fmc, 0x8, 0x0c);  /* ADS42LB69_Reg 0x07 SYNC_IN delay = 0 ps   */
    adc3110_spi_ads45_write(fd, fmc, 0xb, 0x00);  /* ADS42LB69_Reg 0x0B Channel A(even) Gain disabled 0dB /No FLIP   */
    adc3110_spi_ads45_write(fd, fmc, 0xc, 0x00);  /* ADS42LB69_Reg 0x0C Channel B(Odd) Gain disabled 0dB /No OVR   */
    adc3110_spi_ads45_write(fd, fmc, 0xd, 0x00);  /* ADS42LB69_Reg 0x0D OVR pin normal   */
    //adc3110_spi_ads45_writfd,e( fmc, 0xe, 0x90);  /* High frequency  JFG  */
    adc3110_spi_ads45_write(fd, fmc, 0xf, 0x00);  /* ADS42LB69_Reg 0x0F Normal operation    */
    //adc3110_spi_ads45_writfd,e( fmc, 0xf, 0x44);  /* ADS42LB69_Reg 0x0F Test operation (ramp)    */
    adc3110_spi_ads45_write(fd, fmc, 0x10, 0x00);  /* ADS42LB69_Reg 0x10 Custom Pattern MSB    */
    adc3110_spi_ads45_write(fd, fmc, 0x11, 0x01);  /* ADS42LB69_Reg 0x11 Custom Pattern LSB   */
    adc3110_spi_ads45_write(fd, fmc, 0x14, 0x0c);  /* ADS42LB69_Reg 0x14 LVDS strenght (Default DATA/CLK))   */
    adc3110_spi_ads45_write(fd, fmc, 0x15, 0x01);  /* ADS42LB69_Reg 0x15 DDR Mode enabled   */
    adc3110_spi_ads45_write(fd, fmc, 0x16, 0x00);  /* ADS42LB69_Reg 0x16 DDR Timing 0ps(default)   */
    adc3110_spi_ads45_write(fd, fmc, 0x17, 0x00);  /* ADS42LB69_Reg 0x17 QDR CLKOUT delay   */
    adc3110_spi_ads45_write(fd, fmc, 0x18, 0x00);  /* ADS42LB69_Reg 0x18 QDR CLKOUT timing   */
    adc3110_spi_ads45_write(fd, fmc, 0x1f, 0xff);  /* ADS42LB69_Reg 0x1F Fast OVR   */
    adc3110_spi_ads45_write(fd, fmc, 0x30, 0x00);  /* ADS42LB69_Reg 0x30 SYNC IN disabled   */
  }

  if( chan_set & (ADC3110_CHAN_SET_6|ADC3110_CHAN_SET_7))
  {
    /* *****************************************************************/
    /* Initialisation ADC_3110 ads42lb69 67                            */      
    /* *****************************************************************/
    if(adc3110_verbose_flag) printf("Initialisation ADC_3110 ads42lb69 67\n");
    adc3110_spi_ads67_write(fd, fmc, 0x8, 0x19);  /* ADS42LB69_Reg 0x08 RESET device   */
    usleep( 10000);
    adc3110_spi_ads67_write(fd, fmc, 0x4, 0x00);  /* ADS42LB69_Reg 0x08 RESET device   */
    adc3110_spi_ads67_write(fd, fmc, 0x5, 0x00);  /* ADS42LB69_Reg 0x08 RESET device   */
    adc3110_spi_ads67_write(fd, fmc, 0x6, 0x00);  /* ADS42LB69_Reg 0x06 LVDS CLKDIV=0 : Bypassed   */
    adc3110_spi_ads67_write(fd, fmc, 0x7, 0x00);  /* ADS42LB69_Reg 0x07 SYNC_IN delay = 0 ps   */
    adc3110_spi_ads67_write(fd, fmc, 0x8, 0x0c);  /* ADS42LB69_Reg 0x07 SYNC_IN delay = 0 ps   */
    adc3110_spi_ads67_write(fd, fmc, 0xb, 0x00);  /* ADS42LB69_Reg 0x0B Channel A(even) Gain disabled 0dB /No FLIP   */
    adc3110_spi_ads67_write(fd, fmc, 0xc, 0x00);  /* ADS42LB69_Reg 0x0C Channel B(Odd) Gain disabled 0dB /No OVR   */
    adc3110_spi_ads67_write(fd, fmc, 0xd, 0x00);  /* ADS42LB69_Reg 0x0D OVR pin normal   */
    //adc3110_spi_ads67_write( fmc, 0xe, 0x90);  /* High frequency  JFG  */
    adc3110_spi_ads67_write(fd, fmc, 0xf, 0x00);  /* ADS42LB69_Reg 0x0F Normal operation    */
    //adc3110_spi_ads67_write( fmc, 0xf, 0x44);  /* ADS42LB69_Reg 0x0F Test operation (ramp)    */
    adc3110_spi_ads67_write(fd, fmc, 0x10, 0x00);  /* ADS42LB69_Reg 0x10 Custom Pattern MSB    */
    adc3110_spi_ads67_write(fd, fmc, 0x11, 0x01);  /* ADS42LB69_Reg 0x11 Custom Pattern LSB   */
    adc3110_spi_ads67_write(fd, fmc, 0x14, 0x0c);  /* ADS42LB69_Reg 0x14 LVDS strenght (Default DATA/CLK))   */
    adc3110_spi_ads67_write(fd, fmc, 0x15, 0x01);  /* ADS42LB69_Reg 0x15 DDR Mode enabled   */
    adc3110_spi_ads67_write(fd, fmc, 0x16, 0x00);  /* ADS42LB69_Reg 0x16 DDR Timing 0ps(default)   */
    adc3110_spi_ads67_write(fd, fmc, 0x17, 0x00);  /* ADS42LB69_Reg 0x17 QDR CLKOUT delay   */
    adc3110_spi_ads67_write(fd, fmc, 0x18, 0x00);  /* ADS42LB69_Reg 0x18 QDR CLKOUT timing   */
    adc3110_spi_ads67_write(fd, fmc, 0x1f, 0xff);  /* ADS42LB69_Reg 0x1F Fast OVR   */
    adc3110_spi_ads67_write(fd, fmc, 0x30, 0x00);  /* ADS42LB69_Reg 0x30 SYNC IN disabled   */
  }
  return;
}

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * Function name : adc3110_ads42lb69_dump
 * Prototype     : void
 * Parameters    : fmc  FMC identifier (1 or 2)
 * Return        : void
 *----------------------------------------------------------------------------
 * Description   : display the content of the ads42lb69 registers.
 *
 *++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
void
adc3110_ads42lb69_dump(int fd, int fmc)
{
  int reg, data;

  printf("\nDump ADS42LB69 Channel 01:");
  for( reg = 0; reg < 256; reg += 8)
  {
    int i;

    printf("\n%2x : ", reg);
    for( i = 0; i <  8; i++)
    {
      data = adc3110_spi_ads01_read(fd, fmc, reg+i);
      printf("%04x ", (unsigned short)data);
    }
  }
  printf("\n");

  printf("\nDump ADS42LB69 Channel 23:");
  for( reg = 0; reg < 256; reg += 8)
  {
    int i;

    printf("\n%2x : ", reg);
    for( i = 0; i <  8; i++)
    {
      data = adc3110_spi_ads23_read(fd, fmc, reg+i);
      printf("%04x ", (unsigned short)data);
    }
  }
  printf("\n");
  printf("\nDump ADS42LB69 Channel 45:");
  for( reg = 0; reg < 256; reg += 8)
  {
    int i;

    printf("\n%2x : ", reg);
    for( i = 0; i <  8; i++)
    {
      data = adc3110_spi_ads45_read(fd, fmc, reg+i);
      printf("%04x ", (unsigned short)data);
    }
  }
  printf("\n");

  printf("\nDump ADS42LB69 Channel 67:");
  for( reg = 0; reg < 256; reg += 8)
  {
    int i;

    printf("\n%2x : ", reg);
    for( i = 0; i <  8; i++)
    {
      data = adc3110_spi_ads23_read(fd, fmc, reg+i);
      printf("%04x ", (unsigned short)data);
    }
  }
  printf("\n");
  return;
}

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * Function name : adc3110_ads42lb69_set_mode
 * Prototype     : int
 * Parameters    : fmc  FMC identifier (1 or 2)
 *                 chan channel number (0 -> ADC3110_CHAN_NUM - 1)
 *                 mode  operation mode
 * Return        : mode  current value of ADC42LB67 register 0xF
 *----------------------------------------------------------------------------
 * Description   : allow to set the operation mode of for ADC channel referred 
 * by chan. The mode (loaded in register 0xF) shall be:
 * - ADC3110_ADS_MODE_NORM    → normal operation
 * - ADC3110_ADS_MODE_ZERO    → output all 0s
 * - ADC3110_ADS_MODE_ONE     → output all 1s
 * - ADC3110_ADS_MODE_TOGGLE  → output toggles 0s and 1s
 * - ADC3110_ADS_MODE_RAMP    → output digital ramp from 0 to 65535
 * - ADC3110_ADS_MODE_TEST1   → output test pattern 1
 * - ADC3110_ADS_MODE_TEST2   → output alternate test pattern 1 & 2
 * - ADC3110_ADS_MODE_SINE    → output sine wave
 *
 *++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

int 
adc3110_ads42lb69_set_mode(int fd, int fmc,
			    int chan,
			    int mode)
{
  int csr;

  if( chan >= ADC3110_CHAN_NUM)
  {
    return( -1);
  }
  csr =  adc3110_spi_read(fd, fmc, adc3110_spi_ads[chan], 0xf);
  if( chan & 1)
  {
    csr = (csr&0xf0) | (mode&0xf);
  }
  else
  {
    csr = (csr&0xf) | ((mode<<4)&0xf0);
  }
  adc3110_spi_write(fd, fmc, adc3110_spi_ads[chan], 0xf, csr);
  csr =  adc3110_spi_read(fd, fmc, adc3110_spi_ads[chan], 0xf);
  return( csr);
}

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * Function name : adc3110_ads42lb69_set_pattern
 * Prototype     : int
 * Parameters    : fmc  FMC identifier (1 or 2)
 * 				   chan  channel number (0 -> ADC3110_CHAN_NUM - 1)
 * 				   pattern	32 bit pattern loaded in registers 0x10 → 0x13
 * Return        : pattern	current value of ADC42LB67 register 0x10 → 0x13
 *----------------------------------------------------------------------------
 * Description   : allow to set for any channel the data pattern to be used in 
 * test mode
 *
 *++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
int 
adc3110_ads42lb69_set_pattern(int fd, int fmc,
			       int chan,
			       int pattern)
{
  int csr;

  if( chan >= ADC3110_CHAN_NUM)
  {
    return( -1);
  }
  csr = (pattern >> 24) & 0xff;
  adc3110_spi_write(fd, fmc, adc3110_spi_ads[chan], 0x10, csr);
  csr = (pattern >> 16) & 0xff;
  adc3110_spi_write(fd, fmc, adc3110_spi_ads[chan], 0x11, csr);
  csr = (pattern >> 8) & 0xff;
  adc3110_spi_write(fd, fmc, adc3110_spi_ads[chan], 0x12, csr);
  csr = pattern & 0xff;
  adc3110_spi_write(fd, fmc, adc3110_spi_ads[chan], 0x13, csr);

  csr = adc3110_spi_read(fd, fmc, adc3110_spi_ads[chan], 0x10) << 24;
  csr |= adc3110_spi_read(fd, fmc, adc3110_spi_ads[chan], 0x11) << 16;
  csr |= adc3110_spi_read(fd, fmc, adc3110_spi_ads[chan], 0x12) << 8;
  csr |= adc3110_spi_read(fd, fmc, adc3110_spi_ads[chan], 0x13);

  return( csr);
} 

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * Function name : adc3110_calib_set_idelay
 * Prototype     : int
 * Parameters    : fmc  FMC identifier (1 or 2)
 *                 chan  channel number (0 -> ADC3110_CHAN_NUM - 1) 
 *                 		 if - 1 adjust for all channels
 *                 idelay  calibration delay ( 0 -> 0x1ff) 
 *                 		   if -1 reset IDELAY
 * Return        : current value of IDELAY register
 *----------------------------------------------------------------------------
 * Description   : adjust the data interface calibration delay to delay for 
 * channel chan. If chan equal -1, ajustment is applied to all channels. 
 * If delay is set to -1, default delay value ir restored.
 *
 *++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

int 
adc3110_calib_set_idelay(int fd, int fmc,
			  int chan,
			  int idelay)
{
  int csr;
  volatile int tmp;


  if( chan >= ADC3110_CHAN_NUM)
  {
    return( -1);
  }
  adc3110_csr_wr(fd, fmc, ADC3110_CSR_IDELAY, 0xc0007fff); /*  RESET IDELAYE3 + ISERDES3 */
  tmp =  adc3110_csr_rd(fd, fmc, ADC3110_CSR_IDELAY);
  if( idelay == -1) return( tmp);

  csr = 0x0;
  if( chan == -1) csr |= 0x7fff;
  else  csr |= (chan/2) << 12;
  csr |= (idelay&0x1ff) << 16;
  if( chan & 1) csr|= 0xfc0;
  else csr |= 0x3f;

  usleep(10000);
  adc3110_csr_wr(fd, fmc, ADC3110_CSR_IDELAY, 0x0);
  tmp =  adc3110_csr_rd(fd, fmc, ADC3110_CSR_IDELAY);
  adc3110_csr_wr(fd, fmc, ADC3110_CSR_IDELAY, csr);
  tmp =  adc3110_csr_rd(fd, fmc, ADC3110_CSR_IDELAY);
  adc3110_csr_wr(fd, fmc, ADC3110_CSR_IDELAY, csr|0x10000000);
  tmp =  adc3110_csr_rd(fd, fmc, ADC3110_CSR_IDELAY);
  adc3110_csr_wr(fd, fmc, ADC3110_CSR_IDELAY, 0x0000000); /* force to use test IOxOS pattern */
  csr =  adc3110_csr_rd(fd, fmc, ADC3110_CSR_IDELAY);
  return(csr);
}

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * Function name : adc3110_calib_get_idelay
 * Prototype     : int
 * Parameters    : fmc  FMC identifier (1 or 2)
 *                 chan  channel number ( 0 -> ADC3110_CHAN_NUM - 1)
 * Return        : current value of IDELAY register
 *----------------------------------------------------------------------------
 * Description   : return the current value of ADC3110_CSR_IDELAY register for 
 * channel chan
 *
 *++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

int 
adc3110_calib_get_idelay(int fd, int fmc,
			  int chan)
{
  int csr;


  if( chan >= ADC3110_CHAN_NUM)
  {
    return( -1);
  }
  csr = 0x0;
  if( chan == -1) csr |= 0x7fff;
  else  csr |= (chan/2) << 12;
  if( chan & 1) csr|= 0xfc0;
  else csr |= 0x3f;

  adc3110_csr_wr(fd, fmc, ADC3110_CSR_IDELAY, csr);
  csr =  adc3110_csr_rd(fd, fmc, ADC3110_CSR_IDELAY);
  return(csr);
}


/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * Function name : adc3110_gpio_trig
 * Prototype     : int
 * Parameters    : fmc number (1 or 2)
 * Return        : fail/success
 *----------------------------------------------------------------------------
 * Description   : 
 *
 *++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/


int
adc3110_gpio_trig(int fd, int fmc)
{
  adc3110_csr_wr(fd, fmc, ADC3110_CSR_GPIO, 0x10); /* set output low */
  adc3110_csr_wr(fd, fmc, ADC3110_CSR_GPIO, 0x20); /* set output high */
  adc3110_csr_wr(fd, fmc, ADC3110_CSR_GPIO, 0x10); /* set output low */
  return(0);
}


