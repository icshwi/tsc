/*=========================< begin file & file header >=======================
 *  References
 *
 *    filename : xilinxlib.h
 *    author   : CG
 *    company  : IOxOS
 *    creation : februar 25,2021
 *
 *----------------------------------------------------------------------------
 *  Description
 *
 *    That library contains a set of function to access the Xilinx resources
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

#ifndef _H_XILINXLIB
#define _H_XILINXLIB

/* ------------------------------------------------------------------------------------------------------------------ */
/*  Definition                                                                                                        */
/* ------------------------------------------------------------------------------------------------------------------ */

/* arguments: cmd, sel, register, data, priv */
typedef int (*xilinx_func_t)(int, int, int, int, int *, int);

typedef struct
{
  xilinx_func_t   func;
}
xilinx_t;

/* ------------------------------------------------------------------------------------------------------------------ */

#define XILINX_DRP_READ           0   /* Read a DRP register        */
#define XILINX_DRP_WRITE          1   /* Write a DRP register       */

/* ------------------------------------------------------------------------------------------------------------------ */
/*  Function Prototypes                                                                                               */
/* ------------------------------------------------------------------------------------------------------------------ */

extern int  xilinx_set_verbose      (int mode);
extern void xilinx_configure        (xilinx_t *xil, xilinx_func_t func);
extern int  xilinx_drp_write        (xilinx_t *xil, int fd, int sel, int reg, int data, int priv);
extern int  xilinx_drp_read         (xilinx_t *xil, int fd,  int sel, int reg, int *data, int priv);
extern int  xilinx_qpll_calc_param  (int type, double freq_refclk, double linerate, int *fbdiv, int *refclk_div, int *xout_div);
extern int  xilinx_qpll_configure   (xilinx_t *xil, int fd,  int sel, int type, double freq_refclk, double linerate, int priv);

#endif /*  _H_XILINXLIB */

