#include <asm/uaccess.h>         /* copy_to_user and copy_from_user */
#include <linux/errno.h>         /* error codes                     */
#include <linux/slab.h>          /* kmalloc and kfree               */
#include <linux/cdev.h>          /* struct cdev                     */
#include <linux/pci.h>
#include <linux/fs.h>            /* chrdev allocation + semaphore   */
#include <linux/module.h>        /* module                          */
#include <linux/interrupt.h>
#include <linux/poll.h>
#include <linux/init.h>          // modules
#include <linux/sched.h>         // module
