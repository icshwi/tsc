/*=========================< begin file & file header >=======================
 *  References
 *
 *    filename : xilinxlib.c
 *    author   : CG
 *    company  : IOxOS
 *    creation : februar 25,2021
 *
 *----------------------------------------------------------------------------
 *  Description
 *
 *    That library contains a set of function to access Xilinx DRP ressources.
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
#include <math.h>
#include <tsculib.h>
#include <tscextlib.h>
#include <xilinxlib.h>

/* ------------------------------------------------------------------------------------------------------------------ */
/* local macros used to display messages
 */

#define xilinx_info(...)          ({ if (xilinx_verbose_mode) fprintf(stdout, __VA_ARGS__); })
#define xilinx_error(...)         ({ if (xilinx_verbose_mode) fprintf(stderr, __VA_ARGS__); })

/* ------------------------------------------------------------------------------------------------------------------ */
/* Module Private's Variables
 */

static int xilinx_verbose_mode = 0;       /* verbose mode                       */

/* ------------------------------------------------------------------------------------------------------------------ */

/*!+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * \brief   Set verbose mode
 *
 * \param   mode        enable (=1) or disable (0) verbose mode
 *
 * \return  current verbose mode
 *++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

int xilinx_set_verbose(int mode)
{
  xilinx_verbose_mode = mode;
  return(xilinx_verbose_mode);
}
/* ------------------------------------------------------------------------------------------------------------------ */

/*!+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * \brief   Configure function to access Xilinx resources
 *
 * \param   xil             Pointer to a xilinx descriptor
 * \param   func            access function (DRP READ & DRP WRITE commands)
 *++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

void xilinx_configure(xilinx_t *xil, xilinx_func_t func)
{
  if (xil != NULL)
  {
    xil->func = func;
  }
}

/* ------------------------------------------------------------------------------------------------------------------ */

/*!+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * \brief   Read a data value from register of DAC38j84 on SPI bus
 *
 * \param   xil             Pointer to a xilinx descriptor
 * \param   sel             select DRP interface
 * \param   reg             register address
 * \param   data            integer pointer to store data read
 * \param   priv            private data
 *
 * \return  >= on success, <0 on error
 *++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

int xilinx_drp_read(xilinx_t *xil, int fd, int sel, int reg, int *data, int priv)
{
  if (xil == NULL || xil->func == NULL)
  {
    return(-1);
  }
  return xil->func(XILINX_DRP_READ, fd, sel, reg, data, priv);
}

/* ------------------------------------------------------------------------------------------------------------------ */

/*!+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * \brief   Write a data value into register of DAC38j84 on SPI bus
 *
 * \param   xil             Pointer to a xilinx descriptor
 * \param   sel             select DRP interface
 * \param   reg             register address
 * \param   data            data value to write
 * \param   priv            private data
 *
 * \return  >= on success, <0 on error
 *++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

int xilinx_drp_write(xilinx_t *xil, int fd, int sel, int reg, int data, int priv)
{
  if (xil == NULL || xil->func == NULL)
  {
    return(-1);
  }
  return xil->func(XILINX_DRP_WRITE, fd, sel, reg, &data, priv);
}

/* ------------------------------------------------------------------------------------------------------------------ */

/*!+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * \brief   Calculate QPLL dividers from refclk frequency and linerate 
 *
 * \param   type            0 = QPLL0, 1 = QPLL1
 * \param   freq_refclk     REFCLK pin frequency in MHz
 * \param   linerate        linerate in GHz
 * \param   fbdiv           pointer to FBDIV integer value
 * \param   refclk_div      pointer to REFCLK_DIV integer value
 * \param   xout_div        pointer to RXOUT_DIV/TXOUT_DIV integer value
 *
 * \return  >= on success, <0 on error
 *++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

int xilinx_qpll_calc_param(int type, double freq_refclk, double linerate, int *fbdiv, int *refclk_div, int *xout_div)
{
  int n_div, m_div, d, d_div;

  if (fbdiv == NULL || refclk_div == NULL || xout_div == NULL)
  {
    return(-1);
  }

  /* invalid reference clock */
  if (freq_refclk < 60.0 || freq_refclk > 820.0)
  {
    return(-1);
  }
  
  /* invalud linerate for QPLL0 (9.8 to 12.5) or QPLL1 (8.0 to 12.5) */
  if ((type == 0 && (linerate < 9.8 || linerate > 12.5)) ||
      (type == 1 && (linerate < 8.0 || linerate > 12.5)))
  {
    return(-1);
  }

  m_div = 1;
  d_div = 1;
