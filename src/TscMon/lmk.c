/*=========================< begin file & file header >=======================
 *  References
 *  
 *    filename : lmk.c
 *    author   : JFG, XP
 *    company  : IOxOS
 *    creation : Sept 15,2015
 *
 *----------------------------------------------------------------------------
 *  Description
 *
 *	  This file contains a set of functions to drive lmk.
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

#include <debug.h>
#include <sys/types.h>
#include <sys/time.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <cli.h>
#include <tscioctl.h>
#include <tsculib.h>
#include <aio.h>
#include <errno.h>
#include <ctype.h>
#include <time.h>
#include "TscMon.h"

#define LMK_OK   0
#define LMK_ERR -1
#define IFC_CSR_LMKCTL        0x48
#define IFC_CSR_LMKDAT        0x4c
#define IFC_LMKCTL_READ  0x80000000
#define IFC_LMKCTL_WRITE 0xc0000000
#define IFC_LMKCTL_LMK0  0x00000000
#define IFC_LMKCTL_LMK1  0x00010000
#define IFC_LMKCTL_ADD   0x0000001f

extern int tsc_fd;

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * Function name : tsc_lmk
 * Prototype     : int
 * Parameters    : pointer to command parameter list
 * Return        : ok  if command executed
 *                 nok if error
 *----------------------------------------------------------------------------
 * Description   : manage lmk command execution
 *
 *++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

int 
tsc_lmk( struct cli_cmd_para *c)
{
  int idx, reg, data;
  int tmp;


  idx = 0;
  if( c->ext)
  {
    if( c->ext[0] == '1') idx = 1;
  }
  if( c->cnt < 2)
  {
    printf("Not enough arguments -> usage:\n");
    tsc_print_usage( c);
    return( LMK_ERR);
  }
  if( sscanf( c->para[1],"%x", &reg) != 1)
  {
    printf("Bad register argument [%s] -> usage:\n", c->para[1]);
    tsc_print_usage( c);
    return( LMK_ERR);
  }
  reg &= IFC_LMKCTL_ADD;
  reg |= idx << 16;
  if(!strcmp( "read", c->para[0]))
  {
    int tmo;

    tsc_pon_read(tsc_fd, IFC_CSR_LMKCTL, &tmp);
    reg |= tmp & ~(IFC_LMKCTL_WRITE | IFC_LMKCTL_LMK1 | IFC_LMKCTL_ADD);
    reg |= IFC_LMKCTL_READ;
    tsc_pon_write(tsc_fd, IFC_CSR_LMKCTL, &reg);
    tmo = 100;
    while( tmo--)
    {
      tsc_pon_read(tsc_fd, IFC_CSR_LMKCTL, &tmp);
      if( !(tmp & IFC_LMKCTL_READ))
      {
	break;
      }
    }
    tsc_pon_read(tsc_fd, IFC_CSR_LMKDAT, &data);
    printf("LMK%d reg:%02x = %08x\n", idx, reg&IFC_LMKCTL_ADD, data);
    return( LMK_OK);
  }
  if(!strcmp( "write", c->para[0]))
  {
    if( c->cnt < 3)
    {
      printf("Not enough arguments -> usage:\n");
      tsc_print_usage( c);
      return( LMK_ERR);
    }
    if( sscanf( c->para[2],"%x", &data) != 1)
    {
      printf("Bad data argument [%s] -> usage:\n", c->para[2]);
      tsc_print_usage( c);
      return( LMK_ERR);
    }
    tsc_pon_write(tsc_fd, IFC_CSR_LMKDAT, &data);
    tsc_pon_read(tsc_fd, IFC_CSR_LMKCTL, &tmp);
    reg |= tmp & ~(IFC_LMKCTL_WRITE | IFC_LMKCTL_LMK1 | IFC_LMKCTL_ADD);
    reg |= IFC_LMKCTL_WRITE;
    tsc_pon_write(tsc_fd, IFC_CSR_LMKCTL, &reg);
    return( LMK_OK);
  }
  return( LMK_ERR);
}
