/*=========================< begin file & file header >=======================
 *  References
 *  
 *    filename : fscope3112lib.c
 *    author   : JFG, XP
 *    company  : IOxOS
 *    creation : october 18,2018
 *
 *----------------------------------------------------------------------------
 *  Description
 *
 *    That library contains a set of function to access the TOSCA II XUSER
 *    FASTSCOPE ADC3112 data acquisition logic.
 *
 *----------------------------------------------------------------------------
 *
 *  Copyright (C) IOxOS Technologies SA <ioxos@ioxos.ch>
 *  Copyright (C) 2019  European Spallation Source ERIC
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

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <tsculib.h>
#include <tscioctl.h>
#include <tscextlib.h>
#include <fscope3112lib.h>
#include <adc3112lib.h>
#include <errno.h>

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * Function name : fscope3112_XXX
 * Prototype     : int
 * Parameters    : void
 * Return        : 
 *----------------------------------------------------------------------------
 * Description   : 
 *
 *++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

int 
fscope3112_XXX(void)
{

  return( 0);
}

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * Function name : fscope3112_csr_rd
 * Prototype     : int
 * Parameters    : register index
 * Return        : register content
 *----------------------------------------------------------------------------
 * Description   : 
 *
 *++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

int 
fscope3112_csr_rd(int fd, int csr)
{
  int data;
  tsc_csr_read(fd, FSCOPE3112_CSR_ADDR( csr), &data);
  return( data);
}

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * Function name : fscope3112_csr_wr
 * Prototype     : vois
 * Parameters    : register index
 * Return        : register content
 *----------------------------------------------------------------------------
 * Description   : 
 *
 *++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

void 
fscope3112_csr_wr(int fd, int csr,
		   int data)
{
  tsc_csr_write(fd, FSCOPE3112_CSR_ADDR( csr), &data);
  return;
}

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * Function name : fscope3112_identify
 * Prototype     : int
 * Parameters    : none
 * Return        : fail/success
 *----------------------------------------------------------------------------
 * Description   : 
 *
 *++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

int 
fscope3112_identify(int fd)
{
  int id;

  id = fscope3112_csr_rd(fd, FSCOPE3112_CSR_SIGN);

  return( id);
}

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * Function name : fscope3112_map_usr
 * Prototype     : char *
 * Parameters    : map pointer
 *                 address offset
 *                 size
 *                 space
 * Return        : fail/success
 *----------------------------------------------------------------------------
 * Description   : 
 *
 *++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

struct tsc_ioctl_map_win *
fscope3112_map_usr(int fd, int fmc,
		    char **buf,
		    int offset,
		    int size)
{
  struct tsc_ioctl_map_win *map;
  int ret;

  map = malloc( sizeof( struct tsc_ioctl_map_win));
  bzero( (void *)map, sizeof(struct tsc_ioctl_map_win));
  map->req.rem_addr = FSCOPE3112_MAP_USR_OFF + offset;
  if( fmc == ADC3112_FMC2)
  {
    map->req.mode.space = MAP_SPACE_USR2;
  }
  else 
  {
    map->req.mode.space = MAP_SPACE_USR1;
  }
  map->req.mode.sg_id = MAP_ID_MAS_PCIE_MEM;
  map->req.size = size;
  ret = tsc_map_alloc(fd, map);
  if( ret < 0)
  {
    printf("cannot allocate user map %lx, %x [err: %d-%d]\n",  map->sts.loc_base, map->sts.size, ret, errno);
    *buf = NULL;
    return( NULL);
  }
  *buf = (char *)tsc_pci_mmap(fd, map->sts.loc_base, map->sts.size);
  return( map);
}

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * Function name : fscope3112_unmap_usr
 * Prototype     : int
 * Parameters    : pointer to map 
 * Return        : fail/success
 *----------------------------------------------------------------------------
 * Description   : 
 *
 *++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

void
fscope3112_unmap_usr(int fd, struct tsc_ioctl_map_win *map,
		      char *u_addr)
{
  tsc_pci_munmap(u_addr, map->sts.size);
  tsc_map_free(fd, map);
  free( map);
  return;
}

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * Function name : fscope3112_acq
 * Prototype     : int
 * Parameters    : fmc
 * Return        : fail/success
 *----------------------------------------------------------------------------
 * Description   : 
 *
 *++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

void
fscope3112_acq_init(int fd)
{
  fscope3112_csr_wr(fd, FSCOPE3112_CSR_INTR, 0);
  return;
}

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * Function name : fscope3112_acq
 * Prototype     : int
 * Parameters    : fmc
 * Return        : fail/success
 *----------------------------------------------------------------------------
 * Description   : 
 *
 *++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

int
fscope3112_intr_status(int fd)
{
  return( fscope3112_csr_rd(fd, FSCOPE3112_CSR_INTR));
}

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * Function name : fscope3112_acq_arm
 * Prototype     : int
 * Parameters    : fmc
 * Return        : fail/success
 *----------------------------------------------------------------------------
 * Description   : 
 *
 *++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

void
fscope3112_acq_arm(int fd, int fmc,
		     int trig_mode,
		     int trig_pos,
		     int buf_size)
{
  int ctl;
  ctl = 0x40040001 | ((trig_pos&7)<<5) | (buf_size << 12);
  if( fmc == ADC3112_FMC2)
  {
    fscope3112_csr_wr(fd, FSCOPE3112_CSR_ACQ2_CTL, 0x80040001);   /* FASTSCOPE Abort/Stop previous acquisition SRAM1 (in case of..) */
    fscope3112_csr_wr(fd, FSCOPE3112_CSR_TRIG2_STS, trig_mode & ~0x80000000);  
    fscope3112_csr_wr(fd, FSCOPE3112_CSR_TRIG2_STS, trig_mode);   /* FASTSCOPE Define trigger mode                                  */
    usleep( 2000);
    fscope3112_csr_wr(fd, FSCOPE3112_CSR_ACQ2_CTL, ctl);          /* Arm data acquisition SRAM1                                     */
    usleep( 2000);
    if( trig_mode == FSCOPE3112_TRIG_MODE_AUTO)
    {
      fscope3112_csr_wr(fd, FSCOPE3112_CSR_TRIG2_ADDR, 0x40000000); /* FASTSCOPE Manual trigger command                               */
    }
    usleep( 2000);
  }
  else 
  {
    fscope3112_csr_wr(fd, FSCOPE3112_CSR_ACQ1_CTL, 0x80040001);   /* FASTSCOPE Abort/Stop previous acquisition SRAM1 (in case of..) */
    fscope3112_csr_wr(fd, FSCOPE3112_CSR_TRIG1_STS, trig_mode & ~0x80000000);
    fscope3112_csr_wr(fd, FSCOPE3112_CSR_TRIG1_STS, trig_mode);   /* FASTSCOPE Define trigger mode                                  */
    usleep( 2000);
    fscope3112_csr_wr(fd, FSCOPE3112_CSR_ACQ1_CTL, ctl);          /* Arm data acquisition SRAM2                                     */
    usleep( 2000);
    if( trig_mode == FSCOPE3112_TRIG_MODE_AUTO)
    {
      fscope3112_csr_wr(fd, FSCOPE3112_CSR_TRIG1_ADDR, 0x40000000); /* FASTSCOPE Manual trigger command                               */
    }
    usleep( 2000);
  }
  return;
}

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * Function name : fscope3112_acq_rearm
 * Prototype     : int
 * Parameters    : fmc
 * Return        : fail/success
 *----------------------------------------------------------------------------
 * Description   : 
 *
 *++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

