/*=========================< begin file & file header >=======================
 *  References
 *  
 *    filename : adc3110.c
 *    author   : JFG
 *    company  : IOxOS
 *    creation : november 14,2011
 *    version  : 0.0.1
 *
 *----------------------------------------------------------------------------
 *  Description
 *
 *     That file contains a set of function called by XprsMon to perform read
 *     or write cycles through the I"C interface.
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
 *----------------------------------------------------------------------------
 *  Change History
 *  
 * $Log: adc3110.c,v $
 * Revision 1.10  2014/12/19 09:36:19  ioxos
 * add data check [JFG]
 *
 * Revision 1.9  2014/08/27 07:11:26  ioxos
 * add adc3110 check operation [JFG]
 *
 * Revision 1.8  2014/07/11 14:08:57  ioxos
 * correct size of sign [JFG]
 *
 * Revision 1.7  2014/07/04 11:46:57  ioxos
 * cosmetics [JFG]
 *
 * Revision 1.6  2014/05/07 14:06:00  ioxos
 * i2c macros [JFG]
 *
 * Revision 1.5  2014/03/17 10:18:52  ioxos
 * acq command for adc3110/11 [JFG]
 *
 * Revision 1.4  2014/01/20 14:47:24  ioxos
 * cosmetics [JFG]
 *
 * Revision 1.3  2013/08/27 12:22:31  ioxos
 * swap two bytes registers [JFG]
 *
 * Revision 1.2  2013/05/14 06:29:17  ioxos
 * new register mapping + signature + acq fifo + temperature control [JFG]
 *
 * Revision 1.1  2013/04/15 13:55:14  ioxos
 * first checkin [JFG]
 *
 * Revision 1.7  2012/09/03 13:19:05  ioxos
 * adapt pec_adc3110_xx(), pev_pex_xx() and pev_bmr_xx() to new FPGA and library [JFG]
 *
 * Revision 1.6  2012/08/28 13:40:46  ioxos
 * cleanup + update adc3110 status + reset [JFG]
 *
 * Revision 1.5  2012/06/01 13:59:44  ioxos
 * -Wall cleanup [JFG]
 *
 * Revision 1.4  2012/02/14 16:06:43  ioxos
 * add support for FMC [JFG]
 *
 * Revision 1.3  2012/02/03 16:27:37  ioxos
 * dynamic use of elbc for adc3110 [JFG]
 *
 * Revision 1.2  2012/01/27 15:55:44  ioxos
 * prepare release 4.01 supporting x86 & ppc [JFG]
 *
 * Revision 1.1  2012/01/27 13:39:15  ioxos
 * first checkin [JFG]
 *
 *
 *=============================< end file header >============================*/

#ifndef lint
static char *rcsid = "$Id: adc3110.c,v 1.10 2014/12/19 09:36:19 ioxos Exp $";
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
#include <pev791xlib.h>
#include <tscioctl.h>
#include <tsculib.h>

#define BUS_SPI 1
#define BUS_I2C 2
#define BUS_SBC 3

#define MAP_OLDno

#ifdef MAP_OLD
#define ADC_BASE_SERIAL_A     0x80001048
#define ADC_BASE_SERIAL_B     0x80001448
#define ADC_BASE_BMOV_A       0x80001058
#define ADC_BASE_BMOV_B       0x80001458
#else
#define ADC_BASE_SERIAL_A     0x0000120c
#define ADC_BASE_SERIAL_B     0x0000130c
#define ADC_BASE_BMOV_A       0x00001100
#define ADC_BASE_BMOV_B       0x00001110
#endif

struct pev_adc3110_devices
{
  char *name;
  uint cmd;
  int idx;
  int bus;
}
adc3110_devices[] =
{
  { "lmk",  0x02000000, -1, BUS_SBC},
  { "ads01", 0x01000000, 0, BUS_SBC},
  { "ads23", 0x01010000, 1, BUS_SBC},
  { "ads45", 0x01020000, 2, BUS_SBC},
  { "ads67", 0x01030000, 3, BUS_SBC},
  { "tmp102",0x01040048, 0, BUS_I2C},
  { "eeprom",0x40010050, 1, BUS_I2C},
  { NULL, 0}
};

int adc3110_init_flag = 0;
struct cli_cmd_history adc3110_history;
char adc3110_prompt[32];
uint adc3110_histo[2][0x10000];
struct adc3110_calib_res
{
  uint tot[2];
  uint min[2];
  uint max[2];
  float mean[2];
  float sig[2];
};

struct adc3110_sign
{
  char board_name[8];
  char serial[4];
  char version[8];
  char revision[2];
  char rsv[6];
  char test_date[8];
  char calib_date[8];
  char offset_adc[8][8];
  char pad[144];
  int cks;
} adc3110_sign;

struct adc3110_acq_res
{
  uint tot[2];
  short min[2];
  short max[2];
  float mean[2];
  float sig[2];
};
#define I2C_CTL_EXEC_IDLE  0x00000000
#define I2C_CTL_EXEC_RUN   0x00100000
#define I2C_CTL_EXEC_DONE  0x00200000
#define I2C_CTL_EXEC_ERR   0x00300000
#define I2C_CTL_EXEC_MASK  0x00300000

char *
adc3110_rcsid()
{
  return( rcsid);
}

char filename[0x100];
struct cli_cmd_history adc3110_history;

void 
adc3110_init()
{
  if( !adc3110_init_flag)
  {
    cli_history_init( &adc3110_history);
    adc3110_init_flag = 1;
  }
  return;
}


int 
adc3110_acq_res( struct adc3110_acq_res *r,
		 short *buf,
		 int size,
		 int chan)
{
  int i;

  r->tot[chan] = 0;
  r->mean[chan] = 0;
  r->sig[chan] = 0;
  r->min[chan] = 0x7fff;
  r->max[chan] = 0x8000;
  for( i = 0; i < size; i++)
  {
    if( buf[i] < r->min[chan])r->min[chan] = buf[i];
    if( buf[i] > r->max[chan])r->max[chan] = buf[i];
    r->mean[chan] += (float)buf[i];
  }
  r->mean[chan] = r->mean[chan]/size;

  for( i = 0; i < size; i++)
  {
    r->sig[chan] += (buf[i] - r->mean[chan])*(buf[i] - r->mean[chan]);
  }
  r->sig[chan] = sqrt(r->sig[chan]/size);

  return(0);
}

