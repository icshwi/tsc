/*=========================< begin file & file header >=======================
 *  References
 *
 *    filename : daq1430lib.h
 *    author   : CG, JFG, XP
 *    company  : IOxOS
 *    creation : september 23,2020
 *
 *----------------------------------------------------------------------------
 *  Description
 *
 *    That library contains a set of function to access the DAQ1430 FMC
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

#ifndef _H_DAQ1430LIB
#define _H_DAQ1430LIB

/* ------------------------------------------------------------------------------------------------------------------ */
/*  DAQ_1430 Definition                                                                                               */
/* ------------------------------------------------------------------------------------------------------------------ */

#define DAQ1430_FMC1            1         /* FMC #1             */
#define DAQ1430_FMC_NUM         1         /* Number of FMC      */
#define DAQ1430_CHAN_NUM        10        /* Number of channels */
#define DAQ1430_CHAN_SET_ALL    0x3ff     /* All channel mask   */
#define DAQ1430_CHAN_SET_0      (1<<0)    /* Channel #0         */
#define DAQ1430_CHAN_SET_1      (1<<1)    /* Channel #1         */
#define DAQ1430_CHAN_SET_2      (1<<2)    /* Channel #2         */
#define DAQ1430_CHAN_SET_3      (1<<3)    /* Channel #3         */
#define DAQ1430_CHAN_SET_4      (1<<4)    /* Channel #4         */
#define DAQ1430_CHAN_SET_5      (1<<5)    /* Channel #5         */
#define DAQ1430_CHAN_SET_6      (1<<6)    /* Channel #6         */
#define DAQ1430_CHAN_SET_7      (1<<7)    /* Channel #7         */
#define DAQ1430_CHAN_SET_8      (1<<8)    /* Channel #8         */
#define DAQ1430_CHAN_SET_9      (1<<9)    /* Channel #9         */
#define DAQ1430_CHAN_GROUP      1         /* 1 channel per gscope data pipeline    */

/* ------------------------------------------------------------------------------------------------------------------ */
/*  DAQ_1430 Register Definition                                                                                      */
/* ------------------------------------------------------------------------------------------------------------------ */

#define DAQ1430_CSR_BASE          ADC_CSR_BASE      /* FMC Base address                   */
#define DAQ1430_CSR_SIGN          ADC_CSR_SIGN      /* Signature                          */
#define DAQ1430_CSR_CTL           ADC_CSR_CTL       /* Main Control & Status              */
#define DAQ1430_CSR_LED           ADC_CSR_LED       /* Front-Panel LED Management         */
#define DAQ1430_CSR_SERIAL        ADC_CSR_SERIAL    /* Serial Interface                   */
#define DAQ1430_CSR_GPIO          ADC_CSR_GPIO      /* Front-Panel GPIO Management        */
#define DAQ1430_CSR_DISC          ADC_CSR_DISC      /* Discriminator Function             */
#define DAQ1430_CSR_FMC           ADC_CSR_FMC       /* IFC Carrier Support                */
#define DAQ1430_CSR_OFF_01        0x08              /* ADC #0 & #1 offset correction      */
#define DAQ1430_CSR_OFF_23        0x09              /* ADC #2 & #3 offset correction      */
#define DAQ1430_CSR_OFF_45        0x0A              /* ADC #4 & #5 offset correction      */
#define DAQ1430_CSR_OFF_67        0x0B              /* ADC #6 & #7 offset correction      */
#define DAQ1430_CSR_OFF_89        0x0C              /* ADC #8 & #9 offset correction      */
#define DAQ1430_CSR_IDELAY        0x0E              /* IDELAY/ISERDES Calibration Support */
#define DAQ1430_CSR_ADCDIR        0x0F              /* ADCDIR data                        */
#define DAQ1430_CSR_DAC_CTL       0x10              /* DAC38J84 Control & Status          */
#define DAQ1430_CSR_DAC_DAT       0x11              /* DAC38J84 Data                      */

/* Build an address from a register index */
#define DAQ1430_CSR_ADDR(csr)     (DAQ1430_CSR_BASE | ((csr&0xff)<<2))

/* Signature */
#define DAQ1430_SIGN_ID                   0x14300000

