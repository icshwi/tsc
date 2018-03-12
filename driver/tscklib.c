/*=========================< begin file & file header >=======================
 *  References
 *  
 *    filename : tscklib.c
 *    author   : JFG, XP
 *    company  : IOxOS
 *    creation : Sept 30,2015
 *
 *----------------------------------------------------------------------------
 *  Description
 *
 *   This file contains all kernel libraries (functions used by the driver).
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
#include <linux/fsl_ifc.h>

#define DBGno
#include "debug.h"

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * Function name : tsc_msb32
 * Prototype     : unsigned int
 * Parameters    : 32 bit unsigned integer
 * Return        : most significant bit
 *----------------------------------------------------------------------------
 * Description   : return the index (from 0 to 31) of the most significant bit
 *                 from a 32 bit integer
 *
 *++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

unsigned int 
tsc_msb32( unsigned int x)
{
  static const unsigned int bval[] =
  {0,1,2,2,3,3,3,3,4,4,4,4,4,4,4,4};

  unsigned int r = 0;
  if( x & 0xffff0000){ r += 16; x >>= 16;}
  if( x & 0x0000ff00){ r += 8; x >>= 8;}
  if( x & 0x000000f0){ r += 4; x >>= 4;}
  return( r + bval[x]);

}

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * Function name : tsc_dev_init
 * Prototype     : int
 * Parameters    : pointer to tsc device control structure
 * Return        : error/success
 *----------------------------------------------------------------------------
 * Description   : initialize tsc device internal resources
 *
 *++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

int 
tsc_dev_init( struct tsc_device *ifc)
{
  int retval = 0;

  /* create protection mutex for CSR access */
  mutex_init( &ifc->csr_lock);

  /* initialize interrupt handling mechanism */
  retval = tsc_irq_init( ifc);

  /* initialize data structures controlling the PCI master mapping */
  retval = tsc_map_mas_init( ifc);

  /* initialize data structures controlling access to SHM */
  retval = tsc_shm_init( ifc, TSC_SHM1_IDX); /* SHM 1 */
  retval = tsc_shm_init( ifc, TSC_SHM2_IDX); /* SHM 2 */

  /* initialize data structures controlling RDWR access */
  retval = tsc_rdwr_init( ifc);

  /* initialize nano timer  */
  retval = tsc_timer_init( ifc);

  /* initialize data structures controlling SPI FLASH */
  retval = tsc_sflash_init( ifc);

  /* initialize data structures controlling the DMA channels */
  retval = tsc_dma_init( ifc);

  /* initialize data structures controlling the I2C controller */
  retval = tsc_i2c_init( ifc);

  return( retval);
}

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * Function name : tsc_dev_exit
 * Prototype     : void
 * Parameters    : pointer to tsc device control structure
 * Return        : void
 *----------------------------------------------------------------------------
 * Description   : return allocated resources to OS
 *
 *++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

void 
tsc_dev_exit( struct tsc_device *ifc)
{
  debugk(( KERN_NOTICE "tsc :  entering tsc_dev_exit( %p)\n", ifc));

  /* release data structures controlling RDWR */
  tsc_rdwr_exit( ifc);

  /* release data structures controlling SHM */
  tsc_shm_exit( ifc, TSC_SHM1_IDX);
  tsc_shm_exit( ifc, TSC_SHM2_IDX);

  /* release data structures controlling the PCI master mapping */
  tsc_map_mas_exit( ifc);

  /* release data structures controlling interrupt handling */
  tsc_irq_exit( ifc);

  /* release data structures controlling DMA controllers */
  tsc_dma_exit( ifc);

  /* release data structures controlling I2C controllers */
  tsc_i2c_exit( ifc);

  mutex_destroy( &ifc->csr_lock);

  return;
}

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * Function name : tsc_irq_init
 * Prototype     : int
 * Parameters    : pointer to tsc device control structure
 * Return        : error/success
 *----------------------------------------------------------------------------
 * Description   : initialize tsc irq
 *
 *++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

