/*=========================< begin file & file header >=======================
 *  References
 *  
 *    filename : i2clib.c
 *    author   : JFG, XP
 *    company  : IOxOS
 *    creation : Sept 27,2015
 *
 *----------------------------------------------------------------------------
 *  Description
 *
 *    This file contains the low level functions to control I2Cs implemented
 *    in the tsc project.
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
 * Function name : tsc_i2c_irq
 * Prototype     : void
 * Parameters    : pointer to tsc device control structure, source, argument
 * Return        : void
 *----------------------------------------------------------------------------
 * Description   : interupt handler for i2c
 *
 *++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

void
tsc_i2c_irq( struct tsc_device *ifc,
	     int src,
	     void *arg)
{
  ifc->i2c_ctl->status = I2C_STATUS_DONE;
  wake_up_interruptible(&ifc->i2c_ctl->i2c_irq_wait);
  return;
}

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * Function name : tsc_i2c_reset
 * Prototype     : int
 * Parameters    : pointer to tsc device control structure
 * Return        : 0
 *----------------------------------------------------------------------------
 * Description   : reset i2c controller
 *
 *++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

int 
tsc_i2c_reset( struct tsc_device *ifc)
{
  return(0);
}

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * Function name : tsc_i2c_wait
 * Prototype     : int
 * Parameters    : pointer to tsc device control structure, interrupt
 * Return        : mutex status
 *----------------------------------------------------------------------------
 * Description   : wait i2c controller interrupt
 *
 *++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

int 
tsc_i2c_wait( struct tsc_device *ifc,
	      int irq)
{
  int jiffies, retval;
 
  jiffies = msecs_to_jiffies(10); /* timeout after 10 msec */
  ifc->i2c_ctl->status = I2C_STATUS_RESET;
  retval = wait_event_interruptible_timeout(ifc->i2c_ctl->i2c_irq_wait, ifc->i2c_ctl->status & I2C_STATUS_DONE, jiffies);
  debugk(("masking [%d : %x]\n", retval, ioread32( ifc->csr_ptr + TSC_CSR_ILOC_ITC_IMS)));
  iowrite32(irq, ifc->csr_ptr + TSC_CSR_ILOC_ITC_IMS);
  return retval;
}

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * Function name : tsc_i2c_cmd
 * Prototype     : int
 * Parameters    : pointer to tsc device control structure
 *                 pointer to i2c device control structure
 * Return        : return of wait function
 *----------------------------------------------------------------------------
 * Description   : execute i2c command
 *
 *++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

int 
tsc_i2c_cmd( struct tsc_device *ifc,
	     struct tsc_ioctl_i2c *i)
{
  int retval;
  int irq;

  debugk(("in tsc_i2c_cmd()...\n"));

  /* unmask I2C  interrupts */
  irq = TSC_ALL_ITC_IM( TSC_IRQ_SRC_I2C_OK);
  irq |= TSC_ALL_ITC_IM( TSC_IRQ_SRC_I2C_ERR);
  debugk(("I2C IRQ unmasking...%x\n", irq));
  iowrite32( irq, ifc->csr_ptr + TSC_CSR_ILOC_ITC_IMC);

  /* load command register */
  iowrite32( i->cmd, ifc->csr_ptr + TSC_CSR_I2C_CMD);

  /* trig command cycle */
  iowrite32( i->device & ~TSC_I2C_CTL_TRIG_MASK, ifc->csr_ptr + TSC_CSR_I2C_CTL);
  iowrite32( i->device |  TSC_I2C_CTL_TRIG_CMD, ifc->csr_ptr + TSC_CSR_I2C_CTL);

  /* wait for command to be ready */
  retval = tsc_i2c_wait( ifc, irq);
  i->status = ioread32( ifc->csr_ptr + TSC_CSR_I2C_CTL);
  debugk(("%08x\n", i->status));
  return( retval);
}

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * Function name : tsc_i2c_read
 * Prototype     : int
 * Parameters    : pointer to tsc device control structure
 *                 pointer to i2c device control structure
 * Return        : i2c read status
 *----------------------------------------------------------------------------
 * Description   : read i2c
 *
 *++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

