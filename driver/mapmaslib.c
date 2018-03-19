/*=========================< begin file & file header >=======================
 *  References
 *  
 *    filename : mapmaslib.c
 *    author   : JFG, XP
 *    company  : IOxOS
 *    creation : Sept 27,2015
 *
 *----------------------------------------------------------------------------
 *  Description
 *
 *    This file contains the low level functions to drive the address mappers
 *    implemented on the TSC.
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

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * Function name : tsc_map_mas_set sg
 * Prototype     : int
 * Parameters    : pointer to tsc device control structure
 *                 pointer to map control data structure
 *                 mapping offset
 * Return        : none
 *                 < 0  in case of error
 *----------------------------------------------------------------------------
 * Description   : set sg mapping
 *
 *++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

int 
tsc_map_mas_set_sg( struct tsc_device *ifc,
		    struct map_ctl *map_ctl_p,
		    int offset)
{
  struct map_blk *p;
  int mode;
  int rem_addr;
  int npg;
  int pg_size;
  int mmu_ptr;
  int pcie_off = 0;

  debugk(("in tsc_map_mas_set_sg( %p, %p, %x)\n", ifc, map_ctl_p, offset));
  p = map_ctl_p->map_p;
  npg = p[offset].npg;
  pg_size = map_ctl_p->pg_size;
  mode = (int)p[offset].mode | (int)(p[offset].rem_addr & 0x30000);
  rem_addr = (int)((p[offset].rem_addr >> 18) & 0x3ffff);
  debugk(("%x * %x : %x - %x : %x : %d\n", npg, pg_size, rem_addr, mode, p[offset].flag, p[offset].usr));

  mmu_ptr = offset*4;
  switch( map_ctl_p->sg_id)
  {
    case MAP_MAS_PCIE_MEM:
    {
      mmu_ptr |= TSC_PCIE_MMUADD_MEM_SEL;
      break;
    }
    case MAP_MAS_PCIE_PMEM:
    {
      mmu_ptr |= TSC_PCIE_MMUADD_PMEM_SEL;
      break;
    }
    default:
    {
      return(MAP_ERR_BAD_SGID);
    }
  }
  debugk(("loading MMU : %x\n", mmu_ptr));

  /* choose TOSCA Agent #1 PCIe_EP CSR area (when accessing Tosca in remote)
     in that case registers are between 0x400 - 0x7ff of CSR area */
  if (ifc->pdev->device == PCI_DEVICE_ID_IOXOS_TSC_CENTRAL_2) {
	  pcie_off = 0x400;
  } else {
	  pcie_off = 0;
  }

  mutex_lock( &ifc->csr_lock);
  iowrite32( mmu_ptr, ifc->csr_ptr + pcie_off + TSC_CSR_PCIE_MMUADD);
  for( npg = 0; npg < p[offset].npg; npg++)
  {
    mode = (int)p[offset].mode | (int)((p[offset].rem_addr + (npg*pg_size)) & 0x30000);
    debugk(("mode = %x\n", mode));
    iowrite32( mode, ifc->csr_ptr + pcie_off + TSC_CSR_PCIE_MMUDAT);
    rem_addr = (int)(((p[offset].rem_addr + (npg*pg_size)) >> 18) & 0x3ffff);
    debugk(("rem_addr = %x\n", rem_addr));
    iowrite32( rem_addr, ifc->csr_ptr + pcie_off + TSC_CSR_PCIE_MMUDAT);
  }
  mutex_unlock( &ifc->csr_lock);
  return(0);
}

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * Function name : tsc_map_mas_clear_sg
 * Prototype     : int
 * Parameters    : pointer to tsc device control structure
 *                 pointer to map control data structure
 *                 mapping offset
 *                 number of pages to clear
 * Return        : < 0  in case of error
 *----------------------------------------------------------------------------
 * Description   : clear sg for mapping
 *++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

