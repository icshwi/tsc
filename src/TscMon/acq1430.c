/*=========================< begin file & file header >=======================
 *  References
 *
 *    filename : acq1430.c
 *    author   : JFG
 *    company  : IOxOS
 *    creation : november 14,2011
 *
 *----------------------------------------------------------------------------
 *  Description
 *
 *     That file contains a set of function called by TscMon to perform
 *     the control of the ACQ1430.
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

#ifndef lint
static char *rcsid = "$Id: acq1430.c,v 1.10 2014/12/19 09:36:19 ioxos Exp $";
#endif
#define TSC
#define ACQ1430

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

#include <tscextlib.h>
#include <tscioctl.h>
#include <tsculib.h>
#include <fmclib.h>
#include <adclib.h>
#include <ads42lb69lib.h>
#include <dac38j84lib.h>
#include <daq1430lib.h>
#include <dacwgenlib.h>
#include <scope1430lib.h>
#include <gscopelib.h>

#define min(a,b)      ((a < b) ? a : b)

#define BUS_SPI 1
#define BUS_I2C 2
#define BUS_SBC 3
#define BUS_INT 4
#define BUS_DRP 5

#define ADC_BASE_BMOV_A       0x00001100
#define ADC_BASE_BMOV_B       0x00001110

#define ADC_BASE_A          0x00001200
#define ADC_BASE_B          0x00001300

#define ADC_BASE_SIGN_A     (ADC_BASE_A +0x00)
#define ADC_BASE_SIGN_B     (ADC_BASE_B +0x00)

#define ADC_BASE_CSR_A     (ADC_BASE_A +0x04)
#define ADC_BASE_CSR_B     (ADC_BASE_B +0x04)

#define ADC_BASE_LED_A     (ADC_BASE_A +0x08)
#define ADC_BASE_LED_B     (ADC_BASE_B +0x08)

#define ADC_BASE_SERIAL_A   (ADC_BASE_A +0x0c)
#define ADC_BASE_SERIAL_B   (ADC_BASE_B +0x0c)

#define ADC_BASE_GPIO_A     (ADC_BASE_A +0x014)
#define ADC_BASE_GPIO_B     (ADC_BASE_B +0x014)

#define ADC_BASE_DACCTLL_A  (ADC_BASE_A +0x034)
#define ADC_BASE_DACCTLL_B  (ADC_BASE_B +0x034)

#define ADC_BASE_IDELAY_A  (ADC_BASE_A +0x038)
#define ADC_BASE_IDELAY_B  (ADC_BASE_B +0x038)

#define XUSER_SCOPE_TMEM_BUF_ADDR   0x100000
#define XUSER_SCOPE_TMEM_BUF_SIZE   0x20000

static struct tsc_acq1430_devices
{
  char *name;
  uint cmd;
  int idx;
  int bus;
}
acq1430_devices[] =
{
  { "lmk",            0x02000000,    -1, BUS_SBC },
  { "ads01",          0x01000000,     0, BUS_SBC },
  { "ads23",          0x01010000,     1, BUS_SBC },
  { "ads45",          0x01020000,     2, BUS_SBC },
  { "ads67",          0x01030000,     3, BUS_SBC },
  { "ads89",          0x01040000,     4, BUS_SBC },
  { "ads09",          0x01000000, 0x3FF, BUS_SBC },
  { "dac",            0x01070000,     7, BUS_SBC },
  { "wgen",           0x00000000,     0, BUS_INT },
  { "atest",          0x00000000,     1, BUS_INT },
  { "jesd",           0x00000000,     2, BUS_INT },
  { "gt226_lane0",    0x00000000,     0, BUS_DRP },
  { "gt226_lane1",    0x00010000,     1, BUS_DRP },
  { "gt226_lane2",    0x00020000,     2, BUS_DRP },
  { "gt226_lane3",    0x00030000,     3, BUS_DRP },
  { "gt228_lane0",    0x00040000,     4, BUS_DRP },
  { "gt228_lane1",    0x00050000,     5, BUS_DRP },
  { "gt228_lane2",    0x00060000,     6, BUS_DRP },
  { "gt228_lane3",    0x00070000,     7, BUS_DRP },
  { "gt226_common",   0x00080000,     8, BUS_DRP },
  { "gt228_common",   0x00090000,     9, BUS_DRP },
  { "plle3_ads01",    0x03000000,    20, BUS_SBC },
  { "plle3_ads23",    0x03010000,    21, BUS_SBC },
  { "plle3_ads45",    0x03020000,    22, BUS_SBC },
  { "plle3_ads67",    0x03030000,    23, BUS_SBC },
  { "plle3_ads89",    0x03040000,    24, BUS_SBC },
  { "mmcme3_ads",     0x03050000,    25, BUS_SBC },
  { "mmcme3_dac",     0x03070000,    27, BUS_SBC },
  { "all",            0x01000000,    15, BUS_SBC },
  { "tmp102",         0x40040048,     0, BUS_I2C },
  { "eeprom",         0x40010057,     1, BUS_I2C },
  { NULL, 0}
};

static int     acq1430_init_flag = 0;
static struct  cli_cmd_history acq1430_history;

/* waveform generator initialize ok flag */
static int      acq1430_wgen_init_ok = 0;


static int      acq1430_xuser_scope_type = -1;

#define ACQ1430_XUSER_SCOPE_1430    0   /* XUSER_SCOPE for 1430               */
#define ACQ1430_XUSER_GSCOPE_SMEM   1   /* XUSER_SCOPE Generic SMEM for 1430  */

static char    acq1430_prompt[32];
static uint    acq1430_histo[2][0x10000];

/*static char    filename[0x100];*/

//static unsigned char acq1430_buffer[1024];

struct acq1430_calib_res
{
  uint tot[2];
  uint min[2];
  uint max[2];
  float mean[2];
  float sig[2];
};

static struct acq1430_sign
{
  char board_name[8];
  char serial[4];
  char version[8];
  char revision[2];
  char rsv[6];
  char test_date[8];
  char calib_date[8];
  char offset_adc[10][8];
  char pad[147];
  char cks;
}
acq1430_sign;

struct acq1430_acq_res
{
  uint tot[2];
  short min[2];
  short max[2];
  float mean[2];
  float sig[2];
};

extern int tsc_fd;

/* -------------------------------------------------------------------------------------------------------------------*/

char *
acq1430_rcsid()
{
  return( rcsid);
}

/* -------------------------------------------------------------------------------------------------------------------*/

static int parse_str_array(char *instr, char *astr[])
{
  int i = 0;

  while (astr[i] != NULL)
  {
    if (!strcasecmp(instr, astr[i]))
      return i;
    i++;
  }
  return(-1);
}

/*--------------------------------------------------------------------------------------------------------------------*/

static int parse_field(char *field_name, char *field, int field_size, struct cli_cmd_history *history)
{
  char field_str[32];
  char prompt[64], rc=0;
  char *p;

  memset(field_str, 0, sizeof(field_str));
  strncpy(field_str, field, field_size);

  snprintf(prompt, sizeof(prompt), "%-30s :  [%s] : ", field_name, field_str);

  p = cli_get_cmd(history, prompt);

  if (p[0] != 0)
  {
    if (p[0] == 'q')
    {
      rc = -1;
    }
    else
    {
      rc = 1;
      strncpy(field, p, field_size);
    }
  }
  return (rc);
}

/*--------------------------------------------------------------------------------------------------------------------*/

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * Function name : acq1430_init
 * Prototype     : void
 * Parameters    : void
 * Return        : void
 *----------------------------------------------------------------------------
 * Description   : acq1430 init
 *
 *++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

void acq1430_init()
{
  if (!acq1430_init_flag)
  {
    daq1430_set_verbose(1);

    daq1430_reset(tsc_fd, 1);

    if (scope1430_identify(tsc_fd))
    {
      acq1430_xuser_scope_type = ACQ1430_XUSER_SCOPE_1430;
    }
    else if (gscope_identify(tsc_fd) == 0x12351430)
    {
      acq1430_xuser_scope_type = ACQ1430_XUSER_GSCOPE_SMEM;
    }
    else
    {
      acq1430_xuser_scope_type = -1;  /* unsupported */
    }

    cli_history_init( &acq1430_history);
    acq1430_init_flag = 1;
  }
}

/*--------------------------------------------------------------------------------------------------------------------*/

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * Function name : acq1430_acq_res
 * Prototype     : int
 * Parameters    : acq1430 structure
 * Return        : 0
 *----------------------------------------------------------------------------
 * Description   : acq1430 acquisition
 *
 *++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

int acq1430_acq_res( struct acq1430_acq_res *r, short *buf, int size, int chan)
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

/*--------------------------------------------------------------------------------------------------------------------*/

int acq1430_acq_check(int16_t *buf_a, int16_t *buf_b, int size)
{
  int i, nerr, min, max;

  nerr = 0;
  for (i = 0; i < size - 6; i++)
  {
    if(!(i & 0xffff))
    {
      printf("checking : %06x\r", i);
      fflush(stdout);
    }
    min = buf_a[i] - 40;
    max = buf_a[i] + 40;
    if ((buf_a[i+6] < min) || (buf_a[i+6] > max))
    {
      nerr++;
    }
    min = buf_b[i] - 40;
    max = buf_b[i] + 40;
    if ((buf_b[i+6] < min) || (buf_b[i+6] > max)) {
      nerr++;
    }
    printf("checking : %06x -> done [nerr = %d]\n", i, nerr);
  }
  return nerr;
}

/*--------------------------------------------------------------------------------------------------------------------*/



/*--------------------------------------------------------------------------------------------------------------------*/

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * Function name : acq1430_acq
 * Prototype     : int
 * Parameters    : cli command parameter structure, identifier, fmc, size, check
 * Return        : success/error
 *----------------------------------------------------------------------------
 * Description   : acq1430 command
 *
 *++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

/* t:<trig>
 * s:<smem>
 * c:<csr>
 * a:<last_addr>      -> Only manual trigger is writeable
 */

