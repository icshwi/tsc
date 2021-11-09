/*=========================< begin file & file header >=======================
 *  References
 *
 *    filename : ads42lb69lib.c
 *    author   : CG
 *    company  : IOxOS
 *    creation : october 21,2020
 *
 *----------------------------------------------------------------------------
 *  Description
 *
 *    That library contains a set of function to contorl FMC slots
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
#include <string.h>
#include <tsculib.h>
#include <tscextlib.h>
#include <fmclib.h>
#include <adclib.h>
#include <ads42lb69lib.h>

/*--------------------------------------------------------------------------------------------------------------------*/

static int ads42lb69_num = 0;       /*! Number of ADCs         */
static int *ads42lb69_map = NULL;   /*! ADC to ADS map         */

/*--------------------------------------------------------------------------------------------------------------------*/

/*!
 * \brief   set number of ADS42LB69 and its mapping
 *
 * \param   ads_num   Number of ADCs 
 * \param   ads_map   ADS42LB69 mapping (ADS to SPI)
 */

void ads42lb69_configure(int ads_num, int *ads_map)
{
  ads42lb69_num = ads_num;
  ads42lb69_map = ads_map;
}

/*--------------------------------------------------------------------------------------------------------------------*/

/*!
 * \brief   Initializes ADS42LB69 Analog-to-Digital converters
 *
 * \param   fmc       FMC slot (1 or 2)
 * \param   chan_set  channel bitmask selection (1-bit per ADC channel)
 * \param   regs      register values used for the initialization
 * \param   quiet     no messages when 1
 *
 * \return  >=0 on sucess, <0 on error 
 */

