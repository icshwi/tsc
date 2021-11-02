/*=========================< begin file & file header >=======================
 *  References
 *  
 *    filename : adc3117lib.c
 *    author   : JFG, XP
 *    company  : IOxOS
 *    creation : october 18,2018
 *
 *----------------------------------------------------------------------------
 *  Description
 *
 *    That library contains a set of function to access the TOSCA II XUSER
 *    FASTSCOPE ADC3117 data acquisition logic.
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
#include <adc3117lib.h>

/* local macros used to display messages */
#define adc3117_info(...)          ({ if (adc3117_verbose_mode) fprintf(stdout, __VA_ARGS__); })
#define adc3117_error(...)         ({ if (adc3117_verbose_mode) fprintf(stderr, __VA_ARGS__); })

static char adc3117_line[2048];           /* buffer used to parse line of text file     */
static int  adc3117_verbose_mode = 0;     /* verbose mode                               */

/*!
 * \brief   Set verbose mode
 *
 * \param   mode        enable (=1) or disable (0) verbose mode
 *
 * \return  current verbose mode
 */

int adc3117_set_verbose(int mode)
{
  adc3117_verbose_mode = mode;
  return(adc3117_verbose_mode);
}


/*!
 * \brief   perform a reset of the ADC3117 FMC 
 *
 * \param   fmc         fmc slot number (1 or 2)
 *
 * \return  1 on success, -1 on error
 */

int adc3117_reset(int fd, int fmc)
{
  int sign, ret, to, val;

  ret = fmc_init(fd);
  if (ret < 0) 
  {
    return(ret);
  }
    
  sign = ADC3117_SIGN_ID;
    
  ret = fmc_identify(fd, fmc, &sign, NULL, NULL);
  if (ret < 0)
  {
    return(ret);
  }
  
  /* Re-initialize XRA sequencer */
  ret = fmc_csr_write(fd, fmc, ADC_CSR_CTL, 0x2);
  if (ret < 0)
  {
    return(ret);
  }

  /* Wait for initialization to complete */
  to = 10000;
  while(to > 0)
  {
    ret = fmc_csr_read(fd, fmc, ADC3117_CSR_CTL, &val);
    if (ret < 0)
    {
      return(ret);
    }
    /* XRA_SEQ_RDY=1 & ADC_CFG_BUSY=0 */
    if ((val & ((1<<2)|(1<<3))) == (1<<3)) 
    {
      break;
    }
    usleep(2000);
    to--;
  }

  /* Initialize all DAC offset */
  ret = adc3117_init_dacoff(fd, fmc);

  if (ret >=0)
  {
    /* set VCAL to VREF_4V */
    ret = adc3117_set_vcal(fd, fmc, ADC3117_VCAL_VREF_4V);
  }

  if (ret >= 0)
  {
    /* initialize all ADC channels */
    ret = adc3117_set_ch_cfg(fd, fmc, 0xfffff, ADC3117_SW_GND, ADC3117_SW_GND, ADC3117_GAIN_x1, 0);
  }

  if (ret >= 0)
  {
    ret = adc3117_update_cfg(fd, fmc);
  }

  return(ret);
}

/*----------------------------------------------------------------------------------------------------------------------*/

/*!
 * \brief   Set VCAL mux
 *
 * \param   fmc         fmc slot number (1 or 2)
 * \param   vcal_sel    VCAL selection (VREF_VAR, VREF_4V, DACOUT_0, DACOUT_1)
 *
 * \return  1 on success, -1 on error
 */

int adc3117_set_vcal(int fd, int fmc, int vcal_sel)
{
  int ret, data;
  
  data = (vcal_sel & 3);
  
  ret = fmc_csr_write(fd, fmc, ADC3117_CSR_ADC_CFG, data);
  
  return(ret);
}

/*----------------------------------------------------------------------------------------------------------------------*/

/*!
 * \brief   Set ADC channel configuration
 *
 * \param   fmc         fmc slot number (1 or 2)
 * \param   adc_ch_mask ADC channel mask (1 bit per channel)
 * \param   sw_n        SW_N mux selection (INPUT, GND, OFFSET or VCAL)
 * \param   sw_p        SW_P mux selection (INPUT, GND, OFFSET or VCAL)
 * \param   gain        gain selection (x1, x2, x5, x10)
 * \param   offset      16-bit digital offset value (signed, twos complement)
 *
 * \return  1 on success, -1 on error
 */

