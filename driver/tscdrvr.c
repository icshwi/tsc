/*=========================< begin file & file header >=======================
 *  References
 *  
 *    filename : tscdrvr.c
 *    author   : JFG, XP
 *    company  : IOxOS
 *    creation : Sept 30,2015
 *    version  : 1.0.0
 *
 *----------------------------------------------------------------------------
 *  Description
 *
 *   This file is the main file of the device driver modules for the ifc1211
 *   It contain all entry points for the driver.
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
 *=============================< end file header >============================*/

#include "tscos.h"
#include "tscdrvr.h"

static int ifc1211_probe(struct pci_dev *, const struct pci_device_id *);
static void ifc1211_remove(struct pci_dev *);

#define DBGno

#include "debug.h"

#define DRIVER_VERSION "1.40"

struct ifc1211 ifc1211;      /* driver main data structure for device */

static const char device_name_io[]      = IFC1211_NAME_IO;
static const char device_name_central[] = IFC1211_NAME_CENTRAL;

static DEFINE_PCI_DEVICE_TABLE(ifc1211_id_io) = {
    { PCI_DEVICE(PCI_VENDOR_ID_IOXOS, PCI_DEVICE_ID_IOXOS_IFC1211_IO) },
	{ },
};

static DEFINE_PCI_DEVICE_TABLE(ifc1211_id_central) = {
	{ PCI_DEVICE(PCI_VENDOR_ID_IOXOS, PCI_DEVICE_ID_IOXOS_IFC1211_CENTRAL) },
	{ },
};

static struct pci_driver ifc1211_driver_io = {
	.name     = device_name_io,
	.id_table = ifc1211_id_io,
	.probe    = ifc1211_probe,
	.remove   = ifc1211_remove,
};
static struct pci_driver ifc1211_driver_central = {
	.name     = device_name_central,
	.id_table = ifc1211_id_central,
	.probe    = ifc1211_probe,
	.remove   = ifc1211_remove,
};

static struct class *bridge_sysfs_class_io;	     /* Sysfs class */
static struct class *bridge_sysfs_class_central; /* Sysfs class */

static int ifc1211_probe(struct pci_dev *, const struct pci_device_id *);
static void ifc1211_remove(struct pci_dev *);

/*----------------------------------------------------------------------------
 * Function name : ifc1211_irq
 * Prototype     : int
 * Parameters    : inode -> pointer to device node data structure
 *                 filp  -> pointer to the file data structure
 * Return        : 0 if OK
 *----------------------------------------------------------------------------
 * Description   ifc1211_irq() is the low level interrupt handler for the
 *               ifc1211 interface.
 *
 *----------------------------------------------------------------------------*/

irqreturn_t ifc1211_irq( int irq, void *arg){
	struct ifc1211_device *ifc;
	register uint ip;
	register uint base;
	register uint src, idx;

	ifc = (struct ifc1211_device *)arg;
	debugk(( KERN_ALERT "ifc1211: entering ifc1211_irq( %x, %p)\n", irq, arg));

	/* generate IACK cycle */
	ip = ioread32(  ifc->csr_ptr + IFC1211_CSR_ILOC_ITC_IACK);

	/* get interrupt source */
	src  = ip & 0x7fff;
	idx  = src >> 8;
	base = (( ip >> 2) & 0xc00);
	ip   = 1 << ((ip>>8)&0xf);

	/* mask interrupt source */
	iowrite32( ip, ifc->csr_ptr + base + IFC1211_CSR_ILOC_ITC_IMS);

	/* increment interrupt counter */
	ifc->irq_tbl[idx].cnt += 1;

	/* activates tasklet handling interrupts */
	ifc->irq_tbl[idx].func( ifc, src, ifc->irq_tbl[idx].arg);

	/* clear IP and restart interrupt scanning */
	iowrite32( ip<<16, ifc->csr_ptr + base + IFC1211_CSR_ILOC_ITC_IACK);

	return( IRQ_HANDLED);
}

