/*=========================< begin file & file header >=======================
 *  References
 *  
 *    filename : adc3210lib.c
 *    author   : JFG, XP
 *    company  : IOxOS
 *    creation : october 18,2018
 *
 *----------------------------------------------------------------------------
 *  Description
 *
 *    That library contains a set of function to access the TOSCA II XUSER
 *    FASTSCOPE ADC3210 data acquisition logic.
 *
 *----------------------------------------------------------------------------
 *
 *  Copyright Notice
 *  
 *    Copyright and all other rights in this document are reserved by 
 *    IOxOS Technologies SA. This documents contains proprietary information    
 *    and is supplied on express condition that it may not be disclosed, 
 *    reproduced in whole or in part, or used for any other purpose other
 *    than that for which it is supplies, without the written consent of  
 *    IOxOS Technologies SA                                                        
 *
 *=============================< end file header >============================*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <tsculib.h>
#include <tscextlib.h>
#include <adclib.h>
#include <adc3210lib.h>

int adc3210_verbose_flag = 0;
int adc3210_spi_ads[ADC3210_CHAN_NUM] =
{
  ADC3210_SPI_ADS01,
  ADC3210_SPI_ADS01,
  ADC3210_SPI_ADS23,
  ADC3210_SPI_ADS23,
};


int adc3210_set_verbose(int vf)
{
  adc3210_verbose_flag = vf;
  return(adc3210_verbose_flag);
}

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * Function name : adc3210_XXX
 * Prototype     : int
 * Parameters    : void
 * Return        : 
 *----------------------------------------------------------------------------
 * Description   : 
 *
 *++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

int 
adc3210_XXX( void)
{

  return( 0);
}

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * Function name : adc3210_reset
 * Prototype     : void
 * Parameters    : fmc FMC identifier (1 or 2)
 * Return        : void
 *----------------------------------------------------------------------------
 * Description   : perform a reset of the ADC3210 FMC by setting and re-setting
 * bit 8 of the control register (ADC_CSR_CTL)
 *
 *++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

void adc3210_reset(int fd, int fmc)
{
  int sign;
  int pon;

  pon = 0xc0000000;
  tsc_pon_write(fd, 0xc, &pon);
  usleep( 20000);
  sign = adc_csr_rd(fd, fmc, ADC_CSR_SIGN);
  adc_csr_wr(fd, fmc, ADC_CSR_SIGN, sign);
  adc_csr_wr(fd, fmc, ADC_CSR_CTL, 0x1c00);
  usleep( 50000);
  adc_csr_wr(fd, fmc, ADC_CSR_CTL, 0x4000);
  usleep( 50000);
  adc_csr_wr(fd, fmc, ADC_CSR_CTL, 0x00);

  return;
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

int adc3210_spi_read(int fd, int fmc, int cmd,int reg)
{
  int tmo, data, csr;

  csr = ADC3210_TCSR_SPI_CTL1;
  if( cmd & ADC3210_SPI_2)
  {
    csr = ADC3210_TCSR_SPI_CTL2;
    cmd &= ~ADC3210_SPI_2;
  }
  cmd |=  0x80000000 | (reg&0xffff);
  tmo = 1000;
  adc_csr_wr(fd, fmc, csr, cmd);
  if(adc3210_verbose_flag)printf("in adc3210_spi_read( %x, %x, %x) csr = %x\n", fmc, cmd, reg, csr);
  while( --tmo)
  {
    if( !(adc_csr_rd(fd, fmc, csr) & 0x80000000)) break;
  }
  if( !tmo)
  {
    printf("adc3210_spi_read() : cmd = %08x -> timeout...\n", cmd);
    return(-1);
  }
  data = adc_csr_rd(fd, fmc, csr+1);
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

int adc3210_spi_write(int fd, int fmc, int cmd, int reg, int data)
{
  int tmo, csr;

  csr = ADC3210_TCSR_SPI_CTL1;
  if( cmd & ADC3210_SPI_2)
  {
    csr = ADC3210_TCSR_SPI_CTL2;
    cmd &= ~ADC3210_SPI_2;
  }

  cmd |=  0xc0000000 | (reg&0xffff);
  tmo = 1000;
  if(adc3210_verbose_flag)printf("in adc3210_spi_write( %x, %x, %x, %x) csr = %x\n", fmc, cmd, reg, data, csr);
  adc_csr_wr(fd, fmc, csr+1, data);
  data = adc_csr_rd(fd, fmc, csr+1);
  adc_csr_wr(fd, fmc, csr, cmd);
  while( --tmo)
  {
    if( !(adc_csr_rd(fd, fmc, csr) & 0x80000000)) break;
  }
  if( !tmo)
  {
    printf("adc3210_spi_write() : cmd = %08x -> timeout...\n", cmd);
    return(-1);
  }

  return( 0);
}

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * Function name : adc3210_i2c_cmd
 * Prototype     : int
 * Parameters    : fmc  fmc number (1 or 2)
 *                 dev  I2C device address
 *                 cmd  I2C command
 *                 reg  register index
 * Return        : content of register
 *----------------------------------------------------------------------------
 * Description   : returns the content of register reg located in the resource 
 * identified by cmd
 *
 *++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
int i2c_wait(int fd, int fmc, int tmo)
{
  int csr;

  do
  {
    csr = adc_csr_rd(fd, fmc, ADC3210_TCSR_I2C_CTL);
    if( ( csr & ADC3210_I2C_STS_MASK) != ADC3210_I2C_STS_RUNNING) break;
  } while( --tmo);
  if( !tmo)
  {
    printf("adc3210_i2c_wait() : csr = %08x -> timeout...\n", csr);
    return( -1);
  }
  return( csr);
}

int
adc3210_i2c_cmd(int fd, int fmc, int dev, int cmd)
{
  int tmo, csr;

  if(adc3210_verbose_flag) printf("adc3210_i2c_cmd( %x, %x, %x)\n", fmc, dev, cmd);
  /* load command register */
  adc_csr_wr(fd, fmc, ADC3210_TCSR_I2C_CMD, cmd);

  /* trig command cycle */
  adc_csr_wr(fd, fmc, ADC3210_TCSR_I2C_CTL, dev & ~ADC3210_I2C_TRIG_MASK);
  adc_csr_wr(fd, fmc, ADC3210_TCSR_I2C_CTL, dev | ADC3210_I2C_TRIG_CMD);

  /* wait for command to be ready */
  tmo = 1000;
  csr = i2c_wait(fd, fmc, tmo);

  return( csr);
}

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * Function name : adc3210_i2c_read
 * Prototype     : int
 * Parameters    : fmc  fmc number (1 or 2)
 *                 cmd  I2C device address
 *                 reg  register index
 * Return        : content of register
 *----------------------------------------------------------------------------
 * Description   : returns the content of register reg located in the resource 
 * identified by cmd
 *
 *++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

