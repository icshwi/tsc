/*=========================< begin file & file header >=======================
 *  References
 *  
 *    filename : tscextlib.h
 *    author   : JFG, XP
 *    company  : IOxOS
 *    creation : june 30,2008
 *
 *----------------------------------------------------------------------------
 *  Description
 *
 *    This file contains the declarations of all exported functions define in
 *    tscextlib.c
 *
 *----------------------------------------------------------------------------
 *
 *  Copyright (C) IOxOS Technologies SA <ioxos@ioxos.ch>
 *  Copyright (C) 2019  European Spallation Source ERIC
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

#ifndef _H_TSCEXTLIB
#define _H_TSCEXTLIB

#include <tscioctl.h>

struct tscext_ioctl_map_pg
{
  uint size;                            /* mapping size required by user            */
  char flag; char sg_id; ushort mode;   /* mapping mode                             */
  uint64_t rem_addr;                       /* remote address to be mapped              */
  uint64_t loc_addr;                       /* local address returned by mapper         */
  uint offset;                          /* offset of page containing local address  */
  uint win_size;                        /* size actually mapped                     */
  uint64_t rem_base;                       /* remote address of window actually mapped */
  uint64_t loc_base;                       /* local address of window actually mapped  */
  void *usr_addr;                       /* user address pointing to local address   */
  uint64_t pci_base;                       /* pci base address of SG window            */
};

float tscext_bmr_conv_11bit_u( unsigned short);
float tscext_bmr_conv_11bit_s( unsigned short);
float tscext_bmr_conv_16bit_u( unsigned short);
int tscext_pex_write( uint, uint);
int tscext_bmr_read(int, uint, uint, uint *, uint);
int tscext_bmr_write(int, uint, uint, uint, uint);
int tscext_csr_rd(int, int);
void tscext_csr_wr(int, int, int);
int tscext_map_alloc( struct tscext_ioctl_map_pg *);
int tscext_map_free( struct tscext_ioctl_map_pg *);
void *tscext_mmap( struct tscext_ioctl_map_pg *);
int tscext_munmap( struct tscext_ioctl_map_pg *);

#endif /*  _H_TSCEXTLIB */
