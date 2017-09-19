/*=========================< begin file & file header >=======================
 *  References
 *  
 *    filename : tdma.c
 *    author   : JFG
 *    company  : IOxOS
 *    creation : Sept 15,2015
 *    version  : 0.0.1
 *
 *----------------------------------------------------------------------------
 *  Description
 *
 *     That file contains a set of function called by TscMon to handle the
 *     IFC1211 TDMA controller.
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

extern struct tsc_kbuf_ctl tsc_kbuf_ctl[];
struct tsc_ioctl_map_ctl mas_mem_map_ctl;
struct tsc_ioctl_map_ctl mas_pmem_map_ctl;

char *
tdma_rcsid()
{
  return( rcsid);
}

int
tdma_init( void)
{
  mas_mem_map_ctl.sg_id =  MAP_ID_MAS_PCIE_MEM;
  mas_mem_map_ctl.map_p = (struct tsc_map_blk *)0;
  tsc_map_read( &mas_mem_map_ctl);
  printf("MEM Win Base : 0x%08lx\n", mas_mem_map_ctl.win_base);
  mas_pmem_map_ctl.sg_id =  MAP_ID_MAS_PCIE_PMEM;
  mas_pmem_map_ctl.map_p = (struct tsc_map_blk *)0;
  tsc_map_read( &mas_pmem_map_ctl);
  printf("PMEM Win Base : 0x%08lx\n", mas_pmem_map_ctl.win_base);
  return( 0);
}

int
tsc_tdma_move( int chan,
	       dma_addr_t des_addr,
	       dma_addr_t src_addr,
	       int  size,
	       int mode)
{
  int fd;
  char *tdma_ptr;
  volatile int *reg_p;
  int i, csr_base;
  int tmo;

  printf("TDMA#%02x : moving data from %llx -> %llx [%x]\n", chan, src_addr, des_addr, size);
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

  *(int *)&tdma_ptr[0x100] = 0;                                      /* reset mode register */
  *(int *)&tdma_ptr[0x104] = 0x9b;                                   /* clear status register */
  *(int *)&tdma_ptr[0x110] = 0x50000 | (int)(src_addr >> 32) & 0xff; /* set SATR            */
  *(int *)&tdma_ptr[0x114] = (int)(src_addr & 0xffffffff);           /* set SAR             */
  *(int *)&tdma_ptr[0x118] = 0x50000 | (int)(des_addr >> 32) & 0xff; /* set SATR            */
  *(int *)&tdma_ptr[0x11c] = (int)(des_addr & 0xffffffff);           /* set SAR             */
  *(int *)&tdma_ptr[0x120] = size & 0x3ffffff;                       /* set BCR             */
  *(int *)&tdma_ptr[0x100] = mode;                                   /* start DMA           */
  *(int *)&tdma_ptr[0x100];                                          /* flush write access  */
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

int
tsc_tdma_status( int chan)
{
  int fd;
  char *tdma_ptr;
  int i, csr_base;
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

int 
tsc_tdma( struct cli_cmd_para *c)
{
  int retval;
  int chan;
  int  npara;
  long long tmp;

  chan = 0;
  if( c->cnt < 1) 
  {
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
    uint para;
    dma_addr_t des_addr, src_addr;
    int size, mode;
    char sp, idx;

    if( c->cnt < 4)
    {
      printf("Not enough parameters for tdma %s command! Type \"? tdma\" for help \n", c->para[0]);
      return( TSC_ERR);
    }

    npara = sscanf( c->para[1], "%llx:%c%c", &tmp, &sp, &idx);
    if( npara > 0)
    {
      des_addr = (dma_addr_t)tmp;
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
	    if( tsc_kbuf_ctl[idx].kbuf_p)
	    {
              des_addr += tsc_kbuf_ctl[idx].kbuf_p->b_base;
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

    npara = sscanf( c->para[2], "%llx:%c%c", &tmp, &sp, &idx);
    if( npara > 0)
    {
      src_addr = (dma_addr_t)tmp;
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
	    if( tsc_kbuf_ctl[idx].kbuf_p)
	    {
              src_addr += tsc_kbuf_ctl[idx].kbuf_p->b_base;
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
