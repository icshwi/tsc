/*=========================< begin file & file header >=======================
 *  References
 *  
 *    filename : tscmasioctl.h
 *    author   : JFG, XP
 *    company  : IOxOS
 *    creation : Sept 30,2015
 *
 *----------------------------------------------------------------------------
 *  Description
 *
 *    This file contains the declarations for the ioctl commands supported
 *    by the TSC master driver.
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

#ifndef _H_TSCMASIOCTL
#define _H_TSCMASIOCTL

#define TSC_IOCTL_MAS            0x00030000
#define TSC_IOCTL_MAS_MAP_SET    (TSC_IOCTL_MAS | 0x1)
#define TSC_IOCTL_MAS_MAP_GET    (TSC_IOCTL_MAS | 0x2)
#define TSC_IOCTL_MAS_IRQ_SET    (TSC_IOCTL_MAS | 0x3)
#define TSC_IOCTL_MAS_IRQ_GET    (TSC_IOCTL_MAS | 0x4)

struct tsc_ioctl_mas_map
{
  uint64_t rem_addr;
  uint64_t loc_addr;
  uint size;
  uint mode;
  uint pg_idx;
  uint ivec;
};

#define MAS_MAP_MODE_SWAP_AUTO      0x100
#define MAS_MAP_MODE_SWAP_DW        0x200
#define MAS_MAP_MODE_SWAP_QW        0x300
#define MAS_MAP_MODE_SWAP_MASK      0x300

#define MAS_MAP_MODE_SHM             0x20000
#define MAS_MAP_MODE_USR             0x30000
#define MAS_MAP_MODE_SPACE           0xf0000

#define MAS_MAP_NO_IVEC             -1
#define MAS_MAP_IVEC_MIN             0
#define MAS_MAP_IVEC_MAX          0xff

#define MAS_MAP_IDX_INV             -1

#endif /*  _H_TSCMASIOCTL */

