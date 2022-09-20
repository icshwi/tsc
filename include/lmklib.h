/*=========================< begin file & file header >=======================
 *  References
 *
 *    filename : lmklib.h
 *    author   : JFG, XP
 *    company  : IOxOS
 *    creation : october 18,2018
 *
 *----------------------------------------------------------------------------
 *  Description
 *
 *
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

#ifndef _H_LMKLIB
#define _H_LMKLIB

/* ------------------------------------------------------------------------------------------------------------------ */
/*  Definition                                                                                                        */
/* ------------------------------------------------------------------------------------------------------------------ */

#define LMK_REG_READ     0
#define LMK_REG_WRITE    1

/* arguments: read/write, register, data, priv */
typedef int (*lmk_func_t)(int, int, int, int*, int);

typedef struct
{
  lmk_func_t   func;
}
lmk_t;

/* ------------------------------------------------------------------------------------------------------------------ */
/*  Function Prototypes                                                                                               */
/* ------------------------------------------------------------------------------------------------------------------ */

extern void lmk_configure       (lmk_t *lmk, lmk_func_t func);
extern int  lmk_read            (lmk_t *lmk, int fd, int reg, int *data, int priv);
extern int  lmk_write           (lmk_t *lmk, int fd, int reg, int data,  int priv);
extern int  lmk04806_init       (lmk_t *lmk, int fd, int lmk_reg[], int priv, int quiet);
extern int  lmk04806_dump       (lmk_t *lmk, int fd, int priv);

#endif /* _H_LMKLIB */
