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
#include <fmclib.h>
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
  int sign, ret;

  ret = fmc_init(fd);
  if (ret < 0)
    return;
    
  sign = 0x32100000;
  
  ret = fmc_identify(fd, fmc, &sign, NULL, NULL);
  if (ret < 0)
    return;
  
  fmc_csr_write(fd, fmc, ADC_CSR_CTL, 0x1c00);
  usleep( 50000);
  fmc_csr_write(fd, fmc, ADC_CSR_CTL, 0x4000);
  usleep( 50000);
  fmc_csr_write(fd, fmc, ADC_CSR_CTL, 0x00);

  return;
}

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * Function name : adc_jesd_present
 * Prototype     : int
 * Parameters    : fmc  fmc number (1 or 2)
 *                 idx  JESD204 interface index
 * Return        : 1 = JESD204 interface present, 0 = absent 
 *----------------------------------------------------------------------------
 * Description   : returns if the JESD204 interface is present or abent
 *
 *++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

int adc3210_jesd_present(int fd, int fmc, int idx)
{
  int csr, cmd, res;

  csr = ADC3210_TCSR_JSD_CTL1;
  cmd = ((idx&3)<<20);
  
  fmc_csr_write(fd, fmc, csr, cmd);
  
  fmc_csr_read(fd, fmc, csr, &res);

  return ((((res>>20)&3) != idx) ? 0 : 1);
}

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * Function name : adc_jesd_read
 * Prototype     : int
 * Parameters    : fmc  fmc number (1 or 2)
 *                 idx  JESD204 interface index
 *                 reg  register index
 * Return        : content of register
 *----------------------------------------------------------------------------
 * Description   : returns the content of JESD204 interface register reg 
 * located in the resource identified by idx
 *
 *++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

int adc3210_jesd_read(int fd, int fmc, int idx, int reg)
{
  int tmo, data, csr, cmd, res;

  if (adc3210_jesd_present(fd, fmc, idx) != 1)
  {
    printf("adc3210_jesd_read() = device not present !\n");
    return (-2);
  }
  
  csr = ADC3210_TCSR_JSD_CTL1;
  cmd = (ADC3210_JESD_GO | ADC3210_JESD_RnW | ((idx&3)<<20) | ((reg<<2)&0xfffc));
  tmo = 1000;
  fmc_csr_write(fd, fmc, csr, cmd);
  if(adc3210_verbose_flag) printf("in adc3210_jesd_read( %x, %x, %x) csr = %x\n", fmc, idx, reg, csr);
  while( --tmo)
  {
    fmc_csr_read(fd, fmc, csr, &res);
    if (!(res & ADC3210_JESD_BUSY)) break;
  }
  if(tmo == 0 || (res & ADC3210_JESD_TIMEOUT) || (res & ADC3210_JESD_ERROR))
  {
    printf("adc3210_jesd_read() : cmd = %08x, res = %08x -> %s...\n",
      cmd, res, ((res & ADC3210_JESD_ERROR) ? "error" : "timeout"));
    return(-1);
  }
  fmc_csr_read(fd, fmc, csr+1, &data);
  //printf("cmd = %08x - data = %08x\n", cmd, data);

  return( data);
}

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * Function name : adc_jesd_write
 * Prototype     : int
 * Parameters    : fmc  fmc number (1 or 2)
 *                 idx  JESD204 interface index
 *                 reg  register index
 *                 data  data to be written in register
 * Return        : 0  if JESD command OK
 * 				  -1  in case of timeout
 *----------------------------------------------------------------------------
 * Description   : writes data in the JESD204 interface register reg located 
 * in the resource identified by idx
 *
 *++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