int adc3210_i2c_read(int fd, int fmc, int dev)
{
  int tmo, csr, data;

  if(adc3210_verbose_flag) printf("adc3210_i2c_read( %x, %x)\n", fmc, dev);
  /* trig read cycle */
  //printf("adc3210_i2c_read: trig read cycle %08x\n", dev | ADC3210_I2C_TRIG_DATR);
  adc_csr_wr(fd, fmc, ADC3210_TCSR_I2C_CTL, dev & ~ADC3210_I2C_TRIG_MASK);
  adc_csr_wr(fd, fmc, ADC3210_TCSR_I2C_CTL, dev | ADC3210_I2C_TRIG_DATR);

  /* wait for data to be ready */
  tmo = 1000;
  csr = i2c_wait(fd, fmc, tmo);
  if( csr == -1)
  {
    return( csr);
  }

  /* get data */
  data = adc_csr_rd(fd, fmc, ADC3210_TCSR_I2C_DATR);

  return( data);
}

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * Function name : adc_spi_write
 * Prototype     : int
 * Parameters    : fmc  fmc number (1 or 2)
 *                 cmd  I2C device address
 *                 reg  register index
 *                 data  data to be written in register
 * Return        : 0  if SPI command OK
 * 				  -1  in case of timeout
 *----------------------------------------------------------------------------
 * Description   : writes data in the register reg located in the resource 
 * identified by cmd
 *
 *++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

