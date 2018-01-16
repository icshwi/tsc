/*=========================< begin file & file header >=======================
 *  References
 *  
 *    filename : adc3112.c
 *    author   : JFG
 *    company  : IOxOS
 *    creation : June 11,2014
 *    version  : 0.0.1
 *
 *----------------------------------------------------------------------------
 *  Description
 *
 *     That file contains a set of function called by XprsMon to cotrol an
 *     ADC3112 FMC.
 *
 *----------------------------------------------------------------------------
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

#ifndef lint
static char *rcsid = "$Id: adc3112.c,v 1.10 2016/01/15 10:21:19 ioxos Exp $";
#endif

#define DEBUGno
#include <debug.h>

#include <sys/types.h>
#include <sys/time.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <time.h>
#include <cli.h>
#include <unistd.h>
#include "../../include/tscextlib.h"
#include <tscioctl.h>
#include <tsculib.h>

#define BUS_SPI   0x01
#define BUS_I2C   0x02
#define BUS_SBC   0x03
#define BUS_READ  0x10
#define BUS_WRITE 0x20

#define MAP_OLDno

#define ADC_REG_BMOV_A        0x1100
#define ADC_REG_BMOV_B        0x1110
#define ACQ_REG_BASE_A        0x11c0
#define ACQ_REG_BASE_B        0x11d0
#define ACQ_REG_BASE_1        0x11c0
#define ACQ_REG_BASE_2        0x11d0

#define ADC_REG_SIGN_A       0x1200
#define ADC_REG_SIGN_B       0x1300

#define ADC_REG_SERIAL_A     0x120c
#define ADC_REG_SERIAL_B     0x130c


#define ADC_REG_GAIN_A        0x1220
#define ADC_REG_GAIN_B        0x1320

#define ADC_REG_TTIM_A         0x1230
#define ADC_REG_TTIM_B         0x1330

#define ADS_REG_TEMP 0x2b

#define ADC_NUM_SAMPLES       0x8000                /* 32k samples */
#define ADC_SAMPLE_SIZE       sizeof(short)

#define CAL_ALL_BIT  0x8000
#define CAL_STEP_NUM 40
#define CAL_BIT_NUM  16
#define CAL_STEP_WIDTH  78.125
#define NUM_FMC 2
#define ADC_NUM_CHAN 4
#define ADC_OFF_CHAN_0  0x00000
#define ADC_OFF_CHAN_1  (ADC_OFF_CHAN_0 + (ADC_NUM_SAMPLES*ADC_SAMPLE_SIZE))
#define ADC_OFF_CHAN_2  (ADC_OFF_CHAN_0 + (2*ADC_OFF_CHAN_1))
#define ADC_OFF_CHAN_3  (ADC_OFF_CHAN_0 + (3*ADC_OFF_CHAN_1))

#define ADC_ADS01_CMD     0x01000000
#define ADC_ADS23_CMD     0x01010000
#define ADC_XRA01_CMD     0x01020000
#define ADC_XRA23_CMD     0x01030000
#define ADC_LMK_CMD       0x02000000
#define ADC_SY_CMD        0x02010000
#define ADC_DAC_CMD       0x03000000
#define ADC_XRATRIG_CMD   0x03010000
#define ADC_EEPROM_CMD    0x01010051
#define FMC_IDX(fmc)      (fmc-1)
struct tsc_adc3112_devices
{
  char *name;
  uint cmd;
  int idx;
  int bus;
}
adc3112_devices[] =
{
  { "ads01",   0x01000000, 0, BUS_SPI|BUS_READ|BUS_WRITE},
  { "ads1",    0x01000000, 0, BUS_SPI|BUS_READ|BUS_WRITE},
  { "ads23",   0x01010000, 1, BUS_SPI|BUS_READ|BUS_WRITE},
  { "ads2",    0x01010000, 1, BUS_SPI|BUS_READ|BUS_WRITE},
  { "ads",     0x00000000, -1, 0},
  { "ads0123", 0x00000000, -1, 0},
  { "xra01",   0x01020000, 2, BUS_SPI|BUS_WRITE},
  { "xra23",   0x01030000, 3, BUS_SPI|BUS_WRITE},
  { "lmk",     0x02000000, 0, BUS_SPI|BUS_READ|BUS_WRITE},
  { "sy",      0x02010000, 0, BUS_SPI|BUS_WRITE},
  { "dac",     0x03000000, 0, BUS_SPI|BUS_WRITE},
  { "xratrig", 0x03010000, 0, BUS_SPI|BUS_WRITE},
  { "tmp102",  0x01040048, 0, BUS_I2C},
  { "eeprom",  0x01010051, 1, BUS_I2C},
  { NULL, 0}
};

struct adc3112_reg
{
  int bmov;
  int base;
  int sign;
  int serial;
  int gain;
  int ttim;
} 
adc3112_reg[NUM_FMC];

struct adc3112_sign
{
  char board_name[8];   /*  0x00  [0] */
  char serial[4];       /*  0x08  [8] */
  char version[8];      /*  0x0c  [12] */
  char revision[2];     /*  0x14  [20] */
  char rsv[6];          /*  0x16  [22] */
  char test_date[8];    /*  0x1c  [28] */
  char calib_date[8];   /*  0x24  [36] */
  char itl_corr[4][8];  /*  0x2c  [44] */
  char pad[176];        /*  0x4c  [76] */
  int cks;              /*  0x00 [252] */
} adc3112_sign;         /* 0x100 [256] */
struct adc3112_acq_res
{
  struct adc3112_acq_chan
  {
    uint tot;
    short min;
    short max;
    float mean;
    float sig;
    int off_min;
    int off_max;
  } chan[ADC_NUM_CHAN];
};

struct adc3112_calib_ctl
{
  int reg_ttim_cal;
  int reg_acq_csr; 
  int reg_adc_serial; 
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
  } chan[ADC_NUM_CHAN];
} adc3112_calib_ctl[NUM_FMC];

struct adc3112_itl_ctl
{
  int reg_itl_gain;
  int reg_acq_csr; 
  struct adc3112_itl_chan
  {
    int chan;
    int gain;
    int offset;
    char *data_buf;
  } chan[ADC_NUM_CHAN];
  struct adc3112_acq_res acq_res;
} adc3112_itl_ctl[NUM_FMC];

int adc3112_init_flag[NUM_FMC] = {0,0};
int adc3112_verbose_flag = 0;
struct cli_cmd_history adc3112_history;
char adc3112_prompt[32];
struct tsc_ioctl_map_win adc3112_mas_map;

#define I2C_CTL_EXEC_IDLE  0x00000000
#define I2C_CTL_EXEC_RUN   0x00100000
#define I2C_CTL_EXEC_DONE  0x00200000
#define I2C_CTL_EXEC_ERR   0x00300000
#define I2C_CTL_EXEC_MASK  0x00300000

char *
adc3112_rcsid()
{
  return( rcsid);
}

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * Function name : adc3112_spi_read
 * Prototype     : 
 * Parameters    : 
 * Return        : 
 *----------------------------------------------------------------------------
 * Description   : 
 *
 *++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

int
adc3112_spi_read( int fmc,
		  int cmd,
	 	  int reg)
{
  int tmo, data, csr;

  csr = adc3112_reg[FMC_IDX(fmc)].serial;
  cmd |=  0x80000000 | reg;
  tmo = 1000;
  //printf("adc3112_spi_read( %x, %x, %x) csr = %x\n", fmc, cmd, reg, csr);
  tscext_csr_wr( csr, cmd);
  while( --tmo)
  {
    if( !(tscext_csr_rd( csr) & 0x80000000)) break;
  }
  data = tscext_csr_rd( csr + 4);
  //printf("cmd = %08x - data = %08x\n", cmd, data);
  if( !tmo)
  {
    printf("adc3112_spi_read() : cmd = %08x -> timeout...\n", cmd);
    return(-1);
  }

  return( data);
}

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * Function name : adc3112_spi_write
 * Prototype     : 
 * Parameters    : 
 * Return        : 
 *----------------------------------------------------------------------------
 * Description   : 
 *
 *++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

int
adc3112_spi_write( int fmc,
		   int cmd,
		   int reg,
		   int data)
{
  int tmo, csr;

  csr = adc3112_reg[FMC_IDX(fmc)].serial;
  cmd |= 0xc0000000 | reg;
  //printf("adc3112_spi_write( %x, %x, %x, %x) csr = %x\n", fmc, cmd, reg, data, csr);
  tscext_csr_wr( csr + 4, data);          /* load data */
  tscext_csr_rd( csr+4);                  /* flush data */
  tscext_csr_wr( csr, cmd);               /* write data */
  tmo = 1000;
  while( --tmo)
  {
    if( !(tscext_csr_rd( csr) & 0x80000000)) break; /* check end of write cycle */
  }
  if( !tmo)
  {
    printf("adc3112_spi_write() : cmd = %08x  -> timeout...\n", cmd);
    return(-1);
  }

  return(0);
}

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * Function name : adc3112_init
 * Prototype     : void
 * Parameters    : void
 * Return        : void
 *----------------------------------------------------------------------------
 * Description   : adc3112 init
 *
 *++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

void 
adc3112_init( int fmc)
{
  int fmc_idx;

  fmc_idx = fmc - 1;
  if( !adc3112_init_flag[fmc_idx])
  {
    cli_history_init( &adc3112_history);
    adc3112_init_flag[fmc_idx] = 1;
    if( fmc_idx)
    {
      adc3112_reg[fmc_idx].bmov = ADC_REG_BMOV_B;
      adc3112_reg[fmc_idx].base = ACQ_REG_BASE_B;
      adc3112_reg[fmc_idx].sign = ADC_REG_SIGN_B;
      adc3112_reg[fmc_idx].serial = ADC_REG_SERIAL_B;
      adc3112_reg[fmc_idx].gain = ADC_REG_GAIN_B;
      adc3112_reg[fmc_idx].ttim = ADC_REG_TTIM_B;
    }
    else
    {
      adc3112_reg[fmc_idx].bmov = ADC_REG_BMOV_A;
      adc3112_reg[fmc_idx].base = ACQ_REG_BASE_A;
      adc3112_reg[fmc_idx].sign = ADC_REG_SIGN_A;
      adc3112_reg[fmc_idx].serial = ADC_REG_SERIAL_A;
      adc3112_reg[fmc_idx].gain = ADC_REG_GAIN_A;
      adc3112_reg[fmc_idx].ttim = ADC_REG_TTIM_A;
    }
    bzero( &adc3112_calib_ctl[fmc_idx], sizeof(struct adc3112_calib_ctl));
    bzero( &adc3112_itl_ctl[fmc_idx], sizeof(struct adc3112_itl_ctl));

    /* set FMC#1 ADS01 to 4 wire SPI */
    printf("set FMC#%d ADS01 to 4 wire SPI\n", fmc);
    adc3112_spi_write( fmc, ADC_ADS01_CMD, 0, 0x8000);

    /* set FMC#1 ADS23 to 4 wire SPI */
    printf("set FMC#%d ADS23 to 4 wire SPI\n", fmc);
    adc3112_spi_write( fmc, ADC_ADS23_CMD, 0, 0x8000);

  }

  return;
}

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * Function name : adc3112_map_usr
 * Prototype     : char *
 * Parameters    : map win structure, remote address, size, user
 * Return        : success/error
 *----------------------------------------------------------------------------
 * Description   : map user space
 *
 *++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

char *
adc3112_map_usr( struct tsc_ioctl_map_win *map,
                 ulong rem_addr,
		 uint size,
		 int usr)
{
  bzero( map, sizeof(map));
  map->req.rem_addr = rem_addr;
  if( usr == 2)
  {
    map->req.mode.space = MAP_SPACE_USR2;
  }
  else 
  {
    map->req.mode.space = MAP_SPACE_USR1;
  }
  map->req.mode.sg_id = MAP_ID_MAS_PCIE_MEM;
  map->req.size = size;
  tsc_map_alloc( map);
  return( tsc_pci_mmap( map->sts.loc_base, map->sts.size));
}

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * Function name : adc3112_unmap_usr
 * Prototype     : int
 * Parameters    : map win structure, buffer pointer
 * Return        : success/error
 *----------------------------------------------------------------------------
 * Description   : unmap user space
 *
 *++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

int
adc3112_unmap_usr( struct tsc_ioctl_map_win *map,
		   char *u_addr)
{
  tsc_pci_munmap( u_addr, map->sts.size);
  return( tsc_map_free( map));
}

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * Function name : adc3112_trig_acq
 * Prototype     : 
 * Parameters    : 
 * Return        : 
 *----------------------------------------------------------------------------
 * Description   : 
 *
 *++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

int
adc3112_trig_acq( int reg_base,
		  int trig)
{
  //printf("Trig data acquisition...\n");
  tscext_csr_wr( reg_base + 0x0, 0x80040001);/* FASTSCOPE Abort/Stop previous acquisition SRAM1 (in case of..) */
  tscext_csr_wr( reg_base + 0x4, trig);      /* FASTSCOPE Define trigger mode */
  tscext_csr_wr( reg_base + 0x0, 0x40040001);/* Arm data acquisition SRAM1 */
  usleep( 2000);
  tscext_csr_wr( reg_base + 0x8, 0x40000000);/* FASTSCOPE Manual trigger command */
  usleep( 2000);

#ifdef JFG
  /* data simulation */
  for( i = 0x10000; i < 0x20000; i += 4)
  {
    short d;

    d = (short)( (i/4)&0x3fff);
    *(int *)&adc_buf[i] =(int) (tsc_swap_16(d)<<16) |  (int)tsc_swap_16(d);
  }
  for( i = 0x20000; i < 0x30000; i += 4)
  {
    *(int *)&adc_buf[i] = 0x03000200;
  }
  for( i = 0x30000; i < 0x40000; i += 4)
  {
    *(int *)&adc_buf[i] = 0;
  }
#endif
  return( 1);
}


/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * Function name : adc3112_acq_res
 * Prototype     : int
 * Parameters    : adc3112 structure
 * Return        : 0
 *----------------------------------------------------------------------------
 * Description   : 
 *
 *++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