int 
tsc_irq_init( struct tsc_device *ifc)
{
  int retval;
  int i;
  int ena;

  retval = 0;
  debugk(( KERN_NOTICE "tsc : Entering tsc_irq_init( %p)\n", ifc));

  /* install default interrupt handlers */
  ifc->irq_tbl = (struct tsc_irq_handler *)kmalloc( TSC_IRQ_NUM * sizeof(struct tsc_irq_handler), GFP_KERNEL);
  for( i = 0; i < TSC_IRQ_NUM; i++)
  {
    tsc_irq_unregister( ifc, i);
  }
  /* initialize interrupt controllers */
  /* mask all interrupt sources */
  iowrite32( TSC_ALL_ITC_MASK_ALL, ifc->csr_ptr + TSC_CSR_ILOC_ITC_IMS);
  iowrite32( TSC_ALL_ITC_MASK_ALL, ifc->csr_ptr + TSC_CSR_ITC_IMS);
  iowrite32( TSC_ALL_ITC_MASK_ALL, ifc->csr_ptr + TSC_CSR_IDMA_ITC_IMS);
  iowrite32( TSC_ALL_ITC_MASK_ALL, ifc->csr_ptr + TSC_CSR_USER_ITC_IMS);

  /* clear all pending interrupts and enable controller */
  ena = TSC_ALL_ITC_CSR_CLEARIP | TSC_ALL_ITC_CSR_GLENA;
  iowrite32( ena, ifc->csr_ptr + TSC_CSR_ILOC_ITC_CSR);
  iowrite32( ena | TSC_ITC_CSR_AUTOIACK, ifc->csr_ptr + TSC_CSR_ITC_CSR);
  iowrite32( ena, ifc->csr_ptr + TSC_CSR_IDMA_ITC_CSR);
  iowrite32( ena, ifc->csr_ptr + TSC_CSR_USER_ITC_CSR);

  return( retval);
}

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * Function name : tsc_irq_exit
 * Prototype     : int
 * Parameters    : pointer to tsc device control structure
 * Return        : void
 *----------------------------------------------------------------------------
 * Description   : irq clearing and exit
 *
 *++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

void 
tsc_irq_exit( struct tsc_device *ifc)
{
  int dis;

  debugk(( KERN_NOTICE "tsc : Entering tsc_irq_exit( %p)\n", ifc));
  /* disable interrupt controllers */
  /* mask all interrupt sources */
  iowrite32( TSC_ALL_ITC_MASK_ALL, ifc->csr_ptr + TSC_CSR_ILOC_ITC_IMS);
  iowrite32( TSC_ALL_ITC_MASK_ALL, ifc->csr_ptr + TSC_CSR_ITC_IMS);
  iowrite32( TSC_ALL_ITC_MASK_ALL, ifc->csr_ptr + TSC_CSR_IDMA_ITC_IMS);
  iowrite32( TSC_ALL_ITC_MASK_ALL, ifc->csr_ptr + TSC_CSR_USER_ITC_IMS);

  /* clear all pending interrupts and disable controller */
  dis = TSC_ALL_ITC_CSR_CLEARIP;
  iowrite32( dis, ifc->csr_ptr + TSC_CSR_ILOC_ITC_CSR);
  iowrite32( dis, ifc->csr_ptr + TSC_CSR_ITC_CSR);
  iowrite32( dis, ifc->csr_ptr + TSC_CSR_IDMA_ITC_CSR);
  iowrite32( dis, ifc->csr_ptr + TSC_CSR_USER_ITC_CSR);

  /* release data structures controlling interrupt handling */
  if( ifc->irq_tbl)
  {
    kfree( ifc->irq_tbl);
  }
  return;
}

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * Function name : tsc_map_mas_init
 * Prototype     : int
 * Parameters    : pointer to tsc device control structure
 * Return        : error/success
 *----------------------------------------------------------------------------
 * Description   : initialize tsc address mapping data structures for PCI
 *                 master access.
 *
 *++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