int ads42lb69_init(int fd, int fmc, int chan_set, int regs[], int quiet, int fmt)
{
  int adc, ret, count, tmp;

  if (fmc < 0 || fmc > 2 || ads42lb69_map == NULL)
    return(-1);

  if (!quiet)
  {
    printf("Initialisation ADS42LB69 ");

    for (adc=0; adc<(ads42lb69_num/2); adc++)
    {
      if (chan_set & ((1<<(2*adc+0)) | (1<<(2*adc+1))))
      {
        printf("%d%d", (2*adc+0), (2*adc+1));
      }
    }
    printf("\n");
  }

  count = 0;
  for (adc=0; adc<ads42lb69_num; adc++)
  {
    if (chan_set & (1<<adc))
    {
      count++;
    }
  }

  for (adc=0; adc<(ads42lb69_num/2); adc++)
  {
    if (chan_set & ((1<<(2*adc+0)) | (1<<(2*adc+1))))
    {
      /* ADS42LB69_Reg 0x08 RESET device */
      ret = adc_spi_write(fd, fmc, ads42lb69_map[2*adc+0], 0x08, 0x01);
      if (ret < 0) 
      {
        return(ret);
      }
      usleep(20000);

      /* ADS42LB69_Reg 0x06 LVDS CLKDIV=0 : Bypassed */
      ret = adc_spi_write(fd, fmc, ads42lb69_map[2*adc+0], 0x06, regs[0x06]);
      if (ret < 0) 
      {
        return(ret);
      }

      /* ADS42LB69_Reg 0x07 SYNC_IN delay = 0 ps            */
      ret = adc_spi_write(fd, fmc, ads42lb69_map[2*adc+0], 0x07, regs[0x07]);
      if (ret < 0) 
      {
        return(ret);
      }

      /*
         Register 8, Set data format accordingly.
         fmt == 0,                                            Data format = Twos complement.
         fmt == ADS42LB69_REG8_DATA_FORMAT_OFFSET_BIN (0x10), Data format = Offset binary.
      */
      tmp = regs[0x08];
      if (fmt > 0) {
        tmp = tmp | ADS42LB69_REG8_DATA_FORMAT_OFFSET_BIN;
      }
      /* ADS42LB69_Reg 0x08 Data format = fmt + align test pattern on channel A & B */
      ret = adc_spi_write(fd, fmc, ads42lb69_map[2*adc+0], 0x08, (regs[0x08] | fmt));
      if (ret < 0) 
      {
        return(ret);
      }

      /* ADS42LB69_Reg 0x0B Channel A(even) Gain enabled with 0dB - /No FLIP   */
      ret = adc_spi_write(fd, fmc, ads42lb69_map[2*adc+0], 0x0B, regs[0x0B]);
      if (ret < 0) 
      {
        return(ret);
      }

      /* ADS42LB69_Reg 0x0C Channel B(Odd) Gain enabled with 0dB - /No OVR  */
      ret = adc_spi_write(fd, fmc, ads42lb69_map[2*adc+0], 0x0C, regs[0x0C]);
      if (ret < 0)
      {
        return(ret);
      }

      /* ADS42LB69_Reg 0x0D OVR pin normal   */
      ret = adc_spi_write(fd, fmc, ads42lb69_map[2*adc+0], 0x0D, regs[0x0D]);
      if (ret < 0) 
      {
        return(ret);
      }

      /* ADS42LB69_Reg 0x0F Normal operation    */
      ret = adc_spi_write(fd, fmc, ads42lb69_map[2*adc+0], 0x0F, regs[0x0F]);
      if (ret < 0) 
      {
        return(ret);
      }

      /* ADS42LB69_Reg 0x14 LVDS 100 ohm strength (Default DATA/CLK))   */
      ret = adc_spi_write(fd, fmc, ads42lb69_map[2*adc+0], 0x14, regs[0x14]);
      if (ret < 0) 
      {
        return(ret);
      }

      /* ADS42LB69_Reg 0x15 QDR Mode enabled */
      ret = adc_spi_write(fd, fmc, ads42lb69_map[2*adc+0], 0x15, regs[0x15]);
      if (ret < 0) 
      {
        return(ret);
      }

      /* ADS42LB69_Reg 0x16 DDR Timing 0ps (default) */
      ret = adc_spi_write(fd, fmc, ads42lb69_map[2*adc+0], 0x16, regs[0x16]);
      if (ret < 0) 
      {
        return(ret);
      }

      /* ADS42LB69_Reg 0x17 QDR CLKOUT A timing   */
      ret = adc_spi_write(fd, fmc, ads42lb69_map[2*adc+0], 0x17, regs[0x17]);
      if (ret < 0) 
      {
        return(ret);
      }

      /* ADS42LB69_Reg 0x18 QDR CLKOUT B timing   */
      ret = adc_spi_write(fd, fmc, ads42lb69_map[2*adc+0], 0x18, regs[0x18]);
      if (ret < 0) 
      {
        return(ret);
      }

      /* ADS42LB69_Reg 0x1F Fast OVR   */
      ret = adc_spi_write(fd, fmc, ads42lb69_map[2*adc+0], 0x1F, regs[0x1F]);
      if (ret < 0) 
      {
        return(ret);
      }

      /* ADS42LB69_Reg 0x30 SYNC IN disabled   */
      ret = adc_spi_write(fd, fmc, ads42lb69_map[2*adc+0], 0x20, regs[0x20]);
      if (ret < 0) 
      {
        return(ret);
      }
    }
  }
  return(1);
}

/*--------------------------------------------------------------------------------------------------------------------*/

/*!
 * \brief   Display all registers of ADS42LB69 Analog-to-Digital converters
 *
 * \param   fmc       FMC slot (1 or 2)
 * \param   chan_set  ADC channel bitmask selection (1-bit per ADC channel)
 *
 * \return  >=0 on sucess, <0 on error 
 */

