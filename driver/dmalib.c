/*=========================< begin file & file header >=======================
 *  References
 *  
 *    filename : dmalib.c
 *    author   : JFG, XP
 *    company  : IOxOS
 *    creation : Sept 27,2015
 *    version  : 0.0.1
 *
 *----------------------------------------------------------------------------
 *  Description
 *
 *    This file contains the low level functions to drive the DMA
 *    implemented on the TSC project.
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

#include "tscos.h"
#include "tscdrvr.h"

#define DBGno
#include "debug.h"

#define DMA_NO_PIPE  0
#define DMA_PIPE     1

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * Function name : tsc_dma_irq
 * Prototype     : void
 * Parameters    : pointer to tsc device control structure, source, argument
 * Return        : void
 *----------------------------------------------------------------------------
 * Description   : dma interrupt handler
 *
 *++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

void
tsc_dma_irq( struct  tsc_device *ifc,
	     int src,
	     void *arg)
{
  struct dma_ctl *dma_ctl_p;
  int idx;
  int base;

  dma_ctl_p = (struct dma_ctl *)arg;
  dma_ctl_p->state = DMA_STATE_DONE;
  dma_ctl_p->status |= DMA_STATUS_DONE | (src << 16);
  idx = TSC_ALL_ITC_IACK_SRC(src);
  base = TSC_ITC_IACK_BASE(src);

  switch (dma_ctl_p->chan)
  {
    case DMA_CHAN_0:
    case DMA_CHAN_1:
      if ((idx == ITC_SRC_DMA_RD0_ERR) ||
          (idx == ITC_SRC_DMA_WR0_ERR) ||
          (idx == ITC_SRC_DMA_RD1_ERR) ||
          (idx == ITC_SRC_DMA_WR1_ERR))
      {
        dma_ctl_p->status |= DMA_STATUS_ERR;
      }
      break;
    case DMA_CHAN_2:
    case DMA_CHAN_3:
      if ((idx == ITC_SRC_DMA2_RD0_ERR) ||
          (idx == ITC_SRC_DMA2_WR0_ERR) ||
          (idx == ITC_SRC_DMA2_RD1_ERR) ||
          (idx == ITC_SRC_DMA2_WR1_ERR))
      {
        dma_ctl_p->status |= DMA_STATUS_ERR;
      }
      break;
  }

  if (dma_ctl_p->sgt)
  {
    dma_unmap_sg(&ifc->pdev->dev, dma_ctl_p->sgt->sgl, dma_ctl_p->sgt->nents, dma_ctl_p->dir);
    sg_free_table(dma_ctl_p->sgt);
  }

  debugk(("DMA #%d IRQ masking -> %08x [%x]\n", dma_ctl_p->chan, dma_ctl_p->irq, dma_ctl_p->status));
  iowrite32( dma_ctl_p->irq, ifc->csr_ptr + base + TSC_CSR_ILOC_ITC_IMS);
  up( &dma_ctl_p->sem);
  return;
}

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * Function name : dma_init
 * Prototype     : int
 * Parameters    : pointer to dma control structure
 * Return        : success
 *----------------------------------------------------------------------------
 * Description   : dma channel initialization
 *
 *++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

int
dma_init( struct dma_ctl *dma_ctl_p)
{
  debugk(("in dma_init(): %d\n", dma_ctl_p->chan));
  /* register interrupt hanlers for DMA */
  if( dma_ctl_p->chan == DMA_CHAN_0)
  {
    dma_ctl_p->irq  = TSC_IDMA_ITC_IM_RD0_END | TSC_IDMA_ITC_IM_RD0_ERR;
    dma_ctl_p->irq |= TSC_IDMA_ITC_IM_WR0_END | TSC_IDMA_ITC_IM_WR0_ERR;
    tsc_irq_register(dma_ctl_p->ifc, ITC_CTL(ITC_SRC_DMA_RD0_END), ITC_SRC_DMA_RD0_END & 0xf, tsc_dma_irq, (void *)dma_ctl_p);
    tsc_irq_register(dma_ctl_p->ifc, ITC_CTL(ITC_SRC_DMA_RD0_ERR), ITC_SRC_DMA_RD0_ERR & 0xf, tsc_dma_irq, (void *)dma_ctl_p);
    tsc_irq_register(dma_ctl_p->ifc, ITC_CTL(ITC_SRC_DMA_WR0_END), ITC_SRC_DMA_WR0_END & 0xf, tsc_dma_irq, (void *)dma_ctl_p);
    tsc_irq_register(dma_ctl_p->ifc, ITC_CTL(ITC_SRC_DMA_WR0_ERR), ITC_SRC_DMA_WR0_ERR & 0xf, tsc_dma_irq, (void *)dma_ctl_p);
    iowrite32(TSC_IDMA_CSR_ENA, dma_ctl_p->ifc->csr_ptr + TSC_CSR_IDMA_RD_0_CSR);
    iowrite32(TSC_IDMA_CSR_ENA, dma_ctl_p->ifc->csr_ptr + TSC_CSR_IDMA_WR_0_CSR);
  }
  if( dma_ctl_p->chan == DMA_CHAN_1)
  {
    dma_ctl_p->irq  = TSC_IDMA_ITC_IM_RD1_END | TSC_IDMA_ITC_IM_RD1_ERR;
    dma_ctl_p->irq |= TSC_IDMA_ITC_IM_WR1_END | TSC_IDMA_ITC_IM_WR1_ERR;
    tsc_irq_register(dma_ctl_p->ifc, ITC_CTL(ITC_SRC_DMA_RD1_END), ITC_SRC_DMA_RD1_END & 0xf, tsc_dma_irq, (void *)dma_ctl_p);
    tsc_irq_register(dma_ctl_p->ifc, ITC_CTL(ITC_SRC_DMA_RD1_ERR), ITC_SRC_DMA_RD1_ERR & 0xf, tsc_dma_irq, (void *)dma_ctl_p);
    tsc_irq_register(dma_ctl_p->ifc, ITC_CTL(ITC_SRC_DMA_WR1_END), ITC_SRC_DMA_WR1_END & 0xf, tsc_dma_irq, (void *)dma_ctl_p);
    tsc_irq_register(dma_ctl_p->ifc, ITC_CTL(ITC_SRC_DMA_WR1_ERR), ITC_SRC_DMA_WR1_ERR & 0xf, tsc_dma_irq, (void *)dma_ctl_p);
    iowrite32(TSC_IDMA_CSR_ENA, dma_ctl_p->ifc->csr_ptr + TSC_CSR_IDMA_RD_1_CSR);
    iowrite32(TSC_IDMA_CSR_ENA, dma_ctl_p->ifc->csr_ptr + TSC_CSR_IDMA_WR_1_CSR);
  }
  if( dma_ctl_p->chan == DMA_CHAN_2)
  {
    dma_ctl_p->irq  = TSC_IDMA_ITC_IM_RD0_END | TSC_IDMA_ITC_IM_RD0_ERR;
    dma_ctl_p->irq |= TSC_IDMA_ITC_IM_WR0_END | TSC_IDMA_ITC_IM_WR0_ERR;
    tsc_irq_register(dma_ctl_p->ifc, ITC_CTL(ITC_SRC_DMA2_RD0_END), ITC_SRC_DMA2_RD0_END & 0xf, tsc_dma_irq, (void *)dma_ctl_p);
    tsc_irq_register(dma_ctl_p->ifc, ITC_CTL(ITC_SRC_DMA2_RD0_ERR), ITC_SRC_DMA2_RD0_ERR & 0xf, tsc_dma_irq, (void *)dma_ctl_p);
    tsc_irq_register(dma_ctl_p->ifc, ITC_CTL(ITC_SRC_DMA2_WR0_END), ITC_SRC_DMA2_WR0_END & 0xf, tsc_dma_irq, (void *)dma_ctl_p);
    tsc_irq_register(dma_ctl_p->ifc, ITC_CTL(ITC_SRC_DMA2_WR0_ERR), ITC_SRC_DMA2_WR0_ERR & 0xf, tsc_dma_irq, (void *)dma_ctl_p);
    iowrite32(TSC_IDMA_CSR_ENA, dma_ctl_p->ifc->csr_ptr + TSC_CSR_IDMA2_RD_0_CSR);
    iowrite32(TSC_IDMA_CSR_ENA, dma_ctl_p->ifc->csr_ptr + TSC_CSR_IDMA2_WR_0_CSR);
  }
  if( dma_ctl_p->chan == DMA_CHAN_3)
  {
    dma_ctl_p->irq  = TSC_IDMA_ITC_IM_RD1_END | TSC_IDMA_ITC_IM_RD1_ERR;
    dma_ctl_p->irq |= TSC_IDMA_ITC_IM_WR1_END | TSC_IDMA_ITC_IM_WR1_ERR;
    tsc_irq_register(dma_ctl_p->ifc, ITC_CTL(ITC_SRC_DMA2_RD1_END), ITC_SRC_DMA2_RD1_END & 0xf, tsc_dma_irq, (void *)dma_ctl_p);
    tsc_irq_register(dma_ctl_p->ifc, ITC_CTL(ITC_SRC_DMA2_RD1_ERR), ITC_SRC_DMA2_RD1_ERR & 0xf, tsc_dma_irq, (void *)dma_ctl_p);
    tsc_irq_register(dma_ctl_p->ifc, ITC_CTL(ITC_SRC_DMA2_WR1_END), ITC_SRC_DMA2_WR1_END & 0xf, tsc_dma_irq, (void *)dma_ctl_p);
    tsc_irq_register(dma_ctl_p->ifc, ITC_CTL(ITC_SRC_DMA2_WR1_ERR), ITC_SRC_DMA2_WR1_ERR & 0xf, tsc_dma_irq, (void *)dma_ctl_p);
    iowrite32(TSC_IDMA_CSR_ENA, dma_ctl_p->ifc->csr_ptr + TSC_CSR_IDMA2_RD_1_CSR);
    iowrite32(TSC_IDMA_CSR_ENA, dma_ctl_p->ifc->csr_ptr + TSC_CSR_IDMA2_WR_1_CSR);
  }
  dma_ctl_p->mode  = 0;
  mutex_init( &dma_ctl_p->dma_lock);
  sema_init( &dma_ctl_p->sem, 0);
  dma_ctl_p->state = DMA_STATE_IDLE;

  return 0;
}

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * Function name : dma_alloc
 * Prototype     : int
 * Parameters    : pointer to dma control structure
 * Return        : error/success
 *----------------------------------------------------------------------------
 * Description   : allocate DMA channel
 *
 *++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

