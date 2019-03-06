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

static int tsc_probe(struct pci_dev *, const struct pci_device_id *);
static void tsc_remove(struct pci_dev *);

#define DBGno

#include "debug.h"

#define DRIVER_VERSION "3.00"

struct tsc tsc;      /* driver main data structure for device */

static const char device_name_central[] = TSC_NAME_CENTRAL;

static const struct pci_device_id tsc_id_central[] = {
	{ PCI_DEVICE(PCI_VENDOR_ID_IOXOS, PCI_DEVICE_ID_IOXOS_TSC_CENTRAL_1) },
	{ PCI_DEVICE(PCI_VENDOR_ID_IOXOS, PCI_DEVICE_ID_IOXOS_TSC_CENTRAL_2) },
	{ PCI_DEVICE(PCI_VENDOR_ID_IOXOS, PCI_DEVICE_ID_IOXOS_TSC_CENTRAL_3) },
	{ },
};

static struct pci_driver tsc_driver_central = {
	.name     = device_name_central,
	.id_table = tsc_id_central,
	.probe    = tsc_probe,
	.remove   = tsc_remove,
};

static struct class *bridge_sysfs_class_central; /* Sysfs class */

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
		printk("tsc: %s: Invalid argument\n", __func__);
		return IRQ_NONE;
	}

	ifc = (struct tsc_device *)arg;
	debugk((KERN_ALERT "tsc: entering tsc_irq( %x, %p)\n", irq, arg));

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
		printk("tsc: %s: irq %d will not be handled, irq 0-7 are supported\n",
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
	int minor = -1;

	debugk(( KERN_ALERT "tsc: entering tsc_open( %p, %p)\n", inode, filp));

	minor = iminor( file_inode(filp));
	debugk(( KERN_ALERT "tsc: minor value: %x)\n", minor));

	ifc = &tsc.ifc_central[minor];

	mutex_lock( &ifc->mutex_ctl);

	filp->private_data = (void *)ifc;

	mutex_unlock( &ifc->mutex_ctl);

	return( 0);
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

	//debugk(( KERN_ALERT "tsc: entering tsc_ioctl( %p, %x, %lx)\n", filp, cmd, arg));
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

	debugk(( KERN_ALERT "ifc: entering tsc_mmap( %p, %p)\n", filp, vma));

	size   = vma->vm_end - vma->vm_start;
	off    = vma->vm_pgoff << PAGE_SHIFT;
	debugk(( KERN_ALERT "ifc: entering tsc_mmap( %p, %p, %lx [%lx])\n", filp, vma,  off, size));
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

	ifc = ( struct tsc_device *)filp->private_data;
	mutex_lock( &ifc->mutex_ctl);

	debugk(( KERN_ALERT "tsc: entering tsc_release( %p, %p)\n", inode, filp));
	filp->private_data = (void *)NULL;

	mutex_unlock( &ifc->mutex_ctl);

	return( 0);
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
	int retval;
	struct tsc_device *ifc = NULL;
	short tmp;
	static int card = 0;

	retval = 0;
	debugk(( KERN_ALERT "tsc: entering tsc_probe( %p, %p) card:%d\n", pdev, id, card));

	if (card >= tsc.nr_devs) {
		dev_err(&pdev->dev, "This should not happen\n");
		return -1;
	}

	if (tsc.ifc_central == NULL){
		tsc.ifc_central = (struct tsc_device *)kzalloc(tsc.nr_devs * sizeof(struct tsc_device), GFP_KERNEL);
		if (tsc.ifc_central == NULL) {
			dev_err(&pdev->dev, "Failed to allocate memory for device structure\n");
			retval = -ENOMEM;
			goto tsc_probe_err_alloc_dev;
		}
	}
	ifc = &tsc.ifc_central[card];
	debugk((KERN_NOTICE "tsc_central : device %d data structure allocated %p\n", card, ifc));
	card++;

	/* Enable the device */
	retval = pci_enable_device(pdev);
	if (retval) {
		dev_err(&pdev->dev, "Unable to enable tsc device\n");
		goto tsc_probe_err_enable;
	}
	ifc->pdev = pdev;
	debugk((KERN_NOTICE "tsc : PCI device enabled\n"));

	/* Map Registers */
	retval = pci_request_regions( pdev, device_name_central);
	if (retval) {
		dev_err(&pdev->dev, "Unable to reserve resources\n");
		goto tsc_probe_err_resource;
	}
	debugk((KERN_NOTICE "tsc_central : PCI region allocated\n"));

	/* map CSR registers through BAR 3 (size 8k) */
	debugk((KERN_NOTICE "tsc : pci resource start BAR3 : %lx\n", pci_resource_start(pdev, 3)));
	ifc->csr_ptr = ioremap_nocache( pci_resource_start(pdev, 3), 2*4096);
	if( !ifc->csr_ptr) {
		dev_err(&pdev->dev, "Unable to remap CSR region\n");
		retval = -EIO;
		goto tsc_probe_err_remap;
	}
	debugk((KERN_NOTICE "tsc : CSR registers mapped -> %p : %08x\n",
			ifc->csr_ptr, ioread32( ifc->csr_ptr + TSC_CSR_ILOC_OPT_DYN_DAT)));

	/* map PON registers through IFC bus (size 4k) */
	debugk((KERN_NOTICE "tsc : map PON register thorough IFC bus : %lx\n", 0xfffd00000));
	ifc->pon_ptr = ioremap_nocache( 0xfffd00000, 0x1000);
	if( !ifc->pon_ptr) {
		dev_err(&pdev->dev, "Unable to remap PON registers\n");
	}
	debugk((KERN_NOTICE "tsc : PON registers mapped -> %p : %08x\n",
			ifc->pon_ptr, ioread32be( ifc->pon_ptr)));

	/* use MSI interrupt mechanism if possible */
	if( pci_enable_msi( pdev)){
		debugk((KERN_NOTICE "tsc : Cannot enable MSI\n"));
		goto tsc_probe_err_enable_msi;
	}
	debugk((KERN_NOTICE "tsc : MSI enabled : ifc irq = %d\n", pdev->irq));

	/* register interrupt service routine tsc_irq */
	if( request_irq( pdev->irq, tsc_irq, IRQF_SHARED, device_name_central, ifc)){
		debugk((KERN_NOTICE "tsc_central : Cannot register IRQ\n"));
		goto tsc_probe_err_request_irq;
	}
	debugk((KERN_NOTICE "tsc_central : IRQ registered\n"));

	/* create protection mutex for control device */
	mutex_init( &ifc->mutex_ctl);

	/* call tsc initialization function */
	retval = tsc_dev_init( ifc);

	if( retval < 0){
		tsc_dev_exit( ifc);
		goto tsc_probe_err_request_irq;
	}

	/* enable interrupts and PCIe master access from FPGA */
	pci_read_config_word( ifc->pdev, PCI_COMMAND, &tmp);
	tmp |= PCI_COMMAND_MASTER;
	tmp &= ~PCI_COMMAND_INTX_DISABLE;
	debugk(( KERN_NOTICE "tsc : enable interrupts and PCIe master access [%04x]\n", tmp));
	pci_write_config_word( ifc->pdev, PCI_COMMAND, tmp);

	pci_set_drvdata(pdev, ifc);
	return( retval);

tsc_probe_err_request_irq:
	pci_disable_msi( ifc->pdev);
tsc_probe_err_enable_msi:
	iounmap( ifc->csr_ptr);
tsc_probe_err_remap:
  	pci_release_regions( pdev);
tsc_probe_err_resource:
  	pci_disable_device( pdev);
tsc_probe_err_enable:
	kfree( tsc.ifc_central);
tsc_probe_err_alloc_dev:
  	return( retval);
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
	static int devs = 1;

	debugk(( KERN_ALERT "tsc: entering tsc_remove(%p) %x\n", pdev, pdev->device));

	if (tsc.ifc_central != NULL){
		tsc_dev_exit(ifc);
		mutex_destroy( &ifc->mutex_ctl);
		free_irq( ifc->pdev->irq, (void *)ifc);
		pci_disable_msi(ifc->pdev);
		iounmap( ifc->csr_ptr);
		if (devs == tsc.nr_devs) {
			kfree(tsc.ifc_central);
			tsc.ifc_central = NULL;
		}
	}

	pci_release_regions(pdev);
    pci_disable_device(pdev);

	devs++;
	return;
}

