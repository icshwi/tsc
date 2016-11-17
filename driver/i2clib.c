/*=========================< begin file & file header >=======================
 *  References
 *  
 *    filename : i2clib.c
 *    author   : JFG
 *    company  : IOxOS
 *    creation : Sept 27,2015
 *    version  : 0.0.1
 *
 *----------------------------------------------------------------------------
 *  Description
 *
 *    This file contains the low level functions to control I2Cs implemented
 *    in the ifc1211.
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
 *=============================< end file header >============================*/

#include "tscos.h"
#include "tscdrvr.h"

#define DBGno
#include "debug.h"

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * Function name : tsc_i2c_irq
 * Prototype     : int
 * Parameters    : pointer to ifc1211 device control structure
 * Return        : error/success
 *----------------------------------------------------------------------------
 * Description   : 
 *
 *++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

void
tsc_i2c_irq( struct ifc1211_device *ifc,
	     int src,
	     void *arg)
{
  up( &ifc->i2c_ctl->sem);
  return;
}

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * Function name : tsc_i2c_reset
 * Prototype     : int
 * Parameters    : pointer to ifc1211 device control structure
 *                 pointer to I2C device control structure
 * Return        : number of word actually written in I2C
 *----------------------------------------------------------------------------
 * Description   : reset I2C controller
 *
 *++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

int 
tsc_i2c_reset( struct ifc1211_device *ifc)
{
  return(0);
}

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * Function name : tsc_i2c_wait
 * Prototype     : int
 * Parameters    : pointer to ifc1211 device control structure
 *                 pointer to I2C device control structure
 * Return        : number of word actually written in I2C
 *----------------------------------------------------------------------------
 * Description   : wait I2C controller interrupt
 *
 *++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

int 
tsc_i2c_wait( struct ifc1211_device *ifc,
	      int irq)
{
  int jiffies, retval;
 
  jiffies =  msecs_to_jiffies( 10); /* timeout after 10 msec */
  retval = down_timeout( &ifc->i2c_ctl->sem, jiffies);
  debugk(("masking [%d : %x]\n", retval, ioread32( ifc->csr_ptr + IFC1211_CSR_ILOC_ITC_IMS)));
  iowrite32( irq, ifc->csr_ptr + IFC1211_CSR_ILOC_ITC_IMS);
  return( retval);
}

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * Function name : tsc_i2c_cmd
 * Prototype     : int
 * Parameters    : pointer to ifc1211 device control structure
 *                 pointer to I2C device control structure
 * Return        : number of word actually written in I2C
 *----------------------------------------------------------------------------
 * Description   : read <cnt> data from I2C
 *
 *++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

int 
tsc_i2c_cmd( struct ifc1211_device *ifc,
	     struct tsc_ioctl_i2c *i)
{
  int retval;
  int irq;

  debugk(("in tsc_i2c_cmd()...\n"));

  /* unmask I2C  interrupts */
  irq = IFC1211_ALL_ITC_IM( IFC1211_IRQ_SRC_I2C_OK);
  irq |= IFC1211_ALL_ITC_IM( IFC1211_IRQ_SRC_I2C_ERR);
  debugk(("I2C IRQ unmasking...%x\n", irq));
  iowrite32( irq, ifc->csr_ptr + IFC1211_CSR_ILOC_ITC_IMC);

  /* load command register */
  iowrite32( i->cmd, ifc->csr_ptr + IFC1211_CSR_I2C_CMD);

  /* trig command cycle */
  iowrite32( i->device & ~IFC1211_I2C_CTL_TRIG_MASK, ifc->csr_ptr + IFC1211_CSR_I2C_CTL);
  iowrite32( i->device |  IFC1211_I2C_CTL_TRIG_CMD, ifc->csr_ptr + IFC1211_CSR_I2C_CTL);

