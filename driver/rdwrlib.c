/*=========================< begin file & file header >=======================
 *  References
 *  
 *    filename : rdwrlib.c
 *    author   : JFG
 *    company  : IOxOS
 *    creation : may 27,2015
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

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * Function name : tsc_rdwr_init
 * Prototype     : int
 * Parameters    : pointer to IFC1211 device control structure
 * Return        : error/success
 *----------------------------------------------------------------------------
 * Description   : prepare translation windows to access RDWR
 *
 *++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

int 
tsc_rdwr_init( struct ifc1211_device *ifc)
{
  int retval;
  struct tsc_ioctl_map_win mas_win;

  debugk(("in tsc_rdwr_init( %p)\n", ifc));

  /* Check PCI MEM mapping has been initialized */
  ifc->rdwr_ctl = NULL;
  if( !ifc->map_mas_pci_mem)
  {
    return(-1);
  }
  ifc->rdwr_ctl = kzalloc( sizeof( struct rdwr_ctl), GFP_KERNEL);
  if( !ifc->rdwr_ctl)
  {
    return(-1);
  }
  retval = -1;
  /* prepare dynamic window to access the RDWR bus  */
  memset(  &mas_win, 0, sizeof( mas_win));
  mas_win.req.rem_addr = 0;                          /* point to RDWR  base */
  mas_win.req.size = IFC1211_PCIE_MMU_PG_64K;      /* map just 1 page */
  mas_win.req.mode.space = MAP_SPACE_SHM;
  mas_win.req.mode.sg_id = MAP_ID_MAS_PCIE_MEM ;
  ifc->rdwr_ctl->offset = tsc_map_mas_alloc( ifc, &mas_win);
  debugk(("rdwr loc_addr = %lx offset: %x\n",  mas_win.req.loc_addr, ifc->rdwr_ctl->offset));
  if( ifc->rdwr_ctl->offset >= 0)
  {
    ifc->rdwr_ctl->rdwr_ptr = ioremap_nocache( mas_win.req.loc_addr,  IFC1211_PCIE_MMU_PG_64K);
    retval = 0;
  }

  return( retval);

}

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * Function name : tsc_rdwr_exit
 * Prototype     : int
 * Parameters    : pointer to IFC1211 device control structure
 * Return        : error/success
 *----------------------------------------------------------------------------
 * Description   : release resources allocated for RDWR control
 *
 *++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

void 
tsc_rdwr_exit( struct ifc1211_device *ifc)
{
  debugk(("in tsc_rdwr_exit( %p)\n", ifc));

  if( ifc->rdwr_ctl)
  {
    if( ifc->rdwr_ctl->rdwr_ptr)
    {
      tsc_map_mas_free( ifc, MAP_ID_MAS_PCIE_MEM, ifc->rdwr_ctl->offset);
      iounmap( ifc->rdwr_ctl->rdwr_ptr);
    }
    kfree( ifc->rdwr_ctl);
  }
}

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * Function name : tsc_read_blk
 * Prototype     : int
 * Parameters    : pointer to IFC1211 device control structure
 *                 pointer to read/write control structure
 * Return        : error/success
 *----------------------------------------------------------------------------
 * Description   : read a block of data from a remote IO address to a user
 *                 buffer
 *
 *++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

static void data_read( void *d, void __iomem *s, int len, int ds){
  int n0, n1, n2, i;
  int msk;

  msk = ds-1;
  n0 = (ds - ((long)s&msk))&msk;
  i = n0;
  while(i--)
  {
    *(char *)d++ = *(char *)s++;
  }
  if( len > n0)
  {
    n2 = ((long)s+len)&(ds-1);
    if( len > (n0+n2))
    {
      n1 = len - (n0+n2);
      while( n1 > 0)
      {
	switch( ds)
	{
	  case 1:
	  {
	    *(char *)d = *(char *)s;
	    break;
	  }
	  case 2:
	  {
	    *(short *)d = *(short *)s;
	    break;
	  }
	  case 4:
	  {
	    *(int *)d = *(int *)s;
	    break;
	  }
	  case 8:
	  {
	    *(long long *)d = *(long long *)s;
	    break;
	  }
	}
	n1 -= ds; s += ds; d += ds;
      }
    }
    while( n2--)
    {
      *(char *)d++ = *(char *)s++;
    }
  }
}