/* acq<???> [t:<trig>] [s:<smem>] [c:<csr>] [a:<last_addr>] [{f|h}:<acq_name_h>] [d:<acq_name_d>] */
int acq1430_acq(struct cli_cmd_para *c, int idx, int size, int check)
{
  int ret, unit, trig, smem, csr, tmo, nerr, i, file_his, file_dat, status;
  int16_t * buf0, *buf1;
  char *p, *acq_name_h, *acq_name_d, acq_name_h1[128], acq_name_h2[128], acq_name_d1[128], acq_name_d2[128];
  FILE *acq_file_h1, *acq_file_h2, *acq_file_d1, *acq_file_d2;
  uint64_t addr;
  struct tsc_ioctl_map_win * map;
  struct acq1430_acq_res acq_res;

  buf0 = NULL;
  buf1 = NULL;

  acq_file_h1 = NULL;
  acq_file_h2 = NULL;
  acq_file_d1 = NULL;
  acq_file_d2 = NULL;

  acq_name_h = NULL;
  acq_name_d = NULL;

  nerr = 0;
  trig = -1;
  smem = -1;
  csr = -1;

  file_his = 0;
  file_dat = 0;
  printf("parameter count: %ld\n", c->cnt);
  i = 2;
  for (i=2; i<c->cnt; i++)
  {
    p = c->para[i];
    printf("Setting parameters %d: %s\n", i, p);
    switch(p[0])
    {
      case 't':
        sscanf( p, "t:%x", &trig);
        break;

      case 's':
        sscanf( p, "s:%x", &smem);
        break;

      case 'c':
        sscanf( p, "c:%x", &csr);
        break;

      case 'f':
      case 'h':
        file_his = 1;
        acq_name_h = ((p[1] == ':') ? &p[2] : NULL);
        break;

      case 'd':
        file_dat = 1;
        acq_name_d = ((p[1] == ':') ? &p[2] : NULL);
        break;

      default:
        break;
    }
  }

  if (acq1430_xuser_scope_type == -1)
  {
    fprintf(stderr, "unsupported XUSER_SCOPE\n");
    return(-1);
  }



  if (acq1430_xuser_scope_type == ACQ1430_XUSER_GSCOPE_SMEM)
  {
    fprintf(stderr, "not implemented yet !\n");
    return(-1);
  }

  if (acq1430_xuser_scope_type == ACQ1430_XUSER_SCOPE_1430)
  {
    unit = SCOPE1430_UNIT((2*idx), SCOPE1430_SMEM);

    if (smem != -1)
    {
      ret = scope1430_csr_wr(tsc_fd, ((unit & 1) ? SCOPE1430_CSR_SMEM2_BASE : SCOPE1430_CSR_SMEM1_BASE), smem);
    }
    else
    {
      ret = scope1430_smem_init(tsc_fd, unit, 0x1000000, 8, SCOPE1430_SMEM_BURST_2K);
    }

    if (ret >= 0)
    {
      if (trig != -1)
      {
        ret = scope1430_csr_wr(tsc_fd, ((unit & 1) ? SCOPE1430_CSR_SMEM2_TRIG : SCOPE1430_CSR_SMEM1_TRIG), trig);
      }
      else
      {
        ret = scope1430_trigger_disable(tsc_fd, unit);
      }
    }

    if (ret >= 0)
    {
      if (csr != -1)
      {
        ret = scope1430_csr_wr(tsc_fd, ((unit & 1) ? SCOPE1430_CSR_SMEM2_CS : SCOPE1430_CSR_SMEM1_CS), csr);
      }
      else
      {
        ret = scope1430_trigger_set_pos(tsc_fd, unit, SCOPE1430_TRIG_POS_1_2);
      }
    }

    if (ret >= 0)
    {
      ret = scope1430_acq_set_mode(tsc_fd, unit, (1<<0));
    }

    if (ret >= 0)
    {
      ret = scope1430_smem_set_legacy_mode(tsc_fd, unit, (2*idx+0), (2*idx+1), 0);
    }

    if (ret >= 0)
    {
      ret = scope1430_acq_arm(tsc_fd, unit);
    }

    if (ret >= 0)
    {
      usleep(200000);
      ret = scope1430_manual_trigger(tsc_fd, unit);
    }

    if (ret >= 0)
    {
      tmo = 800;
      while (tmo > 0)
      {
        ret = scope1430_acq_status(tsc_fd, unit, &status);
        if (ret < 0)
          break;

        if ((status & SCOPE1430_STATUS_MASK) == SCOPE1430_STATUS_DONE)
          break;

        usleep(1000);
        tmo--;
      }
    }
    if (ret < 0)
    {
      return (ret);
    }
    if (tmo == 0)
    {
      fprintf(stderr, "Acquistion timeout...\n");
      return(-1);
    }
    ret = scope1430_acq_read(tsc_fd, unit, (2*idx+0), (uint16_t **)&buf0, NULL, 1);
    if (ret < 0)
    {
      return(ret);
    }
    ret = scope1430_acq_read(tsc_fd, unit, (2*idx+1), (uint16_t **)&buf1, NULL, 1);
    if (ret < 0)
    {
      return(ret);
    }
    map = scope1430_hwbuf_get_map_win(unit);
    addr = 0;
    if (map != NULL)
    {
      addr = map->req.rem_addr;
    }
    scope1430_cleanup(tsc_fd);
  }

  nerr = 0;

  if (file_his)
  {
    snprintf(acq_name_h1, sizeof(acq_name_h1), "%s_0.his", ((acq_name_h == NULL) ? "acq_file" : acq_name_h));
    snprintf(acq_name_h2, sizeof(acq_name_h2), "%s_1.his", ((acq_name_h == NULL) ? "acq_file" : acq_name_h));
    acq_file_h1 = fopen(acq_name_h1, "w");
    if (acq_file_h1 == NULL)
    {
      fprintf(stderr, "cannot create acquisition file %s\n", acq_name_h1);
      nerr++;
    }
    if (nerr == 0)
    {
      acq_file_h2 = fopen(acq_name_h2, "w");
      if (acq_file_h2 == NULL)
      {
        fprintf(stderr, "cannot create acquisition file %s\n", acq_name_h2);
        nerr++;
      }
    }
    printf("saving data acquision data from offset %lx [%x] to histogram file %s and %s\n", addr, size, acq_name_h1, acq_name_h2);
  }

  if (nerr == 0)
  {
    if (file_dat)
    {
      snprintf(acq_name_d1, sizeof(acq_name_d1), "%s_0.csv", ((acq_name_h == NULL) ? "acq_file" : acq_name_d));
      snprintf(acq_name_d2, sizeof(acq_name_d2), "%s_1.csv", ((acq_name_h == NULL) ? "acq_file" : acq_name_d));
      acq_file_d1 = fopen(acq_name_d1, "w");
      if (acq_file_d1 == NULL)
      {
        fprintf(stderr, "cannot create acquisition file %s\n", acq_name_d1);
        nerr++;
      }
      if (nerr == 0)
      {
        acq_file_d2 = fopen(acq_name_d2, "w");
        if (acq_file_d2 == NULL)
        {
          fprintf(stderr, "cannot create acquisition file %s\n", acq_name_d2);
          nerr++;
        }
      }
      printf("saving data acquision data from offset %lx [%x] to data file %s and %s\n", addr, size, acq_name_d1, acq_name_d2);
    }
  }

  if (nerr == 0)
  {
    /* save data */
    for (i=0; i<(size/2); i++)
    {
      if( !(i & 0xffff))
      {
        printf("saving : %06x\r", i);
        fflush( stdout);
      }
      if (file_dat)
      {
        fprintf(acq_file_d1, "%d\n", buf0[i]);
        fprintf(acq_file_d2, "%d\n", buf1[i]);
      }
      if (file_his)
      {
        fprintf(acq_file_h1, "%d, %d\n", i, buf0[i]);
        fprintf(acq_file_h2, "%d, %d\n", i, buf1[i]);
      }
    }
    printf("saving : %06x -> done\n", i);

    /* check data */
    if (check)
    {
      acq1430_acq_check(buf0, buf1, size);
    }

    /* calculate statistics */
    acq1430_acq_res(&acq_res, buf0, size, 0);
    acq1430_acq_res(&acq_res, buf1, size, 1);

    printf("Channel 0 : %f +/- %f [%d : %d]\n", acq_res.mean[0], acq_res.sig[0], acq_res.min[0], acq_res.max[0]);
    printf("Channel 1 : %f +/- %f [%d : %d]\n", acq_res.mean[1], acq_res.sig[1], acq_res.min[1], acq_res.max[1]);

    printf("Transfering histogramming files to host...");
    fflush( stdout);
  }

  if (acq_file_h1 != NULL)
  {
    fclose(acq_file_h1);
  }
  if (acq_file_h2 != NULL)
  {
    fclose(acq_file_h2);
  }
  if (acq_file_d1 != NULL)
  {
    fclose(acq_file_d1);
  }
  if (acq_file_d2 != NULL)
  {
    fclose(acq_file_d2);
  }
  if (buf0 != NULL)
  {
    free(buf0);
  }
  if (buf1 != NULL)
  {
    free(buf1);
  }

  if (nerr == 0)
  {
    printf(" -> done\n");
  }

  return(nerr);
}

/*--------------------------------------------------------------------------------------------------------------------*/

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * Function name : acq1430_calib_res
 * Prototype     : int
 * Parameters    : acq1430 calibration structure
 * Return        : success/error
 *----------------------------------------------------------------------------
 * Description   : acq1430 calibration
 *
 *++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
//
//int acq1430_calib_res( struct acq1430_calib_res *r)
//{
//  int i;
//
//  r->tot[0] = 0;
//  r->mean[0] = 0;
//  r->sig[0] = 0;
//  r->min[0] = 0xffff;
//  r->max[0] = 0x0;
//  r->tot[1] = 0;
//  r->sig[1] = 0;
//  r->min[1] = 0xffff;
//  r->max[1] = 0x0;
//  for( i = 0; i < 0x10000; i++)
//  {
//    if(acq1430_histo[0][i])
//    {
//      if( i < r->min[0])r->min[0] = i;
//      if( i > r->max[0])r->max[0] = i;
//    }
//    r->tot[0] += acq1430_histo[0][i];
//    r->mean[0] += (float)(i*acq1430_histo[0][i]);
//    if(acq1430_histo[1][i])
//    {
//      if( i < r->min[1])r->min[1] = i;
//      if( i > r->max[1])r->max[1] = i;
//    }
//    r->tot[1] += acq1430_histo[1][i];
//    r->mean[1] += (float)(i*acq1430_histo[1][i]);
//  }
//  r->mean[0] = r->mean[0]/r->tot[0];
//  r->mean[1] = r->mean[1]/r->tot[1];
//
//  for( i = 0; i < 0x10000; i++)
//  {
//    r->sig[0] += (i - r->mean[0])*(i - r->mean[0])*acq1430_histo[0][i];
//    r->sig[1] += (i - r->mean[1])*(i - r->mean[1])*acq1430_histo[1][i];
//  }
//  r->sig[0] = sqrt(r->sig[0]/r->tot[0]);
//  r->sig[1] = sqrt(r->sig[1]/r->tot[1]);
//
//  return(0);
//}

/*--------------------------------------------------------------------------------------------------------------------*/

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * Function name : adc_write
 * Prototype     : void
 * Parameters    : register, data, channel, fmc
 * Return        : void
 *----------------------------------------------------------------------------
 * Description   : write to adc
 *
 *++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

/*static void adc_write(int reg, int data, int chan, int fmc)
{
  int cmd;

  cmd =  0xc1000000 | (chan << 16) | reg;
  //printf("cmd = %08x - data = %08x\n", cmd, data);
  if( fmc == 2)
  {
    tscext_csr_wr(tsc_fd, ADC_BASE_SERIAL_B + 4, data);
    tscext_csr_wr(tsc_fd, ADC_BASE_SERIAL_B, cmd);
  }
  else
  {
    tscext_csr_wr(tsc_fd, ADC_BASE_SERIAL_A + 4, data);
    tscext_csr_wr(tsc_fd, ADC_BASE_SERIAL_A, cmd);
  }
}*/

/*--------------------------------------------------------------------------------------------------------------------*/

int acq1430_calib_idelay(struct cli_cmd_para *c, int chan, int fmc)
{
  int rc, tmp, step;
  
  const char *__usage = "adc1430.<fmc> %s {calib|calidel} [<step>]\n";
  
  if (chan < -1 || chan > 4)
  {
    fprintf(stderr, "invalid adc channel\n");
    fprintf(stderr, __usage, c->para[0]);
    return (-1);
  }

  step = -1;

  if (c->cnt >= 3)
  {
    if (sscanf(c->para[2], "%d", &tmp) != 1 && (tmp < 1) && (tmp > 32))
    {
      fprintf(stderr, "invalid delay step, expected a numeric value from 1 to 32\n");
      fprintf(stderr, __usage, c->para[0]);
      return (-1);      
    }
    step = tmp;
  }
  
  rc = daq1430_calibrate(tsc_fd, fmc, chan, step);
  
  return(rc);
}

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * Function name : acq1430_calib_idelay
 * Prototype     : int
 * Parameters    : cli command parameter structure, channel, fmc
 * Return        : success/error
 *----------------------------------------------------------------------------
 * Description   : calibrate idelay of acq1430
 *
 *++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