int adc3210_i2c_write(int fd, int fmc, int dev, int reg, int data)
{
  int tmo, csr;

  if(adc3210_verbose_flag) printf("adc3210_i2c_write( %x, %x, %x, %x)\n", fmc, dev, reg, data);
  /* load command register */
  adc_csr_wr(fd, fmc, ADC3210_TCSR_I2C_CMD, reg);

  /* load data register */
  adc_csr_wr(fd, fmc, ADC3210_TCSR_I2C_DATW, data);

  /* trig write cycle */
  adc_csr_wr(fd, fmc, ADC3210_TCSR_I2C_CTL, dev & ~ADC3210_I2C_TRIG_MASK);
  adc_csr_wr(fd, fmc, ADC3210_TCSR_I2C_CTL, dev | ADC3210_I2C_TRIG_DATW);

  /* wait for data to be written */
  tmo = 1000;
  csr = i2c_wait(fd,  fmc, tmo);
  return( csr);

}

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * Function name : adc3210_lmk_init
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

void adc3210_lmk_init(int fd, int fmc,  int lmk_reg[])
{
  int reg;

  if(adc3210_verbose_flag) printf("Initialisation LMK04906\n");
  return;
  adc3210_spi_lmk_write(fd, fmc, 0x0, 0x00020000); /* LMK04803B_R00 Generate a programmable RESET to the LMK04803B   */
  usleep( 50000);
  adc3210_spi_lmk_write(fd, fmc, 0xb, lmk_reg[0xb]);
  usleep( 50000);
  for( reg = 0; reg <= 0x10; reg++)
  {
    //printf("lmk reg %02d : %08x..", reg, adc3210_spi_lmk_read( fmc, reg));
    if( reg != 0xb)
    {
      adc3210_spi_lmk_write(fd, fmc, reg, lmk_reg[ reg]);
    }
    //printf("%08x\n", adc3210_spi_lmk_read( fmc, reg));
  }
  for( reg = 0x11; reg < 0x18; reg++)
  {
    //printf("lmk reg %02d : %08x\n", reg, adc3210_spi_lmk_read( fmc, reg));
  }
  for( reg = 0x18; reg < 0x1f; reg++)
  {
    //printf("lmk reg %02d : %08x..", reg, adc3210_spi_lmk_read( fmc, reg));
    adc3210_spi_lmk_write(fd, fmc, reg, lmk_reg[ reg]);
    //printf("%08x\n", adc3210_spi_lmk_read( fmc, reg));
  }

  /* --------------------------------------------*/
  /* Enable On-board 100 MHz clock from +OSC575  */
  /* --------------------------------------------*/
  adc_csr_wr(fd, fmc, ADC_CSR_LED, 0x80000003);   /* FP Led flashing + CCHD575-100MHz  Power-on */
  usleep( 20000);
  adc3210_spi_lmk_write(fd, fmc, 0x1e, lmk_reg[ 0x1e]);             /*  LMK04803B_R30 PLL2 P/N Recallibration */
  adc3210_spi_lmk_write(fd, fmc, 0x0c, lmk_reg[ 0x0c] | 0x800000);  /*  LMK04906_R12 LD pin programmable  */
  usleep( 20000);
  /* --------------------------------------------*/
  /* Check if manual SYNC to be performed        */
  /* --------------------------------------------*/
  if( !(lmk_reg[ 0x0b]&0x8000))
  {
    printf("Perform manual synchronisation of clock outputs\n");
    adc3210_spi_lmk_write(fd, fmc, 0x1e, lmk_reg[ 0x0b] | 0x10000); /* force manual SYNC */
    usleep( 20000);
    adc3210_spi_lmk_write(fd, fmc, 0x1e, lmk_reg[ 0x0b]);
  }

  return;
}

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * Function name : adc3210_lmk_dump
 * Prototype     : void
 * Parameters    : fmc  FMC identifier (1 or 2)
 * Return        : void
 *----------------------------------------------------------------------------
 * Description   : displays the content of the 32 registers
 *
 *++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

