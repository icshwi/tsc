/*=========================< begin file & file header >=======================
 *  References
 *  
 *    filename : fscope3112lib.h
 *    author   : JFG, XP
 *    company  : IOxOS
 *    creation : october 18,2018
 *
 *----------------------------------------------------------------------------
 *  Description
 *
 *    That library contains a set of function to access the ADC3112 FMC
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

#define ADC3112_FMC1    1
#define ADC3112_FMC2    2
#define ADC3112_FMC_NUM     2
#define ADC3112_CHAN_NUM    4
#define ADC3112_DAC_A       1
#define ADC3112_DAC_B       2
#define ADC3112_DAC_AB      3

#define ADC3112_CSR_BASE   0x00001000
#define ADC3112_CSR_OFF_FMC2    0x100
#define ADC3112_CSR_SIGN         0x80
#define ADC3112_CSR_CTL          0x81
#define ADC3112_CSR_LED          0x82
#define ADC3112_CSR_SERIAL       0x83
#define ADC3112_CSR_GPIO         0x85
#define ADC3112_CSR_FMC          0x87
#define ADC3112_CSR_GAIN         0x88
#define ADC3112_CSR_TTIM         0x8c
#define ADC3112_CSR_TTRIG        0x8d
#define ADC3112_CSR_TTRIG_CNT    0x8e
#define ADC3112_CSR_TTRIG_EVT    0x8f
#define ADC3112_CSR_ADDR( csr)   (ADC3112_CSR_BASE | (csr<<2))

#define ADC3112_SIGN_ID    0x31120000
#define ADC3112_SIGN_MASK  0xffff0000

#define ADC3112_SPI_ADS01     0x01000000
#define ADC3112_SPI_ADS23     0x01010000
#define ADC3112_SPI_XRA01     0x01020000
#define ADC3112_SPI_XRA23     0x01030000
#define ADC3112_SPI_LMK       0x02000000
#define ADC3112_SPI_SY        0x02010000
#define ADC3112_SPI_DAC       0x03000000
#define ADC3112_SPI_XRATRIG   0x03010000
#define ADC3112_I2C_EEPROM    0x01010051
#define ADC3112_I2C_THERMO    0x01040048

/* XRA registers */
#define ADC3112_XRA_GSR       0x00
#define ADC3112_XRA_OCR       0x02
#define ADC3112_XRA_PIR       0x04
#define ADC3112_XRA_GCR       0x06
#define ADC3112_XRA_PUR       0x08
#define ADC3112_XRA_IER       0x0a
#define ADC3112_XRA_TSCR      0x0c
#define ADC3112_XRA_ISR       0x0e
#define ADC3112_XRA_REIR      0x10
#define ADC3112_XRA_FEIR      0x12
#define ADC3112_XRA_IFR       0x14

#define adc3112_spi_ads01_read(fd, fmc, reg)             adc3112_spi_read(fd, fmc, ADC3112_SPI_ADS01, reg) 
#define adc3112_spi_ads01_write(fd, fmc, reg, data)      adc3112_spi_write(fd, fmc, ADC3112_SPI_ADS01, reg, data) 

#define adc3112_spi_ads23_read(fd, fmc, reg)             adc3112_spi_read(fd, fmc, ADC3112_SPI_ADS23, reg) 
#define adc3112_spi_ads23_write(fd, fmc, reg, data)      adc3112_spi_write(fd, fmc, ADC3112_SPI_ADS23, reg, data) 

#define adc3112_spi_xra01_read(fd, fmc, reg)             adc3112_spi_read(fd, fmc, ADC3112_SPI_XRA01, reg) 
#define adc3112_spi_xra01_write(fd, fmc, reg, data)      adc3112_spi_write(fd, fmc, ADC3112_SPI_XRA01, reg, data) 

#define adc3112_spi_xra23_read(fd, fmc, reg)             adc3112_spi_read(fd, fmc, ADC3112_SPI_XRA23, reg) 
#define adc3112_spi_xra23_write(fd, fmc, reg, data)      adc3112_spi_write(fd, fmc, ADC3112_SPI_XRA23, reg, data) 

#define adc3112_spi_xratrig_read(fd, fmc, reg)             adc3112_spi_read(fd, fmc, ADC3112_SPI_XRATRIG, reg) 
#define adc3112_spi_xratrig_write(fd, fmc, reg, data)      adc3112_spi_write(fd, fmc, ADC3112_SPI_XRATRIG, reg, data) 

#define adc3112_spi_lmk_read(fd, fmc, reg)               adc3112_spi_read(fd, fmc, ADC3112_SPI_LMK, reg) 
#define adc3112_spi_lmk_write(fd, fmc, reg, data)        adc3112_spi_write(fd, fmc, ADC3112_SPI_LMK, reg, data) 

#define adc3112_spi_sy_read(fd, fmc, reg)               adc3112_spi_read(fd, fmc, ADC3112_SPI_SY, reg) 
#define adc3112_spi_sy_write(fd, fmc, reg, data)        adc3112_spi_write(fd, fmc, ADC3112_SPI_SY, reg, data) 

#define adc3112_spi_dac_read(fd, fmc, reg)               adc3112_spi_read(fd, fmc, ADC3112_SPI_DAC, reg) 
#define adc3112_spi_dac_write(fd, fmc, reg, data)        adc3112_spi_write(fd, fmc, ADC3112_SPI_DAC, reg, data) 

