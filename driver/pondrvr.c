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
 *   This file is the main file of the device driver modules for the IFC1211
 *   It contain all entry points for the driver:
 *     -> pon1211_init()    :
 *     -> pon1211_exit()    :
 *     -> pon1211_open()    :
 *     -> pon1211_release() :
 *     -> pon1211_read()    :
 *     -> pon1211_write()   :
 *     -> pon1211_llseek()  :
 *     -> pon1211_ioctl()   :
 *     -> pon1211_mmap()    :
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
#define PON1211_COUNT 1
#define PON1211_NAME "pon1211"

struct pon1211_device
{
  struct device *dev_ctl;                    /* IFC1211 control device                */
  void __iomem *pon_ptr;                     /* Base Address of pon registers     */
};
struct pon1211 
{
  struct cdev cdev;
  dev_t dev_id;
  struct pon1211_device *pon;
} pon1211; /* driver main data structure */

static const char device_name[] = "IFC1211_PON";
static struct class *pon_sysfs_class;	/* Sysfs class */

/*----------------------------------------------------------------------------
 * File operations for pon1211 device
 *----------------------------------------------------------------------------*/
struct file_operations pon1211_fops = 
{
  .owner =    THIS_MODULE,
#ifdef JFG
  .mmap =     pon1211_mmap,
  .llseek =   pon1211_llseek,
  .read =     pon1211_read,
  .write =    pon1211_write,
  .open =     pon1211_open,
  .unlocked_ioctl = pon1211_ioctl,
  .release =  pon1211_release,
#endif
};

/*----------------------------------------------------------------------------
 * Function name : pon1211_init
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

static int pon1211_init( void)
{
  int retval;
  dev_t pon1211_dev_id;
  struct pon1211_device *pon;

  debugk(( KERN_ALERT "pon1211: entering pon1211_init( void)\n"));

  /*--------------------------------------------------------------------------
   * device number dynamic allocation 
   *--------------------------------------------------------------------------*/
  retval = alloc_chrdev_region( &pon1211_dev_id, 0, PON1211_COUNT, PON1211_NAME);
  if( retval < 0) {
    debugk(( KERN_WARNING "pon1211: Error %d cannot allocate device number\n", retval));
    goto pon1211_init_err_alloc_chrdev;
  }
  else {
    debugk((KERN_WARNING "pon1211: registered with major number:%i\n", MAJOR( pon1211_dev_id)));
  }
  pon1211.dev_id = pon1211_dev_id;

  /*--------------------------------------------------------------------------
   * register driver
   *--------------------------------------------------------------------------*/
  cdev_init( &pon1211.cdev, &pon1211_fops);
  pon1211.cdev.owner = THIS_MODULE;
  pon1211.cdev.ops = &pon1211_fops;
  retval = cdev_add( &pon1211.cdev, pon1211.dev_id ,PON1211_COUNT);
  if(retval) {
    debugk((KERN_NOTICE "pon1211 : Error %d adding device\n", retval));
    goto pon1211_init_err_cdev_add;
  }
  debugk((KERN_NOTICE "pon1211 : device added\n"));
  pon1211.pon = (struct pon1211_device *)kzalloc(sizeof(struct pon1211_device), GFP_KERNEL);
  pon = pon1211.pon;

  /*--------------------------------------------------------------------------
   * Create sysfs entries - on udev systems this creates the dev files
   *--------------------------------------------------------------------------*/
  pon_sysfs_class = class_create( THIS_MODULE, device_name);
  if (IS_ERR( pon_sysfs_class))
  {
    dev_err( pon->dev_ctl, "Error creating pon class.\n");
    retval = PTR_ERR( pon_sysfs_class);
    goto pon1211_err_class;
  }
  /*--------------------------------------------------------------------------
   * Create IFC1211 control device in file system
   *--------------------------------------------------------------------------*/
  pon->dev_ctl = device_create( pon_sysfs_class, NULL, pon1211_dev_id, NULL, "ifc1211/pon");

  return( 0);

  /*--------------------------------------------------------------------------
   * Cleanup after an error has been detected
   *--------------------------------------------------------------------------*/
pon1211_err_class:
  cdev_del( &pon1211.cdev);
pon1211_init_err_cdev_add:
  unregister_chrdev_region( pon1211.dev_id, PON1211_COUNT);
pon1211_init_err_alloc_chrdev:

  return( retval);
}

static void pon1211_exit(void)
{
  debugk(( KERN_ALERT "pon1211: entering pon1211_exit( void)\n"));
  device_destroy( pon_sysfs_class, pon1211.dev_id);
  class_destroy( pon_sysfs_class);
  cdev_del( &pon1211.cdev);
  unregister_chrdev_region( pon1211.dev_id, PON1211_COUNT);
}

module_init( pon1211_init);
module_exit( pon1211_exit);


MODULE_LICENSE("GPL");
MODULE_AUTHOR("IOxOS Technologies [JFG]");
MODULE_VERSION(DRIVER_VERSION);
MODULE_DESCRIPTION("driver for IOxOS Technologies IFC1211 PON registers");
