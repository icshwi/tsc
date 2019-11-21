/*=========================< begin file & file header >=======================
 *  References
 *  
 *    filename : tscdrvr.c
 *    author   : JFG, XP
 *    company  : IOxOS
 *    creation : Sept 30,2015
 *
 *----------------------------------------------------------------------------
 *  Description
 *
 *   This file is the main file of the device driver modules for the tsc
 *   It contain all entry points for the driver.
 *
 *----------------------------------------------------------------------------
 *
 *  Copyright (C) IOxOS Technologies SA <ioxos@ioxos.ch>
 *  Copyright (C) 2019  European Spallation Source ERIC
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

#define DRIVER_VERSION TSC_VERSION

LIST_HEAD(tsc_devlist);             /**< List of device context structs for available devices. */
DEFINE_MUTEX(tsc_devlist_lock);     /**< Lock that serializes access to the list of available devices. */

static const struct pci_device_id tsc_id_central[] = {
	{ PCI_DEVICE(PCI_VENDOR_ID_IOXOS, PCI_DEVICE_ID_IOXOS_TSC_CENTRAL_1) },
	{ PCI_DEVICE(PCI_VENDOR_ID_IOXOS, PCI_DEVICE_ID_IOXOS_TSC_CENTRAL_2) },
	{ PCI_DEVICE(PCI_VENDOR_ID_IOXOS, PCI_DEVICE_ID_IOXOS_TSC_CENTRAL_3) },
	{ },
};

static struct class *bridge_sysfs_class_central; /* Sysfs class */
static const char device_name_central[] = TSC_NAME_CENTRAL;

static int tsc_probe(struct pci_dev *, const struct pci_device_id *);
static void tsc_remove(struct pci_dev *);

static struct pci_driver tsc_driver_central = {
	.name     = device_name_central,
	.id_table = tsc_id_central,
	.probe    = tsc_probe,
	.remove   = tsc_remove,
};

static char *central_devnode(struct device *dev, umode_t *mode)
{
	if (mode)
		*mode = 0666;
	return NULL;
}

/*----------------------------------------------------------------------------
 * Function name : tsc_irq
 * Prototype     : irqreturn_t
 * Parameters    : irq and argument
 * Return        : 0 if OK
 *----------------------------------------------------------------------------
 * Description   : tsc_irq() is the low level interrupt handler for the
 *                 tsc interface.
 *
 *----------------------------------------------------------------------------*/

irqreturn_t tsc_irq(int irq, void *arg){
	struct tsc_device *ifc;
	register uint ip, itc;
	register uint base;
	register uint src, idx;

	if (arg == NULL)
	{
		printk(KERN_ERR "%s: %s: Invalid argument\n", device_name_central, __func__);
		return IRQ_NONE;
	}

	ifc = (struct tsc_device *)arg;
	debugk((KERN_DEBUG "%s: entering tsc_irq( %x, %p)\n", device_name_central, irq, arg));

	/* generate IACK cycle */
	ip = ioread32(ifc->csr_ptr + TSC_CSR_ILOC_ITC_IACK);
	if (ip == 0x0)
		return IRQ_NONE;

	/* get interrupt source */
	src  = ip & 0x7fff;
	itc = TSC_ALL_ITC_IACK_ITC(src);
	idx  = TSC_ALL_ITC_IACK_IP(src);
	base = TSC_ITC_IACK_BASE(src);
	ip   = 1 << ((ip >> 8) & 0xf);

	/* mask interrupt source */
	iowrite32(ip, ifc->csr_ptr + base + TSC_CSR_ILOC_ITC_IMS);

	if (idx > 7)
	{
		printk(KERN_NOTICE "%s: %s: irq %d will not be handled, irq 0-7 are supported\n", device_name_central,
			__func__, idx);
		/* clear IP and restart interrupt scanning */
		iowrite32(ip<<16, ifc->csr_ptr + base + TSC_CSR_ILOC_ITC_IACK);
		return IRQ_NONE;
	}

	/* increment interrupt counter */
	ifc->irq_tbl[itc][idx].cnt += 1;

	/* activates tasklet handling interrupts */
	if (ifc->irq_tbl[itc][idx].func)
		ifc->irq_tbl[itc][idx].func(ifc, src, ifc->irq_tbl[itc][idx].arg);

	/* clear IP and restart interrupt scanning */
	iowrite32(ip<<16, ifc->csr_ptr + base + TSC_CSR_ILOC_ITC_IACK);

	return IRQ_HANDLED;
}