int ads42lb69_dump(int fd, int fmc, int chan_set)
{
  int i, reg, data, ads, ret;

  if (fmc < 0 || fmc > 2 || ads42lb69_map == NULL)
    return(-1);

  for (ads=0; ads<(ads42lb69_num/2); ads++)
  {
    if (chan_set & ((1<<(2*ads+0)) | (1<<(2*ads+1))))
    {
      printf("\nDump ADS42LB69 channel %d%d:", (2*ads+0), (2*ads+1));

      for (reg=0; reg<32; reg += 8)
      {
        printf("\n%02X : ", reg);
        for (i=0; i<8; i++)
        {
          ret = adc_spi_read(fd, fmc, ads42lb69_map[2*ads+0], (reg+i), &data);
          if (ret < 0) return(ret);

          printf("%04X ", (data&0xffff));
        }
      }
      printf("\n");
    }
  }
  return(1);
}

/*--------------------------------------------------------------------------------------------------------------------*/

/*!
 * \brief   retrieve the mode of operation of ADC channel 
 * \details Allow to get the operation mode of ADC channel referred by chan. 
 *          the mode (loaded in register 0xF) shall be:
 * - ADS42LB69_MODE_NORM    → normal operation
 * - ADS42LB69_MODE_ZERO    → output all 0s
 * - ADS42LB69_MODE_ONE     → output all 1s
 * - ADS42LB69_MODE_TOGGLE  → output toggles 0s and 1s
 * - ADS42LB69_MODE_RAMP    → output digital ramp from 0 to 65535
 * - ADS42LB69_MODE_TEST1   → output test pattern 1
 * - ADS42LB69_MODE_TEST2   → output alternate test pattern 1 & 2
 * - ADS42LB69_MODE_SINE    → output sine wave
 *
 * \param   fmc       FMC slot (1 or 2)
 * \param   chan      ADC channel number
 * \param   mode      pointer to operation mode
 *
 * \return  >=0 on succes, current value of ADS42LB69, <0 on error 
 */

int ads42lb69_get_mode(int fd, int fmc, int chan, int *mode)
{
  int tmp, ret;

  if (fmc<0 || fmc>2 || chan<0 || chan >= ads42lb69_num || ads42lb69_map == NULL || mode == NULL)
    return(-1);

  ret = adc_spi_read(fd, fmc, ads42lb69_map[chan], 0xf, &tmp);
  if (ret < 0) return(ret);

  (*mode) = (((chan & 1) ? tmp : (tmp>>4)) & 0xf);
  
  return(tmp&0xff);
}

/*--------------------------------------------------------------------------------------------------------------------*/

/*!
 * \brief   change the mode of operation of ADC channel 
 * \details Allow to set the operation mode of ADC channel referred by chan. 
 *          the mode (loaded in register 0xF) shall be:
 * - ADS42LB69_MODE_NORM    → normal operation
 * - ADS42LB69_MODE_ZERO    → output all 0s
 * - ADS42LB69_MODE_ONE     → output all 1s
 * - ADS42LB69_MODE_TOGGLE  → output toggles 0s and 1s
 * - ADS42LB69_MODE_RAMP    → output digital ramp from 0 to 65535
 * - ADS42LB69_MODE_TEST1   → output test pattern 1
 * - ADS42LB69_MODE_TEST2   → output alternate test pattern 1 & 2
 * - ADS42LB69_MODE_SINE    → output sine wave
 *
 * \param   fmc       FMC slot (1 or 2)
 * \param   chan      ADC channel number
 * \param   mode      operation mode
 *
 * \return  >=0 on succes, current value of register 0xf, <0 on error 
 */

int ads42lb69_set_mode(int fd, int fmc, int chan, int mode)
{
  int tmp, ret;

  if (fmc<0 || fmc>2 || chan < 0 || chan >= ads42lb69_num || ads42lb69_map == NULL)
    return(-1);

  ret = adc_spi_read(fd, fmc, ads42lb69_map[chan], 0xf, &tmp);
  if (ret < 0) return(ret);

  tmp = ((chan & 1) ? ((tmp & 0xf0) | (mode & 0xf)) : ((tmp & 0xf) | ((mode << 4) & 0xf0)));

  ret = adc_spi_write(fd, fmc, ads42lb69_map[chan], 0xf, tmp);
  if (ret < 0) return (ret);

  ret = adc_spi_read(fd, fmc, ads42lb69_map[chan], 0xf, &tmp);
  if (ret < 0) return (ret);

  return(tmp&0xff);
}