int 
adc3112_acq_res( struct adc3112_acq_res *r,
		 short *buf,
		 int size,
		 int chan)
{
  int i;

  size = size/sizeof(short);

  r->chan[chan].tot = 0;
  r->chan[chan].mean = 0;
  r->chan[chan].sig = 0;
  r->chan[chan].min = 0x7fff;
  r->chan[chan].max = 0x8000;
  for( i = 0; i < size; i++)
  {
    if( buf[i] < r->chan[chan].min)
    {
      r->chan[chan].min = buf[i];
      r->chan[chan].off_min = i;
    }
    if( buf[i] > r->chan[chan].max)
    {
      r->chan[chan].max = buf[i];
      r->chan[chan].off_max = i;
    }
    r->chan[chan].mean += (float)buf[i];
  }
  r->chan[chan].mean = r->chan[chan].mean/size;

  for( i = 0; i < size; i++)
  {
    r->chan[chan].sig += (buf[i] - r->chan[chan].mean)*(buf[i] - r->chan[chan].mean);
  }
  r->chan[chan].sig = sqrt(r->chan[chan].sig/size);

  return(0);
}

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * Function name : adc3112_fpfbuf_save
 * Prototype     : 
 * Parameters    : 
 * Return        : 
 *----------------------------------------------------------------------------
 * Description   : 
 *
 *++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

int
adc3112_fpfbuf_save( struct cli_cmd_para *c,
		     int fmc)
{
  int i, j, n;
  char *acq_buf, *avg_buf, *zoom_buf;
  char *acq_name_h;
  char *acq_name_h1, *acq_name_h2, *acq_name_h3, *acq_name_h4;
  FILE *acq_file_h1, *acq_file_h2, *acq_file_h3, *acq_file_h4;
  char *default_name = "fpf_file";
  int num_frames;
  int num_samples;
  int csr;

  acq_name_h = default_name;
  if( c->cnt > 2)
  {
    acq_name_h = c->para[2];
  }
  acq_name_h1 = (char *)malloc( strlen( acq_name_h) + 16);
  strcpy( acq_name_h1, acq_name_h);
  strcat( acq_name_h1, "_acq.his");
  acq_name_h2 = (char *)malloc( strlen( acq_name_h) + 16);
  strcpy( acq_name_h2, acq_name_h);
  strcat( acq_name_h2, "_avg.his");
  acq_name_h3 = (char *)malloc( strlen( acq_name_h) + 16);
  strcpy( acq_name_h3, acq_name_h);
  strcat( acq_name_h3, "_zoom.his");
  acq_name_h4 = (char *)malloc( strlen( acq_name_h) + 16);
  strcpy( acq_name_h4, acq_name_h);
  strcat( acq_name_h4, "_raw.his");

  acq_file_h1 = fopen( acq_name_h1, "w");
  if( !acq_file_h1)
  {
    printf("cannot create acquisition file %s\n", acq_name_h1);
    free( acq_name_h1);
    return( -1);
  }
  acq_file_h2 = fopen( acq_name_h2, "w");
  if( !acq_file_h2)
  {
    printf("cannot create average file %s\n", acq_name_h2);
    fclose( acq_file_h1);
    free( acq_name_h1);
    free( acq_name_h2);
    return( -1);
  }
  acq_file_h3 = fopen( acq_name_h3, "w");
  if( !acq_file_h3)
  {
    printf("cannot create zoom file %s\n", acq_name_h3);
    fclose( acq_file_h1);
    fclose( acq_file_h2);
    free( acq_name_h1);
    free( acq_name_h2);
    free( acq_name_h3);
    return( -1);
  }
  acq_file_h4 = fopen( acq_name_h4, "w");
  if( !acq_file_h4)
  {
    printf("cannot create raw file %s\n", acq_name_h4);
    fclose( acq_file_h1);
    fclose( acq_file_h2);
    fclose( acq_file_h3);
    free( acq_name_h1);
    free( acq_name_h2);
    free( acq_name_h3);
    free( acq_name_h4);
    return( -1);
  }

  acq_buf = adc3112_map_usr( &adc3112_mas_map, 0x100000, 0x200000, fmc);
  avg_buf = acq_buf + 0x100000;
  zoom_buf = acq_buf + 0x110000;

  num_samples = 400;
  num_frames = 480;
  if( fmc == 1)
  {
    csr = ACQ_REG_BASE_1;
  }
  else
  {
    csr = ACQ_REG_BASE_2;
  }
  num_samples = tscext_csr_rd( csr+4);
  printf("CSR = %08x - %08x\n", csr+4, num_samples);
  num_frames = (num_samples >> 12) & 0x1ff;
  num_samples &= 0x1ff;

  printf("saving raw data from offset %x [512*512] to histogram file %s\n", 0x100000, acq_name_h1);
  n = 0;
  for( i = 0; i < 512; i++)
  {
    for( j = 0; j < 512; j++)
    {
      short data;
      int idx;

      idx = ((512*4)+j)*sizeof(short);
      data = tsc_swap_16(*(short *)&acq_buf[idx])/16;
      fprintf( acq_file_h4, "%d, %d\n", n, data);
      n++;
    }
  }
  printf("saving acquision data from offset %x [%d*%d] to histogram file %s\n", 0x100000, num_samples, num_frames, acq_name_h1);
  n = 0;
  for( i = 0; i < num_frames; i++)
  {
    for( j = 0; j < num_samples; j++)
    {
      short data;
      int idx;

      idx = ((512*j)+i)*sizeof(short);
      data = tsc_swap_16(*(short *)&acq_buf[idx])/16;
      fprintf( acq_file_h1, "%d, %d\n", n, data);
      n++;
    }
  }
  printf("saving average data from offset %x [32*%d] to histogram file %s\n", 0x200000, num_frames, acq_name_h2);
  n = 0;
  for( i = 0; i < 32; i++)
  {
    for( j = 0; j < num_frames; j++)
    {
      int data;
      int idx;

      idx = ((512*i)+j)*sizeof(int);
      data = tsc_swap_32(*(int *)&avg_buf[idx]);
      fprintf( acq_file_h2, "%d, %d\n", n, data/num_samples);
      n++;
    }
  }
  printf("saving zoomed data from offset %x [8*%d] to histogram file %s\n", 0x210000, num_frames, acq_name_h2);
  n = 0;
  for( i = 0; i < 4; i++)
  {
    for( j = 0; j < num_samples; j++)
    {
      short data;
      int idx;

      idx = ((4*j)+i)*sizeof(short);
      data = tsc_swap_16(*(short *)&zoom_buf[idx])/16;
      fprintf( acq_file_h3, "%d, %d\n", n, data);
      n++;
    }
  }
  for( i = 0; i < 4; i++)
  {
    for( j = 0; j < num_samples; j++)
    {
      short data;
      int idx;

      idx = ((4*j)+i)*sizeof(short);
      data = tsc_swap_16(*(short *)&zoom_buf[0x1000+idx])/16;
      fprintf( acq_file_h3, "%d, %d\n", n, data);
      n++;
    }
  }

  fclose( acq_file_h1);
  fclose( acq_file_h2);
  fclose( acq_file_h3);
  fclose( acq_file_h4);
  free( acq_name_h1);
  free( acq_name_h2);
  free( acq_name_h3);
  free( acq_name_h4);
  adc3112_unmap_usr( &adc3112_mas_map, acq_buf);
  return(0);
}

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * Function name : adc3112_acq
 * Prototype     : int
 * Parameters    : cli command parameter structure, identifier, fmc, size, check
 * Return        : success/error
 *----------------------------------------------------------------------------
 * Description   : adc3112 acq command
 *
 *++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

#define ADC3112_CHAN1 0x1
#define ADC3112_CHAN2 0x2
#define ADC3112_CHAN3 0x4
#define ADC3112_CHAN4 0x8

