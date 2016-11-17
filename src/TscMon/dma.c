/*=========================< begin file & file header >=======================
 *  References
 *  
 *    filename : dma.c
 *    author   : JFG
 *    company  : IOxOS
 *    creation : Sept 15,2015
 *    version  : 0.0.1
 *
 *----------------------------------------------------------------------------
 *  Description
 *
 *     That file contains a set of function called by TscMon to handle the
 *     IFC1211 DMA controller.
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
static char *rcsid = "$Id: dma.c,v 1.4 2015/12/04 13:27:52 ioxos Exp $";
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
#include "TscMon.h"

struct tsc_ioctl_dma_req dma_req[DMA_CHAN_NUM];
struct tsc_ioctl_dma_sts dma_sts[DMA_CHAN_NUM];
extern struct tsc_kbuf_ctl tsc_kbuf_ctl[];

char *
dma_rcsid()
{
  return( rcsid);
}

int 
tsc_dma( struct cli_cmd_para *c)
{
  int retval;
  int chan;
  long long tmp;

  if( c->cnt > 0) 
  {
    chan = DMA_CHAN_0;
    if( c->ext)
    {
      if( c->ext[0] == '1')
      {
	chan = DMA_CHAN_1;
      }
    }
  }
  else
  {
    tsc_print_usage( c);
    return( TSC_ERR);
  }
  if( !strncmp( "start", c->para[0], 5))
  {
    uint para;
    int  npara;
    char sw;

    if( c->cnt < 4)
    {
      printf("Not enough parameters for dma %s command! Type \"? dma\" for help \n", c->para[0]);
      return( TSC_ERR);
    }
    sw = 0;

//<<<<<<< HEAD
//    npara = sscanf( c->para[1], "%lx:%x.%c", &dma_req[chan].des_addr, &para, &sw);
//=======

    npara = sscanf( c->para[1], "%llx:%x.%c", &tmp, &para, &sw);
    dma_req[chan].des_addr = (dma_addr_t)tmp;
//>>>>>>> 5042c01d1d3599737cf51040056ce6585d536cbb

	dma_req[chan].des_mode = 0;
    if( para ==  DMA_SPACE_KBUF)
    {
      dma_req[chan].des_space = DMA_SPACE_PCIE;
      dma_req[chan].des_addr += tsc_kbuf_ctl[0].kbuf_p->b_base;
    }
    else 
    {
      dma_req[chan].des_space = (char)para;
    }
    if( npara < 2)
    {
      printf("dma %s command need destination address space [%s] Type \"? dma\" for help \n", c->para[0], c->para[1]);
      return( TSC_ERR);
    }
    if( npara == 3)
    {
 	if( sw == 'w')
	{
	  printf("Set destination word swapping\n");
	  dma_req[chan].des_space |= DMA_SPACE_WS;
	}
 	if( sw == 'd')
	{
	  printf("Set destination double word swapping\n");
	  dma_req[chan].des_space |= DMA_SPACE_DS;
	}
 	if( sw == 'q')
	{
	  printf("Set destination quad word swapping\n");
	  dma_req[chan].des_space |= DMA_SPACE_QS;
	}

    }
    sw = 0;
//<<<<<<< HEAD
//    npara = sscanf( c->para[2], "%lx:%x.%c", &dma_req[chan].src_addr, &para, &sw);
//=======
    npara = sscanf( c->para[2], "%llx:%x.%c", &tmp, &para, &sw);
    dma_req[chan].src_addr = (dma_addr_t)tmp;
//>>>>>>> 5042c01d1d3599737cf51040056ce6585d536cbb
    if( para ==  DMA_SPACE_KBUF)
    {
      dma_req[chan].src_space = DMA_SPACE_PCIE;
      dma_req[chan].src_addr += tsc_kbuf_ctl[0].kbuf_p->b_base;
    }
    else 
    {
    dma_req[chan].src_space = (char)para;
    }
    dma_req[chan].src_mode = 0;
    if( npara < 2)
    {
      printf("dma %s command need source address space [%s] Type \"? dma\" for help \n", c->para[0], c->para[2]);
      return( TSC_ERR);
    }
    if( npara == 3)
    {
 	if( sw == 'w')
	{
	  printf("Set source word swapping\n");
	  dma_req[chan].src_space |= DMA_SPACE_WS;
	}
 	if( sw == 'd')
	{
	  printf("Set source double word swapping\n");
	  dma_req[chan].src_space |= DMA_SPACE_DS;
	}
 	if( sw == 'q')
	{
	  printf("Set source quad word swapping\n");
	  dma_req[chan].src_space |= DMA_SPACE_QS;
	}
    }

    sscanf( c->para[3], "%x", &dma_req[chan].size);
    dma_req[chan].start_mode = (char)DMA_START_CHAN(chan);
    dma_req[chan].end_mode = 0;
    //dma_req[chan].wait_mode = DMA_WAIT_INTR | DMA_WAIT_1S | (5<<4); /* 5 sec timeout */
    dma_req[chan].wait_mode = 0;
    if( tsc_dma_alloc( chan))
    {
      printf("Cannot perform DMA transfer on channel #%d -> %s\n", chan, strerror(errno));
      return( TSC_ERR);
    }
    retval = tsc_dma_move(&dma_req[chan]);
    if( retval < 0)
    {
      printf("Cannot perform DMA transfer on channel #%d -> %s\n", chan, strerror(errno));
    }
    else 
    {
      printf("OK -> status = %08x\n", dma_req[chan].dma_status);
      dma_req[chan].wait_mode = DMA_WAIT_INTR | DMA_WAIT_1S | (5<<4); /* 5 sec timeout */
      tsc_dma_wait( &dma_req[chan]);
      if(  dma_req[chan].dma_status & DMA_STATUS_TMO)
      {
  	printf("NOK -> timeout - status = %08x\n",  dma_req[chan].dma_status);
      }
      else if(  dma_req[chan].dma_status & DMA_STATUS_ERR)
      {
	printf("NOK -> transfer error - status = %08x\n", dma_req[chan].dma_status);
      }
      else
      {
	  printf("OK -> status = %08x\n", dma_req[chan].dma_status);
      }
    }
    tsc_dma_free( chan);
    usleep(2000);
    return( TSC_OK);
  }
  else if( !strncmp( "status", c->para[0], 5))
  {
    struct tsc_ioctl_dma_sts *sts;

    sts = &dma_sts[chan];
    sts->dma.chan = (char)chan;
    printf("DMA#%d CSR registers\n", chan);
    tsc_dma_status( sts);
    printf("RD: %08x: %08x : %08x: %08x\n", sts->rd_csr, sts->rd_ndes, sts->rd_cdes, sts->rd_cnt);
    printf("WR: %08x: %08x : %08x: %08x\n", sts->wr_csr, sts->wr_ndes, sts->wr_cdes, sts->wr_cnt);
    return( TSC_OK);
  }
  else if( !strncmp( "wait", c->para[0], 3))
  {
    int retval;

    dma_req[chan].start_mode = (char)DMA_START_CHAN(chan);
    dma_req[chan].wait_mode = DMA_WAIT_INTR | DMA_WAIT_1S | (5<<4); /* 5 sec timeout */
    retval = tsc_dma_wait( &dma_req[chan]);
    if( retval < 0)
    {
      printf("Cannot perform DMA wait on channel #%d -> %s\n", chan, strerror(errno));
    }
    else 
    {
      if(  dma_req[chan].dma_status & DMA_STATUS_TMO)
      {
  	printf("NOK -> timeout - status = %08x\n",  dma_req[chan].dma_status);
      }
      else if(  dma_req[chan].dma_status & DMA_STATUS_ERR)
      {
	printf("NOK -> transfer error - status = %08x\n", dma_req[chan].dma_status);
      }
      else
      {
	  printf("OK -> status = %08x\n", dma_req[chan].dma_status);
      }
    }
    return( TSC_OK);
  }
  else if( !strncmp( "allocate", c->para[0], 3))
  {
    printf("DMA#%d allocate...", chan);
    if( tsc_dma_alloc( chan))
    {    
      printf(" BUSY\n");
    }
    else 
    {    
      printf(" DONE\n");
    }
    return( TSC_OK);
  }
  else if( !strncmp( "free", c->para[0], 3))
  {
    printf("DMA#%d free...", chan);
    if( tsc_dma_free( chan))
    {    
      printf(" BUSY\n");
    }
    else 
    {    
      printf(" DONE\n");
    }
    return( TSC_OK);
  }
  else if( !strncmp( "clear", c->para[0], 3))
  {
    tsc_dma_clear( chan);
    return( TSC_OK);
  }
  return( TSC_ERR);
}