/*--------------------------------------------------------------------------------------------------------------------*/

/*!
 * \brief   set the pattern of ADC channel 
 * \details Allow to set for any channel the data pattern to be used in test 
 *          mode. 
 *
 * \param   fmc       FMC slot (1 or 2)
 * \param   chan      ADC channel number
 * \param   pattern   pattern value (loaded in registers 0x10 → 0x13)
 *
 * \return  >=0 on succes, current value of registers 0x10 → 0x13 , <0 on error 
 */

int ads42lb69_set_pattern(int fd, int fmc, int chan, int pattern)
{
  int csr, i, ret;

  if(fmc < 0 || fmc > 2 || chan < 0 || chan >= ads42lb69_num || ads42lb69_map == NULL)
    return( -1);

  csr = pattern;
  for (i=0; i<4; i++)
  {
    ret = adc_spi_write(fd, fmc, ads42lb69_map[chan], (0x13-i), (csr&0xff));
    if (ret < 0) return (ret);
    csr >>= 8;
  }
  return(1);
}

/*--------------------------------------------------------------------------------------------------------------------*/

/*!
 * \brief   get the data pattern to be used in test mode of an ADC channel 
 *
 * \param   fmc       FMC slot (1 or 2)
 * \param   chan      ADC channel number
 * \param   pattern   pointer to the 32-bit pattern value
 *
 * \return  >=0 on succes, <0 on error 
 */

int ads42lb69_get_pattern(int fd, int fmc, int chan, int * pattern)
{
  int ret, data, tmp, i;

  if(fmc < 0 || fmc > 2 || chan < 0 || chan >= ads42lb69_num || ads42lb69_map == NULL)
    return( -1);

  data = 0;
  for (i=0; i<4; i++)
  {
    ret = adc_spi_read(fd, fmc, ads42lb69_map[chan], (0x10+i), &tmp);
    if (ret < 0) return(ret);

    data |= ((tmp & 0xff) << 8);
  }
  if (pattern != NULL)
  {
    (*pattern) = data;
  }
  return(1);
}

/*--------------------------------------------------------------------------------------------------------------------*/

/*!
 * \brief   set digital offset 
 * \details Allow to set for any channel the digital offset when avaiable.
 *
 * \param   fmc       FMC slot (1 or 2)
 * \param   offset    16-bit offset 
 *
 * \return  >=0 on succes, <0 on error 
 */

int ads42lb69_set_offset(int fd, int fmc, int offset[])
{
  int i, rc;

  if(fmc < 0 || fmc > 2 || ads42lb69_map == NULL || offset == NULL)
    return( -1);

  for (i=0; i<(ads42lb69_num/2); i++)
  {
    rc = fmc_csr_write(fd, fmc, ADS42LB69_CSR_OFF+i, offset[i]);
    if (rc < 0)
      break;
  }
  return(rc);
}

/*--------------------------------------------------------------------------------------------------------------------*/

/*!
 * \brief   get digital offset 
 * \details Read digital offset for a given channel group.
 *
 * \param   fmc       FMC slot (1 or 2)
 * \param   offset    pointer to 16-bit offset value 
 *
 * \return  >=0 on succes, <0 on error 
 */

int ads42lb69_get_offset(int fd, int fmc, int offset[])
{
  int i, rc;

  if(fmc < 0 || fmc > 2 || ads42lb69_map == NULL || offset == NULL)
    return( -1);

  for (i=0; i<(ads42lb69_num/2); i++)
  {
    rc = fmc_csr_read(fd, fmc, ADS42LB69_CSR_OFF+i, &offset[i]);
    if (rc < 0)
      break;
  }
  return(rc);
}