int
adc3112_acq( struct cli_cmd_para *c,
	     int idx,
	     int fmc,
	     int size,
	     int chan,
	     int no_trig,
	     int start)
{
  int trig;
  int i, tmo, cnt;
  char *adc_buf, *p;
  char *acq_name_h, *acq_name_d;
  char *acq_name_h1, *acq_name_h2, *acq_name_h3, *acq_name_h4;
  char *acq_name_d1, *acq_name_d2, *acq_name_d3, *acq_name_d4;
  FILE *acq_file_h1, *acq_file_h2, *acq_file_h3, *acq_file_h4;
  FILE *acq_file_d1, *acq_file_d2, *acq_file_d3, *acq_file_d4;
  char *default_name = "acq_file";
  struct adc3112_acq_res acq_res;
  int file_his, file_dat;
  int reg_base;

  acq_name_h = NULL;
  acq_name_d = NULL;
  acq_name_h1 = NULL;
  acq_name_d1 = NULL;
  acq_name_h2 = NULL;
  acq_name_d2 = NULL;
  acq_name_h3 = NULL;
  acq_name_d3 = NULL;
  acq_name_h4 = NULL;
  acq_name_d4 = NULL;
  trig = 0;
  file_his = 0;
  file_dat = 0;
  printf("parameter count: %ld\n", c->cnt);
  cnt = c->cnt - 2;
  i = 2;
  while( cnt--)
  {
    p =  c->para[i++];
    printf("Setting wave parameters %d: %s\n", i, p);
    if( p[0] == 't')
    {
      sscanf( p, "t:%x", &trig);
    }
    if(( p[0] == 'f') || ( p[0] == 'h'))
    {
      file_his = 1;
      acq_name_h = &p[2];
    }
    if( p[0] == 'd')
    {
      file_dat = 1;
      acq_name_d = &p[2];
    }
  }
  if( file_his)
  {
    if( !acq_name_h)
    {
      acq_name_h = default_name;
    }
    if( chan & ADC3112_CHAN1)
    {
      acq_name_h1 = (char *)malloc( strlen( acq_name_h) + 8);
      strcpy( acq_name_h1, acq_name_h);
      strcat( acq_name_h1, "_0.his");
      acq_file_h1 = fopen( acq_name_h1, "w");
      if( !acq_file_h1)
      {
        printf("cannot create acquisition file %s\n", acq_name_h1);
        free( acq_name_h1);
        return( -1);
      }
    }
    if( chan & ADC3112_CHAN2)
    {
      acq_name_h2 = (char *)malloc( strlen( acq_name_h) + 8);
      strcpy( acq_name_h2, acq_name_h);
      strcat( acq_name_h2, "_1.his");
      acq_file_h2 = fopen( acq_name_h2, "w");
      if( !acq_file_h2)
      {
        printf("cannot create acquisition file %s\n", acq_name_h2);
        fclose( acq_file_h1);
        free( acq_name_h1);
        free( acq_name_h2);
        return( -1);
      }
    }
    if( chan & ADC3112_CHAN3)
    {
      acq_name_h3 = (char *)malloc( strlen( acq_name_h) + 8);
      strcpy( acq_name_h3, acq_name_h);
      strcat( acq_name_h3, "_2.his");
      acq_file_h3 = fopen( acq_name_h3, "w");
      if( !acq_file_h3)
      {
        printf("cannot create acquisition file %s\n", acq_name_h3);
        fclose( acq_file_h1);
        fclose( acq_file_h2);
        free( acq_name_h1);
        free( acq_name_h2);
        free( acq_name_h3);
        return( -1);
      }  
    }
    if( chan & ADC3112_CHAN4)
    {
      acq_name_h4 = (char *)malloc( strlen( acq_name_h) + 8);
      strcpy( acq_name_h4, acq_name_h);
      strcat( acq_name_h4, "_3.his");
      acq_file_h4 = fopen( acq_name_h4, "w");
      if( !acq_file_h4)
      {
        printf("cannot create acquisition file %s\n", acq_name_h4);
        fclose( acq_file_h1);
        fclose( acq_file_h2);
        fclose( acq_file_h3);
        free( acq_name_h1);
        free( acq_name_h2);
        free( acq_name_h3);
        free( acq_name_h4);
        return( -1);
      }
    }
  }
  if( file_dat)
  {
    if( !acq_name_d)
    {
      acq_name_d = default_name;
    }
    if( chan & ADC3112_CHAN1)
    {
      acq_name_d1 = (char *)malloc( strlen( acq_name_d) + 8);
      strcpy( acq_name_d1, acq_name_d);
      strcat( acq_name_d1, "_0.csv");
      acq_file_d1 = fopen( acq_name_d1, "w");
      if( !acq_file_d1)
      {
        printf("cannot create acquisition file %s\n", acq_name_d1);
        free( acq_name_d1);
        return( -1);
      }
    }
    if( chan & ADC3112_CHAN2)
    {
      acq_name_d2 = (char *)malloc( strlen( acq_name_d) + 8);
      strcpy( acq_name_d2, acq_name_d);
      strcat( acq_name_d2, "_1.csv");
      acq_file_d2 = fopen( acq_name_d2, "w");
      if( !acq_file_d2)
      {
        printf("cannot create acquisition file %s\n", acq_name_d2);
        fclose( acq_file_d1);
        free( acq_name_d1);
        free( acq_name_d2);
        return( -1);
      }
    }
    if( chan & ADC3112_CHAN3)
    {
      acq_name_d3 = (char *)malloc( strlen( acq_name_d) + 8);
      strcpy( acq_name_d3, acq_name_d);
      strcat( acq_name_d3, "_2.csv");
      acq_file_d3 = fopen( acq_name_d3, "w");
      if( !acq_file_d3)
      {
        printf("cannot create acquisition file %s\n", acq_name_d3);
        fclose( acq_file_d1);
        fclose( acq_file_d2);
        free( acq_name_d1);
        free( acq_name_d2);
        free( acq_name_d3);
        return( -1);
      }
    }
    if( chan & ADC3112_CHAN4)
    {
      acq_name_d4 = (char *)malloc( strlen( acq_name_d) + 8);
      strcpy( acq_name_d4, acq_name_d);
      strcat( acq_name_d4, "_3.csv");
      acq_file_d4 = fopen( acq_name_d4, "w");
      if( !acq_file_d4)
      {
        printf("cannot create acquisition file %s\n", acq_name_d4);
        fclose( acq_file_d1);
        fclose( acq_file_d2);
        fclose( acq_file_d3);
        free( acq_name_d1);
        free( acq_name_d2);
        free( acq_name_d3);
        free( acq_name_d4);
        return( -1);
      }
    }

  }

  adc_buf = adc3112_map_usr( &adc3112_mas_map, 0x100000, 0x100000, fmc);
  if( fmc == 2)
  {
    reg_base =ACQ_REG_BASE_B;
  }
  else 
  {
    reg_base =ACQ_REG_BASE_A;
  }

  if( !no_trig)
  {
    printf("Trig data acquisition...\n");
    tmo = adc3112_trig_acq( reg_base, trig);
  }

  if( tmo)
  {
    short *buf0, *buf1, *buf2, *buf3;
    int n, data, offset;

    printf("Save acquired data...\n");
    start = 0x2000;
    if( chan & ADC3112_CHAN1)
    {
      printf("Loding channel 0...");
      buf0 = ( short *)malloc(size);
      n = 0;
      offset = 0x00000;
      for( i = offset+start; i < size; i += 4)
      {
        data = *(int *)&adc_buf[i];
        buf0[n+1] = tsc_swap_16((short)data)/16;
        buf0[n] = tsc_swap_16((short)(data >> 16))/16;
        if( file_his)
        {
          fprintf( acq_file_h1, "%d, %d\n", n, buf0[n]);
          fprintf( acq_file_h1, "%d, %d\n", n+1, buf0[n+1]);
        }
        if( file_dat)
        {
          fprintf( acq_file_d1, "%d\n", buf0[n]);
          fprintf( acq_file_d1, "%d\n", buf0[n+1]);
        }
        n += 2;
      }
      printf("\n");
    }
    if( chan & ADC3112_CHAN2)
    {
      printf("Loding channel 1...");
      buf1 = ( short *)malloc(size);
      n = 0;
      offset = 0x10000;
      for( i = offset+start; i < offset+size; i += 4)
      {
        data = *(int *)&adc_buf[i];
        buf1[n+1] = tsc_swap_16((short)data)/16;
        buf1[n] = tsc_swap_16((short)(data >> 16))/16;;
        if( file_his)
        {
          fprintf( acq_file_h2, "%d, %d\n", n, buf1[n]);
          fprintf( acq_file_h2, "%d, %d\n", n+1, buf1[n+1]);
        }
        if( file_dat)
        {
          fprintf( acq_file_d2, "%d\n", buf1[n]);
          fprintf( acq_file_d2, "%d\n", buf1[n+1]);
        }
        n += 2;
      }
      printf("\n");
    }
    if( chan & ADC3112_CHAN3)
    {
      printf("Loding channel 2...");
      buf2 = ( short *)malloc(size);
      n = 0;
      offset = 0x20000;
      for( i = offset+start; i < offset+size; i += 4)
      {
        data = *(int *)&adc_buf[i];
        buf2[n+1] = tsc_swap_16((short)data)/16;
        buf2[n] = tsc_swap_16((short)(data >> 16))/16;
        if( file_his)
        {
          fprintf( acq_file_h3, "%d, %d\n", n, buf2[n]);
          fprintf( acq_file_h3, "%d, %d\n", n+1, buf2[n+1]);
        }
        if( file_dat)
        {
          fprintf( acq_file_d3, "%d\n", buf2[n]);
          fprintf( acq_file_d3, "%d\n", buf2[n+1]);
        }
        n += 2;
      }
      printf("\n");
    }
    if( chan & ADC3112_CHAN4)
    {
      printf("Loding channel 3...");
      buf3 = ( short *)malloc(size);
      n = 0;
      offset = 0x30000;
      for( i = offset+start; i < offset+size; i += 4)
      {
        data = *(int *)&adc_buf[i];
        buf3[n+1] = tsc_swap_16((short)data)/16;
        buf3[n] = tsc_swap_16((short)(data >> 16))/16;
        if( file_his)
        {
          fprintf( acq_file_h4, "%d, %d\n", n, buf3[n]);
          fprintf( acq_file_h4, "%d, %d\n", n+1, buf3[n+1]);
        }
        if( file_dat)
        {
          fprintf( acq_file_d4, "%d\n", buf3[n]);
          fprintf( acq_file_d4, "%d\n", buf3[n+1]);
        }
        n += 2;
      }
      printf("\n");
    }
    size = size - start;
    if( chan & ADC3112_CHAN1)
    {
      printf("Processing data channel 0...\n");
      adc3112_acq_res( &acq_res, buf0, size, 0);
      free( (void *)buf0);
    }
    if( chan & ADC3112_CHAN2)
    {
      printf("Processing data channel 1...\n");
      adc3112_acq_res( &acq_res, buf1, size, 1);
      free( (void *)buf1);
    }
    if( chan & ADC3112_CHAN3)
    {
      printf("Processing data channel 2...\n");
      adc3112_acq_res( &acq_res, buf2, size, 2);
      free( (void *)buf2);
    }
    if( chan & ADC3112_CHAN4)
    {
      printf("Processing data channel 3...\n");
      adc3112_acq_res( &acq_res, buf3, size, 3);
      free( (void *)buf3);
    }
    printf(" -> done\n");
    if( chan & ADC3112_CHAN1)
    {
      printf("Channel 0 : %f +/- %f [%d (0x%x) : %d (0x%x) ]\n", 
	     acq_res.chan[0].mean, acq_res.chan[0].sig, acq_res.chan[0].min, 2*acq_res.chan[0].off_min, acq_res.chan[0].max, 2*acq_res.chan[0].off_max);
    }
    if( chan & ADC3112_CHAN2)
    {
      printf("Channel 1 : %f +/- %f [%d (0x%x) : %d (0x%x)]\n", 
 	     acq_res.chan[1].mean, acq_res.chan[1].sig, acq_res.chan[1].min, 2*acq_res.chan[1].off_min, acq_res.chan[1].max, 2*acq_res.chan[1].off_max);
    }
    if( chan & ADC3112_CHAN3)
    {
      printf("Channel 2 : %f +/- %f [%d (0x%x) : %d (0x%x)]\n", 
	     acq_res.chan[2].mean, acq_res.chan[2].sig, acq_res.chan[2].min, 2*acq_res.chan[2].off_min, acq_res.chan[2].max, 2*acq_res.chan[2].off_max);
    }
    if( chan & ADC3112_CHAN4)
    {
      printf("Channel 3 : %f +/- %f [%d (0x%x) : %d (0x%x)]\n", 
	     acq_res.chan[3].mean, acq_res.chan[3].sig, acq_res.chan[3].min, 2*acq_res.chan[3].off_min, acq_res.chan[3].max, 2*acq_res.chan[3].off_max);
    }
  }
  else
  {
    printf("Acquistion timeout...\n");
  }
  printf("\n");

  printf("Transfering histogramming files to host...");
  fflush( stdout);
  if( file_his)
  {
    if( chan & ADC3112_CHAN1) fclose( acq_file_h1);
    if( chan & ADC3112_CHAN2) fclose( acq_file_h2);
    if( chan & ADC3112_CHAN3) fclose( acq_file_h3);
    if( chan & ADC3112_CHAN4) fclose( acq_file_h4);
    if( chan & ADC3112_CHAN1) free( acq_name_h1);
    if( chan & ADC3112_CHAN2) free( acq_name_h2);
    if( chan & ADC3112_CHAN3) free( acq_name_h3);
    if( chan & ADC3112_CHAN4) free( acq_name_h4);
  }
  if( file_dat)
  {
    if( chan & ADC3112_CHAN1) fclose( acq_file_d1);
    if( chan & ADC3112_CHAN2) fclose( acq_file_d2);
    if( chan & ADC3112_CHAN3) fclose( acq_file_d3);
    if( chan & ADC3112_CHAN4) fclose( acq_file_d4);
    if( chan & ADC3112_CHAN1) free( acq_name_d1);
    if( chan & ADC3112_CHAN2) free( acq_name_d2);
    if( chan & ADC3112_CHAN3) free( acq_name_d3);
    if( chan & ADC3112_CHAN4) free( acq_name_d4);
  }
  printf(" -> done\n");
  adc3112_unmap_usr( &adc3112_mas_map, adc_buf);
  return(0);
}

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * Function name : adc3112_itl_buf
 * Prototype     : 
 * Parameters    : 
 * Return        : 
 *----------------------------------------------------------------------------
 * Description   : 
 *
 *++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

int
adc3112_itl_buf( struct cli_cmd_para *c,
		      int idx,
		      int fmc,
	 	      int chan)
{
  int i, cnt, size;
  short *buf0, *buf1, *buf;
  short lev0[32], lev1[32];
  int i0, i1;
  int n, data, offset, delta, start;
  char *adc_buf, *p;
  char *acq_name_h, *acq_name_d;
  char *acq_name_h1;
  char *acq_name_d1;
  FILE *acq_file_h1;
  FILE *acq_file_d1;
  char *acq_name_h2;
  char *acq_name_d2;
  FILE *acq_file_h2;
  FILE *acq_file_d2;
  char *default_name = "acq_file";
  struct adc3112_acq_res acq_res;
  int file_his, file_dat;

  acq_name_h = NULL;
  acq_name_d = NULL;
  file_his = 0;
  file_dat = 0;
  printf("parameter count: %ld\n", c->cnt);
  cnt = c->cnt - 2;
  i = 2;
  while( cnt--)
  {
    p =  c->para[i++];
    if(( p[0] == 'f') || ( p[0] == 'h'))
    {
      file_his = 1;
      acq_name_h = &p[2];
    }
    if( p[0] == 'd')
    {
      file_dat = 1;
      acq_name_d = &p[2];
    }
  }
  if( file_his)
  {
    if( !acq_name_h)
    {
      acq_name_h = default_name;
    }
    acq_name_h1 = (char *)malloc( strlen( acq_name_h) + 10);
    strcpy( acq_name_h1, acq_name_h);
    if( chan == 0) strcat( acq_name_h1, "_o_0.his");
    else strcat( acq_name_h1, "_o_1.his");
    acq_file_h1 = fopen( acq_name_h1, "w");
    if( !acq_file_h1)
    {
      printf("cannot create acquisition file %s\n", acq_name_h1);
      free( acq_name_h1);
      return( -1);
    }
    acq_name_h2 = (char *)malloc( strlen( acq_name_h) + 10);
    strcpy( acq_name_h2, acq_name_h);
    if( chan == 0) strcat( acq_name_h2, "_e_0.his");
    else strcat( acq_name_h2, "_e_1.his");
    acq_file_h2 = fopen( acq_name_h2, "w");
    if( !acq_file_h2)
    {
      printf("cannot create acquisition file %s\n", acq_name_h2);
      fclose( acq_file_h1);
      free( acq_name_h1);
      free( acq_name_h2);
      return( -1);
    }
  }
  if( file_dat)
  {
    if( !acq_name_d)
    {
      acq_name_d = default_name;
    }
    acq_name_d1 = (char *)malloc( strlen( acq_name_d) + 10);
    strcpy( acq_name_d1, acq_name_d);
    if( chan == 0) strcat( acq_name_d1, "_o_0.csv");
    else strcat( acq_name_d1, "_o_1.csv");
    acq_file_d1 = fopen( acq_name_d1, "w");
    if( !acq_file_d1)
    {
      printf("cannot create acquisition file %s\n", acq_name_d1);
      free( acq_name_d1);
      return( -1);
    }
    acq_name_d2 = (char *)malloc( strlen( acq_name_d) + 10);
    strcpy( acq_name_d2, acq_name_d);
    if( chan == 0) strcat( acq_name_d2, "_e_0.csv");
    else strcat( acq_name_d2, "_e_1.csv");
    acq_file_d2 = fopen( acq_name_d2, "w");
    if( !acq_file_d2)
    {
      printf("cannot create acquisition file %s\n", acq_name_d2);
      fclose(acq_file_d1);
      free( acq_name_d1);
      free( acq_name_d2);
      return( -1);
    }
  }

  adc_buf = adc3112_map_usr( &adc3112_mas_map, 0x100000, 0x100000, fmc);

  printf("Save acquired data...\n");
  start = 0x2000;
  size = 0xa000;
  printf("Loding channel %d...", chan);
  buf0 = ( short *)malloc(size+32);
  buf1 = ( short *)malloc(size+32);
  buf = ( short *)malloc(2*size);
  if( chan == 0) offset = 0x00000;
  else offset = 0x10000;
  delta = 0x20000;

  n = 0;
  for( i = offset+start; i < offset+start + 64; i += 4)
  {
    data = *(int *)&adc_buf[i];
    lev0[n+1] = tsc_swap_16((short)data)&1;
    lev0[n] = tsc_swap_16((short)(data >> 16))&1;
    data = *(int *)&adc_buf[delta+i];
    lev1[n+1] = tsc_swap_16((short)data)&1;
    lev1[n] = tsc_swap_16((short)(data >> 16))&1;
    n += 2;
  }
  for( i = 1; i < 16; i++)
  {
    if( lev0[i-1] < lev0[i]) break;
  }
  i0 = i;
  for( i = 1; i < 16; i++)
  {
    if( lev1[i-1] < lev1[i]) break;
  }
  i1 = i;

  n = 0;
  for( i = offset+start; i < offset + size + 64; i += 4)
  {
    data = *(int *)&adc_buf[i+i0];
    buf0[n+1] = tsc_swap_16((short)data)/16;
    buf0[n] = tsc_swap_16((short)(data >> 16))/16;
    data = *(int *)&adc_buf[delta+i+i1];
    buf1[n+1] = tsc_swap_16((short)data)/16;
    buf1[n] = tsc_swap_16((short)(data >> 16))/16;
    n += 2;
  }
  for( i = 0; i < size-start; i += 2)
  {
    buf[i]   = buf0[i0+i/2];
    buf[i+1] = buf1[i1+i/2];
    if( file_his)
    {
      fprintf( acq_file_h1, "%d, %d\n", i, buf[i]);
      fprintf( acq_file_h1, "%d, %d\n", i+1, buf[i+1]);
      fprintf( acq_file_h2, "%d, %d\n", i+1, buf[i+1]);
      fprintf( acq_file_h2, "%d, %d\n", i, buf[i]);
    }
    if( file_dat)
    {
      fprintf( acq_file_d1, "%d\n", buf[i]);
      fprintf( acq_file_d1, "%d\n", buf[i+1]);
      fprintf( acq_file_d2, "%d\n", buf[i+1]);
      fprintf( acq_file_d2, "%d\n", buf[i]);
   }
  }
  free( (void *)buf0);
  free( (void *)buf1);
  printf("\n");
  size = 2*(size - start);
  printf("Processing data channel %d...\n", chan);
  adc3112_acq_res( &acq_res, buf, size, 0);
  free( (void *)buf);
  printf(" -> done\n");
  printf("Channel %d : %f +/- %f [%d (0x%x) : %d (0x%x) ]\n", chan,
	 acq_res.chan[chan].mean, acq_res.chan[chan].sig, acq_res.chan[chan].min, 2*acq_res.chan[chan].off_min, acq_res.chan[chan].max, 2*acq_res.chan[chan].off_max);
  printf("\n");

  printf("Transfering histogramming files to host...");
  fflush( stdout);
  if( file_his)
  {
    fclose( acq_file_h1);
    fclose( acq_file_h2);
    free( acq_name_h1);
    free( acq_name_h2);
  }
  if( file_dat)
  {
    fclose( acq_file_d1);
    fclose( acq_file_d2);
    free( acq_name_d1);
    free( acq_name_d2);
  }
  printf(" -> done\n");
  adc3112_unmap_usr( &adc3112_mas_map, adc_buf);
  return(0);
}