int 
tsc_map_mas_init( struct tsc_device *ifc)
{
  int retval;
  long base;
  long size;
  int pg_num;
  uint pcie_mmuadd;

  retval = 0;
  debugk(( KERN_NOTICE "tsc : Entering tsc_map_mas_init( %p)\n", ifc));

  /* get master interface current configuration */
  pcie_mmuadd = ioread32( ifc->csr_ptr + TSC_CSR_PCIE_MMUADD);

  /* initialize data structures controlling the PCI PMEM master mapping */
  size = pci_resource_len( ifc->pdev, 0);
  debugk(("PMEM size: %08lx - %08x\n", size, TSC_PCIE_MMUADD_PMEM_SIZE(pcie_mmuadd)));
  ifc->map_mas_pci_pmem = NULL;
  if( size)
  {
    pg_num = size/TSC_PCIE_MMU_PG_4M;
    base = pci_resource_start( ifc->pdev, 0);
    debugk(( KERN_NOTICE "calling map_init( %x, %lx, %x, %x)\n", 
	     MAP_MAS_PCIE_PMEM, base, pg_num, TSC_PCIE_MMU_PG_4M));
    ifc->map_mas_pci_pmem =  map_init( MAP_MAS_PCIE_PMEM, base, pg_num, TSC_PCIE_MMU_PG_4M);
    ifc->map_mas_pci_pmem->win_size = size;
    mutex_init( &ifc->map_mas_pci_pmem->map_lock);
  }

  /* initialize data structures controlling the PCI MEM master mapping */
  size = pci_resource_len( ifc->pdev, 2);
  debugk(("MEM size: %08lx - %08x\n", size, TSC_PCIE_MMUADD_MEM_SIZE(pcie_mmuadd)));
/*==> TOSCA   printk("MEM size: %08lx - %08x\n", size, TSC_PCIE_MMUADD_MEM_SIZE(pcie_mmuadd)*0x10);*/
  ifc->map_mas_pci_mem = NULL;
  if( size)
  {
    pg_num = size/TSC_PCIE_MMU_PG_64K;
/*==> TOSCA         pg_num = size/TSC_PCIE_MMU_PG_1M; */
    base = pci_resource_start( ifc->pdev, 2);
    debugk(( KERN_NOTICE "calling map_init( %x, %lx, %x, %x)\n", 
             MAP_MAS_PCIE_MEM, base, pg_num, TSC_PCIE_MMU_PG_64K));
    ifc->map_mas_pci_mem =  map_init( MAP_MAS_PCIE_MEM, base, pg_num, TSC_PCIE_MMU_PG_64K);
/*==> TOSCA     debugk(( KERN_NOTICE "calling tsc_map_init( %x, %lx, %x, %x)\n", */
/*==> TOSCA 	     MAP_MAS_PCIE_MEM, base, pg_num, TSC_PCIE_MMU_PG_1M));      */
/*==> TOSCA     ifc->map_mas_pci_mem =  map_init( MAP_MAS_PCIE_MEM, base, pg_num, TSC_PCIE_MMU_PG_1M); */
    ifc->map_mas_pci_mem->win_size = size;
    mutex_init( &ifc->map_mas_pci_mem->map_lock);
  }

  return( retval);
}

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * Function name : tsc_map_mas_exit
 * Prototype     : void
 * Parameters    : pointer to TSC device control structure
 * Return        : void
 *----------------------------------------------------------------------------
 * Description   : master mapping exit
 *
 *++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

void
tsc_map_mas_exit( struct tsc_device *ifc)
{
  debugk(( KERN_NOTICE "tsc : Entering tsc_map_mas_exit( %p)\n", ifc));
  if( ifc->map_mas_pci_mem)
  {
    /* clear HW sg */
    tsc_map_mas_clear_sg( ifc, ifc->map_mas_pci_mem, 0, ifc->map_mas_pci_mem->pg_num);
    /* release control structures */
    mutex_destroy( &ifc->map_mas_pci_mem->map_lock);
    map_exit( ifc->map_mas_pci_mem);
  }

  if( ifc->map_mas_pci_pmem)
  {
    /* clear HW sg */
    tsc_map_mas_clear_sg( ifc, ifc->map_mas_pci_pmem, 0, ifc->map_mas_pci_pmem->pg_num);
    /* release control structures */
    mutex_destroy( &ifc->map_mas_pci_pmem->map_lock);
    map_exit( ifc->map_mas_pci_pmem);
  }
}

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * Function name : tsc_map_id
 * Prototype     : struct map_ctl *
 * Parameters    : pointer to tsc device control structure
 *                 pointer to map control data structure
 * Return        : none
 *----------------------------------------------------------------------------
 * Description   : set id for mapping
 *
 *++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

static struct map_ctl *
tsc_map_id( struct tsc_device *ifc,
	    struct tsc_ioctl_map_ctl *m)
{
  struct map_ctl *map_ctl_p;

  if( m->sg_id == MAP_MAS_PCIE_PMEM)
  {
    map_ctl_p = ifc->map_mas_pci_pmem;
  }
  else if( m->sg_id == MAP_MAS_PCIE_MEM)
  {
    map_ctl_p = ifc->map_mas_pci_mem;
  }
  else if( m->sg_id == MAP_SLV_PCIE1_PMEM)
  {
    map_ctl_p = ifc->map_slv_pci1_pmem;
  }
  else if( m->sg_id == MAP_SLV_PCIE1_MEM)
  {
    map_ctl_p = ifc->map_slv_pci1_mem;
  }
  else 
  {
    map_ctl_p = NULL;
  }
  return(  map_ctl_p);
}

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * Function name : tsc_map_read
 * Prototype     : int
 * Parameters    : device structure and mapping structure
 * Return        : error/success
 *----------------------------------------------------------------------------
 * Description   : read mapping
 *
 *++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

