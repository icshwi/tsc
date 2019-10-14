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


#define adc3110_spi_ads01_read(fd, fmc, reg)             adc3110_spi_read(fd, fmc, ADC3110_SPI_ADS01, reg) 
#define adc3110_spi_ads01_write(fd, fmc, reg, data)      adc3110_spi_write(fd, fmc, ADC3110_SPI_ADS01, reg, data) 

#define adc3110_spi_ads23_read(fd, fmc, reg)             adc3110_spi_read(fd, fmc, ADC3110_SPI_ADS23, reg) 
#define adc3110_spi_ads23_write(fd, fmc, reg, data)      adc3110_spi_write(fd, fmc, ADC3110_SPI_ADS23, reg, data) 

#define adc3110_spi_ads45_read(fd, fmc, reg)             adc3110_spi_read(fd, fmc, ADC3110_SPI_ADS45, reg) 
#define adc3110_spi_ads45_write(fd, fmc, reg, data)      adc3110_spi_write(fd, fmc, ADC3110_SPI_ADS45, reg, data) 

#define adc3110_spi_ads67_read(fd, fmc, reg)             adc3110_spi_read(fd, fmc, ADC3110_SPI_ADS67, reg) 
#define adc3110_spi_ads67_write(fd, fmc, reg, data)      adc3110_spi_write(fd, fmc, ADC3110_SPI_ADS67, reg, data) 

#define adc3110_spi_lmk_read(fd, fmc, reg)               adc3110_spi_read(fd, fmc, ADC3110_SPI_LMK, reg) 
#define adc3110_spi_lmk_write(fd, fmc, reg, data)        adc3110_spi_write(fd, fmc, ADC3110_SPI_LMK, reg, data) 

#define adc3110_i2c_eeprom_read(fd, fmc, reg)            adc3110_i2c_read(fd, fmc, ADC3110_I2C_EEPROM, reg) 
#define adc3110_i2c_eeprom_write(fd, fmc, reg, data)     adc3110_i2c_write(fd, fmc, ADC3110_I2C_EEPROM, reg, data) 

#define adc3110_i2c_thermo_read(fd, fmc, reg)            adc3110_i2c_read(fd, fmc, ADC3110_I2C_THERMO, reg) 
#define adc3110_i2c_thermo_write(fd, fmc, reg, data)     adc3110_i2c_write(fd, fmc, ADC3110_I2C_THERMO, reg, data) 
 
#define ADC3110_ADS_MODE_NORM      0x000
#define ADC3110_ADS_MODE_ZERO      0x001
#define ADC3110_ADS_MODE_ONE       0x002
#define ADC3110_ADS_MODE_TOGGLE    0x003
#define ADC3110_ADS_MODE_RAMP      0x004
#define ADC3110_ADS_MODE_TST1      0x006
#define ADC3110_ADS_MODE_TST2      0x007
#define ADC3110_ADS_MODE_RAND      0x00a
#define ADC3110_ADS_MODE_SINE      0x00b


int adc3110_set_verbose(int vf);
int adc3110_identify(int fd, int fmc);
void adc3110_reset(int fd, int fmc);
int adc3110_spi_read(int fd, int fmc, int cmd, int reg);
int adc3110_spi_write(int fd, int fmc, int cmd, int reg, int data);
uint adc3110_i2c_read(int fd, int fmc, uint cmd, uint reg);
int adc3110_i2c_write(int fd, int fmc, uint cmd, uint reg, uint data);
void adc3110_lmk_init(int fd, int fmc, int reg[]);
void adc3110_lmk_dump(int fd, int fmc);
void adc3110_ads42lb69_init(int fd, int fmc, int chan_set); 
void adc3110_ads42lb69_dump(int fd, int fmc); 
int adc3110_ads42lb69_set_mode(int fd, int fmc, int chan, int mode); 
int adc3110_ads42lb69_set_pattern(int fd, int fmc, int chan, int pattern); 
int adc3110_calib_set_idelay(int fd, int fmc, int chan, int idelay);
int adc3110_calib_get_idelay(int fd, int fmc, int chan);
int adc3110_gpio_trig(int fd, int fmc);
    