/*--------------------------------------------------------------------------------------------------------------------*/

/*!
 * \brief   Reset all IDELAY/ISERDES
 *
 * \param   fmc         FMC slot (1 or 2)
 * 
 * \return  >=0 on succes, <0 on error 
 */

int ads42lb69_calib_reset_idelay(int fd, int fmc)
{
  int rc, tmp;
  
  /* sanity check */
  if (fmc < 0 || fmc > 2 || ads42lb69_map == NULL)
  {
    return (-1);
  }

  /* Set RESET */
  tmp = (ADS42LB69_IDLY_IDELAYE3_RST | ADS42LB69_IDLY_ISERDESE3_RST);

  rc = fmc_csr_write(fd, fmc, ADS42LB69_CSR_IDELAY, tmp);
  if (rc < 0)
  {
    return (rc);
  }

  tmp = 0;
  rc = fmc_csr_write(fd, fmc, ADS42LB69_CSR_IDELAY, tmp);
  if (rc < 0)
  {
    return (rc);
  }

  return (1);
}

/*--------------------------------------------------------------------------------------------------------------------*/

/*!
 * \brief   set IDELAY of an ADC channel
 *
 * \details adjust the data interface calibration delay to delay for channel 
 *          chan. If chan equal -1, ajustment is applied to all channels.
 *          If delay is set to -1, default delay value is restored.
 * 
 * \param   fmc       FMC slot (1 or 2)
 * \param   chan      ADC channel number
 * \param   index     idelay index (-1 = first idelay or a value between 0 to 5)
 * \param   idelay    calibration delay (0 → 511), if -1 reset IDELAY
 *
 * \return  >=0 on succes, <0 on error 
 */

int ads42lb69_calib_set_idelay(int fd, int fmc, int chan, int index, int idelay)
{
  int rdbk, tmp, ret;

  /* sanity check */
  if(fmc < 0 || fmc > 2 || index < -1 || index > 5 || chan < -1 || chan >= ads42lb69_num || ads42lb69_map == NULL)
  {
    return( -1);
  }

  /* select channel */
  if (chan != -1)
  {
    /* select idelay */
    if (index != -1) 
    {
      tmp = (((chan / 2) << 12) | ((chan & 1)  ? (1 << (index+6)) : (1<<index)));
    }
    else
    {
      tmp = (((chan / 2) << 12) | ((chan & 1)  ? ADS42LB69_IDLY_IO_ALL_B : ADS42LB69_IDLY_IO_ALL_A));
    }
  }
  else
  {
    tmp = (ADS42LB69_IDLY_IO_ALL | ADS42LB69_IDLY_SEL_ALL);
  }

  /* Set IDELAY value */
  tmp |= ((idelay&0x1ff) << 16);

  ret = fmc_csr_write(fd, fmc, ADS42LB69_CSR_IDELAY, (tmp | ADS42LB69_IDLY_CMD_LD));
  if (ret<0)
  {
    return(ret);
  }

  /* Load IDELAY */
//  ret = fmc_csr_write (fmc, ADS42LB69_CSR_IDELAY, (tmp | ADS42LB69_IDLY_CMD_LD));
//  if (ret<0)
//  {
//    return(ret);
//  }
//  usleep(40);

//  ret = fmc_csr_write (fmc, ADS42LB69_CSR_IDELAY, tmp);
//  if (ret<0)
//  {
//    return(ret);
//  }

  ret = fmc_csr_read(fd, fmc, ADS42LB69_CSR_IDELAY, &rdbk);
  if (ret<0)
  {
    return(ret);
  }

  if (((rdbk>>16)&0x1ff) != (idelay&0x1ff))
  {
    fprintf(stderr, "idelay mismatch 0x%X != 0x%X (ch = %d)\n", ((rdbk>>16)&0x1ff), idelay&0x1ff, chan);
    return (-2);
  }

  return(1);
}

