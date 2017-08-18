/*=========================< begin file & file header >=======================
 *  References
 *  
 *    filename : dmalib.c
 *    author   : JFG
 *    company  : IOxOS
 *    creation : Sept 27,2015
 *    version  : 0.0.1
 *
 *----------------------------------------------------------------------------
 *  Description
 *
 *    This file contains the low level functions to drive the address mappers
 *    implemented on the IFC1211.
 *
 *----------------------------------------------------------------------------
 *
 * Copyright (c) 2015 IOxOS Technologies SA <ioxos@ioxos.ch>
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 * GPL license :
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 *
 *----------------------------------------------------------------------------
 *  Change History
 *  
 *
 *=============================< end file header >============================*/

#include "tscos.h"
#include "tscdrvr.h"

#define DBGno
#include "debug.h"


#define DMA_NO_PIPE  0
#define DMA_PIPE     1

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * Function name : tsc_dma_irq
 * Prototype     : int
 * Parameters    : pointer to IFC1211 device control structure
 * Return        : error/success
 *----------------------------------------------------------------------------
 * Description   : DMA interrupt handler
 *
 *++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

void
tsc_dma_irq( struct  ifc1211_device *ifc,
	     int src,
	     void *arg)
{
  struct dma_ctl *dma_ctl_p;
  int idx;

  dma_ctl_p = (struct dma_ctl *)arg;
  dma_ctl_p->state = DMA_STS_DONE;
  dma_ctl_p->status |= DMA_STATUS_DONE | (src << 16);
  idx = IFC1211_ALL_ITC_IACK_SRC(src);
  if( (idx == ITC_SRC_DMA_RD0_ERR) ||
      (idx == ITC_SRC_DMA_WR0_ERR) ||
      (idx == ITC_SRC_DMA_RD1_ERR) ||
      (idx == ITC_SRC_DMA_WR1_ERR)    )
  {
    dma_ctl_p->status |= DMA_STATUS_ERR;
  }
  debugk(("DMA #%d IRQ masking -> %08x [%x]\n", dma_ctl_p->chan, dma_ctl_p->irq, dma_ctl_p->status));
  iowrite32( dma_ctl_p->irq, ifc->csr_ptr + IFC1211_CSR_IDMA_ITC_IMS);
  up( &dma_ctl_p->sem);
  return;
}

void
tsc_dma2_irq( struct  ifc1211_device *ifc,
	      int src,
	      void *arg)
{
  struct dma_ctl *dma_ctl_p;
  int idx;

  dma_ctl_p = (struct dma_ctl *)arg;
  dma_ctl_p->state = DMA_STS_DONE;
  dma_ctl_p->status |= DMA_STATUS_DONE | (src << 16);
  idx = IFC1211_ALL_ITC_IACK_SRC(src);
  if( (idx == ITC_SRC_DMA2_RD0_ERR) ||
      (idx == ITC_SRC_DMA2_WR0_ERR) ||
      (idx == ITC_SRC_DMA2_RD1_ERR) ||
      (idx == ITC_SRC_DMA2_WR1_ERR)    )
  {
    dma_ctl_p->status |= DMA_STATUS_ERR;
  }
  debugk(("DMA #%d IRQ masking -> %08x [%x]\n", dma_ctl_p->chan, dma_ctl_p->irq, dma_ctl_p->status));
  iowrite32( dma_ctl_p->irq, ifc->csr_ptr + IFC1211_CSR_IDMA2_ITC_IMS);
  up( &dma_ctl_p->sem);
  return;
}

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * Function name : dma_init
 * Prototype     : int
 * Parameters    : pointer to IFC1211 device control structure
 * Return        : error/success
 *----------------------------------------------------------------------------
 * Description   : DMA channel initialization
 *
 *++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

