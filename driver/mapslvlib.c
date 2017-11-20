/*=========================< begin file & file header >=======================
 *  References
 *  
 *    filename : mapslvlib.c
 *    author   : JFG
 *    company  : IOxOS
 *    creation : Sept 27,2015
 *    version  : 0.0.1
 *
 *----------------------------------------------------------------------------
 *  Description
 *
 *    This file contains the low level functions to drive the address mappers
 *    implemented on the tsc.
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

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * Function name : tsc_map_slv_set sg
 * Prototype     : int
 * Parameters    : pointer to tsc device control structure
 *                 pointer to map control data structure
 *                 mapping offset
 * Return        : none
 *                 < 0  in case of error
 *----------------------------------------------------------------------------
 * Description   : allocate translation window in PCI master map
 *
 *++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

int 
tsc_map_slv_set_sg( struct tsc_device *ifc,
		    struct map_ctl *map_ctl_p,
		    int offset)
{
  struct map_blk *p;
  int mode;
  ulong rem_addr;
  int npg;
  int pg_size;
  int mmu_ptr;

  debugk(("in tsc_map_slv_set_sg( %p, %p, %x)\n", ifc, map_ctl_p, offset));
  p = map_ctl_p->map_p;
  npg = p[offset].npg;
  pg_size = map_ctl_p->pg_size;
  mode = (int)p[offset].mode | (int)(p[offset].rem_addr & 0x30000);
  rem_addr = (int)((p[offset].rem_addr >> 18) & 0x3ffff);
  debugk(("%x * %x : %lx - %x : %x : %d\n", npg, pg_size, rem_addr, mode, p[offset].flag, p[offset].usr));

  mmu_ptr = offset*8;
  switch( map_ctl_p->sg_id)
  {
    default:
    {
      return(-1);
    }
  }
  debugk(("loading MMU : %x\n", mmu_ptr));
  mutex_lock( &ifc->csr_lock);
  iowrite32( mmu_ptr, ifc->csr_ptr + TSC_CSR_MMUADD);
  debugk(("mode = %x : rem_addr = %lx\n", (int)p[offset].mode, p[offset].rem_addr));
  for( npg = 0; npg < p[offset].npg; npg++)
  {
    uint tmp;

    iowrite32( (int)p[offset].mode, ifc->csr_ptr + TSC_CSR_MMUDAT);
    rem_addr = (int)(p[offset].rem_addr + (npg*pg_size));
    debugk(("rem_addr = %lx", rem_addr));
    tmp = (uint)(rem_addr >> 16) & 0xfff0;
    debugk((" - %x - 0", tmp));
    iowrite32( tmp, ifc->csr_ptr + TSC_CSR_MMUDAT);
    iowrite32( 0, ifc->csr_ptr + TSC_CSR_MMUDAT);
    tmp = (uint)(rem_addr >> 4) & 0xfff0;
    debugk((" - %x\n", tmp));
    iowrite32( tmp, ifc->csr_ptr + TSC_CSR_MMUDAT);
  }
  mutex_unlock( &ifc->csr_lock);
  return(0);
}

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * Function name : tsc_map_slv_clear_sg
 * Prototype     : int
 * Parameters    : pointer to TSC device control structure
 *                 pointer to map control data structure
                   mapping offset
                   number of pages to clear
 * Return        : none
 *                 < 0  in case of error
 *----------------------------------------------------------------------------
 * Description   : allocate translation window in PCI master map
 *
 *++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

int 
tsc_map_slv_clear_sg( struct tsc_device *ifc,
		      struct map_ctl *map_ctl_p,
		      int offset,
		      int npg)
{
  struct map_blk *p;
  int mmu_ptr;

  debugk(("in tsc_map_slv_clear_sg( %p, %p, %x, %x)\n", ifc, map_ctl_p, offset, npg));
  p = map_ctl_p->map_p;

  mmu_ptr = offset*8;
  switch( map_ctl_p->sg_id)
  {
    default:
    {
      return(-1);
    }
  }
  debugk(("clearing MMU : %x %x\n", mmu_ptr, npg));
  mutex_lock( &ifc->csr_lock);
  iowrite32( mmu_ptr, ifc->csr_ptr + TSC_CSR_MMUADD);
  while( npg--)
  {
    iowrite32( 0, ifc->csr_ptr + TSC_CSR_MMUDAT);
    iowrite32( 0, ifc->csr_ptr + TSC_CSR_MMUDAT);
    iowrite32( 0, ifc->csr_ptr + TSC_CSR_MMUDAT);
    iowrite32( 0, ifc->csr_ptr + TSC_CSR_MMUDAT);
  }
  mutex_unlock( &ifc->csr_lock);
  return(0);
}

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * Function name : tsc_map_slv_set_mode
 * Prototype     : int
 * Parameters    : pointer to TSC device control structure
 *                 pointer to mapping mode control data structure
 * Return        : encoded hw mode field
 *                 0  in case of error
 *----------------------------------------------------------------------------
 * Description   : allocate translation window in PCI master map
 *
 *++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

short 
tsc_map_slv_set_mode( struct tsc_device *ifc,
		      struct tsc_ioctl_map_mode *m)
{
  short mode;

  mode = 0;
  if( m->space == MAP_SPACE_PCIE)
  {
    mode |= TSC_MMUDAT_DES_PCIE;
  }
  else if( m->space == MAP_SPACE_SHM)
  {
    mode |= TSC_MMUDAT_DES_SHM;
  }
  else if( m->space == MAP_SPACE_USR)
  {
    mode |= TSC_MMUDAT_DES_USR;
  }

  mode |= TSC_MMUDAT_PG_ENA | TSC_MMUDAT_WR_ENA;

  return( mode);

}

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * Function name : tsc_map_slv_alloc
 * Prototype     : int
 * Parameters    : pointer to TSC device control structure
 *                 mapper identifier
 *                 pointer to mapping request data structure
 *                  - remote address
 *                  - size of requested window
 *                  - remote access mode
 * Return        : mapping offset
 *                 < 0  in case of error
 *----------------------------------------------------------------------------
 * Description   : allocate translation window in PCI slave map
 *
 *++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

int 
tsc_map_slv_alloc( struct tsc_device *ifc,
                   struct tsc_ioctl_map_win *w)
{
  struct map_ctl *map_ctl_p;
  struct map_req map_req;
  int retval;

  debugk(("in tsc_map_slv_alloc( %p)\n", ifc));

  return( MAP_ERR_BAD_SGID);

  debugk(("create new slave window: %lx, %x, %x, %x\n", w->req.rem_addr, w->req.size, w->req.mode, w->req.mode.sg_id));

  /* set mapping parameters */
  map_req.rem_addr = w->req.rem_addr;
  map_req.size = w->req.size;

  /* set requested mode */
  map_req.mode = tsc_map_slv_set_mode( ifc, &w->req.mode);
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
    w->req.loc_addr = map_req.loc_base + map_ctl_p->win_base;
    w->req.rem_addr = map_req.rem_base;;
    w->req.size = map_req.win_size;
    debugk(("local address = %lx\n", map_req.loc_addr));
    tsc_map_slv_set_sg( ifc, map_ctl_p, w->pg_idx);
  }
  else
  {
    w->pg_idx = MAP_IDX_INV;
  }
  mutex_unlock( &map_ctl_p->map_lock);
  return( retval);
}
EXPORT_SYMBOL( tsc_map_slv_alloc);

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * Function name : tsc_map_slv_free
 * Prototype     : int
 * Parameters    : pointer to TSC device control structure
 *                 mapper identifier
 *                 offset returne by allocation function
 * Return        : error/success
 *----------------------------------------------------------------------------
 * Description   : allocate translation window in PCI slave map
 *
 *++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

int 
tsc_map_slv_free( struct tsc_device *ifc,
		  int sg_id,
		  uint offset)
{
  struct map_ctl *map_ctl_p;
  int npg;

  return( MAP_ERR_BAD_SGID);

  debugk(("calling map_blk_free( %p, %x)\n", map_ctl_p, offset));
  mutex_lock( &map_ctl_p->map_lock);
  npg = map_blk_free( map_ctl_p, offset);
  debugk(("npg = %x\n", npg));
  if( npg >= 0)
  {
    tsc_map_slv_clear_sg( ifc, map_ctl_p, offset, npg);
    npg = 0;
  }
  mutex_unlock( &map_ctl_p->map_lock);
  return( npg);
}
EXPORT_SYMBOL( tsc_map_slv_free);