/* MAIN_CSR */
#define DAQ1430_CTL_ADC_RESET             (1<< 8) /* Reset ADC & DAC                */
#define DAQ1430_CTL_ADC_SYNC              (1<< 9)
#define DAQ1430_CTL_ADC_01_PWRD           (1<<10)
#define DAQ1430_CTL_ADC_23_PWRD           (1<<11)
#define DAQ1430_CTL_ADC_4567_PWRD         (1<<12)
#define DAQ1430_CTL_ADC_89_PWRD           (1<<13)
#define DAQ1430_CTL_MMCM_RESET            (1<<14) /* FPGA MMCM/PLL RESET            */
#define DAQ1430_CTL_MMCM_LOCKED           (1<<15) /* FPGA MMCM LOCKED               */
#define DAQ1430_CTL_PLLE3_LOCKED          (1<<16) /* FPGA PLL LOCKED                */
#define DAQ1430_CTL_TMP102_ALERTn         (1<<17) /* TMP102 ALERTn status signal    */
#define DAQ1430_CTL_ACQ_DPRAM_BUF         (1<<25) /* DPRAM Buffer Present           */
#define DAQ1430_CTL_ACQ_DPRAM_BUFSIZ_2K   (0<<26) /* 2K samples /  4K bytes         */
#define DAQ1430_CTL_ACQ_DPRAM_BUFSIZ_4K   (1<<26) /* 4K samples /  8K bytes         */
#define DAQ1430_CTL_ACQ_DPRAM_BUFSIZ_8K   (2<<26) /* 8K samples / 16K bytes         */
#define DAQ1430_CTL_ACQ_FIFO_BUF          (1<<28) /* FIFO Buffer Present            */
#define DAQ1430_CTL_ACQ_DIR_BUF           (1<<29) /* Direct Mode Present            */
#define DAQ1430_CTL_ACQ_DISCRI            (1<<30) /* Discriminator Function Present */
#define DAQ1430_CTL_ACQ_OFF_COMP          (1<<31) /* Offset Compensation Present    */


/* LED */
#define DAQ1430_LED_OSC_EN                (1<<31) /* Enable Oscillator              */
/* SERIAL_CTL */
#define DAQ1430_SER_LMK_STATUS_LD         (1<<20) /* LMK Status_LD signal           */
#define DAQ1430_SER_LMK_STATUS_CLKIN0     (1<<22) /* LMK Status_CLKIN0 signal       */
#define DAQ1430_SER_LMK_STATUS_CLKIN1     (1<<23) /* LMK Status_CLKIN1 signal       */

#define DAQ1430_SER_SEL_ADS01             (0<<16) /* ADS42LB69 device #01           */
#define DAQ1430_SER_SEL_ADS23             (1<<16) /* ADS42LB69 device #23           */
#define DAQ1430_SER_SEL_ADS45             (2<<16) /* ADS42LB69 device #45           */
#define DAQ1430_SER_SEL_ADS67             (3<<16) /* ADS42LB69 device #67           */
#define DAQ1430_SER_SEL_ADS89             (4<<16) /* ADS42LB69 device #89           */
#define DAQ1430_SER_SEL_DAC38J84          (7<<16) /* DAC38J84                       */

#define DAQ1430_SER_SEL_PLLE3_ADS01       (0<<16) /* PLLE3 for ADS42LB69 device #01 */
#define DAQ1430_SER_SEL_PLLE3_ADS23       (1<<16) /* PLLE3 for ADS42LB69 device #23 */
#define DAQ1430_SER_SEL_PLLE3_ADS45       (2<<16) /* PLLE3 for ADS42LB69 device #45 */
#define DAQ1430_SER_SEL_PLLE3_ADS67       (3<<16) /* PLLE3 for ADS42LB69 device #67 */
#define DAQ1430_SER_SEL_PLLE3_ADS89       (4<<16) /* PLLE3 for ADS42LB69 device #89 */
#define DAQ1430_SER_SEL_MMCME3_ADS        (5<<16) /* MMCME3 for ADS                 */
#define DAQ1430_SER_SEL_MMCME3_DOC        (7<<16) /* MMCME3 for DAC38J84            */

#define DAQ1430_SER_DEV_ADS               (1<<24) /* Select ADS42LB69 & DAC38J84    */
#define DAQ1430_SER_DEV_LMK               (2<<24) /* Select LMK04806                */
#define DAQ1430_SER_DEV_DRP               (3<<24) /* Select DRP interface           */

/* GPIO */

/* DAC_CS */
#define DAQ1430_DAC_GT_TX_RESET_DONE      (1<<16) /* GT TX RESET DONE               */
#define DAQ1430_DAC_GT228_QPLL0_LOCK      (1<<18) /* GT_228 QPLL0 LOCKED            */
#define DAQ1430_DAC_GT226_QPLL0_LOCK      (1<<18) /* GT_226 QPLL0 LOCKED            */
#define DAQ1430_DAC_GT228_REFCLK_LOST     (1<<20) /* GT_228 REFCLK LOST             */
#define DAQ1430_DAC_GT226_REFCLK_LOST     (1<<21) /* GT_228 REFCLK LOST             */
#define DAQ1430_DAC_ALARM                 (1<<24) /* DAC_ALARM signal               */

