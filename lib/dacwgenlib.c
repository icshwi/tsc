/*=========================< begin file & file header >=======================
 *  References
 *
 *    filename : dacwgenlib.c
 *    author   : CG
 *    company  : IOxOS
 *    creation : August 24,2020
 *
 *----------------------------------------------------------------------------
 *  Description
 *
 *    That library contains a set of function to handle DAC Waveform Generator.
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

#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <math.h>

#include <tsculib.h>

#include "dacwgenlib.h"

static uint     dacwgen_reg_base;                     /* Base address of registers  */
static uint     dacwgen_buf_base;                     /* Base address of buffers    */

#define DACWGEN_TMPBUF_COUNT    4096

static uint16_t dacwgen_tmpbuf[DACWGEN_TMPBUF_COUNT]; /* temporary buffer           */

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * Function name : dacwgen_update_buf
 * Prototype     : int
 * Parameters    : addr, data
 * Return        : status of write operation
 *----------------------------------------------------------------------------
 * Description   : copy temporary buffer into waveform generator channel
 *
 *++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

static int dacwgen_update_buf(int fd, int ch)
{
  uint usr_addr;
  int rc = 0;

  usr_addr = dacwgen_buf_base + ((ch&3)<<17);

  rc = tsc_usr_write(fd, usr_addr, (char *)&dacwgen_tmpbuf, sizeof(dacwgen_tmpbuf), 4, 0, 1);

  return(rc);
}

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * Function name : dacwgen_reg_write
 * Prototype     : int
 * Parameters    : addr, data
 * Return        : status of write operation
 *----------------------------------------------------------------------------
 * Description   : write a given data into waveform generator register
 *
 *++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

static int dacwgen_reg_write(int fd, uint addr, int data)
{
  uint usr_addr;
  int rc = 0, tmp;

  usr_addr = dacwgen_reg_base + (addr&0xff);

  /* swap when needed */
  if (CheckByteOrder() == 1)
  {
    tmp = tsc_swap_32(data);
  }
  else
  {
    tmp = data;
  }

  rc = tsc_usr_write(fd, usr_addr, (char *)&tmp, 0, 4, 0, 1);

  return(rc);
}

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * Function name : dacwgen_reg_read
 * Prototype     : int
 * Parameters    : addr, data
 * Return        : status of read operation
 *----------------------------------------------------------------------------
 * Description   : read a given data into waveform generator register
 *
 *++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

static int dacwgen_reg_read(int fd, uint addr, int *data)
{
  uint usr_addr;
  int rc = -1, tmp;

  if (data != NULL)
  {
    usr_addr = dacwgen_reg_base + (addr&0xff);

    rc = tsc_usr_read(fd, usr_addr, (char *)&tmp, 0, 4, 0, 1);

    /* swap when needed */
    if (CheckByteOrder() == 1)
      tmp = tsc_swap_32(tmp);

    if (data != NULL)
    {
      (*data) = tmp;
    }
  }
  return(rc);
}

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * Function name : dacwgen_reg_modify
 * Prototype     : int
 * Parameters    : addr, data, mask
 * Return        : status of read operation
 *----------------------------------------------------------------------------
 * Description   : read and write a part of a waveform generator register
 *
 *++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

static int dacwgen_reg_modify(int fd, uint addr, int data, int mask)
{
  int tmp, rc = 0;

  if (mask != 0xffffffff)
  {
    rc = dacwgen_reg_read(fd, addr, &tmp);
    if (rc < 0)
      return(rc);
  }
  tmp = ((tmp & (~mask)) | (data & mask));

  rc = dacwgen_reg_write(fd, addr, tmp);

  return(rc);
}

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * Function name : dacwgen_fill_file
 * Prototype     : int
 * Parameters    : channel and filename
 * Return        : status of operation
 *----------------------------------------------------------------------------
 * Description   : load a waveform channel buffer with data from a file
 *
 *++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

int dacwgen_fill_file(int fd, int ch, char *filename, int quiet)
{
  FILE *in;
  int rc = 0, i, data;
  int is_big_endian;

  is_big_endian = CheckByteOrder();

  in = fopen(filename, "r");
  if (in == NULL)
  {
    if (!quiet)
      fprintf(stderr, "cannot open file '%s'\n", filename);

    return(-1);
  }
  for (i=0; i<DACWGEN_TMPBUF_COUNT; i++)
  {
    if (fscanf(in, "%*d %d\n", &data) != 1)
    {
      fclose(in);
      if (!quiet)
        fprintf(stderr, "'%s' -> bad file format\n", filename);
      return(-1);
    }
    if (data < 0 || data > 0xFFFF)
    {
      fclose(in);
      if (!quiet)
        fprintf(stderr, "'%s' -> invalid value (%d)\n", filename, data);
      return(-1);
    }

    /* swap when needed */
    if (is_big_endian)
      data = (int)tsc_swap_16((short)data);

    dacwgen_tmpbuf[i] = (uint16_t)data;
  }

  rc = dacwgen_update_buf(fd, ch);

  return(rc);
}

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * Function name : dacwgen_start_wave
 * Prototype     : int
 * Parameters    : channel, mode
 * Return        : status of operation
 *----------------------------------------------------------------------------
 * Description   : start waveform generator
 *
 *++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