/*----------------------------------------------------------------------------
 * Function name : ifc1211_open
 * Prototype     : int
 * Parameters    : inode -> pointer to device node data structure
 *                 filp  -> pointer to the file data structure
 * Return        : 0 if OK
 *----------------------------------------------------------------------------
 * Description   ifc1211_open() opens the ifc1211 control device giving
 *               access to the device internal registers (through PCI BAR3).
 *
 *----------------------------------------------------------------------------*/

static int ifc1211_open( struct inode *inode, struct file *filp){
	struct ifc1211_device *ifc;
	int minor = -1;

	debugk(( KERN_ALERT "ifc1211: entering ifc1211_open( %p, %p)\n", inode, filp));

	minor = iminor( file_inode(filp)); // Get minor value
	debugk(( KERN_ALERT "ifc1211: minor value: %x)\n", minor));

	// IO device
	if (minor == 0) {
		ifc = ifc1211.ifc_io;
	}
	// CENTRAL device
	else if (minor == 1){
		ifc = ifc1211.ifc_central;
	}
	else {
		debugk(( KERN_ALERT "ifc1211: opening failed !\n"));
		return -1;
	}

	mutex_lock( &ifc->mutex_ctl);

	filp->private_data = (void *)ifc;

	mutex_unlock( &ifc->mutex_ctl);

	return( 0);
}

/*----------------------------------------------------------------------------
 * Function name : ifc1211_ioctl
 * Prototype     : int
 * Parameters    : inode -> pointer to device node data structure
 *                 filp  -> pointer to the file data structure
 *                 cmd   -> command code
 *                 arg   -> command argument
 * Return        : 0 if OK
 *----------------------------------------------------------------------------
 * Description   ifc1211_release() releasess the ifc1211 control device
 *
 *----------------------------------------------------------------------------*/

static long ifc1211_ioctl( struct file *filp, unsigned int cmd, unsigned long arg){
	struct ifc1211_device *ifc;
	int retval;
	int minor = -1;

	debugk(( KERN_ALERT "ifc1211: entering ifc1211_ioctl( %p, %x, %lx)\n", filp, cmd, arg));
	ifc = ( struct ifc1211_device *)filp->private_data;

	minor = iminor( file_inode(filp)); // Get minor value

	retval = 0;
	switch ( cmd &  TSC_IOCTL_OP_MASK){
    	case TSC_IOCTL_ID:{
    		if( cmd == TSC_IOCTL_ID_NAME){
    			if (minor == 0){
    				if( copy_to_user( (void *)arg, IFC1211_NAME_IO, strlen(IFC1211_NAME_IO))){
    					retval = -EFAULT;
    				}
    			}
    			else if (minor == 1){
    				if( copy_to_user( (void *)arg, IFC1211_NAME_CENTRAL, strlen(IFC1211_NAME_CENTRAL))){
    					retval = -EFAULT;
    				}
    			}
    		}
    		else if( cmd == TSC_IOCTL_ID_VERSION){
    			if( copy_to_user( (void *)arg, DRIVER_VERSION, strlen( DRIVER_VERSION))){
    				retval = -EFAULT;
    			}
    		}
    		else if( cmd == TSC_IOCTL_ID_VENDOR){
    			if( copy_to_user( (void *)arg, &ifc->pdev->vendor, sizeof( short))){
    				retval = -EFAULT;
    			}
    		}
    		else if( cmd == TSC_IOCTL_ID_DEVICE){
    			if( copy_to_user( (void *)arg, &ifc->pdev->device, sizeof( short))){
    				retval = -EFAULT;
    			}
    		}
    		else{
    			retval = -EINVAL;
    		}
    		break;
    	}
    	case TSC_IOCTL_CSR:{
    		retval = ioctl_csr( ifc, cmd, arg);
    		break;
    	}
    	case TSC_IOCTL_MAP:{
    		retval = ioctl_map( ifc, cmd, arg);
    		break;
    	}
    	case TSC_IOCTL_RDWR:{
    		retval = ioctl_rdwr( ifc, cmd, arg);
    		break;
    	}
    	case TSC_IOCTL_DMA:{
    		retval = ioctl_dma( ifc, cmd, arg);
    		break;
    	}
    	case TSC_IOCTL_KBUF:{
    		retval = ioctl_kbuf( ifc, cmd, arg);
    		break;
    	}
    	case TSC_IOCTL_SFLASH:{
    		retval = ioctl_sflash( ifc, cmd, arg);
    		break;
    	}
    	case TSC_IOCTL_TIMER:{
    		retval = ioctl_timer( ifc, cmd, arg);
    		break;
    	}
    	case TSC_IOCTL_FIFO:{
    		retval = ioctl_fifo( ifc, cmd, arg);
    		break;
    	}
    	case TSC_IOCTL_I2C:{
    		retval = ioctl_i2c( ifc, cmd, arg);
    		break;
    	}
    	case TSC_IOCTL_SEMAPHORE:{
    		retval = ioctl_semaphore( ifc, cmd, arg);
    		break;
    	}
    	default:{
    		retval = -EINVAL;
    	}
	}

	return( retval);
}

