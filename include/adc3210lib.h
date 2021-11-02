/*=========================< begin file & file header >=======================
 *  References
 *
 *    filename : fscope3210lib.h
 *    author   : JFG, XP
 *    company  : IOxOS
 *    creation : october 18,2018
 *
 *----------------------------------------------------------------------------
 *  Description
 *
 *    That library contains a set of function to access the ADC3210 FMC
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

#ifndef _H_ADC3210LIB
#define _H_ADC3210LIB

#define ADC3210_FMC1    1
#define ADC3210_FMC2    2
#define ADC3210_FMC_NUM     2
#define ADC3210_CHAN_NUM    4
#define ADC3210_CHAN_SET_ALL    0xff
#define ADC3210_CHAN_SET_0      0x01
#define ADC3210_CHAN_SET_1      0x02
#define ADC3210_CHAN_SET_2      0x04
#define ADC3210_CHAN_SET_3      0x08
#define ADC3210_CHAN_SET_4      0x10
#define ADC3210_CHAN_SET_5      0x20
#define ADC3210_CHAN_SET_6      0x40
#define ADC3210_CHAN_SET_7      0x80

#define ADC3210_TCSR_BASE   0x00001000
#define ADC3210_TCSR_OFF_FMC2    0x100
#define ADC3210_TCSR_SIGN         0x80
#define ADC3210_TCSR_CTL          0x81
#define ADC3210_TCSR_LED          0x82
#define ADC3210_TCSR_SPARE        0x83
#define ADC3210_TCSR_SPI_CTL1     0x84
#define ADC3210_TCSR_SPI_DATA1    0x85
#define ADC3210_TCSR_SPI_CTL2     0x86
#define ADC3210_TCSR_SPI_DATA2    0x87
#define ADC3210_TCSR_I2C_CTL      0x88
#define ADC3210_TCSR_I2C_CMD      0x89
#define ADC3210_TCSR_I2C_DATW     0x8a
#define ADC3210_TCSR_I2C_DATR     0x8b
#define ADC3210_TCSR_JSD_CTL1     0x8c
#define ADC3210_TCSR_JSD_CTL2     0x8d
#define ADC3210_TCSR_JSD_CTL3     0x8e
#define ADC3210_TCSR_JSD_CTL4     0x8f

#define ADC3210_TCSR_ADDR( csr)   (ADC3210_TCSR_BASE | (csr<<2))

#define ADC3210_SIGN_ID    0x32100000
#define ADC3210_SIGN_MASK  0xffff0000

#define ADC3210_SPI_1         0x00000000
#define ADC3210_SPI_2         0x00800000

#define ADC3210_SPI_ADS01     (ADC3210_SPI_1 | 0x00000000)
#define ADC3210_SPI_ADS23     (ADC3210_SPI_1 | 0x00010000)
#define ADC3210_SPI_ADS45     (ADC3210_SPI_1 | 0x00020000)
#define ADC3210_SPI_ADS67     (ADC3210_SPI_1 | 0x00030000)

#define ADC3210_SPI_DAC0      (ADC3210_SPI_2 | 0x00040000)
#define ADC3210_SPI_DAC1      (ADC3210_SPI_2 | 0x00050000)
#define ADC3210_SPI_DAC2      (ADC3210_SPI_2 | 0x00060000)
#define ADC3210_SPI_LMK       (ADC3210_SPI_2 | 0x00030000)

#define ADC3210_I2C_TADC              0x44080014
#define ADC3210_I2C_VCXO              0x2501005a
#define ADC3210_I2C_TMP102            0x40040048

#define ADC3210_I2C_STS_MASK          0x00300000
#define ADC3210_I2C_STS_IDLE          0x00000000
#define ADC3210_I2C_STS_RUNNING       0x00100000
#define ADC3210_I2C_STS_DONE          0x00200000
#define ADC3210_I2C_STS_ERROR         0x00300000

#define ADC3210_I2C_TRIG_MASK         0x00c00000
#define ADC3210_I2C_TRIG_CMD          0x00400000
#define ADC3210_I2C_TRIG_DATW         0x00800000
#define ADC3210_I2C_TRIG_DATR         0x00c00000

#define ADC3210_JREG_IF_CTL_0         0x000   /* JESD Interface Control Register 0 */
#define ADC3210_JREG_IF_CTL_1         0x001   /* JESD Interface Control Register 1 */
#define ADC3210_JREG_IF_CTL_2         0x002   /* JESD Interface Control Register 1 */
#define ADC3210_JREG_IF_CONF_0        0x010   /* JESD Interface Link Configuration Lane 0 */
#define ADC3210_JREG_IF_CONF_1        0x014   /* JESD Interface Link Configuration Lane 1 */
#define ADC3210_JREG_IF_CONF_2        0x018   /* JESD Interface Link Configuration Lane 2 */
#define ADC3210_JREG_IF_CONF_3        0x01C   /* JESD Interface Link Configuration Lane 3 */