  /* wait for command to be ready */
  retval = tsc_i2c_wait( ifc, irq);
  i->status = ioread32( ifc->csr_ptr + IFC1211_CSR_I2C_CTL);
  debugk(("%08x\n", i->status));
  return( retval);
}

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * Function name : tsc_i2c_read
 * Prototype     : int
 * Parameters    : pointer to ifc1211 device control structure
 *                 pointer to I2C device control structure
 * Return        : I2C read status
 *----------------------------------------------------------------------------
 * Description   : read <cnt> data from I2C
 *
 *++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

int 
tsc_i2c_read( struct ifc1211_device *ifc,
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

  dev = i->device & ~IFC1211_I2C_CTL_STR_REPEAT; /* clear Restart */

  /* unmask I2C  interrupts */
  irq = IFC1211_ALL_ITC_IM( IFC1211_IRQ_SRC_I2C_OK);
  irq |= IFC1211_ALL_ITC_IM( IFC1211_IRQ_SRC_I2C_ERR);
  debugk(("I2C IRQ unmasking...%x\n", irq));
  iowrite32( irq, ifc->csr_ptr + IFC1211_CSR_ILOC_ITC_IMC);

  /* trig read cycle */
  iowrite32( dev & ~IFC1211_I2C_CTL_TRIG_MASK, ifc->csr_ptr + IFC1211_CSR_I2C_CTL);
  iowrite32( dev |  IFC1211_I2C_CTL_TRIG_DATR, ifc->csr_ptr + IFC1211_CSR_I2C_CTL);

  /* wait for data to be ready */
  retval = tsc_i2c_wait( ifc, irq);
  debugk(("retval = %d\n", retval));

  /* get data */
  i->data = ioread32( ifc->csr_ptr + IFC1211_CSR_I2C_DATR);
  i->status = ioread32( ifc->csr_ptr + IFC1211_CSR_I2C_CTL);
  debugk(("%08x\n", i->status));

  return(retval);
}

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * Function name : tsc_i2c_write
 * Prototype     : int
 * Parameters    : pointer to ifc1211 device control structure
 *                 pointer to I2C device control structure
 * Return        : number of word actually written in I2C
 *----------------------------------------------------------------------------
 * Description   : write <cnt> data in I2C
 *
 *++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

int 
tsc_i2c_write( struct ifc1211_device *ifc,
	       struct tsc_ioctl_i2c *i)
{
  uint dev;
  int retval;
  int irq;

  /* load command register */
  dev = i->device;
  if( dev & IFC1211_I2C_CTL_STR_REPEAT)
  {
    /* if split transaction > perform command cycle */
    retval = tsc_i2c_cmd( ifc, i);
    if( retval)
    {
      debugk(("retval = %d\n", retval));
      //return( retval);
    }
    /* clear start repeat bit */
    dev &= ~IFC1211_I2C_CTL_STR_REPEAT;
  }
  else
  {
    /* combine command with write cycle */
    iowrite32( i->cmd, ifc->csr_ptr + IFC1211_CSR_I2C_CMD);
  }
  /* load data register */
  iowrite32( i->data, ifc->csr_ptr + IFC1211_CSR_I2C_DATW);

  /* unmask I2C  interrupts */
  irq = IFC1211_ALL_ITC_IM( IFC1211_IRQ_SRC_I2C_OK);
  irq |= IFC1211_ALL_ITC_IM( IFC1211_IRQ_SRC_I2C_ERR);
  debugk(("I2C IRQ unmasking...%x\n", irq));
  iowrite32( irq, ifc->csr_ptr + IFC1211_CSR_ILOC_ITC_IMC);

  /* trig write cycle */
  iowrite32( dev & ~IFC1211_I2C_CTL_TRIG_MASK, ifc->csr_ptr + IFC1211_CSR_I2C_CTL);
  iowrite32( dev |  IFC1211_I2C_CTL_TRIG_DATW, ifc->csr_ptr + IFC1211_CSR_I2C_CTL);

  /* wait for data to be written */
  retval = tsc_i2c_wait( ifc, irq);
  debugk(("retval = %d\n", retval));

  return(0);
}