int 
tsc_map_read( struct tsc_device *ifc,
	      struct tsc_ioctl_map_ctl *m)
{
  struct map_ctl *map_ctl_p;
  int retval;

  debugk(("in tsc_map_read( %p, %p, %x)\n", ifc, m, m->sg_id));

  map_ctl_p = tsc_map_id( ifc, m);

  debugk(("map_ctl_p = %p [%x]\n", map_ctl_p, m->sg_id));
  /* get copy of map control structure */
  if( map_ctl_p)
  {
    m->pg_num = map_ctl_p->pg_num;
    m->pg_size = map_ctl_p->pg_size;
    m->win_base = map_ctl_p->win_base;
    m->win_size = map_ctl_p->win_size;
    if( m->map_p)
    {
      if( copy_to_user( m->map_p, map_ctl_p->map_p, (sizeof( struct map_blk))*map_ctl_p->pg_num))
      {
        retval = -EIO;
      }
    }
    return(0);
  }

  return( -EINVAL);

}

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * Function name : tsc_map_clear
 * Prototype     : int
 * Parameters    : pointer to tsc device control structure
 *                 pointer to map control data structure
 * Return        : < 0  in case of error
 *----------------------------------------------------------------------------
 * Description   : clear mapping
 *
 *++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

int 
tsc_map_clear( struct tsc_device *ifc,
	      struct tsc_ioctl_map_ctl *m)
{
  struct map_ctl *map_ctl_p;
  int retval;
  int mas;

  debugk(("in tsc_map_clear( %p, %p, %x)\n", ifc, m, m->sg_id));

  retval = 0;
  mas = 0;
  if( m->sg_id == MAP_MAS_PCIE_PMEM)
  {
    map_ctl_p = ifc->map_mas_pci_pmem;
    mas = 1;
  }
  else if( m->sg_id == MAP_MAS_PCIE_MEM)
  {
    map_ctl_p = ifc->map_mas_pci_mem;
    mas = 1;
  }
  else if( m->sg_id == MAP_SLV_PCIE1_PMEM)
  {
    map_ctl_p = ifc->map_slv_pci1_pmem;
    mas = 1;
  }
  else if( m->sg_id == MAP_SLV_PCIE1_MEM)
  {
    map_ctl_p = ifc->map_slv_pci1_mem;
    mas = 1;
  }
  else 
  {
    map_ctl_p = NULL;
    retval = -EINVAL;
  }
  if( map_ctl_p)
  {
    mutex_lock( &map_ctl_p->map_lock);
    map_clear( map_ctl_p);
    if( mas)
    {
      tsc_map_mas_clear_sg( ifc, map_ctl_p, 0, map_ctl_p->pg_num);
    }
    mutex_unlock( &map_ctl_p->map_lock);
  }
  return( retval);

}

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * Function name : tsc_timer_init
 * Prototype     : int
 * Parameters    : pointer to tsc device control structure
 * Return        : 0
 *----------------------------------------------------------------------------
 * Description   : initialize timer
 *
 *++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

int 
tsc_timer_init( struct tsc_device *ifc)
{
  struct tsc_ioctl_timer tmr;

  debugk(("in tsc_timer_init( %p)\n", ifc));

  timer_init( ifc);
  tmr.mode = TSC_GLTIM_BASE_1000 | TSC_GLTIM_SYNC_ENA | TSC_GLTIM_100MHZ;
  tmr.time.msec = 0;
  tmr.time.usec = 0;
  tsc_timer_start( ifc, &tmr);

  return( 0);

}

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * Function name : tsc_sflash_init
 * Prototype     : int
 * Parameters    : pointer to tsc device control structure
 * Return        : error/success
 *----------------------------------------------------------------------------
 * Description   : prepare access to sflash
 *
 *++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

