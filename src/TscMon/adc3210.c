/*=========================< begin file & file header >=======================
 *  References
 *  
 *    filename : adc3210.c
 *    author   : JFG
 *    company  : IOxOS
 *    creation : november 14,2011
 *
 *----------------------------------------------------------------------------
 *  Description
 *
 *     That file contains a set of function called by TscMon to
 *     control adc3210.
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
static char *rcsid = "$Id: adc3210.c,v 1.10 2014/12/19 09:36:19 ioxos Exp $";
#endif
#define TSC

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
#include <adclib.h>
#include <adc3210lib.h>
#include <lmk04616.h>

#define BUS_SPI 1
#define BUS_I2C 2
#define BUS_TCSR 3


struct tsc_adc3210_devices
{
  char *name;
  uint cmd;
  int idx;
  int bus;
}
#define ADC3210_DEV_ADS                  0x100
#define ADC3210_DEV_ADS01  (ADC3210_ADS | 0x0)
#define ADC3210_DEV_ADS23  (ADC3210_ADS | 0x1)
#define ADC3210_DEV_ADS45  (ADC3210_ADS | 0x2)
#define ADC3210_DEV_ADS67  (ADC3210_ADS | 0x3)
#define ADC3210_DEV_LMK                  0x200
#define ADC3210_DEV_DAC                  0x300
#define ADC3210_DEV_DAC0   (ADC3210_DAC | 0x0)
#define ADC3210_DEV_DAC1   (ADC3210_DAC | 0x1)
#define ADC3210_DEV_DAC2   (ADC3210_DAC | 0x2)

adc3210_devices[] = 
{
    { "ads01", ADC3210_SPI_ADS01, 0, BUS_SPI},
    { "ads23", ADC3210_SPI_ADS23, 1, BUS_SPI},
    { "ads45", ADC3210_SPI_ADS45, 2, BUS_SPI},
    { "ads67", ADC3210_SPI_ADS67, 3, BUS_SPI},
    { "lmk",  ADC3210_SPI_LMK, 0x10, BUS_SPI},
    { "dac0", ADC3210_SPI_DAC0, 0x80000000, BUS_SPI},
    { "dac1", ADC3210_SPI_DAC1, 0x80000001, BUS_SPI},
    { "dac2", ADC3210_SPI_DAC2, 0x80000002, BUS_SPI},
    { "tadc", ADC3210_I2C_TADC, 0, BUS_I2C},
    { "vcxo", ADC3210_I2C_VCXO, 0, BUS_I2C},
    { "tcsr", 0, 0, BUS_TCSR},
    { NULL, 0}
};

int adc3210_init_flag = 0;
extern int script_exit;
extern int tsc_fd;

char *
adc3210_rcsid()
{
  return( rcsid);
}

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * Function name : adc3210_save
 * Prototype     : void
 * Parameters    : cli command parameter structure
 * Return        : void
 *----------------------------------------------------------------------------
 * Description   : read
 *
 *++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

int 
adc3210_save( struct cli_cmd_para *c)
{
  int smem;
  int base, offset, size;
  char *p;
  unsigned char *acq_buf;
  FILE *acq_file;
  struct tsc_ioctl_map_win shm_mas_map_win;
  int i, n;
  unsigned short data;

  smem = strtoul( c->ext, &p, 16);
  if( c->cnt < 5)
  {
    printf("adc3210 save command needs more arguments\n");
    printf("usage: adc3110.<smem> <filename> save <base> <size> <offset>\n");
    return(-1);
  }
  if( sscanf( c->para[2],"%x", &base) != 1)
  {
    printf("wrong base value\n");
    printf("usage: adc3110.<smem> <filename> save <base> <size> <offset>\n");
    return(-1);
  }
  if( sscanf( c->para[3],"%x", &size) != 1)
  {
    printf("wrong size value\n");
    printf("usage: adc3110.<smem> <filename> save <base> <size> <offset>\n");
    return(-1);
  }
  if( sscanf( c->para[4],"%x", &offset) != 1)
  {
    printf("wrong offset value\n");
    printf("usage: adc3110.<smem> <filename> save <base> <size> <offset>\n");
    return(-1);
  }
  printf("save data from offset %x [%x] to file %s\n", base+offset, size, c->para[0]);
		
  acq_buf = NULL;
  if( size > 0)
  {
    acq_file = fopen( c->para[0], "w");
    if( !acq_file)
    {
      printf("cannot create acquisition file %s\n", c->para[0]);
      return( -1);
    }
    bzero( &shm_mas_map_win, sizeof(shm_mas_map_win));
    shm_mas_map_win.req.mode.sg_id= MAP_ID_MAS_PCIE_PMEM;
    shm_mas_map_win.req.mode.space = MAP_SPACE_SHM;
    if( smem == 2)
    {
      shm_mas_map_win.req.mode.space = MAP_SPACE_SHM2;
    }
    shm_mas_map_win.req.rem_addr = base;
    shm_mas_map_win.req.size = size;
    tsc_map_alloc(tsc_fd, &shm_mas_map_win);
    acq_buf = (unsigned char *)tsc_pci_mmap(tsc_fd, shm_mas_map_win.sts.loc_base, shm_mas_map_win.sts.size);
    n = 0;
    if( acq_buf)
    {
      for( i = offset; i < size; i += 2)
      {
	data = *(unsigned short *)&acq_buf[i];
	fprintf( acq_file, "%4d %6d\n", n, data);
	n++;
      }
      for( i = 0; i < offset; i += 2)
      {
	data = *(unsigned short *)&acq_buf[i];
	fprintf( acq_file, "%4d %6d\n", n, data);
	n++;
      }
      tsc_pci_munmap( acq_buf, shm_mas_map_win.sts.size);
    }
    tsc_map_free(tsc_fd, &shm_mas_map_win);

    fclose( acq_file);
  }
  return(0);
}

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * Function name : adc3210_read
 * Prototype     : void
 * Parameters    : cli command parameter structure
 * Return        : void
 *----------------------------------------------------------------------------
 * Description   : read
 *
 *++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

int 
adc3210_read( struct cli_cmd_para *c,
	      struct tsc_adc3210_devices *add)
{
  int fmc, reg, data;
  char *p;

  fmc = strtoul( c->ext, &p, 16);
  if( c->cnt < 3)
  {
    printf("adc3210 read command needs more arguments\n");
    printf("usage: adc3210.<fmc> <dev> read <reg>\n");
    return(-1);
  }
  printf("%s.%s %s %s %s\n", c->cmd, c->ext, c->para[0], c->para[1], c->para[2]);
  if( sscanf( c->para[2],"%x", &reg) != 1)
  {
    printf("wrong register number\n");
    printf("usage: adc3210.<fmc> <dev> read <reg>\n");
    return(-1);
  }
  if( add->bus == BUS_SPI)
  {
    data = adc3210_spi_read(tsc_fd, fmc, add->cmd, reg);
    if( add->idx & 0x80000000)
    {
      data = adc3210_spi_read(tsc_fd, fmc, add->cmd, reg);
    }
    printf("cmd = %08x - data = %08x\n", add->cmd, data);
  }
  if( add->bus == BUS_TCSR)
  {
    data = adc3210_csr_rd(tsc_fd, fmc, reg);
    printf("reg = %08x - data = %08x\n", reg, data);
  }
  if( add->bus == BUS_I2C)
  {
    data = adc3210_i2c_read(tsc_fd, fmc,add->cmd);
    printf("reg = %08x - data = %08x\n", reg, data);
  }
  return(0);
}

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * Function name : adc3210_rcmp
 * Prototype     : void
 * Parameters    : cli command parameter structure
 * Return        : void
 *----------------------------------------------------------------------------
 * Description   : read
 *
 *++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

int 
adc3210_rcmp( struct cli_cmd_para *c,
	      struct tsc_adc3210_devices *add)
{
  int fmc, reg, data, cmp, mask;
  char *p;

  fmc = strtoul( c->ext, &p, 16);
  if( c->cnt < 5)
  {
    printf("adc3210 rcmp command needs more arguments\n");
    printf("usage: adc3210.<fmc> <dev> rcmp <reg> <cmp> <mask>\n");
    return(-1);
  }
  printf("%s.%s %s %s %s %s %s\n", c->cmd, c->ext, c->para[0], c->para[1], c->para[2], c->para[3], c->para[4]);
  if( sscanf( c->para[2],"%x", &reg) != 1)
   {
    printf("wrong register number\n");
    printf("usage: adc3210.<fmc> <dev> rcmp <reg> <cmp> <mask>\n");
    return(-1);
  }
  if( sscanf( c->para[3],"%x", &cmp) != 1)
  {
    printf("wrong compare value\n");
    printf("usage: adc3210.<fmc> <dev> rcmp <reg> <cmp> <mask>\n");
    return(-1);
  }
  if( sscanf( c->para[4],"%x", &mask) != 1)
  {
    printf("wrong mask value\n");
    printf("usage: adc3210.<fmc> <dev> rcmp <reg> <cmp> <mask>\n");
    return(-1);
  }
  if( add->bus == BUS_SPI)
  {
    data = adc3210_spi_read(tsc_fd, fmc, add->cmd, reg);
    if( add->idx & 0x80000000)
    {
      data = adc3210_spi_read(tsc_fd, fmc, add->cmd, reg);
    }
    printf("cmd = %08x - data = %08x\n", add->cmd, data);
  }
  if( add->bus == BUS_TCSR)
  {
    data = adc3210_csr_rd(tsc_fd, fmc, reg);
    printf("reg = %08x - data = %08x\n", reg, data);
  }
  if( add->bus == BUS_I2C)
  {
    data = adc3210_i2c_read(tsc_fd, fmc,add->cmd);
    printf("reg = %08x - data = %08x\n", reg, data);
  }
  printf("%04x :  %08x - %08x [%08x]", reg, cmp, data, mask);
  script_exit = 0;
  if( (data & mask) != (cmp & mask)) script_exit = 1;
  if( script_exit) printf(" -> ERROR !!!\n");
  else  printf("\n");

  return(0);
}

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * Function name : adc3210_write
 * Prototype     : void
 * Parameters    : cli command parameter structure
 * Return        : void
 *----------------------------------------------------------------------------
 * Description   : read
 *
 *++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

int 
adc3210_write( struct cli_cmd_para *c,
	       struct tsc_adc3210_devices *add)
{
  int fmc, reg, data;
  char *p;

  fmc = strtoul( c->ext, &p, 16);
  if( c->cnt < 3)
  {
    printf("adc3210 write command needs more arguments\n");
    printf("usage: adc3210.<fmc> <dev> read <reg> <data>\n");
    return(-1);
  }
  printf("%s.%s %s %s %s %s\n", c->cmd, c->ext, c->para[0], c->para[1], c->para[2], c->para[3]);
  if( sscanf( c->para[2],"%x", &reg) != 1)
  {
    printf("wrong register number\n");
    printf("usage: adc3210.<fmc> <dev> write <reg> <data>\n");
    return(-1);
  }
  if( sscanf( c->para[3],"%x", &data) != 1)
  {
    printf("wrong data format\n");
    printf("usage: adc3210.<fmc> <dev> write <reg> <data>\n");
    return(-1);
  }
  if( add->bus == BUS_SPI)
  {
    adc3210_spi_write(tsc_fd, fmc, add->cmd, reg, data);
    printf("cmd = %08x - data = %08x\n", add->cmd, data);
  }
  if( add->bus == BUS_TCSR)
  {
    adc3210_csr_wr(tsc_fd, fmc, reg, data);
    printf("reg = %08x - data = %08x\n", reg, data);
  }
  if( add->bus == BUS_I2C)
  {
    data = adc3210_i2c_write(tsc_fd, fmc,add->cmd, reg, data);
    printf("reg = %08x - data = %08x\n", reg, data);
  }
  return(0);

}

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * Function name : adc3210_init
 * Prototype     : void
 * Parameters    : void
 * Return        : void
 *----------------------------------------------------------------------------
 * Description   : adc3210 init
 *
 *++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

int 
adc3210_init( struct cli_cmd_para *c,
	       struct tsc_adc3210_devices *add)
{
  if( !adc3210_init_flag)
  {
    adc3210_init_flag = 1;
  }
  return(0);
}

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * Function name : adc3210_conv
 * Prototype     : void
 * Parameters    : void
 * Return        : void
 *----------------------------------------------------------------------------
 * Description   : adc3210 init
 *
 *++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

int 
adc3210_conv( struct cli_cmd_para *c,
	       struct tsc_adc3210_devices *add)
{
  int fmc;
  char *p;
  int i, chan_set;
  int val;
  float temp, volt;

  fmc = strtoul( c->ext, &p, 16);
  chan_set = 0xf;
  if( c->cnt > 2)
  {
    sscanf( c->para[2],"%x", &chan_set);
  }
  for( i = 0; i < 4; i++)
  {
    if( chan_set & (1<<i))
    {
      usleep(300000);
      val = adc3210_ltc2489_conv(tsc_fd, fmc, i);
      volt = ((float)val * 1.25)/65536.;
/*===> JFG voltage divider ( 100k + 100k) <===*/
      volt = (volt * 2.)/1.;
      temp = 80. - ((120.*(volt-0.53))/0.24);
      if( val)
      {
	printf("chan %d : val = %d voltage = %5.2fV temperature = %5.2f°C\n", i, val, volt, temp);
      }
      else 
      {
	printf("chan %d : val = %d\n", i, val);
      }
    }
  }
  return(0);
}

