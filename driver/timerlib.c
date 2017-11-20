/*=========================< begin file & file header >=======================
 *  References
 *  
 *    filename : timerlib.c
 *    author   : JFG
 *    company  : IOxOS
 *    creation : Nov 30,2015
 *    version  : 0.0.1
 *
 *----------------------------------------------------------------------------
 *  Description
 *
 *    This file contains the low level functions to drive the address mappers
 *    implemented on the tsc.
 *
 *----------------------------------------------------------------------------
 *
 * Copyright (c) 2015 IOxOS Technologies SA <ioxos@ioxos.ch>
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 * GPL license :
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 *
 *----------------------------------------------------------------------------
 *  Change History
 *  
 *
 *  
 *=============================< end file header >============================*/
#include "tscos.h"
#include "tscdrvr.h"

#define DBGno
#include "debug.h"

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * Function name : tsc_timer_irq
 * Prototype     : int
 * Parameters    : pointer to tsc device control structure
 *                 interrupt source identifier
 * Return        : 1 if source attached to handler
 *----------------------------------------------------------------------------
 * Description   : 
 *
 *++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
int 
tsc_timer_irq( struct tsc_device *ifc,
	       int src,
	       void *arg)
{
  return( 0);
}
EXPORT_SYMBOL( tsc_timer_irq);

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * Function name : timer_init
 * Prototype     : int
 * Parameters    : pointer to tsc device control structure
 * Return        : 1 if source attached to handler
 *----------------------------------------------------------------------------
 * Description   : 
 *
 *++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
int 
timer_init( struct tsc_device *ifc)
{
  /* register timer IRQ */
  /* start timer        */
  return( 0);
}

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * Function name : tsc_timer_start
 * Prototype     : int
 * Parameters    : pointer to tsc device control structure
 *                 pointer to timer control structure
 * Return        : 1 if source attached to handler
 *----------------------------------------------------------------------------
 * Description   : 
 *
 *++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
int 
tsc_timer_start( struct tsc_device *ifc,
		 struct tsc_ioctl_timer *tmr)
{
  uint tmr_csr;
  /* stop timer */
  iowrite32( 0, ifc->csr_ptr + TSC_CSR_GLTIM_CSR);
  /* set time */
  iowrite32( tmr->time.msec, ifc->csr_ptr + TSC_CSR_GLTIM_CNT1);
  iowrite32( tmr->time.usec, ifc->csr_ptr + TSC_CSR_GLTIM_CNT2);
  /* restart timer according to new mode */
  tmr_csr = TSC_GLTIM_ENA | TIMER_SYNC_ENA | tmr->mode;
  iowrite32( tmr_csr, ifc->csr_ptr + TSC_CSR_GLTIM_CSR);
  return( 0);
}
EXPORT_SYMBOL( tsc_timer_start);

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * Function name : tsc_timer_restart
 * Prototype     : int
 * Parameters    : pointer to TSC device control structure
 *                 pointer tin time data structure
 * Return        : 1 if source attached to handler
 *----------------------------------------------------------------------------
 * Description   : if pointer to time data structure not NULL, set time
 *                 if timer is stopped, restart it
 *
 *++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
int 
tsc_timer_restart( struct tsc_device *ifc,
		   struct tsc_time *tm)
{
  uint tmr_csr;

  tmr_csr =  ioread32( ifc->csr_ptr + TSC_CSR_GLTIM_CSR);
  if( ~(tmr_csr & TSC_GLTIM_ENA))
  {
    if( tm)
    {
      iowrite32( tm->msec, ifc->csr_ptr + TSC_CSR_GLTIM_CNT1);
      iowrite32( tm->usec, ifc->csr_ptr + TSC_CSR_GLTIM_CNT2);
    }
    iowrite32( (TSC_GLTIM_ENA | tmr_csr), ifc->csr_ptr + TSC_CSR_GLTIM_CSR);
    return( 0);
  }
  return( -1);
}
EXPORT_SYMBOL( tsc_timer_restart);

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * Function name : tsc_timer_stop
 * Prototype     : int
 * Parameters    : pointer to TSC device control structure
 * Return        : 1 if source attached to handler
 *----------------------------------------------------------------------------
 * Description   : 
 *
 *++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
int 
tsc_timer_stop( struct tsc_device *ifc)
{
  uint tmr_csr;
  tmr_csr =  ioread32( ifc->csr_ptr + TSC_CSR_GLTIM_CSR);
  tmr_csr &=  ~TSC_GLTIM_ENA;
  iowrite32( tmr_csr, ifc->csr_ptr + TSC_CSR_GLTIM_CSR);
  return( 0);
}
EXPORT_SYMBOL( tsc_timer_stop);


/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * Function name : tsc_timer_read
 * Prototype     : int
 * Parameters    : pointer to TSC device control structure
 *                 pointr st structure to hold time information
 * Return        : 1 if source attached to handler
 *----------------------------------------------------------------------------
 * Description   : 
 *
 *++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
int 
tsc_timer_read( struct tsc_device *ifc,
		struct tsc_time *tm)
{
  tm->msec = ioread32( ifc->csr_ptr + TSC_CSR_GLTIM_CNT1);
  tm->usec = ioread32( ifc->csr_ptr + TSC_CSR_GLTIM_CNT2);
  return( 0);
}
EXPORT_SYMBOL( tsc_timer_read);