int 
tsc_sflash_init( struct tsc_device *ifc)
{
  char sflash_id[4];
  struct sflash_para *p;
  int retval;

  tsc_sflash_read_ID( ifc, sflash_id);
  if( sflash_id[0] == SFLASH_ID_INV)
  {
    return(-1);
  }
  debugk(("in tsc_sflash_init() : flash_id = %02x.%02x.%02x\n", sflash_id[0], sflash_id[1], sflash_id[2]));
  ifc->sflash_ctl = kzalloc( sizeof( struct sflash_ctl), GFP_KERNEL);
  if( !ifc->sflash_ctl)
  {
    return(-ENOMEM);
  }
  memcpy( &ifc->sflash_ctl->id[0], sflash_id, 3);
  retval = tsc_sflash_conf( ifc);
  if( retval)
  {  
    debugk(("SFLASH no match found...\n")); 
    kfree( ifc->sflash_ctl);
    ifc->sflash_ctl = NULL;
    return( retval);
  }
  p = ifc->sflash_ctl->para;
  debugk(("SFLASH match S25FL128P: %x - %x - %x -%d\n", p->device_size, p->sector_size, p->page_size, p->erase_tmo));
  mutex_init( &ifc->sflash_ctl->sflash_lock);
  return( 0);
}

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * Function name : tsc_shm_init
 * Prototype     : int
 * Parameters    : pointer to tsc device control structure
 *                 shm index
 * Return        : error/success
 *----------------------------------------------------------------------------
 * Description   : prepare translation windows to access SHM
 *
 *++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

int 
tsc_shm_init( struct tsc_device *ifc,
	      int idx)
{
  int retval;
  struct tsc_ioctl_map_win mas_win;
  int csr_offset, space;

  debugk(("in tsc_shm_init( %p, idx)\n", ifc, idx));

  /* Check PCI MEM mapping has been initialized */
  ifc->shm_ctl[idx] = NULL;
  if( !ifc->map_mas_pci_mem)
  {
    return(-1);
  }
  retval = 0;
  switch( idx)
  {
    case TSC_SHM1_IDX:
    {
      csr_offset = TSC_CSR_SMEM_SRAM_CSR;
      space = MAP_SPACE_SHM;
      break;
    }
    case TSC_SHM2_IDX:
    {
      csr_offset = TSC_CSR_SMEM2_SRAM_CSR;
      space = MAP_SPACE_SHM2;
      break;
    }
    default:
    {
      return(-1);
    }
  }
  ifc->shm_ctl[idx] = kzalloc( sizeof( struct shm_ctl), GFP_KERNEL);
  if( !ifc->shm_ctl[idx])
  {
    return(-ENOMEM);
  }

  /* prepare translation window to access tsc SRAM  */
  /* get SRAM size from SMEM_RAM_CRS register */
  ifc->shm_ctl[idx]->sram_size = ioread32( ifc->csr_ptr + csr_offset);
  ifc->shm_ctl[idx]->sram_size = TSC_SMEM_RAM_SIZE( ifc->shm_ctl[idx]->sram_size);
  debugk(("sram_size = %x\n", ifc->shm_ctl[idx]->sram_size));
  if( ifc->shm_ctl[idx]->sram_size)
  { 
    memset( &mas_win, 0, sizeof( mas_win));
    mas_win.req.rem_addr = 0; /* point to SMEM RAM base */
    mas_win.req.size = ifc->shm_ctl[idx]->sram_size;
    mas_win.req.mode.space = space;
    mas_win.req.mode.sg_id = MAP_ID_MAS_PCIE_MEM;
    mas_win.req.mode.flags = 0;
    ifc->shm_ctl[idx]->sram_offset = tsc_map_mas_alloc( ifc, &mas_win);
    debugk(("sram loc_addr = %lx\n",  mas_win.req.loc_addr));
    if( ifc->shm_ctl[idx]->sram_offset >= 0)
    {
      ifc->shm_ctl[idx]->sram_ptr = ioremap_nocache( mas_win.req.loc_addr, ifc->shm_ctl[idx]->sram_size);
    }
  }
  return( retval);
}

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * Function name : tsc_shm_exit
 * Prototype     : int
 * Parameters    : pointer to tsc device control structure
 * Return        : error/success
 *----------------------------------------------------------------------------
 * Description   : release resources allocated for shm control
 *
 *++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

void 
tsc_shm_exit( struct tsc_device *ifc,
	      int idx)
{
  debugk(("in tsc_shm_exit( %p, %d)\n", ifc, idx));

  switch( idx)
  {
    case TSC_SHM1_IDX:
    {
      break;
    }
    case TSC_SHM2_IDX:
    {
      break;
    }
    default:
    {
      return;
    }
  }
  if( ifc->shm_ctl[idx])
  {
    if( ifc->shm_ctl[idx]->sram_ptr)
    {
      tsc_map_mas_free( ifc, MAP_ID_MAS_PCIE_MEM, ifc->shm_ctl[idx]->sram_offset);
      iounmap( ifc->shm_ctl[idx]->sram_ptr);
    }
    kfree( ifc->shm_ctl[idx]);
  }
}

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * Function name : tsc_dma_init
 * Prototype     : int
 * Parameters    : pointer to tsc device control structure
 * Return        : error/success
 *----------------------------------------------------------------------------
 * Description   : prepare access to dma
 *
 *++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