/*----------------------------------------------------------------------------
 * Function name : tsc_open
 * Prototype     : int
 * Parameters    : inode -> pointer to device node data structure
 *                 filp  -> pointer to the file data structure
 * Return        : 0 if OK
 *----------------------------------------------------------------------------
 * Description   : tsc_open() opens the tsc control device giving
 *                 access to the device internal registers
 *
 *----------------------------------------------------------------------------*/

static int tsc_open( struct inode *inode, struct file *filp){
	struct tsc_device *ifc;

	debugk((KERN_DEBUG "%s: entering tsc_open( %p, %p)\n", device_name_central, inode, filp));

	mutex_lock(&tsc_devlist_lock);
	list_for_each_entry(ifc, &tsc_devlist, list) {
		if (MAJOR(ifc->dev_id) == imajor(inode) &&
		    MINOR(ifc->dev_id) == iminor(inode)) {
			mutex_lock(&ifc->mutex_ctl);
			filp->private_data = (void *)ifc;
			mutex_unlock(&ifc->mutex_ctl);
			mutex_unlock(&tsc_devlist_lock);

			return 0;
		}
	}
	mutex_unlock(&tsc_devlist_lock);

	printk(KERN_ERR "%s: device not found\n", device_name_central);
	return -ENODEV;
}

/*----------------------------------------------------------------------------
 * Function name : tsc_ioctl
 * Prototype     : long
 * Parameters    : filp  -> pointer to the file data structure
 *                 cmd   -> command code
 *                 arg   -> command argument
 * Return        : 0 if OK
 *----------------------------------------------------------------------------
 * Description   : tsc_release() releasess the tsc control device
 *
 *----------------------------------------------------------------------------*/

static long tsc_ioctl(struct file *filp, unsigned int cmd, unsigned long arg){
	struct tsc_device *ifc;
	int retval;

	ifc = (struct tsc_device *)filp->private_data;

	retval = 0;
	switch (cmd &  TSC_IOCTL_OP_MASK)
	{
		case TSC_IOCTL_ID:{
			if(cmd == TSC_IOCTL_ID_NAME){
				if(copy_to_user((void *)arg, TSC_NAME_CENTRAL, strlen(TSC_NAME_CENTRAL))){
					retval = -EFAULT;
				}
    			}
			else if(cmd == TSC_IOCTL_ID_VERSION){
				if(copy_to_user((void *)arg, DRIVER_VERSION, strlen( DRIVER_VERSION))){
					retval = -EFAULT;
				}
    			}
			else if(cmd == TSC_IOCTL_ID_VENDOR){
				if(copy_to_user((void *)arg, &ifc->pdev->vendor, sizeof(short))){
					retval = -EFAULT;
				}
			}
			else if(cmd == TSC_IOCTL_ID_DEVICE){
				if( copy_to_user((void *)arg, &ifc->pdev->device, sizeof(short))){
					retval = -EFAULT;
				}
			}
			else{
				retval = -EINVAL;
			}
			break;
		}
		case TSC_IOCTL_CSR:{
			retval = ioctl_csr(ifc, cmd, arg);
			break;
		}
		case TSC_IOCTL_MAP:{
			retval = ioctl_map(ifc, cmd, arg);
			break;
		}
		case TSC_IOCTL_RDWR:{
			retval = ioctl_rdwr(ifc, cmd, arg);
			break;
		}
		case TSC_IOCTL_DMA:{
			retval = ioctl_dma(ifc, cmd, arg);
			break;
		}
		case TSC_IOCTL_KBUF:{
			retval = ioctl_kbuf(ifc, cmd, arg);
			break;
		}
		case TSC_IOCTL_SFLASH:{
			retval = ioctl_sflash(ifc, cmd, arg);
			break;
		}
		case TSC_IOCTL_TIMER:{
			retval = ioctl_timer(ifc, cmd, arg);
			break;
		}
		case TSC_IOCTL_FIFO:{
			retval = ioctl_fifo(ifc, cmd, arg);
			break;
		}
		case TSC_IOCTL_I2C:{
			retval = ioctl_i2c(ifc, cmd, arg);
			break;
		}
		case TSC_IOCTL_SEMAPHORE:{
			retval = ioctl_semaphore(ifc, cmd, arg);
			break;
		}
		case TSC_IOCTL_USER:{
			retval = ioctl_user_irq(ifc, cmd, arg);
			break;
		}
		default:{
    			retval = -EINVAL;
    		}
	}

	return retval;
}

