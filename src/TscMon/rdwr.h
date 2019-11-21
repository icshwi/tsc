/*=========================< begin file & file header >=======================
 *  References
 *  
 *    filename : rdwr.h
 *    author   : JFG, XP
 *    company  : IOxOS
 *    creation : Sept 30,2015
 *
 *----------------------------------------------------------------------------
 *  Description
 *
 *    This file contains the declarations of all exported functions defined in
 *    rdwr.c
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

#ifndef _H_RDWR
#define _H_RDWR

struct rdwr_cycle_para
{
  uint64_t addr;
  uint64_t data;
  int len;
  union
  {
    uint mode;
    struct rdwr_mode m;
  };
  uint64_t para;
  uint operation;
  int loop;
  struct tsc_ioctl_kbuf_req *kb_p;
};

#define RDWR_OK     0
#define RDWR_ERR    -1

int  rdwr_init( void);
int  rdwr_exit( void);
int tsc_rdwr_pr( struct cli_cmd_para *);
int tsc_rdwr_dr( struct cli_cmd_para *);
int tsc_rdwr_dx( struct cli_cmd_para *);
int tsc_rdwr_fx( struct cli_cmd_para *);
int tsc_rdwr_tx( struct cli_cmd_para *);
int tsc_rdwr_px( struct cli_cmd_para *);
int tsc_rdwr_cr( struct cli_cmd_para *);
int tsc_rdwr_cx( struct cli_cmd_para *);
int tsc_rdwr_cmp( struct cli_cmd_para *c);
int tsc_rdwr_lx( struct cli_cmd_para *);

#endif /* _H_RDWR */