int adc3117_set_ch_cfg(int fd, int fmc, int adc_ch_mask, int sw_n, int sw_p, int gain, int offset)
{
  int data, ret;

  data = ((adc_ch_mask & 0xfffff)<<12);

  ret = fmc_csr_write(fd, fmc, ADC3117_CSR_ADC_CH_SEL, data);
  if (ret<0)
  {
    return(ret);
  }
  
  data = (((offset&0xffff)<<16) | ((gain&3)<<4) | ((sw_p&3)<<2) | (sw_n&3));

  ret = fmc_csr_write(fd, fmc, ADC3117_CSR_ADC_CH_CFG, data);
  
  return(ret);
}

/*----------------------------------------------------------------------------------------------------------------------*/

/*!
 * \brief   Update ADC configuration (i.e. transfer the configuration 
 *          to I/O expanders)
 *
 * \param   fmc       fmc slot number (1 or 2)
 *
 * \return  1 on success, -1 on error
 */

int adc3117_update_cfg(int fd, int fmc)
{
  int data, ret = -1, timeout = 100;

  /* Update configuration */
  ret = fmc_csr_write(fd, fmc, ADC3117_CSR_CTL, 0x1);
  if (ret < 0)
  {
    return(ret);
  }
  
  /* Wait for adc configuration to be updated  */
  while (timeout > 0) {
    ret = fmc_csr_read(fd, fmc, ADC3117_CSR_CTL, &data);
    if (ret < 0)
    {
      return(ret);
    }
    /* Update completed (ADC_CFG_BUSY=0) ? */
    if ((data & (1<<2))==0) 
    {
      ret = 0;
      break;
    }
    usleep(2000);
    timeout --;
  }
  return(ret);
}

/*----------------------------------------------------------------------------------------------------------------------*/

/*!
 * \brief   Write a value into a given register of a given serial device
 *
 * \param   fmc       fmc slot number (1 or 2)
 * \param   sel       device selection
 * \param   add       device register address
 * \param   data      data value to write
 *
 * \return  1 on success, -1 on error
 */

int adc3117_serial_write(int fd, int fmc, int sel, int add, int data)
{
  int cmd, ret;
  
  cmd = ((sel & 0xf) << 24);
  
  ret = adc_spi_write(fd, fmc, cmd, add, data);
  
  return(ret);
}

/*----------------------------------------------------------------------------------------------------------------------*/

/*!
 * \brief   Read a value from a given register of a given serial device
 *
 * \param   fmc       fmc slot number (1 or 2)
 * \param   sel       device selection
 * \param   add       device register address
 * \param   data      pointer to data value read
 */

int adc3117_serial_read(int fd, int fmc, int sel, int add, int *data)
{
  int cmd, ret;
  
  cmd = ((sel & 0xf) << 24);
  
  ret = adc_spi_read(fd, fmc, cmd, add, data);
  
  return(ret);
}

/*----------------------------------------------------------------------------------------------------------------------*/

/*!
 * \brief   Initialize DAC offset (i.e. enable internal reference)
 *
 * \param   fmc       fmc slot number (1 or 2)
 *
 * \return  1 on success, -1 on error
 */

/* initialize all DAC offset */
int adc3117_init_dacoff(int fd, int fmc)
{
  int i, ret;

  /* Enable ext_VREF */
  for (i=0; i<10; i++) 
  {
    /* Enable internal reference and reset DACs to gain = 2 */
    ret = adc3117_serial_write(fd, fmc, (ADC3117_SER_DAC_OFF_0+i), 0x3f, 0x01);
    if (ret < 0)
    {
      return(ret);
    }
  }
  return(0);
}

/*----------------------------------------------------------------------------------------------------------------------*/

/*!
 * \brief   Set DAC offset value for a given ADC channel
 *
 * \param   fmc       fmc slot number (1 or 2)
 * \param   adc_ch    ADC channel
 * \param   val       DAC offset value
 *
 * \return  1 on success, -1 on error
 */

int adc3117_set_dacoff(int fd, int fmc, int adc_ch, int val)
{
  int add, tmp, ret;

  tmp = (adc_ch % 4);
  add = ((tmp == 0 || tmp == 2) ? 0x18 : 0x19);

  tmp = (val & 0xffff);
    
  ret = adc_spi_write(fd, fmc, ADC3117_SER_DAC_OFF_0+((adc_ch & 0xf)/2), add, tmp);
  
  return(ret);
}

/*----------------------------------------------------------------------------------------------------------------------*/