int 
tsc_dma_init( struct tsc_device *ifc)
{
  int chan;

  for( chan = 0; chan < DMA_CHAN_NUM; chan++)
  {
    ifc->dma_ctl[chan] = kzalloc( sizeof( struct dma_ctl), GFP_KERNEL);
    if( !ifc->dma_ctl[chan])
    {
      return(-ENOMEM);
    }
    /* initialize DMA control structure */
    ifc->dma_ctl[chan]->chan = chan;
    ifc->dma_ctl[chan]->ifc = ifc;
    ifc->dma_ctl[chan]->desc_ptr = ifc->shm_ctl[chan/2]->sram_ptr + DMA_DESC_OFFSET + ((chan%2)*DMA_DESC_SIZE);
    ifc->dma_ctl[chan]->desc_offset = DMA_DESC_OFFSET + ((chan%2)*DMA_DESC_SIZE);
    ifc->dma_ctl[chan]->ring_offset = DMA_RING_OFFSET + ((chan%2)*DMA_RING_SIZE);
    dma_init( ifc->dma_ctl[chan]);
  }
  return( 0);
}

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * Function name : tsc_dma_exit
 * Prototype     : void
 * Parameters    : pointer to tsc device control structure
 * Return        : void
 *----------------------------------------------------------------------------
 * Description   : release dma
 *
 *++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

void 
tsc_dma_exit( struct tsc_device *ifc)
{
  int chan;

  for( chan = 0; chan < DMA_CHAN_NUM; chan++)
  {
    if( ifc->dma_ctl[chan])
    {
      mutex_destroy( &ifc->dma_ctl[chan]->dma_lock);
      kfree( ifc->dma_ctl[chan]);
    }
  }
  return;
}

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * Function name : tsc_i2c_init
 * Prototype     : int
 * Parameters    : pointer to tsc device control structure
 * Return        : error/success
 *----------------------------------------------------------------------------
 * Description   : prepare access to i2c
 *
 *++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

int 
tsc_i2c_init( struct tsc_device *ifc)
{
  ifc->i2c_ctl = kzalloc( sizeof( struct i2c_ctl), GFP_KERNEL);
  if( !ifc->i2c_ctl)
  {
    return(-ENOMEM);
  }
  /* initialize I2C control structure */
  mutex_init( &ifc->i2c_ctl->i2c_lock);
  sema_init( &ifc->i2c_ctl->sem, 0);
  tsc_irq_register( ifc, ITC_SRC_I2C_OK, tsc_i2c_irq, (void *)ifc->i2c_ctl);
  tsc_irq_register( ifc, ITC_SRC_I2C_ERR, tsc_i2c_irq, (void *)ifc->i2c_ctl);

  return( 0);
}

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * Function name : tsc_i2c_exit
 * Prototype     : void
 * Parameters    : pointer to tsc device control structure
 * Return        : void
 *----------------------------------------------------------------------------
 * Description   : release i2c
 *
 *++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

void 
tsc_i2c_exit( struct tsc_device *ifc)
{
  if( ifc->i2c_ctl)
  {
    tsc_irq_unregister( ifc, ITC_SRC_I2C_OK);
    tsc_irq_unregister( ifc, ITC_SRC_I2C_ERR);
    mutex_destroy( &ifc->i2c_ctl->i2c_lock);
    kfree( ifc->i2c_ctl);
  }

  return;
}

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * Function name : tsc_csr_op
 * Prototype     : int
 * Parameters    : pointer to tsc device control structure
 *                 pointer to csr operation control structure
 * Return        : error/success
 *----------------------------------------------------------------------------
 * Description   : perform one of the following operations on tsc registers
 *                 - read / read with mask
 *                 - write / write with mask
 *                 - or/ or with mask
 *                 - xor / xor with mask
 *                 - and / and with mask
 *
 *++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

int 
tsc_csr_op( struct tsc_device *ifc,
	    struct tsc_ioctl_csr_op *csr_op)
{
  int retval;
  int tmp;

  retval = 0;

  /* register offset must be int aligned ( not for SMON or PCIEP) */
  if( !( csr_op->operation & (TSC_IOCTL_CSR_SMON | TSC_IOCTL_CSR_PCIEP)))
  {
    if( csr_op->offset & 3)
    {
      return( -EINVAL);
    }
    /* register offset must be below 0x1000 */
    if( csr_op->offset > TSC_CSR_OFFSET_MAX)
    {
      return( -EINVAL);
    }
  }

