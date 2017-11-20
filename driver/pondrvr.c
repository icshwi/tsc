/*=========================< begin file & file header >=======================
 *  References
 *  
 *    filename : pondrvr.c
 *    author   : JFG
 *    company  : IOxOS
 *    creation : Sept 30,2015
 *    version  : 1.0.0
 *
 *----------------------------------------------------------------------------
 *  Description
 *
 *   This file is the main file of the device driver modules for the TSC
 *   It contain all entry points for the driver:
 *     -> pontsc_init()    :
 *     -> pontsc_exit()    :
 *     -> pontsc_open()    :
 *     -> pontsc_release() :
 *     -> pontsc_read()    :
 *     -> pontsc_write()   :
 *     -> pontsc_llseek()  :
 *     -> pontsc_ioctl()   :
 *     -> pontsc_mmap()    :
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

#define DBG

#include "debug.h"

#define DRIVER_VERSION "1.00"
#define PONTSC_COUNT 1
#define PONTSC_NAME "pontsc"

struct pontsc_device
{
  struct device *dev_ctl;                    /* TSC control device                */
  void __iomem *pon_ptr;                     /* Base Address of pon registers     */
};
struct pontsc1
{
  struct cdev cdev;
  dev_t dev_id;
  struct pontsc_device *pon;
} pontsc; /* driver main data structure */

static const char device_name[] = "TSC_PON";
static struct class *pon_sysfs_class;	/* Sysfs class */

/*----------------------------------------------------------------------------
 * File operations for pontsc device
 *----------------------------------------------------------------------------*/
struct file_operations pontsc_fops =
{
  .owner =    THIS_MODULE,
#ifdef JFG
  .mmap =     pontsc_mmap,
  .llseek =   pontsc_llseek,
  .read =     pontsc_read,
  .write =    pontsc_write,
  .open =     pontsc_open,
  .unlocked_ioctl = pontsc_ioctl,
  .release =  pontsc_release,
#endif
};

/*----------------------------------------------------------------------------
 * Function name : pontsc_init
 * Prototype     : int
 * Parameters    : none
 * Return        : 0 if OK
 *----------------------------------------------------------------------------
 * Description
 *
 * Function executed  when the driver is installed
 * Allocate major device number
 * Register driver operations
 * 
 *----------------------------------------------------------------------------*/

static int pontsc_init( void)
{
  int retval;
  dev_t pontsc_dev_id;
  struct pontsc_device *pon;

  debugk(( KERN_ALERT "pontsc: entering pontsc_init( void)\n"));

  /*--------------------------------------------------------------------------
   * device number dynamic allocation 
   *--------------------------------------------------------------------------*/
  retval = alloc_chrdev_region( &pontsc_dev_id, 0, PONTSC_COUNT, PONTSC_NAME);
  if( retval < 0) {
    debugk(( KERN_WARNING "pontsc: Error %d cannot allocate device number\n", retval));
    goto pontsc_init_err_alloc_chrdev;
  }
  else {
    debugk((KERN_WARNING "pontsc: registered with major number:%i\n", MAJOR( pontsc_dev_id)));
  }
  pontsc.dev_id = pontsc_dev_id;

  /*--------------------------------------------------------------------------
   * register driver
   *--------------------------------------------------------------------------*/
  cdev_init( &pontsc.cdev, &pontsc_fops);
  pontsc.cdev.owner = THIS_MODULE;
  pontsc.cdev.ops = &pontsc_fops;
  retval = cdev_add( &pontsc.cdev, pontsc.dev_id ,PONTSC_COUNT);
  if(retval) {
    debugk((KERN_NOTICE "pontsc : Error %d adding device\n", retval));
    goto pontsc_init_err_cdev_add;
  }
  debugk((KERN_NOTICE "pontsc : device added\n"));
  pontsc.pon = (struct pontsc_device *)kzalloc(sizeof(struct pontsc_device), GFP_KERNEL);
  pon = pontsc.pon;

  /*--------------------------------------------------------------------------
   * Create sysfs entries - on udev systems this creates the dev files
   *--------------------------------------------------------------------------*/
  pon_sysfs_class = class_create( THIS_MODULE, device_name);
  if (IS_ERR( pon_sysfs_class))
  {
    dev_err( pon->dev_ctl, "Error creating pon class.\n");
    retval = PTR_ERR( pon_sysfs_class);
    goto pontsc_err_class;
  }
  /*--------------------------------------------------------------------------
   * Create IFCTSC control device in file system
   *--------------------------------------------------------------------------*/
  pon->dev_ctl = device_create( pon_sysfs_class, NULL, pontsc_dev_id, NULL, "tsc/pon");

  return( 0);

  /*--------------------------------------------------------------------------
   * Cleanup after an error has been detected
   *--------------------------------------------------------------------------*/
pontsc_err_class:
  cdev_del( &pontsc.cdev);
pontsc_init_err_cdev_add:
  unregister_chrdev_region( pontsc.dev_id, PONTSC_COUNT);
pontsc_init_err_alloc_chrdev:

  return( retval);
}

static void pontsc_exit(void)
{
  debugk(( KERN_ALERT "pontsc: entering pontsc_exit( void)\n"));
  device_destroy( pon_sysfs_class, pontsc.dev_id);
  class_destroy( pon_sysfs_class);
  cdev_del( &pontsc.cdev);
  unregister_chrdev_region( pontsc.dev_id, PONTSC_COUNT);
}

module_init( pontsc_init);
module_exit( pontsc_exit);


MODULE_LICENSE("GPL");
MODULE_AUTHOR("IOxOS Technologies [JFG]");
MODULE_VERSION(DRIVER_VERSION);
MODULE_DESCRIPTION("driver for IOxOS Technologies TSC PON registers");
