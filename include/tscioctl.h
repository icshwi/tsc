/*=========================< begin file & file header >=======================
 *  References
 *  
 *    filename : tscioctl.h
 *    author   : JFG, XP
 *    company  : IOxOS
 *    creation : Sept 30,2015
 *
 *----------------------------------------------------------------------------
 *  Description
 *
 *    This file contains the declarations for ioctl commands supported by the
 *    TSC control driver
 *
 *----------------------------------------------------------------------------
 *
 *  Copyright Notice
 *  
 *    Copyright and all other rights in this document are reserved by 
 *    IOxOS Technologies SA. This documents contains proprietary information    
 *    and is supplied on express condition that it may not be disclosed, 
 *    reproduced in whole or in part, or used for any other purpose other
 *    than that for which it is supplies, without the written consent of  
 *    IOxOS Technologies SA                                                        
 *  
 *=============================< end file header >============================*/

#ifndef _H_TSCIOCTL
#define _H_TSCIOCTL

#include "tsc.h"

#ifndef _LINUX_TYPES_H
#include <stdint.h>
#endif /* _LINUX_TYPES_H */

#define TSC_BOARD_IFC1211        0x73571211
#define TSC_BOARD_IFC1410        0x73571410

#define TSC_BOARD_TSC_IO         PCI_DEVICE_ID_IOXOS_TSC_IO
#define TSC_BOARD_TSC_CENTRAL_1  PCI_DEVICE_ID_IOXOS_TSC_CENTRAL_1
#define TSC_BOARD_TSC_CENTRAL_2  PCI_DEVICE_ID_IOXOS_TSC_CENTRAL_2
#define TSC_BOARD_TSC_CENTRAL_3  PCI_DEVICE_ID_IOXOS_TSC_CENTRAL_3

#define TSC_IOCTL_OP_MASK        0xffff0000

#define TSC_IOCTL_ID             0x00000000
#define TSC_IOCTL_ID_NAME        (TSC_IOCTL_ID | 0x01)
#define TSC_IOCTL_ID_VERSION     (TSC_IOCTL_ID | 0x05)
#define TSC_IOCTL_ID_VENDOR      (TSC_IOCTL_ID | 0x03)
#define TSC_IOCTL_ID_DEVICE      (TSC_IOCTL_ID | 0x04)

#define TSC_IOCTL_CSR             0x00010000
#define TSC_IOCTL_CSR_RD          (TSC_IOCTL_CSR | 0x01)
#define TSC_IOCTL_CSR_WR          (TSC_IOCTL_CSR | 0x02)
#define TSC_IOCTL_CSR_OR          (TSC_IOCTL_CSR | 0x03)
#define TSC_IOCTL_CSR_XOR         (TSC_IOCTL_CSR | 0x04)
#define TSC_IOCTL_CSR_AND         (TSC_IOCTL_CSR | 0x05)
#define TSC_IOCTL_CSR_RDm         (TSC_IOCTL_CSR | 0x11)
#define TSC_IOCTL_CSR_WRm         (TSC_IOCTL_CSR | 0x12)
#define TSC_IOCTL_CSR_ORm         (TSC_IOCTL_CSR | 0x13)
#define TSC_IOCTL_CSR_XORm        (TSC_IOCTL_CSR | 0x14)
#define TSC_IOCTL_CSR_ANDm        (TSC_IOCTL_CSR | 0x15)
#define TSC_IOCTL_CSR_SMON        (TSC_IOCTL_CSR | 0x100)
#define TSC_IOCTL_CSR_SMON_RD     (TSC_IOCTL_CSR | 0x101)
#define TSC_IOCTL_CSR_SMON_WR     (TSC_IOCTL_CSR | 0x102)
#define TSC_IOCTL_CSR_PCIEP       (TSC_IOCTL_CSR | 0x200)
#define TSC_IOCTL_CSR_PCIEP_RD    (TSC_IOCTL_CSR | 0x201)
#define TSC_IOCTL_CSR_PCIEP_WR    (TSC_IOCTL_CSR | 0x202)
#define TSC_IOCTL_CSR_PON         (TSC_IOCTL_CSR | 0x300)
#define TSC_IOCTL_CSR_PON_RD      (TSC_IOCTL_CSR | 0x301)
#define TSC_IOCTL_CSR_PON_WR      (TSC_IOCTL_CSR | 0x302)