int
adc3210_jesd_write(int fd,  int fmc,
		   int idx,
	 	   int reg,
		   int data)
{
  int tmo, csr, cmd, res;

  if (adc3210_jesd_present(fd, fmc, idx) != 1)
  {
    printf("adc3210_jesd_write() = device not present !\n");
    return (-2);
  }

  csr = ADC3210_TCSR_JSD_CTL1;
  cmd = (ADC3210_JESD_GO | ((idx&3)<<20) | ((reg<<2)&0xfffc));
  tmo = 1000;
  if(adc3210_verbose_flag)printf("in adc3210_jesd_write( %x, %x, %x, %x) csr = %x\n", fmc, idx, reg, data, csr);
  fmc_csr_write(fd, fmc, csr+1, data);
  fmc_csr_write(fd, fmc, csr, cmd);
  while( --tmo)
  {
    fmc_csr_read(fd, fmc, csr, & res);
    if( !(res & ADC3210_JESD_BUSY)) break;
  }
  if(tmo == 0 || res & (ADC3210_JESD_TIMEOUT) || res & (ADC3210_JESD_ERROR))
  {
    printf("adc3210_jesd_write() : cmd = %08x, res = %08x -> %s...\n",
      cmd, res, ((res & ADC3210_JESD_ERROR) ? "error" : "timeout"));
    return(-1);
  }

  return( 0);
}

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * Function name : adc3210_jesd_dump
 * Prototype     : void
 * Parameters    : fmc  FMC identifier (1 or 2)
 *                 idx  JESD204 interface index
 * Return        : void
 *----------------------------------------------------------------------------
 * Description   : display the content of the JESD204 registers.
 *
 *++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