int
dma_init( struct dma_ctl *dma_ctl_p)
{
  debugk(("in dma_init(): %d\n", dma_ctl_p->chan));
  /* register interrupt hanlers for DMA */
  if( dma_ctl_p->chan == DMA_CHAN_0)
  {
    dma_ctl_p->irq  = IFC1211_IDMA_ITC_IM_RD0_END | IFC1211_IDMA_ITC_IM_RD0_ERR;
    dma_ctl_p->irq |= IFC1211_IDMA_ITC_IM_WR0_END | IFC1211_IDMA_ITC_IM_WR0_ERR;
    tsc_irq_register( dma_ctl_p->ifc, ITC_SRC_DMA_RD0_END, tsc_dma_irq, (void *)dma_ctl_p);
    tsc_irq_register( dma_ctl_p->ifc, ITC_SRC_DMA_RD0_ERR, tsc_dma_irq, (void *)dma_ctl_p);
    tsc_irq_register( dma_ctl_p->ifc, ITC_SRC_DMA_WR0_END, tsc_dma_irq, (void *)dma_ctl_p);
    tsc_irq_register( dma_ctl_p->ifc, ITC_SRC_DMA_WR0_ERR, tsc_dma_irq, (void *)dma_ctl_p);
    iowrite32(IFC1211_IDMA_CSR_ENA, dma_ctl_p->ifc->csr_ptr + IFC1211_CSR_IDMA_RD_0_CSR);
    iowrite32(IFC1211_IDMA_CSR_ENA, dma_ctl_p->ifc->csr_ptr + IFC1211_CSR_IDMA_WR_0_CSR);
  }
  if( dma_ctl_p->chan == DMA_CHAN_1)
  {
    dma_ctl_p->irq  = IFC1211_IDMA_ITC_IM_RD1_END | IFC1211_IDMA_ITC_IM_RD1_ERR;
    dma_ctl_p->irq |= IFC1211_IDMA_ITC_IM_WR1_END | IFC1211_IDMA_ITC_IM_WR1_ERR;
    tsc_irq_register(dma_ctl_p->ifc, ITC_SRC_DMA_RD1_END, tsc_dma_irq, (void *)dma_ctl_p);
    tsc_irq_register(dma_ctl_p->ifc, ITC_SRC_DMA_RD1_ERR, tsc_dma_irq, (void *)dma_ctl_p);
    tsc_irq_register(dma_ctl_p->ifc, ITC_SRC_DMA_WR1_END, tsc_dma_irq, (void *)dma_ctl_p);
    tsc_irq_register(dma_ctl_p->ifc, ITC_SRC_DMA_WR1_ERR, tsc_dma_irq, (void *)dma_ctl_p);
    iowrite32(IFC1211_IDMA_CSR_ENA, dma_ctl_p->ifc->csr_ptr + IFC1211_CSR_IDMA_RD_1_CSR);
    iowrite32(IFC1211_IDMA_CSR_ENA, dma_ctl_p->ifc->csr_ptr + IFC1211_CSR_IDMA_WR_1_CSR);
  }
  if( dma_ctl_p->chan == DMA_CHAN_2)
  {
    dma_ctl_p->irq  = IFC1211_IDMA_ITC_IM_RD0_END | IFC1211_IDMA_ITC_IM_RD0_ERR;
    dma_ctl_p->irq |= IFC1211_IDMA_ITC_IM_WR0_END | IFC1211_IDMA_ITC_IM_WR0_ERR;
    tsc_irq_register( dma_ctl_p->ifc, ITC_SRC_DMA2_RD0_END, tsc_dma2_irq, (void *)dma_ctl_p);
    tsc_irq_register( dma_ctl_p->ifc, ITC_SRC_DMA2_RD0_ERR, tsc_dma2_irq, (void *)dma_ctl_p);
    tsc_irq_register( dma_ctl_p->ifc, ITC_SRC_DMA2_WR0_END, tsc_dma2_irq, (void *)dma_ctl_p);
    tsc_irq_register( dma_ctl_p->ifc, ITC_SRC_DMA2_WR0_ERR, tsc_dma2_irq, (void *)dma_ctl_p);
    iowrite32(IFC1211_IDMA_CSR_ENA, dma_ctl_p->ifc->csr_ptr + IFC1211_CSR_IDMA2_RD_0_CSR);
    iowrite32(IFC1211_IDMA_CSR_ENA, dma_ctl_p->ifc->csr_ptr + IFC1211_CSR_IDMA2_WR_0_CSR);
  }
  if( dma_ctl_p->chan == DMA_CHAN_3)
  {
    dma_ctl_p->irq  = IFC1211_IDMA_ITC_IM_RD1_END | IFC1211_IDMA_ITC_IM_RD1_ERR;
    dma_ctl_p->irq |= IFC1211_IDMA_ITC_IM_WR1_END | IFC1211_IDMA_ITC_IM_WR1_ERR;
    tsc_irq_register(dma_ctl_p->ifc, ITC_SRC_DMA2_RD1_END, tsc_dma2_irq, (void *)dma_ctl_p);
    tsc_irq_register(dma_ctl_p->ifc, ITC_SRC_DMA2_RD1_ERR, tsc_dma2_irq, (void *)dma_ctl_p);
    tsc_irq_register(dma_ctl_p->ifc, ITC_SRC_DMA2_WR1_END, tsc_dma2_irq, (void *)dma_ctl_p);
    tsc_irq_register(dma_ctl_p->ifc, ITC_SRC_DMA2_WR1_ERR, tsc_dma2_irq, (void *)dma_ctl_p);
    iowrite32(IFC1211_IDMA_CSR_ENA, dma_ctl_p->ifc->csr_ptr + IFC1211_CSR_IDMA2_RD_1_CSR);
    iowrite32(IFC1211_IDMA_CSR_ENA, dma_ctl_p->ifc->csr_ptr + IFC1211_CSR_IDMA2_WR_1_CSR);
  }
  dma_ctl_p->wr_mode  = 0;
  dma_ctl_p->rd_mode  = 0;
  mutex_init( &dma_ctl_p->dma_lock);
  sema_init( &dma_ctl_p->sem, 0);
  dma_ctl_p->state = DMA_STS_IDLE;

  return(0);
}

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * Function name : dma_alloc
 * Prototype     : int
 * Parameters    : pointer to IFC1211 device control structure
 * Return        : error/success
 *----------------------------------------------------------------------------
 * Description   : Allocate DMA channel
 *
 *++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