struct tsc_ioctl_csr_op
{
  int operation;
  int offset;
  int data;
  int mask;
};
  
#define TSC_IOCTL_MAP                 0x00020000
#define TSC_IOCTL_MAP_READ            (TSC_IOCTL_MAP | 0x1000)
#define TSC_IOCTL_MAP_MAS             (TSC_IOCTL_MAP | 0x2000)
#define TSC_IOCTL_MAP_SLV             (TSC_IOCTL_MAP | 0x3000)
#define TSC_IOCTL_MAP_CLEAR           (TSC_IOCTL_MAP | 0x4000)
#define TSC_IOCTL_MAP_OP_MSK          (TSC_IOCTL_MAP | 0xf000)
#define TSC_IOCTL_MAP_MAS_ALLOC       (TSC_IOCTL_MAP_MAS | 0x1)
#define TSC_IOCTL_MAP_MAS_FREE        (TSC_IOCTL_MAP_MAS | 0x2)
#define TSC_IOCTL_MAP_MAS_MODIFY      (TSC_IOCTL_MAP_MAS | 0x3)
#define TSC_IOCTL_MAP_MAS_GET         (TSC_IOCTL_MAP_MAS | 0x4)
#define TSC_IOCTL_MAP_SLV_ALLOC       (TSC_IOCTL_MAP_SLV | 0x1)
#define TSC_IOCTL_MAP_SLV_FREE        (TSC_IOCTL_MAP_SLV | 0x2)
#define TSC_IOCTL_MAP_SLV_MODIFY      (TSC_IOCTL_MAP_SLV | 0x3)
#define TSC_IOCTL_MAP_SLV_GET         (TSC_IOCTL_MAP_SLV | 0x4)

#define MAP_ID_INVALID         0
#define MAP_ID_MAS_PCIE_MEM    1
#define MAP_ID_MAS_PCIE_PMEM   2

#define MAP_SPACE_INVALID         -1
#define MAP_SPACE_PCIE             0  
#define MAP_SPACE_PCIE1            0  
#define MAP_SPACE_PCIE2            1  
#define MAP_SPACE_SHM              2  
#define MAP_SPACE_SHM1             2  
#define MAP_SPACE_SHM2             3  
#define MAP_SPACE_USR              3  
#define MAP_SPACE_USR1             4  
#define MAP_SPACE_USR2             5  

#define MAP_FLAG_FREE           0
#define MAP_FLAG_BUSY           1
#define MAP_FLAG_PRIVATE        2
#define MAP_FLAG_FORCE          4
#define MAP_FLAG_LOCKED         8

#define MAP_REM_ADDR_AUTO              -1  
#define MAP_LOC_ADDR_AUTO              -1  
#define MAP_IDX_INV                    -1  

#define MAP_SWAP_NO        0x0
#define MAP_SWAP_AUTO      0x1
#define MAP_SWAP_DW        0x2
#define MAP_SWAP_QW        0x3
#define MAP_SWAP_MASK      0x3
#define MAP_SPLIT_D32      0x4

struct tsc_ioctl_map_ctl
{
  struct tsc_map_blk
  {
    char flag; char usr; short npg;
    uint mode;
    uint64_t rem_addr;
    uint tgid;           /* task id to which the block is allocated     */
  } *map_p;
  char rsv; char sg_id; short pg_num;
  int pg_size;
  uint64_t win_base;
  uint64_t win_size;
};

struct tsc_ioctl_map_mode
{
  char sg_id; char flags; short hw;
  char ads; char space; char swap; char am;
};

struct tsc_ioctl_map_req
{
  uint64_t rem_addr;
  uint64_t loc_addr;
  uint size;
  struct tsc_ioctl_map_mode mode;
};

struct tsc_ioctl_map_sts
{
  uint64_t rem_base;
  uint64_t loc_base;
  uint size;
  struct tsc_ioctl_map_mode mode;
};