/*
 *
 * ADC calibration
 *
 */


/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * Function name : adc3112_calib_init
 * Prototype     : 
 * Parameters    : 
 * Return        : 
 *----------------------------------------------------------------------------
 * Description   : 
 *
 *++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

int
adc3112_calib_init( struct adc3112_calib_ctl *cc,
		    int reg_ttim_cal,
		    int reg_acq_csr,
		    int reg_adc_serial,
		    char *data_buf)

{
  int bit, step, chan;
  int reg_gain;

  cc->reg_ttim_cal = reg_ttim_cal;
  cc->reg_acq_csr = reg_acq_csr;
  cc->reg_adc_serial = reg_adc_serial;
  for( chan = 0; chan < ADC_NUM_CHAN; chan++)
  {
    cc->chan[chan].chan = chan;
    cc->chan[chan].delay = 0;
    for( bit = 0; bit < CAL_BIT_NUM; bit++)
    {
      cc->chan[chan].ttim[bit] = 0;
      for( step = 0; step < CAL_STEP_NUM; step++)
      {
        cc->chan[chan].err_cnt[bit][step] = 0;
      }
    }
    cc->chan[chan].data_buf = data_buf + ADC_OFF_CHAN_1*chan;
    /* reset ADC gain to default value */
    reg_gain = reg_ttim_cal - 0x10 + (4*chan);
    tscext_csr_wr( reg_gain, 0x4000);
  }
  return(0);
}

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * Function name : adc3112_calib_get_ttim
 * Prototype     : 
 * Parameters    : 
 * Return        : 
 *----------------------------------------------------------------------------
 * Description   : 
 *
 *++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

int
adc3112_calib_get_ttim( struct adc3112_calib_ctl *cc,
			 int chan)
{
  int bit, data;

  for( bit = 0; bit < 12; bit++)
  {
    data = (bit<<20) | (chan << 26);
    tscext_csr_wr( cc->reg_ttim_cal, data);
    cc->chan[chan].ttim[bit+4] =  tscext_csr_rd( cc->reg_ttim_cal);
  }
  data = (0xc00000) | (chan << 26);
  tscext_csr_wr( cc->reg_ttim_cal, data);
  cc->chan[chan].ttim[0] =  tscext_csr_rd( cc->reg_ttim_cal);
  return(0);
}

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * Function name : adc3112_calib_set_default
 * Prototype     : 
 * Parameters    : 
 * Return        : 
 *----------------------------------------------------------------------------
 * Description   : 
 *
 *++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

int
adc3112_calib_set_default( struct adc3112_calib_ctl *cc,
			   int chan)
{
  int data;

  /* load defaault delay */
  data = 0x40000000 | (chan << 26);
  tscext_csr_wr( cc->reg_ttim_cal, data); 
  tscext_csr_rd( cc->reg_ttim_cal);
  cc->chan[chan].delay = 0;
  /* update ttim array */
  adc3112_calib_get_ttim( cc, chan);

  return(0);
}


/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * Function name : adc3112_calib_inc_delay
 * Prototype     : 
 * Parameters    : 
 * Return        : 
 *----------------------------------------------------------------------------
 * Description   : 
 *
 *++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

int
adc3112_calib_inc_delay( struct adc3112_calib_ctl *cc,
			 int set,
			 int chan)
{
  int bit, data;

  if( cc->chan[chan].delay >= (CAL_STEP_NUM/2))
  {
    return(-1);
  }
  if( set & CAL_ALL_BIT)
  {
    //data = 0xa0f00000 | (chan << 26);
    //tscext_csr_wr( cc->reg_ttim_cal, data); 
    cc->chan[chan].delay += 1;
    for( bit = 0; bit < 12; bit++)
    {
      data = 0xa0000000 | (bit<<20) | (chan << 26);
      tscext_csr_wr( cc->reg_ttim_cal, data);
      cc->chan[chan].ttim[bit+4] =  tscext_csr_rd( cc->reg_ttim_cal);
    }
    data = 0xa0c00000 | (chan << 26);
    tscext_csr_wr( cc->reg_ttim_cal, data);
    cc->chan[chan].ttim[0] =  tscext_csr_rd( cc->reg_ttim_cal);
  }
  else
  {
    //printf("in adc3112_calib_dec_delay( %x, %d)\n", set, chan);
    for( bit = 0; bit < 12; bit++)
    {
      if( set & (1<<bit))
      {
	//printf("increment bit %d\n", bit);
	data = 0xa0000000 | (bit<<20) | (chan << 26);
	tscext_csr_wr( cc->reg_ttim_cal, data);
	cc->chan[chan].ttim[bit+4] =  tscext_csr_rd( cc->reg_ttim_cal);
      }
    }
    if( set & 0x1000)
    {
      //printf("decrement bit 12\n");
      data = 0xa0c00000 | (chan << 26);
      tscext_csr_wr( cc->reg_ttim_cal, data);
      cc->chan[chan].ttim[0] =  tscext_csr_rd( cc->reg_ttim_cal);
    }
  }
  return(0);
}

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * Function name : adc3112_calib_dec_delay
 * Prototype     : 
 * Parameters    : 
 * Return        : 
 *----------------------------------------------------------------------------
 * Description   : 
 *
 *++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

int
adc3112_calib_dec_delay( struct adc3112_calib_ctl *cc,
			 int set,
			 int chan)
{
  int bit, data;

  if( cc->chan[chan].delay <= -(CAL_STEP_NUM/2))
  {
    return(-1);
  }
  if( set & CAL_ALL_BIT)
  {
    //data = 0x80f00000 | (chan << 26);
    //tscext_csr_wr( cc->reg_ttim_cal, data); 
    cc->chan[chan].delay -= 1;
    for( bit = 0; bit < 12; bit++)
    {
      data = 0x80000000 | (bit<<20) | (chan << 26);
      tscext_csr_wr( cc->reg_ttim_cal, data);
      cc->chan[chan].ttim[bit+4] =  tscext_csr_rd( cc->reg_ttim_cal);
    }
    data = 0x80c00000 | (chan << 26);
    tscext_csr_wr( cc->reg_ttim_cal, data);
    cc->chan[chan].ttim[0] =  tscext_csr_rd( cc->reg_ttim_cal);
  }
  else
  {
    //printf("in adc3112_calib_dec_delay( %x, %d)\n", set, chan);
    for( bit = 0; bit < 12; bit++)
    {
      if( set & (1<<bit))
      {
	//printf("decrement bit %d\n", bit);
	data = 0x80000000 | (bit<<20) | (chan << 26);
	tscext_csr_wr( cc->reg_ttim_cal, data);
	cc->chan[chan].ttim[bit+4] =  tscext_csr_rd( cc->reg_ttim_cal);
      }
    }
    if( set & 0x1000)
    {
      //printf("decrement bit 12\n");
      data = 0x80c00000 | (chan << 26);
      tscext_csr_wr( cc->reg_ttim_cal, data);
      cc->chan[chan].ttim[0] =  tscext_csr_rd( cc->reg_ttim_cal);
    }
  }
  return(0);
}


/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * Function name : adc3112_ads_read
 * Prototype     : void
 * Parameters    : register, data, channel, fmc
 * Return        : int
 *----------------------------------------------------------------------------
 * Description   : read from ads
 *
 *++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

int
adc3112_ads_read( int csr,
		  int chan,
	 	  int reg)
{
  int cmd, tmo, data;

  cmd =  0x81000000 | ((chan&2)<<15) | reg;
  tmo = 1000;
  tscext_csr_wr( csr, cmd);
  while( --tmo)
  {
    if( !(tscext_csr_rd( csr) & 0x80000000)) break;
  }
  data = tscext_csr_rd( csr + 4);
  //printf("cmd = %08x - data = %08x\n", cmd, data);
  if( !tmo)
  {
    printf("adc3112_ads_read() -> timeout...\n");
    return(-1);
  }

  return( data);
}

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * Function name : adc3112_ads_write
 * Prototype     : void
 * Parameters    : register, data, channel, fmc
 * Return        : int
 *----------------------------------------------------------------------------
 * Description   : write to ads
 *
 *++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

int
adc3112_ads_write( int csr,
		   int chan,
		   int reg,
		   int data)
{
  int cmd, tmo;

  cmd =  0xc1000000 | ((chan&2)<<15) | reg;
  //printf("cmd = %08x - data = %08x\n", cmd, data);
  tscext_csr_wr( csr + 4, data);          /* load data */
  tscext_csr_rd( csr+4);                  /* flush data */
  tscext_csr_wr( csr, cmd);               /* write data */
  tmo = 1000;
  while( --tmo)
  {
    if( !(tscext_csr_rd( csr) & 0x80000000)) break; /* check end of write cycle */
  }
  if( !tmo)
  {
    printf("adc3112_ads_write() -> timeout...\n");
    return(-1);
  }

  return(0);
}

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * Function name : adc3112_calib_data_check
 * Prototype     : 
 * Parameters    : 
 * Return        : 
 *----------------------------------------------------------------------------
 * Description   : 
 *
 *++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

unsigned short
adc3112_calib_data_check( struct adc3112_calib_ctl *cc,
			  int chan,
			  unsigned short cmp0,
			  unsigned short cmp1)
{
  unsigned short res, tmp, dat0, dat1;
  int i, bit;
  char *p;

  res = 0;
  if( !(chan & 1)) /* hardware add 1 to data for chan 0 and 2 */
  {
    cmp0 += 0x10;
    cmp1 += 0x10;
  }
  for( bit = 0; bit < CAL_BIT_NUM; bit++)
  {
    cc->chan[chan].err_cnt[bit][(CAL_STEP_NUM/2) +  cc->chan[chan].delay] = 0;
  }
  p = cc->chan[chan].data_buf;
  //dat0 = (unsigned short)tsc_swap_16(*(short *)&p[2]);
  //dat1 = (unsigned short)tsc_swap_16(*(short *)&p[0]);
  //printf("chan %d compare : %04x : %04x - %04x : %04x\n", chan, cmp1, cmp0, dat1, dat0);
  for( i = 0; i < (ADC_NUM_SAMPLES*ADC_SAMPLE_SIZE); i += sizeof(int))
  {
    dat0 = (unsigned short)tsc_swap_16(*(short *)&p[i+2]);
    dat1 = (unsigned short)tsc_swap_16(*(short *)&p[i]);
    tmp = cmp0 ^ dat0;
    res |= tmp;
    for( bit = 0; bit < CAL_BIT_NUM; bit++)
    {
      if( tmp & (1<<bit))
      {
        cc->chan[chan].err_cnt[bit][(CAL_STEP_NUM/2) +  cc->chan[chan].delay] += 1;
      }
    }
    tmp = cmp1 ^ dat1;
    res |= tmp;
    for( bit = 0; bit < CAL_BIT_NUM; bit++)
    {
      if( tmp & (1<<bit))
      {
        cc->chan[chan].err_cnt[bit][(CAL_STEP_NUM/2) +  cc->chan[chan].delay] += 1;
      }
    }
  }
  return( res);
}


/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * Function name : adc3112_calib_config
 * Prototype     : 
 * Parameters    : 
 * Return        : 
 *----------------------------------------------------------------------------
 * Description   : 
 *
 *++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

int
adc3112_calib_config( int fmc,
		      uint mode)
{
  int data;

  /* ADS5409_01 DAQ calibration with 0x555/0xAAA */
  adc3112_spi_write( fmc, ADC_ADS01_CMD, 0xe, 0x0000);
  adc3112_spi_write( fmc, ADC_ADS01_CMD, 0xf, 0x0000);
  adc3112_spi_write( fmc, ADC_ADS01_CMD, 0x1, 0x0000);

  data = 0x8000 | ((mode & 0xfff00000) >> 18);
  adc3112_spi_write( fmc, ADC_ADS01_CMD, 0x3c, data);
  data = ((mode & 0xfff0) >> 2);
  adc3112_spi_write( fmc, ADC_ADS01_CMD, 0x3d, data);
  data = 0x8000 | ((mode & 0xfff00000) >> 18);
  adc3112_spi_write( fmc, ADC_ADS01_CMD, 0x3e, data);

  /* ADS5409_23 DAQ calibration with 0x555/0xAAA */
  adc3112_spi_write( fmc, ADC_ADS23_CMD, 0xe, 0x0000);
  adc3112_spi_write( fmc, ADC_ADS23_CMD, 0xf, 0x0000);
  adc3112_spi_write( fmc, ADC_ADS23_CMD, 0x1, 0x0000);

  data = 0x8000 | ((mode & 0xfff00000) >> 18);
  adc3112_spi_write( fmc, ADC_ADS23_CMD, 0x3c, data);
  data = ((mode & 0xfff0) >> 2);
  adc3112_spi_write( fmc, ADC_ADS23_CMD, 0x3d, data);
  data = 0x8000 | ((mode & 0xfff00000) >> 18);
  adc3112_spi_write( fmc, ADC_ADS23_CMD, 0x3e, data);

  return(0);
}

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * Function name : adc3112_calib_restore
 * Prototype     : 
 * Parameters    : 
 * Return        : 
 *----------------------------------------------------------------------------
 * Description   : 
 *
 *++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/