int
dma_alloc( struct dma_ctl *dma_ctl_p)
{
  int retval;

  retval = 0;
  /* protect against concurrent access to queue control structure */
  mutex_lock( &dma_ctl_p->dma_lock);

  if(dma_ctl_p->state != DMA_STS_IDLE)
  {
    retval = -EBUSY;
    goto dma_alloc_exit;
  }
  dma_ctl_p->state = DMA_STS_ALLOCATED;

dma_alloc_exit:
   /* release locking semaphore */
  mutex_unlock( &dma_ctl_p->dma_lock);

  return( retval);
}

int
tsc_dma_alloc( struct ifc1211_device *ifc,
               struct tsc_ioctl_dma *dma_p)
{
  int retval;
  struct dma_ctl *dma_ctl_p;

  if( (dma_p->chan < 0) || (dma_p->chan >= DMA_CHAN_NUM))
  {
    return( -EINVAL);
  }
  dma_ctl_p = ifc->dma_ctl[(int)dma_p->chan];
  retval = dma_alloc( dma_ctl_p);
  dma_p->state = (char)dma_ctl_p->state;
  return( retval);
}


/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * Function name : tsc_dma_free
 * Prototype     : int
 * Parameters    : pointer to IFC1211 device control structure
 * Return        : error/success
 *----------------------------------------------------------------------------
 * Description   : Free DMA channel
 *
 *++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

int
tsc_dma_free( struct ifc1211_device *ifc,
	      struct tsc_ioctl_dma *dma_p)
{
  struct dma_ctl *dma_ctl_p;
  int retval;

  retval = 0;
  if( (dma_p->chan < 0) || (dma_p->chan >= DMA_CHAN_NUM))
  {
    return( -EINVAL);
  }
  dma_ctl_p = ifc->dma_ctl[(int)dma_p->chan];
  if( dma_ctl_p->state == DMA_STS_IDLE)
  {
    goto dma_free_exit;
  }
  if( ( dma_ctl_p->state != DMA_STS_ALLOCATED) &&
      ( dma_ctl_p->state != DMA_STS_DONE)          )
  {
    retval = -EPERM;
    goto dma_free_exit;
  }
  dma_ctl_p->state = DMA_STS_IDLE;

dma_free_exit:
  dma_p->state = (char)dma_ctl_p->state;
  return( retval);
}

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * Function name : tsc_dma_clear
 * Prototype     : int
 * Parameters    : pointer to IFC1211 device control structure
 * Return        : error/success
 *----------------------------------------------------------------------------
 * Description   : Free DMA channel
 *
 *++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

int
tsc_dma_clear( struct ifc1211_device *ifc,
	       struct tsc_ioctl_dma *dma_p)
{
  struct dma_ctl *dma_ctl_p;
  int retval;