struct tsc_ioctl_map_win
{
  int pg_idx;
  union
  {
    struct tsc_ioctl_map_req req;
    struct tsc_ioctl_map_sts sts;
  };
};



#define TSC_IOCTL_ITC            0x00050000
#define TSC_IOCTL_ITC_MSK_SET    (TSC_IOCTL_ITC | 0x1)
#define TSC_IOCTL_ITC_MSK_CLEAR  (TSC_IOCTL_ITC | 0x2)

/* TSC interrupt sources */
#define ITC_SRC_ILOC_ALM0         (TSC_IRQ_CTL_ILOC)
#define ITC_SRC_I2C_CRIT          (TSC_IRQ_CTL_ILOC | TSC_IRQ_SRC_I2C_CRIT)
#define ITC_SRC_I2C_POWER         (TSC_IRQ_CTL_ILOC | TSC_IRQ_SRC_I2C_POWER)
#define ITC_SRC_I2C_OK            (TSC_IRQ_CTL_ILOC | TSC_IRQ_SRC_I2C_OK)
#define ITC_SRC_I2C_ERR           (TSC_IRQ_CTL_ILOC | TSC_IRQ_SRC_I2C_ERR)
#define ITC_SRC_DMA_RD0_END       0x20
#define ITC_SRC_DMA_RD0_ERR       0x21
#define ITC_SRC_DMA_RD1_END       0x22
#define ITC_SRC_DMA_RD1_ERR       0x23
#define ITC_SRC_DMA_WR0_END       0x24
#define ITC_SRC_DMA_WR0_ERR       0x25
#define ITC_SRC_DMA_WR1_END       0x26
#define ITC_SRC_DMA_WR1_ERR       0x27
#define ITC_SRC_DMA2_RD0_END      0x30
#define ITC_SRC_DMA2_RD0_ERR      0x31
#define ITC_SRC_DMA2_RD1_END      0x32
#define ITC_SRC_DMA2_RD1_ERR      0x33
#define ITC_SRC_DMA2_WR0_END      0x34
#define ITC_SRC_DMA2_WR0_ERR      0x35
#define ITC_SRC_DMA2_WR1_END      0x36
#define ITC_SRC_DMA2_WR1_ERR      0x37

#define ITC_IP(src)      (1<<(src&0xf)) /* interrupt pending bitfield from source id     */

#define ITC_CTL_ILOC                  0
#define ITC_CTL_DMA                   2
#define ITC_CTL_USR                   3
#define ITC_CTL(src)       ((src>>4)&7) /* interrupt control id from interrupt source id */

#define ITC_IACK_VEC(iack)              (iack&0xff)  /* extract vector from iack register        */
#define ITC_IACK_SRC(iack)         ((iack>>8)&0x7f)  /* extract source id from iack register     */
#define ITC_IACK_CTL(iack)        ((iack>>12)&0x7)   /* extract controller id from iack register */

#define IRQ_WAIT_INTR     0x01
#define IRQ_WAIT_1MS      0x02
#define IRQ_WAIT_10MS     0x04
#define IRQ_WAIT_100MS    0x06
#define IRQ_WAIT_1S       0x08
#define IRQ_WAIT_10S      0x0a
#define IRQ_WAIT_100S     0x0c

#define TSC_IOCTL_RDWR             0x00060000
#define TSC_IOCTL_RDWR_READ        (TSC_IOCTL_RDWR | 0x01)
#define TSC_IOCTL_RDWR_WRITE       (TSC_IOCTL_RDWR | 0x02)

#define RDWR_SPACE_PCIE   0
#define RDWR_SPACE_PCIE1  0
#define RDWR_SPACE_PCIE2  1
#define RDWR_SPACE_SHM    2
#define RDWR_SPACE_SHM1   2
#define RDWR_SPACE_SHM2   3
#define RDWR_SPACE_USR    3
#define RDWR_SPACE_USR1   4
#define RDWR_SPACE_USR2   5
#define RDWR_SPACE_KBUF   8
#define RDWR_SPACE_MASK 0xf