int 
tsc_read_blk( struct ifc1211_device *ifc,
	      struct tsc_ioctl_rdwr *rw)
{

  int retval;
  char *ubuf, *kbuf, *kaddr, *kbase;
  ulong rem_addr, rem_base;
  struct tsc_ioctl_map_win win;
  int first, last, nblk, mask;
  int ds;

  retval = 0;
  ubuf = rw->buf;
  rem_addr = rw->rem_addr;
  ds = RDWR_MODE_GET_DS(rw->m.ads);
  kbuf = (char *)kzalloc( IFC1211_PCIE_MMU_PG_64K, GFP_KERNEL);
  kbase = ifc->rdwr_ctl->rdwr_ptr;
  mask = IFC1211_PCIE_MMU_PG_64K-1;
  debugk(("in tsc_read_blk() ; ubuf = %p  kbuf = %p kbase = %p ds = %d\n", ubuf, kbuf, kbase, ds));
  nblk = ((rem_addr + rw->len)/IFC1211_PCIE_MMU_PG_64K) - (rem_addr/IFC1211_PCIE_MMU_PG_64K);
  first = 0;
  last = 0;
  if( !nblk)
  {
    first = rw->len;
  }
  else
  {
    first = IFC1211_PCIE_MMU_PG_64K - ( rem_addr & mask);
    last = (rem_addr + rw->len)&mask;
    nblk -= 1;
  }
  debugk(("in tsc_read_blk() : first=%x - nblk = %x - last = %x\n", first, nblk, last));
  /* transfer first byte from rem_addr  to kbuf and ubuf */
  rw->len = 0;
  win.pg_idx = ifc->rdwr_ctl->offset;
  win.req.mode.sg_id =  MAP_MAS_PCIE_MEM;
  win.req.mode.space =  rw->m.space;
  win.req.mode.am =  rw->m.am;
  win.req.mode.swap =  rw->m.swap;
  if( first)
  {
    win.req.rem_addr = rem_addr;
    retval = tsc_map_mas_modify( ifc, &win, &rem_base);
    if( retval)
    {
      retval = -EINVAL;
      goto tsc_read_blk_exit;
    }
    kaddr = kbase + (rem_addr - rem_base);
    data_read( kbuf, kaddr, first, ds);
    if( copy_to_user( ubuf, kbuf, first))
    {
      retval = -EFAULT;
      goto tsc_read_blk_exit;
    }
    rem_addr += first;
    ubuf += first;
    rw->len += first;
  }
  /* transfer nblk*64K from rem_addr+first to kbuf and ubuf */
  while( nblk--)
  {
    win.req.rem_addr = rem_addr;
    retval = tsc_map_mas_modify( ifc, &win, &rem_base);
    if( retval)
    {
      retval = -EINVAL;
      goto tsc_read_blk_exit;
    }
    kaddr = kbase + (rem_addr - rem_base);
    data_read( kbuf, kaddr, IFC1211_PCIE_MMU_PG_64K, ds);
    if( copy_to_user( ubuf, kbuf, IFC1211_PCIE_MMU_PG_64K))
    {
      retval = -EFAULT;
      goto tsc_read_blk_exit;
    }
    rem_addr += IFC1211_PCIE_MMU_PG_64K;
    ubuf += IFC1211_PCIE_MMU_PG_64K;
    rw->len += IFC1211_PCIE_MMU_PG_64K;
  }
  /* transfer last byte from rem_addr+first +nblk*64k to kbuf and ubuf */
  if( last)
  {
    win.req.rem_addr = rem_addr;
    retval = tsc_map_mas_modify( ifc, &win, &rem_base);
    if( retval)
    {
      retval = -EINVAL;
      goto tsc_read_blk_exit;
    }
    kaddr = kbase + (rem_addr - rem_base);
    data_read( kbuf, kaddr, last, ds);
    if( copy_to_user( ubuf, kbuf, last))
    {
      retval = -EFAULT;
      goto tsc_read_blk_exit;
    }
    rem_addr += last;
    ubuf += last;
    rw->len += last;
  }
  debugk(("data transfer from %lx to %lx [%lx]\n", rw->rem_addr, rem_addr, rem_addr-rw->rem_addr));

tsc_read_blk_exit:
  kfree(kbuf);
  return( retval);

}

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * Function name : tsc_read_sgl
 * Prototype     : int
 * Parameters    : pointer to IFC1211 device control structure
 *                 pointer to read/write control structure
 * Return        : error/success
 *----------------------------------------------------------------------------
 * Description   : read single data from a remote IO address to a user
 *                 buffer
 *
 *++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