  retval = 0;
  if( (dma_p->chan < 0) || (dma_p->chan >= DMA_CHAN_NUM))
  {
    return( -EINVAL);
  }
  dma_ctl_p = ifc->dma_ctl[(int)dma_p->chan];
  if( ( dma_ctl_p->state != DMA_STS_STARTED) &&
      ( dma_ctl_p->state != DMA_STS_WAITING)          )
  {
    retval = -EPERM;
    goto dma_clear_exit;
  }
  if( dma_ctl_p->chan == DMA_CHAN_0)
  {
    iowrite32( IFC1211_IDMA_CSR_KILL, dma_ctl_p->ifc->csr_ptr + IFC1211_CSR_IDMA_RD_0_CSR);
    iowrite32( IFC1211_IDMA_CSR_KILL, dma_ctl_p->ifc->csr_ptr + IFC1211_CSR_IDMA_WR_0_CSR);
    iowrite32( IFC1211_IDMA_CSR_ENA, dma_ctl_p->ifc->csr_ptr + IFC1211_CSR_IDMA_RD_0_CSR);
    iowrite32( IFC1211_IDMA_CSR_ENA, dma_ctl_p->ifc->csr_ptr + IFC1211_CSR_IDMA_WR_0_CSR);
  }
  if( dma_ctl_p->chan == DMA_CHAN_1)
  {
    iowrite32( IFC1211_IDMA_CSR_KILL, dma_ctl_p->ifc->csr_ptr + IFC1211_CSR_IDMA_RD_1_CSR);
    iowrite32( IFC1211_IDMA_CSR_KILL, dma_ctl_p->ifc->csr_ptr + IFC1211_CSR_IDMA_WR_1_CSR);
    iowrite32( IFC1211_IDMA_CSR_ENA, dma_ctl_p->ifc->csr_ptr + IFC1211_CSR_IDMA_RD_1_CSR);
    iowrite32( IFC1211_IDMA_CSR_ENA, dma_ctl_p->ifc->csr_ptr + IFC1211_CSR_IDMA_WR_1_CSR);
  }
  if( dma_ctl_p->chan == DMA_CHAN_2)
  {
    iowrite32( IFC1211_IDMA_CSR_KILL, dma_ctl_p->ifc->csr_ptr + IFC1211_CSR_IDMA2_RD_0_CSR);
    iowrite32( IFC1211_IDMA_CSR_KILL, dma_ctl_p->ifc->csr_ptr + IFC1211_CSR_IDMA2_WR_0_CSR);
    iowrite32( IFC1211_IDMA_CSR_ENA, dma_ctl_p->ifc->csr_ptr + IFC1211_CSR_IDMA2_RD_0_CSR);
    iowrite32( IFC1211_IDMA_CSR_ENA, dma_ctl_p->ifc->csr_ptr + IFC1211_CSR_IDMA2_WR_0_CSR);
  }
  if( dma_ctl_p->chan == DMA_CHAN_3)
  {
    iowrite32( IFC1211_IDMA_CSR_KILL, dma_ctl_p->ifc->csr_ptr + IFC1211_CSR_IDMA2_RD_1_CSR);
    iowrite32( IFC1211_IDMA_CSR_KILL, dma_ctl_p->ifc->csr_ptr + IFC1211_CSR_IDMA2_WR_1_CSR);
    iowrite32( IFC1211_IDMA_CSR_ENA, dma_ctl_p->ifc->csr_ptr + IFC1211_CSR_IDMA2_RD_1_CSR);
    iowrite32( IFC1211_IDMA_CSR_ENA, dma_ctl_p->ifc->csr_ptr + IFC1211_CSR_IDMA2_WR_1_CSR);
  }
  dma_ctl_p->state = DMA_STS_ALLOCATED;

dma_clear_exit:
  dma_p->state = (char)dma_ctl_p->state;
  return( retval);
}

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * Function name : dma_wait
 * Prototype     : int
 * Parameters    : pointer to IFC1211 device control structure
 * Return        : error/success
 *----------------------------------------------------------------------------
 * Description   : Wait for DMA transfer to complete
 *
 *++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

int
dma_wait( struct dma_ctl *dma_ctl_p,
	  int mode)
{
  int tmo, scale, i;
  int retval;

  debugk(("in tsc_dma_wait() : mode = %x\n", mode));
  if( dma_ctl_p->state != DMA_STS_STARTED)
  {
    if( dma_ctl_p->state == DMA_STS_DONE)
    {
      dma_ctl_p->status |= DMA_STATUS_ENDED;
      return( 0);
    }
    debugk(("tsc_dma_wait() : Bad state  %d\n", dma_ctl_p->state));
    return( -EPERM);
  }
  dma_ctl_p->state = DMA_STS_WAITING;
  tmo = ( mode & 0xf0) >> 4;
  if( tmo)
  {
    int jiffies;

    i = ( mode & 0x0e) >> 1;
    scale = 1;
    if( i)
    {
      i -= 1;
      while(i--)
      {
        scale = scale*10;
      }
    }
    jiffies =  msecs_to_jiffies(tmo*scale) + 1;
    retval = down_timeout( &dma_ctl_p->sem, jiffies);
  }
  else
  {
    retval = down_interruptible( &dma_ctl_p->sem);
  }
  if( retval)
  {
    debugk(("DMA wait timeout\n"));
    dma_ctl_p->state = DMA_STS_STARTED;
    dma_ctl_p->status |= DMA_STATUS_TMO;
  }
  else 
  {
    debugk(("DMA IRQ received -> %x [%x]\n", dma_ctl_p->status, dma_ctl_p->state));
    dma_ctl_p->state = DMA_STS_DONE;
    dma_ctl_p->status |= DMA_STATUS_ENDED;
  }
  return(0);
}