#define RDWR_SWAP_NO      0
#define RDWR_SWAP_AUTO    1
#define RDWR_SWAP_DS      2
#define RDWR_SWAP_QS      3

#define RDWR_SIZE_BYTE  1
#define RDWR_SIZE_SHORT 2
#define RDWR_SIZE_INT   4
#define RDWR_SIZE_DBL   8

#define RDWR_MODE_SET_DS( m, ds)   ((m & 0xf0) | (ds & 0xf))
#define RDWR_MODE_GET_DS( m)       (m & 0xf) 
#define RDWR_MODE_SET_AS( m, as)   ((m & 0xf) | ((as<<4) & 0xf0))
#define RDWR_MODE_GET_AS( m)       ((m & 0xf0) >> 4) 
#define RDWR_MODE_SET_SWAP( m, sw) ((m & 0xf) | ((sw<<4) & 0xf0))
#define RDWR_MODE_GET_SWAP( m)     ((m & 0xf0) >> 4) 

#define RDWR_MODE_A8    1
#define RDWR_MODE_A16   2
#define RDWR_MODE_A24   3
#define RDWR_MODE_A32   4
#define RDWR_MODE_A64   8
#define RDWR_MODE_AS(x)   (x<<4)

#define RDWR_MODE_SET( ads, space, am)   (((ads&0xff)<<24) | ((space&0xff)<<16) | (am&0xffff))

#define RDWR_SWAP_DATA         0x80
#define RDWR_LOOP        0x80000000

struct tsc_ioctl_rdwr
{
  uint64_t rem_addr;
  char *buf;
  int len;
  union
  {
    uint mode;
    struct rdwr_mode {char ads; char space; char swap; char am;}m;
  }; 
};

#define TSC_IOCTL_DMA                0x00080000
#define TSC_IOCTL_DMA_MOVE         (TSC_IOCTL_DMA | 0x1)
#define TSC_IOCTL_DMA_STATUS       (TSC_IOCTL_DMA | 0x2)
#define TSC_IOCTL_DMA_WAIT         (TSC_IOCTL_DMA | 0x3)
#define TSC_IOCTL_DMA_CLEAR        (TSC_IOCTL_DMA | 0x4)
#define TSC_IOCTL_DMA_ALLOC        (TSC_IOCTL_DMA | 0x5)
#define TSC_IOCTL_DMA_FREE         (TSC_IOCTL_DMA | 0x6)
#define TSC_IOCTL_DMA_MODE         (TSC_IOCTL_DMA | 0x7)
#define TSC_IOCTL_DMA_TRANSFER     (TSC_IOCTL_DMA | 0x8)

#define DMA_CHAN_NUM    4        /* number of DMA channels         */
#define DMA_CHAN_0      0        /* DMA channel #0                 */
#define DMA_CHAN_1      1        /* DMA channel #1                 */
#define DMA_CHAN_2      2        /* DMA channel #2                 */
#define DMA_CHAN_3      3        /* DMA channel #3                 */

struct tsc_ioctl_dma_req
{
  uint64_t src_addr;
  uint64_t des_addr;
  uint size;
  unsigned char src_space; unsigned char src_mode; unsigned char des_space; unsigned char des_mode;
  unsigned char start_mode; unsigned char end_mode; unsigned char intr_mode; unsigned char wait_mode;
  uint dma_status;
};

struct tsc_ioctl_dma_mode
{
  short rsv; char op; char chan;
  short mode;
};

struct tsc_ioctl_dma
{
  short status; char state; char chan;
};

struct tsc_ioctl_dma_sts
{
  struct tsc_ioctl_dma dma;
  uint rd_csr;
  uint rd_ndes;
  uint rd_cdes;
  uint rd_cnt;
  uint wr_csr;
  uint wr_ndes;
  uint wr_cdes;
  uint wr_cnt;
};

