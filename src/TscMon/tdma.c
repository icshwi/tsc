/*=========================< begin file & file header >=======================
 *  References
 *  
 *    filename : tdma.c
 *    author   : JFG, XP
 *    company  : IOxOS
 *    creation : Sept 15,2015
 *
 *----------------------------------------------------------------------------
 *  Description
 *
 *    That file contains a set of function called by TscMon to handle the
 *    TSC TDMA controller.
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

#ifndef lint
static char *rcsid = "$Id: $";
#endif

#define DEBUGno
#include <debug.h>
#include <sys/types.h>
#include <sys/time.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <cli.h>
#include <tscioctl.h>
#include <tsculib.h>
#include <aio.h>
#include <errno.h>
#include <ctype.h>
#include <time.h>
#include <fcntl.h>
#include <sys/mman.h>
#include "TscMon.h"
#include "tdma.h"

extern struct tsc_kbuf_ctl tsc_kbuf_ctl[];
struct tsc_ioctl_map_ctl mas_mem_map_ctl;
struct tsc_ioctl_map_ctl mas_pmem_map_ctl;

extern int tsc_fd;

char *
tdma_rcsid()
{
  return( rcsid);
}

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * Function name : tdma_init
 * Prototype     : int
 * Parameters    : quiet mode
 * Return        : 0
 *----------------------------------------------------------------------------
 * Description   : dma init buffer
 *
 *++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

int
tdma_init(int quiet)
{
  mas_mem_map_ctl.sg_id =  MAP_ID_MAS_PCIE_MEM;
  mas_mem_map_ctl.map_p = (struct tsc_map_blk *)0;
  tsc_map_read(tsc_fd, &mas_mem_map_ctl);
  if (quiet == 0){
	  printf("MEM Win Base : 0x%08lx\n", mas_mem_map_ctl.win_base);
  }
  mas_pmem_map_ctl.sg_id =  MAP_ID_MAS_PCIE_PMEM;
  mas_pmem_map_ctl.map_p = (struct tsc_map_blk *)0;
  tsc_map_read(tsc_fd, &mas_pmem_map_ctl);
  if (quiet == 0){
	  printf("PMEM Win Base : 0x%08lx\n", mas_pmem_map_ctl.win_base);
  }
  return( 0);
}

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * Function name : tsc_tdma_move
 * Prototype     : int
 * Parameters    : channel, source, destination, size, mode
 * Return        : ok or nok
 *----------------------------------------------------------------------------
 * Description   : execute dma move
 *
 *++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

int
tsc_tdma_move( int chan,
		   uint64_t des_addr,
		   uint64_t src_addr,
	       int  size,
	       int mode)
{
  int fd;
  char *tdma_ptr;
  volatile int *reg_p;
  int csr_base;
  int tmo;

  printf("TDMA#%02x : moving data from %lx -> %lx [%x]\n", chan, src_addr, des_addr, size);
  fd = open("/dev/ioxos/tdma_ctl", O_RDWR);

  if( fd < 0)
  {
    printf("cannot open device /dev/ioxos/tdma_ctl\n");
    return( TSC_ERR);
  }
  tdma_ptr = NULL;
  csr_base = (chan&0x30)<<8;
  tdma_ptr = mmap( NULL, 0x1000 ,PROT_READ|PROT_WRITE, MAP_SHARED, fd, csr_base);
  if( !tdma_ptr)
  {
    printf("Cannot map TDMA registers\n");
    return( TSC_ERR);
  }
  tdma_ptr += (chan&3)*0x80;
  if(chan&4) tdma_ptr += 0x300;
  //printf("TDMA registers mapped at address %p [%08x]\n", tdma_ptr, *tdma_ptr);

  *(int *)&tdma_ptr[0x100] = 0;                                        /* reset mode register */
  *(int *)&tdma_ptr[0x104] = 0x9b;                                     /* clear status register */
  *(int *)&tdma_ptr[0x110] = 0x50000 | ((int)(src_addr >> 32) & 0xff); /* set SATR            */
  *(int *)&tdma_ptr[0x114] = (int)(src_addr & 0xffffffff);             /* set SAR             */
  *(int *)&tdma_ptr[0x118] = 0x50000 | ((int)(des_addr >> 32) & 0xff); /* set SATR            */
  *(int *)&tdma_ptr[0x11c] = (int)(des_addr & 0xffffffff);             /* set SAR             */
  *(int *)&tdma_ptr[0x120] = size & 0x3ffffff;                         /* set BCR             */
  *(int *)&tdma_ptr[0x100] = mode;                                     /* start DMA           */
  tmo = *(int *)&tdma_ptr[0x100];                                      /* flush write access  */
  usleep(1000);
  reg_p = (volatile int *)&tdma_ptr[0x104];
  tmo = size;
  while(  (*reg_p & 4) && --tmo);
  if( !tmo)
  {
    printf("transfer timeout -> status = %08x\n", *reg_p);
  }