//int acq1430_calib_idelay( struct cli_cmd_para *c, int chan, int fmc)
//{
//  struct tsc_ioctl_map_win adc_mas_map_usr[5];
///*  char *adc_buf1[5], *adc_buf2[5], *p;*/
//  char *adc_buf1[5], *adc_buf2[5];
//  int csr_base[5], idelay, idelay_base, tmp;
//  int i, n;
//  int res[5][2][64];
//  unsigned short data_ref;
//  int min, max;
//  int start, end;
//
//  idelay_base = ADC_BASE_IDELAY_A;
////  if(fmc == 2)
////  {
////    idelay_base = ADC_BASE_IDELAY_B;
////  }
//  if( chan != -1)
//  {
//    if( (chan < 0) || (chan > 4))
//    {
//      printf("Bad ADC channel [%d]\n", chan);
//      return(-1);
//    }
//  }
//  start = 0;
//  end = 0x200;
//  if( c->cnt > 2)
//  {
//    int s,e;
//    if( sscanf( c->para[2],"%x..%x", &s, &e) != 2)
//    {
//      printf("Bad idelay range [%s]\n", c->para[2]);
//      return(-1);
//    }
//    if( (s < end) && ( s >= 0) && (e < 0x200))
//    {
//      start = s;
//      end = e;
//    }
//  }
//
//  /* Allocate TMEM_BUF for XUSER_SCOPE */
//
//  for( i = 0; i < 5; i++)
//  {
//    bzero( &adc_mas_map_usr[i], sizeof(adc_mas_map_usr[i]));
//    adc_mas_map_usr[i].req.mode.sg_id= MAP_ID_MAS_PCIE_MEM;
////    if( fmc == 2)
////    {
////        adc_mas_map_usr[i].req.mode.space = MAP_SPACE_USR2;
/////*        adc_mas_map_usr[i].req.rem_addr = 0x1100000 + (0x20000*i);*/
////        adc_mas_map_usr[i].req.rem_addr = XUSER_SCOPE_TMEM_BUF_ADDR + (XUSER_SCOPE_TMEM_BUF_SIZE * i);
////        csr_base[i] = 0x11d0;
////    }
////    else
////    {
//      /* 01, 23, 45, 67 */
//      if( i < 4)
//      {
//        adc_mas_map_usr[i].req.mode.space = MAP_SPACE_USR1;
///*        adc_mas_map_usr[i].req.rem_addr = 0x1100000 + (0x20000*i);*/
//        adc_mas_map_usr[i].req.rem_addr = XUSER_SCOPE_TMEM_BUF_ADDR + (XUSER_SCOPE_TMEM_BUF_SIZE * i);
//        csr_base[i] = 0x11c0;
//      }
//      /* 89 */
//      else
//      {
//        adc_mas_map_usr[i].req.mode.space = MAP_SPACE_USR2;
///*        adc_mas_map_usr[i].req.rem_addr = 0x1100000;*/
//        adc_mas_map_usr[i].req.rem_addr = XUSER_SCOPE_TMEM_BUF_ADDR;
//        csr_base[i] = 0x11d0;
//      }
////    }
//    adc_mas_map_usr[i].req.size = 0x20000;
//    tsc_map_alloc(tsc_fd, &adc_mas_map_usr[i]);
//    adc_buf1[i] = (char *)tsc_pci_mmap(tsc_fd, adc_mas_map_usr[i].sts.loc_base, adc_mas_map_usr[i].sts.size);
//    adc_buf2[i] = adc_buf1[i] + 0x10000;
//    for( n = 0; n < 64; n++)
//    {
//      res[i][0][n] = 1;
//      res[i][1][n] = 1;
//    }
//  }
//
//
//  /* Set RAMP MODE */
//  /* ADC channel initialization : 0x44 -> digital ramp */
//  if( chan == -1)
//  {
//    printf("Adjust idelay for all ADC channels [ads01 -> adc89]\n");
//    for( i = 0; i < 5; i++)
//    {
//      daq1430_ads42lb69_set_mode(fmc, 2*i+0, ADS42LB69_MODE_RAMP);
//      daq1430_ads42lb69_set_mode(fmc, 2*i+1, ADS42LB69_MODE_RAMP);
//      //adc_write( 0xf, 0x44, i, fmc);
//    }
//  }
//  else
//  {
//    printf("Adjust idelay for ads%d%d\n", 2*chan, 2*chan + 1);
//    daq1430_ads42lb69_set_mode(fmc, 2*chan+0, ADS42LB69_MODE_RAMP);
//    daq1430_ads42lb69_set_mode(fmc, 2*chan+1, ADS42LB69_MODE_RAMP);
//    //adc_write( 0xf, 0x44, chan, fmc);
//  }
//  usleep( 5000);
//
//  //tscext_csr_wr(tsc_fd, idelay_base, 0x8000ffff); /*  RESET IDELAYE3 + ISERDES3 */
//  //tscext_csr_wr(tsc_fd, idelay_base, 0x00000000); /*                            */
//  //usleep( 1000);
//
//  /* scan IDELAY     */
////  for( i = 0; i < 5; i++)
////  {
//    //printf("adc_buf1[i] = %p -> %08x\n", adc_buf1[i], *(int *)&adc_buf1[i][0]);
////  }
//
//  for( idelay = start; idelay < end; idelay +=8)
//    //for( idelay = 0xa0; idelay < 0xa1; idelay +=8)
//  {
//    printf("idelay = %02x\r", idelay);
//    fflush(stdout);
//
//    for( i = 0; i < 5; i++)
//    {
//      if( (chan == -1) || (chan == i))
//      {
//        /* fill memory with 0xa5 */
//        memset( adc_buf1[i], 0xa5, adc_mas_map_usr[i].sts.size);
//        //printf("%3d : set adc_buf: %08x\n", idelay, *(unsigned long *)&adc_buf1[i][0]);
//
//        /* set IDELAY value */
//        //tscext_csr_wr(tsc_fd, idelay_base, 0x00000FFF | (idelay << 16) | (i << 12)); /* Load IDELAY Count Channel_xy */
//        //tscext_csr_wr(tsc_fd, idelay_base, 0x10000FFF | (idelay << 16) | (i << 12)); /*                              */
//        tmp = idelay;
//        daq1430_calib_set_idelay(fmc, chan, tmp);
//      }
//    }
//    /* trig data acquistion */
//
//    if(( chan == -1) || ( chan < 4))
//    {
//      //printf("trig acquisition for channel %d\n", chan);
////      tscext_csr_wr(tsc_fd, csr_base[0] + 4, 0x00000000); /*  SCOPE_ACQ1430 SRAM1 Trigger mode    */
////      tscext_csr_wr(tsc_fd, csr_base[0] + 0, 0x80000021); /*  SCOPE_ACQ1430 SRAM1 Trigger mode    */
////      tscext_csr_wr(tsc_fd, csr_base[0] + 0, 0x40000021); /*  SCOPE_ACQ1430 SRAM1 Mode            */
////      usleep( 1000);
////      tscext_csr_wr(tsc_fd, csr_base[0] + 8, 0x40000000); /*  Force Trigger          */
//
//      scope1430_trigger_disable(SCOPE1430_UNIT_SRAM1);
//      scope1430_trigger_set_pos(SCOPE1430_UNIT_SRAM1, SCOPE1430_TRIG_POS_1_2);
//      scope1430_acq_set_mode(SCOPE1430_UNIT_SRAM1, SCOPE1430_MODE_SINGLE);
//      scope1430_acq_arm(tsc_fd, SCOPE1430_UNIT_SRAM1);
//      usleep( 1000);
//      scope1430_manual_trigger(tsc_fd, SCOPE1430_UNIT_SRAM1);
//    }
//    if(( chan == -1) || ( chan == 4))
//    {
//      //printf("trig acquisition for channel %d\n", chan);
////      tscext_csr_wr(tsc_fd, csr_base[4] + 4, 0x00000000); /*  SCOPE_ACQ1430 SRAM2 Trigger mode    */
////      tscext_csr_wr(tsc_fd, csr_base[4] + 0, 0x80000021); /*  SCOPE_ACQ1430 SRAM2 Trigger mode    */
////      tscext_csr_wr(tsc_fd, csr_base[4] + 0, 0x40000021); /*  SCOPE_ACQ1430 SRAM2 Mode            */
////      usleep( 1000);
////      tscext_csr_wr(tsc_fd, csr_base[4] + 8, 0x40000000); /*  Force Trigger          */
//
//      scope1430_trigger_disable(SCOPE1430_UNIT_SRAM2);
//      scope1430_trigger_set_pos(SCOPE1430_UNIT_SRAM2, SCOPE1430_TRIG_POS_1_2);
//      scope1430_acq_set_mode(SCOPE1430_UNIT_SRAM2, SCOPE1430_MODE_SINGLE);
//      scope1430_acq_arm(tsc_fd, SCOPE1430_UNIT_SRAM2);
//      usleep( 1000);
//      scope1430_manual_trigger(tsc_fd, SCOPE1430_UNIT_SRAM2);
//    }
//    usleep( 2000);                       /*  wait for acquisition to complete          */
//
//    for( i = 0; i < 5; i++)
//    {
//      if( (chan == -1) || (chan == i))
//      {
//        //printf("check data integrity  for channel %d [%d]\n", i, chan);
//        data_ref = *(unsigned char *)&adc_buf1[i][0x2000] | (*(unsigned char *)&adc_buf1[i][0x2001] << 8);
//        /* check data first channel*/
//        for( n = 0x2002; n < 0x10000; n+=2)
//        {
//          unsigned short data;
//          //if( n < 16) printf("%04x\n", data_ref);
//          data = *(unsigned char *)&adc_buf1[i][n] | (*(unsigned char *)&adc_buf1[i][n+1] << 8);
//          if( data != ((data_ref + 1)&0xffff))
//          {
//  	    //printf("CHAN%d:%3d : error at offset %04x : %04x != %04x\n", i*2, idelay/8, n, data, data_ref+1);
//            res[i][0][idelay/8] = 0;
//            break;
//          }
//          data_ref = data;
//        }
//        /* check data second channel*/
//        data_ref = *(unsigned char *)&adc_buf2[i][0x2000] | (*(unsigned char *)&adc_buf2[i][0x2001] << 8);
//        for( n = 0x2002; n < 0x10000; n+=2)
//        {
//          unsigned short data;
//
//          //if( n < 16)printf("%04x\n", data_ref);
//          data = *(unsigned char *)&adc_buf2[i][n] | (*(unsigned char *)&adc_buf2[i][n+1] << 8);
//          if( data != ((data_ref + 1)&0xffff))
//          {
//            //printf("CHAN%d:%3d : error at offset %04x : %04x != %04x\n", i*2+1, idelay/8, n, data, data_ref+1);
//            res[i][1][idelay/8] = 0;
//            break;
//          }
//          data_ref = data;
//        }
//      }
//    }
//  }
//  for( i = 0; i < 5; i++)
//  {
//    if( (chan == -1) || (chan == i))
//    {
//      int max_found;
//      int start_found;
//
//      max_found = 0;
//      start_found = 0;
//      printf("\n");
//      min = end/8; max = start/8;
//      printf("chan %d : ", 2*i);
//      for( n = start/8; n < end/8; n++)
//      {
//        printf("%d", res[i][0][n]);
//        if( res[i][0][n])
//        {
//          if( n < min)
//	  {
//	    if( start_found == 1)
//	    {
//	      min = n;
//	      start_found = 2;
//	    }
//	  }
//          if( n > max)
//          {
//	    if( (max_found != 2) && ( start_found == 2))
//	    {
//	      max = n;
//	      max_found = 1;
//	    }
//	  }
//        }
//	else
//	{
//	  if( start_found == 0) start_found = 1;
//	  if( max_found == 1) max_found = 2;
//	}
//      }
//      idelay = (min+max)*4;
//      printf(" [%02x-%02x-%02x]\n", min*8, idelay, max*8);
//      /* set IDELAY value */
//      tscext_csr_wr(tsc_fd, idelay_base, 0x0000003F | (idelay << 16) | (i << 12)); /* Load IDELAY Count Channel_xy */
//      tscext_csr_wr(tsc_fd, idelay_base, 0x1000003F | (idelay << 16) | (i << 12)); /*                              */
//      printf("Loading IDELAY %03x in channel %d\n", idelay, i*2);
//
//      max_found = 0;
//      start_found = 0;
//      printf("\n");
//      min = end/8; max = start/8;
//      printf("chan %d : ", 2*i+1);
//      for( n = start/8; n < end/8; n++)
//      {
//        printf("%d", res[i][1][n]);
//        if( res[i][1][n])
//        {
//          if( n < min)
//	  {
//	    if( start_found == 1)
//	    {
//	      min = n;
//	      start_found = 2;
//	    }
//	  }
//          if( n > max)
//          {
//	    if( (max_found != 2) && ( start_found == 2))
//	    {
//	      max = n;
//	      max_found = 1;
//	    }
//	  }
//        }
//	else
//	{
//	  if( start_found == 0) start_found = 1;
//	  if( max_found == 1) max_found = 2;
//	}
//      }
//      idelay = (min+max)*4;
//      printf(" [%02x-%02x-%02x]\n", min*8, idelay, max*8);
//      /* set IDELAY value */
//      tscext_csr_wr(tsc_fd, idelay_base, 0x00000FC0 | (idelay << 16) | (i << 12)); /* Load IDELAY Count Channel_xy */
//      tscext_csr_wr(tsc_fd, idelay_base, 0x10000FC0 | (idelay << 16) | (i << 12)); /*                              */
//      printf("Loading IDELAY %03x in channel %d\n", idelay, i*2 + 1);
//
//    }
//  }
//
//  /* Free XUSER_SCOPE TMEM_BUF */
//  for( i = 0; i < 5; i++)
//  {
//    //printf("unmapping adc_buf\n");
//    tsc_pci_munmap( adc_buf1[i], adc_mas_map_usr[i].sts.size);
//    //printf("free adc_mas_map_usr\n");
//    tsc_map_free(tsc_fd, &adc_mas_map_usr[i]);
//  }
//
//  return(0);
//}
/*--------------------------------------------------------------------------------------------------------------------*/

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * Function name : wgen_start
 * Prototype     : int
 * Parameters    : cli command parameter structure
 * Return        : success/error
 *----------------------------------------------------------------------------
 * Description   : Start waveform generator
 *
 *++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

/* acq1430.x wgen start <ch> <mode> */

static int acq1430_wgen_start(struct cli_cmd_para *c)
{
  int rc = 0, chan, mode;
  char tmp;

  chan = 0;
  mode = 0;

  if (!acq1430_wgen_init_ok)
  {
    rc = dacwgen_init(tsc_fd);
    if (rc<0)
      return(rc);
    acq1430_wgen_init_ok = 1;
  }

  if (c->cnt < 4)
  {
    fprintf(stderr, "acq1430.<fmc> wgen start command needs more arguments\n"                               \
                    "usage: acq1430.<fmc> wgen start <ch> <mode>\n"                                         \
                    "       where <ch>   = channel number (0 to 3)\n"                                       \
                    "             <mode> = 's' = single,\n"                                                 \
                    "                      'l' = loop,\n"                                                   \
                    "                      'd' = periodic xy-symetrical f(x) = f(p-x),\n"                   \
                    "                      'i' = periodic  y-symetrical f(x) = f(p-x) & f(-x) = -f(x) or\n" \
                    "                      'q' = square wave\n");
    return (-1);
  }

  if (sscanf(c->para[2], "%d", &chan) != 1)
  {
    fprintf(stderr, "missing channel number\n");
    return(-1);
  }

  if (chan < 0 || chan > 4)
  {
    fprintf(stderr, "invalid channel number\n");
    return(-1);
  }

  if (sscanf(c->para[3], "%c", &tmp) != 1)
  {
    fprintf(stderr, "missing mode\n");
    return(-1);
  }

  switch(tmp)
  {
    /* single */
    case 's':
      mode = XUSER_WGEN_MODE_LUT_SINGLE;
      break;

    /* loop */
    case 'l':
      mode = XUSER_WGEN_MODE_LUT_CYCLIC;
      break;

      /* periodic xy-symetrical */
    case 'd':
      mode = XUSER_WGEN_MODE_PERIODIC_XY_SYMETRICAL;
      break;

      /* periodic y-symetrical */
    case 'i':
      mode = XUSER_WGEN_MODE_PERIODIC_Y_SYMETRICAL;
      break;

      /* square wave */
    case 'q':
      mode = XUSER_WGEN_MODE_SQUARE_WAVE;
      break;

    default:
      fprintf(stderr, "invalid mode '%c'.\n", tmp);
      return (-1);
  }

  if (rc >= 0)
  {
    rc = dacwgen_start_wave(tsc_fd, chan, mode);
  }

  return (rc);
}

