/*=========================< begin file & file header >=======================
 *  References
 *  
 *    filename : fscope3110lib.h
 *    author   : JFG, XP
 *    company  : IOxOS
 *    creation : october 18,2018
 *
 *----------------------------------------------------------------------------
 *  Description
 *
 *    That library contains a set of function to access the ADC3110 FMC
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

#ifndef _H_ADC3110LIB
#define _H_ADC3110LIB

#define IFC1211

#define ADC3110_FMC1    1
#define ADC3110_FMC2    2
#define ADC3110_FMC_NUM     2
#define ADC3110_CHAN_NUM    8
#define ADC3110_CHAN_SET_ALL    0xff
#define ADC3110_CHAN_SET_0      0x01
#define ADC3110_CHAN_SET_1      0x02
#define ADC3110_CHAN_SET_2      0x04
#define ADC3110_CHAN_SET_3      0x08
#define ADC3110_CHAN_SET_4      0x10
#define ADC3110_CHAN_SET_5      0x20
#define ADC3110_CHAN_SET_6      0x40
#define ADC3110_CHAN_SET_7      0x80

#define ADC3110_CSR_BASE   0x00001000
#define ADC3110_CSR_OFF_FMC2    0x100
#define ADC3110_CSR_SIGN         0x80
#define ADC3110_CSR_CTL          0x81
#define ADC3110_CSR_LED          0x82
#define ADC3110_CSR_SERIAL       0x83
#define ADC3110_CSR_GPIO         0x85
#define ADC3110_CSR_DISC         0x86
#define ADC3110_CSR_FMC          0x87
#define ADC3110_CSR_OFF_01       0x88
#define ADC3110_CSR_OFF_23       0x89
#define ADC3110_CSR_OFF_45       0x8a
#define ADC3110_CSR_OFF_67       0x8b
#define ADC3110_CSR_IDELAY       0x8e
#define ADC3110_CSR_ADDR( csr)   (ADC3110_CSR_BASE | (csr<<2))

#define ADC3110_SIGN_ID    0x31100000
#define ADC3110_SIGN_MASK  0xffff0000

#define ADC3110_SPI_ADS01     0x01000000
#define ADC3110_SPI_ADS23     0x01010000
#define ADC3110_SPI_ADS45     0x01020000
#define ADC3110_SPI_ADS67     0x01030000
#define ADC3110_SPI_LMK       0x02000000
#define ADC3110_I2C_EEPROM    0x01010051
#define ADC3110_I2C_THERMO    0x01040048


#define adc3110_spi_ads01_read(fd, fmc, reg, data)        adc_spi_read(fd, fmc, ADC3110_SPI_ADS01, reg, data)
#define adc3110_spi_ads01_write(fd, fmc, reg, data)       adc_spi_write(fd, fmc, ADC3110_SPI_ADS01, reg, data)

#define adc3110_spi_ads23_read(fd, fmc, reg, data)        adc_spi_read(fd, fmc, ADC3110_SPI_ADS23, reg, data)
#define adc3110_spi_ads23_write(fd, fmc, reg, data)       adc_spi_write(fd, fmc, ADC3110_SPI_ADS23, reg, data)

#define adc3110_spi_ads45_read(fd, fmc, reg, data)        adc_spi_read(fd, fmc, ADC3110_SPI_ADS45, reg, data)
#define adc3110_spi_ads45_write(fd, fmc, reg, data)       adc_spi_write(fd, fmc, ADC3110_SPI_ADS45, reg, data)

#define adc3110_spi_ads67_read(fd, fmc, reg, data)        adc_spi_read(fd, fmc, ADC3110_SPI_ADS67, reg, data)
#define adc3110_spi_ads67_write(fd, fmc, reg, data)       adc_spi_write(fd, fmc, ADC3110_SPI_ADS67, reg, data)

#define adc3110_spi_lmk_read(fd, fmc, reg, data)          adc_spi_read(fd, fmc, ADC3110_SPI_LMK, reg, data)
#define adc3110_spi_lmk_write(fd, fmc, reg, data)         adc_spi_write(fd, fmc, ADC3110_SPI_LMK, reg, data)

#define adc3110_i2c_eeprom_read(fd, fmc, reg)            fmc_i2c_rd(fd, fmc, ADC3110_I2C_EEPROM, reg, data)
#define adc3110_i2c_eeprom_write(fd, fmc, reg, data)     fmc_i2c_wr(fd, fmc, ADC3110_I2C_EEPROM, reg, data)

#define adc3110_i2c_thermo_read(fd, fmc, reg)            adc_i2c_read(fd, fmc, ADC3110_I2C_THERMO, reg)
#define adc3110_i2c_thermo_write(fd, fmc, reg, data)     adc_i2c_write(fd, fmc, ADC3110_I2C_THERMO, reg, data)

#define ADC3110_ADS_MODE_NORM      0x000
#define ADC3110_ADS_MODE_ZERO      0x001
#define ADC3110_ADS_MODE_ONE       0x002
#define ADC3110_ADS_MODE_TOGGLE    0x003
#define ADC3110_ADS_MODE_RAMP      0x004
#define ADC3110_ADS_MODE_TST1      0x006
#define ADC3110_ADS_MODE_TST2      0x007
#define ADC3110_ADS_MODE_RAND      0x00a
#define ADC3110_ADS_MODE_SINE      0x00b

#define adc3110_csr_rd(fd, fmc, csr, data)              fmc_csr_rd(fd, fmc, csr, data)
#define adc3110_csr_wr(fd, fmc, csr, data)              fmc_csr_rd(fd, fmc, csr, data)
#define adc3110_identify(fd, fmc)                       fmc_identify(fd, fmc, NULL, NULL, NULL)
#define adc3110_spi_read(fd, fmc, cmd, reg, data)       fmc_spi_read(fd, fmc, cmd, reg, data)
#define adc3110_spi_write(fd, fmc, cmd, reg, data)      fmc_spi_write(fd, fmc, cmd, reg, data)
#define adc3110_i2c_read(fd, fmc, dev, reg, data)       fmc_i2c_read(fd, fmc, cmd, reg, data)
#define adc3110_i2c_write(fd, fmc, dev, reg, data)      fmc_i2c_write(fd, fmc, cmd, reg, data)

int adc3110_set_verbose(int vf);
int adc3110_reset(int fd, int fmc);
void adc3110_lmk_init(int fd, int fmc, int reg[]);
void adc3110_lmk_dump(int fd, int fmc);
void adc3110_ads42lb69_init(int fd, int fmc, int chan_set); 
void adc3110_ads42lb69_dump(int fd, int fmc); 
int adc3110_ads42lb69_set_mode(int fd, int fmc, int chan, int mode); 
int adc3110_ads42lb69_set_pattern(int fd, int fmc, int chan, int pattern); 
int adc3110_calib_set_idelay(int fd, int fmc, int chan, int idelay);
int adc3110_calib_get_idelay(int fd, int fmc, int chan);
int adc3110_gpio_trig(int fd, int fmc);

#endif /* _H_ADC3110LIB */