#define DMA_MODE_CACHE_ENA            0x8000
#define DMA_MODE_WR_POST_MASK         0x3000  
#define DMA_MODE_WR_POST(x)      ((x<<12)&DMA_MODE_WR_POST_MASK)
#define DMA_MODE_RD_REQ_MASK          0x3000
#define DMA_MODE_RD_REQ(x)       ((x<<12)&DMA_MODE_RD_REQ_MASK)
#define DMA_MODE_SHM_NO_INC            0x200
#define DMA_MODE_SHM_NO_UPD            0x100
#define DMA_MODE_SHM_MASK              0x300
#define DMA_MODE_RELAX                  0x20
#define DMA_MODE_SNOOP                  0x10
#define DMA_MODE_ADD_NO_INC              0x4
#define DMA_MODE_ADD_NO_UPD              0x2
#define DMA_MODE_ADD_MASK                0x6
#define DMA_MODE_GET                     0x0
#define DMA_MODE_SET                     0x1
#define DMA_MODE_SG                      0x8

#define DMA_SPACE_PCIE       0x00 // Over PCIe EP 0
#define DMA_SPACE_PCIE1      0x01 // Over PCIe EP 1
#define DMA_SPACE_SHM        0x02
#define DMA_SPACE_SHM2       0x03
#define DMA_SPACE_USR        0x04
#define DMA_SPACE_USR2       0x05
#define DMA_SPACE_KBUF       0x08 // Over PCIe EP 0
#define DMA_SPACE_KBUF1      0x09 // Over PCIe EP 1
#define DMA_SPACE_MASK       0x07

#define DMA_START_PIPE_NO    0x00
#define DMA_START_PIPE       0x01
#define DMA_START_CHAN(chan)  ((chan&3) << 4)
#define DMA_START_CHAN_MASK              0x30
#define DMA_START_CHAN_GET(x)      ((x>>4)&3)
#define DMA_SPACE_WS      0x10
#define DMA_SPACE_DS      0x20
#define DMA_SPACE_QS      0x30

#define DMA_INTR_ENA      0x01

#define DMA_WAIT_INTR     0x01
#define DMA_WAIT_1MS      0x02
#define DMA_WAIT_10MS     0x04
#define DMA_WAIT_100MS    0x06
#define DMA_WAIT_1S       0x08
#define DMA_WAIT_10S      0x0a
#define DMA_WAIT_100S     0x0c

#define DMA_PCIE_TC0      0x00  /* Traffic Class 0 */
#define DMA_PCIE_TC1      0x01  /* Traffic Class 1 */
#define DMA_PCIE_TC2      0x02  /* Traffic Class 2 */
#define DMA_PCIE_TC3      0x03  /* Traffic Class 3 */
#define DMA_PCIE_TC4      0x04  /* Traffic Class 4 */
#define DMA_PCIE_TC5      0x05  /* Traffic Class 5 */
#define DMA_PCIE_TC6      0x06  /* Traffic Class 6 */
#define DMA_PCIE_TC7      0x07  /* Traffic Class 7 */
#define DMA_PCIE_RR1      0x00  /* 1 outstanding read request */
#define DMA_PCIE_RR2      0x10  /* 2 outstanding read request */
#define DMA_PCIE_RR3      0x20  /* 3 outstanding read request */

#define DMA_SIZE_PKT_128  0x00000000 
#define DMA_SIZE_PKT_256  0x40000000 
#define DMA_SIZE_PKT_512  0x80000000 
#define DMA_SIZE_PKT_1K   0xc0000000 

#define DMA_STATE_IDLE          0x00
#define DMA_STATE_ALLOCATED     0x01
#define DMA_STATE_STARTED       0x02
#define DMA_STATE_WAITING       0x03
#define DMA_STATE_DONE          0x04

#define DMA_STATUS_RUN_RD0             0x01
#define DMA_STATUS_RUN_RD1             0x02
#define DMA_STATUS_RUN_WR0             0x04
#define DMA_STATUS_RUN_WR1             0x08
#define DMA_STATUS_DONE                0x10
#define DMA_STATUS_WAITING             0x000
#define DMA_STATUS_ENDED               0x100
#define DMA_STATUS_TMO                 0x80
#define DMA_STATUS_ERR                 0x40
#define DMA_STATUS_BUSY                0x20