/*!
 * \brief   Set DAC output value
 *
 * \param   fmc       fmc slot number (1 or 2)
 * \param   dac_ch    DACOUT channel (0 to 1)
 * \param   pot       potentiometer value from 0 to 255 or -1
 * \param   dac_code  DAC 16-bit code from 0 to 65535 or -1
 *
 * \return  1 on success, -1 on error
 */

int adc3117_set_dac_output(int fd, int fmc, int dac_ch, int pot, int dac_code)
{
  int ret, pot_sel, dac_reg;
  
  pot_sel = ((dac_ch & 1) ? ADC3117_SER_POT_DAC_OUT_1 : ADC3117_SER_POT_DAC_OUT_0);
  dac_reg = ((dac_ch & 1) ? 0x19 : 0x18); 
  
  if (dac_code != -1)
  {
    ret = adc3117_serial_write(fd, fmc, ADC3117_SER_DAC_OUT, dac_reg, (dac_code & 0xffff));
  }
  if (pot != -1 && ret >= 0)
  {
    ret = adc3117_serial_write(fd, fmc, pot_sel, 0, (pot&0xff));
  }
  return ret;
}

/*----------------------------------------------------------------------------------------------------------------------*/

/*!
 * \brief   Set VREF_VAR digital potentiometer value
 *
 * \param   fmc       fmc slot number (1 or 2)
 * \param   pot       potentiometer value from 0 to 255
 *
 * \return  1 on success, -1 on error
 */

int adc3117_set_vref_var(int fd, int fmc, int pot)
{
  return adc3117_serial_write(fd, fmc, ADC3117_SER_POT_VREF_VAR, 0, (pot&0xff));
}

/*----------------------------------------------------------------------------------------------------------------------*/

/*!
 * \brief   Change the sample rate for all ADC channels
 *
 * \param   fmc       fmc slot number (1 or 2)
 * \param   rate      rate value (0 or 20 to 16777215)
 *
 * \return  1 on success, -1 on error
 * 
 * \note    This function is only supported since ADC_3117_FDK revision 3 !
 */

int adc3117_set_rate(int fd, int fmc, int rate)
{
  int ret;
  uint data;

  data = (rate & 0xffffff);
  
  ret = fmc_csr_write(fd, fmc, ADC3117_CSR_ADC_RATE, data);
  
  return(ret);
}

/*----------------------------------------------------------------------------------------------------------------------*/

/*!
 * \brief   Calculate sum of 8-bit unsigned integer 
 *
 * \param   buf       pointer to a buffer of 8-bit unsigned integer
 * \param   len       length of the buffer in number of 8-bit unsigned integers
 *
 * \return  calculated checksum
 */

static unsigned char adc3117_calc_chksum(unsigned char *buf, int len)
{
  int i;
  unsigned char cks = 0;
  unsigned char ch;

  for (i=0; i<len; i++)
  {
    ch = buf[i];
    cks += ch;
  }
  return cks;
}

/*----------------------------------------------------------------------------------------------------------------------*/

/*!
 * \brief   Display VREF calibration data 
 *
 * \param   h         pointer to VREF calibration data
 *
 * \return  1 on success, -1 on error
 */

int adc3117_vref_show(adc3117_vref_t *h)
{
  int i;
  
  if (h->magic != ADC3117_VREF_MAGIC)
  {
    adc3117_error("No valid calibration data available!!\n");
    return (-1);
  }
  printf("VREF calibration parameters\n");
  printf("Temperature = %f\n", h->temp);
  printf("+-----+---------+\n");
  printf("| POT | VOLT    |\n");
  printf("+-----+---------+\n");
  
  for (i=0; i < 32; i++)
  {
    printf("| %03d | %6.5f |\n", h->vref[i].digpot, h->vref[i].volt);
  }
  printf("+-----+---------+\n\n");
  
  return(1);
}

/*----------------------------------------------------------------------------------------------------------------------*/

/*!
 * \brief   Store VREF calibration data into I2C EEPROM
 *
 * \param   fmc       fmc slot number (1 or 2)
 * \param   dev       I2C device
 * \param   h         pointer to VREF calibration data
 *
 * \return  1 on success, -1 on error
 */