  mutex_lock( &ifc->csr_lock);
  switch( csr_op->operation)
  {
    case TSC_IOCTL_CSR_RD:
    case TSC_IOCTL_CSR_RDm:
    { 
      csr_op->data = ioread32( ifc->csr_ptr + csr_op->offset);
      if( csr_op->operation == TSC_IOCTL_CSR_RDm)
      {
	csr_op->data &= csr_op->mask;
      }
      break;
    }
    case TSC_IOCTL_CSR_WR:
    case TSC_IOCTL_CSR_WRm:
    {
      if( csr_op->operation == TSC_IOCTL_CSR_RDm)
      {
        tmp = ioread32( ifc->csr_ptr + csr_op->offset);
        tmp &= ~csr_op->mask;
	csr_op->data = tmp | ( csr_op->data & csr_op->mask);
      }
      iowrite32(  csr_op->data, ifc->csr_ptr + csr_op->offset);
      break;
    }
    case TSC_IOCTL_CSR_OR:
    case TSC_IOCTL_CSR_ORm:
    {
      tmp = ioread32( ifc->csr_ptr + csr_op->offset);
      if( csr_op->operation == TSC_IOCTL_CSR_ORm)
      {
	csr_op->data = (csr_op->data | tmp) & csr_op->mask;
        tmp &= ~csr_op->mask;
	csr_op->data = csr_op->data | tmp;
      }
      else
      {
	csr_op->data |= tmp;
      }
      iowrite32(  csr_op->data, ifc->csr_ptr + csr_op->offset);
      break;
    }
    case TSC_IOCTL_CSR_XOR:
    case TSC_IOCTL_CSR_XORm:
    {
      tmp = ioread32( ifc->csr_ptr + csr_op->offset);
      if( csr_op->operation == TSC_IOCTL_CSR_XORm)
      {
	csr_op->data = (csr_op->data ^ tmp) & csr_op->mask;
        tmp &= ~csr_op->mask;
	csr_op->data = csr_op->data | tmp;
      }
      else
      {
        csr_op->data ^= tmp;
      }
      iowrite32(  csr_op->data, ifc->csr_ptr + csr_op->offset);
      break;
    }
    case TSC_IOCTL_CSR_AND:
    case TSC_IOCTL_CSR_ANDm:
    {
      tmp = ioread32( ifc->csr_ptr + csr_op->offset);
      if( csr_op->operation == TSC_IOCTL_CSR_XORm)
      {
	csr_op->data = (csr_op->data & tmp) & csr_op->mask;
        tmp &= ~csr_op->mask;
	csr_op->data = csr_op->data | tmp;
      }
      else
      {
	csr_op->data &= tmp;
      }
      iowrite32(  csr_op->data, ifc->csr_ptr + csr_op->offset);
      break;
    }
    case TSC_IOCTL_CSR_SMON_RD:
    { 
      iowrite32(  csr_op->offset, ifc->csr_ptr + TSC_CSR_A7_SMON_ADDPT);
      csr_op->data = ioread32( ifc->csr_ptr + TSC_CSR_A7_SMON_DAT);
      break;
    }
    case TSC_IOCTL_CSR_SMON_WR:
    { 
      iowrite32(  csr_op->offset, ifc->csr_ptr + TSC_CSR_A7_SMON_ADDPT);
      iowrite32(  csr_op->data, ifc->csr_ptr + TSC_CSR_A7_SMON_DAT);
      break;
    }
    case TSC_IOCTL_CSR_PON_RD:
    { 
      csr_op->data = ioread32be( ifc->pon_ptr + csr_op->offset);
      break;
    }
    case TSC_IOCTL_CSR_PON_WR:
    { 
      iowrite32be(  csr_op->data, ifc->pon_ptr + csr_op->offset);
      break;
    }
    case TSC_IOCTL_CSR_PCIEP_RD:
    {
      char *csr_ptr;

      csr_ptr =  ifc->csr_ptr;
      /* check for PCIEP0 or PCIEP1 */
      if( csr_op->offset & TSC_A7_PCIEP1_ADDPT_CFG)
      {
	csr_ptr += TSC_CSR_A7_PCIE1_BASE;
        csr_op->offset &= ~TSC_A7_PCIEP1_ADDPT_CFG;
      }
      iowrite32( csr_op->offset, csr_ptr + TSC_CSR_A7_PCIEP_ADDPT);
      if( csr_op->offset & TSC_A7_PCIEP_ADDPT_DRP)
      {
	/* read PCIe EP DRP register */
        csr_op->data = ioread32( csr_ptr + TSC_CSR_A7_PCIEP_DRPDAT);
      }
      else 
      {
	/* read PCIe EP CFG register */
        csr_op->data = ioread32( csr_ptr + TSC_CSR_A7_PCIEP_CFGDAT);
      }
      break;
    }
    case TSC_IOCTL_CSR_PCIEP_WR:
    { 
      char *csr_ptr;

      csr_ptr =  ifc->csr_ptr;
      /* check for PCIEP0 or PCIEP1 */
      if( csr_op->offset & TSC_A7_PCIEP1_ADDPT_CFG)
      {
	csr_ptr += TSC_CSR_A7_PCIE1_BASE;
      }
      iowrite32(  csr_op->offset, csr_ptr + TSC_CSR_A7_PCIEP_ADDPT);
      if( csr_op->offset & TSC_A7_PCIEP_ADDPT_DRP)
      {
	/* write PCIe EP CFG register */
        iowrite32(  csr_op->data, csr_ptr + TSC_CSR_A7_PCIEP_DRPDAT);
      }
      else 
      {
	/* write PCIe EP DRP register */
        iowrite32(  csr_op->data, csr_ptr + TSC_CSR_A7_PCIEP_CFGDAT);
      }
      break;
    }
    default:
    {
      retval = -EINVAL;
    }
  }
  mutex_unlock( &ifc->csr_lock);
  return( retval);
}

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * Function name : tsc_kbuf_alloc
 * Prototype     : int
 * Parameters    : pointer to tsc device control structure
 *                 pointer to kernel buffer control structure
 * Return        : error/success
 *----------------------------------------------------------------------------
 * Description   : allocate kerne buffer suitable for DMA
 *
 *++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