int 
tsc_map_mas_clear_sg( struct tsc_device *ifc,
		      struct map_ctl *map_ctl_p,
		      int offset,
		      int npg)
{
  struct map_blk *p;
  int mmu_ptr;
  int pcie_off = 0;

  debugk(("in tsc_map_mas_clear_sg( %p, %p, %x, %x)\n", ifc, map_ctl_p, offset, npg));
  p = map_ctl_p->map_p;

  mmu_ptr = offset*4;
  switch( map_ctl_p->sg_id)
  {
    case MAP_MAS_PCIE_MEM:
    {
      mmu_ptr |= TSC_PCIE_MMUADD_MEM_SEL;
      break;
    }
    case MAP_MAS_PCIE_PMEM:
    {
      mmu_ptr |= TSC_PCIE_MMUADD_PMEM_SEL;
      break;
    }
    default:
    {
      return(MAP_ERR_BAD_SGID);
    }
  }
  debugk(("clearing MMU : %x %x\n", mmu_ptr, npg));

  /* choose TOSCA Agent #1 PCIe_EP CSR area (when accessing Tosca in remote)
     in that case registers are between 0x400 - 0x7ff of CSR area */
  if (ifc->pdev->device == PCI_DEVICE_ID_IOXOS_TSC_CENTRAL_2) {
	  pcie_off = 0x400;
  } else {
	  pcie_off = 0;
  }

  mutex_lock( &ifc->csr_lock);
  iowrite32( mmu_ptr, ifc->csr_ptr + pcie_off + TSC_CSR_PCIE_MMUADD);
  while( npg--)
  {
    iowrite32( 0, ifc->csr_ptr + pcie_off + TSC_CSR_PCIE_MMUDAT);
    iowrite32( 0, ifc->csr_ptr + pcie_off + TSC_CSR_PCIE_MMUDAT);
  }
  mutex_unlock( &ifc->csr_lock);
  return(0);
}
/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * Function name : tsc_map_mas_set_mode
 * Prototype     : int
 * Parameters    : pointer to tsc device control structure
 *                 pointer to mapping mode control data structure
 * Return        : encoded hw mode field
 *                 0  in case of error
 *----------------------------------------------------------------------------
 * Description   : set mode
 *
 *++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

short 
tsc_map_mas_set_mode( struct tsc_device *ifc,
		      struct tsc_ioctl_map_mode *m)
{
  short mode;

  mode = 0;
  if( m->space == MAP_SPACE_SHM1)
  {
    mode |= TSC_PCIE_MMUDAT_DES_SHM1;
  }
  else if( m->space == MAP_SPACE_SHM2)
  {
    mode |= TSC_PCIE_MMUDAT_DES_SHM2;
  }
  else if( m->space == MAP_SPACE_USR)
  {
    mode |= TSC_PCIE_MMUDAT_DES_USR;
  }
  else if( m->space == MAP_SPACE_USR1)
  {
    mode |= TSC_PCIE_MMUDAT_DES_USR1;
  }
  else if( m->space == MAP_SPACE_USR2)
  {
    mode |= TSC_PCIE_MMUDAT_DES_USR2;
  }
  if(mode){
    mode |= (1<<0) | (1<<1);    // Enable page, Enable write
  }
  return( mode);

}
/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * Function name : tsc_map_mas_alloc
 * Prototype     : int
 * Parameters    : pointer to tsc device control structure
 *                 pointer to mapping request data structure
 * Return        : mapping offset
 *                 < 0  in case of error
 *----------------------------------------------------------------------------
 * Description   : allocate translation window in PCI master map
 *
 *++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

int 
tsc_map_mas_alloc( struct tsc_device *ifc,
                   struct tsc_ioctl_map_win *w)
{
  struct map_ctl *map_ctl_p;
  struct map_req map_req;
  int retval;

  debugk(("in tsc_map_mas_alloc( %p)\n", ifc));

  if(  w->req.mode.sg_id == MAP_MAS_PCIE_PMEM)
  {
    map_ctl_p = ifc->map_mas_pci_pmem;
  }
  else if(  w->req.mode.sg_id == MAP_MAS_PCIE_MEM)
  {
    map_ctl_p = ifc->map_mas_pci_mem;
  }
  else 
  {
    return( MAP_ERR_BAD_SGID);
  }

  /* set mapping parameters */
  map_req.rem_addr = w->req.rem_addr;
  map_req.size = w->req.size;

  /* set request mode */
  map_req.mode = tsc_map_mas_set_mode(ifc, &w->req.mode);
  map_req.flag = 0;
  map_req.sg_id = w->req.mode.sg_id;

  /* allocate block in mapper */
  mutex_lock( &map_ctl_p->map_lock);
  if( w->req.mode.flags & MAP_FLAG_FORCE)
  {
    map_req.loc_addr = w->req.loc_addr;
    retval = map_blk_force( map_ctl_p, &map_req);
  }
  else 
  {
    retval = map_blk_alloc( map_ctl_p, &map_req);
  }
  debugk(("offset = %x\n", retval));
  if( retval >= 0)
  {
    w->pg_idx = map_req.offset;
    w->req.loc_addr = map_req.loc_addr + map_ctl_p->win_base;
    debugk(("local address = %lx\n", map_req.loc_addr));
    tsc_map_mas_set_sg( ifc, map_ctl_p, w->pg_idx);
  }
  else
  {
    w->pg_idx = MAP_IDX_INV;
  }
  mutex_unlock( &map_ctl_p->map_lock);

  return( retval);
}
EXPORT_SYMBOL( tsc_map_mas_alloc);

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * Function name : tsc_map_mas_modify
 * Prototype     : int
 * Parameters    : pointer to tsc device control structure
 *                 pointer to mapping request data structure
 *                 remote address
 * Return        : < 0  in case of error
 *----------------------------------------------------------------------------
 * Description   : modify translation window in pci master map
 *
 *++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

