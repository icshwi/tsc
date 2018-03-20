/*=========================< begin file & file header >=======================
 *  References
 *  
 *    filename : maplib.c
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
#include "maplib.h"

#define DBGno
#include "debug.h"
#define task_tgid_nr(current) 0

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * Function name : map_init
 * Prototype     : struct map_ctl *
 * Parameters    : sg_id   -> map identifier
 *                 pg_num  -> number of pages in the map
 *                 pg_size -> size of each page
 *                 
 * Return        : pointer to map control structure
 *                 null pointer if failure
 *----------------------------------------------------------------------------
 * Description   : allocate and initialize map control structure
 *
 *++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

struct map_ctl *
map_init( int sg_id,
	  long base,
	  int pg_num,
	  int pg_size)
{
  struct map_ctl *mc;
  int map_size;

  mc = kzalloc( sizeof(struct map_ctl), GFP_KERNEL);
  if( mc)
  {
    mc->sg_id  = (char)sg_id;
    mc->pg_num = (short)pg_num;
    mc->pg_size = pg_size;
    mc->win_base = base;
    map_size = (pg_num+1)*sizeof( struct map_blk);
    mc->map_p = (struct map_blk *)kzalloc( map_size, GFP_KERNEL);
    if( !mc->map_p)
    {
      kfree( mc);
      return( NULL);
    }
    mc->map_p[0].npg = pg_num;               /* all pages     */
    mc->map_p[0].flag = MAP_FLAG_FREE; /* are free      */
    mc->map_p[pg_num].npg = 1;               /* last page     */
    mc->map_p[pg_num].flag = MAP_FLAG_BUSY;  /* shall be busy */
  }

  return(mc);
}

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * Function name : map_clear
 * Prototype     : void
 * Parameters    : map_ctl_p -> pointer to map control structure
 * Return        : none
 *----------------------------------------------------------------------------
 * Description   : clear map control structure
 *
 *++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

void
map_clear( struct map_ctl *map_ctl_p)
{
  memset( map_ctl_p->map_p, 0, (map_ctl_p->pg_num+1)*sizeof(struct map_blk));
  map_ctl_p->map_p[0].npg = map_ctl_p->pg_num;               /* all pages     */
  map_ctl_p->map_p[0].flag = MAP_FLAG_FREE;                  /* are free      */
  map_ctl_p->map_p[map_ctl_p->pg_num].npg = 1;               /* last page     */
  map_ctl_p->map_p[map_ctl_p->pg_num].flag = MAP_FLAG_BUSY;  /* shall be busy */
  return;
}

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * Function name : map_blk_force
 * Prototype     : struct map_ctl *
 * Parameters    : map_ctl_p -> pointer to map control structure
 *                 map_req_p -> pointer to map request structure
 * Return        : page offset if mapping successful
 *                 -1 if failure - bad address alignment
 *                               - bad requested size
 *                               - requested local address range busy
 *----------------------------------------------------------------------------
 * Description   : try to created a mapping between map_req_p->loc_addr and
 *                 map_req_p->rem_addr
 *                 map_ctl_p is supposed to be pointing on a valid map
 *                 initialized by map_init()
 *
 *++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

int
map_blk_force( struct map_ctl *map_ctl_p,
               struct map_req *map_req_p)
{
  struct map_blk *p;
  int i;
  u32 size;
  ushort npg;
  int off, end;
  int retval = -1;

  /* in forced mode, local and remote addresses smust be aligned to page size */
  /* need cast to ulong for 64bit divide/modulo on 32bit kernel [JFG]         */
  if( ((ulong)map_req_p->loc_addr % map_ctl_p->pg_size) ||
      ((ulong)map_req_p->rem_addr % map_ctl_p->pg_size)    )
  {
    return( MAP_ERR_BAD_ADDR);
  }
 