int
dma_alloc( struct dma_ctl *dma_ctl_p)
{
  int retval;

  retval = 0;
  /* protect against concurrent access to queue control structure */
  mutex_lock( &dma_ctl_p->dma_lock);

  if(dma_ctl_p->state != DMA_STATE_IDLE)
  {
    retval = -EBUSY;
    goto dma_alloc_exit;
  }
  dma_ctl_p->state = DMA_STATE_ALLOCATED;

dma_alloc_exit:
   /* release locking semaphore */
  mutex_unlock( &dma_ctl_p->dma_lock);

  return( retval);
}

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * Function name : tsc_dma_alloc
 * Prototype     : int
 * Parameters    : pointer to dma control structure
 * Return        : error/success
 *----------------------------------------------------------------------------
 * Description   : allocate DMA channel
 *
 *++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

int
tsc_dma_alloc( struct tsc_device *ifc,
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
 * Function name : dma_free
 * Prototype     : int
 * Parameters    : pointer to dma control structure
 * Return        : error/success
 *----------------------------------------------------------------------------
 * Description   : free DMA channel
 *
 *++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

int
dma_free(struct dma_ctl *dma_ctl_p)
{
  int retval = 0;

  /* protect against concurrent access to queue control structure */
  mutex_lock(&dma_ctl_p->dma_lock);

  if(dma_ctl_p->state == DMA_STATE_IDLE)
  {
    goto dma_free_exit;
  }
  if((dma_ctl_p->state != DMA_STATE_ALLOCATED) &&
     (dma_ctl_p->state != DMA_STATE_DONE))
  {
    retval = -EPERM;
    goto dma_free_exit;
  }
  dma_ctl_p->state = DMA_STATE_IDLE;

dma_free_exit:
  /* release locking semaphore */
  mutex_unlock(&dma_ctl_p->dma_lock);

  return retval;
}

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * Function name : tsc_dma_free
 * Prototype     : int
 * Parameters    : pointer to tsc device control structure, dma structure
 * Return        : error/success
 *----------------------------------------------------------------------------
 * Description   : free DMA channel
 *
 *++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

int
tsc_dma_free(struct tsc_device *ifc,
             struct tsc_ioctl_dma *dma_p)
{
  struct dma_ctl *dma_ctl_p;
  int retval = 0;

  if((dma_p->chan < 0) || (dma_p->chan >= DMA_CHAN_NUM))
  {
    return -EINVAL;
  }

  dma_ctl_p = ifc->dma_ctl[(int)dma_p->chan];
  retval = dma_free(dma_ctl_p);
  dma_p->state = (char)dma_ctl_p->state;
  return retval;
}

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * Function name : dma_clear
 * Prototype     : int
 * Parameters    : pointer to dma control structure
 * Return        : error/success
 *----------------------------------------------------------------------------
 * Description   : clear DMA channel
 *
 *++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

int
dma_clear(struct dma_ctl *dma_ctl_p)
{
  int retval = 0;

  /* protect against concurrent access to queue control structure */
  mutex_lock(&dma_ctl_p->dma_lock);

  if( ( dma_ctl_p->state != DMA_STATE_STARTED) &&
      ( dma_ctl_p->state != DMA_STATE_WAITING)          )
  {
    retval = -EPERM;
    goto dma_clear_exit;
  }
  if( dma_ctl_p->chan == DMA_CHAN_0)
  {
    iowrite32( TSC_IDMA_CSR_KILL, dma_ctl_p->ifc->csr_ptr + TSC_CSR_IDMA_RD_0_CSR);
    iowrite32( TSC_IDMA_CSR_KILL, dma_ctl_p->ifc->csr_ptr + TSC_CSR_IDMA_WR_0_CSR);
    iowrite32( TSC_IDMA_CSR_ENA, dma_ctl_p->ifc->csr_ptr + TSC_CSR_IDMA_RD_0_CSR);
    iowrite32( TSC_IDMA_CSR_ENA, dma_ctl_p->ifc->csr_ptr + TSC_CSR_IDMA_WR_0_CSR);
  }
  if( dma_ctl_p->chan == DMA_CHAN_1)
  {
    iowrite32( TSC_IDMA_CSR_KILL, dma_ctl_p->ifc->csr_ptr + TSC_CSR_IDMA_RD_1_CSR);
    iowrite32( TSC_IDMA_CSR_KILL, dma_ctl_p->ifc->csr_ptr + TSC_CSR_IDMA_WR_1_CSR);
    iowrite32( TSC_IDMA_CSR_ENA, dma_ctl_p->ifc->csr_ptr + TSC_CSR_IDMA_RD_1_CSR);
    iowrite32( TSC_IDMA_CSR_ENA, dma_ctl_p->ifc->csr_ptr + TSC_CSR_IDMA_WR_1_CSR);
  }
  if( dma_ctl_p->chan == DMA_CHAN_2)
  {
    iowrite32( TSC_IDMA_CSR_KILL, dma_ctl_p->ifc->csr_ptr + TSC_CSR_IDMA2_RD_0_CSR);
    iowrite32( TSC_IDMA_CSR_KILL, dma_ctl_p->ifc->csr_ptr + TSC_CSR_IDMA2_WR_0_CSR);
    iowrite32( TSC_IDMA_CSR_ENA, dma_ctl_p->ifc->csr_ptr + TSC_CSR_IDMA2_RD_0_CSR);
    iowrite32( TSC_IDMA_CSR_ENA, dma_ctl_p->ifc->csr_ptr + TSC_CSR_IDMA2_WR_0_CSR);
  }
  if( dma_ctl_p->chan == DMA_CHAN_3)
  {
    iowrite32( TSC_IDMA_CSR_KILL, dma_ctl_p->ifc->csr_ptr + TSC_CSR_IDMA2_RD_1_CSR);
    iowrite32( TSC_IDMA_CSR_KILL, dma_ctl_p->ifc->csr_ptr + TSC_CSR_IDMA2_WR_1_CSR);
    iowrite32( TSC_IDMA_CSR_ENA, dma_ctl_p->ifc->csr_ptr + TSC_CSR_IDMA2_RD_1_CSR);
    iowrite32( TSC_IDMA_CSR_ENA, dma_ctl_p->ifc->csr_ptr + TSC_CSR_IDMA2_WR_1_CSR);
  }
  dma_ctl_p->state = DMA_STATE_ALLOCATED;

dma_clear_exit:
  /* release locking semaphore */
  mutex_unlock(&dma_ctl_p->dma_lock);

  return retval;
}

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * Function name : tsc_dma_clear
 * Prototype     : int
 * Parameters    : pointer to tsc device control structure, dma structure
 * Return        : error/success
 *----------------------------------------------------------------------------
 * Description   : clear dma channel
 *
 *++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

int
tsc_dma_clear(struct tsc_device *ifc,
              struct tsc_ioctl_dma *dma_p)
{
  struct dma_ctl *dma_ctl_p;
  int retval = 0;

  if((dma_p->chan < 0) || (dma_p->chan >= DMA_CHAN_NUM))
  {
    return -EINVAL;
  }
  dma_ctl_p = ifc->dma_ctl[(int)dma_p->chan];
  retval = dma_clear(dma_ctl_p);
  dma_p->state = (char)dma_ctl_p->state;
  return retval;
}

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * Function name : dma_wait
 * Prototype     : int
 * Parameters    : pointer to tsc dma control structure, mode
 * Return        : error/success
 *----------------------------------------------------------------------------
 * Description   : wait for dma transfer to complete
 *
 *++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

int
dma_wait( struct dma_ctl *dma_ctl_p,
	  int mode)
{
  int tmo, scale, i;
  int retval;

  debugk(("in tsc_dma_wait() : mode = %x\n", mode));
  /* protect against concurrent access to queue control structure */
  mutex_lock(&dma_ctl_p->dma_lock);
  if( dma_ctl_p->state != DMA_STATE_STARTED)
  {
    if( dma_ctl_p->state == DMA_STATE_DONE)
    {
      dma_ctl_p->status |= DMA_STATUS_ENDED;
      mutex_unlock(&dma_ctl_p->dma_lock);
      return( 0);
    }
    debugk(("tsc_dma_wait() : Bad state  %d\n", dma_ctl_p->state));
    mutex_unlock(&dma_ctl_p->dma_lock);
    return( -EPERM);
  }
  dma_ctl_p->state = DMA_STATE_WAITING;
  /* release locking semaphore */
  mutex_unlock(&dma_ctl_p->dma_lock);

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

  /* protect against concurrent access to queue control structure */
  mutex_lock(&dma_ctl_p->dma_lock);
  if( retval)
  {
    debugk(("DMA wait timeout\n"));
    dma_ctl_p->state = DMA_STATE_STARTED;
    dma_ctl_p->status |= DMA_STATUS_TMO;
  }
  else 
  {
    debugk(("DMA IRQ received -> %x [%x]\n", dma_ctl_p->status, dma_ctl_p->state));
    dma_ctl_p->state = DMA_STATE_DONE;
    dma_ctl_p->status |= DMA_STATUS_ENDED;
  }
  /* release locking semaphore */
  mutex_unlock(&dma_ctl_p->dma_lock);

  return retval;
}

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * Function name : tsc_dma_wait
 * Prototype     : int
 * Parameters    : pointer to dma control structure, dma request
 * Return        : success
 *----------------------------------------------------------------------------
 * Description   : wait for dma transfer to complete
 *
 *++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

int
tsc_dma_wait( struct tsc_device *ifc,
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
 * Function name : dma_set_pipe
 * Prototype     : int
 * Parameters    : pointer to dma control structure, pipe
 * Return        : success
 *----------------------------------------------------------------------------
 * Description   : set pipe for dma
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
    rd_pipe = TSC_IDMA_PCSR_PIPE_ENABLE | TSC_IDMA_PCSR_PIPE_MODE_2 | TSC_IDMA_PCSR_PIPE_RESET;
    wr_pipe = TSC_IDMA_PCSR_PIPE_ENABLE | TSC_IDMA_PCSR_PIPE_MODE_2 | TSC_IDMA_PCSR_PIPE_RESET;
  }
  if( dma_ctl_p->chan == DMA_CHAN_0)
  {
    iowrite32(  rd_pipe, dma_ctl_p->ifc->csr_ptr + TSC_CSR_IDMA_RD_0_PCSR);
    iowrite32(  wr_pipe, dma_ctl_p->ifc->csr_ptr + TSC_CSR_IDMA_WR_0_PCSR);
  }
  if( dma_ctl_p->chan == DMA_CHAN_1)
  { 
    iowrite32(  rd_pipe, dma_ctl_p->ifc->csr_ptr + TSC_CSR_IDMA_RD_1_PCSR);
    iowrite32(  wr_pipe, dma_ctl_p->ifc->csr_ptr + TSC_CSR_IDMA_WR_1_PCSR);
  }
  if( dma_ctl_p->chan == DMA_CHAN_2)
  {
    iowrite32(  rd_pipe, dma_ctl_p->ifc->csr_ptr + TSC_CSR_IDMA2_RD_0_PCSR);
    iowrite32(  wr_pipe, dma_ctl_p->ifc->csr_ptr + TSC_CSR_IDMA2_WR_0_PCSR);
  }
  if( dma_ctl_p->chan == DMA_CHAN_3)
  { 
    iowrite32(  rd_pipe, dma_ctl_p->ifc->csr_ptr + TSC_CSR_IDMA2_RD_1_PCSR);
    iowrite32(  wr_pipe, dma_ctl_p->ifc->csr_ptr + TSC_CSR_IDMA2_WR_1_PCSR);
  }
  return(0);
}

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * Function name : dma_cpy_desc
 * Prototype     : void
 * Parameters    : pointer to dma control structure, offset, dma descriptor
 * Return        : void
 *----------------------------------------------------------------------------
 * Description   : copy dma descriptor
 *
 *++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

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

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * Function name : dma_get_desc
 * Prototype     : void
 * Parameters    : pointer to dma control structure, offset, dma descriptor
 * Return        : void
 *----------------------------------------------------------------------------
 * Description   : acquire dma descriptor
 *
 *++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

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

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * Function name : dma_set_ctl
 * Prototype     : uint
 * Parameters    : space, trig, intr, mode
 * Return        : dma controller value
 *----------------------------------------------------------------------------
 * Description   : set dma controller value
 *
 *++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