int
adc3112_calib_restore( fmc)
{
  /* ADS5409_01 exit calibration mode */
  adc3112_spi_write( fmc, ADC_ADS01_CMD, 0xe, 0xaaa8);
  adc3112_spi_write( fmc, ADC_ADS01_CMD, 0xf, 0xa000);
  adc3112_spi_write( fmc, ADC_ADS01_CMD, 0x1, 0x0002);
  adc3112_spi_write( fmc, ADC_ADS01_CMD, 0x3c, 0);
  adc3112_spi_write( fmc, ADC_ADS01_CMD, 0x3d, 0);
  adc3112_spi_write( fmc, ADC_ADS01_CMD, 0x3e, 0);

  /* ADS5409_23 exit calibration mode */
  adc3112_spi_write( fmc, ADC_ADS23_CMD, 0xe, 0xaaa8);
  adc3112_spi_write( fmc, ADC_ADS23_CMD, 0xf, 0xa000);
  adc3112_spi_write( fmc, ADC_ADS23_CMD, 0x1, 0x0002);
  adc3112_spi_write( fmc, ADC_ADS23_CMD, 0x3c, 0);
  adc3112_spi_write( fmc, ADC_ADS23_CMD, 0x3d, 0);
  adc3112_spi_write( fmc, ADC_ADS23_CMD, 0x3e, 0);

  return(0);
}


/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * Function name : adc3112_calib_trig
 * Prototype     : 
 * Parameters    : 
 * Return        : 
 *----------------------------------------------------------------------------
 * Description   : 
 *
 *++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

int
adc3112_calib_trig( struct adc3112_calib_ctl *cc,
		    uint mode)
{
  int i, chan;
  unsigned short calib_res[4];
  unsigned short cmp0, sync0;
  unsigned short cmp1, sync1;
  char *p;

  cmp0 = (unsigned short)((mode&0xfff00000)>>16);
  cmp1 = (unsigned short)(mode&0xfff0);
  sync0 = (unsigned short)((mode&0x10000)>>16);
  sync1 = (unsigned short)(mode&1);

  /* clear data buffers for chan 0 -> 3 */
  for( chan = 0; chan < ADC_NUM_CHAN; chan++)
  {
    p = cc->chan[chan].data_buf;
    for( i = 0; i < (ADC_NUM_SAMPLES*ADC_SAMPLE_SIZE); i += sizeof(int))
    {
      *(int *)&p[i] = 0;
    }
  }

  /* start data acquisition */
  adc3112_trig_acq( cc->reg_acq_csr, 0);
#ifdef JFG
  /* display data buffers for cha 0 -> 3 */
  for( chan = 0; chan < ADC_NUM_CHAN; chan++)
  {
    p = cc->chan[chan].data_buf;
    for( i = 0; i < 0x10; i += sizeof(int))
    {
      if( !(i & 0xf)) printf("\n%03x : ", i);
      printf("%04x %04x ", (unsigned short)tsc_swap_16(*(short *)&p[i+2]), (unsigned short)tsc_swap_16(*(short *)&p[i]));
    }
  }
  printf("\n");
#endif

  calib_res[0] = adc3112_calib_data_check( cc, 0, cmp0|sync0, cmp1|sync1);
  cc->chan[0].cal_res[(CAL_STEP_NUM/2) + cc->chan[0].delay] = calib_res[0];
  calib_res[1] = adc3112_calib_data_check( cc, 1, cmp1|sync0, cmp0|sync1);
  cc->chan[1].cal_res[(CAL_STEP_NUM/2) + cc->chan[1].delay] = calib_res[1];
  calib_res[2] = adc3112_calib_data_check( cc, 2, cmp0|sync0, cmp1|sync1);
  cc->chan[2].cal_res[(CAL_STEP_NUM/2) + cc->chan[2].delay] = calib_res[2];
  calib_res[3] = adc3112_calib_data_check( cc, 3, cmp1|sync0, cmp0|sync1);
  cc->chan[3].cal_res[(CAL_STEP_NUM/2) + cc->chan[3].delay] = calib_res[3];
    
  //printf("data checking : %5d : %04x %04x %04x %04x\n", (CAL_STEP_NUM/2) + cc->chan[0].delay, calib_res[0], calib_res[1], calib_res[2], calib_res[3]);

  return(0);
}

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * Function name : adc3112_calib_show_res
 * Prototype     : 
 * Parameters    : 
 * Return        : 
 *----------------------------------------------------------------------------
 * Description   : 
 *
 *++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

int
adc3112_calib_show_res( struct adc3112_calib_ctl *cc)
{
  int step;

  if( adc3112_verbose_flag) printf("delay   CH0  CH1  CH2  CH3\n");
  for( step = 0; step < (CAL_STEP_NUM+1); step++)
  {
    if( adc3112_verbose_flag) printf("%5d : %04x %04x %04x %04x\n", step - (CAL_STEP_NUM/2), 
	   cc->chan[0].cal_res[step], cc->chan[1].cal_res[step], cc->chan[2].cal_res[step], cc->chan[3].cal_res[step]);
  }
  return(0);
}

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * Function name : adc3112_calib_show_err_cnt
 * Prototype     : 
 * Parameters    : 
 * Return        : 
 *----------------------------------------------------------------------------
 * Description   : 
 *
 *++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

int
adc3112_calib_show_err_cnt( struct adc3112_calib_ctl *cc,
			    int chan)
{
  int step;
  int bit;
  int min[CAL_BIT_NUM], max[CAL_BIT_NUM];

  for( bit = 0; bit < CAL_BIT_NUM; bit++)
  {
    min[bit] = -1;
    max[bit] = -1;
  }
  if( adc3112_verbose_flag) printf("\n Delay   D11  D10  D9   D8   D7   D6   D5   D4   D3   D2   D1   D0  SYNC\n");
  for( step = 0; step < (CAL_STEP_NUM+1); step++)
  {
   if( adc3112_verbose_flag)  printf("%5d :", step - (CAL_STEP_NUM/2)); 
    for( bit = 0; bit < 12; bit++)
    {
      if( (cc->chan[chan].err_cnt[15-bit][step] == 0) && (min[15-bit] == -1))
      {
	min[15-bit] = step;
      }
      if( (cc->chan[chan].err_cnt[15-bit][step] != 0) && (min[15-bit] != -1) && (max[15-bit] == -1))
      {
	max[15-bit] = step - 1;;
      }
      if( cc->chan[chan].err_cnt[15-bit][step] == ADC_NUM_SAMPLES)
      {
        if( adc3112_verbose_flag) printf(" XXXX");
      } 
      else
      {
        if( adc3112_verbose_flag) printf(" %04x", cc->chan[chan].err_cnt[15-bit][step]);
      }
    }
    if( cc->chan[chan].err_cnt[0][step] == ADC_NUM_SAMPLES)
    {
      if( adc3112_verbose_flag) printf(" XXXX\n");
    }
    else 
    { 
      if( adc3112_verbose_flag) printf(" %04x\n", cc->chan[chan].err_cnt[0][step]);
    }
  }
  return(0);
}

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * Function name : adc3112_calib_show_min_max
 * Prototype     : 
 * Parameters    : 
 * Return        : 
 *----------------------------------------------------------------------------
 * Description   : 
 *
 *++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

int
adc3112_calib_show_min_max( struct adc3112_calib_ctl *cc,
			    int chan)
{
  int step;
  int bit;
  int min[CAL_BIT_NUM], max[CAL_BIT_NUM], first[CAL_BIT_NUM];

  for( bit = 0; bit < CAL_BIT_NUM; bit++)
  {
    min[bit] = -16;
    max[bit] = 16;
    first[bit] = 0;
  }

  for( bit = 0; bit < 12; bit++)
  {
    for( step = 0; step < (CAL_STEP_NUM/2); step++)
    {
      if( cc->chan[chan].err_cnt[15-bit][(CAL_STEP_NUM/2) + step] == 0)
      {
	first[15-bit] = step;
	break;
      }
      if( cc->chan[chan].err_cnt[15-bit][(CAL_STEP_NUM/2) - step] == 0)
      {
	first[15-bit] = -step;
	break;
      }
    }
  }
  for( step = 0; step < (CAL_STEP_NUM/2); step++)
  {
    if( cc->chan[chan].err_cnt[0][(CAL_STEP_NUM/2) + step] == 0)
    {
      first[15-bit] = step;
      break;
    }
    if( cc->chan[chan].err_cnt[0][(CAL_STEP_NUM/2) - step] == 0)
    {
      first[15-bit] = -step;
      break;
    }
  }
  for( bit = 0; bit < 12; bit++)
  {
    for( step = first[15-bit]; step < (CAL_STEP_NUM/2); step++)
    {
      if( (cc->chan[chan].err_cnt[15-bit][(CAL_STEP_NUM/2) + step] != 0) && (max[15-bit] == 16))
      {
	max[15-bit] = step;
      }
    }
  }
  for( step = first[0]; step < (CAL_STEP_NUM/2); step++)
  {
    if( (cc->chan[chan].err_cnt[0][(CAL_STEP_NUM/2) + step] != 0) && (max[0] == 16))
    {
      max[0] = step;
    }
  }
  for( bit = 0; bit < 12; bit++)
  {
    for( step = first[15-bit]; step > -(CAL_STEP_NUM/2); step--)
    {
      if( (cc->chan[chan].err_cnt[15-bit][(CAL_STEP_NUM/2) + step] != 0) && (min[15-bit] == -16))
      {
	min[15-bit] = step;
      }
    }
  }
  for( step = first[0]; step > -(CAL_STEP_NUM/2); step--)
  {
    if( (cc->chan[chan].err_cnt[0][(CAL_STEP_NUM/2) + step] != 0) && (min[0] == -16))
    {
      min[0] = step;
    }
  }

  if( adc3112_verbose_flag) printf(" MAX  :");
  cc->chan[chan].hold_time =  max[15];
  for( bit = 0; bit < 12; bit++)
  {
    if( max[15-bit] < cc->chan[chan].hold_time) cc->chan[chan].hold_time = max[15-bit]; 
    if( adc3112_verbose_flag) printf(" %3d ", max[15-bit]);
  }
  if( adc3112_verbose_flag) printf(" %3d\n", max[0]);
  if( max[0] < cc->chan[chan].hold_time) cc->chan[chan].hold_time = max[0]; 
  cc->chan[chan].hold_time =   (cc->chan[chan].hold_time - 1)*CAL_STEP_WIDTH;

  if( adc3112_verbose_flag) printf(" MEAN :");
  for( bit = 0; bit < 12; bit++)
  {
    cc->chan[chan].delta[15-bit] =  (max[15-bit] + min[15-bit])/2;
    if( adc3112_verbose_flag) printf(" %3d ", cc->chan[chan].delta[15-bit]);
  }
  cc->chan[chan].delta[0] = (max[0] + min[0])/2; 
  if( adc3112_verbose_flag) printf(" %3d\n", cc->chan[chan].delta[0]);

  if( adc3112_verbose_flag) printf(" MIN  :");
  cc->chan[chan].set_time =  min[15];
  for( bit = 0; bit < 12; bit++)
  {
    if( min[15-bit] > cc->chan[chan].set_time) cc->chan[chan].set_time = min[15-bit]; 
    if( adc3112_verbose_flag) printf(" %3d ", min[15-bit]);
  }
  if( adc3112_verbose_flag) printf(" %3d\n", min[0]);
  if( min[0] > cc->chan[chan].set_time) cc->chan[chan].set_time = min[0];
  cc->chan[chan].set_time =   (cc->chan[chan].set_time - 1)*CAL_STEP_WIDTH;

  return(0);
}

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * Function name : adc3112_calib_adjust_delay
 * Prototype     : 
 * Parameters    : 
 * Return        : 
 *----------------------------------------------------------------------------
 * Description   : 
 *
 *++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

int
adc3112_calib_adjust_delay( struct adc3112_calib_ctl *cc,
			    int chan)
{
  int step, bit;

  if( adc3112_verbose_flag) printf("IODelay #%d :", chan);
  for( bit = 0; bit < 12; bit++)
  {
    if( adc3112_verbose_flag) printf(" %03x",  (cc->chan[chan].ttim[15-bit] >> 8) & 0xfff);
  }
  if( adc3112_verbose_flag) printf(" %03x\n",  (cc->chan[chan].ttim[0] >> 8) & 0xfff);

  for( bit = 0; bit < 12; bit++)
  {
    if( cc->chan[chan].delta[bit+4] < 0)
    {
      for( step = 0; step < -cc->chan[chan].delta[bit+4]; step++)
      {
	adc3112_calib_dec_delay( cc, 1<<bit, chan);
      }
    }
    if( cc->chan[chan].delta[bit+4] > 0)
    {
      for( step = 0; step < cc->chan[chan].delta[bit+4]; step++)
      {
	adc3112_calib_inc_delay( cc, 1<<bit, chan);
      }
    }
  }
  if( cc->chan[chan].delta[0] < 0)
  {
    for( step = 0; step < -cc->chan[chan].delta[0]; step++)
    {
      adc3112_calib_dec_delay( cc, 1<<12, chan);
    }
  }
  if( cc->chan[chan].delta[0] > 0)
  {
    for( step = 0; step < cc->chan[chan].delta[0]; step++)
    {
      adc3112_calib_inc_delay( cc, 1<<12, chan);
    }
  }

  if( adc3112_verbose_flag) printf("           :");
  for( bit = 0; bit < 12; bit++)
  {
    if( adc3112_verbose_flag) printf(" %03x",  (cc->chan[chan].ttim[15-bit] >> 8) & 0xfff);
  }
  if( adc3112_verbose_flag) printf(" %03x\n",  (cc->chan[chan].ttim[0] >> 8) & 0xfff);
  return(0);
}

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * Function name : adc3112_calib_reset_delay
 * Prototype     : 
 * Parameters    : 
 * Return        : 
 *----------------------------------------------------------------------------
 * Description   : 
 *
 *++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