int adc3117_vref_store(int fd, int fmc, uint dev, adc3117_vref_t *h)
{
  int ret;
  if (h->magic != ADC3117_VREF_MAGIC)
  {
    adc3117_error("Need valid calibration data !!\n");
    return(-1);
  }
  ret = fmc_i2c_eeprom_write(fd, fmc, dev, ADC3117_VREF_OFF, (unsigned char *)h, sizeof(adc3117_vref_t));
  if (ret < 0)
  {
    adc3117_error("Error writing calibration data into eeprom !\n");
    return(ret);
  }
  adc3117_info("Done\n");
  return(1);
}

/*----------------------------------------------------------------------------------------------------------------------*/

/*!
 * \brief   Load VREF calibration data from I2C EEPROM
 *
 * \param   fmc       fmc slot number (1 or 2)
 * \param   dev       I2C device
 * \param   h         pointer to VREF calibration data
 *
 * \return  1 on success, -1 on error
 */

int adc3117_vref_load(int fd, int fmc, uint dev, adc3117_vref_t *h)
{
  int ret;
  unsigned char calc_cks;
  
  ret = fmc_i2c_eeprom_read(fd, fmc, dev, ADC3117_VREF_OFF, (unsigned char *)h, sizeof(adc3117_vref_t));
  if (ret < 0)
  {
    adc3117_error("Error reading eeprom !");
    return(ret);
  }
  
  if (h->magic != ADC3117_VREF_MAGIC)
  {
    adc3117_error("No valid signature found !\n");
    return(-1);
  }
  
  if (h->len != sizeof(adc3117_vref_t))
  {
    adc3117_error("Invalid length (should be %d but is %d)\n", (int)sizeof(adc3117_vref_t), h->len);
    return(-1);
  }
  /* verify checksum only if calculated (in early version of TscMon, cks was set to zero */
  if (h->cks != 0)
  {
    calc_cks = adc3117_calc_chksum((unsigned char *)h, sizeof(adc3117_vref_t));
    if (calc_cks != 0xFF)
    {
      adc3117_error("Invalid checksum ! (%08X)\n", h->cks);
      return(-1);
    }
  }
  adc3117_info("Done\n");
  return(1);
}

/*----------------------------------------------------------------------------------------------------------------------*/

/*!
 * \brief   Read VREF calibration data from a text file
 *
 * \param   filename  name of the input file
 * \param   h         pointer to VREF calibration data
 *
 * \return  1 on success, -1 on error
 */

int adc3117_vref_read(char *filename, adc3117_vref_t *h)
{
  FILE *fin;
  int i;
  unsigned char calc_cks;
  
  if (filename == NULL || h == NULL)
  {
    return(-1);
  }
  
  fin = fopen(filename, "r");
  if (fin == NULL)
  {
    adc3117_error("Cannot open file %s\n", filename);
    return(-1);
  }
  adc3117_info("Reading VREF calibration from file %s\n", filename);

  fgets(adc3117_line, sizeof(adc3117_line), fin);
  if(sscanf(adc3117_line, "%f", &h->temp) != 1)
  {
    adc3117_error("formatting error!!\n");
    fclose(fin);
    return(-1);
  }
  i = 0;
  while((i<32) && fgets(adc3117_line, sizeof(adc3117_line), fin))
  {
    if( sscanf(adc3117_line, "%d, %f", &h->vref[i].digpot, &h->vref[i].volt) != 2)
    {
      adc3117_error("formatting error!!\n");
      fclose(fin);
      return(-1);
    }
    i++;
  }
  h->magic = ADC3117_VREF_MAGIC;
  h->len = sizeof(adc3117_vref_t);
  h->cks = 0;
  calc_cks = adc3117_calc_chksum((unsigned char *)h, sizeof(adc3117_vref_t));
  h->cks = (((int)(~calc_cks))&0xff);
  fclose(fin);
  return(1);
}

/*----------------------------------------------------------------------------------------------------------------------*/

/*!
 * \brief   Write VREF calibration data into a text file
 *
 * \param   filename  name of the output file
 * \param   h         pointer to VREF calibration data
 *
 * \return  1 on success, -1 on error
 */

int adc3117_vref_write(char *filename, adc3117_vref_t *h)
{
  int i;
  FILE *fout;
  
  fout = fopen(filename, "w");
  if(fout == NULL)
  {
    adc3117_error("Cannot open file %s\n", filename);
    return(-1);
  }
  adc3117_info("Writing VREF calibration to file %s...", filename);
  
  if (h->magic != ADC3117_VREF_MAGIC)
  {
    adc3117_error("No valid calibration data available!!\n");
    fclose(fout);
    return(-1);
  }
  fprintf(fout, "%f\n", h->temp);
  for( i = 0; i < 32; i++)
  {
    fprintf(fout, "%d, %f\n", h->vref[i].digpot, h->vref[i].volt);
  }
  fclose(fout);
  adc3117_info("Done\n");
  return(1);
}