/*--------------------------------------------------------------------------------------------------------------------*/

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * Function name : wgen_stop
 * Prototype     : int
 * Parameters    : cli command parameter structure
 * Return        : success/error
 *----------------------------------------------------------------------------
 * Description   : Stop waveform generator
 *
 *++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

/* acq1430.x wgen stop */

static int acq1430_wgen_stop(struct cli_cmd_para *c)
{
  int rc = 0;

  if (!acq1430_wgen_init_ok)
  {
    rc = dacwgen_init(tsc_fd);
    if (rc<0)
      return(rc);
    acq1430_wgen_init_ok = 1;
  }

  if (rc >=0)
    rc = dacwgen_stop_wave(tsc_fd, -1);

  return (rc);
}

/*--------------------------------------------------------------------------------------------------------------------*/

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * Function name : wgen_load
 * Prototype     : int
 * Parameters    : cli command parameter structure
 * Return        : success/error
 *----------------------------------------------------------------------------
 * Description   : Load a file into waveform generator buffer
 *
 *++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

/* acq1430.x wgen load <ch> <filename> */

static int acq1430_wgen_load(struct cli_cmd_para *c)
{
  int rc, chan;

  if (!acq1430_wgen_init_ok)
  {
    rc = dacwgen_init(tsc_fd);
    if (rc >= 0) acq1430_wgen_init_ok = 1;
  }

  if(c->cnt < 4)
  {
    fprintf(stderr, "acq1430.<fmc> wgen load command needs more arguments\n"                                \
                    "usage: acq1430.<fmc> wgen load <ch> <filename>\n"                                      \
                    "       where <ch>   = channel number (0 to 3)\n"                                       \
                    "             <filename> = name of the file to load\n");
    return(-1);
  }

  if (sscanf(c->para[2], "%d", &chan) != 1)
  {
    fprintf(stderr, "missing channel number\n");
    return(-1);
  }

  if (chan < 0 || chan > 4)
  {
    fprintf(stderr, "invalid channel number\n");
    return(-1);
  }

  rc = dacwgen_fill_file(tsc_fd, chan, c->para[3], 0);

  return(rc);
}

/*--------------------------------------------------------------------------------------------------------------------*/

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * Function name : wgen_set
 * Prototype     : int
 * Parameters    : cli command parameter structure
 * Return        : success/error
 *----------------------------------------------------------------------------
 * Description   : Set parameters of waveform generator
 *
 *++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

/* acq1430.x wgen set <ch> <default> [<initial> [<step>]] */

static int acq1430_wgen_set(struct cli_cmd_para *c)
{
  int rc, chan, def_val, ini_val = -1, step = -1;

  if (!acq1430_wgen_init_ok)
  {
    rc = dacwgen_init(tsc_fd);
    if (rc<0)
      return(rc);
    acq1430_wgen_init_ok = 1;
  }

  if(c->cnt < 4)
  {
    fprintf(stderr, "acq1430.<fmc> wgen set command needs more arguments\n"                                 \
                    "usage: acq1430.<fmc> wgen set <ch> <default> <initial> <step>\n"                       \
                    "       where <ch>      = channel number (0 to 3)\n"                                    \
                    "             <default> = default value when waveform generator is stopped\n"           \
                    "             <initial> = initial value\n"                                              \
                    "             <step>    = step value\n");
    return(-1);
  }

  if (sscanf(c->para[2], "%d", &chan) != 1)
  {
    fprintf(stderr, "missing channel number\n");
    return(-1);
  }

  if (chan < 0 || chan > 4)
  {
    fprintf(stderr, "invalid channel number\n");
    return(-1);
  }

  if (c->para[3][0] == '-')
  {
    def_val = -1;
  }
  else
  {
    if (sscanf(c->para[3], "%x", &def_val) != 1)
    {
      fprintf(stderr, "missing default value\n");
      return (-1);
    }

    if (def_val < 0 && def_val > 0xFFFF)
    {
      fprintf(stderr, "invalid default value (must be between 0x0000 and 0xFFFF)\n");
      return (-1);
    }
  }

  if (c->cnt > 4)
  {
    if (c->para[4][0] == '-')
    {
      ini_val = -1;
    }
    else
    {
      if (sscanf(c->para[4], "%d", &ini_val) != 1)
      {
        fprintf(stderr, "missing initial value\n");
        return (-1);
      }

      if (ini_val < 0 && ini_val > 0xFFFFFF)
      {
        fprintf(stderr, "invalid initial value (must be between 0 and 16'777'215)\n");
        return (-1);
      }
    }
  }

  if (c->cnt > 5)
  {
    if (sscanf(c->para[5], "%d", &step) != 1)
    {
      fprintf(stderr, "missing step value\n");
      return (-1);
    }

    if (step < 0 && step > 0xFFFFFF)
    {
      fprintf(stderr, "invalid step value (must be between 0 and 16'777'215)\n");
      return (-1);
    }
  }

  rc = dacwgen_set_wave(tsc_fd, chan, def_val, ini_val, step, 0);

  return (rc);
}

/*--------------------------------------------------------------------------------------------------------------------*/

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * Function name : acq1430_save_raw
 * Prototype     : int
 * Parameters    : cli command parameter structure
 * Return        : success/error
 *----------------------------------------------------------------------------
 * Description   : -
 *
 *++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

static int acq1430_save_raw(char *acq_name, char *acq_buf, int size)
{
  char fname[64];
  FILE *fout = stdout;
  int i, chan, big_endian;
  short sample;

  big_endian = CheckByteOrder();

  if (acq_name != NULL)
  {
    strncpy(fname, acq_name, sizeof(fname));
    strncat(fname, ".his",   sizeof(fname));

    fout = fopen(fname, "w");
    if(fout == NULL)
    {
      printf("cannot create acquisition file %s\n", fname);
      return(-1);
    }
  }

  chan = 0;
  for(i=0; i<size; i+=2)
  {
    if (big_endian)
    {
      sample = ((acq_buf[i+1]<<8) | acq_buf[i+0]);
    }
    else
    {
      sample = ((acq_buf[i+0]<<8) | acq_buf[i+1]);
    }
    fprintf(fout, "%d %d\n", chan, sample);
    chan++;
  }

  if(acq_name != NULL)
  {
    fclose(fout);
  }

  return 1;
}

/*--------------------------------------------------------------------------------------------------------------------*/

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * Function name : acq1430_save_legacy
 * Prototype     : int
 * Parameters    : cli command parameter structure
 * Return        : success/error
 *----------------------------------------------------------------------------
 * Description   : -
 *
 *++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

static int acq1430_save_legacy(char *acq_name, char *acq_buf, int size, int trig_pos, int pre_size)
{
  char fname[64];
  FILE *fout_a = stdout;
  FILE *fout_b = stdout;
  int i, n, big_endian, pos;
  short sample_a, sample_b;

  big_endian = CheckByteOrder();

  if (acq_name != NULL)
  {
    strncpy(fname, acq_name, sizeof(fname));
    strncat(fname, "_a.his", sizeof(fname));

    fout_a = fopen(fname, "w");
    if(fout_a == NULL)
    {
      printf("cannot create acquisition file %s\n", fname);
      return(-1);
    }
    strncpy(fname, acq_name, sizeof(fname));
    strncat(fname, "_b.his", sizeof(fname));

    fout_b = fopen(fname, "w");
    if(fout_b == NULL)
    {
      fclose(fout_a);
      printf("cannot create acquisition file %s\n", fname);
      return(-1);
    }
  }
  n = 0;
  pos = ((trig_pos+4) % pre_size);

  for(i=0; i<size; i+=4)
  {
    if (big_endian)
    {
      sample_a = ((acq_buf[pos+1]<<8) | acq_buf[pos+0]);
      sample_b = ((acq_buf[pos+3]<<8) | acq_buf[pos+2]);
    }
    else
    {
      sample_a = ((acq_buf[pos+0]<<8) | acq_buf[pos+1]);
      sample_b = ((acq_buf[pos+3]<<8) | acq_buf[pos+2]);
    }
    fprintf(fout_a, "%d %d\n", n, sample_a);
    fprintf(fout_b, "%d %d\n", n, sample_b);
    n++;

    if (pos < pre_size)
    {
      if (pos == trig_pos)
      {
        pos = pre_size;
      }
      else
      {
        pos = ((pos + 4) % pre_size);
      }
    }
    else
    {
      pos += 4;
    }
  }

  if(acq_name != NULL)
  {
    fclose(fout_a);
    fclose(fout_b);
  }

  return 1;
}

/*--------------------------------------------------------------------------------------------------------------------*/

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * Function name : acq1430_save_new
 * Prototype     : int
 * Parameters    : cli command parameter structure
 * Return        : success/error
 *----------------------------------------------------------------------------
 * Description   : -
 *
 *++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

static int acq1430_save_new(char *acq_name, char *acq_buf, int size, int trig_pos, int pre_size)
{
  char fname[64];
  FILE *fout[8];
  int i, n, ch, big_endian, pos, err;
  short sample[16];

  big_endian = CheckByteOrder();

  if (acq_name != NULL)
  {
    err = 0;
    for (ch=0; ch<8; ch++)
    {
      snprintf(fname, sizeof(fname), "%s_%d.his", acq_name, ch);
      fout[ch] = fopen(fname, "w");
      if (fout[ch] == NULL)
      {
        printf("cannot create acquisition file %s\n", fname);
        err = 1;
        break;
      }
    }
    if (err)
    {
      for (i=0; i<ch; i++)
      {
        fclose(fout[i]);
      }
      return (-1);
    }
  }

  /*printf("trig_pos = 0x%X\n", trig_pos);*/
  n = 0;
  pos = ((trig_pos+32) % pre_size);
  /*printf("pos = 0x%X\n", pos);*/
  for(i=0; i<size; i+=32)
  {
    if (i == pre_size)
    {
      /*printf("i = 0x%X, pos = 0x%X, pre_size = 0x%X\n", i, pos, pre_size);*/
      /*printf("trig\n");*/
      pos = pre_size;
      /*printf("i = 0x%X, pos = 0x%X, pre_size = 0x%X\n", i, pos, pre_size);*/
    }

    for (ch=0; ch<16; ch++)
    {
      if (big_endian)
      {
        sample[ch] = ((acq_buf[pos+1]<<8) | acq_buf[pos+0]);
      }
      else
      {
        sample[ch] = ((acq_buf[pos+0]<<8) | acq_buf[pos+1]);
      }

      if (pos < pre_size)
      {
        pos = (pos + 2) % pre_size;
      }
      else
      {
        pos = (pos + 2);
      }
    }

    for (ch=0; ch<8; ch++)
    {
      fprintf(fout[ch], "%d %d\n", (n+0), sample[2*ch+0]);
      fprintf(fout[ch], "%d %d\n", (n+1), sample[2*ch+1]);
    }
    n+=2;
  }

  if(acq_name != NULL)
  {
    for (ch=0; ch<8; ch++)
    {
      fclose(fout[ch]);
    }
  }

  return 1;
}

/*--------------------------------------------------------------------------------------------------------------------*/

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * Function name : acq1430_save
 * Prototype     : int
 * Parameters    : cli command parameter structure
 * Return        : success/error
 *----------------------------------------------------------------------------
 * Description   : -
 *
 *++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/


/* acq1430.x <filename> save{.s1|.s2|.u2|.u1} <offset> <size> {<fmt>} */