static uint 
dma_set_ctl( uint space,
	     uint trig,
	     uint intr,
	     uint mode)
{
  uint ctl;

  debugk(("in dma_set_ctl( %x, %x, %x, %x)...", space, trig, intr, mode));
  ctl = TSC_IDMA_DES0_UPDATE_TIME | trig | intr;
  ctl |= mode & ( DMA_MODE_ADD_MASK | DMA_MODE_SHM_MASK | DMA_MODE_SNOOP | DMA_MODE_RELAX);
  ctl |= (mode & DMA_MODE_WR_POST_MASK) << 12;


  if( (space & DMA_SPACE_MASK) == DMA_SPACE_PCIE)
  { 
    //ctl |= TSC_IDMA_DES0_PSIZE_512 | TSC_IDMA_DES0_RELAX;
    ctl |= TSC_IDMA_DES0_PSIZE_512;
  }



  if( (space & DMA_SPACE_MASK) == DMA_SPACE_USR) 
  {
    ctl |= (mode & 0xc000) << 4;
  }

  debugk(("%x\n", ctl));
  return( ctl);
}

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * Function name : dma_set_sg_desc
 * Prototype     : int
 * Parameters    : dma controller structure, dev addr,
 *                 pcie addr. , size, space, mode, tot sg count, current sg
 * Return        : dma descriptor offset
 *----------------------------------------------------------------------------
 * Description   : set sg dma descriptor
 *
 *++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