/* ------------------------------------------------------------------------------------------------------------------ */

#define DAQ1430_SPI_ADS01                 (DAQ1430_SER_DEV_ADS | DAQ1430_SER_SEL_ADS01)
#define DAQ1430_SPI_ADS23                 (DAQ1430_SER_DEV_ADS | DAQ1430_SER_SEL_ADS23)
#define DAQ1430_SPI_ADS45                 (DAQ1430_SER_DEV_ADS | DAQ1430_SER_SEL_ADS45)
#define DAQ1430_SPI_ADS67                 (DAQ1430_SER_DEV_ADS | DAQ1430_SER_SEL_ADS67)
#define DAQ1430_SPI_ADS89                 (DAQ1430_SER_DEV_ADS | DAQ1430_SER_SEL_ADS89)
#define DAQ1430_SPI_DAC38J84              (DAQ1430_SER_DEV_ADS | DAQ1430_SER_SEL_DAC38J84)
#define DAQ1430_SPI_LMK                   (DAQ1430_SER_DEV_LMK)

#define DAQ1430_I2C_BUS                   2             /* I2C bus number         */
#define DAQ1430_I2C_EEPROM                0x01010057    /* I2C EEPROM command     */
#define DAQ1430_I2C_THERMO                0x01040048    /* I2C Thermometer        */
#define DAQ1430_I2C_ADC                   0x00000034    /* I2C ADC                */

/* ATEST */
#define DAQ1430_ATEST_OFF                 0x0           /* ATEST off              */

/* ------------------------------------------------------------------------------------------------------------------ */

#define DAQ1430_LMK_MODE_CLKDIST          0   /* Clock Distribution Mode        */
#define DAQ1430_LMK_MODE_INTREF           1   /* Single PLL Mode                */
#define DAQ1430_LMK_MODE_DUALPLL          2   /* Dual PLL Mode                  */

#define DAQ1430_LMK_REF_FMC               0   /* FMC_CLK2 as reference input    */
#define DAQ1430_LMK_REF_RTM               1   /* RTM_CLK0 as reference input    */

#define DAQ1430_LMK_FREQ_250M             0   /* 250 MHz in dual PLL mode only  */
#define DAQ1430_LMK_FREQ_100M             1   /* 100 MHz in dual PLL mode only  */
#define DAQ1430_LMK_FREQ_10M              2   /*  10 MHz in dual PLL mode only  */

#define DAQ1430_LMK_STATUS_LD             (1<<20)
#define DAQ1430_LMK_STATUS_CLKIN0         (1<<22)
#define DAQ1430_LMK_STATUS_CLKIN1         (1<<23)

/* ------------------------------------------------------------------------------------------------------------------ */

/* ------------------------------------------------------------------------------------------------------------------ */
/*  DAQ_1430 Function Prototypes                                                                                      */
/* ------------------------------------------------------------------------------------------------------------------ */

extern int  daq1430_set_verbose           (int mode);
extern int  daq1430_reset                 (int fd, int fmc);
extern int  daq1430_lmk_init              (int fd, int fmc, int mode, int ref, int freq);
extern int  daq1430_lmk_set_adc_divider   (int fd, int fmc, int divider);
extern int  daq1430_lmk_dump              (int fd, int fmc);
extern int  daq1430_lmk_get_status        (int fd, int fmc, int *data);
extern int  daq1430_ads42lb69_init        (int fd, int fmc, int chan_set);
extern int  daq1430_ads42lb69_dump        (int fd, int fmc, int chan_set);
extern int  daq1430_ads42lb69_set_mode    (int fd, int fmc, int chan, int mode);
extern int  daq1430_ads42lb69_set_pattern (int fd, int fmc, int chan, int pattern);
extern int  daq1430_direct_dump           (int fd, int fmc, int chan);
extern int  daq1430_calib_set_idelay      (int fd, int fmc, int chan, int idelay);
extern int  daq1430_calib_get_idelay      (int fd, int fmc, int chan, int *idelay);
extern int  daq1430_calib_dump_idelay     (int fd, int fmc);
extern int  daq1430_calibrate             (int fd, int fmc, int chan, int step);
extern int  daq1430_read_atest            (int fd, int fmc, int atest_sel, int *measured, int *adc_code, const char ** descr);
extern int  daq1430_gpio_trig             (int fmc);
extern int  daq1430_dac_init              (int fd, int fmc, double freq_refclk, double freq_dac, double linerate, int interp, int k, int rdb, int scr, int sysref_mode);

#endif /*  _H_DAQ1430LIB */