/*----------------------------------------------------------------------------
 * Function name : tsc_mmap
 * Prototype     : int
 * Parameters    : filp  -> pointer to the file data structure
 *                 vma  -> pointer to 
 * Return        : 0 if OK
 *----------------------------------------------------------------------------
 * Description   : tsc__mmap() maps a memory window in user space
 *
 *----------------------------------------------------------------------------*/

static int tsc_mmap( struct file *filp, struct vm_area_struct *vma){
	int retval;
	ssize_t size;
	off_t off;

	debugk((KERN_DEBUG "%s: entering tsc_mmap( %p, %p)\n", device_name_central, filp, vma));

	size   = vma->vm_end - vma->vm_start;
	off    = vma->vm_pgoff << PAGE_SHIFT;
	debugk((KERN_DEBUG "%s: entering tsc_mmap( %p, %p, %lx [%lx])\n", device_name_central, filp, vma,  off, size));
	if( (off & 0xc00000000) == 0xc00000000)
	{
	  vma->vm_page_prot = pgprot_noncached(vma->vm_page_prot);
	  retval = io_remap_pfn_range( vma,
				       vma->vm_start,
				       vma->vm_pgoff,
				       size,
				       vma->vm_page_prot);
	}
	else 
	{
	  retval = remap_pfn_range( vma,  vma->vm_start, vma->vm_pgoff, size, vma->vm_page_prot);
	}
	return( retval);
}

/*----------------------------------------------------------------------------
 * Function name : tsc_release
 * Prototype     : int
 * Parameters    : inode -> pointer to device node data structure
 *                 filp  -> pointer to the file data structure
 * Return        : 0
 *----------------------------------------------------------------------------
 * Description   : tsc_release() releases the tsc control device
 *
 *----------------------------------------------------------------------------*/

static int tsc_release( struct inode *inode, struct file *filp){
	struct tsc_device *ifc;

	ifc = (struct tsc_device *)filp->private_data;
	mutex_lock(&ifc->mutex_ctl);

	debugk((KERN_DEBUG "%s: entering tsc_release( %p, %p)\n", device_name_central, inode, filp));
	filp->private_data = (void *)NULL;

	mutex_unlock(&ifc->mutex_ctl);

	return 0;
}

struct file_operations tsc_fops = {
										.owner          = THIS_MODULE,
										.mmap           = tsc_mmap,
#ifdef JFG
										.llseek         = tsc_llseek,
										.read           = tsc_read,
										.write          = tsc_write,
#endif
										.open           = tsc_open,
										.unlocked_ioctl = tsc_ioctl,
										.release        = tsc_release,
};

/*----------------------------------------------------------------------------
 * Function name : tsc_probe
 * Prototype     : int
 * Parameters    : pdev -> pointer to pci device table entry
 *                 id   -> pointer to pci identifier
 * Return        : 0 if OK
 *----------------------------------------------------------------------------
 * Description   : tsc_probe() is called for each pci device found in the
 *                 pci device table whose id/did matched the list declared in
 *                 tsc_ids structure.
 *
 *----------------------------------------------------------------------------*/