int
fscope3112_acq_rearm(int fd, int fmc)
{
  int ctl;

  if( fmc == ADC3112_FMC2)
  {
    ctl = fscope3112_csr_rd(fd, FSCOPE3112_CSR_ACQ2_CTL);
    ctl = 0xc0000000 & (ctl & 0x3fffffff);
    fscope3112_csr_wr(fd, FSCOPE3112_CSR_ACQ2_CTL, ctl);
  }
  else 
  {
    ctl = fscope3112_csr_rd(fd, FSCOPE3112_CSR_ACQ1_CTL); 
    ctl = 0xc0000000 & (ctl & 0x3fffffff);
    fscope3112_csr_wr(fd, FSCOPE3112_CSR_ACQ1_CTL, ctl);
  }
  return(ctl);
}
/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * Function name : fscope3112_acq_stop
 * Prototype     : int
 * Parameters    : fmc
 * Return        : fail/success
 *----------------------------------------------------------------------------
 * Description   : 
 *
 *++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

int
fscope3112_acq_stop(int fd, int fmc)
{
  int ctl;

  if( fmc == ADC3112_FMC2)
  {
    ctl = fscope3112_csr_rd(fd, FSCOPE3112_CSR_ACQ2_CTL);
    ctl = 0x80000000 | (ctl & 0x3fffffff);
    fscope3112_csr_wr(fd, FSCOPE3112_CSR_ACQ2_CTL, ctl);
  }
  else 
  {
    ctl = fscope3112_csr_rd(fd, FSCOPE3112_CSR_ACQ1_CTL); 
    ctl = 0x80000000 | (ctl & 0x3fffffff);
    fscope3112_csr_wr(fd, FSCOPE3112_CSR_ACQ1_CTL, ctl);
  }
  return(ctl);
}