int dacwgen_start_wave(int fd, int ch, int mode)
{
  int rc, data, mask, timeout;

  mask = 0xc0;

  if (mode != -1)
  {
    mask |= 7;
  }

  mask = (mask << ((3-(ch&3))*8));

  rc = dacwgen_reg_modify(fd, XUSER_WGEN_CS0_CS, 0, mask);

  if (rc < 0)
    return(rc);

  data = (XUSER_WGEN_CS_RUN(ch) | XUSER_WGEN_CS_ENABLE(ch));

  if (mode != -1)
  {
    data |= XUSER_WGEN_CS_MODE(ch, mode);
  }

  rc = dacwgen_reg_modify(fd, XUSER_WGEN_CS0_CS, data, mask);

  if (rc < 0)
    return(rc);

  timeout = 1000;
  while (timeout > 0) 
  {
    rc = dacwgen_reg_read(fd, XUSER_WGEN_CS0_CS, &data);
    if (rc < 0)
      return(rc);

    if (data & XUSER_WGEN_CS_BUSY(ch))
      break;
  
    usleep(10);
  
    timeout--;
  }
  
  return(rc);
}

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * Function name : dacwgen_stop_wave
 * Prototype     : int
 * Parameters    : channel (or -1 for all channels)
 * Return        : status of operation
 *----------------------------------------------------------------------------
 * Description   : stop waveform generator
 *
 *++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

int dacwgen_stop_wave(int fd, int ch)
{
  int rc, mask, timeout, data;

  if (ch != -1)
  {
    mask = (0xC0<<((3-(ch&3))*8));
  }
  else
  {
    mask = 0xC0C0C0C0;
  }
  rc = dacwgen_reg_modify(fd, XUSER_WGEN_CS0_CS, 0x00000000, mask);

  timeout = 1000;
  while (timeout > 0) 
  {
    rc = dacwgen_reg_read(fd, XUSER_WGEN_CS0_CS, &data);
    if (rc < 0)
      return(rc);

    if (!(data & XUSER_WGEN_CS_BUSY(ch)))
      break;
  
    usleep(10);
  
    timeout--;
  }

  return(rc);
}

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * Function name : dacwgen_init
 * Prototype     : int
 * Parameters    : -
 * Return        : status of operation
 *----------------------------------------------------------------------------
 * Description   : initialize waveform generator
 *
 *++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

int dacwgen_init(int fd)
{
  int rc, tmp, wsiz;

  rc = tsc_csr_read(fd, TSC_CSR_TMEM4_BRG_CSR, &tmp);
  if (rc < 0)
    return(rc);

  wsiz = ((tmp>>4)&0xf);

  /* CS = 0 -> control & status registers */
  dacwgen_reg_base = TSC_TMEM_OFFSET(4, wsiz);

  /* CS = 1 -> buffer */
  dacwgen_buf_base = TSC_TMEM_OFFSET(5, wsiz);

  rc = dacwgen_reg_read(fd, XUSER_WGEN_CS0_SIGN, &tmp);
  if (rc < 0)
    return(rc);

  return ((tmp == XUSER_WGEN_SIGN) ? 1 : -1);
}

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * Function name : dacwgen_set_wave
 * Prototype     : int
 * Parameters    : channel, init, step (-1 = no change)
 * Return        : status of operation
 *----------------------------------------------------------------------------
 * Description   : configure waveform generator parameters
 *
 *++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

/* dacwgen set c:<chan> r:<ref>, f:<init> s:<step> */

int dacwgen_set_wave(int fd, int ch, int ref, int init, int step, int quiet)
{
  int rc, data, mask, tmp, enabled;

  /* update DEFAULT value */
  if (ref != -1)
  {
    if (ch&1)
    {
      mask = 0xFFFF0000;
      data = ((ref & 0xffff) << 16);
    }
    else
    {
      mask = 0x0000FFFF;
      data = (ref & 0xffff);
    }
    if (ch<2)
    {
      rc = dacwgen_reg_modify(fd, XUSER_WGEN_CS0_DEF_AB, data, mask);
    }
    else
    {
      rc = dacwgen_reg_modify(fd, XUSER_WGEN_CS0_DEF_CD, data, mask);
    }
    if (rc < 0)
      return (rc);
  }

  enabled = 0;

  /* need to stop waveform generator if enabled */
  if (init != -1 || step != -1)
  {
    rc = dacwgen_reg_read(fd, XUSER_WGEN_CS0_CS, &tmp);
    if (tmp & XUSER_WGEN_CS_RUN(ch))
    {
      rc = dacwgen_stop_wave(fd, ch);
      if (rc < 0)
        return (rc);
      enabled = 1;
    }
  }

  /* update INITial value */
  if (init != -1)
  {
    rc = dacwgen_reg_write(fd, XUSER_WGEN_CS0_INI_A+(8*(ch&3)), init);
    if (rc < 0)
      return (rc);
  }

  /* update STEP value */
  if (step != -1)
  {
    rc = dacwgen_reg_write(fd, XUSER_WGEN_CS0_STEP_A+(8*(ch&3)), step);
    if (rc < 0)
      return (rc);
  }

  /* need to start again if it was enabled before */
  if (enabled == 1 && (init != -1 || step != -1))
  {
    rc = dacwgen_start_wave(fd, ch, -1);
  }

  return(rc);
}