void adc3210_lmk_dump(int fd, int fmc)
{
  int reg, data;

  printf("\nDump LMK registers:");
  for( reg = 0; reg < 32; reg += 4)
  {
    int i;

    printf("\n%2x : ", reg);
    for( i = 0; i <  4; i++)
    {
      data = adc3210_spi_lmk_read(fd, fmc, reg+i);
      printf("%08x ", data);
    }
  }
  printf("\n");

  return;
}

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * Function name : adc3210_ltc2489_conv
 * Prototype     : int
 * Parameters    : fmc  FMC identifier (1 or 2)
 *                 chan ADC channel selection (see LTC2489 data sheet)
 * Return        : data
 *----------------------------------------------------------------------------
 * Description   : perform a conversion if needed and return data output
 *
 *++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
int ltc2489_last_chan = 0;
int ltc2489_chan_sel[4] = { 0xb0, 0xb8, 0xb1, 0xb9};

int adc3210_ltc2489_conv(int fd, int fmc, int chan)
{
  int data, val;

  if( (chan < 0) || (chan > 3))
  {
    return(-1);
  }
  if(adc3210_verbose_flag) printf("Reading ltc2489\n");
  if( ltc2489_chan_sel[chan] != ltc2489_last_chan)
  {
    if(adc3210_verbose_flag) printf("Initiate ltc2489 conversion : %d : %x\n", chan, ltc2489_chan_sel[chan]);
    adc3210_i2c_write(fd, fmc, ADC3210_I2C_TADC, ltc2489_chan_sel[chan], 0);
    ltc2489_last_chan = ltc2489_chan_sel[chan];
    usleep(500000);
    adc3210_i2c_read(fd, fmc, ADC3210_I2C_TADC);
  }

  usleep(500000);
  data = adc3210_i2c_read(fd, fmc, ADC3210_I2C_TADC);

  val = ( tsc_swap_32(data) >> 14) & 0xffff;

  return( val);
}


/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * Function name : adc3210_ad9695_init
 * Prototype     : void
 * Parameters    : fmc  FMC identifier (1 or 2)
 * Return        : void
 *----------------------------------------------------------------------------
 * Description   : initializes the four ad9695 dual analog-to-digital 
 * converters present on the ADC_3210 FMC
 *
 *++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

void adc3210_ad9695_init(int fmc, int chan_set)
{
  if(adc3210_verbose_flag) printf("Initialisation AD9695\n");
  return;
}

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * Function name : adc3210_ad9695_dump
 * Prototype     : void
 * Parameters    : fmc  FMC identifier (1 or 2)
 * Return        : void
 *----------------------------------------------------------------------------
 * Description   : display the content of the ad9695 registers.
 *
 *++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
void adc3210_ad9695_dump(int fd, int fmc)
{
  int reg, data;

  printf("\nDump AD9695 Channel 01:");
  for( reg = 0; reg < 256; reg += 8)
  {
    int i;

    printf("\n%2x : ", reg);
    for( i = 0; i <  8; i++)
    {
      data = adc3210_spi_ads01_read(fd,  fmc, reg+i);
      printf("%04x ", (unsigned short)data);
    }
  }
  printf("\n");

  printf("\nDump AD9695 Channel 23:");
  for( reg = 0; reg < 256; reg += 8)
  {
    int i;

    printf("\n%2x : ", reg);
    for( i = 0; i <  8; i++)
    {
      data = adc3210_spi_ads23_read(fd,  fmc, reg+i);
      printf("%04x ", (unsigned short)data);
    }
  }
  printf("\n");
  printf("\nDump AD9695 Channel 45:");
  for( reg = 0; reg < 256; reg += 8)
  {
    int i;

    printf("\n%2x : ", reg);
    for( i = 0; i <  8; i++)
    {
      data = adc3210_spi_ads45_read(fd,  fmc, reg+i);
      printf("%04x ", (unsigned short)data);
    }
  }
  printf("\n");

  printf("\nDump AD9695 Channel 67:");
  for( reg = 0; reg < 256; reg += 8)
  {
    int i;

    printf("\n%2x : ", reg);
    for( i = 0; i <  8; i++)
    {
      data = adc3210_spi_ads23_read(fd, fmc, reg+i);
      printf("%04x ", (unsigned short)data);
    }
  }
  printf("\n");
  return;
}

