/*=========================< begin file & file header >=======================
 *  References
 *  
 *    filename : dmalib.h
 *    author   : JFG, XP
 *    company  : IOxOS
 *    creation : july 30,2015
 *
 *----------------------------------------------------------------------------
 *  Description
 *
 *    This file contains the declarations of all exported functions define in
 *    dmalib.c
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

#ifndef _H_DMALIB
#define _H_DMALIB

#define DMA_CTRL_NUM    2        /* number of DMA controllers         */
#define DMA_CHAIN_NUM  32        /* max number of descriptor in chain */

#define DMA_DESC_OFFSET  0x3000        /* max number of descriptor in chain */
#define DMA_DESC_SIZE     0x800        /* max size for desc chain (2*32*32) */
#define DMA_RING_OFFSET  0x1000        /* offset in SHM for ring buffer     */
#define DMA_RING_SIZE    0x1000        /* ring size 4k                      */

#define DMA_STS_IDLE          0x00
#define DMA_STS_ALLOCATED     0x01
#define DMA_STS_STARTED       0x02
#define DMA_STS_WAITING       0x03
#define DMA_STS_DONE          0x04


/* DMA chain hardware descriptor (8*32bit) */
struct dma_desc
{
  uint ctl;                      /* control register                */
  uint wcnt;                     /* word counter (in bytes)         */
  uint shm_addr;                 /* offset in shared memory         */
  uint next;                     /* offset of next chain descriptor */
  uint rem_addr_l;               /* remote address bit  0:31        */
  uint rem_addr_h;               /* remote address bit 32:63        */
  uint status;                   /* transfer status                 */
  uint time_msec;                /* msec timer                      */
};

struct dma_ctl
{
  int chan;                      /* DMA controller channel number                                   */
  int state;                     /* DMA channel state                                               */
  struct tsc_device *ifc;
  void __iomem *desc_ptr;        /* kernel pointer to SHM allocated to DMA descriptor               */
  uint desc_offset;              /* PCI base address of SHM allocated to DMA descriptor             */
  uint ring_offset;              /* PCI base address of SHM allocated to DMA ring buffer            */
  struct mutex dma_lock;         /* mutex to lock DMA access                     */
  struct semaphore sem;           /* semaphore to synchronize with DMA interrputs  */
  struct dma_desc rd_desc;
  struct dma_desc wr_desc;
  int status;                    /* DMA transfer status                                             */
  int irq;                       /* IRQs associated to the DMA channel                              */
  short rd_mode; short wr_mode;  /* DMA default mode                                                */
};

void tsc_dma_irq(struct tsc_device *ifc, int src, void *dma_ctl_p);
int  dma_init(struct dma_ctl *dma);
int  tsc_dma_move(struct tsc_device *ifc, struct tsc_ioctl_dma_req *dma_req_p);
int  tsc_dma_wait(struct tsc_device *ifc, struct tsc_ioctl_dma_req *dma_req_p);
int  tsc_dma_status(struct tsc_device *ifc, struct tsc_ioctl_dma_sts *dma_sts_p);
int  tsc_dma_mode(struct tsc_device *ifc, struct tsc_ioctl_dma_mode *dma_mode_p);
int  tsc_dma_alloc(struct tsc_device *ifc, struct tsc_ioctl_dma *dma_p);
int  tsc_dma_free(struct tsc_device *ifc, struct tsc_ioctl_dma *dma_p);
int  tsc_dma_clear(struct tsc_device *ifc, struct tsc_ioctl_dma *dma_p);
int  tsc_dma_transfer(struct tsc_device *ifc, struct tsc_ioctl_dma_req *dma_req_p);

#endif /*  _H_DMALIB */