int
tsc_dma_wait( struct ifc1211_device *ifc,
	      struct tsc_ioctl_dma_req *dr_p)
{
  int retval;
  int chan;
  struct dma_ctl *dma_ctl_p;

  retval = 0;
  chan =  DMA_START_CHAN_GET(dr_p->start_mode);
  dma_ctl_p = ifc->dma_ctl[chan];
  if( dr_p->wait_mode & DMA_WAIT_INTR)
  {
    retval = dma_wait( dma_ctl_p, dr_p->wait_mode);
  }
  dr_p->dma_status = dma_ctl_p->status;
  return( retval);
}

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * Function name : tsc_dma_move
 * Prototype     : int
 * Parameters    : pointer to IFC1211 device control structure
 * Return        : error/success
 *----------------------------------------------------------------------------
 * Description   : Allocate DMA channel
 *
 *++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

int
dma_set_pipe( struct dma_ctl *dma_ctl_p,
	      int pipe)
{
  int rd_pipe, wr_pipe;

  rd_pipe = 0;
  wr_pipe = 0;
  if( pipe)
  {
    rd_pipe = IFC1211_IDMA_PCSR_PIPE_ENABLE | IFC1211_IDMA_PCSR_PIPE_MODE_2 | IFC1211_IDMA_PCSR_PIPE_RESET;
    wr_pipe = IFC1211_IDMA_PCSR_PIPE_ENABLE | IFC1211_IDMA_PCSR_PIPE_MODE_2 | IFC1211_IDMA_PCSR_PIPE_RESET;
  }
  if( dma_ctl_p->chan == DMA_CHAN_0)
  {
    iowrite32(  rd_pipe, dma_ctl_p->ifc->csr_ptr + IFC1211_CSR_IDMA_RD_0_PCSR);
    iowrite32(  wr_pipe, dma_ctl_p->ifc->csr_ptr + IFC1211_CSR_IDMA_WR_0_PCSR);
  }
  if( dma_ctl_p->chan == DMA_CHAN_1)
  { 
    iowrite32(  rd_pipe, dma_ctl_p->ifc->csr_ptr + IFC1211_CSR_IDMA_RD_1_PCSR);
    iowrite32(  wr_pipe, dma_ctl_p->ifc->csr_ptr + IFC1211_CSR_IDMA_WR_1_PCSR);
  }
  if( dma_ctl_p->chan == DMA_CHAN_2)
  {
    iowrite32(  rd_pipe, dma_ctl_p->ifc->csr_ptr + IFC1211_CSR_IDMA2_RD_0_PCSR);
    iowrite32(  wr_pipe, dma_ctl_p->ifc->csr_ptr + IFC1211_CSR_IDMA2_WR_0_PCSR);
  }
  if( dma_ctl_p->chan == DMA_CHAN_3)
  { 
    iowrite32(  rd_pipe, dma_ctl_p->ifc->csr_ptr + IFC1211_CSR_IDMA2_RD_1_PCSR);
    iowrite32(  wr_pipe, dma_ctl_p->ifc->csr_ptr + IFC1211_CSR_IDMA2_WR_1_PCSR);
  }
  return(0);
}

void 
dma_cpy_desc( struct dma_ctl *dma_ctl_p,
	       int offset,
	       struct dma_desc *dd)
{
  int i, *p;

  p = (int *)dd;
  debugk(("in dma_cpy_desc() : %p %x\n", dma_ctl_p->desc_ptr, offset));
  for( i = 0; i < sizeof(struct dma_desc); i += sizeof(int))
  {
    iowrite32( *p++, dma_ctl_p->desc_ptr + offset + i);
  }
}

void 
dma_get_desc( struct dma_ctl *dma_ctl_p,
	       int offset,
	       struct dma_desc *dd)
{
  int i, *p;

  p = (int *)dd;

  for( i = 0; i < sizeof(struct dma_desc); i += sizeof(int))
  {
    *p++ = ioread32( dma_ctl_p->desc_ptr + offset + i);
  }
}

static uint 
dma_set_ctl( uint space,
	     uint trig,
	     uint intr,
	     uint mode)
{
  uint ctl;

  debugk(("in dma_set_ctl( %x, %x, %x, %x)...", space, trig, intr, mode));
  ctl = IFC1211_IDMA_DES0_UPDATE_TIME | trig | intr;
  ctl |= mode & ( DMA_MODE_ADD_MASK | DMA_MODE_SHM_MASK | DMA_MODE_SNOOP | DMA_MODE_RELAX);
  ctl |= (mode & DMA_MODE_WR_POST_MASK) << 12;


  if( (space & DMA_SPACE_MASK) == DMA_SPACE_PCIE)
  { 
    //ctl |= IFC1211_IDMA_DES0_PSIZE_512 | IFC1211_IDMA_DES0_RELAX;
    ctl |= IFC1211_IDMA_DES0_PSIZE_512;
  }



  if( (space & DMA_SPACE_MASK) == DMA_SPACE_USR) 
  {
    ctl |= (mode & 0xc000) << 4;
  }

  debugk(("%x\n", ctl));
  return( ctl);
}