#define adc3112_i2c_eeprom_read(fd, fmc, reg)            adc3112_i2c_read(fd, fmc, ADC3112_I2C_EEPROM, reg) 
#define adc3112_i2c_eeprom_write(fd, fmc, reg, data)     adc3112_i2c_write(fd, fmc, ADC3112_I2C_EEPROM, reg, data) 

#define adc3112_i2c_thermo_read(fd, fmc, reg)            adc3112_i2c_read(fd, fmc, ADC3112_I2C_THERMO, reg) 
#define adc3112_i2c_thermo_write(fd, fmc, reg, data)     adc3112_i2c_write(fd, fmc, ADC3112_I2C_THERMO, reg, data) 

#define ADC3112_XRA_FE_IN_GND   0x55        /* XRA1404 OCR    Grounded ADC inputs  */
#define ADC3112_XRA_FE_IN_POS   0x22        /* XRA1404 OCR    SE Pos input  */
#define ADC3112_XRA_FE_IN_NEG   0xEE        /* XRA1404 OCR    SE Neg input  */
#define ADC3112_XRA_FE_MASK     0xFF

#define ADC3112_XRA_TRIG_ADC0     0x83
#define ADC3112_XRA_TRIG_ADC1     0x84
#define ADC3112_XRA_TRIG_ADC2     0x85
#define ADC3112_XRA_TRIG_ADC3     0x86
#define ADC3112_XRA_TRIG_DIS      0x82
#define ADC3112_XRA_TRIG_HIZ      0x87
#define ADC3112_XRA_TRIG_GPIO     0x57
#define ADC3112_XRA_TRIG_ADC      0x47
#define ADC3112_XRA_TRIG_RISE     0x20
 
#define ADC3112_TTRIG_ENA       0xc0
#define ADC3112_TTRIG_ARM       0x01
#define ADC3112_TTRIG_RESET     0x02
#define ADC3112_TTRIG_ABORT     0x03

#define ADC3112_ADS_FILTER_NO    0x000
#define ADC3112_ADS_FILTER_LOW   0x100
#define ADC3112_ADS_FILTER_HIGH  0x200
#define ADC3112_ADS_FILTER_MASK  0xf00

#define CAL_ALL_BIT  0x8000
#ifdef IFC1211
#define CAL_STEP_NUM 64
#define CAL_STEP_WIDTH  64
#else
#define CAL_STEP_NUM 40
#define CAL_STEP_WIDTH  78.125
#endif
#define CAL_BIT_NUM  16
#define ADC_NUM_SAMPLES       0x8000                /* 32k samples */
#define ADC_SAMPLE_SIZE       sizeof(short)

struct adc3112_calib_ctl
{
  int fmc;
  struct adc3112_calib_chan
  {
    int chan;
    int delay;
    int ttim[CAL_BIT_NUM];
    int err_cnt[CAL_BIT_NUM][CAL_STEP_NUM+1];
    int cal_res[CAL_STEP_NUM+1];
    int delta[CAL_BIT_NUM];
    int hold_time;
    int set_time;
    char *data_buf;
  } chan[ADC3112_CHAN_NUM];
};

int adc3112_set_verbose(int vf);
int adc3112_identify(int fd, int fmc);
void adc3112_reset(int fd, int fmc);
int adc3112_spi_read(int fd, int fmc, int cmd, int reg);
int adc3112_spi_write(int fd, int fmc, int cmd, int reg, int data);
uint adc3112_i2c_read(int fd, int fmc, uint cmd, uint reg);
int adc3112_i2c_write(int fd, int fmc, uint cmd, uint reg, uint data);
void adc3112_xra1404_init(int fd, int fmc);
void adc3112_xratrig_enable(int fd, int fmc, int src, int level);
void adc3112_xratrig_disable(int fd, int fmc); 
void adc3112_lmk_init(int fd, int fmc, int reg[]);
void adc3112_lmk_dump(int fd, int fmc);
void adc3112_ads5409_reset(int fd, int fmc, int filter); 
void adc3112_ads5409_init(int fd, int fmc, int corr); 
void adc3112_ads5409_dump(int fd, int fmc); 
int  adc3112_ads5409_enable(int fd, int fmc, int m01, int m23);
int adc3112_gpio_reset(int fd, int fmc);
int adc3112_gpio_trig(int fd, int fmc);
int adc3112_ttrig_arm(int fd, int fmc);
void adc3112_calib_init(int fd, struct adc3112_calib_ctl *cc, int fmc);
void adc3112_calib_config(int fd, int fmc, uint mode);
void adc3112_calib_set_default(int fd,  struct adc3112_calib_ctl *cc, int chan);
int adc3112_calib_dec_delay(int fd, struct adc3112_calib_ctl *cc, int set, int chan);
int adc3112_calib_inc_delay(int fd, struct adc3112_calib_ctl *cc, int set, int chan);
void adc3112_calib_adjust_delay(int fd, struct adc3112_calib_ctl *cc, int chan);
void adc3112_calib_reset_delta( struct adc3112_calib_ctl *cc);
unsigned short adc3112_calib_data_check( struct adc3112_calib_ctl *cc, int chan, unsigned short cmp0, unsigned short cmp1, char *adc_buf);
int adc3112_calib_show_res( struct adc3112_calib_ctl *cc);
int adc3112_calib_show_err_cnt( struct adc3112_calib_ctl *cc, int chan);
int adc3112_calib_show_min_max( struct adc3112_calib_ctl *cc, int chan);
void adc3112_calib_get_ttim(int fd, struct adc3112_calib_ctl *cc, int chan);
void adc3112_calib_restore(int fd, int fmc);

    