int 
adc3210_temp( struct cli_cmd_para *c,
	       struct tsc_adc3210_devices *add)
{
  int fmc;
  char *p;
  int chan;
  int val;
  float temp1, temp2, volt1, volt2;

  fmc = strtoul( c->ext, &p, 16);
  if( !strncmp(  add->name, "ads", 3))
  {
    chan = add->idx & 3;

    /* select central diode */
    adc3210_spi_write(tsc_fd, fmc, add->cmd, 0x18e6, 0x01);
    usleep(500000);
    val = adc3210_ltc2489_conv(tsc_fd, fmc, chan);
    val = adc3210_ltc2489_conv(tsc_fd, fmc, chan);
    volt1 = ((float)val * 1.25)/65536.;
/*===> JFG voltage divider ( 100k + 100k) <===*/
    volt1 = (volt1 * 2.)/1.;
    temp1 = 80. - ((120.*(volt1-0.53))/0.24);

    adc3210_spi_write(tsc_fd, fmc, add->cmd, 0x18e6, 0x02);
    usleep(500000);
    val = adc3210_ltc2489_conv(tsc_fd, fmc, chan);
    val = adc3210_ltc2489_conv(tsc_fd, fmc, chan);
    volt2 = ((float)val * 1.25)/65536.;
/*===> JFG voltage divider ( 100k + 100k) <===*/
    volt2 = (volt2 * 2.)/1.;
    printf("volt1 = %6.3fV - volt2 = %6.3fV\n", volt1, volt2);
    volt2 = (volt2 - volt1)*1000.;
    temp2 = (( volt2 - 70.)*113.)/30.;
    printf("%s central :  %5.2f°C -  %5.2f°C\n", add->name, temp1, temp2);

    /* select chan A diode */
    adc3210_spi_write(tsc_fd, fmc, add->cmd, 0x18e6, 0x41);
    usleep(500000);
    val = adc3210_ltc2489_conv(tsc_fd, fmc, chan);
    val = adc3210_ltc2489_conv(tsc_fd, fmc, chan);
    volt1 = ((float)val * 1.25)/65536.;
/*===> JFG voltage divider ( 100k + 100k) <===*/
    volt1 = (volt1 * 2.)/1.;
    temp1 = 80. - ((120.*(volt1-0.53))/0.24);

    adc3210_spi_write(tsc_fd, fmc, add->cmd, 0x18e6, 0x42);
    usleep(500000);
    val = adc3210_ltc2489_conv(tsc_fd, fmc, chan);
    val = adc3210_ltc2489_conv(tsc_fd, fmc, chan);
    volt2 = ((float)val * 1.25)/65536.;
/*===> JFG voltage divider ( 100k + 100k) <===*/
    volt2 = (volt2 * 2.)/1.;
    printf("volt1 = %6.3fV - volt2 = %6.3fV\n", volt1, volt2);
    volt2 = (volt2 - volt1)*1000.;
    temp2 = (( volt2 - 70.)*113.)/30.;
    printf("%s chan A  :  %5.2f°C -  %5.2f°C\n", add->name, temp1, temp2);

    /* select chan B diode */
    adc3210_spi_write(tsc_fd, fmc, add->cmd, 0x18e6, 0x51);
    usleep(500000);
    val = adc3210_ltc2489_conv(tsc_fd, fmc, chan);
    val = adc3210_ltc2489_conv(tsc_fd, fmc, chan);
    volt1 = ((float)val * 1.25)/65536.;
/*===> JFG voltage divider ( 100k + 100k) <===*/
    volt1 = (volt1 * 2.)/1.;
    temp1 = 80. - ((120.*(volt1-0.53))/0.24);

    adc3210_spi_write(tsc_fd, fmc, add->cmd, 0x18e6, 0x52);
    usleep(500000);
    val = adc3210_ltc2489_conv(tsc_fd, fmc, chan);
    val = adc3210_ltc2489_conv(tsc_fd, fmc, chan);
    volt2 = ((float)val * 1.25)/65536.;
/*===> JFG voltage divider ( 100k + 100k) <===*/
    volt2 = (volt2 * 2.)/1.;
    printf("volt1 = %6.3fV - volt2 = %6.3fV\n", volt1, volt2);
    volt2 = (volt2 - volt1)*1000.;
    temp2 = (( volt2 - 70.)*113.)/30.;
    printf("%s chan B  :  %5.2f°C -  %5.2f°C\n", add->name, temp1, temp2);
    adc3210_spi_write(tsc_fd, fmc, add->cmd, 0x18e6, 0x01);
  }
  return(0);
}