int
dma_set_rd_desc( struct dma_ctl *dma_ctl_p,
		 uint  shm_addr, 
		 dma_addr_t des_addr, 
		 uint size,
		 unsigned char space,
		 uint mode)
{
  struct dma_ctl *dc;
  struct dma_desc *dd;
  uint next, trig, intr;
  long long tmp;

  debugk(("in dma_set_rd_desc( %d : %x, %llx, %x, %x)...\n", dma_ctl_p->chan, shm_addr, (long long)des_addr, size, space));
  dc = (struct dma_ctl *)dma_ctl_p;
  dd = (struct dma_desc *)&dc->rd_desc;

  next = IFC1211_IDMA_DES3_START_NOW | IFC1211_IDMA_DES3_LAST;
  trig = IFC1211_IDMA_DES0_TRIGOUT_NO;
  intr = IFC1211_IDMA_DES0_INTR_END;

  /* prepare transfer descriptor */
  dd->ctl = dma_set_ctl( space, trig, intr, dc->rd_mode);
  dd->wcnt = (uint)space << 24 | (size & IFC1211_IDMA_DES1_WC_MASK);
  dd->shm_addr = (shm_addr & IFC1211_IDMA_DES2_ADDR_MASK) | 3;      /* SHM local buffer */
  dd->next = next;                   
  dd->rem_addr_l = (uint)des_addr;                               /* des address bit 0:31         */
  tmp = (long long)des_addr;                                     /* if des address  is 64 bit    */
  dd->rem_addr_h = (uint)(tmp >> 32);                            /* get bit 48:64                */
  dd->status = 0;
  dd->time_msec = 0;

  /* copy transfer descriptor in SHM */
  dma_cpy_desc( dc, 0, dd);

  return( dc->desc_offset);
}

int
dma_set_wr_desc( struct dma_ctl *dma_ctl_p,
		 uint  shm_addr, 
		 dma_addr_t src_addr, 
		 uint size,
		 unsigned char space,
		 uint mode)
{
  struct dma_ctl *dc;
  struct dma_desc *dd;
  uint next, trig, intr;
  long long tmp;

  debugk(("in dma_set_wr_desc( %d : %x, %llx, %x, %x)...\n", dma_ctl_p->chan, shm_addr, (long long)src_addr, size, space));
  dc = (struct dma_ctl *)dma_ctl_p;
  dd = (struct dma_desc *)&dc->wr_desc;

  next = IFC1211_IDMA_DES3_START_NOW | IFC1211_IDMA_DES3_LAST;
  trig = IFC1211_IDMA_DES0_TRIGOUT_NO;
  intr = IFC1211_IDMA_DES0_INTR_ERR;

  /* prepare transfer descriptor */
  dd->ctl = dma_set_ctl( space, trig, intr, dc->wr_mode);
  dd->wcnt = (uint)space << 24 | (size & IFC1211_IDMA_DES1_WC_MASK);
  dd->shm_addr = (shm_addr & IFC1211_IDMA_DES2_ADDR_MASK) | 3;      /* SHM local buffer */
  dd->next = next;                   
  dd->rem_addr_l = (uint)src_addr;                               /* des address bit 0:31         */
  tmp = (long long)src_addr;                                     /* if des address  is 64 bit    */
  dd->rem_addr_h = (uint)(tmp >> 32);                            /* get bit 48:64                */
  dd->status = 0;
  dd->time_msec = 0;

  /* copy transfer descriptor in SHM */
  dma_cpy_desc( dc, sizeof( struct dma_desc), dd);

  return(dc->desc_offset + sizeof( struct dma_desc));
}


int
tsc_dma_move( struct ifc1211_device *ifc,
	      struct tsc_ioctl_dma_req *dr_p)
{
  struct dma_ctl *dma_ctl_p;
  int retval;
  int chan;
  int pipe;
  int rdo, wro;
  int csr_rdo, csr_wro;
  int sts_rdo, sts_wro;
  int irq, csr_itc_imc;
  uint shm_offset;
  unsigned char space_shm;

  debugk(("in tsc_dma_move() : %llx:%x %llx:%x %x %x %x\n",
	  (long long)dr_p->des_addr, dr_p->des_space,
	  (long long)dr_p->src_addr, dr_p->src_space,
	  dr_p->size, dr_p->des_mode, dr_p->src_mode));