/*--------------------------------------------------------------------------------------------------------------------*/

/*!
 * \brief   read directly ADC value using register
 *
 * \param   fmc       fmc  FMC identifier (1 or 2)
 * \param   chan      ADC channel (-1 = all channels)
 * \param   value     pointer to for ADC value
 *
 * \return  return the status of the operation  <0 on error
 */

int ads42lb69_direct_read(int fd, int fmc, int chan, int *value)
{
  int ret, tmp;

  if (fmc < 0 || fmc > 2 || chan < 0 || chan >= ads42lb69_num || ads42lb69_map == NULL)
    return(-1);

  /* select value to read */
  ret = fmc_csr_read(fd, fmc, ADS42LB69_CSR_GPIO, &tmp);
  if (ret < 0)
  {
    return(ret);
  }

  tmp &= ~0xf;
  tmp |= (chan & 0xf);
  ret = fmc_csr_write(fd, fmc, ADS42LB69_CSR_GPIO, tmp);
  if (ret < 0)
  {
    return(ret);
  }

  ret = fmc_csr_read(fd, fmc, ADS42LB69_CSR_ADCDIR, value);

  return(ret);
}

/*--------------------------------------------------------------------------------------------------------------------*/

/*!
 * \brief   display direct ADC value
 *
 * \param   fmc           fmc  FMC identifier (1 or 2)
 * \param   chan          ADC channel (-1 = all channels)
 *
 * \return  return the status of the operation  <0 on error
 */

int ads42lb69_direct_dump(int fd, int fmc, int chan)
{
  int i, j, val, ret;

  if (fmc < 1 || fmc > 2 || chan < -1 || chan >= ads42lb69_num || ads42lb69_map == NULL)
  {
    return(-1);
  }
  
  for (i=0; i<ads42lb69_num; i++)
  {
    if (chan == -1 || chan == i)
    {
      printf("ads%d%d channel %d : ", 2*(i/2)+0, 2*(i/2)+1, i);
      
      for (j=0; j<8; j++)
      {
        ret = ads42lb69_direct_read(fd, fmc, i, &val);
        if (ret < 0)
        {
          return(ret);
        }
        printf(" %04X %04X", (val&0xffff), ((val>>16)&0xffff));
      }
      printf("\n");
    }
  }
  return(0);
}

/*--------------------------------------------------------------------------------------------------------------------*/

/*!
 * \brief   display current IDELAY values
 *
 * \param   fmc           fmc  FMC identifier (1 or 2)
 * \param   chan          ADC channel (-1 = all channels)
 * \param   idelay_name   pointer an array of IDELAY name
 *
 * \return  return the status of the operation  <0 on error
 */

int ads42lb69_calib_dump_idelay(int fd, int fmc, int chan, char *cell_str[])
{
  int csr, ret=0, tmp, i, j, idelay, ads, idly;
  char *s;
  
  if (fmc < 1 || fmc > 2 || chan < -1 || chan >= ads42lb69_num || ads42lb69_map == NULL || cell_str == NULL)
  {
    return(-1);
  }

  for (i=0; i<ads42lb69_num; i++)
  {
    if (chan == -1 || chan == i)
    {
      for (j=0; j<6; j++)
      {
        ads = (i/2);
        idly = (j + ((i&1) ? 6 : 0));
        
        csr = ((ads<<12) | (1<<idly));
        ret = fmc_csr_write(fd, fmc, ADS42LB69_CSR_IDELAY, csr);
        if (ret<0)
        {
          return(ret);
        }
        ret = fmc_csr_read(fd, fmc, ADS42LB69_CSR_IDELAY, &tmp);
        if (ret < 0)
        {
          return(ret);
        }
        idelay = ((tmp>>16) & 0x1ff);
        s = cell_str[idly];
        printf("ads%d%d %-10s : %3d (0x%03X)\n", 2*(i/2)+0, 2*(i/2)+1, ((s!=NULL) ? s: "-"), idelay, idelay);
      }
    }
  }
  return(ret);
}