int
dma_set_sg_desc(struct dma_ctl *dma_ctl_p,
                uint  dev_addr,
                uint64_t pci_addr,
                uint size,
                unsigned char space,
                uint mode,
                int sg_count,
                int count)
{
  struct dma_ctl *dc;
  struct dma_desc *dd;
  uint next, trig, intr;
  uint64_t tmp;

  debugk(("in dma_set_sg_desc( %d : %x, %llx, %x, %x)...\n", dma_ctl_p->chan, dev_addr, pci_addr, size, space));
  dc = (struct dma_ctl *)dma_ctl_p;
  dd = (struct dma_desc *)&dc->dma_desc;

  if(count == (sg_count - 1))
  {
    next = TSC_IDMA_DES3_START_NOW | TSC_IDMA_DES3_LAST;
    intr = TSC_IDMA_DES0_INTR_ERR_END;
  }
  else
  {
    next = TSC_IDMA_DES3_START_NOW | (dma_ctl_p->desc_offset + (sizeof(*dd) * (count + 1)));
    intr = TSC_IDMA_DES0_INTR_ERR;
  }

  trig = TSC_IDMA_DES0_TRIGOUT_NO;

  /* prepare transfer descriptor */
  dd->ctl = dma_set_ctl(space, trig, intr, dc->mode);
  dd->wcnt = (uint)space << 24 | (size & TSC_IDMA_DES1_WC_MASK);
  dd->shm_addr = (dev_addr & TSC_IDMA_DES2_ADDR_MASK) | 3;       /* SHM local buffer             */
  dd->next = next;                   
  dd->rem_addr_l = (uint)pci_addr;                               /* des address bit 0:31         */
  tmp = pci_addr;                                                /* if des address  is 64 bit    */
  dd->rem_addr_h = (uint)(tmp >> 32);                            /* get bit 48:64                */
  dd->status = 0;
  dd->time_msec = 0;

  /* copy transfer descriptor in SHM */
  dma_cpy_desc(dc, count * sizeof(*dd), dd);

  return dc->desc_offset;
}

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * Function name : dma_set_desc
 * Prototype     : int
 * Parameters    : dma controller structure, shm addr,
 *                 addr. , size, space, mode, write
 * Return        : dma descriptor offset
 *----------------------------------------------------------------------------
 * Description   : set dma descriptor
 *
 *++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