/*----------------------------------------------------------------------------
 * Function name : ifc1211_mmap
 * Prototype     : int
 * Parameters    : filp  -> pointer to the file data structure
 *                 vma  -> pointer to 
 * Return        : 0 if OK
 *----------------------------------------------------------------------------
 * Description   tsc__mmap() maps a memory window in user space
 *
 *----------------------------------------------------------------------------*/

static int ifc1211_mmap( struct file *filp, struct vm_area_struct *vma){
	int retval;
	ssize_t size;
	off_t off;

	debugk(( KERN_ALERT "ifc: entering ifc1211_mmap( %p, %p)\n", filp, vma));

	size   = vma->vm_end - vma->vm_start;
	off    = vma->vm_pgoff << PAGE_SHIFT;
	//printk( KERN_ALERT "ifc: entering ifc1211_mmap( %p, %p, %lx [%lx])\n", filp, vma,  off, size);
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
 * Function name : ifc1211_release
 * Prototype     : int
 * Parameters    : inode -> pointer to device node data structure
 *                 filp  -> pointer to the file data structure
 * Return        : 0 if OK
 *----------------------------------------------------------------------------
 * Description   ifc1211_release() releasess the ifc1211 control device
 *
 *----------------------------------------------------------------------------*/

static int ifc1211_release( struct inode *inode, struct file *filp){
	struct ifc1211_device *ifc;

	ifc = ( struct ifc1211_device *)filp->private_data;
	mutex_lock( &ifc->mutex_ctl);

	debugk(( KERN_ALERT "ifc1211: entering ifc1211_release( %p, %p)\n", inode, filp));
	filp->private_data = (void *)NULL;

	mutex_unlock( &ifc->mutex_ctl);

	return( 0);
}

/*----------------------------------------------------------------------------
 * File operations for ifc1211 device
 *----------------------------------------------------------------------------*/
struct file_operations ifc1211_fops = {
										.owner          = THIS_MODULE,
										.mmap           = ifc1211_mmap,
#ifdef JFG
										.llseek         = ifc1211_llseek,
										.read           = ifc1211_read,
										.write          = ifc1211_write,
#endif
										.open           = ifc1211_open,
										.unlocked_ioctl = ifc1211_ioctl,
										.release        = ifc1211_release,
};

/*----------------------------------------------------------------------------
 * Function name : ifc1211_probe
 * Prototype     : int
 * Parameters    : pdev -> pointer to pci device table entry
 *                 id   -> pointer to pci identifier
 * Return        : 0 if OK
 *----------------------------------------------------------------------------
 * Description   ifc1211_probe() is called for each PCI device found in the
 *               pci device table whose id/did matched the list declared in
 *               ifc1211_ids structure.
 *
 *----------------------------------------------------------------------------*/

static int ifc1211_probe( struct pci_dev *pdev, const struct pci_device_id *id){
	int retval;
	struct ifc1211_device *ifc = NULL;
	short tmp;

	retval = 0;
	debugk(( KERN_ALERT "ifc1211: entering ifc1211_probe( %p, %p)\n", pdev, id));

	/*--------------------------------------------------------------------------
	 * allocate device control structure
	 *--------------------------------------------------------------------------*/

	if (id->device == PCI_DEVICE_ID_IOXOS_IFC1211_IO){
		ifc1211.ifc_io = (struct ifc1211_device *)kzalloc(sizeof(struct ifc1211_device), GFP_KERNEL);
		if (ifc1211.ifc_io == NULL) {
			dev_err(&pdev->dev, "Failed to allocate memory for device structure\n");
			retval = -ENOMEM;
			goto ifc1211_probe_err_alloc_dev;
		}
		ifc = ifc1211.ifc_io;
		debugk((KERN_NOTICE "ifc1211_io : device data structure allocated %p\n", ifc));
	}
	else if (id->device == PCI_DEVICE_ID_IOXOS_IFC1211_CENTRAL){
		ifc1211.ifc_central = (struct ifc1211_device *)kzalloc(sizeof(struct ifc1211_device), GFP_KERNEL);
		if (ifc1211.ifc_central == NULL) {
			dev_err(&pdev->dev, "Failed to allocate memory for device structure\n");
			retval = -ENOMEM;
			goto ifc1211_probe_err_alloc_dev;
		}
		ifc = ifc1211.ifc_central;
		debugk((KERN_NOTICE "ifc1211_central : device data structure allocated %p\n", ifc));
	}

	/* Enable the device */
	retval = pci_enable_device(pdev);
	if (retval) {
		dev_err(&pdev->dev, "Unable to enable ifc1211 device\n");
		goto ifc1211_probe_err_enable;
	}
	ifc->pdev = pdev;
	debugk((KERN_NOTICE "ifc1211 : PCI device enabled\n"));

	/* Map Registers */
	if (id->device == PCI_DEVICE_ID_IOXOS_IFC1211_IO){
		retval = pci_request_regions( pdev, device_name_io);
		if (retval) {
			dev_err(&pdev->dev, "Unable to reserve resources\n");
			goto ifc1211_probe_err_resource;
		}
		debugk((KERN_NOTICE "ifc1211_io : PCI region allocated\n"));
	}
	else if (id->device == PCI_DEVICE_ID_IOXOS_IFC1211_CENTRAL){
		retval = pci_request_regions( pdev, device_name_central);
		if (retval) {
			dev_err(&pdev->dev, "Unable to reserve resources\n");
			goto ifc1211_probe_err_resource;
		}
		debugk((KERN_NOTICE "ifc1211_central : PCI region allocated\n"));
	}

	/* map CSR registers through BAR 3 (size 8k) */
	debugk((KERN_NOTICE "ifc1211 : pci resource start BAR3 : %lx\n", pci_resource_start(pdev, 3)));
	ifc->csr_ptr = ioremap_nocache( pci_resource_start(pdev, 3), 2*4096);
	if( !ifc->csr_ptr) {
		dev_err(&pdev->dev, "Unable to remap CSR region\n");
		retval = -EIO;
		goto ifc1211_probe_err_remap;
	}
	debugk((KERN_NOTICE "ifc1211 : CSR registers mapped -> %p : %08x\n",
			ifc->csr_ptr, ioread32( ifc->csr_ptr + IFC1211_CSR_ILOC_OPT_DYN_DAT)));

	/* map PON registers through IFC bus (size 4k) */
	debugk((KERN_NOTICE "ifc1211 : map PON register thorough IFC bus : %lx\n", 0xfffd00000));
	ifc->pon_ptr = ioremap_nocache( 0xfffd00000, 0x1000);
	if( !ifc->pon_ptr) {
		dev_err(&pdev->dev, "Unable to remap PON registers\n");
	}
	debugk((KERN_NOTICE "ifc1211 : PON registers mapped -> %p : %08x\n",
			ifc->pon_ptr, ioread32be( ifc->pon_ptr)));

	/* use MSI interrupt mechanism if possible */
	if( pci_enable_msi( pdev)){
		debugk((KERN_NOTICE "ifc1211 : Cannot enable MSI\n"));
		goto ifc1211_probe_err_enable_msi;
	}
	debugk((KERN_NOTICE "ifc1211 : MSI enabled : ifc irq = %d\n", pdev->irq));

	/* register interrupt service routine tsc_irq */
	if (id->device == PCI_DEVICE_ID_IOXOS_IFC1211_IO){
		if( request_irq( pdev->irq, ifc1211_irq, IRQF_SHARED, device_name_io, ifc)){
			debugk((KERN_NOTICE "ifc1211_io : Cannot register IRQ\n"));
			goto ifc1211_probe_err_request_irq;
		}
		debugk((KERN_NOTICE "ifc1211_io : IRQ registered\n"));
	}
	else if (id->device == PCI_DEVICE_ID_IOXOS_IFC1211_CENTRAL){
		if( request_irq( pdev->irq, ifc1211_irq, IRQF_SHARED, device_name_central, ifc)){
			debugk((KERN_NOTICE "ifc1211_central : Cannot register IRQ\n"));
			goto ifc1211_probe_err_request_irq;
		}
		debugk((KERN_NOTICE "ifc1211_central : IRQ registered\n"));
	}

	/* create protection mutex for control device */
	mutex_init( &ifc->mutex_ctl);

	/* call ifc1211 initialization function */
	retval = tsc_dev_init( ifc);

	if( retval < 0){
		tsc_dev_exit( ifc);
		goto ifc1211_probe_err_request_irq;
	}

	/* enable interrupts and PCIe master access from FPGA */
	pci_read_config_word( ifc->pdev, PCI_COMMAND, &tmp);
	tmp |= PCI_COMMAND_MASTER;
	tmp &= ~PCI_COMMAND_INTX_DISABLE;
	debugk(( KERN_NOTICE "ifc1211 : enable interrupts and PCIe master access [%04x]\n", tmp));
	pci_write_config_word( ifc->pdev, PCI_COMMAND, tmp);

	return( retval);

ifc1211_probe_err_request_irq:
 	 pci_disable_msi( ifc->pdev);
ifc1211_probe_err_enable_msi:
	iounmap( ifc->csr_ptr);
ifc1211_probe_err_remap:
  	pci_release_regions( pdev);
ifc1211_probe_err_resource:
  	pci_disable_device( pdev);
ifc1211_probe_err_enable:
  	if (id->device == PCI_DEVICE_ID_IOXOS_IFC1211_IO){
  		kfree( ifc1211.ifc_io);
  	}
  	else if (id->device == PCI_DEVICE_ID_IOXOS_IFC1211_CENTRAL){
  		kfree( ifc1211.ifc_central);
  	}
ifc1211_probe_err_alloc_dev:
  	return( retval);
}

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * Function name : ifc1211_remove
 * Prototype     : void
 * Parameters    : pointer to pci_dev data structure
 * Return        : void
 *----------------------------------------------------------------------------
 * Description   : return devices resources to OS
 *
 *++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

static void ifc1211_remove( struct pci_dev *pdev){
	struct ifc1211_device *ifc;

	debugk(( KERN_ALERT "ifc1211: entering ifc1211_remove(%p) %x\n", pdev, pdev->device));

	// IO is present
	if ((ifc1211.ifc_io != NULL) && (pdev->device == PCI_DEVICE_ID_IOXOS_IFC1211_IO)){
		ifc = ifc1211.ifc_io;
		tsc_dev_exit(ifc);
		mutex_destroy( &ifc->mutex_ctl);
		free_irq( ifc->pdev->irq, (void *)ifc);
		pci_disable_msi( ifc->pdev);
		iounmap( ifc->csr_ptr);
		kfree(ifc);
		ifc1211.ifc_io = NULL;
	}

	// CENTRAL is present
	if ((ifc1211.ifc_central != NULL) && (pdev->device == PCI_DEVICE_ID_IOXOS_IFC1211_CENTRAL)){
		ifc = ifc1211.ifc_central;
		tsc_dev_exit(ifc);
		mutex_destroy( &ifc->mutex_ctl);
		free_irq( ifc->pdev->irq, (void *)ifc);
		pci_disable_msi(ifc->pdev);
		iounmap( ifc->csr_ptr);
		kfree(ifc);
		ifc1211.ifc_central = NULL;
	}

	pci_release_regions(pdev);
    pci_disable_device(pdev);

	return;
}

/*----------------------------------------------------------------------------
 * Function name : ifc1211_init
 * Prototype     : int
 * Parameters    : none
 * Return        : 0 if OK
 *----------------------------------------------------------------------------
 * Description
 *
 * Function executed  when the driver is installed
 * Allocate major device number
 * Register driver operation
 * 
 *----------------------------------------------------------------------------*/

static int ifc1211_init(void){
	int retval;
	dev_t ifc1211_dev_id;
	struct ifc1211_device *ifc_io = NULL;
	struct ifc1211_device *ifc_central = NULL;
	int major = -1;
	char name_io[32] = "bus/bridge/tsc_ctl_io";
	char name_central[32] = "bus/bridge/tsc_ctl_central";

	debugk(( KERN_ALERT "ifc1211: entering ifc1211_init( void)\n"));

	/*--------------------------------------------------------------------------
	 * device number dynamic allocation
	 *--------------------------------------------------------------------------*/
	retval = alloc_chrdev_region( &ifc1211_dev_id, IFC1211_MINOR_START, IFC1211_COUNT, IFC1211_NAME);
	if( retval < 0) {
		debugk(( KERN_WARNING "ifc1211: Error %d cannot allocate device number\n", retval));
		goto ifc1211_init_err_alloc_chrdev;
	}
	else {
		debugk((KERN_WARNING "ifc1211: registered with major number:%i\n", MAJOR( ifc1211_dev_id)));
	}
	ifc1211.dev_id = ifc1211_dev_id;

	/*--------------------------------------------------------------------------
	 * register driver
	 *--------------------------------------------------------------------------*/
	cdev_init( &ifc1211.cdev, &ifc1211_fops);
	ifc1211.cdev.owner = THIS_MODULE;
	ifc1211.cdev.ops = &ifc1211_fops;
	retval = cdev_add( &ifc1211.cdev, ifc1211.dev_id ,IFC1211_COUNT);
	if(retval) {
		debugk((KERN_NOTICE "ifc1211 : Error %d adding device\n", retval));
		goto ifc1211_init_err_cdev_add;
	}
	major = MAJOR(ifc1211_dev_id);
	debugk((KERN_NOTICE "ifc1211: device added\n"));

	/*--------------------------------------------------------------------------
	 * Register as PCI driver
	 *--------------------------------------------------------------------------*/

	// IO --------------

	ifc1211.ifc_io = NULL;
	retval = pci_register_driver( &ifc1211_driver_io);
	if(retval) {
		debugk((KERN_NOTICE "ifc1211_io : Error %d registering driver\n", retval));
		//goto ifc1211_init_err_pci_register_driver;
	}

	/* verify if ifc1211 device has been discovered */
	if( !ifc1211.ifc_io){
		device_destroy(bridge_sysfs_class_io, MKDEV(MAJOR(ifc1211.dev_id), 0));
		pci_unregister_driver( &ifc1211_driver_io);
		debugk((KERN_NOTICE "ifc1211_io : didn't find ifc1211_IO PCI device\n"));
	}
	else {
		ifc_io = ifc1211.ifc_io;
		debugk((KERN_NOTICE "ifc1211_io : driver registered [%p]\n", ifc_io));
	}

	// CENTRAL --------------

	ifc1211.ifc_central = NULL;
	retval = pci_register_driver( &ifc1211_driver_central);
	if(retval){
		debugk((KERN_NOTICE "ifc1211_central : Error %d registering driver\n", retval));
	}

	/* verify if ifc1211 device has been discovered */
	if( !ifc1211.ifc_central){
		device_destroy(bridge_sysfs_class_central, MKDEV(MAJOR(ifc1211.dev_id), 1));
		pci_unregister_driver( &ifc1211_driver_central);
		debugk((KERN_NOTICE "ifc1211_central : didn't find ifc1211_CENTRAL PCI device\n"));
	}
	else {
		ifc_central = ifc1211.ifc_central;
		debugk((KERN_NOTICE "ifc1211_central : driver registered [%p]\n", ifc_central));
	}

	/*--------------------------------------------------------------------------
	 * Check if at least one FPGA has been found
	 *--------------------------------------------------------------------------*/

	if((ifc1211.ifc_io == NULL) && (ifc1211.ifc_central == NULL)){
		goto ifc1211_no_device;
	}
	/*--------------------------------------------------------------------------
	 * Create sysfs entries - on udev systems this creates the dev files
	 *--------------------------------------------------------------------------*/

	if (ifc1211.ifc_io != NULL) {
		bridge_sysfs_class_io = class_create( THIS_MODULE, device_name_io);
		if (IS_ERR( bridge_sysfs_class_io)){
			retval = PTR_ERR(bridge_sysfs_class_io);
			ifc1211_remove( ifc_io->pdev);
		}
		else {
			device_create(bridge_sysfs_class_io, NULL, MKDEV(major, 0), NULL, name_io, 0);
		}
	}

	if (ifc1211.ifc_central != NULL) {
		bridge_sysfs_class_central = class_create( THIS_MODULE, device_name_central);
		if (IS_ERR( bridge_sysfs_class_central)){
			retval = PTR_ERR(bridge_sysfs_class_central);
			ifc1211_remove(ifc_central->pdev);
		}
		else {
			device_create(bridge_sysfs_class_central, NULL, MKDEV(major, 1), NULL, name_central, 1);
		}
	}

	if((ifc1211.ifc_io == NULL) && (ifc1211.ifc_central == NULL)){
		goto ifc1211_no_device;
	}

	return( 0);

	/*--------------------------------------------------------------------------
	 * Cleanup after an error has been detected
   *--------------------------------------------------------------------------*/
ifc1211_no_device:
  	cdev_del( &ifc1211.cdev);
ifc1211_init_err_cdev_add:
  	unregister_chrdev_region(ifc1211.dev_id, IFC1211_COUNT);
ifc1211_init_err_alloc_chrdev:
	return( retval);
}

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * Function name : ifc1211_exit
 * Prototype     : void
 * Parameters    : none
 * Return        : none
 *----------------------------------------------------------------------------
 * Description   : Function called when ifcmas driver is removed
 *                 Free all allocated resources
 *
 *++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

static void ifc1211_exit(void){
	debugk(( KERN_ALERT "ifc1211: entering ifc1211_exit( void)\n"));

	// IO
	if (ifc1211.ifc_io != NULL) {
		device_destroy(bridge_sysfs_class_io, MKDEV(MAJOR(ifc1211.dev_id), 0));
	    pci_unregister_driver( &ifc1211_driver_io);
		class_destroy(bridge_sysfs_class_io);
	}

	// CENTRAL
	if (ifc1211.ifc_central != NULL) {
		device_destroy(bridge_sysfs_class_central, MKDEV(MAJOR(ifc1211.dev_id), 1));
		pci_unregister_driver( &ifc1211_driver_central);
		class_destroy(bridge_sysfs_class_central);
	}

	cdev_del(&ifc1211.cdev);
	unregister_chrdev_region( ifc1211.dev_id, IFC1211_COUNT);
}

module_init( ifc1211_init);
module_exit( ifc1211_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("IOxOS Technologies [JFG]");
MODULE_VERSION(DRIVER_VERSION);
MODULE_DESCRIPTION("driver for IOxOS Technologies IFC1211 control interface");

/*================================< end file >================================*/