  p = map_ctl_p->map_p;
  size = map_req_p->size;
  if( !size)
  {
    return( MAP_ERR_BAD_SIZE);
  }
  npg = (ushort)(((size-1)/map_ctl_p->pg_size) + 1);
  off = (ulong)map_req_p->loc_addr / map_ctl_p->pg_size;
  end = off + npg;
  if( end > map_ctl_p->pg_num)
  {
    return( MAP_ERR_NO_SPACE);
  }
   /* Scan list block per block */
  retval = MAP_ERR_NO_SPACE;
  for( i = 0; i < map_ctl_p->pg_num; i += p[i].npg)
  {
    int low, high;

    low = i;
    high = i+p[i].npg;
    if( ((low <= off) && ( end <= high)) && (p[i].flag == MAP_FLAG_FREE))
    {
      /* free block able to hold requested window found */
      if( end < high)
      {
	/* create a free high block at (off+npg) */
	if( p[high].flag == MAP_FLAG_FREE)
	{
	  /* merge higher block with next one */
	  p[end].npg = high - end + p[high].npg;
	}
	else
	{
	  p[end].npg = high - end;
	}
	p[end].flag = MAP_FLAG_FREE;
	p[end].usr = 0;
      }
      p[off].npg = npg;
      p[off].flag = MAP_FLAG_BUSY | (map_req_p->flag & (MAP_FLAG_PRIVATE | MAP_FLAG_LOCKED));
      p[off].usr = 1;
      p[off].rem_addr = map_req_p->rem_addr;
      p[off].mode = map_req_p->mode;
      p[off].tgid = task_tgid_nr(current);
      p[off].tgid = 0;
      if( off > low)
      {
	/* create a free lower block */
	p[low].npg = off-low;
	p[low].flag = MAP_FLAG_FREE;
	p[low].usr = 0;
      }

      /* return local address in map parameters */
      map_req_p->win_size = map_ctl_p->pg_size * p[off].npg;
      map_req_p->loc_base = off * map_ctl_p->pg_size;
      map_req_p->rem_base = p[off].rem_addr;
      retval = off;
      break;
    }
  }
  map_req_p->offset = retval;
  return(retval);
}

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * Function name : map_blk_alloc
 * Prototype     : int
 * Parameters    : map_ctl_p -> pointer to map control structure
 *                 map_req_p -> pointer to map request structure
 * Return        : page offset if mapping successful
 *                 -1 if failure - not enough space
 *----------------------------------------------------------------------------
 * Description   : try to allocated local address and map it to the requested
 *                 remote address map_req_p->rem_addr
 *                 map_ctl_p is supposed to be pointing on a valid map
 *                 initialized by map_init()
 *
 *++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

int
map_blk_alloc( struct map_ctl *map_ctl_p,
               struct map_req *map_req_p)
{
  struct map_blk *p;
  int i;
  u32 size;
  ushort npg;
  int off;

  p = map_ctl_p->map_p;
  size = map_req_p->size + (u32)((ulong)map_req_p->rem_addr % map_ctl_p->pg_size);
  if( !size)
  {
    return( MAP_ERR_BAD_SIZE);
  }
  npg = (ushort)(((size-1)/map_ctl_p->pg_size) + 1);
  off = MAP_ERR_NO_SPACE;
  map_req_p->loc_addr = -1;