int
dma_set_desc(struct dma_ctl *dma_ctl_p,
		 uint  shm_addr,
		 uint64_t addr,
		 uint size,
		 unsigned char space,
		 uint mode,
		 uint write)
{
  struct dma_ctl *dc;
  struct dma_desc *dd;
  uint next, trig, intr;
  uint64_t tmp;

  debugk(("in dma_set_desc( %d : %x, %llx, %x, %x)...\n", dma_ctl_p->chan, shm_addr, addr, size, space));
  dc = (struct dma_ctl *)dma_ctl_p;
  dd = (struct dma_desc *)&dc->dma_desc;

  next = TSC_IDMA_DES3_START_NOW | TSC_IDMA_DES3_LAST;
  trig = TSC_IDMA_DES0_TRIGOUT_NO;
  intr = TSC_IDMA_DES0_INTR_ERR_END;

  /* prepare transfer descriptor */
  dd->ctl = dma_set_ctl(space, trig, intr, dc->mode);
  dd->wcnt = (uint)space << 24 | (size & TSC_IDMA_DES1_WC_MASK);
  dd->shm_addr = (shm_addr & TSC_IDMA_DES2_ADDR_MASK) | 3;       /* SHM local buffer         */
  dd->next = next;                   
  dd->rem_addr_l = (uint)addr;                                   /* address bit 0:31         */
  tmp = addr;                                                    /* if address  is 64 bit    */
  dd->rem_addr_h = (uint)(tmp >> 32);                            /* get bit 48:64            */
  dd->status = 0;
  dd->time_msec = 0;

  /* copy transfer descriptor in SHM */
  dma_cpy_desc(dc, sizeof(*dd) * write, dd);

  return dc->desc_offset + (sizeof(*dd) * write);
}