static int acq1430_save(struct cli_cmd_para *c)
{
  const char *_acq1430_save_usage =
    "usage: acq1430.1 <filename> save{.s|.s2|.u|u2} {<offset> {<size>}}\n"  \
    "where  <filename>      is the basename of the file or '?' or '-'\n"    \
    "       <offset>        start position of samples\n"                    \
    "       <size>          size in bytes\n";

  char acq_name[64];
  int offset, size, trig_pos, pre_size, mode, sram, rc;
  char *acq_buf;
  int space, tmp, csr_base;
  struct tsc_ioctl_map_win shm_mas_map_win;
  /*struct acq1430_calib_res res;*/

  mode = -1;
  sram = 0;
  space = MAP_SPACE_SHM;
  csr_base = 0x11E0;
  if(c->para[1][4] == '.')
  {
    if(c->para[1][5] == 's')
    {
      if (c->para[1][6] == '2')
      {
        space = MAP_SPACE_SHM2;
        csr_base = 0x11E0;
      }
    }
    if(c->para[1][5] == 'u')
    {
      sram = 1;
      if (c->para[1][6] == '2')
      {
        space = MAP_SPACE_USR2;
        csr_base = 0x11D0;
      }
      else
      {
        space = MAP_SPACE_USR1;
        csr_base = 0x11C0;
      }
    }
  }
  if (c->cnt < 1)
  {
    fprintf(stderr, "acq1430 acq command needs more arguments\n");
    fprintf(stderr, "%s", _acq1430_save_usage);
    return(-1);
  }

  if (c->cnt>2)
  {
    if( sscanf( c->para[2],"%x", &offset) != 1)
    {
      fprintf(stderr, "wrong offset value\n");
      fprintf(stderr, "%s", _acq1430_save_usage);
      return(-1);
    }
  }

  if (c->cnt>3)
  {
    if( sscanf( c->para[3],"%x", &size) != 1)
    {
      fprintf(stderr, "wrong size value\n");
      fprintf(stderr, "%s", _acq1430_save_usage);
      return(-1);
    }
  }

  if(  c->para[0][0] != '-')
  {
    if(  c->para[0][0] == '?')
    {
      strcpy( acq1430_prompt, "enter filename: ");
      c->para[0] = cli_get_cmd( &acq1430_history, acq1430_prompt);
    }
    strncpy(acq_name, c->para[0], sizeof(acq_name));
  }

  /* if offset and size are not given -> read the offset, size, trig_pos and pre_size from registers */
  if (c->cnt==2)
  {
    if (sram)
    {
      fprintf(stderr, "this mode is no supported for sram scope\n");
      fprintf(stderr, "%s", _acq1430_save_usage);
      return(-1);
    }

    /* read base address & size */
    tmp = tscext_csr_rd(tsc_fd, csr_base + 0xC);

    offset   = (tmp & 0x3FF00000);
    size     = ((tmp & 0xFF) * 0x100000);
    if (size == 0) {
      size = 0x10000000;
    }

    /* read last pre-trigger address */
    tmp = tscext_csr_rd(tsc_fd, csr_base + 0x8);

    trig_pos = (tmp - offset);

    /* read control register */
    tmp = tscext_csr_rd(tsc_fd, csr_base + 0x0);

    /* calculate pre-trigger area */
    pre_size = (((tmp>>5) & 7) * (size/8));

    /* -1 = raw mode , 0 = legacy mode, 1 = new mode */
//    if ((tmp&(1<<15))!=0 && ((tmp>>4)&7)!=0)
    if ((tmp&(1<<15))!=0)
    {
      mode = 1;
    }
    else
    {
      mode = 0;
    }
  }

  acq_buf = NULL;

  if (size > 0)
  {
    bzero( &shm_mas_map_win, sizeof(shm_mas_map_win));
    shm_mas_map_win.req.mode.sg_id = MAP_ID_MAS_PCIE_PMEM;
    shm_mas_map_win.req.mode.space = space;
    shm_mas_map_win.req.rem_addr   = offset;
    shm_mas_map_win.req.size       = size;
    tsc_map_alloc(tsc_fd,&shm_mas_map_win);
    acq_buf = (char *)tsc_pci_mmap(tsc_fd, shm_mas_map_win.sts.loc_base, shm_mas_map_win.sts.size);
  }

  if (!acq_buf || ( size <= 0))
  {
    printf("cannot allocate acquisition buffer\n");
    return( -1);
  }

  /* raw mode -> offset and size need to be specified */
  if (mode == -1)
  {
    rc = acq1430_save_raw(acq_name, acq_buf, size);
  }

  /* legacy mode -> automatically */
  if (mode == 0)
  {
    rc = acq1430_save_legacy(acq_name, acq_buf, size, trig_pos, pre_size);
  }


  if (mode == 1)
  {
    rc = acq1430_save_new(acq_name, acq_buf, size, trig_pos, pre_size);
  }

  tsc_pci_munmap( acq_buf, shm_mas_map_win.sts.size);
  tsc_map_free(tsc_fd, &shm_mas_map_win);

  return(rc);
}

/*--------------------------------------------------------------------------------------------------------------------*/

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * Function name : acq1430_read
 * Prototype     : int
 * Parameters    : cli command parameter structure
 * Return        : success/error
 *----------------------------------------------------------------------------
 * Description   : acq1430 read command
 *
 *++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

static int acq1430_read(struct cli_cmd_para *c, struct tsc_acq1430_devices *add)
{
  int reg, data, ret = -1, min, max, i, tmp;
  const char *str;

  const char *__usage = "usage: acq1430.1 %s read <reg>\n";

  if (c->cnt < 3)
  {
    fprintf(stderr, "acq1430 read command needs more arguments\n");
    fprintf(stderr, __usage, add->name);
    return(-1);
  }

  /* lmk read status command */
  if (!strcmp(c->para[2],"status"))
  {
    if (add->idx == -1 && add->bus == BUS_SBC)
    {
      ret = daq1430_lmk_get_status(tsc_fd, 1, &data);
      if (ret < 0)
      {
        return(ret);
      }
      printf("LMK_ST_LD     = %s\n"   \
             "LMK_ST_CLKIN0 = %s\n"   \
             "LMK_ST_CLKIN1 = %s\n", ((data & DAQ1430_LMK_STATUS_LD)      ? "high" : "low"),
                                     ((data & DAQ1430_LMK_STATUS_CLKIN0)  ? "high" : "low"),
                                     ((data & DAQ1430_LMK_STATUS_CLKIN1)  ? "high" : "low"));
      return(0);
    }
    else if (add->idx == 7 && add->bus == BUS_SBC)
    {
      int status_lane[8], status_main, l, b;
      int status_lane_bit[12] = { 0, 1, 2, 3, 8, 9, 10, 11, 12, 13, 14, 15 };
      char status_lane_str[12][9];
      ret = dac38j84_read_status(tsc_fd, 1, &status_main, (int *)&status_lane);
      if (ret < 0)
      {
        return(ret);
      }
      printf("DAC38J84 status:\n");

      printf("  alarm_sysref_lane3 = %s\n"    \
             "  alarm_sysref_lane2 = %s\n"    \
             "  alarm_sysref_lane1 = %s\n"    \
             "  alarm_sysref_lane0 = %s\n"    \
             "  alarm_pap_d        = %s\n"    \
             "  alarm_pap_c        = %s\n"    \
             "  alarm_pap_b        = %s\n"    \
             "  alarm_pap_a        = %s\n"    \
             "  alarm_rw0_pll      = %s\n"    \
             "  alarm_rw1_pll      = %s\n"    \
             "  alarm_dac_pll      = %s\n", ((status_main & (1<<15)) ? "ERROR" : "OK"),
                                            ((status_main & (1<<14)) ? "ERROR" : "OK"),
                                            ((status_main & (1<<13)) ? "ERROR" : "OK"),
                                            ((status_main & (1<<12)) ? "ERROR" : "OK"),
                                            ((status_main & (1<<11)) ? "ERROR" : "OK"),
                                            ((status_main & (1<<10)) ? "ERROR" : "OK"),
                                            ((status_main & (1<< 9)) ? "ERROR" : "OK"),
                                            ((status_main & (1<< 8)) ? "ERROR" : "OK"),
                                            ((status_main & (1<< 3)) ? "UNLOCKED" : "LOCKED"),
                                            ((status_main & (1<< 2)) ? "UNLOCKED" : "LOCKED"),
                                            ((status_main & (1<< 0)) ? "UNLOCKED" : "LOCKED"));

      memset(status_lane_str, 0, sizeof(status_lane_str));
      for (b=0; b<12; b++)
      {
        for (l=0; l<8; l++)
        {
          status_lane_str[b][l] = ((status_lane[l] & (1<<status_lane_bit[b])) ? 'E' : '-');
        }
      }
      printf("Lane status:                       76543210\n" \
             "  multiframe alignement error    = %s\n"    \
             "  frame alignement error         = %s\n"    \
             "  link configuration error       = %s\n"    \
             "  elastic buffer overflow        = %s\n"    \
             "  elastic buffer match error     = %s\n"    \
             "  code synchronization error     = %s\n"    \
             "  8b/10b not-in-table code error = %s\n"    \
             "  8b/10b disparity error         = %s\n"    \
             "  write_error                    = %s\n"    \
             "  write_full                     = %s\n"    \
             "  read_error                     = %s\n"    \
             "  read_empty                     = %s\n", status_lane_str[11],
                                                        status_lane_str[10],
                                                        status_lane_str[9],
                                                        status_lane_str[8],
                                                        status_lane_str[7],
                                                        status_lane_str[6],
                                                        status_lane_str[5],
                                                        status_lane_str[4],
                                                        status_lane_str[3],
                                                        status_lane_str[2],
                                                        status_lane_str[1],
                                                        status_lane_str[0]);

      return(0);
    }
  }

  if (add->bus == BUS_INT && add->idx == 1)
  {
    if (!strcmp(c->para[2], "all"))
    {
      min = 1;
      max = 62;
    }
    else
    {
      if (sscanf(c->para[2],"%d",&min) != 1 || min < 1 || min > 63)
      {
        fprintf(stderr, "wrong index expected values are all or a number between 1 to 62\n");
        fprintf(stderr, __usage, add->name);
        return(-1);
      }
      max =  min;
    }
    for (i=min; i<=max; i++)
    {
      ret = daq1430_read_atest(tsc_fd, 1, i, &data, &tmp, &str);
      printf("%-90s (%2d) : %12.05f V (0x%08X)\n", str, i, ((float)data/10000.0), tmp);
    }
    return (ret);
  }

  if( sscanf( c->para[2],"%x", &reg) != 1)
  {
    fprintf(stderr, "wrong register number\n");
    fprintf(stderr, __usage, add->name);
    return(-1);
  }

  if (add->bus == BUS_SBC && (add->idx <= 4 || add->idx == 7 || (add->idx >= 20 && add->idx <= 27)))
  {
    ret = adc_spi_read(tsc_fd, 1, add->cmd, reg, &data);
  }
  else if (add->bus == BUS_I2C)
  {
    ret = fmc_i2c_read(tsc_fd, 1, add->cmd, reg, (uint *)&data);
  }
  /* jesd */
  else if (add->bus == BUS_INT && add->idx == 2)
  {
    ret = dac38j84_jesd_read(tsc_fd, 1, reg, &data);
  }
  /* drp */
  else if  (add->bus == BUS_DRP)
  {
    ret = dac38j84_drp_read(tsc_fd, 1, add->idx, reg, &data);
  }
  else
  {
    fprintf(stderr, "acq1430 read command not supported by this device\n");
    return(-1);
  }

  if (ret < 0)
  {
    fprintf(stderr, "%s: error occurred (%d)\n", add->name, ret);
  }
  else
  {
    printf("cmd = %08x - data = %08x\n", add->cmd, data);
  }

  return(ret);
}

/*--------------------------------------------------------------------------------------------------------------------*/

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * Function name : acq1430_write
 * Prototype     : int
 * Parameters    : cli command parameter structure
 * Return        : success/error
 *----------------------------------------------------------------------------
 * Description   : acq1430 write command
 *
 *++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

static int acq1430_write(struct cli_cmd_para *c, struct tsc_acq1430_devices *add)
{
  int reg, data, ret = -1, i;

  const char *__usage = "usage: acq1430.1 %s write <reg> <data>\n";

  if( c->cnt < 4)
  {
    fprintf(stderr, "acq1430 write command needs more arguments\n");
    fprintf(stderr, __usage, add->name);
    return(-1);
  }
  if( sscanf( c->para[2],"%x", &reg) != 1)
  {
    printf("wrong register number\n");
    fprintf(stderr, __usage, add->name);
    return(-1);
  }
  if( sscanf( c->para[3],"%x", &data) != 1)
  {
    printf("wrong data value\n");
    fprintf(stderr, __usage, add->name);
    return(-1);
  }
/*  printf("%s.%s %s %s %s %s \n", c->cmd, c->ext, c->para[0], c->para[1], c->para[2], c->para[3]);*/

  if (add->bus == BUS_SBC && (add->idx == 0x3FF || add->idx == 15 || add->idx <= 4 || add->idx == 7 || (add->idx >= 20 && add->idx <= 27)))
  {
    printf("cmd = %08x - data = %08x\n", add->cmd, data);

    /* write into all ADSs */
    if (add->idx == 0x3FF || add->idx == 15)
    {
      for (i=0; i<5; i++)
      {
        ret = adc_spi_write(tsc_fd, 1, (add->cmd | (i<<16)), reg, data);
        if (ret < 0)
        {
          break;
        }
      }
    }
    else
    {
      ret = adc_spi_write(tsc_fd, 1, add->cmd, reg, data);
    }
  }
  else if (add->bus == BUS_I2C)
  {
    printf("cmd = %08x - data = %08x\n", add->cmd, data);
    ret = fmc_i2c_write(tsc_fd, 1, add->cmd, reg, (uint)data);
  }
  /* jesd */
  else if (add->bus == BUS_INT && add->idx == 2)
  {
    ret = dac38j84_jesd_write(tsc_fd, 1, reg, data);
  }
  /* drp */
  else if  (add->bus == BUS_DRP)
  {
    ret = dac38j84_drp_write(tsc_fd, 1, add->idx, reg, data);
  }
  else
  {
    fprintf(stderr, "acq1430 write command not supported by this device\n");
    return(-1);
  }

  if (ret < 0)
  {
    fprintf(stderr, "%s: error occurred (%d)\n", add->name, ret);
  }

  return(ret);
}