int 
tsc_i2c_read( struct tsc_device *ifc,
	      struct tsc_ioctl_i2c *i)
{
  uint dev;
  int retval;
  int irq;

  /* perform command cycle */
  retval = tsc_i2c_cmd( ifc, i);
  if( retval)
  {
    debugk(("retval = %d\n", retval));
    //return( retval);
  }
  debugk(("in tsc_i2c_read()..."));

  dev = i->device & ~TSC_I2C_CTL_STR_REPEAT; /* clear Restart */

  /* unmask I2C  interrupts */
  irq = TSC_ALL_ITC_IM( TSC_IRQ_SRC_I2C_OK);
  irq |= TSC_ALL_ITC_IM( TSC_IRQ_SRC_I2C_ERR);
  debugk(("I2C IRQ unmasking...%x\n", irq));
  iowrite32( irq, ifc->csr_ptr + TSC_CSR_ILOC_ITC_IMC);

  /* trig read cycle */
  iowrite32( dev & ~TSC_I2C_CTL_TRIG_MASK, ifc->csr_ptr + TSC_CSR_I2C_CTL);
  iowrite32( dev |  TSC_I2C_CTL_TRIG_DATR, ifc->csr_ptr + TSC_CSR_I2C_CTL);

  /* wait for data to be ready */
  retval = tsc_i2c_wait( ifc, irq);
  debugk(("retval = %d\n", retval));

  /* get data */
  i->data = ioread32( ifc->csr_ptr + TSC_CSR_I2C_DATR);
  i->status = ioread32( ifc->csr_ptr + TSC_CSR_I2C_CTL);
  debugk(("%08x\n", i->status));

  return(retval);
}

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * Function name : tsc_i2c_write
 * Prototype     : int
 * Parameters    : pointer to tsc device control structure
 *                 pointer to i2c device control structure
 * Return        : 0
 *----------------------------------------------------------------------------
 * Description   : write <cnt> data in i2c
 *
 *++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

int 
tsc_i2c_write( struct tsc_device *ifc,
	       struct tsc_ioctl_i2c *i)
{
  uint dev;
  int retval;
  int irq;

  /* load command register */
  dev = i->device;
  if( dev & TSC_I2C_CTL_STR_REPEAT)
  {
    /* if split transaction > perform command cycle */
    retval = tsc_i2c_cmd( ifc, i);
    if( retval)
    {
      debugk(("retval = %d\n", retval));
      //return( retval);
    }
    /* clear start repeat bit */
    dev &= ~TSC_I2C_CTL_STR_REPEAT;
  }
  else
  {
    /* combine command with write cycle */
    iowrite32( i->cmd, ifc->csr_ptr + TSC_CSR_I2C_CMD);
  }
  /* load data register */
  iowrite32( i->data, ifc->csr_ptr + TSC_CSR_I2C_DATW);

  /* unmask I2C  interrupts */
  irq = TSC_ALL_ITC_IM( TSC_IRQ_SRC_I2C_OK);
  irq |= TSC_ALL_ITC_IM( TSC_IRQ_SRC_I2C_ERR);
  debugk(("I2C IRQ unmasking...%x\n", irq));
  iowrite32( irq, ifc->csr_ptr + TSC_CSR_ILOC_ITC_IMC);

  /* trig write cycle */
  iowrite32( dev & ~TSC_I2C_CTL_TRIG_MASK, ifc->csr_ptr + TSC_CSR_I2C_CTL);
  iowrite32( dev |  TSC_I2C_CTL_TRIG_DATW, ifc->csr_ptr + TSC_CSR_I2C_CTL);

  /* wait for data to be written */
  retval = tsc_i2c_wait( ifc, irq);
  debugk(("retval = %d\n", retval));

  return(0);
}