int
dma_sg_setup(struct tsc_device *ifc,
             struct dma_ctl *dma_ctl_p,
             struct tsc_ioctl_dma_req *dr_p,
             uint64_t pci_addr,
             uint32_t dev_addr,
             unsigned char space,
             unsigned char mode)
{
  int i = 0;
  unsigned long nr_pages;
  struct page **pages = NULL;
  long got_pages = 0;
  unsigned int pci_offset;
  unsigned int dev_offset = 0;
  struct scatterlist *sg;
  int sg_count;

  pci_offset = offset_in_page(pci_addr);
  nr_pages = (pci_offset + dr_p->size + PAGE_SIZE - 1) >> PAGE_SHIFT;
  pages = kmalloc_array(nr_pages, sizeof(pages[0]), GFP_KERNEL);
  if(!pages)
  {
    printk("Error: kmalloc pages\n");
    return -ENOMEM;
  }

  got_pages = get_user_pages_fast(pci_addr, nr_pages, dma_ctl_p->dir == DMA_FROM_DEVICE, pages);
  if(got_pages != nr_pages)
  {
    printk("Error: not all pages were pinned\n");
    if(got_pages > 0)
      release_pages(pages, got_pages, 0);
    kfree(pages);
    return (got_pages < 0) ? got_pages : -EFAULT;
  }

  if(sg_alloc_table_from_pages(dma_ctl_p->sgt, pages, nr_pages, pci_offset, dr_p->size, GFP_KERNEL))
  {
    printk("Error: alloc scatter gather table from pages\n");
    release_pages(pages, got_pages, 0);
    kfree(pages);
    return -ENOMEM;
  }

  sg_count = dma_map_sg(&ifc->pdev->dev, dma_ctl_p->sgt->sgl, dma_ctl_p->sgt->nents, dma_ctl_p->dir);
  if(!sg_count)
  {
    printk("Error: DMA map scatter gather\n");
    sg_free_table(dma_ctl_p->sgt);
    release_pages(pages, got_pages, 0);
    kfree(pages);
    return -EFAULT;
  }

  if(sg_count > DMA_CHAIN_NUM)
  {
    printk("Error: no mem for dma descriptors\n");
    dma_unmap_sg(&ifc->pdev->dev, dma_ctl_p->sgt->sgl, dma_ctl_p->sgt->nents, dma_ctl_p->dir);
    sg_free_table(dma_ctl_p->sgt);
    release_pages(pages, got_pages, 0);
    kfree(pages);
    return -ENOMEM;
  }

  for_each_sg(dma_ctl_p->sgt->sgl, sg, sg_count, i)
  {
    dma_addr_t hw_address = sg_dma_address(sg);
    unsigned int hw_len = sg_dma_len(sg);

    dma_set_sg_desc(dma_ctl_p, dev_addr + dev_offset, hw_address, hw_len, space, mode, sg_count, i);
    dev_offset += hw_len;
  }

  release_pages(pages, got_pages, 0);
  kfree(pages);
  return 0;
}

int
dma_move_consistent(struct tsc_device *ifc,
                    struct dma_ctl *dma_ctl_p,
                    struct tsc_ioctl_dma_req *dr_p,
                    int chan,
                    int csr_rdo, int csr_wro,
                    int sts_rdo, int sts_wro,
                    int irq,
                    int csr_itc_imc,
                    unsigned char space_shm)
{
  int pipe;
  int rdo, wro;
  uint shm_offset;

  if(dr_p->size == 0 || dr_p->size > TSC_MAX_DMA_LEN)
  {
    printk("Error: DMA size\n");
    return -EINVAL;
  }

  if((dr_p->src_space & DMA_SPACE_MASK) ==  space_shm)
  {
    if((dr_p->des_space & DMA_SPACE_MASK) ==  space_shm)
    {
      return -EINVAL;
    }
    else
    {
      pipe = DMA_NO_PIPE;
      wro = 0;
      shm_offset = (uint)dr_p->src_addr;
      rdo = dma_set_desc(dma_ctl_p, shm_offset, dr_p->des_addr, dr_p->size, dr_p->des_space, dr_p->des_mode, 0);
    }
  }
  else
  {
    if((dr_p->des_space & DMA_SPACE_MASK) ==  space_shm)
    {
      pipe = DMA_NO_PIPE;
      rdo = 0;
      shm_offset = (uint)dr_p->des_addr;
      wro = dma_set_desc(dma_ctl_p, shm_offset, dr_p->src_addr, dr_p->size, dr_p->src_space, dr_p->src_mode, 1);
    }
    else
    {
      pipe = DMA_PIPE;
      rdo = dma_set_desc(dma_ctl_p, dma_ctl_p->ring_offset, dr_p->des_addr, dr_p->size, dr_p->des_space, dr_p->des_mode, 0);
      wro = dma_set_desc(dma_ctl_p, dma_ctl_p->ring_offset, dr_p->src_addr, dr_p->size, dr_p->src_space, dr_p->src_mode, 1);
      if((chan == DMA_CHAN_0) || (chan == DMA_CHAN_2))
      {
	irq &= ~TSC_IDMA_ITC_IM_WR0_END;
      }
      else
      {
	irq &= ~TSC_IDMA_ITC_IM_WR1_END;
      }
    }
  }
  dma_set_pipe(dma_ctl_p, pipe);

  /* reset synchronization semaphore */
  sema_init(&dma_ctl_p->sem, 0);

  /* clear transfer status */
  dma_ctl_p->status = 0;

  /* unmask DMA  interrupts */
  debugk(("DMA IRQ unmasking -> %08x\n", irq));
  iowrite32(irq, ifc->csr_ptr + csr_itc_imc);
  debugk(("rdo = %x - wro = %x\n", rdo, wro));
  if(rdo)
  {
    int ctl;

    dma_ctl_p->status |= sts_rdo;
    ctl = TSC_IDMA_CSR_ENA;
    if(dma_ctl_p->mode & DMA_MODE_CACHE_ENA)
    {
      ctl |= TSC_IDMA_CSR_CACHE_ENA;
    }
    iowrite32(ctl, ifc->csr_ptr + csr_rdo - 4);
    iowrite32(rdo, ifc->csr_ptr + csr_rdo);
  }
  if(wro)
  {
    int ctl;

    dma_ctl_p->status |= sts_wro;
    ctl = TSC_IDMA_CSR_ENA;
    if(dma_ctl_p->mode & DMA_MODE_CACHE_ENA)
    {
      ctl |= TSC_IDMA_CSR_CACHE_ENA;
    }
    iowrite32(ctl, ifc->csr_ptr + csr_wro - 4);
    iowrite32(wro, ifc->csr_ptr + csr_wro);
  }
  return 0;
}