static int tsc_probe( struct pci_dev *pdev, const struct pci_device_id *id){
	int retval = 0;
	struct tsc_device *ifc = NULL;
	short tmp;
	uint16_t pcieflags = 0;
	uint32_t slotcap, slotno;
	struct pci_dev *bus_dev;
	char name_central[32] = "bus/bridge/tsc_ctl_central";
	char device_name[64];

	debugk((KERN_DEBUG "%s: entering tsc_probe( %p, %p)\n", device_name_central, pdev, id));

	ifc = (struct tsc_device *)kzalloc(sizeof(struct tsc_device), GFP_KERNEL);
	if (ifc == NULL) {
		printk(KERN_ERR "%s: Failed to allocate memory for device structure\n", device_name_central);
		return -ENOMEM;
	}
	pci_set_drvdata(pdev, ifc);
	ifc->pdev = pdev;

	bus_dev = pdev->bus->self;
	if (pci_is_pcie(bus_dev)) {
		pci_read_config_word(bus_dev, pci_pcie_cap(bus_dev) + PCI_EXP_FLAGS, &pcieflags);
	}

	if (pcieflags & PCI_EXP_FLAGS_SLOT) {
		/* Construct device name based on slot number. */
		pci_read_config_dword(bus_dev, pci_pcie_cap(bus_dev) + PCI_EXP_SLTCAP, &slotcap);
		slotno = (slotcap >> 19) & 0x1FFF;
		snprintf(device_name, 32, "%s%d", name_central, slotno);
		ifc->card = slotno;
		debugk((KERN_INFO "%s: %s%d\n", device_name_central, name_central, slotno));
		debugk((KERN_INFO "%s: device in physical PCIe slot #%d\n", device_name_central, slotno));
	}
	else {
		/* Construct device name starting from 0 */
		static uint32_t card = 0;
		ifc->card = card;
		snprintf(device_name, 32, "%s%d", name_central, ifc->card);
		debugk((KERN_INFO "%s: %s%d\n", device_name_central, name_central, ifc->card));
		debugk((KERN_INFO "%s: device not in a PCIe slot\n", device_name_central));
		card++;
	}

	/* Enable the device */
	retval = pci_enable_device(pdev);
	if (retval) {
		printk(KERN_ERR "%s: Error to enable tsc device\n", device_name_central);
		goto tsc_probe_err_enable;
	}
	debugk((KERN_INFO "%s: PCI device enabled\n", device_name_central));

	/* Map Registers */
	retval = pci_request_regions(pdev, device_name_central);
	if (retval) {
		printk(KERN_ERR "%s: Unable to reserve resources\n", device_name_central);
		goto tsc_probe_err_resource;
	}
	debugk((KERN_INFO "%s: PCI region allocated\n", device_name_central));

	/* map CSR registers through BAR 3 (size 8k) */
	debugk((KERN_INFO "%s: pci resource start BAR3 : %llx\n", device_name_central, pci_resource_start(pdev, 3)));
	ifc->csr_ptr = pci_iomap(pdev, 3, 0);
	if (!ifc->csr_ptr) {
		printk(KERN_ERR "%s: Unable to remap CSR region\n", device_name_central);
		retval = -EIO;
		goto tsc_probe_err_remap;
	}
	debugk((KERN_INFO "%s: CSR registers mapped -> %p : %08x\n", device_name_central,
			ifc->csr_ptr, ioread32(ifc->csr_ptr + TSC_CSR_ILOC_OPT_DYN_DAT)));

	/* map PON registers through IFC bus (size 4k) */
	debugk((KERN_INFO "%s: map PON register thorough IFC bus : %lx\n", device_name_central, 0xfffd00000));
	ifc->pon_ptr = ioremap_nocache(0xfffd00000, 0x1000);
	if (!ifc->pon_ptr) {
		printk(KERN_ERR "%s: Unable to remap PON registers\n", device_name_central);
	}
	else {
		debugk((KERN_INFO "%s: PON registers mapped -> %p : %08x\n", device_name_central,
			ifc->pon_ptr, ioread32be(ifc->pon_ptr)));
	}

	/*--------------------------------------------------------------------------
	 * device number dynamic allocation
	 *--------------------------------------------------------------------------*/
	retval = alloc_chrdev_region(&ifc->dev_id, TSC_MINOR_START, 1, device_name_central);
	if (retval < 0) {
		printk(KERN_ERR "%s: Error %d cannot allocate device number\n", device_name_central, retval);
		goto tsc_probe_err_alloc_chrdev;
	}
	else {
		debugk((KERN_INFO "%s: allocated with major number: %d\n", device_name_central, MAJOR(ifc->dev_id)));
	}

	/*--------------------------------------------------------------------------
	 * register driver
	 *--------------------------------------------------------------------------*/
	cdev_init(&ifc->cdev, &tsc_fops);
	ifc->cdev.owner = THIS_MODULE;
	ifc->cdev.ops = &tsc_fops;
	retval = cdev_add(&ifc->cdev, ifc->dev_id, 1);
	if (retval) {
		printk(KERN_ERR "%s: Error %d adding device\n", device_name_central, retval);
		goto tsc_probe_err_cdev_add;
	}
	debugk((KERN_INFO "%s: char device added\n", device_name_central));

	ifc->dev_ctl = device_create(bridge_sysfs_class_central, &pdev->dev, ifc->dev_id, NULL, device_name);
	if (IS_ERR(ifc->dev_ctl)) {
		printk(KERN_ERR "%s: can't create device\n", device_name_central);
		retval = PTR_ERR(ifc->dev_ctl);
		goto tsc_probe_err_devcreate;
	}
	debugk((KERN_INFO "%s: device created: major=%d, minor=%d\n", device_name_central,
				MAJOR(ifc->dev_id), MINOR(ifc->dev_id)));

	/* Setup dma operation. */
	if (dma_set_mask_and_coherent(&ifc->pdev->dev, DMA_BIT_MASK(64))) {
		printk(KERN_ALERT "%s: unable to set DMA(64) mask\n", device_name_central);
		if (dma_set_mask_and_coherent(&ifc->pdev->dev, DMA_BIT_MASK(32))) {
			printk(KERN_ERR "%s: unable to set DMA(32) mask\n", device_name_central);
			retval = -EFAULT;
			goto tsc_probe_err_dma_mask;
		}
	}

	/* use MSI interrupt mechanism if possible */
	if (pci_enable_msi(pdev)) {
		printk(KERN_ERR "%s: Cannot enable MSI\n", device_name_central);
		retval = -EFAULT;
		goto tsc_probe_err_enable_msi;
	}
	debugk((KERN_INFO "%s: MSI enabled : ifc irq = %d\n", device_name_central, pdev->irq));

	/* register interrupt service routine tsc_irq */
	if (request_irq(pdev->irq, tsc_irq, IRQF_SHARED, device_name_central, ifc)){
		printk(KERN_ERR "%s: Cannot register IRQ\n", device_name_central);
		retval = -EFAULT;
		goto tsc_probe_err_request_irq;
	}
	debugk((KERN_INFO "%s: IRQ registered\n", device_name_central));

	/* create protection mutex for control device */
	mutex_init(&ifc->mutex_ctl);

	/* call tsc initialization function */
	retval = tsc_dev_init(ifc);

	if (retval < 0) {
		tsc_dev_exit(ifc);
		goto tsc_probe_err_request_irq;
	}

	/* enable interrupts and PCIe master access from FPGA */
	pci_read_config_word(ifc->pdev, PCI_COMMAND, &tmp);
	tmp |= PCI_COMMAND_MASTER;
	tmp &= ~PCI_COMMAND_INTX_DISABLE;
	debugk((KERN_INFO "%s: enable interrupts and PCIe master access [%04x]\n", device_name_central, tmp));
	pci_write_config_word(ifc->pdev, PCI_COMMAND, tmp);

	mutex_lock(&tsc_devlist_lock);
	list_add_tail(&ifc->list, &tsc_devlist);
	mutex_unlock(&tsc_devlist_lock);

	printk(KERN_INFO "%s: device %d added\n", device_name_central, ifc->card);
	return retval;

tsc_probe_err_request_irq:
	pci_disable_msi(ifc->pdev);
tsc_probe_err_enable_msi:
tsc_probe_err_dma_mask:
	device_destroy(bridge_sysfs_class_central, ifc->dev_id);
tsc_probe_err_devcreate:
	cdev_del(&ifc->cdev);
tsc_probe_err_cdev_add:
	unregister_chrdev_region(ifc->dev_id, 1);
tsc_probe_err_alloc_chrdev:
	pci_iounmap(pdev, ifc->csr_ptr);
	iounmap(ifc->pon_ptr);
tsc_probe_err_remap:
	pci_release_regions(pdev);
tsc_probe_err_resource:
	pci_disable_device(pdev);
tsc_probe_err_enable:
	kfree(ifc);
	return retval;
}

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * Function name : tsc_remove
 * Prototype     : void
 * Parameters    : pointer to pci_dev data structure
 * Return        : void
 *----------------------------------------------------------------------------
 * Description   : return devices resources to OS
 *
 *++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