  retval = 0;
  chan =  DMA_START_CHAN_GET(dr_p->start_mode);
  if(chan == DMA_CHAN_0)
  {
    chan = DMA_CHAN_0;
    csr_rdo = IFC1211_CSR_IDMA_RD_0_NDES;
    csr_wro = IFC1211_CSR_IDMA_WR_0_NDES;
    sts_rdo = DMA_STATUS_RUN_RD0;
    sts_wro = DMA_STATUS_RUN_WR0;
    csr_itc_imc = IFC1211_CSR_IDMA_ITC_IMC;
    space_shm = DMA_SPACE_SHM;
  }
  if(chan == DMA_CHAN_1)
  {
    csr_rdo = IFC1211_CSR_IDMA_RD_1_NDES;
    csr_wro = IFC1211_CSR_IDMA_WR_1_NDES;
    sts_rdo = DMA_STATUS_RUN_RD1;
    sts_wro = DMA_STATUS_RUN_WR1;
    csr_itc_imc = IFC1211_CSR_IDMA_ITC_IMC;
    space_shm = DMA_SPACE_SHM;
  }
  if(chan == DMA_CHAN_2)
  {
    csr_rdo = IFC1211_CSR_IDMA2_RD_0_NDES;
    csr_wro = IFC1211_CSR_IDMA2_WR_0_NDES;
    sts_rdo = DMA_STATUS_RUN_RD0;
    sts_wro = DMA_STATUS_RUN_WR0;
    csr_itc_imc = IFC1211_CSR_IDMA2_ITC_IMC;
    space_shm = DMA_SPACE_SHM2;
  }
  if(chan == DMA_CHAN_3)
  {
    csr_rdo = IFC1211_CSR_IDMA2_RD_1_NDES;
    csr_wro = IFC1211_CSR_IDMA2_WR_1_NDES;
    sts_rdo = DMA_STATUS_RUN_RD1;
    sts_wro = DMA_STATUS_RUN_WR1;
    csr_itc_imc = IFC1211_CSR_IDMA2_ITC_IMC;
    space_shm = DMA_SPACE_SHM2;
  }
  dma_ctl_p = ifc->dma_ctl[chan];
  if( ( dma_ctl_p->state != DMA_STS_ALLOCATED) &&
      ( dma_ctl_p->state != DMA_STS_DONE)          )
  {
    return( -EPERM);
  }
  irq = dma_ctl_p->irq;
  if( ( dr_p->src_space & DMA_SPACE_MASK) ==  space_shm)
  {
    if( ( dr_p->des_space & DMA_SPACE_MASK) ==  space_shm)
    {
      return( -EINVAL);
    }
    else
    {
      pipe = DMA_NO_PIPE;
      wro = 0;
      shm_offset = (uint)dr_p->src_addr;
      rdo = dma_set_rd_desc( dma_ctl_p, shm_offset, dr_p->des_addr, dr_p->size, dr_p->des_space, dr_p->des_mode);
    }
  }
  else
  {
    if( ( dr_p->des_space & DMA_SPACE_MASK) ==  space_shm)
    {
      pipe = DMA_NO_PIPE;
      rdo = 0;
      shm_offset = (uint)dr_p->des_addr;
      wro = dma_set_wr_desc( dma_ctl_p, shm_offset, dr_p->src_addr, dr_p->size, dr_p->src_space, dr_p->src_mode);
    }
    else
    {
      pipe = DMA_PIPE;
      rdo = dma_set_rd_desc( dma_ctl_p, dma_ctl_p->ring_offset, dr_p->des_addr, dr_p->size, dr_p->des_space, dr_p->des_mode);
      wro = dma_set_wr_desc( dma_ctl_p, dma_ctl_p->ring_offset, dr_p->src_addr, dr_p->size, dr_p->src_space, dr_p->src_mode);
      if( (chan == DMA_CHAN_0) || (chan == DMA_CHAN_2))
      {
	irq &= ~IFC1211_IDMA_ITC_IM_WR0_END;
      }
      else
      {
	irq &= ~IFC1211_IDMA_ITC_IM_WR1_END;
      }
    }
  }
  dma_set_pipe( dma_ctl_p, pipe);

  /* reset synchronization semaphore */
  sema_init( &dma_ctl_p->sem, 0);

  /* clear transfer status */
  dma_ctl_p->status = 0;