int
dma_move_sg(struct tsc_device *ifc,
            struct dma_ctl *dma_ctl_p,
            struct tsc_ioctl_dma_req *dr_p,
            int chan,
            int csr_rdo, int csr_wro,
            int sts_rdo, int sts_wro,
            int irq,
            int csr_itc_imc,
            unsigned char space_shm,
            uint8_t write)
{
  int retval = 0;
  int status, csr, ctl;

  if(dr_p->size == 0 || dr_p->size > TSC_MAX_SG_DMA_LEN)
  {
    printk("Error: DMA size\n");
    return -EINVAL;
  }

  if(write)
  {
    dma_ctl_p->dir = DMA_TO_DEVICE;
    status = sts_wro;
    csr = csr_wro;
    retval = dma_sg_setup(ifc, dma_ctl_p, dr_p, dr_p->src_addr, (uint32_t)dr_p->des_addr, dr_p->src_space, dr_p->src_mode);
  }
  else
  {
    dma_ctl_p->dir = DMA_FROM_DEVICE;
    status = sts_rdo;
    csr = csr_rdo;
    retval = dma_sg_setup(ifc, dma_ctl_p, dr_p, dr_p->des_addr, (uint32_t)dr_p->src_addr, dr_p->des_space, dr_p->des_mode);
  }
  if(retval)
    return retval;

  /* Reset pipe settings */
  dma_set_pipe(dma_ctl_p, 0);

  /* reset synchronization semaphore */
  sema_init(&dma_ctl_p->sem, 0);

  /* clear transfer status */
  dma_ctl_p->status = 0;

  /* unmask DMA  interrupts */
  debugk(("DMA IRQ unmasking -> %08x\n", irq));
  iowrite32(irq, ifc->csr_ptr + csr_itc_imc);

  dma_ctl_p->status |= status;
  ctl = TSC_IDMA_CSR_ENA;
  if(dma_ctl_p->mode & DMA_MODE_CACHE_ENA)
  {
    ctl |= TSC_IDMA_CSR_CACHE_ENA;
  }
  iowrite32(ctl, ifc->csr_ptr + csr - 4);
  iowrite32(dma_ctl_p->desc_offset, ifc->csr_ptr + csr);

  return 0;
}

int
use_scatter_gather(struct tsc_ioctl_dma_req *dr_p, unsigned char space_shm, uint8_t *write)
{
  if((dr_p->src_space & DMA_SPACE_MASK) ==  DMA_SPACE_PCIE  ||
     (dr_p->src_space & DMA_SPACE_MASK) ==  DMA_SPACE_PCIE1)
  {
    if((dr_p->des_space & DMA_SPACE_MASK) ==  space_shm)
    {
      *write = 1;
      return 1;
    }
  }
  else if((dr_p->des_space & DMA_SPACE_MASK) ==  DMA_SPACE_PCIE  ||
          (dr_p->des_space & DMA_SPACE_MASK) ==  DMA_SPACE_PCIE1)
  {
    if((dr_p->src_space & DMA_SPACE_MASK) ==  space_shm)
    {
      *write = 0;
      return 1;
    }
  }

  return 0;
}

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * Function name : tsc_dma_move
 * Prototype     : int
 * Parameters    : ifc structure, dma request structure
 * Return        : error/success
 *----------------------------------------------------------------------------
 * Description   : execute dma move
 *
 *++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

int
tsc_dma_move(struct tsc_device *ifc,
	     struct tsc_ioctl_dma_req *dr_p)
{
  struct dma_ctl *dma_ctl_p;
  int retval = 0;
  int chan;
  int csr_rdo, csr_wro;
  int sts_rdo, sts_wro;
  int irq, csr_itc_imc;
  unsigned char space_shm;
  uint8_t write = 0;

  debugk(("in tsc_dma_move() : %llx:%x %llx:%x %x %x %x\n",
	  dr_p->des_addr, dr_p->des_space,
	  dr_p->src_addr, dr_p->src_space,
	  dr_p->size, dr_p->des_mode, dr_p->src_mode));

  chan =  DMA_START_CHAN_GET(dr_p->start_mode);
  if(chan == DMA_CHAN_0)
  {
    chan = DMA_CHAN_0;
    csr_rdo = TSC_CSR_IDMA_RD_0_NDES;
    csr_wro = TSC_CSR_IDMA_WR_0_NDES;
    sts_rdo = DMA_STATUS_RUN_RD0;
    sts_wro = DMA_STATUS_RUN_WR0;
    csr_itc_imc = TSC_CSR_IDMA_ITC_IMC;
    space_shm = DMA_SPACE_SHM;
  }
  if(chan == DMA_CHAN_1)
  {
    csr_rdo = TSC_CSR_IDMA_RD_1_NDES;
    csr_wro = TSC_CSR_IDMA_WR_1_NDES;
    sts_rdo = DMA_STATUS_RUN_RD1;
    sts_wro = DMA_STATUS_RUN_WR1;
    csr_itc_imc = TSC_CSR_IDMA_ITC_IMC;
    space_shm = DMA_SPACE_SHM;
  }
  if(chan == DMA_CHAN_2)
  {
    csr_rdo = TSC_CSR_IDMA2_RD_0_NDES;
    csr_wro = TSC_CSR_IDMA2_WR_0_NDES;
    sts_rdo = DMA_STATUS_RUN_RD0;
    sts_wro = DMA_STATUS_RUN_WR0;
    csr_itc_imc = TSC_CSR_IDMA2_ITC_IMC;
    space_shm = DMA_SPACE_SHM2;
  }
  if(chan == DMA_CHAN_3)
  {
    csr_rdo = TSC_CSR_IDMA2_RD_1_NDES;
    csr_wro = TSC_CSR_IDMA2_WR_1_NDES;
    sts_rdo = DMA_STATUS_RUN_RD1;
    sts_wro = DMA_STATUS_RUN_WR1;
    csr_itc_imc = TSC_CSR_IDMA2_ITC_IMC;
    space_shm = DMA_SPACE_SHM2;
  }
  dma_ctl_p = ifc->dma_ctl[chan];
  /* protect against concurrent access to queue control structure */
  mutex_lock(&dma_ctl_p->dma_lock);
  if((dma_ctl_p->state != DMA_STATE_ALLOCATED) &&
     (dma_ctl_p->state != DMA_STATE_DONE))
  {
    mutex_unlock(&dma_ctl_p->dma_lock);
    return -EPERM;
  }
  irq = dma_ctl_p->irq;