#define TSC_IOCTL_KBUF                0x00090000
#define TSC_IOCTL_KBUF_ALLOC         (TSC_IOCTL_KBUF | 0x1)
#define TSC_IOCTL_KBUF_FREE          (TSC_IOCTL_KBUF | 0x2)
#define TSC_IOCTL_KBUF_READ          (TSC_IOCTL_KBUF | 0x3)
#define TSC_IOCTL_KBUF_WRITE         (TSC_IOCTL_KBUF | 0x4)

struct tsc_ioctl_kbuf_req
{
  uint size;
  void *k_base;
  uint64_t b_base;
  void *u_base;
};

struct tsc_ioctl_kbuf_rw
{
  uint size;
  void *k_addr;
  void *buf;
};

#define TSC_IOCTL_TIMER                0x000a0000
#define TSC_IOCTL_TIMER_START         (TSC_IOCTL_TIMER | 0x1)
#define TSC_IOCTL_TIMER_RESTART       (TSC_IOCTL_TIMER | 0x2)
#define TSC_IOCTL_TIMER_STOP          (TSC_IOCTL_TIMER | 0x3)
#define TSC_IOCTL_TIMER_READ          (TSC_IOCTL_TIMER | 0x4)

struct tsc_ioctl_timer
{
  int operation;
  int mode;
  struct tsc_time
  {
    int msec;
    int usec;
  } time;
};

#define TIMER_ENA          0x80000000  /* timer global enable                    */
#define TIMER_1MHZ         0x00000000  /* timer frequency 1 MHz                  */
#define TIMER_5MHZ         0x00000001  /* timer frequency 5 MHz                  */
#define TIMER_25MHZ        0x00000002  /* timer frequency 25 MHz                 */
#define TIMER_100MHZ       0x00000003  /* timer frequency 100 MHz                */
#define TIMER_BASE_1000    0x00000000  /* timer period 1000 usec                 */
#define TIMER_BASE_1024    0x00000008  /* timer period 1024 usec                 */
#define TIMER_SYNC_LOC     0x00000000  /* timer synchronization local            */
#define TIMER_SYNC_USR1    0x00000010  /* timer synchronization user signal #1   */
#define TIMER_SYNC_USR2    0x00000020  /* timer synchronization user signal #2   */
#define TIMER_SYNC_SYSFAIL 0x00000040  /* timer synchronization sysfail          */
#define TIMER_SYNC_IRQ1    0x00000050  /* timer synchronization IRQ#1            */
#define TIMER_SYNC_IRQ2    0x00000060  /* timer synchronization IRQ#2            */
#define TIMER_SYNC_ENA     0x00000080  /* timer synchronization enable           */
#define TIMER_OUT_SYSFAIL  0x00000100  /* issue sync signal on sysfail           */
#define TIMER_OUT_IRQ1     0x00000200  /* issue sync signal on IRQ#1             */
#define TIMER_OUT_IRQ2     0x00000300  /* issue sync signal on IRQ#2   	         */
#define TIMER_SYNC_ERR     0x00010000  /* timer synchronization error            */
#define TIMER_UTIME_MASK   0x0000ffff  /* mask for usec                          */


#define TSC_IOCTL_SFLASH                0x000b0000
#define TSC_IOCTL_SFLASH_RDID           (TSC_IOCTL_SFLASH | 0x1)
#define TSC_IOCTL_SFLASH_RDSR           (TSC_IOCTL_SFLASH | 0x2)
#define TSC_IOCTL_SFLASH_WRSR           (TSC_IOCTL_SFLASH | 0x3)
#define TSC_IOCTL_SFLASH_READ           (TSC_IOCTL_SFLASH | 0x4)
#define TSC_IOCTL_SFLASH_WRITE          (TSC_IOCTL_SFLASH | 0x5)