/*--------------------------------------------------------------------------------------------------------------------*/

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * Function name : acq1430_dump
 * Prototype     : int
 * Parameters    : cli command parameter structure
 * Return        : success/error
 *----------------------------------------------------------------------------
 * Description   : acq1430 dump command
 *
 *++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

static int acq1430_dump(struct cli_cmd_para *c, struct tsc_acq1430_devices *add)
{
  int device, off, size, rc = -1;
  //unsigned char *p;

  const char *__eeprom_usage = "usage: acq1430.1 eeprom dump <offset> <size>\n";

  device = add->cmd;
  /* I2C EEPROM */
  if ((device & 0xF8) == 0x50 && add->bus == BUS_I2C)
  {
    if (c->cnt < 4)
    {
      fprintf(stderr, "acq1430 eeprom dump command needs more arguments\n");
      fprintf(stderr, "%s", __eeprom_usage);
      return(-1);
    }
    if (sscanf(c->para[2],"%x", &off) != 1)
    {
      fprintf(stderr, "bad offset\n");
      fprintf(stderr, "%s", __eeprom_usage);
      return(-1);
    }
    if (sscanf(c->para[3],"%x", &size) != 1)
    {
      fprintf(stderr, "bad size\n");
      fprintf(stderr, "%s", __eeprom_usage);
      return(-1);
    }

    rc= fmc_i2c_eeprom_dump(tsc_fd, 1, device, off, size);
  }
  /* lmk */
  else if (add->bus == BUS_SBC && add->idx == -1)
  {
    rc = daq1430_lmk_dump(tsc_fd, 1);
  }
  else if (add->bus == BUS_SBC && ((add->idx == 0x3FF) || (add->idx >= 0 && add->idx <= 4)))
  {
    if (add->idx >= 0 && add->idx <= 4)
    {
      rc = daq1430_ads42lb69_dump(tsc_fd, 1, (3<<(2*add->idx)));
    }
    else
    {
      rc = daq1430_ads42lb69_dump(tsc_fd, 1, add->idx);
    }
  }
  else if (add->bus == BUS_SBC && add->idx == 7)
  {
    rc = dac38j84_spi_dump(tsc_fd, 1);
  }
  else if (add->bus == BUS_INT && add->idx == 2)
  {
    rc = dac38j84_jesd_dump(tsc_fd, 1);
  }
  else
  {
    fprintf(stderr, "'dump' command not supported for that device\n");
  }

  return(rc);
}

/*--------------------------------------------------------------------------------------------------------------------*/

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * Function name : acq1430_lmk_init
 * Prototype     : int
 * Parameters    : cli command parameter structure
 * Return        : success/error
 *----------------------------------------------------------------------------
 * Description   : acq1430 configure lmk clock generator
 *
 *++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/* 
 * acq1430.1 lmk init
 * acq1430.1 lmk init [intref]                       [d:<divider>]
 * acq1430.1 lmk init [clkdist] [r:<ref>]            [d:<divider>]
 * acq1430.1 lmk init [dualpll] [r:<ref>] [f:<freq>] [d:<divider>]
 */
static int acq1430_lmk_init(struct cli_cmd_para *c, struct tsc_acq1430_devices *add)
{
  int arg, tmp, lmk_mode, lmk_ref, lmk_freq, lmk_divider, rc = -1;
  char *p;

  const char *__usage = "usage: acq1430.1 lmk init [<mode> [(r:<ref> | f:<freq> | d:<divider>)]]\n";

  char *__lmk_modes[] = { "default", "clkdist", "intref", "dualpll", NULL };
  char *__lmk_refs[]  = { "fmc", "rtm", NULL };
  char *__lmk_freqs[] = { "default", "250m", "100m", "10m", NULL };

  /* lmk */
  if (add->bus == BUS_SBC && add->idx == -1)
  {
    lmk_mode    = DAQ1430_LMK_MODE_INTREF;
    lmk_ref     = DAQ1430_LMK_REF_RTM;
    lmk_freq    = DAQ1430_LMK_FREQ_250M;
    lmk_divider = -1;
    
    if (c->cnt >= 3)
    {
      tmp = parse_str_array(c->para[2], __lmk_modes);
      if (tmp == -1)
      {
        fprintf(stderr, "wrong mode '%s' expected 'default', 'clkdist', 'intref' or 'dualpll'\n", c->para[2]);
        fprintf(stderr, "%s", __usage);
        return(-1);
      }
      lmk_mode = ((tmp == 0) ? 0 : (tmp-1));
    }

    if (c->cnt >= 4)
    {
      for (arg = 3; arg < c->cnt; arg++)
      {
        p = c->para[arg];
        
        if (p[0] != '\0' && p[1] == ':')
        {
          switch(p[0])
          {
            case 'd':
              p+= 2;
              if (sscanf(p, "%d", &tmp) != 1)
              {
                fprintf(stderr, "wrong divider '%s' expected a value in range 1 to 1045\n", p);
                fprintf(stderr, "%s", __usage);
                return(-1);
              }
              lmk_divider = tmp;
              break;
       
            case 'r':
              p+= 2;
              tmp = parse_str_array(p, __lmk_refs);
              if (tmp == -1)
              {
                fprintf(stderr, "wrong ref '%s' expected 'fmc' or 'rtm'\n", p);
                fprintf(stderr, "%s", __usage);
                return(-1);
              }
              lmk_ref = tmp;
              if (lmk_mode == DAQ1430_LMK_MODE_INTREF)
              {
                fprintf(stderr, "ref parameter does not make sense in internal reference mode !\n");
              }
              break;

            case 'f':
              p+= 2;
              tmp = parse_str_array(p, __lmk_freqs);
              if (tmp == -1)
              {
                fprintf(stderr, "wrong freq '%s' expected 'default', '250m', '100m' or '10m'\n", p);
                fprintf(stderr, "%s", __usage);
                return(-1);
              }
              lmk_freq = ((tmp==0) ? 0 : (tmp-1));

              if (lmk_mode != DAQ1430_LMK_MODE_DUALPLL)
              {
                fprintf(stderr, "freq parameter is only used in dual pll mode !\n");
              }
              break;

            default:
              fprintf(stderr, "unknown argument '%s'\n", p);
              fprintf(stderr, "%s", __usage);
              return(-1);
          }
        }
      }
    }

    rc = daq1430_lmk_init(tsc_fd, 1, lmk_mode, lmk_ref, lmk_freq);
    if (rc >= 0 && lmk_divider != -1)
    {
      rc = daq1430_lmk_set_adc_divider(tsc_fd, 1, lmk_divider);
    }
  }
  else
  {
    fprintf(stderr, "'init' command not supported for that device\n");
  }
  return(rc);
}

/*--------------------------------------------------------------------------------------------------------------------*/

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * Function name : acq1430_dac_init
 * Prototype     : int
 * Parameters    : cli command parameter structure
 * Return        : success/error
 *----------------------------------------------------------------------------
 * Description   : acq1430 configure DAC
 *
 *++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

/*
 * acq1430.1 dac init [<freq_out> [<freq_in> [<linerate>]]]
 *
 * default: freq_in = 250 MHz, freq_out = 1000.0 MHz, linerate = 10.0 Gbit/s
 */
static int acq1430_dac_init(struct cli_cmd_para *c, struct tsc_acq1430_devices *add)
{
  double dac_freq_in, dac_freq_out, dac_linerate;
  int rc = -1;

  const char *__usage = "usage: acq1430.1 dac init [<freq_out> [<freq_in> [<linerate>]]]\n";

  /* dac */
  if (add->bus == BUS_SBC && add->idx == 7)
  {
    dac_freq_in = 250.0;    /* 250.0 MHz    */
    dac_freq_out = 1000.0;  /*   1.0 GHz    */
    dac_linerate = 10.0;    /*  10.0 Gbit/s */

    if (c->cnt >= 3)
    {
      if (sscanf(c->para[2], "%lf", &dac_freq_out) != 1)
      {
        fprintf(stderr, "output frequency expected in MHz !\n");
        fprintf(stderr, "%s", __usage);
        return(-1);
      }
    }

    if (c->cnt >= 4)
    {
      if (sscanf(c->para[3], "%lf", &dac_freq_in) != 1)
      {
        fprintf(stderr, "input frequency expected in MHz !\n");
        fprintf(stderr, "%s", __usage);
        return(-1);
      }
      /* ask to calculate the linerate */
      dac_linerate = 0.0;
    }

    if (c->cnt >= 5)
    {
      if (sscanf(c->para[4], "%lf", &dac_linerate) != 1)
      {
        fprintf(stderr, "input linerate expected in Gbit/S !\n");
        fprintf(stderr, "%s", __usage);
        return(-1);
      }
    }
    rc = daq1430_dac_init(tsc_fd, 1, dac_freq_in, dac_freq_out, dac_linerate, 1, 4, 4, 1, DAC38J84_SYSREF_PERIODIC);
    if (rc < 0)
    {
      fprintf(stderr, "an error has during DAC initialization ! (%d)\n", rc);
    }
  }
  else
  {
    fprintf(stderr, "'init' command not supported for that device\n");
  }
  return(rc);
}

/*--------------------------------------------------------------------------------------------------------------------*/

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * Function name : acq1430_show
 * Prototype     : int
 * Parameters    : cli command parameter structure
 * Return        : success/error
 *----------------------------------------------------------------------------
 * Description   : acq1430 show command
 *
 *++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

static int acq1430_show(int fmc, struct cli_cmd_para *c, struct tsc_acq1430_devices *add)
{
  int device, ret;
  uint temp, lo, hi;

  device = add->cmd;

  /* only for I2C thermometer */
  if ((device&0xfc) != 0x48)
  {
    fprintf(stderr, "show command not supported for this device\n");
    return(-1);
  }

  ret = adc_read_tmp102(tsc_fd, fmc, device, &temp, &lo, &hi);

  if (ret < 0)
  {
    fprintf(stderr, "%s: an error has occurred (%d)\n", add->name, ret);
  }
  else
  {
    printf("current temperature: %.2f [%.2f - %.2f]\n", (float)temp/16.0, (float)lo/16.0, (float)hi/16.0);
  }
  return(ret);
}

/*--------------------------------------------------------------------------------------------------------------------*/

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * Function name : acq1430_set
 * Prototype     : int
 * Parameters    : cli command parameter structure
 * Return        : success/error
 *----------------------------------------------------------------------------
 * Description   : acq1430 set command
 *
 *++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

static int acq1430_set(int fmc, struct cli_cmd_para *c, struct tsc_acq1430_devices *add)
{
  int device, ret;
  uint temp, lo, hi;
  float flo, fhi;

  const char *__usage = "usage: acq1430.1 %s set <lo> <hi>\n";

  if (add->bus == BUS_INT)
  {
    return(acq1430_wgen_set(c));
  }

  device = add->cmd;

  /* only for I2C thermometer */
  if ((device & 0xfc) != 0x48)
  {
    fprintf(stderr, "set command not supported for that device\n");
    return(-1);
  }

  if( c->cnt < 4)
  {
    fprintf(stderr, "acq1430 set command needs more arguments\n");
    fprintf(stderr, __usage, add->name);
    return(-1);
  }
  if( sscanf( c->para[2],"%f", &flo) != 1)
  {
    fprintf(stderr, "wrong lo value\n");
    fprintf(stderr, __usage, add->name);
    return(-1);
  }
  if( sscanf( c->para[3],"%f", &fhi) != 1)
  {
    fprintf(stderr, "wrong hi value\n");
    fprintf(stderr, __usage, add->name);
    return(-1);
  }
  lo = (uint)(flo*16.0);
  hi = (uint)(fhi*16.0);

  ret = adc_set_tmp102(tsc_fd, fmc, device, &lo, &hi);

  if (ret >= 0)
  {
    ret = adc_read_tmp102(tsc_fd, fmc, device, &temp, &lo, &hi);
  }

  if (ret < 0)
  {
    fprintf(stderr, "%s: an error has occurred (%d)\n", add->name, ret);
  }
  else
  {
    printf("current temperature: %.2f [%.2f - %.2f]\n", (float)temp/16.0, (float)lo/16.0, (float)hi/16.0);
  }
  return(ret);
}

/*--------------------------------------------------------------------------------------------------------------------*/

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * Function name : acq1430_sign
 * Prototype     : int
 * Parameters    : cli command parameter structure
 * Return        : success/error
 *----------------------------------------------------------------------------
 * Description   : acq1430 sign command
 *
 *++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

/*
 * acq1430.1 eeprom sign
 *    display signature
 * acq1430.1 eeprom sign set
 *    set signature interactively
 * acq1430.1 eeprom sign def b:<board_name> s:<serial> v:<version> r:<revision>
 *    set default signature
 *
 */