#ifdef DBG
  reg_p = (int *)&tdma_ptr[0x100];
  for( i = 0x0; i < 0x10; i++)
  {
    if( !(i&3))printf("\n%04x : ", i*4);
    printf("%08x ", reg_p[i]);
  }
  printf("\n");
#endif
  munmap( tdma_ptr, 0x1000);
  close( fd);
  return( TSC_OK);
}

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * Function name : tsc_tdma_status
 * Prototype     : int
 * Parameters    : channel
 * Return        : status
 *----------------------------------------------------------------------------
 * Description   : acquire dma status
 *
 *++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

int
tsc_tdma_status( int chan)
{
  int fd;
  char *tdma_ptr;
  int csr_base;
  int sts;

  fd = open("/dev/ioxos/tdma_ctl", O_RDWR);

  if( fd < 0)
  {
    printf("cannot open device /dev/ioxos/tdma_ctl\n");
    return( TSC_ERR);
  }
  tdma_ptr = NULL;
  csr_base = (chan&0x30)<<8;
  tdma_ptr = mmap( NULL, 0x1000 ,PROT_READ|PROT_WRITE, MAP_SHARED, fd, csr_base);
  if( !tdma_ptr)
  {
    printf("Cannot map TDMA registers\n");
    return( TSC_ERR);
  }
  tdma_ptr += (chan&3)*0x80;
  if(chan&4) tdma_ptr += 0x300;
  sts = *(int *)&tdma_ptr[0x104];
  printf("TDMA#%d%d status\n", (chan%3)>>4, chan&7);
  printf("mode/status = %08x %08x\n", *(int *)&tdma_ptr[0x100], *(int *)&tdma_ptr[0x104]);
  printf("source      = %08x %08x\n", *(int *)&tdma_ptr[0x110], *(int *)&tdma_ptr[0x114]);
  printf("destination = %08x %08x\n", *(int *)&tdma_ptr[0x118], *(int *)&tdma_ptr[0x11c]);
  printf("byte count  = %08x\n", *(int *)&tdma_ptr[0x120]);

  munmap( tdma_ptr, 0x1000);
  close( fd);
  return( sts);
}

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * Function name : tsc_tdma_clear
 * Prototype     : int
 * Parameters    : channel
 * Return        : status
 *----------------------------------------------------------------------------
 * Description   : clear dma
 *
 *++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

int
tsc_tdma_clear( int chan)
{
  int fd;
  char *tdma_ptr;
  int sts, csr_base;

  fd = open("/dev/ioxos/tdma_ctl", O_RDWR);

  if( fd < 0)
  {
    printf("cannot open device /dev/ioxos/tdma_ctl\n");
    return( TSC_ERR);
  }
  tdma_ptr = NULL;
  csr_base = (chan&0x30)<<8;
  tdma_ptr = mmap( NULL, 0x1000 ,PROT_READ|PROT_WRITE, MAP_SHARED, fd, csr_base);
  if( !tdma_ptr)
  {
    printf("Cannot map TDMA registers\n");
    return( TSC_ERR);
  }
  tdma_ptr += (chan&3)*0x80;
  if(chan&4) tdma_ptr += 0x300;
  *(int *)&tdma_ptr[0x100] = 0x8;
  sts = *(int *)&tdma_ptr[0x104];
  printf("TDMA#%d%d status %08x\n", (chan%3)>>4, chan&7, sts);

  munmap( tdma_ptr, 0x1000);
  close( fd);
  return( sts);
}

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * Function name : tsc_tdma
 * Prototype     : int
 * Parameters    : command line parameter structure
 * Return        : status
 *----------------------------------------------------------------------------
 * Description   : main command dma
 *
 *++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