void
adc3210_jesd_dump(int fd, int fmc, int  idx)
{
  int ret, l, i;
  uint8_t ila[16];
  uint8_t calc_cs = 0;
  
  if (adc3210_jesd_present(fd, fmc, idx) != 1)
  {
    printf("adc3210_jesd_dump() = device not present !\n");
    return;
  }
  
  printf("\nJES204 registers:\n\n");
  
  ret = adc3210_jesd_read(fd, fmc, idx, ADC3210_JREG_IF_CTL_0);
  if (ret > 0)
  {
    const char *_jes_states[] = { "idle", "sync", "wait", "run" };
    
    printf("JESD_CTL_0: 0x%08X\n"                 \
    "    [29] RX_LMFC_RESET         = %1d\n"          \
    "    [28] RX_CTRL_RESET         = %1d\n"          \
    "    [27] GT_RX_QPLL0_LOCK      = %1d\n"          \
    "    [26] GT_RX_RESET_DONE      = %1d\n"          \
    "    [25] GT_RX_SYS_RESET       = %1d\n"          \
    "    [24] GT_RX_RESET_GT        = %1d\n"          \
    " [23:20] LANE_SRESET[3:0]      = %1d%1d%1d%1d\n" \
    " [19:16] LINK_READY[3:0]       = %1d%1d%1d%1d\n" \
    " [15:12] LINK_ENABLE[3:0]      = %1d%1d%1d%1d\n" \
    " [11: 8] LANE_ENABLE[3:0]      = %1d%1d%1d%1d\n" \
    " [ 7: 4] LINK_CONF_READY[3:0]  = %1d%1d%1d%1d\n" \
    " [ 3: 2] STATUS                = %s (%02d)\n"    \
    "    [ 1] SCRAMBLE              = %1d\n"          \
    "    [ 0] EOMF_ENABLE           = %1d\n\n",
      (uint)ret, ((ret>>29)&1), ((ret>>28)&1), ((ret>>27)&1), ((ret>>26)&1), ((ret>>25)&1), ((ret>>24)&1),
      ((ret>>23)&1), ((ret>>22)&1), ((ret>>21)&1), ((ret>>20)&1),
      ((ret>>19)&1), ((ret>>18)&1), ((ret>>17)&1), ((ret>>16)&1),
      ((ret>>15)&1), ((ret>>14)&1), ((ret>>13)&1), ((ret>>12)&1),
      ((ret>>11)&1), ((ret>>10)&1), ((ret>>9)&1),  ((ret>>8)&1),
      ((ret>>7)&1),  ((ret>>6)&1),  ((ret>>5)&1),  ((ret>>4)&1),
      _jes_states[((ret>>3)&3)], ((ret>>3)&3), ((ret>>1)&1), (ret&1));

    ret = adc3210_jesd_read(fd, fmc, idx, ADC3210_JREG_IF_CTL_1);
    
    printf("JESD_CTL_1: 0x%08X\n"  \
    " [26:16] MF (F*K / 4)  = %-4d\n"   \
    " [12:08] K             = %-4d\n"   \
    " [07:00] F             = %-4d\n\n",
      ret, ((ret>>16)&0x7ff)+1, ((ret>>8)&0x1f)+1, (ret&0xff)+1);
    
    ret = adc3210_jesd_read(fd, fmc, idx, ADC3210_JREG_IF_CTL_2);
    
    printf("JESD_CTL_2: 0x%08X\n"  \
    " [26:16] LMFC_OFFSET    = %-4d\n"  \
    "    [03] LMFC_READY     = %1d\n"   \
    "    [02] SYSREF_ERROR   = %1d\n"   \
    "    [01] SYSREF_ONESHOT = %1d\n"   \
    "    [00] SYSREF_ENABLE  = %1d\n\n",
      ret, ((ret>>16)&0x7ff), ((ret>>3)&1), ((ret>>2)&1), ((ret>>1)&1), (ret&1));
    
    for (l=0; l<4; l++) 
    {
      printf("JESD_LINK_CONF_LANE_%1d: \n", l);
   
      for (i=0; i<4; i++)
      {
        ret = adc3210_jesd_read(fd, fmc, idx, ADC3210_JREG_IF_CONF_0+4*l+i);
        ila[i*4 + 0] = (uint8_t)((ret>> 0)&0xff);
        ila[i*4 + 1] = (uint8_t)((ret>> 8)&0xff);
        ila[i*4 + 2] = (uint8_t)((ret>>16)&0xff);
        ila[i*4 + 3] = (uint8_t)((ret>>24)&0xff);
      }
      
      if (ila[0] == 0x1c && ila[1] == 0x9c)
      {
        calc_cs = 0;
        for (i=2; i<15; i++)
        {
          calc_cs += ila[i];
        }
        
        printf(" +---------------+----------------+-----------------+--------------+--------------+--------------+\n");
        printf(
          " | DID    = 0x%02X | BID    = 0x%01X   | ADJCNT    = %-3d ",
            ila[2], (ila[3]&0xf), ((ila[3]>>4)&0xf));
                
        printf(
          "| LID    = %-3d | PHADJ  = %1d   | ADJDIR = %1d   |\n" \
          " | L      = %-3d  | SCR    = %1d     | F         = %-3d " \
          "| K      = %-3d ",
          (ila[4]&31),   ((ila[4]>>5)&1), ((ila[4]>>6)&1),
          (ila[5]&31)+1, ((ila[5]>>7)&1), ((ila[6])&0xff)+1,
          ((ila[7])&31)+1);

        printf(
          "| M      = %-3d | N      = %-3d |\n"   \
          " | CS     = %-3d  | N'     = %-3d   | SUBCLASSV = %1d   "   \
          "| S      = %-3d | JESDV  = %1d   ",
          (ila[8]&0xff)+1,
          ((ila[9])&0x1f)+1, ((ila[9]>>6)&3),
          ((ila[10])&0x1f)+1, ((ila[10]>>5)&7),
          ((ila[11])&0x1f)+1, ((ila[11]>>5)&7));

        printf(
          "| CF     = %-3d |\n" \
          " | HD     = %1d    | FCHK    = 0x%02X |-----------------+--------------+--------------+--------------+\n",
          (ila[12]&0x1f), ((ila[12]>>7)&1), ((ila[15])&0xff));
        printf(" +---------------+----------------+\n\n");

        if (calc_cs != ila[15])
        {
          printf("INVALID CHECKSUM (FCHK[0x%02X] != CALC_CS[0x%02X]) !\n\n", (ila[15]&0xff), calc_cs); 
        }

      }
      else
      {
        printf(" LANE NOT CONFIGURED\n\n");
      }
    }
  }

  ret = adc3210_jesd_read(fd, fmc, idx, ADC3210_JREG_PHY_VER);
  if (ret>0)
  {
    printf("JES PHY Version       = %u.%u.%u\n", ((ret>>24)&0xff), ((ret>>16)&0xff), ((ret>>8)&0xff));

    ret = adc3210_jesd_read(fd, fmc, idx, ADC3210_JREG_PHY_IPCONF);

    printf("IP configuration      = %08X\n", ret);

    ret = adc3210_jesd_read(fd, fmc, idx, ADC3210_JREG_PHY_NCOM_IF);

    printf("Number of common      = %d\n", ret);

    ret = adc3210_jesd_read(fd, fmc, idx, ADC3210_JREG_PHY_NTRX_IF);

    printf("Number of transceiver = %d\n", ret);

    ret = adc3210_jesd_read(fd, fmc, idx, ADC3210_JREG_PHY_RXLINERATE);

    printf("RX Linerate           = %5d.%06d GHz\n", (ret/1000000), (ret%1000000));

    ret = adc3210_jesd_read(fd, fmc, idx, ADC3210_JREG_PHY_RXREFCLK);

    printf("RX REFCLK             = %5d.%03d MHz\n\n", (ret/1000), (ret%1000));
  }
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
  int tmo, data, csr, tmp;

  csr = ADC3210_TCSR_SPI_CTL1;
  if( cmd & ADC3210_SPI_2)
  {
    csr = ADC3210_TCSR_SPI_CTL2;
    cmd &= ~ADC3210_SPI_2;
  }
  cmd |=  0x80000000 | (reg&0xffff);
  tmo = 1000;
  fmc_csr_write(fd, fmc, csr, cmd);
  if(adc3210_verbose_flag)printf("in adc3210_spi_read( %x, %x, %x) csr = %x\n", fmc, cmd, reg, csr);
  while( --tmo)
  {
    fmc_csr_read(fd, fmc, csr, &tmp);
    
    if(!(tmp & 0x80000000)) break;
  }
  if( !tmo)
  {
    printf("adc3210_spi_read() : cmd = %08x -> timeout...\n", cmd);
    return(-1);
  }
  fmc_csr_read(fd, fmc, csr+1, &data);
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
  int tmo, csr, tmp;

  csr = ADC3210_TCSR_SPI_CTL1;
  if( cmd & ADC3210_SPI_2)
  {
    csr = ADC3210_TCSR_SPI_CTL2;
    cmd &= ~ADC3210_SPI_2;
  }

  cmd |=  0xc0000000 | (reg&0xffff);
  tmo = 1000;
  if(adc3210_verbose_flag)printf("in adc3210_spi_write( %x, %x, %x, %x) csr = %x\n", fmc, cmd, reg, data, csr);
  fmc_csr_write(fd, fmc, csr+1, data);
  fmc_csr_read(fd, fmc, csr+1, &data);
  fmc_csr_write(fd, fmc, csr, cmd);
  while( --tmo)
  {
    fmc_csr_read(fd, fmc, csr, &tmp);
    if( !(tmp & 0x80000000)) break;
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
    fmc_csr_read(fd, fmc, ADC3210_TCSR_I2C_CTL, &csr);
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
  fmc_csr_write(fd, fmc, ADC3210_TCSR_I2C_CMD, cmd);

  /* trig command cycle */
  fmc_csr_write(fd, fmc, ADC3210_TCSR_I2C_CTL, dev & ~ADC3210_I2C_TRIG_MASK);
  fmc_csr_write(fd, fmc, ADC3210_TCSR_I2C_CTL, dev | ADC3210_I2C_TRIG_CMD);

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
  fmc_csr_write(fd, fmc, ADC3210_TCSR_I2C_CTL, dev & ~ADC3210_I2C_TRIG_MASK);
  fmc_csr_write(fd, fmc, ADC3210_TCSR_I2C_CTL, dev | ADC3210_I2C_TRIG_DATR);

  /* wait for data to be ready */
  tmo = 1000;
  csr = i2c_wait(fd, fmc, tmo);
  if( csr == -1)
  {
    return( csr);
  }

  /* get data */
  fmc_csr_read(fd, fmc, ADC3210_TCSR_I2C_DATR, &data);

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
  fmc_csr_write(fd, fmc, ADC3210_TCSR_I2C_CMD, reg);

  /* load data register */
  fmc_csr_write(fd, fmc, ADC3210_TCSR_I2C_DATW, data);

  /* trig write cycle */
  fmc_csr_write(fd, fmc, ADC3210_TCSR_I2C_CTL, dev & ~ADC3210_I2C_TRIG_MASK);
  fmc_csr_write(fd, fmc, ADC3210_TCSR_I2C_CTL, dev | ADC3210_I2C_TRIG_DATW);

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
  fmc_csr_write(fd, fmc, ADC_CSR_LED, 0x80000003);   /* FP Led flashing + CCHD575-100MHz  Power-on */
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

