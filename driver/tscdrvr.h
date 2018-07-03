/*=========================< begin file & file header >=======================
 *  References
 *  
 *    filename : tscdrvr.h
 *    author   : JFG, XP
 *    company  : IOxOS
 *    creation : june 30,2008
 *
 *----------------------------------------------------------------------------
 *  Description
 *
 *    This file contain the declarations neede by the tsc device driver
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


#ifndef _TSCDRVR
#define _TSCDRVR

#include "../include/tscioctl.h"

#define TSC_SHM_NUM  2                       /* number of SHM agents */
#define TSC_SHM1_IDX 0                       /* index of SHM1 agent  */
#define TSC_SHM2_IDX 1                       /* index of SHM2 agent  */

struct shm_ctl
{
  void __iomem *sram_ptr;                    /* Base Address to access SMEM RAM     */
  int sram_size;                             /* Size of SMEM RAM                    */
  int sram_offset;                           /* address offset allocated by mapper  */
};

struct tsc_device
{
  struct pci_dev *pdev;
  struct device *dev_ctl;                                    /* tsc control device */
  struct mutex mutex_ctl;                                    /* Mutex for locking control device */
  void __iomem *csr_ptr;                                     /* Base Address of device registers */
  void __iomem *pon_ptr;                                     /* Base Address of PON registers */
  struct tsc_irq_handler *irq_tbl[TSC_AGENT_SW_NUM];         /* Interrupt handler table, max [8][16] */
  struct mutex csr_lock;                                     /* Mutex for locking control device */
  struct map_ctl *map_mas_pci_pmem;                          /* master map PCI_PMEM BAR0/1 */
  struct map_ctl *map_mas_pci_mem;                           /* master map PCI_MEM BAR2 */
  struct shm_ctl *shm_ctl[TSC_SHM_NUM];                      /* control structure for SHM */
  struct rdwr_ctl *rdwr_ctl;                                 /* control structure for RDWR access */
  struct sflash_ctl *sflash_ctl;                             /* control structure for SPI FLASH */
  struct dma_ctl *dma_ctl[DMA_CHAN_NUM];                     /* control structure for DMA controller */
  struct i2c_ctl *i2c_ctl;                                   /* control structure for I2C access */
  struct map_ctl *map_slv_pci1_pmem;                         /* slave map PCI1_PMEM BAR0/1 */
  struct map_ctl *map_slv_pci1_mem;                          /* slave map PCI1_MEM BAR2 */
};

struct tsc_irq_handler
{
  void (* func)( struct tsc_device *, int, void *); /* pointer to interrupt handler                  */
  void *arg;                                            /* pointer to be passed when handler is executed */
  int cnt;                                              /* interrupt counter                             */
  int busy;                                             /* busy flag                                     */
};

struct tsc
{
  struct cdev cdev;
  dev_t dev_id;
  struct tsc_device *ifc_central;
  int nr_devs;
};

#define TSC_COUNT                   64       /* Maximum number of TSC devices    */
#define TSC_NAME           "tsc"         /* Name of the tsc device           */
#define TSC_NAME_CENTRAL   "tsc_central" /* Name of the tsc central device   */
#define TSC_MINOR_START              0       /* First minor number                   */
#define TSC_IRQ_NUM                 64       /* Number of interrupt source (4*16)    */

#include "../include/tsc.h"
#include "ioctllib.h"
#include "tscklib.h"

#endif /*  _H_TSCDRVR */