int
adc3110_acq( struct cli_cmd_para *c,
	     int idx,
	     int fmc,
	     int size,
	     int check)
{
  //struct pev_ioctl_map_pg adc_mas_map;
  struct tsc_ioctl_map_win adc_mas_map_win;
  int trig, smem, csr, last_addr;
  int i, tmo, cnt, nerr;
  char *adc_buf, *p;
  char *acq_name_h, *acq_name_d, *acq_name_h1, *acq_name_h2, *acq_name_d1, *acq_name_d2;
  FILE *acq_file_h1, *acq_file_h2,  *acq_file_d1, *acq_file_d2;
  char *default_name = "acq_file";
  struct adc3110_acq_res acq_res;
  int file_his, file_dat;
  int csr_base;

  nerr = 0;
  last_addr = 0;
  trig = 0x88010000;
  smem = 0x01000208;
  csr = 0x81;
  acq_name_h = NULL;
  acq_name_d = NULL;
  csr_base = 0x11e0;
  if( fmc == 2) csr_base = 0x11f0;
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
    if( p[0] == 's')
    {
      sscanf( p, "s:%x", &smem);
    }
    if( p[0] == 'c')
    {
      sscanf( p, "c:%x", &csr);
    }
    if( p[0] == 'a')
    {
      sscanf( p, "a:%x", &last_addr);
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
    acq_name_h1 = (char *)malloc( strlen( acq_name_h) + 8);
    strcpy( acq_name_h1, acq_name_h);
    strcat( acq_name_h1, "_0.his");
    acq_name_h2 = (char *)malloc( strlen( acq_name_h) + 8);
    strcpy( acq_name_h2, acq_name_h);
    strcat( acq_name_h2, "_1.his");
    acq_file_h1 = fopen( acq_name_h1, "w");
    if( !acq_file_h1)
    {
      printf("cannot create acquisition file %s\n", acq_name_h1);
      return( -1);
    }
    acq_file_h2 = fopen( acq_name_h2, "w");
    if( !acq_file_h2)
    {
      printf("cannot create acquisition file %s\n", acq_name_h2);
      fclose( acq_file_h1);
      return( -1);
    }
  }
  if( file_dat)
  {
    if( !acq_name_d)
    {
      acq_name_d = default_name;
    }
    acq_name_d1 = (char *)malloc( strlen( acq_name_d) + 8);
    strcpy( acq_name_d1, acq_name_d);
    strcat( acq_name_d1, "_0.csv");
    acq_name_d2 = (char *)malloc( strlen( acq_name_d) + 8);
    strcpy( acq_name_d2, acq_name_d);
    strcat( acq_name_d2, "_1.csv");
    acq_file_d1 = fopen( acq_name_d1, "w");
    if( !acq_file_d1)
    {
      printf("cannot create acquisition file %s\n", acq_name_d1);
      return( -1);
    }
    acq_file_d2 = fopen( acq_name_d2, "w");
    if( !acq_file_d2)
    {
      printf("cannot create acquisition file %s\n", acq_name_d2);
      fclose( acq_file_d1);
      return( -1);
    }
  }
#ifdef TSC
  bzero( &adc_mas_map_win, sizeof(adc_mas_map_win));
  adc_mas_map_win.req.mode.sg_id= MAP_ID_MAS_PCIE_MEM;
  adc_mas_map_win.req.mode.space = MAP_SPACE_SHM;
  adc_mas_map_win.req.rem_addr = 0x1000000;
  if( fmc == 2) adc_mas_map_win.req.rem_addr = 0x11000000;
  adc_mas_map_win.req.size = 0x2000000;
  tsc_map_alloc( &adc_mas_map_win);
  adc_buf = (char *)tsc_pci_mmap( adc_mas_map_win.sts.loc_base, adc_mas_map_win.sts.size);
#else
  adc_mas_map.rem_addr = 0x11000000;
  adc_mas_map.mode = MAP_ENABLE|MAP_ENABLE_WR|MAP_SPACE_SHM;
  adc_mas_map.flag = 0x0;
  adc_mas_map.sg_id = MAP_MASTER_32;
  adc_mas_map.size = 0x2000000;
  pev_map_alloc( &adc_mas_map);
  adc_buf = pev_mmap( &adc_mas_map);
#endif
  pev_csr_wr( csr_base+0xc, smem);
  pev_csr_wr( csr_base+0x4, trig);
  csr &= 0xfff00fff;
  if( idx == 0) csr |= 0x10000;
  if( idx == 1) csr |= 0x32000;
  if( idx == 2) csr |= 0x54000;
  if( idx == 3) csr |= 0x76000;
  pev_csr_wr( csr_base+0x0, 0x80000000 | csr);
  pev_csr_wr( csr_base+0x0, 0x40000000 | csr);
  usleep( 200000);
  pev_csr_wr( csr_base+0x8, 0x40000000 | last_addr);

  tmo = 800;
  while( --tmo)
  {
    csr = pev_csr_rd( csr_base+0x0);
    if( ( csr & 0x30000000) == 0x30000000) break;
    usleep( 1000);
  }
  //if( tmo)
  if( 1)
  {
    short *buf0, *buf1;
    int n, start;

    start = 0x10;
    if( file_his)
    {
      printf("saving data acquision data from offset %lx [%x] to histogram file %s and %s\n", adc_mas_map_win.req.rem_addr+start, size*4, acq_name_h1, acq_name_h2);
    }
    if( file_dat)
    {
      printf("saving data acquision data from offset %lx [%x] to data file %s and %s\n", adc_mas_map_win.req.rem_addr+start, size*4, acq_name_d1, acq_name_d2);
    }
    buf0 = ( short *)malloc(8*size);
    buf1 = ( short *)malloc(8*size);

    /* read data */
    n = 0;
    for( i = start; i < (start+(size*4)); i += 8)
    {
      int data;

      if( !(i & 0xffff))
      { 
	printf("reading : %06x\r", n);
	fflush( stdout);
      }
      data = *(int *)&adc_buf[i];
      buf0[n+1] = tsc_swap_16((short)data);
      buf0[n] = tsc_swap_16((short)(data >> 16));
      data = *(int *)&adc_buf[i+4];
      buf1[n+1] = tsc_swap_16((short)data);
      buf1[n] = tsc_swap_16((short)(data >> 16));
      n += 2;
    }
    printf("reading : %06x -> done\n", n);

    /* check data */
    if( check)
    {
      for( i = 0; i < n - 6; i++)
      {
        int min, max;
        if( !(i & 0xffff))
        { 
 	  printf("checking : %06x\r", i);
	  fflush( stdout);
        }
        min = buf0[i] - 40;
        max = buf0[i] + 40;
        if( ( buf0[i+6] < min) ||  ( buf0[i+6] > max)) nerr++;
        min = buf1[i] - 40;
        max = buf1[i] + 40;
        if( ( buf1[i+6] < min) ||  ( buf1[i+6] > max)) nerr++;
      }
      printf("checking : %06x -> done [nerr = %d]\n", i, nerr);
    }

    /* save data */
    for( i = 0; i < n; i++)
    {
      if( !(i & 0xffff))
      { 
	printf("checking : %06x\r", i);
	fflush( stdout);
      }
      if( file_dat)
      {
        fprintf( acq_file_d1, "%d\n", buf0[i]);
        fprintf( acq_file_d2, "%d\n", buf1[i]);
      }
      if( file_his)
      {
        fprintf( acq_file_h1, "%d, %d\n", i, buf0[i]);
        fprintf( acq_file_h2, "%d, %d\n", i, buf1[i]);
      }
    }
    printf("saving : %06x -> done\n", i);
#ifdef JFG
    for( i = start; i < (start+(size*4)); i += 8)
    {
      int data;

      if( !(i & 0xffff))
      { 
	printf("%06x\r", i);
	fflush( stdout);
      }
      data = *(int *)&adc_buf[i];
      buf0[n+1] = tsc_swap_16((short)data);
      buf0[n] = tsc_swap_16((short)(data >> 16));
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
      data = *(int *)&adc_buf[i+4];
      buf1[n+1] = tsc_swap_16((short)data);
      buf1[n] = tsc_swap_16((short)(data >> 16));
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
    printf("%06x -> done\n", i);
#endif
    adc3110_acq_res( &acq_res, buf0, size, 0);
    adc3110_acq_res( &acq_res, buf1, size, 1);
    free( (void *)buf0);
    free( (void *)buf1);
    printf("Channel 0 : %f +/- %f [%d : %d]\n", acq_res.mean[0], acq_res.sig[0], acq_res.min[0], acq_res.max[0]);
    printf("Channel 1 : %f +/- %f [%d : %d]\n", acq_res.mean[1], acq_res.sig[1], acq_res.min[1], acq_res.max[1]);
  }
  else
  {
    printf("Acquistion timeout...\n");
    nerr = -1;
  }

  printf("Transfering histogramming files to host...");
  fflush( stdout);
  if( file_his)
  {
    fclose( acq_file_h1);
    fclose( acq_file_h2);
  }
  if( file_dat)
  {
    fclose( acq_file_d1);
    fclose( acq_file_d2);
  }
  printf(" -> done\n");
#ifdef TSC
  tsc_pci_munmap( adc_buf, adc_mas_map_win.sts.size);
  tsc_map_free( &adc_mas_map_win);
#else
  pev_munmap( &adc_mas_map);
  pev_map_free( &adc_mas_map);
#endif
  return( nerr);
}

int 
adc3110_calib_res( struct adc3110_calib_res *r)
{
  int i;

  r->tot[0] = 0;
  r->mean[0] = 0;
  r->sig[0] = 0;
  r->min[0] = 0xffff;
  r->max[0] = 0x0;
  r->tot[1] = 0;
  r->sig[1] = 0;
  r->min[1] = 0xffff;
  r->max[1] = 0x0;
  for( i = 0; i < 0x10000; i++)
  {
    if(adc3110_histo[0][i])
    {
      if( i < r->min[0])r->min[0] = i;
      if( i > r->max[0])r->max[0] = i;
    }
    r->tot[0] += adc3110_histo[0][i];
    r->mean[0] += (float)(i*adc3110_histo[0][i]);
    if(adc3110_histo[1][i])
    {
      if( i < r->min[1])r->min[1] = i;
      if( i > r->max[1])r->max[1] = i;
    }
    r->tot[1] += adc3110_histo[1][i];
    r->mean[1] += (float)(i*adc3110_histo[1][i]);
  }
  r->mean[0] = r->mean[0]/r->tot[0];
  r->mean[1] = r->mean[1]/r->tot[1];

  for( i = 0; i < 0x10000; i++)
  {
    r->sig[0] += (i - r->mean[0])*(i - r->mean[0])*adc3110_histo[0][i];
    r->sig[1] += (i - r->mean[1])*(i - r->mean[1])*adc3110_histo[1][i];
  }
  r->sig[0] = sqrt(r->sig[0]/r->tot[0]);
  r->sig[1] = sqrt(r->sig[1]/r->tot[1]);

  return(0);
}

int 
tsc_adc3110( struct cli_cmd_para *c)
{
  struct pev_adc3110_devices *add;
  uint cmd, data, reg, fmc, tmo;
  char *p;

  adc3110_init();

  if( c->cnt < 2)
  {
    printf("adc3110 command needs more arguments\n");
    printf("usage: adc3110.<fmc> <dev> <op> <reg> [<data>]\n");
    printf("adc3110 device list:\n");
    add = &adc3110_devices[0];
    while( add->name)
    {
      printf("   - %s\n", add->name);
      add++;
    }
    return(-1);
  }

  if( !strcmp( "save", c->para[1]))
  {
    int i, offset, size, data, chan;
    char *acq_name_h1, *acq_name_h2;
    FILE *acq_file_h1, *acq_file_h2;
    char *acq_buf;
    //struct pev_ioctl_map_pg shm_mas_map;
    struct tsc_ioctl_map_win shm_mas_map_win;
    struct adc3110_calib_res res;

    if( c->cnt < 4)
    {
      printf("adc3110 acq command needs more arguments\n");
      printf("usage: adc3110.<fmc> <filename> save <offset> <size>\n");
      return(-1);
    }
    if( sscanf( c->para[2],"%x", &offset) != 1)
    {
      printf("wrong offset value\n");
      printf("usage: adc3110.<fmc> <filename> save <offset> <size>\n");
      return(-1);
    }
    if( sscanf( c->para[3],"%x", &size) != 1)
    {
      printf("wrong size value\n");
      printf("usage: adc3110.<fmc> <filename> save <offset> <size>\n");
      return(-1);
    }

    if(  c->para[0][0] != '-')
    {
      if(  c->para[0][0] == '?')
      {
        strcpy( adc3110_prompt, "enter filename: "); 
        c->para[0] = cli_get_cmd( &adc3110_history, adc3110_prompt);
      }
      acq_name_h1 = (char *)malloc( strlen( c->para[0]) + 8);
      strcpy( acq_name_h1, c->para[0]);
      strcat( acq_name_h1, "_1.his");
      acq_name_h2 = (char *)malloc( strlen( c->para[0]) + 8);
      strcpy( acq_name_h2, c->para[0]);
      strcat( acq_name_h2, "_2.his");

      printf("save data acquision data from offset %x [%x] to file %s and %s\n", offset, size, acq_name_h1, acq_name_h2);
      acq_file_h1 = fopen( acq_name_h1, "w");
      if( !acq_file_h1)
      {
        printf("cannot create acquisition file %s\n", acq_name_h1);
        return( -1);
      }
      acq_file_h2 = fopen( acq_name_h2, "w");
      if( !acq_file_h2)
      {
        printf("cannot create acquisition file %s\n", acq_name_h2);
        fclose( acq_file_h1);
        return( -1);
      }
    }
    acq_buf = NULL;
    if( size > 0)
    {
#ifdef TSC
      bzero( &shm_mas_map_win, sizeof(shm_mas_map_win));
      shm_mas_map_win.req.mode.sg_id= MAP_ID_MAS_PCIE_PMEM;
      shm_mas_map_win.req.mode.space = MAP_SPACE_SHM;
      //shm_mas_map_win.req.mode.space = MAP_SPACE_USR1;
      shm_mas_map_win.req.rem_addr = offset;
      shm_mas_map_win.req.size = size;
      tsc_map_alloc( &shm_mas_map_win);
      acq_buf = (char *)tsc_pci_mmap( shm_mas_map_win.sts.loc_base, shm_mas_map_win.sts.size);
#else
      shm_mas_map.rem_addr = offset;
      shm_mas_map.mode = MAP_ENABLE|MAP_ENABLE_WR|MAP_SPACE_SHM;
      shm_mas_map.flag = 0x0;
      shm_mas_map.sg_id = MAP_MASTER_32;
      shm_mas_map.size = size;
      pev_map_alloc( &shm_mas_map);
      acq_buf = pev_mmap( &shm_mas_map);
#endif
    }
    if( !acq_buf || ( size <= 0))
    {
      printf("cannot allocate acquisition buffer\n");
      return( -1);
    }
    for( i = 0; i < 0x10000; i++)
    {
      adc3110_histo[0][i] = 0;
      adc3110_histo[1][i] = 0;
    }
    chan = 0;
    for( i = 0; i < size; i += 16)
    {
      data = *(unsigned char *)&acq_buf[i] |  (*(unsigned char *)&acq_buf[i+1] << 8);
      adc3110_histo[0][data & 0xffff] += 1;
      if(  c->para[0][0] != '-') fprintf( acq_file_h1, "%d %d\n", chan, data);
      data = *(unsigned char *)&acq_buf[i+2] |  (*(unsigned char *)&acq_buf[i+3] << 8);
      adc3110_histo[0][data & 0xffff] += 1;
      if(  c->para[0][0] != '-') fprintf( acq_file_h1, "%d %d\n", chan+1, data);
      data = *(unsigned char *)&acq_buf[i+4] |  (*(unsigned char *)&acq_buf[i+5] << 8);
      adc3110_histo[0][data & 0xffff] += 1;
      if(  c->para[0][0] != '-') fprintf( acq_file_h1, "%d %d\n", chan+2, data);
      data = *(unsigned char *)&acq_buf[i+6] |  (*(unsigned char *)&acq_buf[i+7] << 8);
      adc3110_histo[0][data & 0xffff] += 1;
      if(  c->para[0][0] != '-') fprintf( acq_file_h1, "%d %d\n", chan+3, data);
      data = *(unsigned char *)&acq_buf[i+8] |  (*(unsigned char *)&acq_buf[i+9] << 8);
      adc3110_histo[1][data & 0xffff] += 1;
      if(  c->para[0][0] != '-') fprintf( acq_file_h2, "%d %d\n", chan,  data);
      data = *(unsigned char *)&acq_buf[i+10] |  (*(unsigned char *)&acq_buf[i+11] << 8);
      adc3110_histo[1][data & 0xffff] += 1;
      if(  c->para[0][0] != '-') fprintf( acq_file_h2, "%d %d\n", chan+1, data);
      data = *(unsigned char *)&acq_buf[i+12] |  (*(unsigned char *)&acq_buf[i+13] << 8);
      adc3110_histo[1][data & 0xffff] += 1;
      if(  c->para[0][0] != '-') fprintf( acq_file_h2, "%d %d\n", chan+2, data);
      data = *(unsigned char *)&acq_buf[i+14] |  (*(unsigned char *)&acq_buf[i+15] << 8);
      adc3110_histo[1][data & 0xffff] += 1;
      if(  c->para[0][0] != '-') fprintf( acq_file_h2, "%d %d\n", chan+3, data);
      chan += 4;
    }
    adc3110_calib_res( &res);
    printf("tot_A: %d - mean_A: %f - sig_A: %f [%d %d]\n", res.tot[0], res.mean[0], res.sig[0], res.min[0], res.max[0]);
    printf("tot_B: %d - mean_B: %f - sig_B: %f [%d %d]\n", res.tot[1], res.mean[1], res.sig[1], res.min[1], res.max[1]);
  
    if(  c->para[0][0] != '-')
    {
      fclose( acq_file_h1);
      fclose( acq_file_h2);
    }
#ifdef TSC
    tsc_pci_munmap( acq_buf, shm_mas_map_win.sts.size);
    tsc_map_free( &shm_mas_map_win);
#else
    pev_munmap( &shm_mas_map);
    pev_map_free( &shm_mas_map);
#endif

    return( 0);
  }

  if( !c->ext) 
  {
    printf("you must specify fmc [1 or 2]\n");
    printf("usage: adc3110.<fmc> <dev> <op> <reg> [<data>]\n");
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
  add = &adc3110_devices[0];
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
    printf("usage: adc3110.<fmc> <dev> <op> <reg> [<data>]\n");
    printf("adc3110 device list:\n");
    add = &adc3110_devices[0];
    while( add->name)
    {
      printf("   - %s\n", add->name);
      add++;
    }
    return(-1);
  }
  if( !strcmp( "read", c->para[1]))
  {
    if( c->cnt < 3)
    {
      printf("adc3110 read command needs more arguments\n");
      printf("usage: adc3110.<fmc> <dev> read <reg>\n");
      return(-1);
    }
    printf("%s.%s %s %s %s\n", c->cmd, c->ext, c->para[0], c->para[1], c->para[2]);
    if( sscanf( c->para[2],"%x", &reg) != 1)
    {
      printf("wrong register number\n");
      printf("usage: adc3110.<fmc> <dev> <op> <reg> [<data>]\n");
      return(-1);
    }
    if( add->bus == BUS_SBC)
    {
      cmd =  0x80000000 | add->cmd | reg;
      tmo = 1000;
      if( fmc == 2)
      {
        pev_csr_wr( ADC_BASE_SERIAL_B, cmd);
        while( --tmo)
        {
	  if( !(pev_csr_rd( ADC_BASE_SERIAL_B) & 0x80000000)) break;
        }
      data = pev_csr_rd( ADC_BASE_SERIAL_B + 4);
      }
      else
      {
        pev_csr_wr( ADC_BASE_SERIAL_A, cmd);
        while( --tmo)
        {
	  if( !(pev_csr_rd( ADC_BASE_SERIAL_A) & 0x80000000)) break;
        }
        data = pev_csr_rd( ADC_BASE_SERIAL_A + 4);
      }
      //if( cmd & 0x02) data = data >> 5; /* LMK */
      printf("cmd = %08x - data = %08x\n", cmd, data);
    }
    if( add->bus == BUS_I2C)
    {
      int status;

      cmd = add->cmd;
      if( fmc == 2)
      {
        cmd |= 0xa0000000;
      }
      else
      {
        cmd |= 0x80000000;
	if( add->idx == 1) cmd += 1;
      }
      if( (cmd & 0x30000) == 0x10000) reg = tsc_swap_16( reg);
      status = tsc_i2c_read( cmd, reg, &data);
      if( (status & I2C_CTL_EXEC_MASK) == I2C_CTL_EXEC_ERR)
      {
        printf("%s: reg=%x -> error = %08x\n", add->name, reg, status);
      } 
      else
      {
	printf("cmd = %08x - data = %08x\n", cmd, data);
      }
    }
  }
  else if( !strcmp( "write", c->para[1]))
  {
    if( c->cnt < 4)
    {
      printf("adc3110 write command needs more arguments\n");
      printf("usage: adc3110.<fmc> <dev> write <reg> <data>\n");
      return(-1);
    }
    if( sscanf( c->para[2],"%x", &reg) != 1)
    {
      printf("wrong register number\n");
      printf("usage: adc3110.<fmc> <dev> <op> <reg> [<data>]\n");
      return(-1);
    }
    if( sscanf( c->para[3],"%x", &data) != 1)
    {
      printf("wrong data value\n");
      return(-1);
    }
    printf("%s.%s %s %s %s %s \n", c->cmd, c->ext, c->para[0], c->para[1], c->para[2], c->para[3]);
    if( add->bus == BUS_SBC)
    {
      cmd =  0xc0000000 | add->cmd | reg;
      printf("cmd = %08x - data = %08x\n", cmd, data);
      if( fmc == 2)
      {
        pev_csr_wr( ADC_BASE_SERIAL_B + 4, data);
        pev_csr_wr( ADC_BASE_SERIAL_B, cmd);
      }
      else 
      {
        pev_csr_wr( ADC_BASE_SERIAL_A + 4, data);
        pev_csr_wr( ADC_BASE_SERIAL_A, cmd);
      }
    }
    if( add->bus == BUS_I2C)
    {
      int status;

      cmd = add->cmd;
      if( fmc == 2)
      {
        cmd |= 0xa0000000;
      }
      else
      {
        cmd |= 0x80000000;
	if( add->idx == 1) cmd += 1;
      }
      printf("cmd = %08x - data = %08x\n", cmd, data);
      if( (cmd & 0x30000) == 0x10000) reg = tsc_swap_16( reg);
      status = tsc_i2c_write( cmd, reg, data);
      if( (status & I2C_CTL_EXEC_MASK) == I2C_CTL_EXEC_ERR)
      {
        printf("%s: reg=%x -> error = %08x\n", add->name, reg, status);
      } 
    }
  }
  else if( !strcmp( "acqfif", c->para[1]) ||
	   !strcmp( "check",  c->para[1])    )
  {
    int offset, size, tmo;

    if( (add->idx < 0) || (add->bus != BUS_SBC))
    {
      printf("wrong device name\n");
      printf("usage: adc3110.<fmc> ads<ij> %s <offset> [<size>]\n", c->para[1]);
      return(-1);
    }
    if( c->cnt < 4)
    {
      printf("adc3110 %s command needs more arguments\n", c->para[1]);
      printf("usage: adc3110.<fmc> <dev> %s <offset> <size>\n", c->para[1]);
      return(-1);
    }
    if( sscanf( c->para[2],"%x", &offset) != 1)
    {
      printf("wrong offset value\n");
      printf("usage: adc3110.<fmc> <dev> %s <offset> <size>\n", c->para[1]);
      return(-1);
    }
    if( sscanf( c->para[3],"%x", &size) != 1)
    {
      printf("wrong size value\n");
      printf("usage: adc3110.<fmc> <dev> %s <offset> <size>\n", c->para[1]);
      return(-1);
    }
    printf("start data acquision on device  %s [%d] at offset %x [%x]\n",  c->para[0], add->idx, offset, size);
    if( fmc == 2)
    {
      int cmd_sav;

      cmd_sav = pev_csr_rd( 0x1188);
      cmd_sav &= ~( 0xff << (8*add->idx));

      cmd = cmd_sav | (1 << (8*add->idx));
      pev_csr_wr( 0x1188, cmd);
      pev_csr_rd( 0x1188);

      cmd = cmd_sav;
      pev_csr_wr( 0x1188, cmd);
      pev_csr_rd( 0x1188);

      pev_csr_wr( ADC_BASE_BMOV_B, offset);
      cmd = 0x90000000 | ( add->idx << 26) | (size & 0x3fffe00);
      pev_csr_wr( ADC_BASE_BMOV_B + 4, cmd);
      pev_csr_rd( ADC_BASE_BMOV_B + 4);

      cmd = cmd_sav | (2 << (8*add->idx));
      pev_csr_wr( 0x1188, cmd);
      pev_csr_rd( 0x1188);

      if( c->para[1][0] == 'c') /* if check operation */
      {
        /* select test pattern generation mode */
        pev_csr_wr( ADC_BASE_SERIAL_B + 4, 0x44);
        pev_csr_wr( ADC_BASE_SERIAL_B,  0xc000000f | add->cmd);
      }
      tmo = 100;
      while( --tmo)
      {
        usleep(2000);
        if( pev_csr_rd( ADC_BASE_BMOV_B + 4) & 0x80000000) break;
      }
      printf("acquisition status : %08x - %08x\n", pev_csr_rd( ADC_BASE_BMOV_B),  pev_csr_rd( ADC_BASE_BMOV_B + 4));
    }
    else
    {
      int cmd_sav;

      cmd_sav = pev_csr_rd( 0x1184);
      cmd_sav &= ~( 0xff << (8*add->idx));

      cmd = cmd_sav | (1 << (8*add->idx));
      pev_csr_wr( 0x1184, cmd);
      pev_csr_rd( 0x1184);

      cmd = cmd_sav;;
      pev_csr_wr( 0x1184, cmd);
      pev_csr_rd( 0x1184);

      pev_csr_wr( ADC_BASE_BMOV_A, offset);
      cmd = 0x90000000 | ( add->idx << 26) | (size & 0x3fffe00);
      pev_csr_wr( ADC_BASE_BMOV_A + 4, cmd);
      pev_csr_rd( ADC_BASE_BMOV_A+4);

      cmd = cmd_sav | (2 << (8*add->idx));
      pev_csr_wr( 0x1184, cmd);
      pev_csr_rd( 0x1184);

      if( c->para[1][0] == 'c') /* if check operation */
      {
        /* select test pattern generation mode */
        pev_csr_wr( ADC_BASE_SERIAL_A + 4, 0x44);
        pev_csr_wr( ADC_BASE_SERIAL_A,  0xc000000f | add->cmd);
      }

      tmo = 100;
      while( --tmo)
      {
        usleep(2000);
        if( pev_csr_rd( ADC_BASE_BMOV_A + 4) & 0x80000000) break;
      }
      printf("acquisition status : %08x - %08x\n", pev_csr_rd( ADC_BASE_BMOV_A),  pev_csr_rd( ADC_BASE_BMOV_A + 4));
    }
    if( c->para[1][0] == 'c')
    {
      //struct pev_ioctl_map_pg shm_mas_map;
      struct tsc_ioctl_map_win shm_mas_map_win;
      char *acq_buf;
      int i;
      uint cmp0, cmp1;
      int nerr0, nerr1;

      printf("Checking test pattern\n");
      usleep(1000000);
      acq_buf = NULL;
      if( size > 0)
      {
#ifdef TSC
        bzero( &shm_mas_map_win, sizeof(shm_mas_map_win));
        shm_mas_map_win.req.mode.sg_id= MAP_ID_MAS_PCIE_MEM;
        shm_mas_map_win.req.mode.space = MAP_SPACE_SHM;
        shm_mas_map_win.req.rem_addr = offset;
        if( fmc == 2)
        {
          shm_mas_map_win.req.rem_addr  |= 0x10000000;
	}
        shm_mas_map_win.req.size = size;
        tsc_map_alloc( &shm_mas_map_win);
        acq_buf = (char *)tsc_pci_mmap( shm_mas_map_win.sts.loc_base, shm_mas_map_win.sts.size);
#else
        shm_mas_map.rem_addr = offset;
        if( fmc == 2)
        {
	  shm_mas_map.rem_addr |= 0x10000000;
	}
        shm_mas_map.mode = MAP_ENABLE|MAP_ENABLE_WR|MAP_SPACE_SHM;
        shm_mas_map.flag = 0x0;
        shm_mas_map.sg_id = MAP_MASTER_32;
        shm_mas_map.size = size;
        pev_map_alloc( &shm_mas_map);
        acq_buf = pev_mmap( &shm_mas_map);
#endif
      }
      if( !acq_buf || ( size <= 0))
      {
        printf("cannot allocate acquisition buffer\n");
        return( -1);
      }
      for( i = 0; i < 0x10000; i++)
      {
        adc3110_histo[0][i] = 0;
        adc3110_histo[1][i] = 0;
      }


      cmp0 = *(unsigned char *)&acq_buf[0] |  (*(unsigned char *)&acq_buf[1] << 8);
      cmp1 = *(unsigned char *)&acq_buf[8] |  (*(unsigned char *)&acq_buf[9] << 8);
      nerr0 = 0;
      nerr1 = 0;
      for( i = 0; i < size; i += 16)
      {
        data = *(unsigned char *)&acq_buf[i] |  (*(unsigned char *)&acq_buf[i+1] << 8);
	if( cmp0 != data)
	{
	  if( nerr0 < 16) printf("%x : %x - %x\n", i/4+0, data, cmp0);
	  nerr0 += 1;
	  cmp0 = data;
	}
	cmp0 = ( cmp0 + 1) & 0xffff;
        adc3110_histo[0][data & 0xffff] += 1;

        data = *(unsigned char *)&acq_buf[i+2] |  (*(unsigned char *)&acq_buf[i+3] << 8);
	if( cmp0 != data)
	{
	  if( nerr0 < 16) printf("%x : %x - %x\n", i/4+1, data, cmp0);
	  nerr0 += 1;
	  cmp0 = data;
	}
	cmp0 = ( cmp0 + 1) & 0xffff;
        adc3110_histo[0][data & 0xffff] += 1;

        data = *(unsigned char *)&acq_buf[i+4] |  (*(unsigned char *)&acq_buf[i+5] << 8);
	if( cmp0 != data)
	{
	  if( nerr0 < 16) printf("%x : %x - %x\n", i/4+2, data, cmp0);
	  nerr0 += 1;
	  cmp0 = data;
	}
	cmp0 = ( cmp0 + 1) & 0xffff;
        adc3110_histo[0][data & 0xffff] += 1;

        data = *(unsigned char *)&acq_buf[i+6] |  (*(unsigned char *)&acq_buf[i+7] << 8);
	if( cmp0 != data)
	{
	  if( nerr0 < 16) printf("%x : %x - %x\n", i/4+3, data, cmp0);
	  nerr0 += 1;
	  cmp0 = data;
	}
	cmp0 = ( cmp0 + 1) & 0xffff;
        adc3110_histo[0][data & 0xffff] += 1;

        data = *(unsigned char *)&acq_buf[i+8] |  (*(unsigned char *)&acq_buf[i+9] << 8);
	if( cmp1 != data)
	{
	  if( nerr1 < 16) printf("%x : %x - %x\n", i/4+0, data, cmp1);
	  nerr1 += 1;
	  cmp1 = data;
	}
	cmp1 = ( cmp1 + 1) & 0xffff;
        adc3110_histo[1][data & 0xffff] += 1;

        data = *(unsigned char *)&acq_buf[i+10] |  (*(unsigned char *)&acq_buf[i+11] << 8);
	if( cmp1 != data)
	{
	  if( nerr1 < 16) printf("%x : %x - %x\n", i/4+1, data, cmp1);
	  nerr1 += 1;
	  cmp1 = data;
	}
	cmp1 = ( cmp1 + 1) & 0xffff;
        adc3110_histo[1][data & 0xffff] += 1;

        data = *(unsigned char *)&acq_buf[i+12] |  (*(unsigned char *)&acq_buf[i+13] << 8);
	if( cmp1 != data)
	{
	  if( nerr1 < 16) printf("%x : %x - %x\n", i/4+2, data, cmp1);
	  nerr1 += 1;
	  cmp1 = data;
	}
	cmp1 = ( cmp1 + 1) & 0xffff;
        adc3110_histo[1][data & 0xffff] += 1;

        data = *(unsigned char *)&acq_buf[i+14] |  (*(unsigned char *)&acq_buf[i+15] << 8);
	if( cmp1 != data)
	{
	  if( nerr1 < 16) printf("%x : %x - %x\n", i/4+3, data, cmp1);
	  nerr1 += 1;
	  cmp1 = data;
	}
	cmp1 = ( cmp1 + 1) & 0xffff;
        adc3110_histo[1][data & 0xffff] += 1;
      }
      if( nerr0 == 0)
      {
	printf("ADC3110 test pattern FMC%d chan#%d -> OK\n", fmc, (2*add->idx));
      }
      else
      {
	printf("ADC3110 test pattern FMC%d chan#%d nerr = %d -> NOK\n", fmc, (2*add->idx), nerr0);
      }
      if( nerr1 == 0)
      {
	printf("ADC3110 test pattern FMC%d chan#%d -> OK\n", fmc, (2*add->idx) + 1);
      }
      else
      {
	printf("ADC3110 test pattern FMC%d chan#%d nerr = %d -> NOK\n", fmc, (2*add->idx) + 1, nerr1);
      }
#ifdef TSC
      tsc_pci_munmap( acq_buf, shm_mas_map_win.sts.size);
      tsc_map_free( &shm_mas_map_win);
#else
      pev_munmap( &shm_mas_map);
      pev_map_free( &shm_mas_map);
#endif
    }
    return(0);
  }
  else if( !strncmp( "acq", c->para[1], 3))
  {
    int size;
    int check;
    int nerr;

    if( (add->idx < 0) || (add->bus != BUS_SBC))
    {
      printf("wrong device name\n");
      printf("usage: adc3110.<fmc> ads<ij> acq64K\n");
      return(-1);
    }
    size = 0x10000;
    check = 0;
    if( !strcmp( "acq64K", c->para[1])) size = 0x10000;
    if( !strcmp( "acq128K", c->para[1])) size = 0x20000;
    if( !strcmp( "acq256K", c->para[1])) size = 0x40000;
    if( !strcmp( "acq512K", c->para[1])) size = 0x80000;
    if( !strcmp( "acq1M", c->para[1])) size = 0x100000;
    if( !strcmp( "acq4M", c->para[1])) size = 0x400000;
    if( !strcmp( "acq64Kc", c->para[1])) 
    {
      size = 0x10000;
      check = 5;
    }
    do
    {
      nerr = adc3110_acq( c, add->idx, fmc, size, check);
      if( !nerr) break;
    } while( check--);

    return(0);
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
    device = 0x40040048;
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
    return(0);
  }
  else if( !strcmp( "set", c->para[1]))
  {
    int status;
    int device;
    uint temp, ctl, lo, hi;
    float flo, fhi;

    device = add->cmd;
    if( device != 0x01040048)
    {
      printf(" set command not supported for that device\n");
      return(-1);
    }
    if( fmc == 2)
    {
      device |= 0x50000000;
    }
    else
    {
      device |= 0x40000000;
    }
    if( c->cnt < 4)
    {
      printf("adc3110 set command needs more arguments\n");
      printf("usage: adc3110.<fmc> tmp109 set <lo> <hi>\n");
      return(-1);
    }
    if( sscanf( c->para[2],"%f", &flo) != 1)
    {
      printf("wrong lo value\n");
      printf("usage: adc3110.<fmc> tmp109 set <lo> <hi>\n");
      return(-1);
    }
    if( sscanf( c->para[3],"%f", &fhi) != 1)
    {
      printf("wrong hi value\n");
      printf("usage: adc3110.<fmc> tmp109 set <lo> <hi>\n");
      return(-1);
    }
    status = tsc_i2c_read( device, 1, &ctl);
    if( (status & I2C_CTL_EXEC_MASK) == I2C_CTL_EXEC_ERR)
    {
      printf("%s: reg=%x -> error = %08x\n", add->name, reg, status);
    }
    else
    {
      tsc_i2c_read( device, 0, &temp);
      lo = (int)(flo*16);
      hi = (int)(fhi*16);
      if( temp & 0x100)
      {
	temp = ((temp & 0xff) << 5) + ((temp & 0xf8) >> 3);
	lo = ((lo & 0x1fe0) >> 5) | ((lo & 0x1f) << 11);
	hi = ((hi & 0x1fe0) >> 5) | ((hi & 0x1f) << 11);
      }
      else
      {
	temp = ((temp & 0xff) << 4) + ((temp & 0xf0) >> 4);
	lo = ((lo & 0xff0) >> 4) | ((lo & 0xf) << 12);
	hi = ((hi & 0xff0) >> 4) | ((hi & 0xf) << 12);
      }
      tsc_i2c_write( device, 2, lo);
      usleep(10000);
      tsc_i2c_write( device, 3, hi);
      usleep(10000);
      printf("current temperature: %.2f [%.2f - %.2f]\n", (float)temp/16, flo, fhi);
    }
    return(0);
  }
  else if( !strcmp( "sign", c->para[1]))
  {
    int device, i;
    unsigned char *p;
    int op;
    char *para_p;

    device = add->cmd;
    if( device != 0x40010050)
    {
      printf(" sign command not supported for that device\n");
      return(-1);
    }
    if( fmc == 2)
    {
      device |= 2;
    }
    p = (unsigned char *)&adc3110_sign;
    for( i = 0x0; i < 0x100; i++)
    {
      //tsc_i2c_read( device, tsc_swap_16( 0x7000 + i), &data);
      tsc_i2c_read( device, (0x7000 + i), &data);
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

      bzero( &adc3110_history, sizeof( struct cli_cmd_history));
      cli_history_init( &adc3110_history);
      printf("setting ADC3110 signature\n");
      para_p = cli_get_cmd( &adc3110_history, "Enter password ->  ");
      if( strcmp(  para_p, "goldorak"))
      {
	printf("wrong password\n");
	return(-1);
      }

      strcpy( &prompt[0], "Board Name [");
      strncat( &prompt[0], &adc3110_sign.board_name[0], 8);
      strcat( &prompt[0], "] : "); 
      para_p = cli_get_cmd( &adc3110_history, prompt);
      if( para_p[0] == 'q') return(-1);
      if( para_p[0]) strncpy( &adc3110_sign.board_name[0], para_p, 8);


      strcpy( &prompt[0], "Serial Number [");
      strncat( &prompt[0], &adc3110_sign.serial[0], 4);
      strcat( &prompt[0], "] : "); 
      para_p = cli_get_cmd( &adc3110_history, prompt);
      if( para_p[0] == 'q') return(-1);
      if( para_p[0]) strncpy( &adc3110_sign.serial[0], para_p, 4);

      strcpy( &prompt[0], "PCB Version :  [");
      strncat( &prompt[0], &adc3110_sign.version[0], 8);
      strcat( &prompt[0], "] : "); 
      para_p = cli_get_cmd( &adc3110_history, prompt);
      if( para_p[0] == 'q') return(-1);
      if( para_p[0]) strncpy( &adc3110_sign.version[0], para_p, 8);

      strcpy( &prompt[0], "Hardware Revision :  [");
      strncat( &prompt[0], &adc3110_sign.revision[0], 2);
      strcat( &prompt[0], "] : "); 
      para_p = cli_get_cmd( &adc3110_history, prompt);
      if( para_p[0] == 'q') return(-1);
      if( para_p[0]) strncpy( &adc3110_sign.revision[0], para_p, 2);

      strcpy( &prompt[0], "Test Date :  [");
      strncat( &prompt[0], &adc3110_sign.test_date[0], 8);
      strcat( &prompt[0], "] : "); 
      para_p = cli_get_cmd( &adc3110_history, prompt);
      if( para_p[0] == 'q') return(-1);
      if( para_p[0]) strncpy( &adc3110_sign.test_date[0], para_p, 8);

      strcpy( &prompt[0], "Calibration Date :  [");
      strncat( &prompt[0], &adc3110_sign.calib_date[0], 8);
      strcat( &prompt[0], "] : "); 
      para_p = cli_get_cmd( &adc3110_history, prompt);
      if( para_p[0] == 'q') return(-1);
      if( para_p[0]) strncpy( &adc3110_sign.calib_date[0], para_p, 8);

      strcpy( &prompt[0], "Offset Compensation Chan0 :  [");
      strncat( &prompt[0], &adc3110_sign.offset_adc[0][0], 8);
      strcat( &prompt[0], "] : "); 
      para_p = cli_get_cmd( &adc3110_history, prompt);
      if( para_p[0] == 'q') return(-1);
      if( para_p[0]) strncpy( &adc3110_sign.offset_adc[0][0], para_p, 8);

      strcpy( &prompt[0], "Offset Compensation Chan1 :  [");
      strncat( &prompt[0], &adc3110_sign.offset_adc[1][0], 8);
      strcat( &prompt[0], "] : "); 
      para_p = cli_get_cmd( &adc3110_history, prompt);
      if( para_p[0] == 'q') return(-1);
      if( para_p[0]) strncpy( &adc3110_sign.offset_adc[1][0], para_p, 8);

      strcpy( &prompt[0], "Offset Compensation Chan2 :  [");
      strncat( &prompt[0], &adc3110_sign.offset_adc[2][0], 8);
      strcat( &prompt[0], "] : "); 
      para_p = cli_get_cmd( &adc3110_history, prompt);
      if( para_p[0] == 'q') return(-1);
      if( para_p[0]) strncpy( &adc3110_sign.offset_adc[2][0], para_p, 8);

      strcpy( &prompt[0], "Offset Compensation Chan3 :  [");
      strncat( &prompt[0], &adc3110_sign.offset_adc[3][0], 8);
      strcat( &prompt[0], "] : "); 
      para_p = cli_get_cmd( &adc3110_history, prompt);
      if( para_p[0] == 'q') return(-1);
      if( para_p[0]) strncpy( &adc3110_sign.offset_adc[3][0], para_p, 8);

      strcpy( &prompt[0], "Offset Compensation Chan4 :  [");
      strncat( &prompt[0], &adc3110_sign.offset_adc[4][0], 8);
      strcat( &prompt[0], "] : "); 
      para_p = cli_get_cmd( &adc3110_history, prompt);
      if( para_p[0] == 'q') return(-1);
      if( para_p[0]) strncpy( &adc3110_sign.offset_adc[4][0], para_p, 8);

      strcpy( &prompt[0], "Offset Compensation Chan5 :  [");
      strncat( &prompt[0], &adc3110_sign.offset_adc[5][0], 8);
      strcat( &prompt[0], "] : "); 
      para_p = cli_get_cmd( &adc3110_history, prompt);
      if( para_p[0] == 'q') return(-1);
      if( para_p[0]) strncpy( &adc3110_sign.offset_adc[5][0], para_p, 8);

      strcpy( &prompt[0], "Offset Compensation Chan6 :  [");
      strncat( &prompt[0], &adc3110_sign.offset_adc[6][0], 8);
      strcat( &prompt[0], "] : "); 
      para_p = cli_get_cmd( &adc3110_history, prompt);
      if( para_p[0] == 'q') return(-1);
      if( para_p[0]) strncpy( &adc3110_sign.offset_adc[6][0], para_p, 8);

      strcpy( &prompt[0], "Offset Compensation Chan7 :  [");
      strncat( &prompt[0], &adc3110_sign.offset_adc[7][0], 8);
      strcat( &prompt[0], "] : "); 
      para_p = cli_get_cmd( &adc3110_history, prompt);
      if( para_p[0] == 'q') return(-1);
      if( para_p[0]) strncpy( &adc3110_sign.offset_adc[7][0], para_p, 8);

    }
    if( op == 2)
    {
      time_t tm;
      int cnt;
      char ct[10];

      tm = time(0);
      strftime( ct, 10, "%d%m%Y", gmtime(&tm));
      printf("current date : %s\n", ct);

      strncpy( &adc3110_sign.board_name[0], " ADC3111", 8);
      strncpy( &adc3110_sign.serial[0], "0000", 4);
      strncpy( &adc3110_sign.version[0], "00000001", 8);
      strncpy( &adc3110_sign.revision[0], "A0", 2);
      strncpy( &adc3110_sign.test_date[0], ct, 8);
      strncpy( &adc3110_sign.calib_date[0], ct, 8);
      strncpy( &adc3110_sign.offset_adc[0][0], "00000000", 8);
      strncpy( &adc3110_sign.offset_adc[1][0], "00000000", 8);
      strncpy( &adc3110_sign.offset_adc[2][0], "00000000", 8);
      strncpy( &adc3110_sign.offset_adc[3][0], "00000000", 8);
      strncpy( &adc3110_sign.offset_adc[4][0], "00000000", 8);
      strncpy( &adc3110_sign.offset_adc[5][0], "00000000", 8);
      strncpy( &adc3110_sign.offset_adc[6][0], "00000000", 8);
      strncpy( &adc3110_sign.offset_adc[7][0], "00000000", 8);

      cnt = c->cnt - 3;
      i = 3;
      while( cnt--)
      {
	char *q;

        q =  c->para[i++];
        if( p[0] == 'b')
        {
          strncpy( &adc3110_sign.board_name[0], &q[2], 8);
        }
        if( p[0] == 's')
        {
	  strncpy( &adc3110_sign.serial[0], &q[2], 4);
        }
        if( p[0] == 'v')
        {
	  strncpy( &adc3110_sign.version[0], &q[2], 8);
        }
        if( p[0] == 'r')
        {
	  strncpy( &adc3110_sign.revision[0], &q[2], 2);
        }
      }
    }
    printf("ADC3110 signature\n");
    p = (unsigned char *)&adc3110_sign.board_name[0];
    printf("Board Name :  %c%c%c%c%c%c%c%c\n", p[0],p[1],p[2],p[3],p[4],p[5],p[6],p[7]);
    p = (unsigned char *)&adc3110_sign.serial[0];
    printf("Serial Number : %c%c%c%c\n", p[0],p[1],p[2],p[3]);
    p = (unsigned char *)&adc3110_sign.version[0];
    printf("PCB Version : %c%c%c%c%c%c%c%c\n", p[0],p[1],p[2],p[3],p[4],p[5],p[6],p[7]);
    p = (unsigned char *)&adc3110_sign.revision[0];
    printf("Hardware Revision : %c%c\n", p[0],p[1]);
    p = (unsigned char *)&adc3110_sign.test_date[0];
    printf("Test Date :  %c%c%c%c%c%c%c%c\n", p[0],p[1],p[2],p[3],p[4],p[5],p[6],p[7]);
    p = (unsigned char *)&adc3110_sign.calib_date[0];
    printf("Calibration Date :  %c%c%c%c%c%c%c%c\n", p[0],p[1],p[2],p[3],p[4],p[5],p[6],p[7]);
    p = (unsigned char *)&adc3110_sign.offset_adc[0][0];
    printf("Offset Compensation Chan#0 : %c%c%c%c%c%c%c%c\n", p[0],p[1],p[2],p[3],p[4],p[5],p[6],p[7]);
    p = (unsigned char *)&adc3110_sign.offset_adc[1][0];
    printf("Offset Compensation Chan#1 : %c%c%c%c%c%c%c%c\n", p[0],p[1],p[2],p[3],p[4],p[5],p[6],p[7]);
    p = (unsigned char *)&adc3110_sign.offset_adc[2][0];
    printf("Offset Compensation Chan#2 : %c%c%c%c%c%c%c%c\n", p[0],p[1],p[2],p[3],p[4],p[5],p[6],p[7]);
    p = (unsigned char *)&adc3110_sign.offset_adc[3][0];
    printf("Offset Compensation Chan#3 : %c%c%c%c%c%c%c%c\n", p[0],p[1],p[2],p[3],p[4],p[5],p[6],p[7]);
    p = (unsigned char *)&adc3110_sign.offset_adc[4][0];
    printf("Offset Compensation Chan#4 : %c%c%c%c%c%c%c%c\n", p[0],p[1],p[2],p[3],p[4],p[5],p[6],p[7]);
    p = (unsigned char *)&adc3110_sign.offset_adc[5][0];
    printf("Offset Compensation Chan#5 : %c%c%c%c%c%c%c%c\n", p[0],p[1],p[2],p[3],p[4],p[5],p[6],p[7]);
    p = (unsigned char *)&adc3110_sign.offset_adc[6][0];
    printf("Offset Compensation Chan#6 : %c%c%c%c%c%c%c%c\n", p[0],p[1],p[2],p[3],p[4],p[5],p[6],p[7]);
    p = (unsigned char *)&adc3110_sign.offset_adc[7][0];
    printf("Offset Compensation Chan#7 : %c%c%c%c%c%c%c%c\n", p[0],p[1],p[2],p[3],p[4],p[5],p[6],p[7]);
    printf("\n");

    if( op)
    {
      if( op == 1)
      {
        para_p = cli_get_cmd( &adc3110_history, "Overwrite ADC3110 signature ? [y/n] ");
        if( para_p[0] != 'y')
        {
  	  printf("EEPROM signature update aborted\n");
  	  return(-1);
        }
      }
      p = (unsigned char *)&adc3110_sign;
      for( i = 0x0; i < 0x100; i++)
      {
	data = p[i];
        tsc_i2c_write( device, (0x7000+i), data);
        //tsc_i2c_write( device, tsc_swap_16(0x7000+i), data);
	usleep(5000);
      }
      printf("EEPROM signature update done\n");
    }
    return(0);
  }
  else if( !strcmp( "dump", c->para[1]))
  {
    int device, i, j, off, size;
    unsigned char *p, *buf;

    device = add->cmd;
    if( device != 0x40010050)
    {
      printf(" sign command not supported for that device\n");
      return(-1);
    }
    if( c->cnt < 4)
    {
      printf("adc3110 eeprom dump command needs more arguments\n");
      printf("usage: adc3110.<fmc> eeprom dump <offset> <size>\n");
      return(-1);
    }
    if( fmc == 2)
    {
      device |= 2;
    }
    if( sscanf( c->para[2],"%x", &off) != 1)
    {
      printf("bad offset\n");
      printf("usage: adc3110.<fmc> eeprom dump <offset> <size>\n");
      return(-1);
    }
    if( sscanf( c->para[3],"%x", &size) != 1)
    {
      printf("bad size\n");
      printf("usage: adc3110.<fmc> eeprom dump <offset> <size>\n");
      return(-1);
    }
    printf("Displaying EEPROM from %x to %x\n", off, off+size);
    buf = (unsigned char *)malloc(size + 0x10);
    p = &buf[0];
    for( i = 0; i < size; i++)
    {
      //tsc_i2c_read( device, tsc_swap_16(off+i), &data);
      tsc_i2c_read( device, (off+i), &data);
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
  else 
  {
    printf("bad operation : %s\n",  c->para[1]);
    printf("usage: adc3110.<fmc> <dev> read <reg>\n");
    printf("       adc3110.<fmc> <dev> write <reg> <data>\n");
    printf("       adc3110.<fmc> <dev> acq <off> <size>\n");
    printf("       adc3110.<fmc> <file> save <off> <size>\n");
    printf("       adc3110.<fmc> <dev> show\n");
    return(-1);
  }
  return(0);

}
