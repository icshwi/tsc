/*=========================< begin file & file header >=======================
 *  References
 *  
 *    filename : dmalib.h
 *    author   : JFG
 *    company  : IOxOS
 *    creation : july 30,2015
 *    version  : 0.0.1
 *
 *----------------------------------------------------------------------------
 *  Description
 *
 *    This file contain the declarations of all exported functions define in
 *    dmalib.c
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
  struct ifc1211_device *ifc;
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

void tsc_dma_irq(struct ifc1211_device *ifc, int src, void *dma_ctl_p);
int  dma_init(struct dma_ctl *dma);
int  tsc_dma_move(struct ifc1211_device *ifc, struct tsc_ioctl_dma_req *dma_req_p);
int  tsc_dma_wait(struct ifc1211_device *ifc, struct tsc_ioctl_dma_req *dma_req_p);
int  tsc_dma_status(struct ifc1211_device *ifc, struct tsc_ioctl_dma_sts *dma_sts_p);
int  tsc_dma_mode(struct ifc1211_device *ifc, struct tsc_ioctl_dma_mode *dma_mode_p);
int  tsc_dma_alloc(struct ifc1211_device *ifc, struct tsc_ioctl_dma *dma_p);
int  tsc_dma_free(struct ifc1211_device *ifc, struct tsc_ioctl_dma *dma_p);
int  tsc_dma_clear(struct ifc1211_device *ifc, struct tsc_ioctl_dma *dma_p);

#endif /*  _H_DMALIB */

/*================================< end file >================================*/
