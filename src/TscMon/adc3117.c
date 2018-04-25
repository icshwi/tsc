/*=========================< begin file & file header >=======================
 *  References
 *  
 *    filename : adc3117.c
 *    author   : JFG
 *    company  : IOxOS
 *    creation : June 11,2014
 *
 *----------------------------------------------------------------------------
 *  Description
 *
 *     That file contains a set of function called by TscMon to control an
 *     ADC3117 FMC.
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

#ifndef lint
static char *rcsid = "$Id: adc3117.c,v 1.10 2016/01/15 10:21:19 ioxos Exp $";
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

#define FMC_IDX(fmc)      (fmc-1)
#define NUM_FMC 2
#define ADC_NUM_CHAN 20

#define ADC_REG_SIGN_A       0x1200
#define ADC_REG_CSR_A        0x1204
#define ADC_REG_SERIAL_A     0x120c
#define ADC_REG_SIGN_B       0x1300
#define ADC_REG_CSR_B        0x1304
#define ADC_REG_SERIAL_B     0x130c

#define ACQ_REG_ADC_CTL          0x1184
#define ACQ_REG_BUF_CTL          0x11a0

char line[256];

struct adc3117_reg
{
  int sign;
  int csr;
  int serial;
}
adc3117_reg[NUM_FMC];

struct tsc_adc3117_devices
{
  char *name;
  uint cmd;
  int idx;
  int bus;
}
adc3117_devices[] =
{
  { "adc0",    0x00000000, 0, 0},
  { "adc1",    0x00000000, 1, 0},
  { "adc2",    0x00000000, 2, 0},
  { "adc3",    0x00000000, 3, 0},
  { "adc4",    0x00000000, 4, 0},
  { "adc5",    0x00000000, 5, 0},
  { "adc6",    0x00000000, 6, 0},
  { "adc7",    0x00000000, 7, 0},
  { "adc8",    0x00000000, 8, 0},
  { "adc9",    0x00000000, 9, 0},
  { "adc10",   0x00000000, 10, 0},
  { "adc11",   0x00000000, 11, 0},
  { "adc12",   0x00000000, 12, 0},
  { "adc13",   0x00000000, 13, 0},
  { "adc14",   0x00000000, 14, 0},
  { "adc15",   0x00000000, 15, 0},
  { "adc16",   0x00000000, 16, 0},
  { "adc17",   0x00000000, 17, 0},
  { "adc18",   0x00000000, 18, 0},
  { "adc19",   0x00000000, 19, 0},
  { "dac0",    0x00000000, 0, BUS_SPI|BUS_READ|BUS_WRITE},
  { "dac1",    0x00000000, 0, BUS_SPI|BUS_READ|BUS_WRITE},
  { "tmp102",  0x40040048, 0, BUS_I2C},
  { "eeprom",  0x40010050, 1, BUS_I2C},
  { NULL, 0}
};

struct adc3117_sign
{
  char board_name[8];   /*  0x00  [0] */
  char serial[4];       /*  0x08  [8] */
  char version[8];      /*  0x0c  [12] */
  char revision[2];     /*  0x14  [20] */
  char rsv[6];          /*  0x16  [22] */
  char test_date[8];    /*  0x1c  [28] */

  char pad[216];        /*  0x24  [36] */
  int cks;              /*  0xfc [252] */
} adc3117_sign;         /* 0x100 [256] */