/*--------------------------------------------------------------------------------------------------------------------*/

/*!
 * \brief   retrieve idelay value of an ADC channel
 *
 * \param   fmc       fmc  FMC identifier (1 or 2)
 * \param   chan      ADC channel (-1 = all channels)
 * \param   index     idelay index (-1 = first idelay or a value between 0 to 5)
 * \param   idelay    pointer to for idelay value
 *
 * \return  return the current value of ADS42LB69_CSR_IDELAY or  <0 on error
 */

int ads42lb69_calib_get_idelay(int fd, int fmc, int chan, int index, int *idelay)
{
  int ret, tmp;

  if (fmc < 0 || fmc > 2 || index < -1 || index > 5 || chan >= ads42lb69_num || ads42lb69_map == NULL)
    return(-1);

  /* select channel */
  if (chan != -1)
  {    
    /* select idelay */
    if (index != -1)
    {
      tmp = (((chan / 2)<<12) | ((chan & 1) ? (1<<(index+6)) : (1<<index)));
    }
    else
    {
      tmp = (((chan / 2)<<12) | ((chan & 1) ? ADS42LB69_IDLY_IO_ALL_B : ADS42LB69_IDLY_IO_ALL_A));
    }
  }
  else
  {
    tmp = (ADS42LB69_IDLY_SEL_ALL | ADS42LB69_IDLY_IO_ALL);
  }

  /* select IDELAY */
  ret = fmc_csr_write(fd, fmc, ADS42LB69_CSR_IDELAY, tmp);
  if (ret < 0) 
  {
    return(ret);
  }

  /* read IDELAY value */
  ret = fmc_csr_read(fd, fmc, ADS42LB69_CSR_IDELAY, &tmp);
  if (ret < 0) 
  {
    return(ret);
  }

  if (idelay != NULL)
  {
    (*idelay) = tmp;
  }
  return(1);
}

/*--------------------------------------------------------------------------------------------------------------------*/

/*!
 * \brief   Calibrate ADCs
 *
 * \param   fmc       FMC identifier (1 or 2)
 * \param   chan      ADC channel group (-1 = all channels)
 * \param   step      step (1 to 32)
 * \param   result    display result
 *
 * \return  =0 on success, >0 number of calibration failure, <0 on error
 */

