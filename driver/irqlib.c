/*=========================< begin file & file header >=======================
 *  References
 *  
 *    filename : irqlib.c
 *    author   : JFG, XP
 *    company  : IOxOS
 *    creation : Sept 27,2015
 *
 *----------------------------------------------------------------------------
 *  Description
 *
 *    This file contains the low level functions to drive the IRQ.
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
#include "tscdrvr.h"

#define DBGno
#include "debug.h"

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * Function name : tsc_irq_register
 * Prototype     : int
 * Parameters    : pointer to tsc device control structure
 *                 interrupt source identifier
 *                 interrupt handler
 *                 argument to by passed to interrupt handler
 * Return        : error/success
 *----------------------------------------------------------------------------
 * Description   : register interrupt handler <func> associated to interrupt
 *                 source identifier <itc><ip>. Pointer <arg> is passed as argument
 *                 when <func> is executed on occurence of interrupt <itc><ip>
 *
 *++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

int 
tsc_irq_register( struct tsc_device *ifc,
		  int itc, int ip,
		  void (* func)( struct tsc_device*, int, void *),
		  void *arg)
{
  if( ifc->irq_tbl[itc][ip].busy)
  {
    return( -EBUSY);
  }
  ifc->irq_tbl[itc][ip].func = func;
  ifc->irq_tbl[itc][ip].arg = arg;
  ifc->irq_tbl[itc][ip].cnt = 0;
  ifc->irq_tbl[itc][ip].busy = 1;

  return( 0);
}
EXPORT_SYMBOL( tsc_irq_register);

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * Function name : tsc_irq_spurious
 * Prototype     : void
 * Parameters    : pointer to tsc device control structure
 *                 interrupt source identifier
 *                 argument associated ti interrupt handler
 * Return        : void
 *----------------------------------------------------------------------------
 * Description   : tsc default interrupt handle, does nothing...
 *
 *++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

void
tsc_irq_spurious( struct tsc_device *p,
		         int src,
		         void *arg)
{
  debugk(("TSC spurious interrupt : %x - %p\n", src, arg));
  return;
}

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * Function name : tsc_irq_check_busy
 * Prototype     : int
 * Parameters    : pointer to TSC device control structure
 *                 interrupt source identifier
 * Return        : 1 if source attached to handler
 *----------------------------------------------------------------------------
 * Description   : check if interrupt source is attached to an handler
 *
 *++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

int 
tsc_irq_check_busy( struct tsc_device *ifc,
		    int itc, int ip)
{
  return( ifc->irq_tbl[itc][ip].busy);
}
EXPORT_SYMBOL( tsc_irq_check_busy);

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * Function name : tsc_irq_unregister
 * Prototype     : int
 * Parameters    : pointer to tsc device control structure
 *                 interrupt source identifier
 * Return        : void
 *----------------------------------------------------------------------------
 * Description   : unregister interrupt handler <func> associated to interrupt
 *                 source identifier <itc><ip>.
 *
 *++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

void 
tsc_irq_unregister( struct tsc_device *ifc,
		    int itc, int ip)
{
  ifc->irq_tbl[itc][ip].func = tsc_irq_spurious;
  ifc->irq_tbl[itc][ip].arg = NULL;
  ifc->irq_tbl[itc][ip].cnt = 0;
  ifc->irq_tbl[itc][ip].busy = 0;
}
EXPORT_SYMBOL( tsc_irq_unregister);

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * Function name : tsc_irq_mask
 * Prototype     : int
 * Parameters    : pointer to tsc device control structure
 *                 operation to be performed (set/clear)
 *                 source identifier
 * Return        : error/success
 *----------------------------------------------------------------------------
 * Description   : initialize tsc address mapping data structures for pci
 *                 master access.
 *
 *++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

int 
tsc_irq_mask( struct tsc_device *ifc,
	      int op,
	      int src)
{
  int retval;
  int ctl;
  int ip;

  debugk(( KERN_NOTICE "tsc : Entering tsc_irq_mask( %p, %x, %x)\n", ifc, op, src));

  retval = -1;
  ctl = ITC_CTL(src);
  ip = ITC_IP(src);

  switch( ctl)
  {
    case ITC_CTL_ILOC:
    {
      if( op == TSC_IOCTL_ITC_MSK_CLEAR)
      {
	iowrite32( ip, ifc->csr_ptr + TSC_CSR_ILOC_ITC_IMC);
	retval = 0;
      }
      if( op == TSC_IOCTL_ITC_MSK_SET)
      {
        iowrite32( ip, ifc->csr_ptr + TSC_CSR_ILOC_ITC_IMS);
	retval = 0;
      }
      break;
    }
    case ITC_CTL_DMA:
    {
      if( op == TSC_IOCTL_ITC_MSK_CLEAR)
      {
	iowrite32( ip, ifc->csr_ptr + TSC_CSR_IDMA_ITC_IMC);
	retval = 0;
      }
      if( op == TSC_IOCTL_ITC_MSK_SET)
      {
        iowrite32( ip, ifc->csr_ptr + TSC_CSR_IDMA_ITC_IMS);
	retval = 0;
      }
      break;
    }
  }

  return( retval);
}
EXPORT_SYMBOL( tsc_irq_mask);