int 
tsc_kbuf_alloc( struct tsc_device *ifc,
		struct tsc_ioctl_kbuf_req *r)
{
  r->k_base = (void *)pci_alloc_consistent( ifc->pdev,  r->size, &r->b_base);
  debugk(( KERN_ALERT "alloc kernel buffer : %p - %llx [%x] %lx\n", r->k_base, (long long)r->b_base, r->size, virt_to_phys(r->k_base)));
  if( !r->k_base)
  {
    return( -EFAULT);
  }
  return(0);
}
EXPORT_SYMBOL( tsc_kbuf_alloc);

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * Function name : tsc_kbuf_free
 * Prototype     : int
 * Parameters    : pointer to tsc device control structure
 *                 pointer to kernel buffer control structure
 * Return        : 0
 *----------------------------------------------------------------------------
 * Description   : free previously allocated kernel buffer
 *
 *++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

int 
tsc_kbuf_free( struct tsc_device *ifc,
	       struct tsc_ioctl_kbuf_req *r)
{
  debugk(( KERN_ALERT "free kernel buffer : %p - %llx [%x]\n", r->k_base, (long long)r->b_base, r->size));
  pci_free_consistent(  ifc->pdev, r->size, r->k_base,  r->b_base);

  return(0);
}
EXPORT_SYMBOL(tsc_kbuf_free);

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * Function name : tsc_semaphore_release
 * Prototype     : int
 * Parameters    : pointer to tsc device control structure
 *                 pointer to semaphore structure
 * Return        : error/success
 *----------------------------------------------------------------------------
 * Description   : release semaphore function
 *
 *++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

int tsc_semaphore_release(struct tsc_device *ifc, struct tsc_ioctl_semaphore *semaphore){
	semaphore_release(semaphore->idx, ifc->shm_ctl[0]->sram_ptr);
	return(0);
}

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * Function name : tsc_semaphore_get
 * Prototype     : int
 * Parameters    : pointer to tsc device control structure
 *                 pointer to semaphore structure
 * Return        : error/success
 *----------------------------------------------------------------------------
 * Description   : get semaphore function
 *
 *++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

int tsc_semaphore_get(struct tsc_device *ifc, struct tsc_ioctl_semaphore *semaphore){
	uint *tag;
	int retval = 0;
	int ret    = 0;

	tag  = (int *)kmalloc(sizeof(int), GFP_KERNEL);
	if(copy_from_user(tag, semaphore->tag, sizeof(int))){
		retval = -EFAULT;
		return retval;
	}
	ret = semaphore_get(semaphore->idx, ifc->shm_ctl[0]->sram_ptr, tag);
	kfree(tag);
	return(retval | ret);
}