int ads42lb69_calibrate(int fd, int fmc, int chan_group, int step, int result)
{
  int ch, tmp, dly, rc, data, pattern1, pattern2, dly_incr, start, end, best_start, best_end, best_dly;
  int i, tap[512], tap_pat[512], err = 0, saved;
  
  if (fmc < 0 || fmc > 2 || chan_group < -1 ||  chan_group >= (ads42lb69_num/2) || ads42lb69_map == NULL)
  {
    return(-1);
  }

  int off_saved[(ads42lb69_num/2)], off_zero[(ads42lb69_num/2)];

  memset(off_zero, 0, sizeof(off_zero));

  dly_incr = 8;
  if (step >= 1 && step <= 32)
  {
    dly_incr = step;
  }

  err = 0;

  /* save offset before calibration */
  rc = ads42lb69_get_offset(fd, fmc, off_saved);
  if (rc<0)
  {
    return(rc);
  }

  ads42lb69_set_offset(fd, fmc, off_zero);
  if (rc < 0)
  {
    return(rc);
  }

  for (ch = 0; ch < ads42lb69_num; ch++)
  {

    if ((ch == (2*chan_group+0)) || (ch == (2*chan_group+1)) || (chan_group == -1))
    {
      /* save mode before calibration */
      rc = ads42lb69_get_mode(fd, fmc, ch, &saved);
      if (rc < 0)
        break;
              
      /* set toggle mode */
      rc = ads42lb69_set_mode(fd, fmc, ch, ADS42LB69_MODE_TOGGLE);
      if (rc < 0)
        break;

      /* select channel */
      rc = fmc_csr_read(fd, fmc, ADS42LB69_CSR_GPIO, &tmp);
      if (rc < 0)
        break;

      tmp &= ~0xf;
      tmp |= (ch&0xf);
      rc = fmc_csr_write(fd, fmc, ADS42LB69_CSR_GPIO, tmp);
      if (rc < 0)
        break;

      /* set all taps to state unknown (=-1) */
      for (dly=0; dly<0x200; dly++)
      {
        tap[dly] = -1;
      }

      pattern1 = 0xAAAA5555;
      pattern2 = 0x5555AAAA;
      
      for (dly = 0x0; dly < 0x200; dly += dly_incr)
      {
        rc = ads42lb69_calib_set_idelay(fd, fmc, ch, -1, dly); 
        if (rc < 0) 
          break;
        //if (rc == -2) break;
        //if (rc < 0)
        //{
        //  return(rc);
        //}

        for (i=0; i<20; i++)
        {
          rc = fmc_csr_read(fd, fmc, ADS42LB69_CSR_ADCDIR, &data);
          if (rc < 0)
            break;

          if (data != pattern1 && data != pattern2)
          {
            tap[dly]++;
          }
          else
          {
            if (tap[dly] == -1)
            {
              tap[dly] = 0;
              tap_pat[dly] = data;
            }
          }
        }
        if (rc < 0)
          break;
      }

      /* set back ADC to previous mode */
      rc = ads42lb69_set_mode(fd, fmc, ch, saved);
      if (rc < 0)
        break;
      
      /* try to find best point */
      start = end = -1;
      best_start = best_end = -1;
      for (dly=0; dly<0x200; dly+= dly_incr)
      {
        if (start == -1)
        {
          if (tap[dly] == 0)
          {
            start = end = dly;
          }
        }
        else
        { 
          if (tap[dly] == 0)
          {
            end = dly;
          }
          else
          {
            if ((end-start) > (best_end - best_start))
            {
              best_start = start;
              best_end   = end;
            }
            start = -1;
          }
        }
      }

      /* unable to find valid data */
      if (start == -1 && end == -1)
      {
        err++;
      }
      else
      {
        if (start != -1)
        {
          if ((end-start) > (best_end - best_start))
          {
            best_start = start;
            best_end   = end;
          }
        }
        best_dly = (((best_start + best_end) / 2) / dly_incr) * dly_incr;

        /* set IDELAY */
        rc = ads42lb69_calib_set_idelay(fd, fmc, ch, -1, best_dly); 
        if (rc < 0)
          break;
      }

      /* display calibration result */
      if (result)
      {
        printf("ads%d%d %c [", (2*(ch/2)+0), (2*(ch/2)+1), (((ch&1) == 0) ? 'A' : 'B'));
        for (dly=0; dly<512; dly += dly_incr)
        {
          if (tap[dly] == 0)
          {
            if (dly >= best_start && dly <= best_end && best_start != -1)
            {
              printf((dly == best_dly) ? "*" : ((dly < best_dly) ? ">" : "<"));
            }
            else
            {
              printf("o");
            }
          }
          else
          {
            printf("-");
          }
        }
        if (best_start == -1)
        {
          printf("] calibration failure\n");
        }
        else
        {
          printf("] [0x%03X -> 0x%03X <- 0x%03X] (pattern = 0x%08X)\n", best_start, best_dly, best_end, tap_pat[best_dly]);
        }
      }

    }
  }

  /* restore offsets */
  tmp = ads42lb69_set_offset(fd, fmc, off_saved);
  if (tmp < 0)
  {
    rc = tmp;
  }

  return ((rc < 0) ? rc : err);
}

/*--------------------------------------------------------------------------------------------------------------------*/