/*----------------------------------------------------------------------------
 * Function name : tsc_initialization
 * Prototype     : int
 * Parameters    : none
 * Return        : 0 if OK
 *----------------------------------------------------------------------------
 * Description   : function executed when the driver is installed
 *                 allocate major device number
 *                 register driver operation
 * 
 *----------------------------------------------------------------------------*/

static int tsc_initialization(void){
	int retval;
	dev_t tsc_dev_id;
	struct tsc_device *ifc_central = NULL;
	int major = -1;
	char name_central[32] = "bus/bridge/tsc_ctl_central";
	char name_device[32];
	struct pci_dev *pdev = NULL;
	int count = 0;

	debugk(( KERN_ALERT "tsc: entering tsc_initialization( void)\n"));

	/*--------------------------------------------------------------------------
	 * device number dynamic allocation
	 *--------------------------------------------------------------------------*/
	retval = alloc_chrdev_region( &tsc_dev_id, TSC_MINOR_START, TSC_COUNT, TSC_NAME);
	if( retval < 0) {
		debugk(( KERN_WARNING "tsc: Error %d cannot allocate device number\n", retval));
		goto tsc_init_err_alloc_chrdev;
	}
	else {
		debugk((KERN_WARNING "tsc: registered with major number:%i\n", MAJOR( tsc_dev_id)));
	}
	tsc.dev_id = tsc_dev_id;

	/*--------------------------------------------------------------------------
	 * register driver
	 *--------------------------------------------------------------------------*/
	cdev_init( &tsc.cdev, &tsc_fops);
	tsc.cdev.owner = THIS_MODULE;
	tsc.cdev.ops = &tsc_fops;
	retval = cdev_add( &tsc.cdev, tsc.dev_id ,TSC_COUNT);
	if(retval) {
		debugk((KERN_NOTICE "tsc : Error %d adding device\n", retval));
		goto tsc_init_err_cdev_add;
	}
	major = MAJOR(tsc_dev_id);
	debugk((KERN_NOTICE "tsc: device added\n"));

	/*--------------------------------------------------------------------------
	 * Register as PCI driver
	 *--------------------------------------------------------------------------*/
	tsc.nr_devs = 0;
	while((pdev=pci_get_device(PCI_VENDOR_ID_IOXOS, PCI_ANY_ID, pdev)) != NULL)
	{
		tsc.nr_devs++;
		debugk(( KERN_ALERT "Device %d found vendor_id=0x%x and device_id=0x%x...\n",tsc.nr_devs, pdev->vendor,pdev->device));
	}

	tsc.ifc_central = NULL;
	retval = pci_register_driver( &tsc_driver_central);
	if(retval){
		debugk((KERN_NOTICE "tsc_central : Error %d registering driver\n", retval));
	}

	/* verify if tsc device has been discovered */
	if( !tsc.ifc_central){
		device_destroy(bridge_sysfs_class_central, MKDEV(MAJOR(tsc.dev_id), 1));
		pci_unregister_driver( &tsc_driver_central);
		debugk((KERN_NOTICE "tsc_central : didn't find tsc_CENTRAL PCI device\n"));
	}
	else {
		ifc_central = tsc.ifc_central;
		debugk((KERN_NOTICE "tsc_central : driver registered [%p]\n", ifc_central));
	}

	/*--------------------------------------------------------------------------
	 * Check if at least one FPGA has been found
	 *--------------------------------------------------------------------------*/
	if(tsc.ifc_central == NULL){
		goto tsc_no_device;
	}

	/*--------------------------------------------------------------------------
	 * Create sysfs entries - on udev systems this creates the dev files
	 *--------------------------------------------------------------------------*/
	bridge_sysfs_class_central = class_create( THIS_MODULE, device_name_central);
	if (IS_ERR( bridge_sysfs_class_central)){
		retval = PTR_ERR(bridge_sysfs_class_central);
		for(count = 0; count < tsc.nr_devs; count++)
			tsc_remove(tsc.ifc_central[count].pdev);
	}
	else {
		for(count = 0; count < tsc.nr_devs; count++) {
			snprintf(name_device, 32, "%s%d", name_central, count);
			device_create(bridge_sysfs_class_central, NULL, MKDEV(major, count), NULL, name_device);
		}
	}

	return( 0);

	/*--------------------------------------------------------------------------
	 * Cleanup after an error has been detected
	 *--------------------------------------------------------------------------*/
tsc_no_device:
  	cdev_del( &tsc.cdev);
tsc_init_err_cdev_add:
  	unregister_chrdev_region(tsc.dev_id, TSC_COUNT);
tsc_init_err_alloc_chrdev:
	return( retval);
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
	int count;
	debugk(( KERN_ALERT "tsc: entering tsc_exit( void)\n"));

	if (tsc.ifc_central != NULL) {
		for(count = 0; count < tsc.nr_devs; count++)
			device_destroy(bridge_sysfs_class_central, MKDEV(MAJOR(tsc.dev_id), count));
		pci_unregister_driver( &tsc_driver_central);
		class_destroy(bridge_sysfs_class_central);
	}

	cdev_del(&tsc.cdev);
	unregister_chrdev_region( tsc.dev_id, TSC_COUNT);
}

module_init( tsc_initialization);
module_exit( tsc_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("IOxOS Technologies [JFG]");
MODULE_VERSION(DRIVER_VERSION);
MODULE_DESCRIPTION("driver for IOxOS Technologies TSC control interface");
MODULE_DEVICE_TABLE(pci, tsc_id_central);

/*================================< end file >================================*/