int
adc3112_calib_reset_delay( struct adc3112_calib_ctl *cc)
{
  int bit, chan;

  for( chan = 0; chan < ADC_NUM_CHAN; chan++)
  {
    for( bit = 0; bit < CAL_BIT_NUM; bit++)
    {
      cc->chan[chan].delta[bit] = 0;
    }
  }
  return(0);
}

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * Function name : adc3112_calib
 * Prototype     : 
 * Parameters    : 
 * Return        : 
 *----------------------------------------------------------------------------
 * Description   : 
 *
 *++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

int
adc3112_calib( struct cli_cmd_para *c,
	       int idx,
	       int fmc,
	       int size)
{
  char *adc_buf;
  int chan;
  int step;
  uint mode;
  char *p;
  int i, cnt;
  int reset;

  printf("Performing %s calibration\n", c->para[0]);
  adc_buf = adc3112_map_usr( &adc3112_mas_map, 0x100000, 0x100000, fmc);
  if( fmc == 2)
  {
    adc3112_calib_init( &adc3112_calib_ctl[1], ADC_REG_TTIM_B, ACQ_REG_BASE_B, ADC_REG_SERIAL_B, adc_buf);
  }
  else 
  {
    adc3112_calib_init( &adc3112_calib_ctl[0], ADC_REG_TTIM_A, ACQ_REG_BASE_A, ADC_REG_SERIAL_A, adc_buf);
  }
  mode = 0x5551aaa0;
  cnt = c->cnt - 2;
  reset = 0;
  adc3112_verbose_flag = 0;
  i = 2;
  while( cnt--)
  {
    p =  c->para[i++];
    if( p[0] == 'd')
    {
      sscanf( p, "d:%x", &mode);
    }
    if( p[0] == 'r')
    {
      reset = 1;
    }
    if( p[0] == 'v')
    {
      adc3112_verbose_flag = 1;
    }
  }
  if( c->cnt > 2)
  {
    sscanf( c->para[2], "d:%x", &mode);
  }
  printf("Configure ADS with calibration value %08x\n", mode);
  adc3112_calib_config( fmc, mode);

  printf("Setting default values for all channels\n");
  for( chan = 0; chan < ADC_NUM_CHAN; chan++)
  {
    adc3112_calib_set_default( &adc3112_calib_ctl[FMC_IDX(fmc)], chan);
  }
  if( reset)
  {
    printf("Adjust time delay to zero\n");
    adc3112_calib_reset_delay( &adc3112_calib_ctl[FMC_IDX(fmc)]);
  }
  else 
  {
    printf("\nAdjust time delay to mean\n");
    for( chan = 0; chan < ADC_NUM_CHAN; chan++)
    {
      adc3112_calib_adjust_delay( &adc3112_calib_ctl[FMC_IDX(fmc)], chan);
    }
  }
  printf("Trig calibration with default delay\n");
  adc3112_calib_trig( &adc3112_calib_ctl[FMC_IDX(fmc)], mode);

  printf("Incrementing time delay...\n");
  for( step = 0; step < (CAL_STEP_NUM/2); step++)
  {
    for( chan = 0; chan < ADC_NUM_CHAN; chan++)
    {
      adc3112_calib_inc_delay( &adc3112_calib_ctl[FMC_IDX(fmc)], CAL_ALL_BIT, chan);
    }
    printf("Start calibration with delay %d\r", adc3112_calib_ctl[FMC_IDX(fmc)].chan[0].delay);
    fflush( stdout);
    adc3112_calib_trig( &adc3112_calib_ctl[FMC_IDX(fmc)], mode);
  }

  printf("\nSetting default values for all channels\n");
  for( chan = 0; chan < ADC_NUM_CHAN; chan++)
  {
    adc3112_calib_set_default( &adc3112_calib_ctl[FMC_IDX(fmc)], chan);
  }
  if( reset)
  {
    printf("Adjust time delay to zero\n");
    adc3112_calib_reset_delay( &adc3112_calib_ctl[FMC_IDX(fmc)]);
  }
  else 
  {
    printf("\nAdjust time delay to mean\n");
    for( chan = 0; chan < ADC_NUM_CHAN; chan++)
    {
      adc3112_calib_adjust_delay( &adc3112_calib_ctl[FMC_IDX(fmc)], chan);
    }
  }

  printf("Decrementing time delay...\n");
  for( step = 0; step < (CAL_STEP_NUM/2); step++)
  {
    for( chan = 0; chan < ADC_NUM_CHAN; chan++)
    {
      adc3112_calib_dec_delay( &adc3112_calib_ctl[FMC_IDX(fmc)], CAL_ALL_BIT, chan);
    }
    printf("Start calibration with delay %d\r", adc3112_calib_ctl[FMC_IDX(fmc)].chan[0].delay);
    fflush( stdout);
    adc3112_calib_trig( &adc3112_calib_ctl[FMC_IDX(fmc)], mode);
  }

  printf("\nSetting default values for all channels\n");
  for( chan = 0; chan < ADC_NUM_CHAN; chan++)
  {
    adc3112_calib_set_default( &adc3112_calib_ctl[FMC_IDX(fmc)], chan);
  }
  if( reset)
  {
    printf("Adjust time delay to zero\n");
    adc3112_calib_reset_delay( &adc3112_calib_ctl[FMC_IDX(fmc)]);
  }
  else 
  {
    printf("\nAdjust time delay to mean\n");
    for( chan = 0; chan < ADC_NUM_CHAN; chan++)
    {
      adc3112_calib_adjust_delay( &adc3112_calib_ctl[FMC_IDX(fmc)], chan);
    }
  }

  printf("Trig calibration with adjusted delay\n");
  adc3112_calib_trig( &adc3112_calib_ctl[FMC_IDX(fmc)], mode);

  printf("Calibration results:\n");
  adc3112_calib_show_res( &adc3112_calib_ctl[FMC_IDX(fmc)]);

  for( chan = 0; chan < ADC_NUM_CHAN; chan++)
  {
    adc3112_calib_show_err_cnt( &adc3112_calib_ctl[FMC_IDX(fmc)], chan);
    adc3112_calib_show_min_max( &adc3112_calib_ctl[FMC_IDX(fmc)], chan);
  }
  printf("+----+--------------+--------------+\n");
  printf("| CH | SETUP [psec] |  HOLD [psec] |\n");
  printf("+----+--------------+--------------+\n");
  for( chan = 0; chan < ADC_NUM_CHAN; chan++)
  {
    printf("| %2d |     %5d    |     %5d    |\n", chan, adc3112_calib_ctl[FMC_IDX(fmc)].chan[chan].set_time, adc3112_calib_ctl[FMC_IDX(fmc)].chan[chan].hold_time);
  }
  printf("+----+--------------+--------------+\n");
  printf("Restore ADC configuration\n");
  adc3112_calib_restore( fmc);

  adc3112_unmap_usr( &adc3112_mas_map, adc_buf);

  return(0);
}


/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * Function name : adc3112_itl_init
 * Prototype     : 
 * Parameters    : 
 * Return        : 
 *----------------------------------------------------------------------------
 * Description   : 
 *
 *++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

int
adc3112_itl_init( struct adc3112_itl_ctl *ic,
		  int reg_itl_gain,
		  int reg_acq_csr,
		  char *data_buf)

{
  int chan;

  ic->reg_itl_gain = reg_itl_gain;
  ic->reg_acq_csr = reg_acq_csr;
  for( chan = 0; chan < ADC_NUM_CHAN; chan++)
  {
    ic->chan[chan].chan = chan;
    ic->chan[chan].data_buf = data_buf + ADC_OFF_CHAN_1*chan;
  }
  return(0);
}

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * Function name : adc3112_itl_set_gain
 * Prototype     : 
 * Parameters    : 
 * Return        : 
 *----------------------------------------------------------------------------
 * Description   : 
 *
 *++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

int
adc3112_itl_set_gain( struct adc3112_itl_ctl *ic,
		      int chan)
{
  int data;
  int reg;

  data = ((-ic->chan[chan].offset)<<16) | (ic->chan[chan].gain & 0xffff);
  reg = ic->reg_itl_gain + (4*chan);
  printf("set gain for channel %d : %x : %x -> %x [%x]\n", chan, ic->chan[chan].gain, ic->chan[chan].offset, data, reg);
  tscext_csr_wr( reg, data);
  return(0);
}

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * Function name : adc3112_itl_trig_acq
 * Prototype     : 
 * Parameters    : 
 * Return        : 
 *----------------------------------------------------------------------------
 * Description   : 
 *
 *++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

int
adc3112_itl_trig_acq( struct adc3112_itl_ctl *ic,
		      int idx,
		      int size )
{
  int tmo, i;
  int chan;

  tmo = adc3112_trig_acq( ic->reg_acq_csr, 0);
  if( tmo)
  {
    short *buf;
    int n, data;

    buf = ( short *)malloc(size);

    n = 0;
    chan = idx;
    printf("Loading data fron channel %d [%p]...", chan, ic->chan[chan].data_buf);
    for( i = 0; i < size; i += 4)
    {
      data = *(int *)&ic->chan[chan].data_buf[i];
      buf[n+1] = tsc_swap_16((short)data)/16;
      buf[n] = tsc_swap_16((short)(data >> 16))/16;
      n += 2;
    }
    printf("\n");
    printf("Processing data channel %d...\n", chan);
    //adc3112_acq_res( &ic->acq_res, buf, size, chan);
    adc3112_acq_res( &ic->acq_res, &buf[0x1000], 0x8000, chan);

    n = 0;
    chan = idx+2;
    printf("Loading data fron channel %d [%p]...", chan, ic->chan[chan].data_buf);
    for( i = 0; i < size; i += 4)
    {
      data = *(int *)&ic->chan[chan].data_buf[i];
      buf[n+1] = tsc_swap_16((short)data)/16;
      buf[n] = tsc_swap_16((short)(data >> 16))/16;
      n += 2;
    }
    printf("\n");
    printf("Processing data channel %d...\n", chan);
    //adc3112_acq_res( &ic->acq_res, buf, size, chan);
    adc3112_acq_res( &ic->acq_res, &buf[0x1000], size, chan);

    free( (void *)buf);
  }
  else
  {
    printf("Acquistion timeout...\n");
    return( -1);
  }
  return(0);
}

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * Function name : adc3112_itl_calib
 * Prototype     : 
 * Parameters    : 
 * Return        : 
 *----------------------------------------------------------------------------
 * Description   : 
 *
 *++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

int
adc3112_itl_calib( struct cli_cmd_para *c,
	           int idx,
	           int fmc,
	           int size)
{
  char *adc_buf;
  struct adc3112_itl_ctl *ic;
  struct adc3112_acq_res *ar;
  int chan;


  ic = &adc3112_itl_ctl[FMC_IDX(fmc)];
  printf("Performing %s interleaving calibration....\n", c->para[0]);
  if( idx == 0)
  {
    printf("please verify sine signal connected to IN1 and enter <CR> : ");
  }
  else
  {
    printf("please verify sine signal connected to IN2 and enter <CR> : ");
  }
  getchar(); /* wait for <CR> */

  adc_buf = adc3112_map_usr( &adc3112_mas_map, 0x100000, 0x100000, fmc);
  if( fmc == 2)
  {
    adc3112_itl_init( ic, ADC_REG_GAIN_B, ACQ_REG_BASE_B, adc_buf);
  }
  else 
  {
    adc3112_itl_init( ic, ADC_REG_GAIN_A, ACQ_REG_BASE_A, adc_buf);
  }

  ar = &ic->acq_res;
  for( chan = idx; chan < ADC_NUM_CHAN; chan += 2)
  {
    int data;

    data = tscext_csr_rd( adc3112_reg[FMC_IDX(fmc)].gain + (4*chan));
    ic->chan[chan].gain = data & 0xffff;
    ic->chan[chan].offset = (int)(-(short)(data >>16));
  }
  printf("Trig data acquisition...[%p]\n", adc_buf);
  adc3112_itl_trig_acq( ic, idx, size);

  for( chan = idx; chan < ADC_NUM_CHAN; chan += 2)
  {
    printf("Channel %d : %f +/- %f [%d (0x%x) : %d (0x%x) ]\n", chan,
	   ar->chan[chan].mean, ar->chan[chan].sig, ar->chan[chan].min, 2*ar->chan[chan].off_min, ar->chan[chan].max, 2*ar->chan[chan].off_max);
  }
  ic->chan[idx].gain   = (int)(ic->chan[idx].gain*((ar->chan[idx].sig + ar->chan[idx+2].sig)/(2*ar->chan[idx].sig)));
  ic->chan[idx+2].gain = (int)(ic->chan[idx+2].gain*((ar->chan[idx].sig + ar->chan[idx+2].sig)/(2*ar->chan[idx+2].sig)));
  adc3112_itl_set_gain( ic, idx);
  adc3112_itl_set_gain( ic, idx+2);

  printf("Trig data acquisition wit adjusted gain...\n");
  adc3112_itl_trig_acq( ic, idx, size);

  for( chan = idx; chan < ADC_NUM_CHAN; chan += 2)
  {
    printf("Channel %d : %f +/- %f [%d (0x%x) : %d (0x%x) ]\n", chan,
	   ar->chan[chan].mean, ar->chan[chan].sig, ar->chan[chan].min, 2*ar->chan[chan].off_min, ar->chan[chan].max, 2*ar->chan[chan].off_max);
  }
  if( idx == 0)
  {
    ic->chan[0].offset += (int)((ar->chan[0].mean+0.5)*4);
    ic->chan[2].offset += (int)((ar->chan[2].mean+0.5)*4);
    adc3112_itl_set_gain( ic, 0);
    adc3112_itl_set_gain( ic, 2);
  }
  else
  {
    ic->chan[1].offset += (int)((ar->chan[1].mean+0.5)*4);
    ic->chan[3].offset += (int)((ar->chan[3].mean+0.5)*4);
    adc3112_itl_set_gain( ic, 1);
    adc3112_itl_set_gain( ic, 3);
  }
  printf("Trig data acquisition wit adjusted gain + offset...\n");
  adc3112_itl_trig_acq( ic, idx, size);

  for( chan = idx; chan < ADC_NUM_CHAN; chan += 2)
  {
    printf("Channel %d : %f +/- %f [%d (0x%x) : %d (0x%x) ]\n", chan,
	   ar->chan[chan].mean, ar->chan[chan].sig, ar->chan[chan].min, 2*ar->chan[chan].off_min, ar->chan[chan].max, 2*ar->chan[chan].off_max);
  }

  adc3112_unmap_usr( &adc3112_mas_map, adc_buf);

  return(0);
}

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * Function name : adc3112_itl_calib_save
 * Prototype     : 
 * Parameters    : 
 * Return        : 
 *----------------------------------------------------------------------------
 * Description   : 
 *
 *++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