static void tsc_remove( struct pci_dev *pdev){
	struct tsc_device *ifc = pci_get_drvdata(pdev);

	debugk((KERN_DEBUG "%s: entering tsc_remove(%p) %x\n", device_name_central, pdev, pdev->device));

	mutex_lock(&tsc_devlist_lock);
	mutex_lock(&ifc->mutex_ctl);

	/* delete from list */
	list_del(&ifc->list);

	/* clean up device and interrupts */
	tsc_dev_exit(ifc);
	free_irq(ifc->pdev->irq, (void *)ifc);

	/* clean up pci */
	device_destroy(bridge_sysfs_class_central, ifc->dev_id);
	cdev_del(&ifc->cdev);
	unregister_chrdev_region(ifc->dev_id, 1);
	pci_disable_device(pdev);
	pci_release_regions(pdev);
	pci_disable_msi(ifc->pdev);
	pci_iounmap(pdev, ifc->csr_ptr);
	iounmap(ifc->pon_ptr);
	pci_set_drvdata(ifc->pdev, NULL);

	mutex_unlock(&ifc->mutex_ctl);
	kfree(ifc);
	mutex_unlock(&tsc_devlist_lock);

	printk(KERN_INFO "%s: device %d removed\n", device_name_central, ifc->card);
}