#define ADC3210_JREG_PHY_VER          0x400   /* Version                          */
#define ADC3210_JREG_PHY_IPCONF       0x401   /* IP configuration                 */
#define ADC3210_JREG_PHY_NCOM_IF      0x402   /* Number of Common Interface       */
#define ADC3210_JREG_PHY_NTRX_IF      0x403   /* Number of Transceiver Interfaces */
#define ADC3210_JREG_PHY_RXLINECODE   0x40E   /* RX Interface Line encoding       */
#define ADC3210_JREG_PHY_PLLSTATUS    0x420   /* PLL status                       */   
#define ADC3210_JREG_PHY_RXLINERATE   0x424   /* RX Line Rate                     */
#define ADC3210_JREG_PHY_RXREFCLK     0x426   /* RX REFCLK                        */
#define ADC3210_JREG_PHY_RXPLL        0x428   /* RXPLL                            */
#define ADC3210_JREG_PHY_SW_CAPABLE   0x434   /* Switch Capable                   */
#define ADC3210_JREG_PHY_INS_LOSS     0x435   /* Insertion Loss                   */ 
#define ADC3210_JREG_PHY_EQUALIZATION 0x436   /* Equalization                     */
#define ADC3210_JREG_PHY_MIN_RATE     0x438   /* Min rate                         */
#define ADC3210_JREG_PHY_MAX_RATE     0x439   /* Max rate                         */


#define ADC3210_JESD_ERROR            (1<<28)
#define ADC3210_JESD_TIMEOUT          (1<<29)
#define ADC3210_JESD_RnW              (1<<30)
#define ADC3210_JESD_GO               (1<<31)
#define ADC3210_JESD_BUSY             (1<<31)

#define adc3210_spi_ads01_read(fd, fmc, reg)             adc3210_spi_read(fd, fmc, ADC3210_SPI_ADS01, reg) 
#define adc3210_spi_ads01_write(fd, fmc, reg, data)      adc3210_spi_write(fd, fmc, ADC3210_SPI_ADS01, reg, data) 

#define adc3210_spi_ads23_read(fd, fmc, reg)             adc3210_spi_read(fd, fmc, ADC3210_SPI_ADS23, reg) 
#define adc3210_spi_ads23_write(fd, fmc, reg, data)      adc3210_spi_write(fd, fmc, ADC3210_SPI_ADS23, reg, data) 

#define adc3210_spi_ads45_read(fd, fmc, reg)             adc3210_spi_read(fd, fmc, ADC3210_SPI_ADS45, reg) 
#define adc3210_spi_ads45_write(fd, fmc, reg, data)      adc3210_spi_write(fd, fmc, ADC3210_SPI_ADS45, reg, data) 

#define adc3210_spi_ads67_read(fd, fmc, reg)             adc3210_spi_read(fd, fmc, ADC3210_SPI_ADS67, reg) 
#define adc3210_spi_ads67_write(fd, fmc, reg, data)      adc3210_spi_write(fd, fmc, ADC3210_SPI_ADS67, reg, data) 