int 
tsc_read_sgl( struct ifc1211_device *ifc,
	      struct tsc_ioctl_rdwr *rw,
	      int loop)
{
  int retval;
  void *kaddr;
  int kbuf[2];
  ulong rem_base;
  struct tsc_ioctl_map_win win;
  int ds;

  retval = 0;
  win.pg_idx = ifc->rdwr_ctl->offset;
  win.req.mode.sg_id =  MAP_MAS_PCIE_MEM;
  win.req.mode.space =  rw->m.space;
  win.req.mode.am =  rw->m.am;
  win.req.mode.swap =  rw->m.swap;
  win.req.rem_addr = rw->rem_addr;
  retval = tsc_map_mas_modify( ifc, &win, &rem_base);
  if( retval)
  {
    retval = -EINVAL;
    goto tsc_read_sgl_exit;
  }
  kaddr = ifc->rdwr_ctl->rdwr_ptr + (rw->rem_addr - rem_base);
  ds = RDWR_MODE_GET_DS(rw->m.ads);
  switch( ds)
  {
    case RDWR_SIZE_BYTE:
    {
      if(loop)
      {
	loop = rw->len;
	while(loop--)
	{
	  *(char *)kbuf = *(char *)kaddr;
	}
      }
      else 
      {
	*(char *)kbuf = *(char *)kaddr;
      }
      break;
    }
    case RDWR_SIZE_SHORT:
    {
      if(loop)
      {
	loop = rw->len/2;
	while(loop--)
	{
	  *(short *)kbuf = *(short *)kaddr;
	}
      }
      else 
      {
	*(short *)kbuf = *(short *)kaddr;
      }
      break;
    }
     case RDWR_SIZE_INT:
    {
      if(loop)
      {
	loop = rw->len/4;
	while(loop--)
	{
	  *(int *)kbuf = *(int *)kaddr;
	}
      }
      else 
      {
	*(int *)kbuf = *(int *)kaddr;
      }
      break;
    }
     case RDWR_SIZE_DBL:
    {
      if(loop)
      {
	loop = rw->len/8;
	while(loop--)
	{
	  *(long long *)kbuf = *(long long *)kaddr;
	}
      }
      else 
      {
	*(long long *)kbuf = *(long long *)kaddr;
      }
      break;
    }
  }
  if( copy_to_user( (void *)rw->buf, kbuf, ds))
  {
    retval = -EFAULT;
  }
tsc_read_sgl_exit:
  return( retval);
}

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * Function name : tsc_rem_read
 * Prototype     : int
 * Parameters    : pointer to IFC1211 device control structure
 *                 pointer to read/write control structure
 * Return        : error/success
 *----------------------------------------------------------------------------
 * Description   : read from a remote IO address and copy to a user buffer
 *
 *++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
int 
tsc_rem_read( struct ifc1211_device *ifc,
	      struct tsc_ioctl_rdwr *rw)
{
  if( rw->len & RDWR_LOOP)
  {
    /* loop data write */
    rw->len &= ~RDWR_LOOP;
    return( tsc_read_sgl( ifc, rw, 1));
  }
  else if( rw->len == 0)
  {
    return( tsc_read_sgl( ifc, rw, 0));
  }
  else 
  {
    return( tsc_read_blk( ifc, rw));
  }
}

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * Function name : tsc_write_blk
 * Prototype     : int
 * Parameters    : pointer to IFC1211 device control structure
 *                 pointer to read/write control structure
 * Return        : error/success
 *----------------------------------------------------------------------------
 * Description   : write a block of data from a user buffer to a remote IO
 *                 address
 *
 *++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