int
adc3112_itl_calib_save( struct cli_cmd_para *c)
{
  printf("adc3112 saving interleave calibration parameters\n");
  return(0);
}

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * Function name : adc3112_itl_calib_restore
 * Prototype     : 
 * Parameters    : 
 * Return        : 
 *----------------------------------------------------------------------------
 * Description   : 
 *
 *++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

int
adc3112_itl_calib_restore( struct cli_cmd_para *c)
{
  printf("adc3112 restoring interleave calibration parameters\n");
  return(0);
}

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * Function name : adc3112_itl_enable
 * Prototype     : 
 * Parameters    : 
 * Return        : 
 *----------------------------------------------------------------------------
 * Description   : 
 *
 *++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

int
adc3112_itl_enable( struct cli_cmd_para *c)
{
  printf("adc3112 enablr interleave\n");
  return(0);
}

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * Function name : adc3112_itl_reset
 * Prototype     : 
 * Parameters    : 
 * Return        : 
 *----------------------------------------------------------------------------
 * Description   : 
 *
 *++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

int
adc3112_itl_reset( struct cli_cmd_para *c)
{
  printf("adc3112 reseting interleave parameters\n");
  return(0);
}

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * Function name : xprs_adc3112
 * Prototype     : 
 * Parameters    : 
 * Return        : 
 *----------------------------------------------------------------------------
 * Description   : main entry point for adc3112 command
 *
 *++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