   /* scan list block per block */
  for( i = 0; i < map_ctl_p->pg_num; i += p[i].npg)
  {
    /* check only free blocks */
    if( (p[i].flag == MAP_FLAG_FREE))
    {
      /* check if this block is big enough */
      if( p[i].npg >= npg)
      {
	/* check if a block big enough was already found*/
	if( off >= 0)
        {
	  /* if this block is a better fit select it */
	  if( p[i].npg < p[off].npg)
          {
	    off = i;
	  }
        }
	/* if not select this block */
        else
        {
	  off = i;
	}
      }
    }
  }
  /* if block was found, update block list */
  if( off >= 0)
  {
      /* if block found is to big, create new free block with residue */
      if( p[off].npg > npg)
      {
         p[off + (long)npg].npg = p[off].npg - npg;
         p[off + (long)npg].flag = MAP_FLAG_FREE;
      }
      /* update block status */
      p[off].npg = npg;
      p[off].flag = MAP_FLAG_BUSY | (map_req_p->flag & (MAP_FLAG_PRIVATE | MAP_FLAG_LOCKED));
      p[off].usr = 1;
      p[off].rem_addr = map_req_p->rem_addr &  ~((u64)map_ctl_p->pg_size-1);
      p[off].mode = map_req_p->mode;
      p[off].tgid = task_tgid_nr(current);
      p[off].tgid = 0;

      /* return local address in map parameters */
      map_req_p->win_size = map_ctl_p->pg_size * p[off].npg;
      map_req_p->rem_base = p[off].rem_addr;
      map_req_p->loc_base = off * map_ctl_p->pg_size;
      map_req_p->loc_addr = map_req_p->rem_addr - p[off].rem_addr;
      map_req_p->loc_addr += map_req_p->loc_base;
  }
  map_req_p->offset = off;

  return( off);
}

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * Function name : map_blk_find
 * Prototype     : int
 * Parameters    : map_ctl_p -> pointer to map control structure
 *                 map_req_p -> pointer to map request structure
 * Return        : page offset if mapping successful
 *                 -1 if failure - not enough space
 *----------------------------------------------------------------------------
 * Description   : find map block
 *
 *++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

int
map_blk_find( struct map_ctl *map_ctl_p,
              struct map_req *map_req_p)
{
  struct map_blk *p;
  int i;
  int off;
  u64 h_addr;

  off = -1;
  map_req_p->loc_addr = -1;
  p = map_ctl_p->map_p;
  for( i = 0; i < map_ctl_p->pg_num; i += p[i].npg)
  {
    /* check only sharable busy blocks */
    if( (p[i].flag & MAP_FLAG_BUSY) && !(p[i].flag & MAP_FLAG_PRIVATE))
    {
      /* check if bus parameters are matching */
      h_addr = p[i].rem_addr + ((u64)map_ctl_p->pg_size * p[i].npg);
      if( (  map_req_p->rem_addr >= p[i].rem_addr) &&
          ( (map_req_p->rem_addr + map_req_p->size) <=  h_addr ) &&
          ( (map_req_p->mode & MAP_MODE_MASK) == (p[i].mode & MAP_MODE_MASK)))
      {
	off = i;
	/* if block is not locked, increment user's count */
	p[off].usr += 1;
	break;
      }
    }
  }
  if( off >= 0)
  {
    /* return local address in map parameters */
    map_req_p->win_size = map_ctl_p->pg_size * p[off].npg;
    map_req_p->rem_base = p[off].rem_addr;
    map_req_p->loc_base = off * map_ctl_p->pg_size;
    map_req_p->loc_addr = map_req_p->rem_addr - p[off].rem_addr;
    map_req_p->loc_addr += map_req_p->loc_base;
  }
  map_req_p->offset = off;
  return( off);
}

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * Function name : map_blk_modify
 * Prototype     : int
 * Parameters    : map_ctl_p -> pointer to map control structure
 *                 map_req_p -> pointer to map request structure
 *                 remote address
 * Return        : success/error
 *----------------------------------------------------------------------------
 * Description   : modify map block
 *
 *++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

int
map_blk_modify( struct map_ctl *map_ctl_p,
                struct map_req *map_req_p,
				uint64_t *rem_addr)
{
  struct map_blk *p;
  int off;

  p = map_ctl_p->map_p;
  off = map_req_p->offset;
  if( (off < 0) || (off >= map_ctl_p->pg_num))
  {
    return( -1);
  }
  if( !(p[off].flag & MAP_FLAG_BUSY) || (p[off].flag & MAP_FLAG_LOCKED))
  {
    return( -1);
  }
  p[off].rem_addr = map_req_p->rem_addr &  ~((u64)map_ctl_p->pg_size-1);
  p[off].mode = map_req_p->mode;
  map_req_p->win_size = map_ctl_p->pg_size * p[off].npg;
  map_req_p->rem_base = p[off].rem_addr;
  map_req_p->loc_base = off * map_ctl_p->pg_size;
  map_req_p->loc_addr = map_req_p->loc_base + map_req_p->rem_addr - map_req_p->rem_base;

  *rem_addr = p[off].rem_addr;
  return( 0);
}

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * Function name : map_blk_free
 * Prototype     : int
 * Parameters    : map_ctl_p -> pointer to map control structure
 *                 offset
 * Return        : success/error
 *----------------------------------------------------------------------------
 * Description   : free map block
 *
 *++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