static void
data_write( void *d, void *s, int len, int ds, int *wpr)
{
  int n0, n1, n2, i;
  int msk;
  int cnt;

  msk = ds-1;
  n0 = (ds - ((long)d&msk))&msk;
  i = n0;
  while(i--)
  {
    *(char *)d++ = *(char *)s++;
  }
  cnt = 0;
  if( len > n0)
  {
    n2 = ((long)d+len)&(ds-1);
    if( len > (n0+n2))
    {
      n1 = len - (n0+n2);
      while( n1 > 0)
      {
	if( !(cnt&0xf) && wpr)
	{
	  int tmp, tmo;
	  tmo = 16;
	  do
	  {
	    tmp =  ioread32( wpr);
	  } while( (tmp & IFC1211_ILOC_SPI_WPOST_BUSY) && tmo--);
	}
	switch( ds)
	{
	  case 1:
	  {
	    *(char *)d = *(char *)s;
	    break;
	  }
	  case 2:
	  {
	    *(short *)d = *(short *)s;
	    break;
	  }
	  case 4:
	  {
	    *(int *)d = *(int *)s;
	    break;
	  }
	  case 8:
	  {
	    *(long long *)d = *(long long *)s;
	    break;
	  }
	}
	n1 -= ds; s += ds; d += ds; cnt++;
      }
    }
    while( n2--)
    {
      *(char *)d++ = *(char *)s++;
    }
  }
}

int 
tsc_write_blk( struct ifc1211_device *ifc,
	       struct tsc_ioctl_rdwr *rw)
{
  int retval;
  char *ubuf, *kbuf, *kaddr, *kbase;
  ulong rem_addr, rem_base;
  struct tsc_ioctl_map_win win;
  int first, last, nblk, mask;
  int ds;
  int *wpost_ready;

  retval = 0;
  wpost_ready = NULL;
  ubuf = rw->buf;
  rem_addr = rw->rem_addr;
  ds = RDWR_MODE_GET_DS(rw->m.ads);
  kbuf = (char *)kzalloc( IFC1211_PCIE_MMU_PG_64K, GFP_KERNEL);
  kbase = ifc->rdwr_ctl->rdwr_ptr;
  mask = IFC1211_PCIE_MMU_PG_64K-1;
  debugk(("in tsc_write() ; ubuf = %p  kbuf = %p kbase = %p ds = %d\n", ubuf, kbuf, kbase, ds));
  nblk = ((rem_addr + rw->len)/IFC1211_PCIE_MMU_PG_64K) - (rem_addr/IFC1211_PCIE_MMU_PG_64K);
  first = 0;
  last = 0;
  if( !nblk)
  {
    first = rw->len;
  }
  else
  {
    first = IFC1211_PCIE_MMU_PG_64K - ( rem_addr & mask);
    last = (rem_addr + rw->len)&mask;
    nblk -= 1;
  }
  debugk(("in tsc_write() : first=%x - nblk = %x - last = %x\n", first, nblk, last));
  /* transfer first byte from rem_addr  to kbuf and ubuf */
  rw->len = 0;
  win.pg_idx = ifc->rdwr_ctl->offset;
  win.req.mode.sg_id =  MAP_MAS_PCIE_MEM;
  win.req.mode.space =  rw->m.space;
  win.req.mode.am =  rw->m.am;
  win.req.mode.swap =  rw->m.swap;
  if( first)
  {
    win.req.rem_addr = rem_addr;
    retval = tsc_map_mas_modify( ifc, &win, &rem_base);
    if( retval)
    {
      retval = -EINVAL;
      goto tsc_write_exit;
    }
    kaddr = kbase + (rem_addr - rem_base);
    if( copy_from_user( kbuf, ubuf, first))
    {
      retval = -EFAULT;
      goto tsc_write_exit;
    }
    data_write( kaddr, kbuf, first, ds, wpost_ready);
    rem_addr += first;
    ubuf += first;
    rw->len += first;
  }
  /* transfer nblk*64K from rem_addr+first to kbuf and ubuf */
  while( nblk--)
  {
    win.req.rem_addr = rem_addr;
    retval = tsc_map_mas_modify( ifc, &win, &rem_base);
    if( retval)
    {
      retval = -EINVAL;
      goto tsc_write_exit;
    }
    kaddr = kbase + (rem_addr - rem_base);
    if( copy_from_user( kbuf, ubuf, IFC1211_PCIE_MMU_PG_64K))
    {
      retval = -EFAULT;
      goto tsc_write_exit;
    }
    data_write( kaddr, kbuf, IFC1211_PCIE_MMU_PG_64K, ds, wpost_ready);
    rem_addr += IFC1211_PCIE_MMU_PG_64K;
    ubuf += IFC1211_PCIE_MMU_PG_64K;
    rw->len += IFC1211_PCIE_MMU_PG_64K;
  }
  /* transfer last byte from rem_addr+first +nblk*64k to kbuf and ubuf */
  if( last)
  {
    win.req.rem_addr = rem_addr;
    retval = tsc_map_mas_modify( ifc, &win, &rem_base);
    if( retval)
    {
      retval = -EINVAL;
      goto tsc_write_exit;
    }
    kaddr = kbase + (rem_addr - rem_base);
    if( copy_from_user( kbuf, ubuf, last))
    {
      retval = -EFAULT;
      goto tsc_write_exit;
    }
    data_write( kaddr, kbuf, last, ds, wpost_ready);
    rem_addr += last;
    ubuf += last;
    rw->len += last;
  }

tsc_write_exit:
  kfree(kbuf);
  return( retval);

}

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * Function name : tsc_write_sgl
 * Prototype     : int
 * Parameters    : pointer to IFC1211 device control structure
 *                 pointer to read/write control structure
 * Return        : error/success
 *----------------------------------------------------------------------------
 * Description   : write single data from a remote IO address to a user
 *                 buffer
 *
 *++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