  /* unmask DMA  interrupts */
  debugk(("DMA IRQ unmasking -> %08x\n", irq));
  iowrite32( irq, ifc->csr_ptr + csr_itc_imc);
  debugk(("rdo = %x - wro = %x\n", rdo, wro));
  if( rdo)
  {
    int ctl;

    dma_ctl_p->status |= sts_rdo;
    ctl = IFC1211_IDMA_CSR_ENA;
    if( dma_ctl_p->rd_mode & DMA_MODE_CACHE_ENA)
    {
      ctl |= IFC1211_IDMA_CSR_CACHE_ENA;
    }
    iowrite32( ctl, ifc->csr_ptr + csr_rdo - 4);
    iowrite32( rdo, ifc->csr_ptr + csr_rdo);
  }
  if( wro)
  {
    int ctl;

    dma_ctl_p->status |= sts_wro;
    ctl = IFC1211_IDMA_CSR_ENA;
    if( dma_ctl_p->wr_mode & DMA_MODE_CACHE_ENA)
    {
      ctl |= IFC1211_IDMA_CSR_CACHE_ENA;
    }
    iowrite32( ctl, ifc->csr_ptr + csr_wro - 4);
    iowrite32( wro, ifc->csr_ptr + csr_wro);
  }
  dma_ctl_p->state = DMA_STS_STARTED;
  if( dr_p->wait_mode & DMA_WAIT_INTR)
  {
    retval = dma_wait( dma_ctl_p, dr_p->wait_mode);
  }
  dr_p->dma_status = dma_ctl_p->status;

  return( retval);
}

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * Function name : tsc_dma_status
 * Prototype     : int
 * Parameters    : pointer to IFC1211 device control structure
 * Return        : error/success
 *----------------------------------------------------------------------------
 * Description   : Get DMA channel status
 *
 *++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

int
dma_status( struct dma_ctl *dma_ctl_p,
	    struct tsc_ioctl_dma_sts *ds_p)

{
  int retval;
  int i, *p;
  int rdo, wro;

  retval = 0;
  /* protect against concurrent access to queue control structure */

  debugk(("in dma_status()\n")); 
  mutex_lock( &dma_ctl_p->dma_lock);
  if( ds_p->dma.chan == DMA_CHAN_0)
  {
    rdo = IFC1211_CSR_IDMA_RD_0_CSR;
    wro = IFC1211_CSR_IDMA_WR_0_CSR;
  }
  else if( ds_p->dma.chan == DMA_CHAN_1)
  {
    rdo = IFC1211_CSR_IDMA_RD_1_CSR;
    wro = IFC1211_CSR_IDMA_WR_1_CSR;
  }
  else if( ds_p->dma.chan == DMA_CHAN_2)
  {
    rdo = IFC1211_CSR_IDMA2_RD_0_CSR;
    wro = IFC1211_CSR_IDMA2_WR_0_CSR;
  }
  else if( ds_p->dma.chan == DMA_CHAN_3)
  {
    rdo = IFC1211_CSR_IDMA2_RD_1_CSR;
    wro = IFC1211_CSR_IDMA2_WR_1_CSR;
  }
  else
  {
    retval = -EINVAL;
    ds_p->dma.status = -1;
    goto dma_status_exit;
  }
  ds_p->dma.status = (short)dma_ctl_p->status;
  ds_p->dma.state =  (char)dma_ctl_p->state;
  p = (int *)(&ds_p->rd_csr);
  for( i = 0; i < 16; i += sizeof(int))
  {
    *p++ = ioread32( dma_ctl_p->ifc->csr_ptr + rdo + i);
  }
  for( i = 0; i < 16; i += sizeof(int))
  {
    *p++ = ioread32( dma_ctl_p->ifc->csr_ptr + wro + i);
  }

dma_status_exit:
  /* release locking semaphore */
  mutex_unlock( &dma_ctl_p->dma_lock);

  return( retval);
}

int
tsc_dma_status( struct ifc1211_device *ifc,
	        struct tsc_ioctl_dma_sts *ds_p)
{
  int retval;
  struct dma_ctl *dma_ctl_p;

  if( (ds_p->dma.chan < 0) || (ds_p->dma.chan >= DMA_CHAN_NUM))
  {
    return( -EINVAL);
  }
  dma_ctl_p = ifc->dma_ctl[(int)ds_p->dma.chan];
  retval = dma_status( dma_ctl_p, ds_p);
  return( retval);
}

int
tsc_dma_mode( struct ifc1211_device *ifc,
	      struct tsc_ioctl_dma_mode *dm_p)
{
  struct dma_ctl *dma_ctl_p;

  if( (dm_p->chan < 0) || (dm_p->chan >= DMA_CHAN_NUM))
  {
    return( -EINVAL);
  }
  dma_ctl_p = ifc->dma_ctl[(int)dm_p->chan];
  if(dm_p->op & DMA_MODE_SET)
  {
    dma_ctl_p->wr_mode = dm_p->wr_mode;
    dma_ctl_p->rd_mode = dm_p->rd_mode;
  }
  dm_p->wr_mode = dma_ctl_p->wr_mode;
  dm_p->rd_mode = dma_ctl_p->rd_mode;

  return( 0);
}

