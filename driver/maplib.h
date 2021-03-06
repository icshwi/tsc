/*=========================< begin file & file header >=======================
 *  References
 *  
 *    filename : maplib.h
 *    author   : JFG
 *    company  : IOxOS
 *    creation : Sept 30,2015
 *
 *----------------------------------------------------------------------------
 *  Description
 *
 *    This file contain the declarations of all exported functions define in
 *    maplib.c
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


#ifndef _H_MAPLIB
#define _H_MAPLIB

#define MAP_INVALID                0
#define MAP_MAS_PCIE_MEM           1
#define MAP_MAS_PCIE_PMEM          2
#define MAP_MAS_PCIE_CSR           3
#define MAP_SLV_PCIE1_MEM          9
#define MAP_SLV_PCIE1_PMEM       0xa

#define MAP_FLAG_FREE           0
#define MAP_FLAG_BUSY           1
#define MAP_FLAG_PRIVATE        2
#define MAP_FLAG_FORCE          4
#define MAP_FLAG_LOCKED         8
#define MAP_MODE_MASK           0xffff

#define MAP_ERR_BAD_SGID         -1    /* Bad mapper identoifier */
#define MAP_ERR_NO_SPACE         -2    /* not enough space left  */
#define MAP_ERR_BAD_ADDR         -3    /* Bad requested address  */
#define MAP_ERR_BAD_SIZE         -4    /* Bad requested size     */
#define MAP_ERR_BAD_IDX          -5    /* Invalid page index     */
 
struct map_ctl
{
  struct map_blk
  {
    char flag; char usr; short npg;
    uint mode;
    uint64_t rem_addr;
    pid_t tgid;           /* task id to which the block is allocated     */
  } *map_p;
  char rsv; char sg_id; short pg_num;
  int pg_size;
  uint64_t win_base;                   /* base address of mapping window */
  uint64_t win_size;                    /* size of mapping window */
  struct mutex map_lock;            /* mutex to lock MAP access                     */
};

struct map_req
{
  uint size;                            /* mapping size required by user            */
  char flag; char sg_id; ushort mode;   /* mapping mode                             */
  uint64_t rem_addr;                       /* remote address to be mapped              */
  uint64_t loc_addr;                       /* local address returned by mapper         */
  pid_t tgid;                           /* task id to which the block is allocated     */
  uint offset;                          /* offset of page containing local address  */
  uint win_size;                        /* size actually mapped                     */
  uint64_t rem_base;                       /* remote address of window actually mapped */
  uint64_t loc_base;                       /* local address of window actually mapped  */
  void *usr_addr;                       /* user address pointing to local address   */
  uint64_t bus_base;                       /* bus base address of SG window            */
};

struct map_ctl *map_init( int sg_id, long base, int pg_num, int pg_size);
int map_blk_force( struct map_ctl *, struct map_req *);
int map_blk_alloc( struct map_ctl *, struct map_req *);
int map_blk_find( struct map_ctl *, struct map_req *);
int map_blk_modify( struct map_ctl *, struct map_req *, uint64_t *);
int map_blk_free( struct map_ctl *, int);
int map_release( struct map_ctl *);
void map_clear( struct map_ctl *);
void map_exit( struct map_ctl *);

#endif /*  _H_MAPLIB */