int 
tsc_write_sgl( struct ifc1211_device *ifc,
	       struct tsc_ioctl_rdwr *rw,
	       int loop)
{
  int retval;
  void *kaddr; 
  int kbuf[2];
  ulong rem_base;
  struct tsc_ioctl_map_win win;
  int ds;

  retval = 0;
  win.pg_idx = ifc->rdwr_ctl->offset;
  win.req.mode.sg_id =  MAP_MAS_PCIE_MEM;
  win.req.mode.space =  rw->m.space;
  win.req.mode.am =  rw->m.am;
  win.req.mode.swap =  rw->m.swap;
  win.req.rem_addr = rw->rem_addr;
  retval = tsc_map_mas_modify( ifc, &win, &rem_base);
  if( retval)
  {
    retval = -EINVAL;
    goto tsc_write_sgl_exit;
  }
  kaddr = ifc->rdwr_ctl->rdwr_ptr + (rw->rem_addr - rem_base);
  ds = RDWR_MODE_GET_DS(rw->m.ads);
  if( copy_from_user( kbuf, (void *)rw->buf, ds))
  {
    retval = -EFAULT;
    goto tsc_write_sgl_exit;
  }
  switch( ds)
  {
    case RDWR_SIZE_BYTE:
    {
      if(loop)
      {
	loop = rw->len;
	while(loop--)
	{
	  *(char *)kaddr = *(char *)kbuf;
	}
      }
      else 
      {
	*(char *)kaddr = *(char *)kbuf;
      }
      break;
    }
    case RDWR_SIZE_SHORT:
    {
      if(loop)
      {
	loop = rw->len/2;
	while(loop--)
	{
	  *(short *)kaddr = *(short *)kbuf;
	}
      }
      else 
      {
	*(short *)kaddr = *(short *)kbuf;
      }
      break;
    }
     case RDWR_SIZE_INT:
    {
      if(loop)
      {
	loop = rw->len/4;
	while(loop--)
	{
	  *(int *)kaddr = *(int *)kbuf;
	}
      }
      else 
      {
	*(int *)kaddr = *(int *)kbuf;
      }
      break;
    }
     case RDWR_SIZE_DBL:
    {
      if(loop)
      {
	loop = rw->len/8;
	while(loop--)
	{
	  *(long long *)kaddr = *(long long *)kbuf;
	}
      }
      else 
      {
	*(long long *)kaddr = *(long long *)kbuf;
      }
      break;
    }
 }
tsc_write_sgl_exit:
  return( retval);
}

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * Function name : tsc_rem_write
 * Prototype     : int
 * Parameters    : pointer to IFC1211 device control structure
 *                 pointer to read/write control structure
 * Return        : error/success
 *----------------------------------------------------------------------------
 * Description   : read user buffer and copy data to remote IO address
 *
 *++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
int 
tsc_rem_write( struct ifc1211_device *ifc,
	       struct tsc_ioctl_rdwr *rw)
{
  if( rw->len & RDWR_LOOP)
  {
    /* loop data write */
    rw->len &= ~RDWR_LOOP;
    return( tsc_write_sgl( ifc, rw, 1));
  }
  else if( rw->len == 0)
  {
    /* single data write */
    return( tsc_write_sgl( ifc, rw, 0));
  }
  else 
  {
    /* block data write */
    return( tsc_write_blk( ifc, rw));
  }
}