struct adc3117_acq_res
{
  struct adc3117_acq_chan
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

struct adc3117_vref
{
  int magic;
  int len;
  int cks;
  float temp;
  struct vref_calib
  {
    int dac;
    float volt;
  } vref[32];
} adc3117_vref;

#define I2C_CTL_EXEC_IDLE  0x00000000
#define I2C_CTL_EXEC_RUN   0x00100000
#define I2C_CTL_EXEC_DONE  0x00200000
#define I2C_CTL_EXEC_ERR   0x00300000
#define I2C_CTL_EXEC_MASK  0x00300000

int adc3117_init_flag[NUM_FMC] = {0,0};
struct cli_cmd_history adc3117_history;
char adc3117_prompt[32];
struct tsc_ioctl_map_win adc3117_mas_map_win;

extern int tsc_fd;

char *
adc3117_rcsid()
{
  return( rcsid);
}

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * Function name : adc3117_init
 * Prototype     : void
 * Parameters    : fmc
 * Return        : void
 *----------------------------------------------------------------------------
 * Description   : adc3117 init
 *
 *++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

void 
adc3117_init( int fmc)
{
  int fmc_idx;

  //printf(" entering adc3117_init(%d)\n", fmc);
  fmc_idx = fmc - 1;
  if( !adc3117_init_flag[fmc_idx])
  {
    //printf("performing adc3117 initialization: %d\n", fmc_idx);
    cli_history_init( &adc3117_history);
    adc3117_init_flag[fmc_idx] = 1;
    if( fmc_idx)
    {
      adc3117_reg[fmc_idx].sign   = ADC_REG_SIGN_B;
      adc3117_reg[fmc_idx].csr    = ADC_REG_CSR_B;
      adc3117_reg[fmc_idx].serial = ADC_REG_SERIAL_B;
      tscext_csr_wr(tsc_fd, 0x1320,1);
      tscext_csr_wr(tsc_fd, 0x1324,0xfffff000);
      tscext_csr_wr(tsc_fd, 0x1328,5);
    }
    else
    {
      adc3117_reg[fmc_idx].sign   = ADC_REG_SIGN_A;
      adc3117_reg[fmc_idx].csr    = ADC_REG_CSR_A;
      adc3117_reg[fmc_idx].serial = ADC_REG_SERIAL_A;
      tscext_csr_wr(tsc_fd, 0x1220,1);
      tscext_csr_wr(tsc_fd, 0x1224,0xfffff000);
      tscext_csr_wr(tsc_fd, 0x1228,5);
   }
  }
  /* start ADC scanning for all channels */
  tscext_csr_wr(tsc_fd, ACQ_REG_ADC_CTL, 0x3ff000);
  tscext_csr_wr(tsc_fd, ACQ_REG_ADC_CTL+4, 2);
  return;
}

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * Function name : adc3117_map_usr
 * Prototype     : char *
 * Parameters    : map win structure, remote address, size, user
 * Return        : success/error
 *----------------------------------------------------------------------------
 * Description   : map user space
 *
 *++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

char *
adc3117_map_usr( struct tsc_ioctl_map_win *map,
		         uint64_t rem_addr,
		 uint size,
		 int usr)
{
  bzero( map, sizeof( struct tsc_ioctl_map_win));
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
  tsc_map_alloc(tsc_fd, map);
  return( (char *)tsc_pci_mmap(tsc_fd, map->sts.loc_base, map->sts.size));
}

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * Function name : adc3117_unmap_usr
 * Prototype     : int
 * Parameters    : map win structure, buffer pointer
 * Return        : success/error
 *----------------------------------------------------------------------------
 * Description   : unmap user space
 *
 *++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

int
adc3117_unmap_usr( struct tsc_ioctl_map_win *map,
		   char *buf_ptr)
{
  tsc_pci_munmap( buf_ptr, map->sts.size);
  return( tsc_map_free(tsc_fd, map));
}

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * Function name : adc3117_trig_acq
 * Prototype     : int
 * Parameters    : register base, trigger
 * Return        : 1
 *----------------------------------------------------------------------------
 * Description   : trig an acquisition
 *
 *++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

int
adc3117_trig_acq( int reg_base,
		  int trig)
{
  //printf("Trig data acquisition...\n");
  tscext_csr_wr(tsc_fd, reg_base + 0x0, 0x80040001);/* FASTSCOPE Abort/Stop previous acquisition SRAM1 (in case of..) */
  tscext_csr_wr(tsc_fd, reg_base + 0x4, trig);      /* FASTSCOPE Define trigger mode */
  tscext_csr_wr(tsc_fd, reg_base + 0x0, 0x40040001);/* Arm data acquisition SRAM1 */
  usleep( 2000);
  tscext_csr_wr(tsc_fd, reg_base + 0x8, 0x40000000);/* FASTSCOPE Manual trigger command */
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
 * Function name : adc3117_acq_res
 * Prototype     : int
 * Parameters    : adc3117 structure
 * Return        : 0
 *----------------------------------------------------------------------------
 * Description   : adc3117 acquisition
 *
 *++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

int 
adc3117_acq_res( struct adc3117_acq_res *r,
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
    if( tsc_swap_16(buf[i]) < r->chan[chan].min)
    {
      r->chan[chan].min =  tsc_swap_16(buf[i]);
      r->chan[chan].off_min = i;
    }
    if(  tsc_swap_16(buf[i]) > r->chan[chan].max)
    {
      r->chan[chan].max =  tsc_swap_16(buf[i]);
      r->chan[chan].off_max = i;
    }
    r->chan[chan].mean += (float) tsc_swap_16(buf[i]);
  }
  r->chan[chan].mean = r->chan[chan].mean/size;

  for( i = 0; i < size; i++)
  {
    r->chan[chan].sig += ( tsc_swap_16(buf[i]) - r->chan[chan].mean)*( tsc_swap_16(buf[i]) - r->chan[chan].mean);
  }
  r->chan[chan].sig = sqrt(r->chan[chan].sig/size);

  return(0);
}

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * Function name : adc3117_acq
 * Prototype     : int
 * Parameters    : cli command parameter structure, identifier, fmc, size, check
 * Return        : success/error
 *----------------------------------------------------------------------------
 * Description   : adc3117 command
 *
 *++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

int
adc3117_acq( struct cli_cmd_para *c,
	     int idx,
	     int fmc)
{
  int tmo;
  char *adc_buf;
  struct adc3117_acq_res acq_res;
  short *data_p;

  printf("Channel %d trig acquisition\n", idx);
  tscext_csr_wr(tsc_fd, ACQ_REG_BUF_CTL, idx/2);
  tscext_csr_wr(tsc_fd, ACQ_REG_BUF_CTL+4, 2);
  tmo = 100000;
  while( --tmo)
  {
    if( !(tscext_csr_rd(tsc_fd, ACQ_REG_BUF_CTL+4) & 0x10)) break;
  }
  adc_buf = adc3117_map_usr( &adc3117_mas_map_win, 0x100000, 0x100000, fmc);
  printf("Processing data channel %d...\n", idx);
  data_p = (short *)(adc_buf + (0x1000*idx));
  adc3117_acq_res( &acq_res, &data_p[0], 0x800, idx);
  printf("Channel %d : %f +/- %f [%d (0x%x) : %d (0x%x) ]\n", idx,
	     acq_res.chan[idx].mean, acq_res.chan[idx].sig, acq_res.chan[idx].min, 
             2*acq_res.chan[idx].off_min, acq_res.chan[idx].max, 2*acq_res.chan[idx].off_max);
  adc3117_unmap_usr( &adc3117_mas_map_win, adc_buf);

  return(0);
}

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * Function name : adc3117_save
 * Prototype     : int
 * Parameters    : cli command parameter structure, identifier, fmc
 * Return        : success/error
 *----------------------------------------------------------------------------
 * Description   : save an acquisition
 *
 *++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

int
adc3117_save( struct cli_cmd_para *c,
		     int idx,
		     int fmc)
{
  int i, cnt;
  char *adc_buf, *p;
  char *name_h, *name_d;
  char *acq_name_h, *acq_name_d;
  FILE *acq_file_h, *acq_file_d;
  char *default_name = "acq_file";
  struct adc3117_acq_res acq_res;
  int file_his, file_dat;
  short *data_p;
  int offset, size;

  name_h = NULL;
  name_d = NULL;
  file_his = 0;
  file_dat = 0;
  offset = 0;
  size = 0x800;
  cnt = c->cnt - 2;
  i = 2;
  while( cnt--)
  {
    p =  c->para[i++];
    if( p[0] == 'o')
    {
      sscanf( p, "o:%d", &offset);
    }
    if( p[0] == 's')
    {
      sscanf( p, "s:%d", &size);
    }
    if( p[0] == 'h')
    {
      file_his = 1;
      if( strlen(p) > 2)
      {
	name_h = &p[2];
      }
    }
    if( p[0] == 'd')
    {
      file_dat = 1;
      if( strlen(p) > 2)
      {
	name_d = &p[2];
      }
    }
  }
  if( file_his)
  {
    if( !name_h)
    {
      name_h = default_name;
    }
    acq_name_h = (char *)malloc( strlen( name_h) + 8);
    sprintf( acq_name_h, "%s_%d.his", name_h, idx);
    acq_file_h = fopen( acq_name_h, "w");
    if( !acq_file_h)
    {
      printf("cannot create acquisition file %s\n", acq_name_h);
      free( acq_name_h);
      return( -1);
    }
  }
  if( file_dat)
  {
    if( !name_d)
    {
      name_d = default_name;
    }
    acq_name_d = (char *)malloc( strlen( name_d) + 8);
    sprintf( acq_name_d, "%s_%d.csv", name_d, idx);
    acq_file_d = fopen( acq_name_d, "w");
    if( !acq_file_d)
    {
      printf("cannot create acquisition file %s\n", acq_name_d);
      free( acq_name_d);
      return( -1);
    }
  }
  adc_buf = adc3117_map_usr( &adc3117_mas_map_win, 0x100000, 0x100000, fmc);
  printf("Save acquired data...\n");
  data_p = (short *)(adc_buf + (0x1000*idx));
  for( i = offset; i < offset+size; i++)
  {
    if( file_his)
    {
      fprintf( acq_file_h, "%d, %d\n", i, tsc_swap_16(data_p[i]));
    }
    if( file_dat)
    {
      fprintf( acq_file_d, "%d\n", tsc_swap_16(data_p[i]));
    }
  }
  printf("Processing data channel %d...\n", idx);
  adc3117_acq_res( &acq_res, &data_p[offset], size, idx);
  printf("Channel %d : %f +/- %f [%d (0x%x) : %d (0x%x) ]\n", idx,
	     acq_res.chan[idx].mean, acq_res.chan[idx].sig, acq_res.chan[idx].min, 
             2*acq_res.chan[idx].off_min, acq_res.chan[idx].max, 2*acq_res.chan[idx].off_max);

  printf("Transfering histogramming files to host...");
  fflush( stdout);
  if( file_his)
  {
    fclose( acq_file_h);
  }
  if( file_dat)
  {
    fclose( acq_file_d);
  }
  printf(" -> done\n");
  adc3117_unmap_usr( &adc3117_mas_map_win, adc_buf);
  return(0);
}

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * Function name : adc3117_eeprom_sign
 * Prototype     : int
 * Parameters    : cli command parameter structure, device, fmc
 * Return        : success/error
 *----------------------------------------------------------------------------
 * Description   : signature of eeprom
 *
 *++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

int
adc3117_eeprom_sign( struct cli_cmd_para *c,
		     int device,
		     int fmc)
{
    int i;
    unsigned char *p;
    int op;
    char *para_p;
    uint data;

    if( device != 0x40010050)
    {
      printf(" sign command not supported for that device\n");
      return(-1);
    }
    if( fmc == 2)
    {
      device |= 2;
    }
    p = (unsigned char *)&adc3117_sign;
    for( i = 0x0; i < 0x100; i++)
    {
      tsc_i2c_read(tsc_fd, device, 0x7000 + i, &data);
      //tsc_i2c_read( device, tsc_swap_16( 0x7000 + i), &data);
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

      bzero( &adc3117_history, sizeof( struct cli_cmd_history));
      cli_history_init( &adc3117_history);
      printf("setting ADC3117 signature\n");
      para_p = cli_get_cmd( &adc3117_history, "Enter password ->  ");
      if( strcmp(  para_p, "goldorak"))
      {
	printf("wrong password\n");
	return(-1);
      }

      strcpy( &prompt[0], "Board Name [");
      strncat( &prompt[0], &adc3117_sign.board_name[0], 8);
      strcat( &prompt[0], "] : "); 
      para_p = cli_get_cmd( &adc3117_history, prompt);
      if( para_p[0] == 'q') return(-1);
      if( para_p[0]) strncpy( &adc3117_sign.board_name[0], para_p, 8);


      strcpy( &prompt[0], "Serial Number [");
      strncat( &prompt[0], &adc3117_sign.serial[0], 4);
      strcat( &prompt[0], "] : "); 
      para_p = cli_get_cmd( &adc3117_history, prompt);
      if( para_p[0] == 'q') return(-1);
      if( para_p[0]) strncpy( &adc3117_sign.serial[0], para_p, 4);

      strcpy( &prompt[0], "PCB Version :  [");
      strncat( &prompt[0], &adc3117_sign.version[0], 8);
      strcat( &prompt[0], "] : "); 
      para_p = cli_get_cmd( &adc3117_history, prompt);
      if( para_p[0] == 'q') return(-1);
      if( para_p[0]) strncpy( &adc3117_sign.version[0], para_p, 8);

      strcpy( &prompt[0], "Hardware Revision :  [");
      strncat( &prompt[0], &adc3117_sign.revision[0], 2);
      strcat( &prompt[0], "] : "); 
      para_p = cli_get_cmd( &adc3117_history, prompt);
      if( para_p[0] == 'q') return(-1);
      if( para_p[0]) strncpy( &adc3117_sign.revision[0], para_p, 2);

      strcpy( &prompt[0], "Test Date :  [");
      strncat( &prompt[0], &adc3117_sign.test_date[0], 8);
      strcat( &prompt[0], "] : "); 
      para_p = cli_get_cmd( &adc3117_history, prompt);
      if( para_p[0] == 'q') return(-1);
      if( para_p[0]) strncpy( &adc3117_sign.test_date[0], para_p, 8);

    }
    if( op == 2)
    {
      time_t tm;
      int cnt;
      char ct[10];

      tm = time(0);
      strftime( ct, 10, "%d%m%Y", gmtime(&tm));
      printf("current date : %s\n", ct);

      strncpy( &adc3117_sign.board_name[0], " ADC3117", 8);
      strncpy( &adc3117_sign.serial[0], "0000", 4);
      strncpy( &adc3117_sign.version[0], "00000001", 8);
      strncpy( &adc3117_sign.revision[0], "A0", 2);
      strncpy( &adc3117_sign.test_date[0], ct, 8);

      cnt = c->cnt - 3;
      i = 3;
      while( cnt--)
      {
	char *q;

        q =  c->para[i++];
        if( p[0] == 'b')
        {
          strncpy( &adc3117_sign.board_name[0], &q[2], 8);
        }
        if( p[0] == 's')
        {
	  strncpy( &adc3117_sign.serial[0], &q[2], 4);
        }
        if( p[0] == 'v')
        {
	  strncpy( &adc3117_sign.version[0], &q[2], 8);
        }
        if( p[0] == 'r')
        {
	  strncpy( &adc3117_sign.revision[0], &q[2], 2);
        }
      }
    }
    printf("ADC3117 signature\n");
    p = (unsigned char *)&adc3117_sign.board_name[0];
    printf("Board Name :  %c%c%c%c%c%c%c%c\n", p[0],p[1],p[2],p[3],p[4],p[5],p[6],p[7]);
    p = (unsigned char *)&adc3117_sign.serial[0];
    printf("Serial Number : %c%c%c%c\n", p[0],p[1],p[2],p[3]);
    p = (unsigned char *)&adc3117_sign.version[0];
    printf("PCB Version : %c%c%c%c%c%c%c%c\n", p[0],p[1],p[2],p[3],p[4],p[5],p[6],p[7]);
    p = (unsigned char *)&adc3117_sign.revision[0];
    printf("Hardware Revision : %c%c\n", p[0],p[1]);
    p = (unsigned char *)&adc3117_sign.test_date[0];
    printf("Test Date :  %c%c%c%c%c%c%c%c\n", p[0],p[1],p[2],p[3],p[4],p[5],p[6],p[7]);
    printf("\n");

    if( op)
    {
      if( op == 1)
      {
        para_p = cli_get_cmd( &adc3117_history, "Overwrite ADC3117 signature ? [y/n] ");
        if( para_p[0] != 'y')
        {
  	  printf("EEPROM signature update aborted\n");
  	  return(-1);
        }
      }
      p = (unsigned char *)&adc3117_sign;
      for( i = 0x0; i < 0x100; i++)
      {
	data = p[i];
        tsc_i2c_write(tsc_fd, device, 0x7000+i, data);
        //tsc_i2c_write( device, tsc_swap_16(0x7000+i), data);
	usleep(5000);
      }
      printf("EEPROM signature update done\n");
    }
  return(0);
}

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * Function name : adc3117_eeprom_dump
 * Prototype     : int
 * Parameters    : cli command parameter, device, fmc
 * Return        : success/error
 *----------------------------------------------------------------------------
 * Description   : dump eeprom
 *
 *++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

int
adc3117_eeprom_dump( struct cli_cmd_para *c,
		     int device,
		     int fmc)
{
  int i, j, off, size;
  unsigned char *p, *buf;
  uint data;

  if( device != 0x40010050)
  {
    printf(" dump command not supported for that device\n");
    return(-1);
  }
  if( c->cnt < 4)
  {
    printf("adc3117 eeprom dump command needs more arguments\n");
    printf("usage: adc3117.<fmc> eeprom dump <offset> <size>\n");
    return(-1);
  }
  if( fmc == 2)
  {
    device |= 2;
  }
  if( sscanf( c->para[2],"%x", &off) != 1)
  {
    printf("bad offset\n");
    printf("usage: adc3117.<fmc> eeprom dump <offset> <size>\n");
    return(-1);
  }
  if( sscanf( c->para[3],"%x", &size) != 1)
  {
    printf("bad size\n");
    printf("usage: adc3117.<fmc> eeprom dump <offset> <size>\n");
    return(-1);
  }
  printf("Displaying EEPROM from %x to %x\n", off, off+size);
  buf = (unsigned char *)malloc(size + 0x10);
  p = &buf[0];
  for( i = 0; i < size; i++)
  {
    data = 0;
    //tsc_i2c_read( device, tsc_swap_16(off+i), &data);
    tsc_i2c_read(tsc_fd, device, off+i, &data);
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
  return(0);
}

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * Function name : adc3117_eeprom_vref
 * Prototype     : int
 * Parameters    : cli command parameter, device, fmc
 * Return        : success/error
 *----------------------------------------------------------------------------
 * Description   : vref management
 *
 *++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

int
adc3117_eeprom_vref( struct cli_cmd_para *c,
		     int device,
		     int fmc)
{
  char *p;
  int data, i;

  if( fmc == 2)
  {
    device |= 2;
  }
  if( !strcmp( "show", c->para[2]))
  {
    printf("DAC vref calibration\n");
    p = (char *)&adc3117_vref;
    if( *(int *)p != 0x56524546)
    {
      printf("No valid calibration data available!!\n");
      return(-1);
    }
    printf("Temperature = %f\n", adc3117_vref.temp);
    printf(" DAC | VOLT\n");
    for( i = 0; i < 32; i++)
    {
      printf(" %03d | %f\n", adc3117_vref.vref[i].dac, adc3117_vref.vref[i].volt);
    }
  }
  else if( !strcmp( "store", c->para[2]))
  {
    printf("Storing DAC vref calibration in EEPROM...");
    p = (char *)&adc3117_vref;
    if( *(int *)p != 0x56524546)
    {
      printf("Need valid calibration data !!\n");
      return(-1);
    }
    for( i = 0; i < sizeof( struct adc3117_vref); i++)
    {
      data = p[i];
      tsc_i2c_write(tsc_fd, device, 0x6000+i, data);
      usleep(5000);
    }
    printf("Done\n");
  }
  else if( !strcmp( "load", c->para[2]))
  {
    printf("Loading DAC vref calibration from EEPROM...");
    p = (char *)&adc3117_vref;
    for( i = 0; i < sizeof( struct adc3117_vref); i++)
    {
      data = 0;
      tsc_i2c_read(tsc_fd, device, 0x6000+i, &data);
      p[i] = (char)data;
    }
    printf("Done\n");
  }
  else
  {
    FILE *ref_file;

    if( c->cnt < 4)
    {
      printf("Needs file name...\n");
      return(-1);
    }
    ref_file = fopen( c->para[3], "r");
    if( !ref_file)
    {
      printf("Cannot open file %s\n", c->para[3]);
      return(-1);
    }
    if( !strcmp( "read", c->para[2]))
    {
      printf("Reading DAC vref calibration from file %s\n", c->para[3]);
      fgets( line, 256, ref_file);
      if( sscanf(line, "%f", &adc3117_vref.temp) != 1)
      {
	printf("formatting error!!\n");
	fclose( ref_file);
	return(-1);
      }
      i = 0;
      while( fgets( line, 256, ref_file) && (i<32))
      {
	if( sscanf(line, "%d, %f", &adc3117_vref.vref[i].dac, &adc3117_vref.vref[i].volt) != 2)
	{
	  printf("formatting error!!\n");
	  fclose( ref_file);
	  return(-1);
	}
	i++;
      }
      adc3117_vref.magic = 0x56524546;
      adc3117_vref.len = sizeof( struct adc3117_vref);
      adc3117_vref.cks = 0;
      fclose( ref_file);
    }
    else if( !strcmp( "write", c->para[2]))
    {
      printf("Writing DAC vref calibration to file %s...", c->para[3]);
      if( adc3117_vref.magic != 0x56524546)
      {
        printf("No valid calibration data available!!\n");
        fclose( ref_file);
        return(-1);
      }
      fprintf( ref_file, "%f\n", adc3117_vref.temp);
      for( i = 0; i < 32; i++)
      {
        fprintf( ref_file, "%d, %f\n", adc3117_vref.vref[i].dac, adc3117_vref.vref[i].volt);
      }
      fclose( ref_file);
      printf("Done\n");
    }
  }
}

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * Function name : tsc_adc3117
 * Prototype     : int
 * Parameters    : cli command parameter structure
 * Return        : success/error
 *----------------------------------------------------------------------------
 * Description   : adc3117 command
 *
 *++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

int 
tsc_adc3117( struct cli_cmd_para *c)
{
  struct tsc_adc3117_devices *add;
  uint fmc;
  char *p;
  uint id;

  if( c->cnt < 2)
  {
    printf("adc3117 command needs more arguments\n");
    printf("usage: adc3117.<fmc> <dev> <op> <reg> [<data>]\n");
    printf("adc3117 device list:\n");
    add = &adc3117_devices[0];
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
    printf("usage: adc3117.<fmc> <dev> <op> <reg> [<data>]\n");
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
  if( fmc == 1)
  {
    id = tscext_csr_rd(tsc_fd, ADC_REG_SIGN_A);
    if( ( id & 0xffff0000) != 0x31170000)
    {
      printf("no ADC3117 installed on FMC#1 [%08x] !!\n", id);
      return(-1);
    }
    //printf("adc3117 identifier = %08x\n", id);
  }
  if( fmc == 2)
  {
    id = tscext_csr_rd(tsc_fd, ADC_REG_SIGN_B);
    if( ( id & 0xffff0000) != 0x31170000)
    {
      printf("no ADC3117 installed on FMC#2 [%08x] !!\n", id);
      return(-1);
    }
    //printf("adc3117 identifier = %08x\n", id);
  }
  add = &adc3117_devices[0];
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
    printf("usage: adc3117.<fmc> <dev> <op> <reg> [<data>]\n");
    printf("adc3117 device list:\n");
    add = &adc3117_devices[0];
    while( add->name)
    {
      printf("   - %s\n", add->name);
      add++;
    }
    return(-1);
  }
  adc3117_init( fmc);
  if( !strcmp( "read", c->para[1]))
  {
  }
  else if( !strcmp( "write", c->para[1]))
  {
  }
  else if( !strcmp( "acq", c->para[1]))
  {
    adc3117_acq( c, add->idx, fmc);
  }
  else if( !strcmp( "save", c->para[1]))
  {
    adc3117_save( c, add->idx, fmc);
  }
  else if( !strcmp( "show", c->para[1]))
  {
    int status;
    int device;
    uint temp, ctl, lo, hi;

    device = add->cmd;
    if( device != 0x40040048)
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
    status = tsc_i2c_read(tsc_fd, device, 1, &ctl);
    if( (status & I2C_CTL_EXEC_MASK) == I2C_CTL_EXEC_ERR)
    {
      printf("%s: ctl register -> error = %08x\n", add->name, status);
    }
    else
    {
      tsc_i2c_read(tsc_fd, device, 0, &temp);
      tsc_i2c_read(tsc_fd, device, 2, &lo);
      tsc_i2c_read(tsc_fd, device, 3, &hi);
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
  else if( !strcmp( "sign", c->para[1]))
  {
    adc3117_eeprom_sign( c, add->cmd, fmc);
  }
  else if( !strcmp( "dump", c->para[1]))
  {
    adc3117_eeprom_dump( c, add->cmd, fmc);
  }
  else if( !strcmp( "vref", c->para[1]))
  {
    if( add->cmd == 0x40010050)
    {
      adc3117_eeprom_vref( c, add->cmd, fmc);
    }
  }
  else 
  {
    printf("bad operation : %s\n",  c->para[1]);
    printf("usage: adc3117.<fmc> <dev> read <reg>\n");
    printf("       adc3117.<fmc> <dev> write <reg> <data>\n");
    return(-1);
  }
  return( 0);
}