/*  n_div = (int)round(linerate / freq_refclk);*/
  n_div = (int)round((1000.0 * linerate) / freq_refclk);

  if (*xout_div != -1)
  {
    d_div = (*xout_div);
    n_div /= d_div;
  }
  
  /* calculate m divider if needed */
  if (n_div > 160)
  {
    for (m_div=2; m_div<=4; m_div++)
    {
      if ((n_div / m_div) < 160)
        break;
    }
    n_div = (n_div / m_div);
  }
  
  /* calculate d divider if needed */
  if (*xout_div == -1)
  {
    if (n_div > 160)
    {
      for (d=1; d<=4; d++)
      {
        d_div *= 2;
        if ((n_div / d_div) < 160)
          break;
      }
      n_div = (n_div / d_div);
    }
  }
  
  if (n_div > 160)
  {
    return(-1);
  }

  xilinx_info("xilinx: QPLL%d linerate = %g Gbit/s, freq_refclk = %g MHz, fbdiv = %d, refclk_div = %d, txout_div/rxout_div = %d\n", 
    (type&1), linerate, freq_refclk, n_div, m_div, d_div);

  (*fbdiv)      = n_div;
  (*refclk_div) = m_div;
  (*xout_div)   = d_div;

  return(0);
}

/* ------------------------------------------------------------------------------------------------------------------ */

/*!+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * \brief   Reconfigure a QPLL
 *
 * \param   xil             Pointer to a xilinx descriptor
 * \param   sel             select DRP interface
 * \param   type            0 = QPLL0, 1 = QPLL1
 * \param   freq_refclk     REFCLK pin frequency in MHz
 * \param   linerate        linerate in GHz
 * \param   priv            private data 
 * 
 * \return  >= on success, <0 on error
 * 
 * \note    QPLL must be in reset before calling this function 
 *++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

int xilinx_qpll_configure(xilinx_t *xil, int fd, int sel, int type, double freq_refclk, double linerate, int priv)
{
  int rc, fbdiv, refclk_div, refclk_div_enc, xout_div, tmp, base;

  /* fixed parameters in IP */
  xout_div = 1;

  rc = xilinx_qpll_calc_param(type, freq_refclk, linerate, &fbdiv, &refclk_div, &xout_div);
  if (rc < 0)
  {
    return(rc);
  }

  base = ((type == 0) ? 0x00 : 0x80);

  /* 0x14/0x94 [15:8] [7:0] QPLLx_FBDIV */
  rc = xilinx_drp_read(xil, fd, sel, (base+0x14), &tmp, priv);
  if (rc < 0)
  {
    return(rc);
  }

  tmp &= 0xFF00;
  tmp |= (fbdiv-2);
  
  rc = xilinx_drp_write(xil, fd, sel, base+0x14, tmp, priv);
  if (rc < 0)
  {
    return(rc);
  }
  
  /* 0x18/0x98 [11:7] QPLLx_REFCLK_DIV[4:0] */
  rc = xilinx_drp_read(xil, fd, sel, base+0x18, &tmp, priv);
  if (rc < 0)
  {
    return(rc);
  }

  refclk_div_enc = ((refclk_div == 1) ? 16 : (refclk_div - 2));

  tmp &= 0xF07F;
  tmp |= (refclk_div_enc << 7);
  
  rc = xilinx_drp_write(xil, fd, sel, base+0x18, tmp, priv);
  if (rc < 0)
  {
    return(rc);
  }

  return (0);
}

/* ------------------------------------------------------------------------------------------------------------------ */