/*----------------------------------------------------------------------------------------------------------------------*/

/*!
 * \brief   Display ADC/DAC correction gain & offset for all ADC channels
 *
 * \param   h         pointer to ADC/DAC correction data
 *
 * \return  1 on success, -1 on error
 */

int adc3117_corr_show(adc3117_corr_t *h)
{ 
  int i;
  unsigned char calc_cks;
  
  if(h->magic != ADC3117_CORR_MAGIC)
  {
    adc3117_error("No valid ADC/DAC gain/offset correction data available!!\n");
    return(-1);
  }
  calc_cks = adc3117_calc_chksum((unsigned char *)h, sizeof(adc3117_corr_t));
  if (calc_cks != 0xFF)
  {
    adc3117_error("Invalid checksum !\n");
    return(-1);
  }
  printf("ADC/DAC gain/offset correction parameters\n\n");
  printf("+-----+----------+----------+\n");
  printf("| ADC | OFFSET  | GAIN    |\n");
  printf("+-----+----------+----------+\n");
  for (i=0; i<20; i++)
  {
    printf("|  %2d | % 6.5f | % 6.5f |\n", i, h->adc[i].offset, h->adc[i].gain);
  }
  printf("+-----+----------+----------+\n\n");

  printf("+-----+----------+----------+\n");
  printf("| DAC | OFFSET  | GAIN    |\n");
  printf("+-----+----------+----------+\n");
  for (i=0; i<2; i++)
  {
    printf("|  %2d | % 6.5f | % 6.5f |\n", i, h->dac[i].offset, h->dac[i].gain);
  }
  printf("+-----+----------+----------+\n\n");
  
  return (1);
}

/*----------------------------------------------------------------------------------------------------------------------*/

/*!
 * \brief   Set to default value ADC/DAC correction gain & offset 
 *          for all ADC channels
 *
 * \param   h         pointer to ADC/DAC correction data
 *
 * \return  1 on success, -1 on error
 */

void adc3117_corr_default(adc3117_corr_t *h)
{
  int i;
  unsigned char calc_cks;
  
  if (h == NULL)
  {
    return;
  }
  
  adc3117_info("Settings default value (offset = 0.0 and gain = 1.0) to all ADC/DAC gain/offset correction parameters...\n");
  
  for (i=0; i<20; i++) 
  {
    h->adc[i].offset = 0.0;
    h->adc[i].gain   = 1.0;
  }
  
  for (i=0; i<2; i++) 
  {
    h->dac[i].offset = 0.0;
    h->dac[i].gain   = 1.0;
  }
  h->magic = ADC3117_CORR_MAGIC;
  h->len = sizeof(adc3117_corr_t);
  h->cks = 0;
  calc_cks = adc3117_calc_chksum((unsigned char *)h, sizeof(adc3117_corr_t));
  h->cks = (((int)(~calc_cks))&0xff);
}

/*----------------------------------------------------------------------------------------------------------------------*/

/*!
 * \brief   Store ADC/DAC correction data into I2C EEPROM
 *
 * \param   fmc       fmc slot number (1 or 2)
 * \param   dev       I2C device
 * \param   h         pointer to ADC/DAC correction data
 *
 * \return  1 on success, -1 on error
 */

int adc3117_corr_store(int fd, int fmc, uint dev, adc3117_corr_t *h)
{
  int ret;

  if (h == NULL)
  {
    return(-1);
  }
  
  adc3117_info("Storing ADC/DAC gain/offset correction into EEPROM...");

  if(h->magic != ADC3117_CORR_MAGIC)
  {
    adc3117_error("Need valid ADC/DAC gain/offset parameters !!\n");
  }
  ret = fmc_i2c_eeprom_write(fd, fmc, dev, ADC3117_CORR_OFF, (unsigned char *)h, sizeof(adc3117_corr_t));
  if (ret < 0)
  {
    adc3117_error("error writing into eeprom !\n");
    return(ret);
  }
  
  adc3117_info("Done\n");
  
  return(1);
}

/*----------------------------------------------------------------------------------------------------------------------*/

/*!
 * \brief   Load a ADC/DAC gain/offset parameters from I2C EEPROM
 *
 * \param   fmc       fmc slot number (1 or 2)
 * \param   dev       I2C device
 * \param   h         pointer to ADC/DAC correction data
 *
 * \return  1 on success, -1 on error
 */