int 
tsc_tdma( struct cli_cmd_para *c)
{
  int retval;
  int chan;
  int  npara;
  uint64_t tmp;

  chan = 0;
  if( c->cnt < 1) 
  {
	printf("Not enough arguments -> usage:\n");
    tsc_print_usage( c);
    return( TSC_ERR);
  }
  chan = 0;
  if( c->ext)
  {
    sscanf( c->ext, "%x", &chan);
  }
  if( !strncmp( "start", c->para[0], 5))
  {
    uint64_t des_addr, src_addr;
    int size, mode;
    char sp, idx;

    if( c->cnt < 4)
    {
      printf("Not enough parameters for tdma %s command! Type \"? tdma\" for help \n", c->para[0]);
      return( TSC_ERR);
    }

    npara = sscanf( c->para[1], "%lx:%c%c", &tmp, &sp, &idx);
    if( npara > 0)
    {
      des_addr = (uint64_t)tmp;
      if( npara == 3)
      {
        if( sp == 'm')
        {
          if( idx == 'm') des_addr += mas_mem_map_ctl.win_base;
	      if( idx == 'p') des_addr += mas_pmem_map_ctl.win_base;
	    }
	    if( sp == 'k')
	    {
	      if( (idx >= '0') && ( idx < '8'))
	      {
	        idx -= '0';
	        if( tsc_kbuf_ctl[(int)idx].kbuf_p)
	        {
              des_addr += tsc_kbuf_ctl[(int)idx].kbuf_p->b_base;
	        }
	      }
	    }
      }
    }
    else
    {
      printf("tdma %s command -> bad destination address space [%s] Type \"? tdma\" for help \n", c->para[0], c->para[1]);
      return( TSC_ERR);
    }

    npara = sscanf( c->para[2], "%lx:%c%c", &tmp, &sp, &idx);
    if( npara > 0)
    {
      src_addr = (uint64_t)tmp;
      if( npara == 3)
      {
        if( sp == 'm')
        {
          if( idx == 'm') src_addr += mas_mem_map_ctl.win_base;
    	  if( idx == 'p') src_addr += mas_pmem_map_ctl.win_base;
    	}
  	    if( sp == 'k')
  	    {
  	      if( (idx >= '0') && ( idx < '8'))
  	      {
  	        idx -= '0';
  	        if( tsc_kbuf_ctl[(int)idx].kbuf_p)
  	        {
              src_addr += tsc_kbuf_ctl[(int)idx].kbuf_p->b_base;
  	        }
  	      }
  	    }
      }
    }
    else
    {
      printf("tdma %s command -> bad source address space [%s] Type \"? tdma\" for help \n", c->para[0], c->para[2]);
      return( TSC_ERR);
    }

    npara = sscanf( c->para[3], "%x", &size);
    if( (npara != 1) || (size < 1) || (size > 0x1000000))
    {
      printf("tdma %s command -> bad transfer size [%s] Type \"? tdma\" for help \n", c->para[0], c->para[3]);
      return( TSC_ERR);
    }
    mode = 0x8000005; /* default mode is direct with max block size = 256 */
    if( c->cnt > 4)
    {
      npara = sscanf( c->para[4], "%x", &mode);
      if( npara != 1)
      {
        printf("tdma %s command -> bad transfer mode [%s] Type \"? tdma\" for help \n", c->para[0], c->para[4]);
        return( TSC_ERR);
      }
    }
    retval = tsc_tdma_move( chan, des_addr, src_addr, size, mode);
    if( retval < 0)
    {
      //printf("Cannot perform TDMA transfer on channel #%d -> %s\n", chan, strerror(errno));
      return( TSC_ERR);
    }
    return( TSC_OK);
  }
  else if( !strncmp( "status", c->para[0], 5))
  {
    tsc_tdma_status( chan);
    return( TSC_OK);
  }
  else if( !strncmp( "clear", c->para[0], 3))
  {
    tsc_tdma_clear( chan);
    return( TSC_OK);
  }
  return( TSC_ERR);
}