int tsc_adc3210_reset(struct cli_cmd_para *c)
{
  int fmc;
  char *p;

  fmc = strtoul( c->ext, &p, 16);
  adc3210_reset(tsc_fd, fmc);
  return 0;
}

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * Function name : tsc_adc3210
 * Prototype     : int
 * Parameters    : cli command parameter structure
 * Return        : success/error
 *----------------------------------------------------------------------------
 * Description   : adc3210 command
 *
 *++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

int 
tsc_adc3210( struct cli_cmd_para *c)
{
  struct tsc_adc3210_devices *add;
  uint fmc;
  char *p;

  if(!strcmp("reset", c->para[0])){
    return tsc_adc3210_reset(c);
  }

  if( c->cnt < 2)
  {
    printf("adc3210 command needs more arguments\n");
    printf("usage: adc3210.<fmc> <dev> <op> <reg> [<data>]\n");
    printf("adc3210 device list:\n");
    add = &adc3210_devices[0];
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
    printf("usage: adc3210.<fmc> <dev> <op> <reg> [<data>]\n");
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

  if( !strcmp( "save", c->para[1]))
  {
    return( adc3210_save( c));
  }

  add = &adc3210_devices[0];
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
    printf("usage: adc3210.<fmc> <dev> <op> <reg> [<data>]\n");
    printf("adc3210 device list:\n");
    add = &adc3210_devices[0];
    while( add->name)
    {
      printf("   - %s\n", add->name);
      add++;
    }
    return(-1);
  }

  if( !strcmp( "read", c->para[1]))
  {
    return( adc3210_read( c, add));
  }
  else if( !strcmp( "rcmp", c->para[1]))
  {
    return( adc3210_rcmp( c, add));
  }
  else if( !strcmp( "write", c->para[1]))
  {
    return( adc3210_write( c, add));
  }
  else if( !strcmp( "init", c->para[1]))
  {
    printf("Executing adc3210.%s, %s, %s\n", c->ext, c->para[0], c->para[1]);
    return( adc3210_init( c, add));
  }
  else if( !strcmp( "conv", c->para[1]))
  {
    return( adc3210_conv( c, add));
  }
  else if( !strcmp( "temp", c->para[1]))
  {
    return( adc3210_temp( c, add));
  }
  else 
  {
    printf("bad operation : %s\n",  c->para[1]);
    printf("usage: adc3210.<fmc> <dev> read <reg>\n");
    printf("       adc3210.<fmc> <dev> write <reg> <data>\n");
    return(-1);
  }
  return(0);

}