int
map_blk_free( struct map_ctl *map_ctl_p,
              int off)
{
  struct map_blk *p;
  int i;
  int tmp_off;
  unsigned short new_size;
  int npg;

  if( (off < 0) || (off >= map_ctl_p->pg_num))
  {
    return( -1);
  }

  p = map_ctl_p->map_p;
  if( !(p[off].flag & MAP_FLAG_BUSY))
  {
    return( -1);
  }
   /* check if the block has been taken by more then one user */
   if( p[off].usr > 1)
   {
      p[off].usr -= 1;
      return( -1);
   }
  if( p[off].flag & MAP_FLAG_LOCKED)
  {
    return( -1);
  }

   /* free block */
   p[off].flag = MAP_FLAG_FREE;
   p[off].usr = 0;
   p[off].rem_addr = 0;
   p[off].mode = 0;
   p[off].tgid = 0;

   /* clear corresponding pages in mmu */
   npg = p[off].npg;
   /* check if next block in list is a free block */
   if( p[off + p[off].npg].flag == MAP_FLAG_FREE)
   {
      /* merge next block with current one */
      new_size = p[off].npg + p[off + p[off].npg].npg;
      p[off + p[off].npg].npg = 0;
      p[off].npg = new_size;
   }

   /* scan list to get offset of previous block */
   tmp_off = 0;
   for( i = 0; i < off; i += p[i].npg)
   {
      tmp_off = i;
   }
   if( i != off)
   {
     return( -1);
   }
   if( tmp_off < off)
   {
     /* check if previous block in list is a free block */
     if( p[tmp_off].flag == MAP_FLAG_FREE)
     {
       /* merge two bloks */
       p[tmp_off].npg += p[off].npg;
       p[off].npg = 0;
     }
   }
   return(npg);
}

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * Function name : map_release
 * Prototype     : int
 * Parameters    : map_ctl_p -> pointer to map control structure
 * Return        : success/error
 *----------------------------------------------------------------------------
 * Description   : release map
 *
 *++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

int
map_release( struct map_ctl *map_ctl_p)
{
  struct map_blk *p;
  int off;

  p = map_ctl_p->map_p;
  if( !p)
  {
    return( -1);
  }
   /* scan list block per block */
  while(1)
  {
    for( off = 0; off < map_ctl_p->pg_num; off += p[off].npg)
    {
      /* check only busy blocks */
      if( p[off].flag == MAP_FLAG_BUSY)
      {
        /* check block created by calling task */
        if( p[off].tgid == task_tgid_nr(current))
        {
 	  /* free block */
	  //printk("map %p : clearing block %x %d ->", p, off, p[off].npg);
	  map_blk_free( map_ctl_p, off);
	  //printk("%d\n",  p[off].npg);
	  break;
        }
      }
    }
    if( off >= map_ctl_p->pg_num)
    {
      break;
    }
  }
  return(0);
}

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * Function name : map_exit
 * Prototype     : void
 * Parameters    : map_ctl_p -> pointer to map control structure
 * Return        : void
 *----------------------------------------------------------------------------
 * Description   : exit mapping
 *
 *++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

void
map_exit( struct map_ctl *map_ctl_p)
{
  if( map_ctl_p)
  {
    kfree( map_ctl_p->map_p);
    kfree( map_ctl_p);
  }
}