int 
tsc_map_mas_modify( struct tsc_device *ifc,
		    struct tsc_ioctl_map_win *w,
		    ulong *rem_base)

{
  struct map_ctl *map_ctl_p;
  struct map_req map_req;
  int retval;

  debugk(("in tsc_map_mas_modify( %p, %x, %x, %lx)\n", ifc, w->req.mode.sg_id, w->pg_idx, w->req.rem_addr));

  if(  w->req.mode.sg_id == MAP_MAS_PCIE_PMEM)
  {
    map_ctl_p = ifc->map_mas_pci_pmem;
  }
  else if(  w->req.mode.sg_id == MAP_MAS_PCIE_MEM)
  {
    map_ctl_p = ifc->map_mas_pci_mem;
  }
  else 
  {
    return( MAP_ERR_BAD_SGID);
  }

  /* set mapping parameters */
  map_req.rem_addr = w->req.rem_addr;
  map_req.offset = w->pg_idx;

  /* set request mode */
  map_req.mode = tsc_map_mas_set_mode( ifc, &w->req.mode);
  map_req.flag = 0;
  map_req.sg_id = w->req.mode.sg_id;

  /* modify block in mapper */
  mutex_lock( &map_ctl_p->map_lock);
  retval = map_blk_modify( map_ctl_p, &map_req, rem_base);
  debugk(("rem_base = %lx\n", *rem_base));
  if( !retval)
  {
    tsc_map_mas_set_sg(ifc, map_ctl_p, w->pg_idx);
  }
  mutex_unlock( &map_ctl_p->map_lock);

  return( retval);
}
EXPORT_SYMBOL( tsc_map_mas_modify);

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * Function name : tsc_map_mas_get
 * Prototype     : int
 * Parameters    : pointer to tsc device control structure
 *                 mapping window structure
 * Return        : error/success
 *----------------------------------------------------------------------------
 * Description   : acquire master mapping
 *
 *++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

int 
tsc_map_mas_get( struct tsc_device *ifc,
                 struct tsc_ioctl_map_win *w)
{
  struct tsc_ioctl_map_sts *s;
  struct map_ctl *map_ctl_p;
  struct map_blk *p;

  s = &w->sts;
  debugk(("entering tsc_map_mas_get( %p, %x, %x)\n", ifc, s->mode.sg_id, w->pg_idx));
  if( s->mode.sg_id == MAP_MAS_PCIE_PMEM)
  {
    map_ctl_p = ifc->map_mas_pci_pmem;
  }
  else if( s->mode.sg_id == MAP_MAS_PCIE_MEM)
  {
    map_ctl_p = ifc->map_mas_pci_mem;
  }
  else 
  {
    return( MAP_ERR_BAD_SGID);
  }
  if( (w->pg_idx < 0) || (w->pg_idx >= map_ctl_p->pg_num))
  {
    return( -1);
  }
  mutex_lock( &map_ctl_p->map_lock);
  p = map_ctl_p->map_p;
  s->rem_base = p[w->pg_idx].rem_addr;
  s->loc_base = map_ctl_p->win_base + (w->pg_idx * map_ctl_p->pg_size);
  s->size = p[w->pg_idx].npg * map_ctl_p->pg_size;
  s->mode.hw = (short)p[w->pg_idx].mode;
  mutex_unlock( &map_ctl_p->map_lock);
  return( 0);
}
EXPORT_SYMBOL( tsc_map_mas_get);

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * Function name : tsc_map_mas_free
 * Prototype     : int
 * Parameters    : pointer to tsc device control structure
 *                 mapper identifier
 *                 offset returned by allocation function
 * Return        : error/success
 *----------------------------------------------------------------------------
 * Description   : free master mapping
 *
 *++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

int 
tsc_map_mas_free( struct tsc_device *ifc,
		  int sg_id,
		  uint offset)
{
  struct map_ctl *map_ctl_p;
  int npg;

  if( sg_id == MAP_MAS_PCIE_PMEM)
  {
    map_ctl_p = ifc->map_mas_pci_pmem;
  }
  else if( sg_id == MAP_MAS_PCIE_MEM)
  {
    map_ctl_p = ifc->map_mas_pci_mem;
  }
  else 
  {
    return( MAP_ERR_BAD_SGID);
  }
  debugk(("calling map_blk_free( %p, %x)\n", map_ctl_p, offset));
  mutex_lock( &map_ctl_p->map_lock);
  npg = map_blk_free( map_ctl_p, offset);
  if( npg >= 0)
  {
    tsc_map_mas_clear_sg( ifc, map_ctl_p, offset, npg);
    npg = 0;
  }
  mutex_unlock( &map_ctl_p->map_lock);
  return( npg);
}
EXPORT_SYMBOL( tsc_map_mas_free);