  if(use_scatter_gather(dr_p, space_shm, &write) &&
    (dma_ctl_p->mode & DMA_MODE_SG))
  {
    retval = dma_move_sg(ifc, dma_ctl_p, dr_p, chan, csr_rdo, csr_wro,
                         sts_rdo, sts_wro, irq, csr_itc_imc, space_shm, write);
  }
  else
  {
    retval = dma_move_consistent(ifc, dma_ctl_p, dr_p, chan, csr_rdo, csr_wro,
                                 sts_rdo, sts_wro, irq, csr_itc_imc, space_shm);
  }
  if(retval < 0)
  {
    mutex_unlock(&dma_ctl_p->dma_lock);
    return retval;
  }

  dma_ctl_p->state = DMA_STATE_STARTED;
  /* release locking semaphore */
  mutex_unlock(&dma_ctl_p->dma_lock);

  if(dr_p->wait_mode & DMA_WAIT_INTR)
  {
    retval = dma_wait(dma_ctl_p, dr_p->wait_mode);
  }
  dr_p->dma_status = dma_ctl_p->status;

  return retval;
}

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * Function name : dma_status
 * Prototype     : int
 * Parameters    : pointer to dma control structure, dma status
 * Return        : error/success
 *----------------------------------------------------------------------------
 * Description   : get dma channel status
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
    rdo = TSC_CSR_IDMA_RD_0_CSR;
    wro = TSC_CSR_IDMA_WR_0_CSR;
  }
  else if( ds_p->dma.chan == DMA_CHAN_1)
  {
    rdo = TSC_CSR_IDMA_RD_1_CSR;
    wro = TSC_CSR_IDMA_WR_1_CSR;
  }
  else if( ds_p->dma.chan == DMA_CHAN_2)
  {
    rdo = TSC_CSR_IDMA2_RD_0_CSR;
    wro = TSC_CSR_IDMA2_WR_0_CSR;
  }
  else if( ds_p->dma.chan == DMA_CHAN_3)
  {
    rdo = TSC_CSR_IDMA2_RD_1_CSR;
    wro = TSC_CSR_IDMA2_WR_1_CSR;
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

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * Function name : tsc_dma_status
 * Prototype     : int
 * Parameters    : pointer to dma control structure, dma status
 * Return        : error/success
 *----------------------------------------------------------------------------
 * Description   : get dma channel status
 *
 *++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

int
tsc_dma_status( struct tsc_device *ifc,
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

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * Function name : tsc_dma_mode
 * Prototype     : int
 * Parameters    : pointer to ifc control structure and dma mode structure
 * Return        : error/success
 *----------------------------------------------------------------------------
 * Description   : get dma channel status
 *
 *++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

int
tsc_dma_mode( struct tsc_device *ifc,
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
    dma_ctl_p->mode = dm_p->mode;
  }
  dm_p->mode = dma_ctl_p->mode;

  return( 0);
}

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * Function name : dma_get_channel
 * Prototype     : int
 * Parameters    : ifc structure, dma control structure
 * Return        : error/channel
 *----------------------------------------------------------------------------
 * Description   : get dma channel
 *
 *++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

int
dma_get_channel(struct tsc_device *ifc,
                struct tsc_ioctl_dma_req *dr_p)
{
  struct dma_ctl *dma_ctl_p = NULL;
  int channel = -1;
  int smem = 0;

  if (((dr_p->src_space & DMA_SPACE_MASK) == DMA_SPACE_SHM2) ||
      ((dr_p->des_space & DMA_SPACE_MASK) == DMA_SPACE_SHM2) ||
      ((dr_p->src_space & DMA_SPACE_MASK) == DMA_SPACE_USR2) ||
      ((dr_p->des_space & DMA_SPACE_MASK) == DMA_SPACE_USR2))
  {
    smem = 2;
  }
  else
  {
    smem = 1;
  }

  switch(smem)
  {
    case 1:
      /* Try channel 0 */
      channel = 0;
      dma_ctl_p = ifc->dma_ctl[channel];
      if (dma_alloc(dma_ctl_p) == 0)
        break;
      /* Try channel 1 */
      channel++;
      dma_ctl_p = ifc->dma_ctl[channel];
      if (dma_alloc(dma_ctl_p) == 0)
        break;
      debugk(("SMEM1 DMA channels are busy!\n"));
      return -EBUSY;
    case 2:
      /* Try channel 2 */
      channel = 2;
      dma_ctl_p = ifc->dma_ctl[channel];
      if (dma_alloc(dma_ctl_p) == 0)
        break;
      /* Try channel 3 */
      channel++;
      dma_ctl_p = ifc->dma_ctl[channel];
      if (dma_alloc(dma_ctl_p) == 0)
        break;
      debugk(("SMEM2 DMA channels are busy!\n"));
      return -EBUSY;
  }
  return channel;
}

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * Function name : tsc_dma_transfer
 * Prototype     : int
 * Parameters    : ifc structure, dma request structure
 * Return        : error/success
 *----------------------------------------------------------------------------
 * Description   : do dma transfer
 *
 *++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

int
tsc_dma_transfer(struct tsc_device *ifc,
          struct tsc_ioctl_dma_req *dr_p)
{
  struct dma_ctl *dma_ctl_p = NULL;
  int channel = 0;
  int retval = 0;

  if(dr_p == NULL)
    return -EINVAL;

  channel = dma_get_channel(ifc, dr_p);
  if(channel < 0)
    return channel;
  else
    dma_ctl_p = ifc->dma_ctl[channel];

  dr_p->start_mode = DMA_START_CHAN(channel);
  if(!(dr_p->wait_mode & DMA_WAIT_INTR))
    dr_p->wait_mode = DMA_WAIT_INTR | DMA_WAIT_10MS | (5 << 4); // Timeout after 50 ms

  retval = tsc_dma_move(ifc, dr_p);
  if(retval == 0)
    retval = dma_free(dma_ctl_p);
  else
  {
    dma_clear(dma_ctl_p);
    dma_free(dma_ctl_p);
    retval = -EIO;
  }
  return retval;
}
