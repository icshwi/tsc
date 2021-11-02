/*=========================< begin file & file header >=======================
 *  References
 *  
 *    filename : fscope3117lib.h
 *    author   : JFG, XP
 *    company  : IOxOS
 *    creation : october 18,2018
 *
 *----------------------------------------------------------------------------
 *  Description
 *
 *    That library contains a set of function to access the ADC3117 FMC
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

#ifndef _H_ADC3117LIB
#define _H_ADC3117LIB

#define ADC3117_FMC1               1                    /* FMC #1                                 */
#define ADC3117_FMC2               2                    /* FMC #2                                 */
#define ADC3117_FMC_NUM            2                    /* Number of FMC                          */
#define ADC3117_CHAN_NUM          20                    /* Number of channels                     */
#define ADC3117_CHAN_SET_ALL      0xfffff               /* All channel mask                       */
#define ADC3117_CHAN_SET_0        (1<<0)                /* Channel #0                             */
#define ADC3117_CHAN_SET_1        (1<<1)                /* Channel #1                             */
#define ADC3117_CHAN_SET_2        (1<<2)                /* Channel #2                             */
#define ADC3117_CHAN_SET_3        (1<<3)                /* Channel #3                             */
#define ADC3117_CHAN_SET_4        (1<<4)                /* Channel #4                             */
#define ADC3117_CHAN_SET_5        (1<<5)                /* Channel #5                             */
#define ADC3117_CHAN_SET_6        (1<<6)                /* Channel #6                             */
#define ADC3117_CHAN_SET_7        (1<<7)                /* Channel #7                             */
#define ADC3117_CHAN_SET_8        (1<<8)                /* Channel #8                             */
#define ADC3117_CHAN_SET_9        (1<<9)                /* Channel #9                             */
#define ADC3117_CHAN_SET_10       (1<<10)               /* Channel #10                            */
#define ADC3117_CHAN_SET_11       (1<<11)               /* Channel #11                            */
#define ADC3117_CHAN_SET_12       (1<<12)               /* Channel #12                            */
#define ADC3117_CHAN_SET_13       (1<<13)               /* Channel #13                            */
#define ADC3117_CHAN_SET_14       (1<<14)               /* Channel #14                            */
#define ADC3117_CHAN_SET_15       (1<<15)               /* Channel #15                            */
#define ADC3117_CHAN_SET_16       (1<<16)               /* Channel #16                            */
#define ADC3117_CHAN_SET_17       (1<<17)               /* Channel #17                            */
#define ADC3117_CHAN_SET_18       (1<<18)               /* Channel #18                            */
#define ADC3117_CHAN_SET_19       (1<<19)               /* Channel #19                            */
#define ADC3117_CHAN_GROUP        4                     /* 4 chnannel per gscope data pipeline    */


#define ADC3117_CSR_BASE          ADC_CSR_BASE          /* FMC Base address                   */
#define ADC3117_CSR_SIGN          ADC_CSR_SIGN          /* Signature                          */
#define ADC3117_CSR_CTL           ADC_CSR_CTL           /* Main Control & Status              */
#define ADC3117_CSR_LED           ADC_CSR_LED           /* Front-Panel LED Management         */
#define ADC3117_CSR_SERIAL        ADC_CSR_SERAL         /* Serial Interface                   */
#define ADC3117_CSR_GPIO          ADC_CSR_GPIO          /* Front-Panel GPIO Management        */
#define ADC3117_CSR_DISC_CTL      ADC_CSR_DISC          /* Discriminator Control              */
#define ADC3117_CSR_FMC           ADC_CSR_FMC           /* IFC Carrier Support                */
#define ADC3117_CSR_ADC_CFG       0x88                  /* ADC Configuration                  */
#define ADC3117_CSR_ADC_CH_SEL    0x89                  /* ADC Channel Selection              */
#define ADC3117_CSR_ADC_CH_CFG    0x8A                  /* ADC Channel Configuration          */
#define ADC3117_CSR_DISC_STA      0x8B                  /* Discriminator Status               */
#define ADC3117_CSR_DAC_CTL       0x8C                  /* DAC Ramp Generator Control         */
#define ADC3117_CSR_DAC_MIN_MAX   0x8D                  /* DAC Ramp Generator Min & Max       */
#define ADC3117_CSR_ADC_RATE      0x8E                  /* ADC Sample Rate                    */

/* Build an address from a register index */
#define ADC3117_CSR_ADDR(csr)     (ADC3117_CSR_BASE | ((csr&0xff)<<2))

#define ADC3117_SIGN_ID           0x31170000
#define ADC3117_SIGN_MASK         0xffff0000

/* ADC VCAL mux */
#define ADC3117_VCAL_VREF_VAR     0             /* Variable reference */
#define ADC3117_VCAL_VREF_4V      1             /* +4.128 V reference */
#define ADC3117_VCAL_DACOUT_0     2             /* DAC output #0      */
#define ADC3117_VCAL_DACOUT_1     3             /* DAC output #1      */
  
/* ADC mux */ 
#define ADC3117_SW_CON            0             /* connector  */
#define ADC3117_SW_GND            1             /* GND        */
#define ADC3117_SW_DACOFF         2             /* DAC offset */
#define ADC3117_SW_VCAL           3             /* VCAL       */
  