int adc3117_corr_load(int fd, int fmc, uint dev, adc3117_corr_t *h)
{
  int ret;
  unsigned char calc_cks;

  adc3117_info("Loading ADC/DAC gain/offset paramters from EEPROM...");
  
  ret = fmc_i2c_eeprom_read(fd, fmc, dev, ADC3117_CORR_OFF, (unsigned char *)h, sizeof(adc3117_corr_t));
  if (ret < 0)
  {
    adc3117_error("error reading eeprom!\n");
    return(ret);
  }
  
  if (h->magic != ADC3117_CORR_MAGIC)
  {
    adc3117_error("No valid signature found !\n");
    return(-1);
  }
  
  if (h->len != sizeof(adc3117_corr_t))
  {
    adc3117_error("Invalid length (should be %d but is %d)\n", (int)sizeof(adc3117_corr_t), h->len);
    return(-1);
  }
  
  calc_cks = adc3117_calc_chksum((unsigned char *)h, sizeof(adc3117_corr_t));
  
  if (calc_cks != 0xFF)
  {
    adc3117_error("Invalid checksum !\n");
    return(-1);
  }
  
  adc3117_info("Done\n");
  
  return(1);
}

/*----------------------------------------------------------------------------------------------------------------------*/

/*!
 * \brief   Read ADC/DAC correction from a text file
 *
 * \param   filename  name of the input file
 * \param   h         pointer to ADC/DAC correction data
 *
 * \return  1 on success, -1 on error
 */

int adc3117_corr_read(char *filename, adc3117_corr_t *h)
{
  int i;
  unsigned char calc_cks;
  FILE * fin;
  
  fin = fopen(filename, "r");
  if(fin == NULL)
  {
    adc3117_error("Cannot open file %s\n", filename);
    return(-1);
  }
  adc3117_info("Reading ADC/DAC offset/gain parameters from file %s\n", filename);

  i=0;
  while ((i<20) && fgets(adc3117_line, sizeof(adc3117_line), fin) != NULL)
  {
    /*printf("a:%d: %s", i, line);*/
    if (sscanf(adc3117_line, "%f, %f", &h->adc[i].offset, &h->adc[i].gain) != 2)
    {
      adc3117_error("formatting error\n");
      fclose(fin);
      return(-1);
    }
    i++;
  }
  i=0;
  while ((i<2) && fgets(adc3117_line, sizeof(adc3117_line), fin) != NULL)
  {
    /*printf("b:%d: %s", i, line);*/
    if (sscanf(adc3117_line, "%f, %f", &h->dac[i].offset, &h->dac[i].gain) != 2)
    {
      adc3117_error("formatting error\n");
      fclose(fin);
      return(-1);
    }
    i++;
  }
  h->magic = ADC3117_CORR_MAGIC;
  h->len = sizeof( adc3117_corr_t);
  h->cks = 0;
  calc_cks = adc3117_calc_chksum((unsigned char *)h, sizeof(adc3117_corr_t));
  h->cks = (((int)(~calc_cks))&0xff);
  fclose(fin);
  return(1);
}

/*----------------------------------------------------------------------------------------------------------------------*/

/*!
 * \brief   Write ADC/DAC correction into a text file
 *
 * \param   filename  name of the output file
 * \param   h         pointer to ADC/DAC correction data
 *
 * \return  1 on success, -1 on error
 */

int adc3117_corr_write(char *filename, adc3117_corr_t *h)
{
  int i;
  FILE *fout;
  
  fout = fopen(filename, "w");
  if (fout == NULL)
  {
    adc3117_error("Cannot open file %s\n", filename);
    return(-1);
  }
  adc3117_info("Writing ADC/DAC offset/gain parameters to file %s...", filename);
  
  if (h->magic != ADC3117_CORR_MAGIC)
  {
    adc3117_error("No valid ADC/DAC offset/gain parameters available!!\n");
    fclose(fout);
    return(-1);
  }
  
  for(i=0; i<20; i++)
  {
    fprintf(fout, "%f, %f\n", h->adc[i].offset, h->adc[i].gain);
  }
  for(i=0; i<2; i++)
  {
    fprintf(fout, "%f, %f\n", h->dac[i].offset, h->dac[i].gain);
  }
  fclose(fout);
  adc3117_info("Done\n");
  return(1);
}

/*----------------------------------------------------------------------------------------------------------------------*/
