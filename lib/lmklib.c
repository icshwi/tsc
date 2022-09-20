/*=========================< begin file & file header >=======================
 *  References
 *
 *    filename : lmklib.c
 *    author   : CG
 *    company  : IOxOS
 *    creation : october 21,2020
 *
 *----------------------------------------------------------------------------
 *  Description
 *
 *    That library contains a set of function to control LMK clock generator
 *    family
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

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <tsculib.h>
#include <tscextlib.h>
#include <lmklib.h>

/*--------------------------------------------------------------------------------------------------------------------*/

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * Function name : lmk_get_priv
 * Prototype     : -
 * Parameters    : -
 * Return        : int    private data
 *----------------------------------------------------------------------------
 * Description   : Return the private data
 *++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

void lmk_configure(lmk_t *lmk, lmk_func_t func)
{
  if (lmk != NULL)
    lmk->func = func;
}
/*--------------------------------------------------------------------------------------------------------------------*/

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * Function name : lmk_read
 * Prototype     : -
 * Parameters    : -
 * Return        : 
 *----------------------------------------------------------------------------
 * Description   : 
 *++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

int lmk_read(lmk_t *lmk, int fd, int reg, int *data, int priv)
{
  if (lmk == NULL || lmk->func == NULL)        
    return (-1);

  return lmk->func(fd, LMK_REG_READ, reg, data, priv);
}

/*--------------------------------------------------------------------------------------------------------------------*/

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * Function name : lmk_write
 * Prototype     : -
 * Parameters    : -
 * Return        : 
 *----------------------------------------------------------------------------
 * Description   : 
 *++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

int lmk_write(lmk_t *lmk, int fd, int reg, int data, int priv)
{
  int tmp = data;
  
  if (lmk == NULL || lmk->func == NULL)
    return (-1);

  return lmk->func(fd, LMK_REG_WRITE, reg, &tmp, priv);
}

/*--------------------------------------------------------------------------------------------------------------------*/

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * Function name : lmk04806_init
 * Prototype     : -
 * Parameters    : -
 * Return        : 
 *----------------------------------------------------------------------------
 * Description   : initialize registers of LMK04806 
 *++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

int lmk04806_init(lmk_t *lmk, int fd, int lmk_reg[], int priv, int quiet)
{
  int ret, r;

  if (!quiet)
    printf("LMK04806 initialisation\n");

  /* LMK04806_R00 Generate a programmable RESET */
  ret = lmk_write(lmk, fd, 0x0, 0x00020000, priv);
  if (ret < 0) return(ret);

  usleep(50000);

  ret = lmk_write(lmk, fd, 0xB, lmk_reg[0xB], priv);
  if (ret < 0) return(ret);

  usleep(50000);

  /* R0 - R16 / R24 - R31 */
  for (r=0x00; r<=0x1F; r++)
  {
    if (((r >= 0 && r <= 16) || r >= 24) && r != 18)
    {
      ret = lmk_write(lmk, fd, r, lmk_reg[r], priv);
      if (ret < 0) return(ret);
    }
  }
  
  return (1);
}

/*--------------------------------------------------------------------------------------------------------------------*/

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * Function name : lmk04806_dump
 * Prototype     : -
 * Parameters    : -
 * Return        : 
 *----------------------------------------------------------------------------
 * Description   : 
 *++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

int lmk04806_dump(lmk_t *lmk, int fd, int priv)
{
  int ret, i, reg, data;

  printf("\nDump LMK04806 registers:");
  for (reg=0; reg<32; reg += 4)
  {
    printf("\n%02X : ", reg);
    for (i=0; i<4; i++)
    {
      ret = lmk_read(lmk, fd, (reg+i), &data, priv);
      if (ret < 0) return(ret);
      printf("%08X ", data);
    }
  }
  printf("\n");
  return(1);
}

/*--------------------------------------------------------------------------------------------------------------------*/