#define adc3210_spi_dac0_read(fd, fmc, reg)             adc3210_spi_read(fd, fmc, ADC3210_SPI_DAC0, reg) 
#define adc3210_spi_dac0_write(fd, fmc, reg, data)      adc3210_spi_write(fd, fmc, ADC3210_SPI_DAC0, reg, data) 

#define adc3210_spi_dac1_read(fd, fmc, reg)             adc3210_spi_read(fd, fmc, ADC3210_SPI_DAC1, reg) 
#define adc3210_spi_dac1_write(fd, fmc, reg, data)      adc3210_spi_write(fd, fmc, ADC3210_SPI_DAC1, reg, data) 

#define adc3210_spi_dac2_read(fd, fmc, reg)             adc3210_spi_read(fd, fmc, ADC3210_SPI_DAC2, reg) 
#define adc3210_spi_dac2_write(fd, fmc, reg, data)      adc3210_spi_write(fd, fmc, ADC3210_SPI_DAC2, reg, data) 

#define adc3210_spi_lmk_read(fd, fmc, reg)               adc3210_spi_read(fd, fmc, ADC3210_SPI_LMK, reg) 
#define adc3210_spi_lmk_write(fd, fmc, reg, data)        adc3210_spi_write(fd, fmc, ADC3210_SPI_LMK, reg, data) 

#define adc3210_i2c_tadc_read(fd, fmc, reg)              adc_i2c_read(fd, fmc, ADC3210_I2C_TADC, reg) 
#define adc3210_i2c_tadc_write(fd, fmc, reg, data)       adc_i2c_write(fd, fmc, ADC3210_I2C_TADC, reg, data) 

#define adc3210_i2c_vcxo_read(fd, fmc, reg)              adc_i2c_read(fd, fmc, ADC3210_I2C_VCXO, reg) 
#define adc3210_i2c_vcxo_write(fd, fmc, reg, data)       adc_i2c_write(fd, fmc, ADC3210_I2C_VCXO, reg, data) 
 
#define ADC3210_ADS_MODE_NORM      0x000
#define ADC3210_ADS_MODE_ZERO      0x001
#define ADC3210_ADS_MODE_ONE       0x002
#define ADC3210_ADS_MODE_TOGGLE    0x003
#define ADC3210_ADS_MODE_RAMP      0x004
#define ADC3210_ADS_MODE_TST1      0x006
#define ADC3210_ADS_MODE_TST2      0x007
#define ADC3210_ADS_MODE_RAND      0x00a
#define ADC3210_ADS_MODE_SINE      0x00b

#define adc3210_csr_rd(fd, fmc, csr, data)           fmc_csr_rd(fd, fmc, csr, data)
#define adc3210_csr_wr(fd, fmc, csr, data)           fmc_csr_wr(fd, fmc, csr, data)
#define adc3210_identify(fd, fmc)                    fmc_identify(fd, fmc, id, NULL, NULL)

int adc3210_set_verbose(int vf);
void adc3210_reset(int fd, int fmc);
int adc3210_spi_read(int fd, int fmc, int cmd, int reg);
int adc3210_spi_write(int fd, int fmc, int cmd, int reg, int data);
int  adc3210_jesd_read (int fd, int fmc, int idx, int reg);
int  adc3210_jesd_write (int fd, int fmc, int idx, int reg, int data);
void adc3210_jesd_dump (int fd, int fmc, int idx);
int adc3210_i2c_read(int fd, int fmc, int dev);
int adc3210_i2c_write(int fd, int fmc, int dev, int reg, int data);
void adc3210_lmk_init(int fd, int fmc, int reg[]);
void adc3210_lmk_dump(int fd, int fmc);
void adc3210_ad9695_init(int fmc, int chan_set);
void adc3210_ad9695_dump(int fd, int fmc);
int adc3210_ltc2489_conv(int fd, int fmc, int chan);

#endif /* _H_ADC3210LIB */