/*----------------------------------------------------------------------------
 * Function name : tsc_initialization
 * Prototype     : int
 * Parameters    : none
 * Return        : 0 if OK
 *----------------------------------------------------------------------------
 * Description   : function executed when the driver is installed
 *                 create class
 *                 register driver operation
 * 
 *----------------------------------------------------------------------------*/

static int tsc_initialization(void){
	int retval;

	debugk((KERN_DEBUG "%s: entering tsc_initialization( void)\n", device_name_central));

	/*--------------------------------------------------------------------------
	 * Create sysfs entries - on udev systems this creates the dev files
	 *--------------------------------------------------------------------------*/
	bridge_sysfs_class_central = class_create(THIS_MODULE, device_name_central);
	if (IS_ERR(bridge_sysfs_class_central)) {
		retval = PTR_ERR(bridge_sysfs_class_central);
		printk(KERN_ERR "%s : Error creating device class, status=%d\n", device_name_central, retval);
		return retval;
	}
	else {
		bridge_sysfs_class_central->devnode = central_devnode;
	}

	retval = pci_register_driver(&tsc_driver_central);
	if (retval) {
		printk(KERN_ERR "%s : Error registering pci driver, status=%d\n", device_name_central, retval);
	}

	printk(KERN_INFO "%s: driver loaded\n", device_name_central);
	return retval;
}

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * Function name : tsc_exit
 * Prototype     : void
 * Parameters    : none
 * Return        : none
 *----------------------------------------------------------------------------
 * Description   : function called when ifc mas driver is removed
 *                 free all allocated resources
 *
 *++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

static void tsc_exit(void){
	debugk((KERN_DEBUG "%s: entering tsc_exit( void)\n", device_name_central));

	pci_unregister_driver(&tsc_driver_central);
	class_destroy(bridge_sysfs_class_central);

	printk(KERN_INFO "%s: driver unloaded\n", device_name_central);
}

module_init(tsc_initialization);
module_exit(tsc_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("IOxOS Technologies [JFG]");
MODULE_VERSION(DRIVER_VERSION);
MODULE_DESCRIPTION("driver for IOxOS Technologies TSC control interface");
MODULE_DEVICE_TABLE(pci, tsc_id_central);

/*================================< end file >================================*/
