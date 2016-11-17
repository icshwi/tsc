/*=========================< begin file & file header >=======================
 *  References
 *  
 *    filename : tscdrvr.h
 *    author   : JFG
 *    company  : IOxOS
 *    creation : june 30,2008
 *    version  : 0.0.1
 *
 *----------------------------------------------------------------------------
 *  Description
 *
 *    This file contain the declarations neede by the ifc1211 device driver
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


#ifndef _TSCDRVR
#define _TSCDRVR

#include "../include/tscioctl.h"

struct shm_ctl
{
  void __iomem *sram_ptr;                    /* Base Address to access SMEM RAM     */
  int sram_size;                             /* Size of SMEM RAM                    */
  int sram_offset;                           /* address offset allocated by mapper  */
};

struct ifc1211_device
{
  struct pci_dev *pdev;
  struct device *dev_ctl;                    /* ifc1211 control device               */
  struct mutex mutex_ctl;                    /* Mutex for locking control device     */
  void __iomem *csr_ptr;                     /* Base Address of device registers     */
  void __iomem *pon_ptr;                     /* Base Address of PON registers        */
  struct ifc1211_irq_handler *irq_tbl;       /* Pointer to interrupt handler table   */
  struct mutex csr_lock;                     /* Mutex for locking control device     */
  struct map_ctl *map_mas_pci_pmem;          /* master map PCI_PMEM BAR0/1           */
  struct map_ctl *map_mas_pci_mem;           /* master map PCI_PMEM BAR2             */
  struct shm_ctl *shm_ctl;                   /* control structure for SHM            */
  struct rdwr_ctl *rdwr_ctl;                 /* control structure for RDWR access    */
  struct sflash_ctl *sflash_ctl;             /* control structure for SPI FLASH      */
  struct dma_ctl *dma_ctl[DMA_CHAN_NUM];     /* control structure for DMA controller */
  struct i2c_ctl *i2c_ctl;                   /* control structure for I2C access     */
};

struct ifc1211_irq_handler
{
  void (* func)( struct ifc1211_device *, int, void *); /* pointer to interrupt handler                  */
  void *arg;                                            /* pointer to be passed when handler is executed */
  int cnt;                                              /* interrupt counter                             */
  int busy;                                             /* busy flag                                     */
};

struct ifc1211
{
  struct cdev cdev;
  dev_t dev_id;
  struct ifc1211_device *ifc_io;
  struct ifc1211_device *ifc_central;
};

#define IFC1211_COUNT                   64       /* Maximum number of IFC1211 devices    */
#define IFC1211_NAME           "ifc1211"         /* Name of the ifc1211 device           */
#define IFC1211_NAME_IO        "ifc1211_io"      /* Name of the ifc1211 io device        */
#define IFC1211_NAME_CENTRAL   "ifc1211_central" /* Name of the ifc1211 central device   */
#define IFC1211_MINOR_START              0       /* First minor number                   */
#define IFC1211_IRQ_NUM                 64       /* Number of interrupt source (4*16)    */

#include "../include/ifc1211.h"
#include "ioctllib.h"
#include "tscklib.h"

#endif /*  _H_TSCDRVR */

/*================================< end file >================================*/