static int acq1430_sign_cmd(int fmc, struct cli_cmd_para *c, struct tsc_acq1430_devices *add)
{
  int device, i, op, cnt, rc;
  char buf[64], *para_p, *q;
  unsigned char *p;

  device = add->cmd;

  if((device & 0xF8) != 0x50)
  {
    fprintf(stderr, "'sign' command not supported for that device\n");
    return(-1);
  }

  rc = fmc_sign_read(tsc_fd, fmc, (uint)device, (fmc_sign_t *)&acq1430_sign);
  if (rc < -1)
  {
    fprintf(stderr, "error reading signature from EEPROM !");
    return (-1);
  }
//  p = (unsigned char *)&acq1430_sign;

  op = 0;
  if (c->cnt > 2)
  {
    if (!strcmp( "set", c->para[2]))
      op = 1;

    if (!strncmp( "default", c->para[2], 3))
      op = 2;
  }

  if (op == 1)
  {
    //char prompt[64];

    memset(&acq1430_history, 0, sizeof( struct cli_cmd_history));
    cli_history_init( &acq1430_history);

    printf("setting DAQ1430 signature\n");
    para_p = cli_get_cmd( &acq1430_history, "Enter password ->  ");
    if (strcmp(  para_p, "goldorak"))
    {
      printf("wrong password\n");
      return(-1);
    }

    rc = parse_field("Board Name",        acq1430_sign.board_name,  8,  &acq1430_history);
    if (rc < 0) return(-1);

    rc = parse_field("Serial Number",     acq1430_sign.serial,      4,      &acq1430_history);
    if (rc < 0) return(-1);

    rc = parse_field("PCB Version",       acq1430_sign.version,     8,     &acq1430_history);
    if (rc < 0) return(-1);

    rc = parse_field("Hardware Revision", acq1430_sign.revision,    2,    &acq1430_history);
    if (rc < 0) return(-1);

    rc = parse_field("Test Date",         acq1430_sign.test_date,   8,  &acq1430_history);
    if (rc < 0) return(-1);

    rc = parse_field("Calibration Date",  acq1430_sign.calib_date,  8, &acq1430_history);
    if (rc < 0) return(-1);

    for (i=0; i<10; i++)
    {
      snprintf(buf, sizeof(buf), "Offset Compensation Chan%d", i);
      rc = parse_field(buf, acq1430_sign.offset_adc[i], 8, &acq1430_history);
      if (rc < 0) return(-1);
    }
  }
  if (op == 2)
  {
    fmc_sign_set((fmc_sign_t *)&acq1430_sign, "DAQ1430", "0000", "000000001", "A0", 1, 1);
    memcpy(acq1430_sign.calib_date, acq1430_sign.test_date, 8);
    for (i=0; i<10; i++)
    {
      memcpy(acq1430_sign.offset_adc[i], "00000000", 8);
    }

    cnt = (c->cnt - 3);
    i = 3;
    while(cnt--)
    {
      q = c->para[i++];
      if  (q[0] == 'b')
      {
        strncpy(acq1430_sign.board_name,  &q[2], 8);
      }
      if (q[0] == 's')
      {
        strncpy(acq1430_sign.serial,      &q[2], 4);
      }
      if (q[0] == 'v')
      {
        strncpy(acq1430_sign.version,     &q[2], 8);
      }
      if (q[0] == 'r')
      {
        strncpy(acq1430_sign.revision,    &q[2], 2);
      }
    }
  }

  printf("DAQ1430 signature\n");
  fmc_sign_print(NULL, (fmc_sign_t *)&acq1430_sign);

  p = (unsigned char *)acq1430_sign.calib_date;
  printf("%-30s : ", "Calibration Date");
  printf("%c%c%c%c%c%c%c%c\n", p[0],p[1],p[2],p[3],p[4],p[5],p[6],p[7]);
  for (i=0; i<10; i++)
  {
    snprintf(buf, sizeof(buf), "Offset Compensation Chan#%d", i);
    printf("%-30s : ", buf);
    p = (unsigned char *)acq1430_sign.offset_adc[i];
    printf("%c%c%c%c%c%c%c%c\n", p[0],p[1],p[2],p[3],p[4],p[5],p[6],p[7]);
  }
  printf("\n");

  if (op)
  {
    if (op == 1)
    {
      para_p = cli_get_cmd( &acq1430_history, "Overwrite DAQ1430 signature ? [y/n] ");
      if( para_p[0] != 'y')
      {
        printf("EEPROM signature update aborted\n");
        return(-1);
      }
    }
    rc = fmc_sign_write(tsc_fd, 1, (uint)device, (fmc_sign_t *)&acq1430_sign);
    if (rc < 0)
    {
      fprintf(stderr, "error writing signature into EEPROM !");
      return(-1);
    }
    printf("EEPROM signature update done\n");
  }
  return(0);
}

/*--------------------------------------------------------------------------------------------------------------------*/

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * Function name : tsc_acq1430
 * Prototype     : int
 * Parameters    : cli command parameter structure
 * Return        : success/error
 *----------------------------------------------------------------------------
 * Description   : acq1430 command
 *
 *++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

/*
 * Implemented commands:
 * =====================
 *
 * acq1430.1 lmk read  <reg>
 * acq1430.1 lmk read  status
 * acq1430.1 lmk write <reg> <data>
 * acq1430.1 lmk dump
 * acq1430.1 lmk init [<mode> [(r:<ref> | f:<freq> | d:<divider>)]]
 *    <mode> = default, clkdist, intref, dualpll
 *    <ref>  = fmc, rtm
 *    <freq> = default, 250m, 100m, 10m
 *
 * acq1430.1 (ads?) read  <reg>
 * acq1430.1 (ads?) write <reg> <data>
 * acq1430.1 (ads?) write <reg> <data>
 * acq1430.1 (ads?) check
 * acq1430.1 (ads?) acq<size> h:<file_his> d:<file_dat> t:<trig> s:<smem> c:<csr> a:<last_addr>
 * acq1430.1 (ads?) calib
 * acq1430.1 (ads?) init
 * acq1430.1 (ads?) dump
 *
 * acq1430.1 <file> save <offset> <size>
 *
 * acq1430.1 dac    read  <reg>
 * acq1430.1 dac    write <reg> <data>
 *
 * acq1430.1 wgen   set <default>
 * acq1430.1 wgen   start
 * acq1430.1 wgen   stop
 * acq1430.1 wgen   load <file>
 *
 * acq1430.1 eeprom read   <offset>
 * acq1430.1 eeprom write  <offset> <data>
 * acq1430.1 eeprom sign
 * acq1430.1 eeprom sign set
 * acq1430.1 eeprom sign def b:<board> s:<serial> v:<ver> r:<rev>
 * acq1430.1 eeprom dump <offset> <size>
 *
 * acq1430.1 tmp102 show
 * acq1430.1 tmp102 set <lo> <hi>
 *
 *
 * acq1430.<fmc> <dev> read <reg>
 * acq1430.<fmc> <dev> write <reg> <val>
 * acq1430.<fmc> <dev> acqfif <offset> <size>
 * acq1430.<fmc> <dev> check <offet> <size>
 * acq1430.<fmc> <dev> acq
 * acq1430.<fmc> <dev> acq64K
 * acq1430.<fmc> <dev> acq128K
 * acq1430.<fmc> <dev> acq256K
 * acq1430.<fmc> <dev> acq512K
 * acq1430.<fmc> <dev> acq1M
 * acq1430.<fmc> <dev> acq4M
 * acq1430.<fmc> <dev> show
 * acq1430.<fmc> <dev> load
 * acq1430.<fmc> <dev> start
 * acq1430.<fmc> <dev> stop
 * acq1430.<fmc> <dev> set
 * acq1430.<fmc> <dev> sign
 * acq1430.<fmc> <dev> dump
 * acq1430.<fmc> <dev> calidel
 * acq1430.<fmc> <dev> acq
 * acq1430.<fmc> <dev> acq
 *
 *
 */