/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * Function name : fscope3112_trig_time
 * Prototype     : int
 * Parameters    : fmc
 * Return        : fail/success
 *----------------------------------------------------------------------------
 * Description   : 
 *
 *++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

int
fscope3112_trig_time(int fd, int fmc,
		      int *msec_p,
		      int *nsec_p)
{
  int csr;
  if( fmc == ADC3112_FMC2)
  {
    csr = fscope3112_csr_rd(fd, FSCOPE3112_CSR_TRIG2_TIM);
  }
  else 
  {
    csr = fscope3112_csr_rd(fd, FSCOPE3112_CSR_TRIG1_TIM);
  }
  if( msec_p && nsec_p)
  {
    *msec_p = (csr>>17)&0x7fff;
    *nsec_p = (csr &0x1ffff) *10;
  }
  return(csr);
}

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * Function name : fscope3112_acq_clear
 * Prototype     : int
 * Parameters    : fmc
 * Return        : fail/success
 *----------------------------------------------------------------------------
 * Description   : clear acquisition buffers
 *
 *++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

void
fscope3112_acq_clear(int fd, int fmc,
		      int chan) 
{
  struct tsc_ioctl_map_win *usr_map;
  char *adc_buf;
  int i;

  if( (chan < 0) || (chan > (ADC3112_CHAN_NUM-1))) return;
  usr_map = fscope3112_map_usr(fd, fmc, &adc_buf, FSCOPE3112_ACQ_SIZE*chan, FSCOPE3112_ACQ_SIZE);
  for( i = 0; i < FSCOPE3112_ACQ_SIZE; i += 4)
  {
    *(int *)&adc_buf[i] = 0;
  }

  fscope3112_unmap_usr(fd, usr_map, adc_buf);
  return;
}

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * Function name : fscope3112_acq_load
 * Prototype     : int
 * Parameters    : fmc
 * Return        : fail/success
 *----------------------------------------------------------------------------
 * Description   : clear acquisition buffers
 *
 *++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