/* ADC gain */  
#define ADC3117_GAIN_x1           0             /* x1         */
#define ADC3117_GAIN_x2           1             /* x2         */
#define ADC3117_GAIN_x5           2             /* x5         */
#define ADC3117_GAIN_x10          3             /* x10        */
  
/* Serial Device */ 
#define ADC3117_SER_DAC_OFF_0     0x0           /* ADC offset compensation DAC #0       */
#define ADC3117_SER_DAC_OFF_1     0x1           /* ADC offset compensation DAC #1       */
#define ADC3117_SER_DAC_OFF_2     0x2           /* ADC offset compensation DAC #2       */
#define ADC3117_SER_DAC_OFF_3     0x3           /* ADC offset compensation DAC #3       */
#define ADC3117_SER_DAC_OFF_4     0x4           /* ADC offset compensation DAC #4       */
#define ADC3117_SER_DAC_OFF_5     0x5           /* ADC offset compensation DAC #5       */
#define ADC3117_SER_DAC_OFF_6     0x6           /* ADC offset compensation DAC #6       */
#define ADC3117_SER_DAC_OFF_7     0x7           /* ADC offset compensation DAC #7       */
#define ADC3117_SER_DAC_OFF_8     0x8           /* ADC offset compensation DAC #8       */
#define ADC3117_SER_DAC_OFF_9     0x9           /* ADC offset compensation DAC #9       */
#define ADC3117_SER_MMCM_DRP      0xA           /* MMCM DRP                             */
#define ADC3117_SER_POT_DAC_OUT_0 0xC           /* Digital Potentiometer of DAC_OUT #0  */
#define ADC3117_SER_POT_DAC_OUT_1 0xD           /* Digital Potentiometer of DAC_OUT #1  */
#define ADC3117_SER_POT_VREF_VAR  0xE           /* Digital Potentiometer of VREF_VAR    */
#define ADC3117_SER_DAC_OUT       0xF           /* DAC_OUT #0 & DAC_OUT #1              */
  
/* I2C EEPROM offsets */  
#define ADC3117_FRU_OFF           0x0000        /* FMC IPMI FRU   EEPROM offset     */
#define ADC3117_FRU_SIZE          0x0400        /* FMC IPMI FRU size                */
#define ADC3117_VREF_OFF          0x6000        /* VREF structure EEPROM offset     */
#define ADC3117_CORR_OFF          0x6200        /* CORR structure EEPROM offset     */
#define ADC3117_SIGN_OFF          0x7000        /* signature      EEPROM offset     */
  
#define ADC3117_VREF_MAGIC        0x56524546    /* 'VREF' in ASCII */
#define ADC3117_CORR_MAGIC        0x434f5252    /* 'CORR' in ASCII */

/* ADC_3117 voltage reference */
typedef struct
{
  int     magic;      /* magic number                 */
  int     len;        /* length                       */
  int     cks;        /* checksum                     */
  float   temp;       /* temperature                  */
  
  struct vref_calib
  {
    int   digpot;     /* digital potentiometer value  */
    float volt;       /* measured voltage             */
  } vref[32];

} __attribute__((packed)) adc3117_vref_t;

/* ADC_3117 offset/gain correction factor */
typedef struct
{
  int     magic;    
  int     len;
  int     cks;
  /* DAC outputs */
  struct dacout_calib
  {
    float offset, gain;   /* default 0.0, 1.0 */
  } dac[2];
  /* ADC inputs */
  struct adc_calib
  {
    float offset, gain;   /* default 0.0, 1.0 */
  } adc[20];

} __attribute__((packed)) adc3117_corr_t;


int  adc3117_set_verbose     (int mode);
int  adc3117_reset           (int fd, int fmc);
int  adc3117_set_vcal        (int fd, int fmc, int vcal_sel);
int  adc3117_set_ch_cfg      (int fd, int fmc, int adc_ch_mask, int sw_n, int sw_p, int gain, int offset);
int  adc3117_update_cfg      (int fd, int fmc);
int  adc3117_serial_write    (int fd, int fmc, int sel, int add, int data);
int  adc3117_serial_read     (int fd, int fmc, int sel, int add, int *data);
int  adc3117_init_dacoff     (int fd, int fmc);
int  adc3117_set_vref_var    (int fd, int fmc, int pot);
int  adc3117_set_dac_output  (int fd, int fmc, int dac_ch, int pot, int dac_code);
int  adc3117_set_dacoff      (int fd, int fmc, int adc_ch, int val);
int  adc3117_set_rate        (int fd, int fmc, int rate);
int  adc3117_vref_show       (adc3117_vref_t *h);
int  adc3117_vref_store      (int fd, int fmc, uint dev, adc3117_vref_t *h);
int  adc3117_vref_load       (int fd, int fmc, uint dev, adc3117_vref_t *h);
int  adc3117_vref_read       (char *filename, adc3117_vref_t *h);
int  adc3117_vref_write      (char *filename, adc3117_vref_t *h);
int  adc3117_corr_show       (adc3117_corr_t *h);
void adc3117_corr_default    (adc3117_corr_t *h);
int  adc3117_corr_store      (int fd, int fmc, uint dev, adc3117_corr_t *h);
int  adc3117_corr_load       (int fd, int fmc, uint dev, adc3117_corr_t *h);
int  adc3117_corr_read       (char *filename, adc3117_corr_t *h);
int  adc3117_corr_write      (char *filename, adc3117_corr_t *h);

#endif /* _H_ADC3117LIB */