int tsc_acq1430(struct cli_cmd_para *c)
{
  struct tsc_acq1430_devices *add;
  uint cmd, data, fmc;
  char *p;
  int adc_ch_mask, rc;
  int fmt = 0;

  if(!c->ext)
  {
    fmc = 1;
  }
  else
  {
    fmc = strtoul( c->ext, &p, 16);
    if (fmc != 1)
    {
      fprintf(stderr, "bad FMC index : %d\n", fmc);
      return(-1);
    }
  }

  acq1430_init();

  if( c->cnt < 2)
  {
    printf("acq1430 command needs more arguments\n");
    printf("usage: acq1430.<fmc> <dev> <op> ...\n");
    printf("acq1430 device list:\n");
    add = &acq1430_devices[0];
    while( add->name)
    {
      printf("   - %s\n", add->name);
      add++;
    }
    return(-1);
  }

  /* acq1430.1 <file> save command */
  if (!strncmp( "save", c->para[1], 4))
  {
    return acq1430_save(c);
  }

  add = &acq1430_devices[0];
  while(add->name != NULL)
  {
    if(!strcmp(c->para[0], add->name))
      break;
    add++;
  }

  if (!add->name)
  {
    fprintf(stderr, "wrong device name\n");
    fprintf(stderr, "usage: acq1430.1 <dev> <op> ...\n");
    fprintf(stderr, "acq1430 device list:\n");
    add = &acq1430_devices[0];
    while( add->name)
    {
      fprintf(stderr, "   - %s\n", add->name);
      add++;
    }
    return(-1);
  }

  /* acq1430.1 <dev> read command */
  if(!strcmp("read", c->para[1]))
  {
    return acq1430_read(c, add);
  }
  /* acq1430.1 <dev> write command */
  else if( !strcmp( "write", c->para[1]))
  {
    return acq1430_write(c, add);
  }
  else if( !strcmp( "acqfif", c->para[1]) ||
           !strcmp( "check",  c->para[1])    )
  {
    int offset, size, tmo;

    if( (add->idx < 0) || (add->idx > 4) || (add->bus != BUS_SBC))
    {
      printf("wrong device name\n");
      printf("usage: acq1430.<fmc> ads<ij> %s <offset> [<size>]\n", c->para[1]);
      return(-1);
    }
    if( c->cnt < 4)
    {
      printf("acq1430 %s command needs more arguments\n", c->para[1]);
      printf("usage: acq1430.<fmc> <dev> %s <offset> <size>\n", c->para[1]);
      return(-1);
    }
    if( sscanf( c->para[2],"%x", &offset) != 1)
    {
      printf("wrong offset value\n");
      printf("usage: acq1430.<fmc> <dev> %s <offset> <size>\n", c->para[1]);
      return(-1);
    }
    if( sscanf( c->para[3],"%x", &size) != 1)
    {
      printf("wrong size value\n");
      printf("usage: acq1430.<fmc> <dev> %s <offset> <size>\n", c->para[1]);
      return(-1);
    }
    printf("start data acquision on device  %s [%d] at offset %x [%x]\n",  c->para[0], add->idx, offset, size);
    if( fmc == 2)
    {
      int cmd_sav;

      cmd_sav = tscext_csr_rd(tsc_fd,  0x1188);
      cmd_sav &= ~( 0xff << (8*add->idx));

      cmd = cmd_sav | (1 << (8*add->idx));
      tscext_csr_wr(tsc_fd, 0x1188, cmd);
      tscext_csr_rd(tsc_fd,  0x1188);

      cmd = cmd_sav;
      tscext_csr_wr(tsc_fd, 0x1188, cmd);
      tscext_csr_rd(tsc_fd,  0x1188);

      tscext_csr_wr(tsc_fd, ADC_BASE_BMOV_B, offset);
      //cmd = 0x80000000 | ( add->idx << 26) | (size & 0x3fffe00) | 0x5;
      //cmd = 0x90000000 | ( add->idx << 26) | (size & 0x3fffe00) | 0x5;
      //cmd = 0xa0000000 | ( add->idx << 26) | (size & 0x3fffe00) | 0x5;
      cmd = 0xb0000000 | ( add->idx << 26) | (size & 0x3fffe00) | 0x5;
      tscext_csr_wr(tsc_fd, ADC_BASE_BMOV_B + 4, cmd);
      tscext_csr_rd(tsc_fd,  ADC_BASE_BMOV_B + 4);

      cmd = cmd_sav | (2 << (8*add->idx));
      tscext_csr_wr(tsc_fd, 0x1188, cmd);
      tscext_csr_rd(tsc_fd,  0x1188);

      if( c->para[1][0] == 'c') /* if check operation */
      {
        /* select test pattern generation mode */
        tscext_csr_wr(tsc_fd, ADC_BASE_SERIAL_B + 4, 0x44);
        tscext_csr_wr(tsc_fd, ADC_BASE_SERIAL_B,  0xc000000f | add->cmd);
      }
      tmo = 100;
      while( --tmo)
      {
        usleep(2000);
        if( tscext_csr_rd(tsc_fd,  ADC_BASE_BMOV_B + 4) & 0x80000000) break;
      }
      printf("acquisition status : %08x - %08x\n", tscext_csr_rd(tsc_fd,  ADC_BASE_BMOV_B),  tscext_csr_rd(tsc_fd, ADC_BASE_BMOV_B + 4));
    }
    else
    {
      int cmd_sav;

      cmd_sav = tscext_csr_rd(tsc_fd,  0x1184);
      cmd_sav &= ~( 0xff << (8*add->idx));

      cmd = cmd_sav | (1 << (8*add->idx));
      tscext_csr_wr(tsc_fd, 0x1184, cmd);
      tscext_csr_rd(tsc_fd,  0x1184);

      cmd = cmd_sav;;
      tscext_csr_wr(tsc_fd, 0x1184, cmd);
      tscext_csr_rd(tsc_fd,  0x1184);

      tscext_csr_wr(tsc_fd, ADC_BASE_BMOV_A, offset);
      //cmd = 0x80000000 | ( add->idx << 26) | (size & 0x3fffe00) | 0x5;
      //cmd = 0x90000000 | ( add->idx << 26) | (size & 0x3fffe00) | 0x5;
      //cmd = 0xa0000000 | ( add->idx << 26) | (size & 0x3fffe00) | 0x5;
      cmd = 0xb0000000 | ( add->idx << 26) | (size & 0x3fffe00) | 0x5;
      tscext_csr_wr(tsc_fd, ADC_BASE_BMOV_A + 4, cmd);
      tscext_csr_rd(tsc_fd,  ADC_BASE_BMOV_A+4);

      cmd = cmd_sav | (2 << (8*add->idx));
      tscext_csr_wr(tsc_fd, 0x1184, cmd);
      tscext_csr_rd(tsc_fd,  0x1184);

      if( c->para[1][0] == 'c') /* if check operation */
      {
        /* select test pattern generation mode */
        tscext_csr_wr(tsc_fd, ADC_BASE_SERIAL_A + 4, 0x44);
        tscext_csr_wr(tsc_fd, ADC_BASE_SERIAL_A,  0xc000000f | add->cmd);
      }

      tmo = 100;
      while( --tmo)
      {
        usleep(2000);
        if( tscext_csr_rd(tsc_fd,  ADC_BASE_BMOV_A + 4) & 0x80000000) break;
      }
      printf("acquisition status : %08x - %08x\n", tscext_csr_rd(tsc_fd,  ADC_BASE_BMOV_A),  tscext_csr_rd(tsc_fd, ADC_BASE_BMOV_A + 4));
    }
    if( c->para[1][0] == 'c')
    {
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
        tsc_map_alloc(tsc_fd, &shm_mas_map_win);
        acq_buf = (char *)tsc_pci_mmap(tsc_fd, shm_mas_map_win.sts.loc_base, shm_mas_map_win.sts.size);
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
        tscext_map_alloc( &shm_mas_map);
        acq_buf = tscext_mmap( &shm_mas_map);
#endif
      }
      if( !acq_buf || ( size <= 0))
      {
        printf("cannot allocate acquisition buffer\n");
        return( -1);
      }
      for( i = 0; i < 0x10000; i++)
      {
        acq1430_histo[0][i] = 0;
        acq1430_histo[1][i] = 0;
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
        acq1430_histo[0][data & 0xffff] += 1;

        data = *(unsigned char *)&acq_buf[i+2] |  (*(unsigned char *)&acq_buf[i+3] << 8);
        if( cmp0 != data)
        {
          if( nerr0 < 16) printf("%x : %x - %x\n", i/4+1, data, cmp0);
          nerr0 += 1;
          cmp0 = data;
        }
        cmp0 = ( cmp0 + 1) & 0xffff;
        acq1430_histo[0][data & 0xffff] += 1;

        data = *(unsigned char *)&acq_buf[i+4] |  (*(unsigned char *)&acq_buf[i+5] << 8);
        if( cmp0 != data)
        {
          if( nerr0 < 16) printf("%x : %x - %x\n", i/4+2, data, cmp0);
          nerr0 += 1;
          cmp0 = data;
        }
        cmp0 = ( cmp0 + 1) & 0xffff;
        acq1430_histo[0][data & 0xffff] += 1;

        data = *(unsigned char *)&acq_buf[i+6] |  (*(unsigned char *)&acq_buf[i+7] << 8);
        if( cmp0 != data)
        {
          if( nerr0 < 16) printf("%x : %x - %x\n", i/4+3, data, cmp0);
          nerr0 += 1;
          cmp0 = data;
        }
        cmp0 = ( cmp0 + 1) & 0xffff;
        acq1430_histo[0][data & 0xffff] += 1;

        data = *(unsigned char *)&acq_buf[i+8] |  (*(unsigned char *)&acq_buf[i+9] << 8);
        if( cmp1 != data)
        {
          if( nerr1 < 16) printf("%x : %x - %x\n", i/4+0, data, cmp1);
          nerr1 += 1;
          cmp1 = data;
        }
        cmp1 = ( cmp1 + 1) & 0xffff;
        acq1430_histo[1][data & 0xffff] += 1;

        data = *(unsigned char *)&acq_buf[i+10] |  (*(unsigned char *)&acq_buf[i+11] << 8);
        if( cmp1 != data)
        {
          if( nerr1 < 16) printf("%x : %x - %x\n", i/4+1, data, cmp1);
          nerr1 += 1;
          cmp1 = data;
        }
        cmp1 = ( cmp1 + 1) & 0xffff;
        acq1430_histo[1][data & 0xffff] += 1;

        data = *(unsigned char *)&acq_buf[i+12] |  (*(unsigned char *)&acq_buf[i+13] << 8);
        if( cmp1 != data)
        {
          if( nerr1 < 16) printf("%x : %x - %x\n", i/4+2, data, cmp1);
          nerr1 += 1;
          cmp1 = data;
        }
        cmp1 = ( cmp1 + 1) & 0xffff;
        acq1430_histo[1][data & 0xffff] += 1;

        data = *(unsigned char *)&acq_buf[i+14] |  (*(unsigned char *)&acq_buf[i+15] << 8);
        if( cmp1 != data)
        {
          if( nerr1 < 16) printf("%x : %x - %x\n", i/4+3, data, cmp1);
          nerr1 += 1;
          cmp1 = data;
        }
        cmp1 = ( cmp1 + 1) & 0xffff;
        acq1430_histo[1][data & 0xffff] += 1;
      }
      if( nerr0 == 0)
      {
        printf("ACQ1430 test pattern FMC%d chan#%d -> OK\n", fmc, (2*add->idx));
      }
      else
      {
        printf("ACQ1430 test pattern FMC%d chan#%d nerr = %d -> NOK\n", fmc, (2*add->idx), nerr0);
      }
      if( nerr1 == 0)
      {
        printf("ACQ1430 test pattern FMC%d chan#%d -> OK\n", fmc, (2*add->idx) + 1);
      }
      else
      {
        printf("ACQ1430 test pattern FMC%d chan#%d nerr = %d -> NOK\n", fmc, (2*add->idx) + 1, nerr1);
      }
#ifdef TSC
      tsc_pci_munmap( acq_buf, shm_mas_map_win.sts.size);
      tsc_map_free(tsc_fd, &shm_mas_map_win);
#else
      tscext_munmap( &shm_mas_map);
      tscext_map_free( &shm_mas_map);
#endif
    }
    return(0);
  }
  else if( !strncmp( "acq", c->para[1], 3))
  {
    int size;
    int check;
    int nerr;

    if( (add->idx < 0) || (add->idx > 4) || (add->bus != BUS_SBC))
    {
      fprintf(stderr, "wrong device name\n"                     \
                      "usage: acq1430.<fmc> ads<ij> %s\n", c->para[1]);
      return(-1);
    }
    size = 0x10000;
    check = 0;
    if( !strcmp( "acq64K",  c->para[1])) size = 0x10000;
    if( !strcmp( "acq128K", c->para[1])) size = 0x20000;
    if( !strcmp( "acq256K", c->para[1])) size = 0x40000;
    if( !strcmp( "acq512K", c->para[1])) size = 0x80000;
    if( !strcmp( "acq1M",   c->para[1])) size = 0x100000;
    if( !strcmp( "acq4M",   c->para[1])) size = 0x400000;
    if( !strcmp( "acq64Kc", c->para[1]))
    {
      size = 0x10000;
      check = 5;
    }
    do
    {
      nerr = acq1430_acq(c, add->idx, size, check);
      if( !nerr) break;
    } while( check--);

    return(0);
  }
//  else if (!strcmp("calib", c->para[1]))
//  {
//    
//    
//    if (!strncmp("ads", c->para[0],3))
//    {
//      int idelay;
//
//      if (c->cnt < 3)
//      {
//        printf("Calling daq1430_set_idelay() with default for all channels\n");
//        daq1430_calib_get_idelay(fmc, -1, &idelay);
//        printf("idelay = %d 0x%04X\n", idelay, idelay);
//      }
//      return(0);
//    }
//  }
  else if (!strcmp("caldump", c->para[1]))
  {
    if (!strcmp("ads09", c->para[0]))
    {
      daq1430_calib_dump_idelay(tsc_fd, 1);
      return(0);
    }
    else
    {
      fprintf(stderr, "unsupported command\n");
    }
  }
  else if (!strcmp( "calidel", c->para[1]) || !strcmp("calib", c->para[1]))
  {
    if (add->bus == BUS_SBC && (add->idx == 15 || add->idx == 0x3FF))
    {
      rc = acq1430_calib_idelay(c, -1, fmc);
    }
    else if (add->bus == BUS_SBC && (add->idx >= 0 && add->idx <= 4))
    {
      rc = acq1430_calib_idelay(c, add->idx, fmc);
    }
    else
    {
      printf("wrong device name\n");
      printf("usage: acq1430.<fmc> ads<ij> calidel\n");
      return(-1);
    }
    return(rc);
  }
  else if (!strcmp("direct", c->para[1]))
  {
    if (add->bus == BUS_SBC && (add->idx == 15 || add->idx == 0x3FF))
    {
      rc = daq1430_direct_dump(tsc_fd, fmc, -1);
    }
    else if (add->bus == BUS_SBC && (add->idx >= 0 && add->idx <= 4))
    {
      rc = daq1430_direct_dump(tsc_fd, fmc, 2*add->idx+0);
      if (rc >= 0)
      {
        rc = daq1430_direct_dump(tsc_fd, fmc, 2*add->idx+1);
      }
    }
    else
    {
      fprintf(stderr, "wrong device name\n");
      fprintf(stderr, "usage: acq1430.<fmc> ads<ij> direct\n");
      rc = -1;
    }
    return(rc);
  }
  else if (!strcmp( "show", c->para[1]))
  {
    return acq1430_show(fmc, c, add);
  }
  else if (!strcmp( "load", c->para[1]) && add->bus == BUS_INT)
  {
    return(acq1430_wgen_load(c));
  }
  else if (!strcmp( "start", c->para[1]) && add->bus == BUS_INT)
  {
    return(acq1430_wgen_start(c));
  }
  else if (!strcmp( "stop", c->para[1]) && add->bus == BUS_INT)
  {
    return(acq1430_wgen_stop(c));
  }
  else if (!strcmp( "set", c->para[1]))
  {
    return acq1430_set(fmc, c, add);
  }
  else if( !strcmp( "sign", c->para[1]))
  {
    return acq1430_sign_cmd(fmc, c, add);
  }
  else if( !strcmp( "init", c->para[1]))
  {
    if (add->bus == BUS_SBC)
    {
      if (!strcmp("lmk", c->para[0]))
      {
        return acq1430_lmk_init(c, add);
      }
      if (!strcmp("dac", c->para[0]))
      {
        return acq1430_dac_init(c, add);
      }
      if (!strncmp("ads", c->para[0], 3))
      {
        if (add->idx >= 0 && add->idx <= 4)
        {
          adc_ch_mask = (3<<(2*add->idx));
        }
        else
        {
          adc_ch_mask = add->idx;
        }
        /* second param should be data format */
        if (c->cnt > 2 ) {
            if (!strcmp("bin", c->para[2])) {
                fmt = ADS42LB69_REG8_DATA_FORMAT_OFFSET_BIN;
                printf("Setting ADS42LB69 data format to offset binary\n");
            } else {
                fmt = 0;
                printf("Setting ADS42LB69 data format to two's complement\n");
            }
        } else {
            fmt = 0;
            printf("Setting ADS42LB69 data format to two's complement\n");
        }
        return daq1430_ads42lb69_init(tsc_fd, fmc, adc_ch_mask, fmt);
      }
    }
  }
  else if( !strcmp( "dump", c->para[1]))
  {
    return acq1430_dump(c, add);
  }
  else
  {
    fprintf(stderr, "bad operation : %s\n"                              \
                    "usage: acq1430.<fmc> <dev> read <reg>\n"           \
                    "       acq1430.<fmc> <dev> write <reg> <data>\n"   \
                    "       acq1430.<fmc> <dev> acq <off> <size>\n"     \
                    "       acq1430.<fmc> <file> save <off> <size>\n"   \
                    "       acq1430.<fmc> <dev> show\n", c->para[1]);
    return(-1);
  }
  return(0);

}