int 
tsc_adc3112( struct cli_cmd_para *c)
{
  struct tsc_adc3112_devices *add;
  struct tsc_time utmi, utmo;
  uint cmd, data, reg, fmc, tmo, usec;
  char *p;
  uint id;


  if( c->cnt < 2)
  {
    printf("adc3112 command needs more arguments\n");
    printf("usage: adc3112.<fmc> <dev> <op> <reg> [<data>]\n");
    printf("adc3112 device list:\n");
    add = &adc3112_devices[0];
    while( add->name)
    {
      printf("   - %s\n", add->name);
      add++;
    }
    return(-1);
  }

  if( !c->ext) 
  {
    printf("you must specify fmc [1 or 2]\n");
    printf("usage: adc3112.<fmc> <dev> <op> <reg> [<data>]\n");
    return(-1);
  }
  else
  {
    fmc = strtoul( c->ext, &p, 16);
    if(( fmc < 1) || ( fmc > 2))
    {
      printf("bad FMC index : %d\n", fmc);
      return( -1);
    }
  }
  if( !strcmp( "fpfbuf", c->para[0]))
  {
    return( adc3112_fpfbuf_save( c, fmc));
  }
  if( fmc == 1)
  {
    id = tscext_csr_rd( ADC_REG_SIGN_A);
    if( ( id & 0xffff0000) != 0x31120000)
    {
      printf("no ADC3112 installed on FMC#1 [%08x] !!\n", id);
      return(-1);
    }
  }
  if( fmc == 2)
  {
    id = tscext_csr_rd( ADC_REG_SIGN_B);
    if( ( id & 0xffff0000) != 0x31120000)
    {
      printf("no ADC3112 installed on FMC#2 [%08x] !!\n", id);
      return(-1);
    }
  }
  add = &adc3112_devices[0];
  while( add->name)
  {
    if( !strcmp(  c->para[0], add->name))
    {
      break;
    }
    add++;
  }
  if( !add->name)
  {
    printf("wrong device name\n");
    printf("usage: adc3112.<fmc> <dev> <op> <reg> [<data>]\n");
    printf("adc3112 device list:\n");
    add = &adc3112_devices[0];
    while( add->name)
    {
      printf("   - %s\n", add->name);
      add++;
    }
    return(-1);
  }
  adc3112_init( fmc);
  if( !strcmp( "read", c->para[1]))
  {
    if( !(add->bus & BUS_READ))
    {
      printf("adc3112 read command not allowed for device %s\n", add->name);
      return(-1);
    }
    if( c->cnt < 3)
    {
      printf("adc3112 read command needs more arguments\n");
      printf("usage: adc3112.<fmc> <dev> read <reg>\n");
      return(-1);
    }
    printf("%s.%s %s %s %s\n", c->cmd, c->ext, c->para[0], c->para[1], c->para[2]);
    if( sscanf( c->para[2],"%x", &reg) != 1)
    {
      printf("wrong register number\n");
      printf("usage: adc3112.<fmc> <dev> <op> <reg> [<data>]\n");
      return(-1);
    }
    if( add->bus & BUS_SPI)
    {
      tsc_timer_read( &utmi);
      data = adc3112_spi_read( fmc, add->cmd, reg);
      if( data < 0)
      {
	return( -1);
      }
      tsc_timer_read( &utmo);
      usec = (utmo.msec - utmi.msec)*1000 + ((utmo.usec & 0x1ffff) - (utmi.usec & 0x1ffff))/100;
      printf("cmd = %08x - data = %08x [%d usec]\n", add->cmd | reg, data, usec);
    }
  }
  else if( !strcmp( "write", c->para[1]))
  {
    if( !(add->bus & BUS_WRITE))
    {
      printf("adc3112 write command not allowed for device %s\n", add->name);
      return(-1);
    }
    if( c->cnt < 4)
    {
      printf("adc3112 write command needs more arguments\n");
      printf("usage: adc3112.<fmc> <dev> write <reg> <data>\n");
      return(-1);
    }
    if( sscanf( c->para[2],"%x", &reg) != 1)
    {
      printf("wrong register number\n");
      printf("usage: adc3112.<fmc> <dev> <op> <reg> [<data>]\n");
      return(-1);
    }
    if( sscanf( c->para[3],"%x", &data) != 1)
    {
      printf("wrong data value\n");
      return(-1);
    }
    printf("%s.%s %s %s %s %s \n", c->cmd, c->ext, c->para[0], c->para[1], c->para[2], c->para[3]);
    if( add->bus & BUS_SPI)
    {
      tsc_timer_read( &utmi);
      if( adc3112_spi_write( fmc, add->cmd, reg, data) < 0)
      {
	return( -1);
      }
      tsc_timer_read( &utmo);
      usec = (utmo.msec - utmi.msec)*1000 + ((utmo.usec & 0x1ffff) - (utmi.usec & 0x1ffff))/100;
      printf("cmd = %08x - data = %08x [%d usec]\n", add->cmd | reg, data, usec);
    }
  }
  else if( !strcmp( "show", c->para[1]))
  {
    int status;
    int device;
    uint temp, ctl, lo, hi;

    device = add->cmd;
    if( device != 0x01040048)
    {
      printf(" show command not supported for that device\n");
      return(-1);
    }
    if( fmc == 2)
    {
      device |= 0xa0000000;
    }
    else
    {
      device |= 0x80000000;
    }
    status = tsc_i2c_read( device, 1, &ctl);
    if( (status & I2C_CTL_EXEC_MASK) == I2C_CTL_EXEC_ERR)
    {
      printf("%s: reg=%x -> error = %08x\n", add->name, reg, status);
    }
    else
    {
      tsc_i2c_read( device, 0, &temp);
      tsc_i2c_read( device, 2, &lo);
      tsc_i2c_read( device, 3, &hi);
      if( temp & 0x100)
      {
	temp = ((temp & 0xff) << 5) + ((temp & 0xf8) >> 3);
	lo = ((lo & 0xff) << 5) + ((lo & 0xf8) >> 3);
	hi = ((hi & 0xff) << 5) + ((hi & 0xf8) >> 3);
      }
      else
      {
	temp = ((temp & 0xff) << 4) + ((temp & 0xf0) >> 4);
	lo = ((lo & 0xff) << 4) + ((lo & 0xf0) >> 4);
	hi = ((hi & 0xff) << 4) + ((hi & 0xf0) >> 4);
      }
      printf("current temperature: %.2f [%.2f - %.2f]\n", (float)temp/16, (float)lo/16, (float)hi/16);
    }
  }
  else if( !strcmp( "temp", c->para[1]))
  {
    if( !strcmp(  add->name, "ads01") ||  !strcmp( add->name, "ads23"))
    {
      cmd =  0x80000000 | add->cmd | ADS_REG_TEMP;
      tmo = 1000;
      if( fmc == 2)
      {
        tscext_csr_wr( ADC_REG_SERIAL_B, cmd);
        while( tmo--)
        {
	  if( !(tscext_csr_rd( ADC_REG_SERIAL_B) & 0x80000000)) break;
        }
	if( !tmo)
	{
	  printf("read operation didn't complete\n");
	  return(-1);
	}
        data = tscext_csr_rd( ADC_REG_SERIAL_B + 4);
	printf("%s temperature = %d °C\n", add->name, (char)data);
	return(0);
      }
      else
      {
        tscext_csr_wr( ADC_REG_SERIAL_A, cmd);
        while( tmo--)
        {
	  if( !(tscext_csr_rd( ADC_REG_SERIAL_A) & 0x80000000)) break;
        }
	if( !tmo)
	{
	  printf("read operation didn't complete\n");
	  return(-1);
	}
        data = tscext_csr_rd( ADC_REG_SERIAL_A + 4);
	printf("%s temperature = %d degree\n", add->name, (char)data);
	return(0);
      }
    }
    printf("operation unsupported for device: %s [%s]\n",  c->para[0],  c->para[1]);
  }
  else if( !strncmp( "acq", c->para[1], 3))
  {
    int size;
    int chan;

    if( !strcmp(  add->name, "ads01") ||  !strcmp( add->name, "ads23"))
    {
      //size = 0x8000;
      size = 0x10000;
      chan = ADC3112_CHAN1 |  ADC3112_CHAN2 |  ADC3112_CHAN3 |  ADC3112_CHAN4;
      adc3112_acq( c, add->idx, fmc, size, chan, 0, 0);
    }
    else 
    {
      printf("acq operation not supported for that device [%s]\n", add->name);
      printf("usage: adc3112.<fmc> ads<ij> acq  [h:<file_his>]  [d:<file_dat>] [t:<trig>] [s:<sram>]\n");
      return(-1);
    }
  }
  else if( !strncmp( "save", c->para[1], 4))
  {
    int size;
    int chan;

    if( !strcmp(  add->name, "ads01") ||  !strcmp( add->name, "ads23"))
    {
      size = 0xa000;
      //size = 0x10000;
      chan = ADC3112_CHAN1 |  ADC3112_CHAN2 |  ADC3112_CHAN3 |  ADC3112_CHAN4;
      if( c->para[1][4] == '0') chan = ADC3112_CHAN1;
      if( c->para[1][4] == '1') chan = ADC3112_CHAN2;
      if( c->para[1][4] == '2') chan = ADC3112_CHAN3;
      if( c->para[1][4] == '3') chan = ADC3112_CHAN4;
      adc3112_acq( c, add->idx, fmc, size, chan, 1, 0x2000);
    }
    else 
    {
      printf("acq operation not supported for that device [%s]\n", add->name);
      printf("usage: adc3112.<fmc> ads<ij> acq  [h:<file_his>]  [d:<file_dat>] [t:<trig>] [s:<sram>]\n");
      return(-1);
    }
  }
  else if( !strncmp( "calib", c->para[1], 3))
  {
    int size;

    if( !strncmp(  add->name, "ads", 3))
    {
      size = 0x1000;
      adc3112_calib( c, add->idx, fmc, size);
    }
    else 
    {
      printf("calib operation not supported for that device [%s]\n", add->name);
      printf("usage: adc3112.<fmc> ads<ij> calib\n");
      return(-1);
    }
  }
  else if( !strncmp( "itl", c->para[1], 3))
  {
    int size;
    int chan;

    if( !strcmp(  add->name, "ads1") ||  !strcmp( add->name, "ads2"))
    {
      if( !strncmp( "buf", c->para[2], 3))
      {
        adc3112_itl_buf( c, add->idx, fmc, add->idx);
      }
      if( !strncmp( "cal", c->para[2], 3))
      {
        size = 0x10000;
        adc3112_itl_calib( c, add->idx, fmc, size);
      }
      if( !strncmp( "save", c->para[2], 3))
      {
	int i, device, itl_corr;
	unsigned char *p;

	device = ADC_EEPROM_CMD;
        if( fmc == 2)
        {
          device |= 0xa0000000;
        }
        else
        {
          device |= 0x80000000;
        }
        p = (unsigned char *)&adc3112_sign;
        for( i = 0x0; i < sizeof(struct adc3112_sign); i++)
        {
          tsc_i2c_read( device, tsc_swap_16( 0x7000 + i), &data);
          p[i] = (unsigned char)data;
        }

	chan = add->idx;
	itl_corr = tscext_csr_rd( adc3112_reg[FMC_IDX(fmc)].gain + (4*chan));
	sprintf( &adc3112_sign.itl_corr[chan][0], "%08x", itl_corr);

	chan += 2;
	itl_corr = tscext_csr_rd( adc3112_reg[FMC_IDX(fmc)].gain + (4*chan));
	sprintf( &adc3112_sign.itl_corr[chan][0], "%08x", itl_corr);

        for( i = 0x0; i < sizeof(struct adc3112_sign); i++)
        {
	  data = p[i];
          tsc_i2c_write( device, tsc_swap_16(0x7000+i), data);
	  usleep(5000);
        }
      }
      if( !strncmp( "restore", c->para[2], 5))
      {
	int i, device, itl_corr;
	unsigned char *p;

	device = ADC_EEPROM_CMD;
        if( fmc == 2)
        {
          device |= 0xa0000000;
        }
        else
        {
          device |= 0x80000000;
        }
	printf("restoring interleave correction from EEPROM\n");
        p = (unsigned char *)&adc3112_sign;
        for( i = 0x0; i < sizeof(struct adc3112_sign); i++)
        {
          tsc_i2c_read( device, tsc_swap_16( 0x7000 + i), &data);
          p[i] = (unsigned char)data;
        }
	chan = add->idx;
	sscanf( &adc3112_sign.itl_corr[chan][0], "%08x", &itl_corr); 
	//printf("restoring chan %d : %08x [%08x]\n", chan, itl_corr, adc3112_reg[FMC_IDX(fmc)].gain + (4*chan));
	tscext_csr_wr( adc3112_reg[FMC_IDX(fmc)].gain + (4*chan), itl_corr);
	chan += 2;
	sscanf( &adc3112_sign.itl_corr[chan][0], "%08x", &itl_corr); 
	//printf("restoring chan %d : %08x [%08x]\n", chan, itl_corr, adc3112_reg[FMC_IDX(fmc)].gain + (4*chan));
	tscext_csr_wr( adc3112_reg[FMC_IDX(fmc)].gain + (4*chan), itl_corr);
      }
      if( !strncmp( "enable", c->para[2], 3))
      {
        /* XRA1404 OCR    ADC_ENABLE = 1  -> ADS5409 ENABLE = '1' + ITL enabled */
	adc3112_spi_write( fmc, ADC_XRATRIG_CMD, 0x2, 0xc0);
	adc3112_spi_write( fmc, ADC_XRATRIG_CMD, 0xc, 0x03);
      }
      if( !strncmp( "disable", c->para[2], 3))
      {
        /* XRA1404 OCR    ADC_ENABLE = 0  -> ADS5409 ENABLE = '0'   ITL disabled */
	adc3112_spi_write( fmc, ADC_XRATRIG_CMD, 0x2, 0x40);
      }
      if( !strncmp( "reset", c->para[2], 5))
      {
	chan = add->idx;
	tscext_csr_wr( adc3112_reg[FMC_IDX(fmc)].gain + (4*chan), 0x4000);
	chan += 2;
	tscext_csr_wr( adc3112_reg[FMC_IDX(fmc)].gain + (4*chan), 0x4000);
      }
    }
    else 
    {
      printf("itl operation not supported for that device [%s]\n", add->name);
      printf("usage: adc3112.<fmc> ads<ij> itl\n");
      return(-1);
    }
  }
  else if( !strcmp( "sign", c->para[1]))
  {
    int device, i;
    unsigned char *p;
    int op;
    char *para_p;

    device = add->cmd;
    if( device != 0x01010051)
    {
      printf(" sign command not supported for that device\n");
      return(-1);
    }
    if( fmc == 2)
    {
      device |= 0xa0000000;
    }
    else
    {
      device |= 0x80000000;
      if( add->idx == 1) device += 1;
    }
    p = (unsigned char *)&adc3112_sign;
    for( i = 0x0; i < 0x100; i++)
    {
      tsc_i2c_read( device, tsc_swap_16( 0x7000 + i), &data);
      p[i] = (unsigned char)data;
    }
    op = 0;
    if( c->cnt > 2)
    {
      if( !strcmp( "set", c->para[2]))
      {
	op = 1;
      }
      if( !strncmp( "default", c->para[2], 3))
      {
	op = 2;
      }
    }

    if( op == 1)
    {
      char prompt[64];

      bzero( &adc3112_history, sizeof( struct cli_cmd_history));
      cli_history_init( &adc3112_history);
      printf("setting ADC3112 signature\n");
      para_p = cli_get_cmd( &adc3112_history, "Enter password ->  ");
      if( strcmp(  para_p, "goldorak"))
      {
	printf("wrong password\n");
	return(-1);
      }

      strcpy( &prompt[0], "Board Name [");
      strncat( &prompt[0], &adc3112_sign.board_name[0], 8);
      strcat( &prompt[0], "] : "); 
      para_p = cli_get_cmd( &adc3112_history, prompt);
      if( para_p[0] == 'q') return(-1);
      if( para_p[0]) strncpy( &adc3112_sign.board_name[0], para_p, 8);


      strcpy( &prompt[0], "Serial Number [");
      strncat( &prompt[0], &adc3112_sign.serial[0], 4);
      strcat( &prompt[0], "] : "); 
      para_p = cli_get_cmd( &adc3112_history, prompt);
      if( para_p[0] == 'q') return(-1);
      if( para_p[0]) strncpy( &adc3112_sign.serial[0], para_p, 4);

      strcpy( &prompt[0], "PCB Version :  [");
      strncat( &prompt[0], &adc3112_sign.version[0], 8);
      strcat( &prompt[0], "] : "); 
      para_p = cli_get_cmd( &adc3112_history, prompt);
      if( para_p[0] == 'q') return(-1);
      if( para_p[0]) strncpy( &adc3112_sign.version[0], para_p, 8);

      strcpy( &prompt[0], "Hardware Revision :  [");
      strncat( &prompt[0], &adc3112_sign.revision[0], 2);
      strcat( &prompt[0], "] : "); 
      para_p = cli_get_cmd( &adc3112_history, prompt);
      if( para_p[0] == 'q') return(-1);
      if( para_p[0]) strncpy( &adc3112_sign.revision[0], para_p, 2);

      strcpy( &prompt[0], "Test Date :  [");
      strncat( &prompt[0], &adc3112_sign.test_date[0], 8);
      strcat( &prompt[0], "] : "); 
      para_p = cli_get_cmd( &adc3112_history, prompt);
      if( para_p[0] == 'q') return(-1);
      if( para_p[0]) strncpy( &adc3112_sign.test_date[0], para_p, 8);

      strcpy( &prompt[0], "Calibration Date :  [");
      strncat( &prompt[0], &adc3112_sign.calib_date[0], 8);
      strcat( &prompt[0], "] : "); 
      para_p = cli_get_cmd( &adc3112_history, prompt);
      if( para_p[0] == 'q') return(-1);
      if( para_p[0]) strncpy( &adc3112_sign.calib_date[0], para_p, 8);

      strcpy( &prompt[0], "Interleave Correction Chan0 :  [");
      strncat( &prompt[0], &adc3112_sign.itl_corr[0][0], 8);
      strcat( &prompt[0], "] : "); 
      para_p = cli_get_cmd( &adc3112_history, prompt);
      if( para_p[0] == 'q') return(-1);
      if( para_p[0]) strncpy( &adc3112_sign.itl_corr[0][0], para_p, 8);

      strcpy( &prompt[0], "Interleave Correction Chan1 :  [");
      strncat( &prompt[0], &adc3112_sign.itl_corr[1][0], 8);
      strcat( &prompt[0], "] : "); 
      para_p = cli_get_cmd( &adc3112_history, prompt);
      if( para_p[0] == 'q') return(-1);
      if( para_p[0]) strncpy( &adc3112_sign.itl_corr[1][0], para_p, 8);

      strcpy( &prompt[0], "Interleave Correction Chan2 :  [");
      strncat( &prompt[0], &adc3112_sign.itl_corr[2][0], 8);
      strcat( &prompt[0], "] : "); 
      para_p = cli_get_cmd( &adc3112_history, prompt);
      if( para_p[0] == 'q') return(-1);
      if( para_p[0]) strncpy( &adc3112_sign.itl_corr[2][0], para_p, 8);

      strcpy( &prompt[0], "Interleave Correction Chan3 :  [");
      strncat( &prompt[0], &adc3112_sign.itl_corr[3][0], 8);
      strcat( &prompt[0], "] : "); 
      para_p = cli_get_cmd( &adc3112_history, prompt);
      if( para_p[0] == 'q') return(-1);
      if( para_p[0]) strncpy( &adc3112_sign.itl_corr[3][0], para_p, 8);
    }
    if( op == 2)
    {
      time_t tm;
      int cnt;
      char ct[10];

      tm = time(0);
      strftime( ct, 10, "%d%m%Y", gmtime(&tm));
      printf("current date : %s\n", ct);

      strncpy( &adc3112_sign.board_name[0], " ADC3112", 8);
      strncpy( &adc3112_sign.serial[0], "0000", 4);
      strncpy( &adc3112_sign.version[0], "00000001", 8);
      strncpy( &adc3112_sign.revision[0], "A0", 2);
      strncpy( &adc3112_sign.test_date[0], ct, 8);
      strncpy( &adc3112_sign.calib_date[0], ct, 8);
      strncpy( &adc3112_sign.itl_corr[0][0], "00004000", 8);
      strncpy( &adc3112_sign.itl_corr[1][0], "00004000", 8);
      strncpy( &adc3112_sign.itl_corr[2][0], "00004000", 8);
      strncpy( &adc3112_sign.itl_corr[3][0], "00004000", 8);

      cnt = c->cnt - 3;
      i = 3;
      while( cnt--)
      {
	char *q;

        q =  c->para[i++];
        if( p[0] == 'b')
        {
          strncpy( &adc3112_sign.board_name[0], &q[2], 8);
        }
        if( p[0] == 's')
        {
	  strncpy( &adc3112_sign.serial[0], &q[2], 4);
        }
        if( p[0] == 'v')
        {
	  strncpy( &adc3112_sign.version[0], &q[2], 8);
        }
        if( p[0] == 'r')
        {
	  strncpy( &adc3112_sign.revision[0], &q[2], 2);
        }
      }
    }
    printf("ADC3112 signature\n");
    p = (unsigned char *)&adc3112_sign.board_name[0];
    printf("Board Name :  %c%c%c%c%c%c%c%c\n", p[0],p[1],p[2],p[3],p[4],p[5],p[6],p[7]);
    p = (unsigned char *)&adc3112_sign.serial[0];
    printf("Serial Number : %c%c%c%c\n", p[0],p[1],p[2],p[3]);
    p = (unsigned char *)&adc3112_sign.version[0];
    printf("PCB Version : %c%c%c%c%c%c%c%c\n", p[0],p[1],p[2],p[3],p[4],p[5],p[6],p[7]);
    p = (unsigned char *)&adc3112_sign.revision[0];
    printf("Hardware Revision : %c%c\n", p[0],p[1]);
    p = (unsigned char *)&adc3112_sign.test_date[0];
    printf("Test Date :  %c%c%c%c%c%c%c%c\n", p[0],p[1],p[2],p[3],p[4],p[5],p[6],p[7]);
    p = (unsigned char *)&adc3112_sign.calib_date[0];
    printf("Calibration Date :  %c%c%c%c%c%c%c%c\n", p[0],p[1],p[2],p[3],p[4],p[5],p[6],p[7]);
    p = (unsigned char *)&adc3112_sign.itl_corr[0][0];
    printf("Interleave Correction Chan#0 : %c%c%c%c%c%c%c%c\n", p[0],p[1],p[2],p[3],p[4],p[5],p[6],p[7]);
    p = (unsigned char *)&adc3112_sign.itl_corr[1][0];
    printf("Interleave Correction Chan#1 : %c%c%c%c%c%c%c%c\n", p[0],p[1],p[2],p[3],p[4],p[5],p[6],p[7]);
    p = (unsigned char *)&adc3112_sign.itl_corr[2][0];
    printf("Interleave Correction Chan#2 : %c%c%c%c%c%c%c%c\n", p[0],p[1],p[2],p[3],p[4],p[5],p[6],p[7]);
    p = (unsigned char *)&adc3112_sign.itl_corr[3][0];
    printf("Interleave Correction Chan#3 : %c%c%c%c%c%c%c%c\n", p[0],p[1],p[2],p[3],p[4],p[5],p[6],p[7]);
    p = (unsigned char *)&adc3112_sign.itl_corr[4][0];
    printf("\n");

    if( op)
    {
      if( op == 1)
      {
        para_p = cli_get_cmd( &adc3112_history, "Overwrite ADC3112 signature ? [y/n] ");
        if( para_p[0] != 'y')
        {
  	  printf("EEPROM signature update aborted\n");
  	  return(-1);
        }
      }
      p = (unsigned char *)&adc3112_sign;
      for( i = 0x0; i < 0x100; i++)
      {
	data = p[i];
        tsc_i2c_write( device, tsc_swap_16(0x7000+i), data);
	usleep(5000);
      }
      printf("EEPROM signature update done\n");
    }
  }
  else if( !strcmp( "dump", c->para[1]))
  {
    int device, i, j, off, size;
    unsigned char *p, *buf;

    device = add->cmd;
    if( device != 0x01010051)
    {
      printf(" sign command not supported for that device\n");
      return(-1);
    }
    if( c->cnt < 4)
    {
      printf("adc3112 eeprom dump command needs more arguments\n");
      printf("usage: adc3112.<fmc> eeprom dump <offset> <size>\n");
      return(-1);
    }
    if( fmc == 2)
    {
      device |= 0xa0000000;
    }
    else
    {
      device |= 0x80000000;
      if( add->idx == 1) device += 1;
    }
    if( sscanf( c->para[2],"%x", &off) != 1)
    {
      printf("bad offset\n");
      printf("usage: adc3112.<fmc> eeprom dump <offset> <size>\n");
      return(-1);
    }
    if( sscanf( c->para[3],"%x", &size) != 1)
    {
      printf("bad size\n");
      printf("usage: adc3112.<fmc> eeprom dump <offset> <size>\n");
      return(-1);
    }
    printf("Displaying EEPROM from %x to %x\n", off, off+size);
    buf = (unsigned char *)malloc(size + 0x10);
    p = &buf[0];
    for( i = 0; i < size; i++)
    {
      tsc_i2c_read( device, tsc_swap_16(off+i), &data);
      p[i] = (unsigned char)data;
    }
    p = (unsigned char *)&buf[0];
    for( i = 0; i < size; i += 0x10)
    {
      for( j = 0; j < 0x10; j++)
      {
        printf("%02x ", p[i+j]);
      }
      printf("\n");
    }
  }
  else 
  {
    printf("bad operation : %s\n",  c->para[1]);
    printf("usage: adc3112.<fmc> <dev> read <reg>\n");
    printf("       adc3112.<fmc> <dev> write <reg> <data>\n");
    return(-1);
  }
  return(0);

}