#define TSC_IOCTL_FIFO            	0x000c0000
#define TSC_IOCTL_FIFO_INIT       	(TSC_IOCTL_FIFO | 0x1)
#define TSC_IOCTL_FIFO_READ         	(TSC_IOCTL_FIFO | 0x2)
#define TSC_IOCTL_FIFO_WRITE         	(TSC_IOCTL_FIFO | 0x3)
#define TSC_IOCTL_FIFO_CLEAR      	(TSC_IOCTL_FIFO | 0x4)
#define TSC_IOCTL_FIFO_WAIT_EF    	(TSC_IOCTL_FIFO | 0x5)
#define TSC_IOCTL_FIFO_WAIT_FF    	(TSC_IOCTL_FIFO | 0x6)
#define TSC_IOCTL_FIFO_STATUS     	(TSC_IOCTL_FIFO | 0x7)

struct tsc_ioctl_fifo
{
  uint idx;
  uint sts;
  uint *data;
  uint cnt;
  uint tmo;
  uint mode;
};

#define TSC_IOCTL_I2C            	0x000d0000
#define TSC_IOCTL_I2C_RESET       	(TSC_IOCTL_I2C | 0x1)
#define TSC_IOCTL_I2C_READ         	(TSC_IOCTL_I2C | 0x2)
#define TSC_IOCTL_I2C_WRITE         	(TSC_IOCTL_I2C | 0x3)
#define TSC_IOCTL_I2C_CMD         	(TSC_IOCTL_I2C | 0x4)

#define I2C_DEV( addr, bus, size) (((bus &0x7)<<29) | (addr&0x7f) | size)

struct tsc_i2c_devices
{
  char *name;
  uint id;
};

struct tsc_ioctl_i2c
{
  uint device;
  uint cmd;
  uint data;
  uint status;
};

#define TSC_IOCTL_SEMAPHORE         0x000e0000
#define TSC_IOCTL_SEMAPHORE_STATUS  (TSC_IOCTL_SEMAPHORE | 0x1)
#define TSC_IOCTL_SEMAPHORE_RELEASE (TSC_IOCTL_SEMAPHORE | 0x2)
#define TSC_IOCTL_SEMAPHORE_GET     (TSC_IOCTL_SEMAPHORE | 0x3)

struct tsc_ioctl_semaphore
{
  uint idx;
  uint sts;
  uint tag;
  uint *location;
};

typedef enum {
	RSP1461_EXT_PIN_LOW,
	RSP1461_EXT_PIN_HIGH,
	RSP1461_EXT_PIN_Z
	} rsp1461_ext_pin_state_t;

typedef enum {
	RSP1461_LED123_GREEN,	// 0
	RSP1461_LED123_RED,		// 1
	RSP1461_LED124_GREEN,	// 2
	RSP1461_LED124_RED,		// 3
	RSP1461_LED125_GREEN,	// 4
	RSP1461_LED125_RED,		// 5
	RSP1461_LED126_GREEN,	// 6
	RSP1461_LED126_RED,		// 7
	} rsp1461_led_t;

typedef enum {
	RSP1461_SFP_FPGA_LANE_0,
	RSP1461_SFP_FPGA_LANE_1,
	RSP1461_SFP_FPGA_LANE_2,
	RSP1461_SFP_FPGA_LANE_3,
	RSP1461_SFP_CPU_SGMII,
	RSP1461_SFP_CPU_XFI_LANE_0,
	RSP1461_SFP_CPU_XFI_LANE_1
	} rsp1461_sfp_id_t;

typedef enum {
	SFP_PRESENT        = 0x08,
	SFP_TX_FAULT       = 0x10,
	SFP_LOSS_OF_SIGNAL = 0x20
	} rsp1461_sfp_status_t;

typedef enum {
	SFP_TX_DISABLE     = 0x01,
	SFP_RX_HIGH_RATE   = 0x02,
	SFP_TX_HIGH_RATE   = 0x04
	} rsp1461_sfp_control_t;

#define TSC_IOCTL_USER              0x000f0000
#define TSC_IOCTL_USER_WAIT         (TSC_IOCTL_USER | 0x1)
#define TSC_IOCTL_USER_SUBSCRIBE    (TSC_IOCTL_USER | 0x2)
#define TSC_IOCTL_USER_UNSUBSCRIBE  (TSC_IOCTL_USER | 0x3)

struct tsc_ioctl_user_irq
{
	int irq;
	int mask;
	int wait_mode;
};
#endif /*  _H_TSCIOCTL */