void
fscope3112_acq_load(int fd, int fmc,
                     int chan, 
		     char *buf,
		     int size)
{
  struct tsc_ioctl_map_win *usr_map;
  char *adc_buf;
  int i, start;

  start =  FSCOPE3112_ACQ_SIZE*chan;
  if( size > FSCOPE3112_ACQ_SIZE)
  {
    size = FSCOPE3112_ACQ_SIZE;
  }
  usr_map = fscope3112_map_usr(fd, fmc, &adc_buf, start, size);
  for( i = 0; i < size; i += 4)
  {
    *(int *)&buf[i] =*(int *)&adc_buf[i];
  }
  fscope3112_unmap_usr(fd, usr_map, adc_buf);

  return;
}


/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * Function name : fscope3112_acq_read
 * Prototype     : int
 * Parameters    : fmc
 *                 channel
 *                 data buffer pointer
 *                 readout offset
 *                 buffer size
 * Return        : fail/success
 *----------------------------------------------------------------------------
 * Description   : transfer data from hardware buffers to local buffer
 *
 *++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

int
fscope3112_acq_read(int fd, int fmc, 
                     int chan, 
                     unsigned short *buf,
		     int size)
{
  struct tsc_ioctl_map_win *usr_map;
  char *adc_buf;
  int i, n, data;
  int start;
  int trig_pos, trig_end, trig_mode, trig_off;
  int buf_size, ctl;

  if( fmc == ADC3112_FMC2)
  {
    ctl = fscope3112_csr_rd(fd, FSCOPE3112_CSR_ACQ2_CTL);
    trig_pos = fscope3112_csr_rd(fd, FSCOPE3112_CSR_TRIG2_ADDR);
    trig_mode = fscope3112_csr_rd(fd, FSCOPE3112_CSR_TRIG2_STS);
  }
  else 
  {
    ctl = fscope3112_csr_rd(fd, FSCOPE3112_CSR_ACQ1_CTL);
    trig_pos = fscope3112_csr_rd(fd, FSCOPE3112_CSR_TRIG1_ADDR);
    trig_mode = fscope3112_csr_rd(fd, FSCOPE3112_CSR_TRIG1_STS);
  }
  switch( trig_mode & FSCOPE3112_TRIG_MODE_MASK)
  {
    case FSCOPE3112_TRIG_MODE_AUTO:
    {
      //printf("trigger mode AUTO %08x\n", trig_mode);
      //buf_size = size-0x20;
      buf_size = 0x400<<(6-((ctl>>12)&7));
      trig_end = buf_size;
      //trig_pos = 0x10;
      trig_pos = 0x0;
      break;
    }
    case  FSCOPE3112_TRIG_MODE_TTRIG & FSCOPE3112_TRIG_MODE_MASK:
    {
      //printf("trigger mode TTIM\n", trig_mode);
      buf_size = 0x400<<(6-((ctl>>12)&7));
      trig_end = (buf_size/8)*((ctl>>5)&7);
      trig_pos = (trig_pos&0xffff) + 0x10;
      break;
    }
    case  FSCOPE3112_TRIG_MODE_GPIO & FSCOPE3112_TRIG_MODE_MASK:
    {
      //printf("trigger mode GPIO\n", trig_mode);
      buf_size = 0x400<<(6-((ctl>>12)&7));
      trig_end = (buf_size/8)*((ctl>>5)&7);
      trig_pos = (trig_pos&0xffff) + 0x10;
      break;
    }
    case FSCOPE3112_TRIG_MODE_ADC & FSCOPE3112_TRIG_MODE_MASK:
    {
      //printf("trigger mode ADC %08x\n", trig_mode);
      buf_size = 0x400<<(6-((ctl>>12)&7));
      trig_end = (buf_size/8)*((ctl>>5)&7);
      trig_pos = (trig_pos&0xffff) + 4;
      printf("buf_size = %x - trig_pos = %x - trig_end = %x [%08x]\n", buf_size, trig_pos, trig_end, ctl);
      break;
    }
    default:
    {
      printf("trigger mode %08x not supported !!\n", trig_mode);
      return(-1);
    }
  }
  //printf("buf_size = %x - trig_pos = %x - trig_end = %x [%08x]\n", buf_size, trig_pos, trig_end, ctl);
  if( size < buf_size)
  {
    printf("buffer size to small!!\n");
    return(-1);
  }
  if( trig_pos > trig_end)
  {
    printf("trigger position invalid !!\n");
    return(-1);
  }

  start =  (FSCOPE3112_ACQ_SIZE*chan);
  usr_map = fscope3112_map_usr(fd, fmc, &adc_buf, start, buf_size);
  if( usr_map == NULL)
  {
    printf("Cannot map adc buffer in user space  -> %p\n", adc_buf);
    return(-1);
  }
  n = 0;
  trig_off = 0;
  start = 0;
  /* if trig pos is odd -> align to a multiple of 4 */
  if( trig_pos & 2)
  {
    unsigned short tmp;

    tmp =  *(unsigned short *)&adc_buf[start+trig_pos];
    trig_off = 2;
    buf[n] = (tsc_swap_16(tmp) >> 4) & 0xfff;
    n += 1;
  }
  for( i = start+trig_pos+trig_off; i < start+trig_end; i += 4)
  {
    data = tsc_swap_32(*(int *)&adc_buf[i]);
    buf[n] = (data >> 4)&0xfff;
    buf[n+1] = (data >> 20)&0xfff;
    //data = *(int *)&adc_buf[i];
    //buf[n+1] = tsc_swap_16((unsigned short)data) >> 4;
    //buf[n] = tsc_swap_16((unsigned short)(data >> 16)) >> 4;
    n += 2;
  }
  for( i = start; i < start+trig_pos-trig_off; i += 4)
  {
    data = tsc_swap_32(*(int *)&adc_buf[i]);
    buf[n] = (data >> 4)&0xfff;
    buf[n+1] = (data >> 20)&0xfff;
    //data = *(int *)&adc_buf[i];
    //buf[n+1] = tsc_swap_16((unsigned short)data) >> 4;
    //buf[n] = tsc_swap_16((unsigned short)(data >> 16)) >> 4;
    n += 2;
  }
  if( trig_off)
  {
    unsigned short tmp;

    tmp =  *(unsigned short *)&adc_buf[start+trig_pos-trig_off];
    buf[n] = (tsc_swap_16(tmp) >> 4)&0xfff;
    n += 1;
  }
  for( i = start+trig_end; i < start+buf_size; i += 4)
  {
    data = tsc_swap_32(*(int *)&adc_buf[i]);
    buf[n] = (data >> 4)&0xfff;
    buf[n+1] = (data >> 20)&0xfff;
    //data = *(int *)&adc_buf[i];
    //buf[n+1] = tsc_swap_16((unsigned short)data) >> 4;
    //buf[n] = tsc_swap_16((unsigned short)(data >> 16)) >> 4;
    n += 2;
  }
  fscope3112_unmap_usr(fd, usr_map, adc_buf);
  return( buf_size);
}

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * Function name : fscope3112_acq_save
 * Prototype     : int
 * Parameters    : fmc
 *                 channel
 *                 filename
 * Return        : fail/success
 *----------------------------------------------------------------------------
 * Description   : 
 *
 *++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

int
fscope3112_acq_save(int fd, int fmc, 
                     int chan, 
                     char *filename)
{
  unsigned short *buf;
  int i;
  FILE *acq_file;
  int buf_size;

  acq_file = fopen(filename, "w");
  if( !acq_file)
  {
    return( -1);
  }
  buf = (unsigned short *)malloc( FSCOPE3112_ACQ_SIZE);
  buf_size = fscope3112_acq_read(fd, fmc, chan, buf, FSCOPE3112_ACQ_SIZE);
  for( i = 0; i < buf_size/2; i++)
  {
    fprintf( acq_file, "%d, %d\n", i, (int)buf[i] - 0x800);
  }
  fclose( acq_file);
  free( buf);
  return( 0);
}
