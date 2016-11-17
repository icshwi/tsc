/*=========================< begin file & file header >=======================
 *  References
 *  
 *    filename : ifc1211.h
 *    author   : JFG
 *    company  : IOxOS
 *    creation : june 30,2008
 *    version  : 0.0.1
 *
 *----------------------------------------------------------------------------
 *  Description
 *
 *    This file contain the declarations related to the IFC1211 device
 *
 *----------------------------------------------------------------------------
 *  Copyright Notice
 *  
 *    Copyright and all other rights in this document are reserved by 
 *    IOxOS Technologies SA. This documents contains proprietary information    
 *    and is supplied on express condition that it may not be disclosed, 
 *    reproduced in whole or in part, or used for any other purpose other
 *    than that for which it is supplies, without the written consent of  
 *    IOxOS Technologies SA                                                        
 *
 *----------------------------------------------------------------------------
 *  Change History
 *  
 *  
 *=============================< end file header >============================*/

#ifndef _H_IFC1211
#define _H_IFC1211

#define	PCI_VENDOR_ID_IOXOS                 0x7357   /* IOXOS PCI VENDOR ID           */
#define	PCI_DEVICE_ID_IOXOS_IFC1211_IO      0x1000   /* IFC1211 PCI DEVICE ID IO      */
#define	PCI_DEVICE_ID_IOXOS_IFC1211_CENTRAL 0x1001   /* IFC1211 PCI DEVICE ID CENTRAL */

/*
 *  Define the number of each that the IFC1211 supports.
 */
#define IFC1211_MAX_MASTER	             64	 /* Max Master Windows                   */
#define IFC1211_MAX_SLAVE	             64  /* Max Slave Windows                    */
#define IFC1211_MAX_DMA		      2	 /* Max DMA Controllers                          */
#define IFC1211_MAX_MAILBOX		      4	 /* Max Mail Box registers                   */
#define IFC1211_MAX_SEMAPHORE              8	 /* Max Semaphores                   */
#define IFC1211_MAX_EVENT                 16	 /* Max Event Queues                 */
#define IFC1211_IRQ_NUM                   64  /* number of interrupt sources         */

#define IFC1211_IRQ_CTL_ILOC                0  /*  ILOC controler IRQ base           */
#define IFC1211_IRQ_CTL_IDMA               32  /*  IDMA controler IRQ base           */
#define IFC1211_IRQ_CTL_MBX                32  /*  FIFO controler IRQ base           */
#define IFC1211_IRQ_CTL_USER               48  /*  USER controler IRQ base           */

#define IFC1211_IRQ_SRC_I2C_CRIT            9  /*  I2C CRIT                           */
#define IFC1211_IRQ_SRC_I2C_OK             10  /*  I2C ended OK                       */
#define IFC1211_IRQ_SRC_I2C_ERR            11  /*  I2C Ended with error               */

#define IFC1211_IRQ_SRC_SYSFAIL             0  /*  VME SYSFAIL interrupt              */
#define IFC1211_IRQ_SRC_IRQ1                1  /*  VME IRQ1 interrupt                 */
#define IFC1211_IRQ_SRC_IRQ2                2  /*  VME IRQ2 interrupt                 */
#define IFC1211_IRQ_SRC_IRQ3                3  /*  VME IRQ3 interrupt                 */
#define IFC1211_IRQ_SRC_IRQ4                4  /*  VME IRQ4 interrupt                 */
#define IFC1211_IRQ_SRC_IRQ5                5  /*  VME IRQ5 interrupt                 */
#define IFC1211_IRQ_SRC_IRQ6                6  /*  VME IRQ6 interrupt                 */
#define IFC1211_IRQ_SRC_IRQ7                7  /*  VME IRQ7 interrupt                 */
#define IFC1211_IRQ_SRC_ACFAIL              8  /*  VME ACFAIL interrupt               */
#define IFC1211_IRQ_SRC_MASERR              9  /*  VME Master Error interrupt         */
#define IFC1211_IRQ_SRC_GTIM0              10  /*  VME Global Time interrupt 0        */
#define IFC1211_IRQ_SRC_GTIM1              11  /*  VME Global Time interrupt 1        */
#define IFC1211_IRQ_SRC_LM0                12  /*  VME Location Monitor 0 interrupt   */
#define IFC1211_IRQ_SRC_LM1                13  /*  VME Location Monitor 1 interrupt   */
#define IFC1211_IRQ_SRC_LM2                14  /*  VME Location Monitor 2 interrupt   */
#define IFC1211_IRQ_SRC_LM3                15  /*  VME Location Monitor 3 interrupt   */

#define IFC1211_IRQ_SRC_IDMA_RD0_END         0  /*  IDMA read channel #0 ended         */
#define IFC1211_IRQ_SRC_IDMA_RD0_ERR         1  /*  IDMA read channel #0 error         */
#define IFC1211_IRQ_SRC_IDMA_RD1_END         2  /*  IDMA read channel #1 ended         */
#define IFC1211_IRQ_SRC_IDMA_RD1_ERR         3  /*  IDMA read channel #1 error         */
#define IFC1211_IRQ_SRC_IDMA_WR0_END         4  /*  IDMA write channel #0 ended        */
#define IFC1211_IRQ_SRC_IDMA_WR0_ERR         5  /*  IDMA write channel #0 error        */
#define IFC1211_IRQ_SRC_IDMA_WR1_END         6  /*  IDMA write channel #1 ended        */
#define IFC1211_IRQ_SRC_IDMA_WR1_ERR         7  /*  IDMA write channel #1 error        */
#define IFC1211_IRQ_SRC_MBX_0                8  /*  Mailbox #0 not empty               */
#define IFC1211_IRQ_SRC_MBX_1                9  /*  Mailbox #0 not empty               */
#define IFC1211_IRQ_SRC_MBX_2               10  /*  Mailbox #0 not empty               */
#define IFC1211_IRQ_SRC_MBX_3               11  /*  Mailbox #0 not empty               */
#define IFC1211_IRQ_SRC_MBX_4               12  /*  Mailbox #0 not empty               */
#define IFC1211_IRQ_SRC_MBX_5               13  /*  Mailbox #0 not empty               */
#define IFC1211_IRQ_SRC_MBX_6               14  /*  Mailbox #0 not empty               */
#define IFC1211_IRQ_SRC_MBX_7               15  /*  Mailbos #7 not empty               */

#define IFC1211_ITC_IM_ALL            (0xffff)  /*  mask for  All interrupts           */
#define IFC1211_ITC_VEC( sts)       (sts&0xff)  /*  get irq ector from iack status     */
#define IFC1211_ITC_SRC( sts)   ((sts>>8)&0xf) /*  get irq source from iack status    */
#define IFC1211_ITC_CTL( ctl)  ((ctl>>12)&0x3)  /*  get irq controller from iack status   */


/*
 *  IFC1211 ASIC register structure overlays and bit field definitions.
 *  refer to IFC1211 user's manual for detailed description.
 *
 */
#define IFC1211_CSR_ILOC_BASE	        	0x000
#define IFC1211_CSR_ILOC_STATIC	        	0x000
#define IFC1211_CSR_ILOC_OPT_DYN_SEL         0x004
#define IFC1211_CSR_ILOC_OPT_DYN_DAT         0x008
#define IFC1211_CSR_ILOC_PON_FSM             0x00c
#define IFC1211_CSR_ILOC_SPI                 0x010
#define IFC1211_CSR_ILOC_EFUSE_USR           0x014
#define IFC1211_CSR_ILOC_SIGN                0x018
#define IFC1211_CSR_ILOC_GENCTL              0x01c

#define IFC1211_CSR_PCIE_MMUADD              0x020
#define IFC1211_CSR_PCIE_MMUDAT              0x024

#define IFC1211_CSR_ILOC_TOSCA2_SIGN         0x034

#define IFC1211_CSR_A7_PCIE_DSN_L            0x038
#define IFC1211_CSR_A7_PCIE_DSN_H            0x03c
#define IFC1211_CSR_A7_SMON_ADDPT            0x040
#define IFC1211_CSR_A7_SMON_DAT              0x044
#define IFC1211_CSR_A7_SMON_STA              0x048
#define IFC1211_CSR_A7_PCIE_REQCNT           0x050
#define IFC1211_CSR_A7_PCIE_ARMCNT           0x054
#define IFC1211_CSR_A7_PCIE_ACKCNT           0x058
#define IFC1211_CSR_A7_PCIE_CTL              0x05c
#define IFC1211_CSR_A7_PCIEP_ADDPT           0x060
#define IFC1211_CSR_A7_PCIEP_DRPDAT          0x064
#define IFC1211_CSR_A7_PCIEP_CFGDAT          0x068
#define IFC1211_CSR_A7_PCIEP_LINK_STA        0x06c
#define IFC1211_CSR_A7_PCIEP_CFG_STA_0       0x070
#define IFC1211_CSR_A7_PCIEP_CFG_STA_1       0x074
#define IFC1211_CSR_A7_PCIEP_CFG_STA_2       0x078
#define IFC1211_CSR_A7_PCIEP_CFG_STA_3       0x07c

#define IFC1211_CSR_ILOC_ITC_IACK            0x080
#define IFC1211_CSR_ILOC_ITC_CSR             0x084
#define IFC1211_CSR_ILOC_ITC_IMC             0x088
#define IFC1211_CSR_ILOC_ITC_IMS             0x08c

#define IFC1211_CSR_I2C_CTL                  0x100
#define IFC1211_CSR_I2C_CMD                  0x104
#define IFC1211_CSR_I2C_DATW                 0x108
#define IFC1211_CSR_I2C_DATR                 0x10c

#define IFC1211_CSR_PVME_BASE                0x400
#define IFC1211_CSR_PVME_SLOT1               0x400
#define IFC1211_CSR_PVME_MASCSR              0x404
#define IFC1211_CSR_PVME_SLVCSR              0x408
#define IFC1211_CSR_PVME_SLVCSR_A32          0x408
#define IFC1211_CSR_PVME_INTG                0x40c
#define IFC1211_CSR_PVME_MMUADD              0x410
#define IFC1211_CSR_PVME_MMUDAT              0x414
#define IFC1211_CSR_PVME_ADDERR              0x418
#define IFC1211_CSR_PVME_STAERR              0x41c
#define IFC1211_CSR_PVME_LOCMON              0x420
#define IFC1211_CSR_PVME_LOCK                0x424
#define IFC1211_CSR_PVME_RMW_MODE            0x428
#define IFC1211_CSR_PVME_RMW_ADD             0x42c
#define IFC1211_CSR_PVME_RMW_DATCMP          0x430
#define IFC1211_CSR_PVME_RMW_DATUPT          0x434
#define IFC1211_CSR_PVME_GLTIM_CSR           0x440
#define IFC1211_CSR_PVME_GLTIM_DBG           0x444
#define IFC1211_CSR_PVME_GLTIM_CNT2          0x448
#define IFC1211_CSR_PVME_GLTIM_CNT1          0x44c
#define IFC1211_CSR_PVME_SLVCSR_A24          0x450
#define IFC1211_CSR_PVME_SLVCSR_A16          0x454
#define IFC1211_CSR_PVME_SLVCSR_2eBRC        0x458
                                                                        
#define IFC1211_CSR_PVME_ITC_IACK            0x480
#define IFC1211_CSR_PVME_ITC_CSR             0x484
#define IFC1211_CSR_PVME_ITC_IMC             0x488
#define IFC1211_CSR_PVME_ITC_IMS             0x48c

#define IFC1211_CSR_VMECSR_ADER0_3	        0x560
#define IFC1211_CSR_VMECSR_ADER0_2	        0x564
#define IFC1211_CSR_VMECSR_ADER0_1	        0x568
#define IFC1211_CSR_VMECSR_ADER0_0	        0x56c
#define IFC1211_CSR_VMECSR_ADER1_3	        0x570
#define IFC1211_CSR_VMECSR_ADER1_2	        0x574
#define IFC1211_CSR_VMECSR_ADER1_1	        0x578
#define IFC1211_CSR_VMECSR_ADER1_0	        0x57c
#define IFC1211_CSR_VMECSR_ADER2_3	        0x580
#define IFC1211_CSR_VMECSR_ADER2_2	        0x584
#define IFC1211_CSR_VMECSR_ADER2_1	        0x588
#define IFC1211_CSR_VMECSR_ADER2_0	        0x58c
#define IFC1211_CSR_VMECSR_ADER3_3	        0x590
#define IFC1211_CSR_VMECSR_ADER3_2	        0x594
#define IFC1211_CSR_VMECSR_ADER3_1	        0x598
#define IFC1211_CSR_VMECSR_ADER3_0	        0x59c
#define IFC1211_CSR_VMECSR_BCR	        	0x5f4
#define IFC1211_CSR_VMECSR_BSR	        	0x5f8
#define IFC1211_CSR_VMECSR_BAR	        	0x5fc
#define IFC1211_CSR_VMECSR_ROM	        	0x780

#define IFC1211_CSR_SMEM_BASE	        	0x800
#define IFC1211_CSR_SMEM_DDR3_CSR	        0x800
#define IFC1211_CSR_SMEM_DDR3_ERR	        0x804
#define IFC1211_CSR_SMEM_DDR3_DELQ	        0x808
#define IFC1211_CSR_SMEM_DDR3_IDEL	        0x80c
#define IFC1211_CSR_SMEM_SRAM_CSR	        0x810

#define IFC1211_CSR_IDMA_BASE	       	 	0x800
#define IFC1211_CSR_IDMA_GCSR	        	0x840

#define IFC1211_CSR_IDMA_RD_0_PCSR	        0x850
#define IFC1211_CSR_IDMA_RD_1_PCSR	        0x854
#define IFC1211_CSR_IDMA_WR_0_PCSR	        0x858
#define IFC1211_CSR_IDMA_WR_1_PCSR	        0x85c

#define IFC1211_CSR_IDMA_ITC_IACK	        0x880
#define IFC1211_CSR_IDMA_ITC_CSR	        	0x884
#define IFC1211_CSR_IDMA_ITC_IMC	        	0x888
#define IFC1211_CSR_IDMA_ITC_IMS	        	0x88c

#define IFC1211_CSR_SEMAPHORE 	 			0x8b8

#define IFC1211_MBX_NUM                      8
#define IFC1211_CSR_MBX_0_CTL	        	0x8C0
#define IFC1211_CSR_MBX_1_CTL	        	0x8C4
#define IFC1211_CSR_MBX_2_CTL	        	0x8C8
#define IFC1211_CSR_MBX_3_CTL	        	0x8CC
#define IFC1211_CSR_MBX_4_CTL	        	0x8D0
#define IFC1211_CSR_MBX_5_CTL	        	0x8D4
#define IFC1211_CSR_MBX_6_CTL	        	0x8D8
#define IFC1211_CSR_MBX_7_CTL	        	0x8DC
static const int IFC1211_CSR_MBX_CTL[IFC1211_MBX_NUM] = {
                                                IFC1211_CSR_MBX_0_CTL, IFC1211_CSR_MBX_1_CTL,
												IFC1211_CSR_MBX_2_CTL, IFC1211_CSR_MBX_3_CTL,
												IFC1211_CSR_MBX_4_CTL, IFC1211_CSR_MBX_5_CTL,
												IFC1211_CSR_MBX_6_CTL, IFC1211_CSR_MBX_7_CTL };

#define IFC1211_CSR_MBX_0_PORT	        0x8E0
#define IFC1211_CSR_MBX_1_PORT	        0x8E4
#define IFC1211_CSR_MBX_2_PORT	        0x8E8
#define IFC1211_CSR_MBX_3_PORT	        0x8EC
#define IFC1211_CSR_MBX_4_PORT	        0x8F0
#define IFC1211_CSR_MBX_5_PORT	        0x8F4
#define IFC1211_CSR_MBX_6_PORT	        0x8F8
#define IFC1211_CSR_MBX_7_PORT	        0x8FC
static const int IFC1211_CSR_MBX_PORT[IFC1211_MBX_NUM] = {
                                                IFC1211_CSR_MBX_0_PORT, IFC1211_CSR_MBX_1_PORT,
			                        IFC1211_CSR_MBX_2_PORT, IFC1211_CSR_MBX_3_PORT,
		                              	IFC1211_CSR_MBX_4_PORT, IFC1211_CSR_MBX_5_PORT,
		                        	IFC1211_CSR_MBX_6_PORT, IFC1211_CSR_MBX_7_PORT };

#define IFC1211_FIFO_NUM                         8
#define IFC1211_CSR_FIFO_0_CTL	        0x8C0
#define IFC1211_CSR_FIFO_1_CTL	        0x8C4
#define IFC1211_CSR_FIFO_2_CTL	        0x8C8
#define IFC1211_CSR_FIFO_3_CTL	        0x8CC
#define IFC1211_CSR_FIFO_4_CTL	        0x8D0
#define IFC1211_CSR_FIFO_5_CTL	        0x8D4
#define IFC1211_CSR_FIFO_6_CTL	        0x8D8
#define IFC1211_CSR_FIFO_7_CTL	        0x8DC
static const int IFC1211_CSR_FIFO_CTL[IFC1211_FIFO_NUM] = {
                                                IFC1211_CSR_FIFO_0_CTL, IFC1211_CSR_FIFO_1_CTL,
			                        IFC1211_CSR_FIFO_2_CTL, IFC1211_CSR_FIFO_3_CTL,
		                              	IFC1211_CSR_FIFO_4_CTL, IFC1211_CSR_FIFO_5_CTL,
		                        	IFC1211_CSR_FIFO_6_CTL, IFC1211_CSR_FIFO_7_CTL };

#define IFC1211_CSR_FIFO_0_PORT	        0x8E0
#define IFC1211_CSR_FIFO_1_PORT	        0x8E4
#define IFC1211_CSR_FIFO_2_PORT	        0x8E8
#define IFC1211_CSR_FIFO_3_PORT	        0x8EC
#define IFC1211_CSR_FIFO_4_PORT	        0x8F0
#define IFC1211_CSR_FIFO_5_PORT	        0x8F4
#define IFC1211_CSR_FIFO_6_PORT	        0x8F8
#define IFC1211_CSR_FIFO_7_PORT	        0x8FC
static const int IFC1211_CSR_FIFO_PORT[IFC1211_FIFO_NUM] = {
                                                IFC1211_CSR_FIFO_0_PORT, IFC1211_CSR_FIFO_1_PORT,
			                        IFC1211_CSR_FIFO_2_PORT, IFC1211_CSR_FIFO_3_PORT,
		                              	IFC1211_CSR_FIFO_4_PORT, IFC1211_CSR_FIFO_5_PORT,
		                        	IFC1211_CSR_FIFO_6_PORT, IFC1211_CSR_FIFO_7_PORT };

#define IFC1211_CSR_IDMA_RD_0_CSR	        0x900
#define IFC1211_CSR_IDMA_RD_0_NDES	        0x904
#define IFC1211_CSR_IDMA_RD_0_CDES	        0x908
#define IFC1211_CSR_IDMA_RD_0_DCNT	        0x90c

#define IFC1211_CSR_IDMA_RD_1_CSR	        0x940
#define IFC1211_CSR_IDMA_RD_1_NDES	        0x944
#define IFC1211_CSR_IDMA_RD_1_CDES	        0x948
#define IFC1211_CSR_IDMA_RD_1_DCNT	        0x94c

#define IFC1211_CSR_IDMA_WR_0_CSR	        0xa00
#define IFC1211_CSR_IDMA_WR_0_NDES	        0xa04
#define IFC1211_CSR_IDMA_WR_0_CDES	        0xa08
#define IFC1211_CSR_IDMA_WR_0_DCNT	        0xa0c

#define IFC1211_CSR_IDMA_WR_1_CSR	        0xa40
#define IFC1211_CSR_IDMA_WR_1_NDES	        0xa44
#define IFC1211_CSR_IDMA_WR_1_CDES	        0xa48
#define IFC1211_CSR_IDMA_WR_1_DCNT	        0xa4c

static const int IFC1211_CSR_IDMA_CSR[4] = { IFC1211_CSR_IDMA_RD_0_CSR, IFC1211_CSR_IDMA_RD_1_CSR,
					        IFC1211_CSR_IDMA_WR_0_CSR, IFC1211_CSR_IDMA_WR_1_CSR };
static const int IFC1211_CSR_IDMA_NDES[4] = { IFC1211_CSR_IDMA_RD_0_NDES, IFC1211_CSR_IDMA_RD_1_NDES,
					         IFC1211_CSR_IDMA_WR_0_NDES, IFC1211_CSR_IDMA_WR_1_NDES };
static const int IFC1211_CSR_IDMA_CDES[4] = { IFC1211_CSR_IDMA_RD_0_CDES, IFC1211_CSR_IDMA_RD_1_CDES,
					         IFC1211_CSR_IDMA_WR_0_CDES, IFC1211_CSR_IDMA_WR_1_CDES };
static const int IFC1211_CSR_IDMA_DCNT[4] = { IFC1211_CSR_IDMA_RD_0_DCNT, IFC1211_CSR_IDMA_RD_1_DCNT,
					         IFC1211_CSR_IDMA_WR_0_DCNT, IFC1211_CSR_IDMA_WR_1_DCNT };

#define IFC1211_CSR_IDMA_ROM 	        0xb80

#define IFC1211_CSR_USER_BASE	        0xc00
#define IFC1211_CSR_GPIO_OUT	            0xc00
#define IFC1211_CSR_GPIO_ENA	            0xc04
#define IFC1211_CSR_GPIO_IN	            0xc08
#define IFC1211_CSR_GPIO_IPOL	        0xc0c

#define IFC1211_CSR_UART_CTL	            0xc10
#define IFC1211_CSR_VME_SERIAL	        0xc14

#define IFC1211_CSR_USER_ITC_IACK	    0xc80
#define IFC1211_CSR_USER_ITC_CSR	        0xc84
#define IFC1211_CSR_USER_ITC_IMC	        0xc88
#define IFC1211_CSR_USER_ITC_IMS	        0xc8c

#define IFC1211_CSR_USER_ROM	                0xf80

#define IFC1211_CSR_OFFSET_MAX	       0xfff

/*
 *  SPI FLASH control bits (CSR + $010)
 */
#define IFC1211_ILOC_SPI_END                0x00
#define IFC1211_ILOC_SPI_CLK                0x01
#define IFC1211_ILOC_SPI_DO                 0x02
#define IFC1211_ILOC_SPI_DI                 0x04
#define IFC1211_ILOC_SPI_CS                 0x08
#define IFC1211_ILOC_SPI_WRPROT_PIN       0x4000
#define IFC1211_ILOC_SPI_WRPROT_SW        0x8000
#define IFC1211_ILOC_SPI_WRPROT(x)   ((x>>14)&3)
#define IFC1211_ILOC_SPI_WPOST_BUSY     0x100000

/*
 *  TOSCA2 Signature (CSR + $034)
 */
#define IFC1211_TOSCA2_SIGN	        0x20000000

/*
 *  VME Master MMU parameters
 */
#define IFC1211_PCIE_MMU_PG_NUM                 1024  /* number of pages 1024 pages  */
#define IFC1211_PCIE_MMU_PG_4M              0x400000  /* page size 4 MBytes          */
#define IFC1211_PCIE_MMU_PG_1M              0x100000  /* page size 1 MBytes          */
#define IFC1211_PCIE_MMU_PG_64K              0x10000  /* page size 64 KBytes         */

/*
 *  Master MMU address pointer(CSR + $20)
 */
#define IFC1211_PCIE_MMUADD_PTR_MASK                  (0x1ffe<<0)  /* Mask for MMU pointer (1024 pages) */
#define IFC1211_PCIE_MMUADD_PMEM_SEL                      (0<<16)  /* Select MMU for A64 PMEM           */
#define IFC1211_PCIE_MMUADD_MEM_SEL                       (1<<16)  /* Select MMU for A32                */
#define IFC1211_PCIE_MMUADD_MEM_SIZE(x)   (0x200000<<((x>>18)&7))  /* calculate PCI MEM size            */
#define IFC1211_PCIE_MMUADD_MEM_ENA                       (1<<23)  /* PCI MEM enable                    */
#define IFC1211_PCIE_MMUADD_PMEM_SIZE(x) (0x2000000<<((x>>26)&7))  /* calculate PCI MEM size            */
#define IFC1211_PCIE_MMUADD_PMEM_A64                      (1<<30)  /* PCI PMEM A64                      */
#define IFC1211_PCIE_MMUADD_PMEM_ENA                      (1<<31)  /* PCI PMEM enable                   */

/*
 *  Master MMU data register(CSR + $24)
 */
#define IFC1211_PCIE_MMUDAT_PG_ENA               (1<<0)  /* Enable page                       */
#define IFC1211_PCIE_MMUDAT_WR_ENA               (1<<1)  /* Enable write                      */
#define IFC1211_PCIE_MMUDAT_D16                  (1<<3)  /* D16 access                        */
#define IFC1211_PCIE_MMUDAT_PROG                 (1<<4)  /* Program access                    */
#define IFC1211_PCIE_MMUDAT_SUPER                (1<<5)  /* Supervisor access                 */
#define IFC1211_PCIE_MMUDAT_SWAP_AUTO            (1<<6)  /* set auto swap be <-> le           */
#define IFC1211_PCIE_MMUDAT_SWAP_DW              (2<<6)  /* set double word swap              */
#define IFC1211_PCIE_MMUDAT_SWAP_QW              (3<<6)  /* set qwad word swap                */
#define IFC1211_PCIE_MMUDAT_SWAP_MASK            (3<<6)  /* set double word swap              */
#define IFC1211_PCIE_MMUDAT_AS_CRCSR             (0<<8)  /* address space CRCSR               */
#define IFC1211_PCIE_MMUDAT_AS_A16               (1<<8)  /* address space A16                 */
#define IFC1211_PCIE_MMUDAT_AS_A24               (2<<8)  /* address space A24                 */
#define IFC1211_PCIE_MMUDAT_AS_A32               (3<<8)  /* address space A32                 */
#define IFC1211_PCIE_MMUDAT_AS_A32_BLT           (4<<8)  /* address space A32 BLT             */
#define IFC1211_PCIE_MMUDAT_AS_A32_MBLT          (5<<8)  /* address space A32 MBLT            */
#define IFC1211_PCIE_MMUDAT_AS_A32_2eVME         (6<<8)  /* address space A32 2eVME           */
#define IFC1211_PCIE_MMUDAT_AS_USER              (7<<8)  /* address space USER                */
#define IFC1211_PCIE_MMUDAT_AS_A32_2eSST_160     (8<<8)  /* address space A32 2eSST 160       */
#define IFC1211_PCIE_MMUDAT_AS_A32_2eSST_233     (9<<8)  /* address space A32 2eSST 233       */
#define IFC1211_PCIE_MMUDAT_AS_A32_2eSST_320    (10<<8)  /* address space A32 2eSST 320       */
#define IFC1211_PCIE_MMUDAT_AS_A32_2eSST_400    (11<<8)  /* address space A32 2eSST 400       */
#define IFC1211_PCIE_MMUDAT_AS_A32_2eBCR        (12<<8)  /* address space A32 2e broadcast    */
#define IFC1211_PCIE_MMUDAT_AS_A24_ADO          (13<<8)  /* address space A24 Address Only    */
#define IFC1211_PCIE_MMUDAT_AS_A32_ADO          (14<<8)  /* address space A32 Address Only    */
#define IFC1211_PCIE_MMUDAT_AS_IACK             (15<<8)  /* address space IACK                */
#define IFC1211_PCIE_MMUDAT_AS_MASK             (15<<8)  /* address space mask                */
#define IFC1211_PCIE_MMUDAT_DES_VME             (1<<12)  /* destination VME bus               */
#define IFC1211_PCIE_MMUDAT_DES_SHM             (2<<12)  /* destination Shared Memory         */
#define IFC1211_PCIE_MMUDAT_DES_SHM1            (2<<12)  /* destination Shared Memory 1       */
#define IFC1211_PCIE_MMUDAT_DES_SHM2            (3<<12)  /* destination Shared Memory 2       */
#define IFC1211_PCIE_MMUDAT_DES_USR             (3<<12)  /* destination User Block            */
#define IFC1211_PCIE_MMUDAT_DES_USR1            (4<<12)  /* destination User Block 1          */
#define IFC1211_PCIE_MMUDAT_DES_USR2            (5<<12)  /* destination User Block 2          */

#define IFC1211_PCIE_MMUDAT_DES_MASK          (0xf<<12)  /* destination Mask                  */

#define IFC1211_PCIE_MMUDAT_AS_INV                   -1
#define IFC1211_PCIE_MMUDAT_AM_MASK                0xff
static const int IFC1211_PCIE_MMUDAT_AM[0x40] = {
                 IFC1211_PCIE_MMUDAT_AS_INV, /* 0x00 */
                 IFC1211_PCIE_MMUDAT_AS_INV, /* 0x01 */
                 IFC1211_PCIE_MMUDAT_AS_INV, /* 0x02 */
                 IFC1211_PCIE_MMUDAT_AS_INV, /* 0x03 */
                 IFC1211_PCIE_MMUDAT_AS_INV, /* 0x04 */
                 IFC1211_PCIE_MMUDAT_AS_INV, /* 0x05 */
                 IFC1211_PCIE_MMUDAT_AS_INV, /* 0x06 */
                 IFC1211_PCIE_MMUDAT_AS_INV, /* 0x07 */
                 IFC1211_PCIE_MMUDAT_AS_A32_MBLT,                                                         /* 0x08 */
                 IFC1211_PCIE_MMUDAT_AS_A32,                                                              /* 0x09 */
                 IFC1211_PCIE_MMUDAT_AS_A32 | IFC1211_PCIE_MMUDAT_PROG,                                /* 0x0a */
                 IFC1211_PCIE_MMUDAT_AS_A32_BLT,                                                          /* 0x0b */
                 IFC1211_PCIE_MMUDAT_AS_A32_MBLT | IFC1211_PCIE_MMUDAT_SUPER,                          /* 0x0c */
                 IFC1211_PCIE_MMUDAT_AS_A32 | IFC1211_PCIE_MMUDAT_SUPER,                               /* 0x0d */
                 IFC1211_PCIE_MMUDAT_AS_A32 | IFC1211_PCIE_MMUDAT_SUPER | IFC1211_PCIE_MMUDAT_PROG, /* 0x0e */
                 IFC1211_PCIE_MMUDAT_AS_A32_BLT | IFC1211_PCIE_MMUDAT_SUPER,                           /* 0x0f */
                 IFC1211_PCIE_MMUDAT_AS_USER,                                                             /* 0x10 */
                 IFC1211_PCIE_MMUDAT_AS_INV,                                                              /* 0x11 */
                 IFC1211_PCIE_MMUDAT_AS_USER | IFC1211_PCIE_MMUDAT_SUPER,                              /* 0x12 */
                 IFC1211_PCIE_MMUDAT_AS_INV,                                                              /* 0x13 */
                 IFC1211_PCIE_MMUDAT_AS_USER,                                                             /* 0x14 */
                 IFC1211_PCIE_MMUDAT_AS_INV,                                                              /* 0x15 */
                 IFC1211_PCIE_MMUDAT_AS_USER | IFC1211_PCIE_MMUDAT_SUPER,                              /* 0x16 */
                 IFC1211_PCIE_MMUDAT_AS_INV,                                                              /* 0x17 */
                 IFC1211_PCIE_MMUDAT_AS_USER,                                                             /* 0x18 */
                 IFC1211_PCIE_MMUDAT_AS_INV,                                                              /* 0x19 */
                 IFC1211_PCIE_MMUDAT_AS_USER | IFC1211_PCIE_MMUDAT_SUPER,                              /* 0x1a */
                 IFC1211_PCIE_MMUDAT_AS_INV,                                                              /* 0x1b */
                 IFC1211_PCIE_MMUDAT_AS_USER,                                                             /* 0x1c */
                 IFC1211_PCIE_MMUDAT_AS_INV,                                                              /* 0x1d */
                 IFC1211_PCIE_MMUDAT_AS_USER | IFC1211_PCIE_MMUDAT_SUPER,                              /* 0x1e */
                 IFC1211_PCIE_MMUDAT_AS_INV,                                                              /* 0x1f */
                 IFC1211_PCIE_MMUDAT_AS_INV,                                                              /* 0x20 */
                 IFC1211_PCIE_MMUDAT_AS_INV,                                                              /* 0x21 */
                 IFC1211_PCIE_MMUDAT_AS_INV,                                                              /* 0x22 */
                 IFC1211_PCIE_MMUDAT_AS_INV,                                                              /* 0x23 */
                 IFC1211_PCIE_MMUDAT_AS_INV,                                                              /* 0x24 */
                 IFC1211_PCIE_MMUDAT_AS_INV,                                                              /* 0x25 */
                 IFC1211_PCIE_MMUDAT_AS_INV,                                                              /* 0x26 */
                 IFC1211_PCIE_MMUDAT_AS_INV,                                                              /* 0x27 */
                 IFC1211_PCIE_MMUDAT_AS_INV,                                                              /* 0x28 */
                 IFC1211_PCIE_MMUDAT_AS_A16,                                                              /* 0x29 */
                 IFC1211_PCIE_MMUDAT_AS_INV,                                                              /* 0x2a */
                 IFC1211_PCIE_MMUDAT_AS_INV,                                                              /* 0x2b */
                 IFC1211_PCIE_MMUDAT_AS_INV,                                                              /* 0x2c */
                 IFC1211_PCIE_MMUDAT_AS_A16|IFC1211_PCIE_MMUDAT_SUPER,                                 /* 0x2d */
                 IFC1211_PCIE_MMUDAT_AS_INV,                                                              /* 0x2e */
                 IFC1211_PCIE_MMUDAT_AS_CRCSR,                                                            /* 0x2f */
                 IFC1211_PCIE_MMUDAT_AS_INV, /* 0x30 */
                 IFC1211_PCIE_MMUDAT_AS_INV, /* 0x31 */
                 IFC1211_PCIE_MMUDAT_AS_INV, /* 0x32 */
                 IFC1211_PCIE_MMUDAT_AS_INV, /* 0x33 */
                 IFC1211_PCIE_MMUDAT_AS_INV, /* 0x34 */
                 IFC1211_PCIE_MMUDAT_AS_INV, /* 0x35 */
                 IFC1211_PCIE_MMUDAT_AS_INV, /* 0x36 */
                 IFC1211_PCIE_MMUDAT_AS_INV, /* 0x37 */
                 IFC1211_PCIE_MMUDAT_AS_INV, /* 0x38 */
                 IFC1211_PCIE_MMUDAT_AS_A24,                                                              /* 0x39 */
                 IFC1211_PCIE_MMUDAT_AS_A24| IFC1211_PCIE_MMUDAT_PROG,                                 /* 0x3a */
                 IFC1211_PCIE_MMUDAT_AS_INV,                                                              /* 0x3b */
                 IFC1211_PCIE_MMUDAT_AS_INV,                                                              /* 0x3c */
                 IFC1211_PCIE_MMUDAT_AS_A24 | IFC1211_PCIE_MMUDAT_SUPER,                               /* 0x3d */
                 IFC1211_PCIE_MMUDAT_AS_A24 | IFC1211_PCIE_MMUDAT_SUPER | IFC1211_PCIE_MMUDAT_PROG, /* 0x3e */
                 IFC1211_PCIE_MMUDAT_AS_IACK
};                                                                                                           /* 0x3f */

/*
 *  PCIe End Point register address pointer
 */
#define IFC1211_A7_PCIEP_ADDPT_DRP           0x00000
#define IFC1211_A7_PCIEP_ADDPT_CFG           0x10000


/*
 *  I2C controller registers (CSR + $100/104/108/10c)
 */
#define IFC1211_I2C_CTL_ADD(a)      ((a&0x7f) | ((a&0x380) << 1))   /* set I2C device address  */
#define IFC1211_I2C_CTL_BTO_FL                            (1<<13)   /* Bus timeout flag         */
#define IFC1211_I2C_CTL_KILL                              (1<<14)   /* kill current transaction */
#define IFC1211_I2C_CTL_STR_REPEAT                        (1<<15)   /* restart transaction      */
#define IFC1211_I2C_CTL_CMDSIZ(x)                 (((x-1)&3)<<16)   /* command size (1-4)       */
#define IFC1211_I2C_CTL_DATSIZ(x)                 (((x-1)&3)<<18)   /* data size (1-4)          */
#define IFC1211_I2C_CTL_EXEC_IDLE                         (0<<20)
#define IFC1211_I2C_CTL_EXEC_RUN                          (1<<20)
#define IFC1211_I2C_CTL_EXEC_DONE                         (2<<20)
#define IFC1211_I2C_CTL_EXEC_ERR                          (3<<20)
#define IFC1211_I2C_CTL_EXEC_MASK                         (3<<20)
#define IFC1211_I2C_CTL_TRIG_CMD                          (1<<22)
#define IFC1211_I2C_CTL_TRIG_DATW                         (2<<22)
#define IFC1211_I2C_CTL_TRIG_DATR                         (3<<22)
#define IFC1211_I2C_CTL_TRIG_MASK                         (3<<22)
#define IFC1211_I2C_CTL_SPEED_100k                        (0<<24)
#define IFC1211_I2C_CTL_SPEED_400k                        (1<<24)
#define IFC1211_I2C_CTL_SPEED_1M                          (2<<24)
#define IFC1211_I2C_CTL_PORT(port)               ((port&0x7)<<29)

/*
 *  Interrupt Controller Control and Status(CSR + $84/$484/$884/$c84)
 */
#define IFC1211_ALL_ITC_CSR_GLENA           (1<<0)   /* Interrupt Controller global enable */
#define IFC1211_ALL_ITC_CSR_CLEARIP         (1<<1)   /* Clear all pending interrupts       */
#define IFC1211_PVME_ITC_CSR_AUTOIACK       (1<<2)   /* Enable VME auto IACK mechanism     */

#define IFC1211_PVME_ITC_IACK_VEC(iack)            (iack&0xff)  /* extract vector from iack       */
#define IFC1211_ALL_ITC_IACK_SRC(iack)         ((iack>>8)&0x3f)  /* extract source from iack      */
#define IFC1211_ALL_ITC_IACK_CTL(iack)       ((iack>>112)&0x3)  /* extract controller from iack  */

/*
 *  Interrupt Controller Mask Set/Clear(CSR + $84/$484/$884/$c84)
 */
#define IFC1211_ALL_ITC_MASK_ALL            0xffff   /* Mask for all interrupt sources     */
#define IFC1211_ALL_ITC_IM( src)    (1<<(src&0xf))   /* get interrupt mask from source     */

/*
 *  VME Slot 1 & General Control and Status(CSR + $400)
 */
#define IFC1211_PVME_SLOT1_ARB_PRI                  0   /* ARB mode : Not pipelined PRI Mode      */
#define IFC1211_PVME_SLOT1_ARB_RRS                  1   /* ARB mode : Not pipelined RRS Mode      */
#define IFC1211_PVME_SLOT1_ARB_PRI_PP               2   /* ARB mode : pipelined PRI Mode          */
#define IFC1211_PVME_SLOT1_ARB_RRS_PP               3   /* ARB mode : pipelined RRS Mode          */
#define IFC1211_PVME_SLOT1_ARB_MASK                 3   /* ARB mode : mask                        */
#define IFC1211_PVME_SLOT1_ARB_TMO_FL           (1<<2)  /* ARB time out flag                      */
#define IFC1211_PVME_SLOT1_ENA                  (1<<3)  /* VME slot 1 Enable                      */
#define IFC1211_PVME_SLOT1_REQ_TMO_MODE         (1<<4)  /* Arbitration Request monitoring mode    */
#define IFC1211_PVME_SLOT1_REQ_TMO_FL           (1<<5)  /* Arbitration Requester monitoringflag   */
#define IFC1211_PVME_SLOT1_BBSY_NOFILT          (1<<6)  /* Disable BBSY Filtering                 */
#define IFC1211_PVME_SLOT1_BCLR_DIS             (1<<7)  /* Disable BCLR generatiom                */
#define IFC1211_PVME_SLOT1_BTO_16US             (0<<8)  /* set Bus Timeout to 16 usec             */
#define IFC1211_PVME_SLOT1_BTO_32US             (1<<8)  /* set Bus Timeout to 32 usec             */
#define IFC1211_PVME_SLOT1_BTO_128US            (2<<8)  /* set Bus Timeout to 128 usec            */
#define IFC1211_PVME_SLOT1_BTO_256US            (3<<8)  /* set Bus Timeout to 256 usec            */
#define IFC1211_PVME_SLOT1_BTO_MSK              (3<<8)  /* set Bus Timeout to 256 usec            */
#define IFC1211_PVME_SLOT1_BTO_FL              (1<<10)  /* Bus Timeout status flag                */
#define IFC1211_PVME_SLOT1_TX_BBSY             (1<<15)  /* Assert Bus busy                        */
#define IFC1211_PVME_SLOT1_RX_BBSY             (1<<15)  /* Detect Bus busy                        */
#define IFC1211_PVME_SLOT1_SYSRST              (1<<15)  /* Generate a 200msec VME sys reset       */
#define IFC1211_PVME_SLOT1_GEO_SHIFT                16  /* Bit shift for Geographic address       */
#define IFC1211_PVME_SLOT1_GEO_MASK         (0xff<<16)  /* Bit mask for Geographic address        */
#define IFC1211_PVME_SLOT1_VME64X_SW           (1<<24)  /* VME64X switch status                   */
#define IFC1211_PVME_SLOT1_SLOT1_SW            (1<<25)  /* SLOT1 switch status                    */
#define IFC1211_PVME_SLOT1_SYSRST_SW           (1<<26)  /* SYS Reset propagation switch status    */
#define IFC1211_PVME_SLOT1_AUTOID_ENA          (1<<27)  /* AUTIID switch status                   */
#define IFC1211_PVME_SLOT1_AUTOID_IDLE         (0<<28)  /* AUTIID idle state or disabled          */
#define IFC1211_PVME_SLOT1_AUTOID_IRQ2         (1<<28)  /* AUTIID IRQ2 pending (waiting IACK)     */
#define IFC1211_PVME_SLOT1_AUTOID_WAIT         (2<<28)  /* AUTIID waiting CRCSR update            */
#define IFC1211_PVME_SLOT1_AUTOID_DONE         (3<<28)  /* AUTIID completed                       */
#define IFC1211_PVME_SLOT1_SYSFAIL             (3<<30)  /* VME64X SYSFAIL status                  */
#define IFC1211_PVME_SLOT1_AUTOID_SEMI         (3<<31)  /* AUTIID semi automatic mode             */
#define IFC1211_PVME_SLOT1_ARB_SET(x)            (x&3)  /* set ARB mode in csr                    */
#define IFC1211_PVME_SLOT1_ARB_GET(x)            (x&3)  /* get ARB mode from csr                  */
#define IFC1211_PVME_SLOT1_BTO_SET(x)        ((x&7)<<8) /* set Bus Timeout to x in csr            */
#define IFC1211_PVME_SLOT1_BTO_GET(x) (0x10<<((x>>8)&7))/* get Bus Timeout to csr                 */

/*
 *  VME master & General Control and Status(CSR + $404)
 */
#define IFC1211_PVME_MASCSR_REQMOD_RWD            0  /* ARB Request mode Release When Done     */
#define IFC1211_PVME_MASCSR_REQMOD_ROR            1  /* ARB Request mode Release When Done     */
#define IFC1211_PVME_MASCSR_REQMOD_FAIR           2  /* ARB Request mode Release When Done     */
#define IFC1211_PVME_MASCSR_REQMOD_NOREL          3  /* ARB Request mode Release When Done     */
#define IFC1211_PVME_MASCSR_REQMOD_MASK           3  /* mask for ARB Request mode              */
#define IFC1211_PVME_MASCSR_REQLEV_0         (0<<2)  /* ARB Request level 0                    */
#define IFC1211_PVME_MASCSR_REQLEV_1         (1<<2)  /* ARB Request level 1                    */
#define IFC1211_PVME_MASCSR_REQLEV_2         (2<<2)  /* ARB Request level 2                    */
#define IFC1211_PVME_MASCSR_REQLEV_3         (3<<2)  /* ARB Request level 3                    */
#define IFC1211_PVME_MASCSR_REQLEV_MASK      (3<<2)  /* mask ARB Request level                 */
#define IFC1211_PVME_MASCSR_BACKOFF          (1<<4)  /* Retry acknowledge force bus release    */
#define IFC1211_PVME_MASCSR_SUPER            (1<<5)  /* VME AM assign to supervisor            */
#define IFC1211_PVME_MASCSR_MAXRETRY         (1<<6)  /* limit number of retry to 1024          */
#define IFC1211_PVME_MASCSR_READERR_DIS      (1<<7)  /* VME Read BERR -> PCIe status OK        */
#define IFC1211_PVME_MASCSR_REJ_FL           (1<<8)  /* VME cycle rejected due to MAS disabled */
#define IFC1211_PVME_MASCSR_HW16_SW          (1<<9)  /* D16 swapping enable                    */
#define IFC1211_PVME_MASCSR_IACK_DW         (1<<10)  /* VME IACK DW status_ID                  */
#define IFC1211_PVME_MASCSR_READ_SMPL_0     (0<<12)  /* Read sampling point +0ns               */
#define IFC1211_PVME_MASCSR_READ_SMPL_6     (1<<12)  /* Read sampling point +6ns               */
#define IFC1211_PVME_MASCSR_READ_SMPL_12    (2<<12)  /* Read sampling point +12ns              */
#define IFC1211_PVME_MASCSR_READ_SMPL_18    (3<<12)  /* Read sampling point +18ns              */
#define IFC1211_PVME_MASCSR_RMW_ENA         (1<<22)  /* Enable Read Modify Write               */
#define IFC1211_PVME_MASCSR_VTON(x)     ((x-2)<<25)  /* VME Master Timer ON                    */
#define IFC1211_PVME_MASCSR_VTON_MASK       (7<<25)  /* VME Master Timer ON                    */
#define IFC1211_PVME_MASCSR_VTOFF(x)        (x<<28)  /* VME Master Timer OFF                   */
#define IFC1211_PVME_MASCSR_VTOFF_MASK      (7<<28)  /* VME Master Timer OFF                   */
#define IFC1211_PVME_MASCSR_ENA             (1<<31)  /* Enable Master port                     */

#define IFC1211_PVME_MASCSR_REQMOD_GET(x)       (x&3)  /* get ARB Request mode                  */
#define IFC1211_PVME_MASCSR_REQMOD_SET(x)       (x&3)  /* set ARB Request mode                  */
#define IFC1211_PVME_MASCSR_REQLEV_GET(x)  ((x>>2)&3)  /* get ARB Request level                 */
#define IFC1211_PVME_MASCSR_REQLEV_SET(x)  ((x&3)<<2)  /* set ARB Request level                 */

/*
 *  VME slave A32 Port Control and Status(CSR + $408)
 */
#define IFC1211_PVME_SLVCSR_SIZE_MASK     (0xf<<0)  /* Mask for window size bitfield      */
#define IFC1211_PVME_SLVCSR_NORETRY         (1<<4)  /* Don't perform slave retry (legacy) */
#define IFC1211_PVME_SLVCSR_WP_1k           (0<<6)  /* Write postion burst size 1k        */
#define IFC1211_PVME_SLVCSR_WP_512          (1<<6)  /* Write postion burst size 512       */
#define IFC1211_PVME_SLVCSR_WP_256          (2<<6)  /* Write postion burst size 256       */
#define IFC1211_PVME_SLVCSR_WP_128          (3<<6)  /* Write postion burst size 128       */
#define IFC1211_PVME_SLVCSR_RRSP_INV        (1<<8)  /* Read Response Invalid              */
#define IFC1211_PVME_SLVCSR_RRSP_UEX        (1<<9)  /* Read Response Unexpected           */
#define IFC1211_PVME_SLVCSR_RRSP_WA        (1<<10)  /* Read Response Wrong Acknowledge    */
#define IFC1211_PVME_SLVCSR_RRSP_OOD       (1<<11)  /* Read Response Out Of Date          */
#define IFC1211_PVME_SLVCSR_ROT_MASK    (0xff<<16)  /* Mask for rotary encoder            */
#define IFC1211_PVME_SLVCSR_AUTO_ADER      (1<<24)  /* use Rotary encode to set vme base  */
#define IFC1211_PVME_SLVCSR_ENA            (1<<31)  /* Enable Master port      */

#define IFC1211_PVME_SLVCSR_SIZE(x)    ((x&8)?(0x100000<<(x&7)):(0x1000000<<(x&7))) /*VME A32 slave size */
#define IFC1211_PVME_SLVCSR_A32_PG        0x100000  /* Page size 1 MByte                 */
#define IFC1211_PVME_SLVCSR_A32_SIZE_MAX  0x80000000  /* Max slave size 4*512 MBytes       */

/*
 *  VME Interrupt Generator(CSR + $40c)
 */
#define IFC1211_PVME_INTG_STATUS_MASK     (0xff<<0)   /* Mask for interrupt vector     */
#define IFC1211_PVME_INTG_LEVEL_MASK       (0x7<<8)   /* Mask for interrupt level      */
#define IFC1211_PVME_INTG_LEVEL_1          (0x1<<8)   /* Interrupt level 1             */
#define IFC1211_PVME_INTG_LEVEL_2          (0x2<<8)   /* Interrupt level 2             */
#define IFC1211_PVME_INTG_LEVEL_3          (0x3<<8)   /* Interrupt level 3             */
#define IFC1211_PVME_INTG_LEVEL_4          (0x4<<8)   /* Interrupt level 4             */
#define IFC1211_PVME_INTG_LEVEL_5          (0x5<<8)   /* Interrupt level 5             */
#define IFC1211_PVME_INTG_LEVEL_6          (0x6<<8)   /* Interrupt level 6             */
#define IFC1211_PVME_INTG_LEVEL_7          (0x7<<8)   /* Interrupt level 7             */
#define IFC1211_PVME_INTG_IP               (0xf<<11)  /* Interrupt pending             */
#define IFC1211_PVME_INTG_CMD_SET          (0x1<<12)  /* Set VME interrupt             */
#define IFC1211_PVME_INTG_CMD_CLEAR        (0x2<<12)  /* Clear VME interrupt           */
#define IFC1211_PVME_INTG_CMD_ID1          (0x9<<12)  /* Init Status-ID[15:8]          */
#define IFC1211_PVME_INTG_CMD_ID2          (0xa<<12)  /* Init Status-ID[15:8]          */
#define IFC1211_PVME_INTG_CMD_ID3          (0xb<<12)  /* Init Status-ID[15:8]          */
#define IFC1211_PVME_INTG_CMD_ID3          (0xb<<12)  /* Init Status-ID[15:8]          */
#define IFC1211_PVME_INTG_RORA               (1<<22)  /* Interrupt Aknowledge RORA     */
#define IFC1211_PVME_INTG_INTPD_MASK      (0xfe<<25)  /* Mask for interrupt pending    */
#define IFC1211_PVME_INTG_STATUS_SET(x)     (x&0xff)  /* Set Interrupt vector x        */
#define IFC1211_PVME_INTG_STATUS_GET(x)     (x&0xff)  /* Set Interrupt vector x        */
#define IFC1211_PVME_INTG_LEVEL_SET(x)    ((x&7)<<8)  /* Set Interrupt level x         */
#define IFC1211_PVME_INTG_LEVEL_GET(x)    ((x>>8)&7)  /* Get Interrupt level x         */
#define IFC1211_PVME_INTG_INTPD_GET(x)((x&0xfe)>>25)  /* get interrupt pending status  */

static const int IFC1211_PVME_INTG_LEVEL[8] = { 0, IFC1211_PVME_INTG_LEVEL_1,
			IFC1211_PVME_INTG_LEVEL_2, IFC1211_PVME_INTG_LEVEL_3,
			IFC1211_PVME_INTG_LEVEL_4, IFC1211_PVME_INTG_LEVEL_5,
			IFC1211_PVME_INTG_LEVEL_6, IFC1211_PVME_INTG_LEVEL_7 };
/*
 *  VME Slave MMU parameters
 */      
#define IFC1211_PVME_MMU_A32_PG_NUM                0x200  /* 512 MMU pages for VME A32 Slave            */
#define IFC1211_PVME_MMU_A32_PG_SIZE            0x100000  /* 1 MBytes page size for VME for A32 Slave   */
#define IFC1211_PVME_MMU_A24_PG_NUM                0x200  /* 512 MMU pages for VME A24 Slave            */
#define IFC1211_PVME_MMU_A24_PG_SIZE              0x1000  /* 4 KBytes page size for VME for A24 Slave   */
#define IFC1211_PVME_MMU_A16_PG_NUM                0x200  /* 512 MMU pages for VME A16 Slave            */
#define IFC1211_PVME_MMU_A16_PG_SIZE               0x100  /* 256 Bytes page size for VME for A16 Slave  */
#define IFC1211_PVME_MMU_2eBRC_PG_NUM              0x200  /* 512 MMU pages for VME 2eBRC Slave          */
#define IFC1211_PVME_MMU_2eBRC_PG_SIZE          0x400000  /* 4 MBytes page size for VME for 2eBRC Slave */

/*
 *  VME Slave MMU address pointer(CSR + $410)
 */
#define IFC1211_PVME_MMUADD_PTR_MASK               (0xff8<<0)  /* Mask for MMU pointer (512 pages) */
#define IFC1211_PVME_MMUADD_A32_SEL                   (0<<16)  /* Select MMU for A32               */
#define IFC1211_PVME_MMUADD_A24_SEL                   (1<<16)  /* Select MMU for A32               */
#define IFC1211_PVME_MMUADD_A16_SEL                   (2<<16)  /* Select MMU for A32               */
#define IFC1211_PVME_MMUADD_2eBRC_SEL                 (3<<16)  /* Select MMU for BCR               */

/*
 *  VME Slave MMU data register(CSR + $410)
 */
#define IFC1211_PVME_MMUDAT_PG_ENA               (1<<0)  /* Enable page               */
#define IFC1211_PVME_MMUDAT_WR_ENA               (1<<1)  /* Enable write              */
#define IFC1211_PVME_MMUDAT_TC_MASK              (3<<2)  /* Mask for traffic class    */
#define IFC1211_PVME_MMUDAT_SP                   (1<<4)  /* Accept only spervisory AM */
#define IFC1211_PVME_MMUDAT_SWAP_AUTO            (1<<6)  /* set auto swap be <-> le   */
#define IFC1211_PVME_MMUDAT_SWAP_DW              (2<<6)  /* set double word swap      */
#define IFC1211_PVME_MMUDAT_SWAP_MASK            (3<<6)  /* set double word swap      */
#define IFC1211_PVME_MMUDAT_DES_PCIE            (0<<12)  /* destination PCI Express   */
#define IFC1211_PVME_MMUDAT_DES_SHM             (2<<12)  /* destination Shared Memory */
#define IFC1211_PVME_MMUDAT_DES_USR             (3<<12)  /* destination User Block    */
#define IFC1211_PVME_MMUDAT_DES_MASK            (f<<12)  /* destination User Block    */
#define IFC1211_PVME_MMUDAT_RPF_32               (0<<0)  /* Read Prefetch 32 bytes    */
#define IFC1211_PVME_MMUDAT_RPF_64               (1<<0)  /* Read Prefetch 64 bytes    */
#define IFC1211_PVME_MMUDAT_RPF_128              (2<<0)  /* Read Prefetch 128 bytes   */
#define IFC1211_PVME_MMUDAT_RPF_256              (3<<0)  /* Read Prefetch 256 bytes   */
#define IFC1211_PVME_MMUDAT_RPF_MASK             (3<<0)  /* Read Prefetch mask        */
#define IFC1211_PVME_MMUDAT_SNOOP_NO             (1<<2)  /* No snoop                  */
#define IFC1211_PVME_MMUDAT_RELAX                (1<<3)  /* Relax Ordering            */
#define IFC1211_PVME_MMUDAT_ADD_MASK         (0xfff<<4)  /* destination address mask  */

/*
 *  VME Master Error Status (CSR + $420)
 */
#define IFC1211_PVME_STAERR_BTO                 (1<<29)   /* VME master error Bus Timeout       */
#define IFC1211_PVME_STAERR_WR                  (1<<30)   /* VME master error write             */
#define IFC1211_PVME_STAERR_VEOF                (1<<31)   /* VME master error overflow          */

/*
 *  VME Location Monitor (CSR + $420)
 */
#define IFC1211_PVME_LOCMON_AS_MASK                (3<<0)  /*  Location Monitor Mask                  */
#define IFC1211_PVME_LOCMON_AS_A16                 (1<<0)  /*  Enable Location Monitor A16            */
#define IFC1211_PVME_LOCMON_AS_A24                 (2<<0)  /*  Enable Location Monitor A24            */
#define IFC1211_PVME_LOCMON_AS_A32                 (3<<0)  /*  Enable Location Monitor A32            */
#define IFC1211_PVME_LOCMON_ADDR_MASK     (0xffffffe0<<0)  /*  Location Monitor Address Masl          */

/*
 *  VME Global Timer (CSR + $440)
 */
#define IFC1211_PVME_GLTIM_1MHZ         (0<<0)  /* timer frequency 1 MHz                  */
#define IFC1211_PVME_GLTIM_5MHZ         (1<<0)  /* timer frequency 5 MHz                  */
#define IFC1211_PVME_GLTIM_25MHZ        (2<<0)  /* timer frequency 25 MHz                 */
#define IFC1211_PVME_GLTIM_100MHZ       (3<<0)  /* timer frequency 100 MHz                */
#define IFC1211_PVME_GLTIM_BASE_1000    (0<<3)  /* timer period 1000 usec                 */
#define IFC1211_PVME_GLTIM_BASE_1024    (1<<3)  /* timer period 1024 usec                 */
#define IFC1211_PVME_GLTIM_SYNC_LOC     (0<<4)  /* timer synchronization local            */
#define IFC1211_PVME_GLTIM_SYNC_USR1    (1<<4)  /* timer synchronization user signal #1   */
#define IFC1211_PVME_GLTIM_SYNC_USR2    (2<<4)  /* timer synchronization user signal #2   */
#define IFC1211_PVME_GLTIM_SYNC_SYSFAIL (4<<4)  /* timer synchronization VME sysfail      */
#define IFC1211_PVME_GLTIM_SYNC_IRQ1    (5<<4)  /* timer synchronization VME IRQ#1        */
#define IFC1211_PVME_GLTIM_SYNC_IRQ2    (6<<4)  /* timer synchronization VME IRQ#2        */
#define IFC1211_PVME_GLTIM_SYNC_ENA     (1<<7)  /* timer synchronization enable           */
#define IFC1211_PVME_GLTIM_OUT_SYSFAIL  (1<<8)  /* issue sync signal on VME sysfail       */
#define IFC1211_PVME_GLTIM_OUT_IRQ1     (2<<8)  /* issue sync signal on VME IRQ#1         */
#define IFC1211_PVME_GLTIM_OUT_IRQ2     (3<<8)  /* issue sync signal on VME IRQ#2         */
#define IFC1211_PVME_GLTIM_SYNC_ERR    (1<<16)  /* timer synchronization error           */
#define IFC1211_PVME_GLTIM_ENA         (1<<31)  /* timer global enable                    */


/*
 *  VME slave A24 Port Control and Status(CSR + $450)
 */
#define IFC1211_PVME_SLVCSR_A24_SIZE_MASK      (0x7<<0)  /* Mask for window size bitfield      */
#define IFC1211_PVME_SLVCSR_A24_ENA             (1<<31)  /* Enable Master port      */
#define IFC1211_PVME_SLVCSR_A24_PG               0x1000  /* granularity     */
#define IFC1211_PVME_SLVCSR_A24_SIZE(x)  (0x4000<<(x&7)) /*VME A24 slave size */
#define IFC1211_PVME_SLVCSR_A24_SIZE_MAX     (0x4000<<7) /*VME A24 msx slave size */


/*
 *  VME slave A16 Port Control and Status(CSR + $454)
 */
#define IFC1211_PVME_SLVCSR_A16_SIZE_MASK      (0x7<<0)  /* Mask for window size bitfield      */
#define IFC1211_PVME_SLVCSR_A16_ENA             (1<<31)  /* Enable Master port      */
#define IFC1211_PVME_SLVCSR_A16_PG                0x100  /* granularity     */
#define IFC1211_PVME_SLVCSR_A16_SIZE(x)  (0x100<<(x&7)) /*VME A16 slave size */
#define IFC1211_PVME_SLVCSR_A16_SIZE_MAX     (0x100<<7) /*VME A16 slave size */

/*
 *  VME slave 2eBRC Port Control and Status(CSR + $458)
 */
#define IFC1211_PVME_SLVCSR_2eBRC_SIZE_MASK         (0x7<<0)  /* Mask for window size bitfield      */
#define IFC1211_PVME_SLVCSR_2eBRC_ENA                (1<<31)  /* Enable Master port      */
#define IFC1211_PVME_SLVCSR_2eBRC_PG                0x400000  /* granularity     */
#define IFC1211_PVME_SLVCSR_2eBRC_SIZE(x)  (0x1000000<<(x&7)) /*VME 2eBRC slave size */
#define IFC1211_PVME_SLVCSR_2eBRC_SIZE_MAX     (0x1000000<<7) /*VME 2eBRC slave size */


/*
 *  VME interrupt IACK (CSR + $480)
 */

/*
 *  VME interrupt CSR (CSR + $484)
 */
#define IFC1211_PVME_ITC_CSR_             0x484


/*
 *  VME interrupt MASK (CSR + $488/C)
 */
#define IFC1211_PVME_ITC_IM_SYSFAIL             (1<<0)  /*  VME SYSFAIL interrupt              */
#define IFC1211_PVME_ITC_IM_IRQ1                (1<<1)  /*  VME IRQ1 interrupt                 */
#define IFC1211_PVME_ITC_IM_IRQ2                (1<<2)  /*  VME IRQ2 interrupt                 */
#define IFC1211_PVME_ITC_IM_IRQ3                (1<<3)  /*  VME IRQ3 interrupt                 */
#define IFC1211_PVME_ITC_IM_IRQ4                (1<<4)  /*  VME IRQ4 interrupt                 */
#define IFC1211_PVME_ITC_IM_IRQ5                (1<<5)  /*  VME IRQ5 interrupt                 */
#define IFC1211_PVME_ITC_IM_IRQ6                (1<<6)  /*  VME IRQ6 interrupt                 */
#define IFC1211_PVME_ITC_IM_IRQ7                (1<<7)  /*  VME IRQ7 interrupt                 */
#define IFC1211_PVME_ITC_IM_ACFAIL              (1<<8)  /*  VME ACFAIL interrupt               */
#define IFC1211_PVME_ITC_IM_MASERR              (1<<9)  /*  VME Master Error interrupt         */
#define IFC1211_PVME_ITC_IM_GTIM0              (1<<10)  /*  VME Global Time interrupt 0        */
#define IFC1211_PVME_ITC_IM_GTIM1              (1<<11)  /*  VME Global Time interrupt 1        */
#define IFC1211_PVME_ITC_IM_LM0                (1<<12)  /*  VME Location Monitor 0 interrupt   */
#define IFC1211_PVME_ITC_IM_LM1                (1<<13)  /*  VME Location Monitor 1 interrupt   */
#define IFC1211_PVME_ITC_IM_LM2                (1<<14)  /*  VME Location Monitor 2 interrupt   */
#define IFC1211_PVME_ITC_IM_LM3                (1<<15)  /*  VME Location Monitor 3 interrupt   */
#define IFC1211_PVME_ITC_IM_LM_ALL           (0xf<<12)  /*  VME Location Monitor all interrupt */
#define IFC1211_PVME_ITC_IM_ALL                 0xffff  /*  VME All interrupts                 */

static const int IFC1211_PVME_ITC_IM_LM[4] = { IFC1211_PVME_ITC_IM_LM0,
						  IFC1211_PVME_ITC_IM_LM1,
						  IFC1211_PVME_ITC_IM_LM2,
						  IFC1211_PVME_ITC_IM_LM3};

static const int IFC1211_PVME_ITC_IM_IRQ[7] = { IFC1211_PVME_ITC_IM_IRQ1,
						   IFC1211_PVME_ITC_IM_IRQ2,
						   IFC1211_PVME_ITC_IM_IRQ3,
						   IFC1211_PVME_ITC_IM_IRQ4,
						   IFC1211_PVME_ITC_IM_IRQ5,
						   IFC1211_PVME_ITC_IM_IRQ6,
						   IFC1211_PVME_ITC_IM_IRQ7};


/*
 *  VME CRCSR BCR/BSR (CSR + $5F4/8)
 */
#define IFC1211_CRCSR_RESET             (1<<0)  /* Pull/Release VME64x reset       */
#define IFC1211_CRCSR_SYSFAIL_ENA       (1<<1)  /* Enable/Disable VME64x SYSFAIL   */
#define IFC1211_CRCSR_SYSFAIL           (1<<2)  /* Set/clear VME64x SYSFAIL        */
#define IFC1211_CRCSR_BERR              (1<<3)  /* Set/clear VME64x BERR flag      */
#define IFC1211_CRCSR_SLV_ENA           (1<<4)  /* enable/disable VME64x slave     */



/*
 *  DDR3 CSR (CSR + $800/C)
 */
#define IFC1211_SMEM_DDR3_SIZE(x) ((x&0xc)?(0x8000000<<((x>>2)&3)):0)  /* calculate SMEM DDR3 size            */

/*
 *  SRAM CSR (CSR + $810/C)
 */
//#define IFC1211_SMEM_RAM_SIZE(x) ((x&0xc)?(0x10000<<((x>>2)&3)):0)  /* calculate SMEM RAM size            */
#define IFC1211_SMEM_RAM_SIZE(x) (0x10000<<((x>>2)&3))  /* calculate SMEM RAM size            */

/*
 *  IDMA Pipeline control bits (CSR + $850/C)
 */
#define IFC1211_IDMA_PCSR_PIPE_CNT(x)	        (x&0x3f)  /* PIPE counter (read only) */
#define IFC1211_IDMA_PCSR_PIPE_MAX_4 	          (0<<6)  /*  4 ring buffer entries   */
#define IFC1211_IDMA_PCSR_PIPE_MAX_8 	          (1<<6)  /*  8 ring buffer entries   */
#define IFC1211_IDMA_PCSR_PIPE_MAX_16 	          (2<<6)  /* 16 ring buffer entries   */
#define IFC1211_IDMA_PCSR_PIPE_MAX_32 	          (3<<6)  /* 32 ring buffer entries   */
#define IFC1211_IDMA_PCSR_TRIG_IN_RD0 	          (0<<8)  /* Trig by read engine 0    */
#define IFC1211_IDMA_PCSR_TRIG_IN_RD1 	          (1<<8)  /* Trig by read engine 1    */
#define IFC1211_IDMA_PCSR_TRIG_IN_WR0 	          (2<<8)  /* Trig by write engine 0   */
#define IFC1211_IDMA_PCSR_TRIG_IN_WR1 	          (3<<8)  /* Trig by write engine 1   */
#define IFC1211_IDMA_PCSR_PIPE_RESET 	         (1<<13)  /* Reset PIPE               */
#define IFC1211_IDMA_PCSR_PIPE_MODE_1 	         (0<<14)  /* select PIPE mode 1       */
#define IFC1211_IDMA_PCSR_PIPE_MODE_2 	         (1<<14)  /* select PIPE mode 2       */
#define IFC1211_IDMA_PCSR_PIPE_ENABLE 	         (1<<15)  /* enable PIPE mode          */

/*
 *  IDMA interrupt MASK (CSR + $888/C)
 */
#define IFC1211_IDMA_ITC_IM_RD0_END          (1<<0)  /* Read engine #0 ended              */
#define IFC1211_IDMA_ITC_IM_RD0_ERR          (1<<1)  /* Read engine #0 error              */
#define IFC1211_IDMA_ITC_IM_RD0              (3<<0)  /* Read engine #0                    */
#define IFC1211_IDMA_ITC_IM_RD1_END          (1<<2)  /* Read engine #1 ended              */
#define IFC1211_IDMA_ITC_IM_RD1_ERR          (2<<3)  /* Read engine #1 error              */
#define IFC1211_IDMA_ITC_IM_RD1              (3<<2)  /* Read engine #1                    */
#define IFC1211_IDMA_ITC_IM_WR0_END          (1<<4)  /* Write engine #0 ended             */
#define IFC1211_IDMA_ITC_IM_WR0_ERR          (1<<5)  /* Write engine #0 error             */
#define IFC1211_IDMA_ITC_IM_WR0              (3<<4)  /* Write engine #0                   */
#define IFC1211_IDMA_ITC_IM_WR1_END          (1<<6)  /* Write engine #1 ended             */
#define IFC1211_IDMA_ITC_IM_WR1_ERR          (1<<7)  /* Write engine #1 error             */
#define IFC1211_IDMA_ITC_IM_WR1              (3<<6)  /* Write engine #1                   */
#define IFC1211_IDMA_ITC_IM_CHAN0         (0x33<<0)  /* Channel #0 event                  */
#define IFC1211_IDMA_ITC_IM_CHAN1         (0xcc<<0)  /* Channel #1 event                  */

/*
 *  IDMA Control Registers (CSR + $900/$940/$a00/$a40)
 */
#define IFC1211_IDMA_CSR_	               (1<<31)  /* DMA Engine enabled              */
#define IFC1211_IDMA_CSR_SUSPEND	       (1<<28)  /* DMA Engine enabled              */
#define IFC1211_IDMA_CSR_ABORT	       (1<<29)  /* DMA Engine enabled              */
#define IFC1211_IDMA_CSR_KILL	       (1<<30)  /* DMA Engine enabled              */
#define IFC1211_IDMA_CSR_ENA	               (1<<31)  /* DMA Engine enabled              */

#define IFC1211_IDMA_NDES_START_NOW          (0<<0)  /* Start now               */
#define IFC1211_IDMA_NDES_WAIT_TIMER         (1<<0)  /* Wait for timer sync     */
#define IFC1211_IDMA_NDES_WAIT_GPIO_1        (2<<0)  /* Wait for GPIO #1        */
#define IFC1211_IDMA_NDES_WAIT_GPIO_2        (3<<0)  /* Wait for GPIO #2        */
#define IFC1211_IDMA_NDES_WAIT_TRIG_1        (4<<0)  /* Wait for DMA trig #1    */
#define IFC1211_IDMA_NDES_WAIT_TRIG_2        (5<<0)  /* Wait for DMA trig #2    */
#define IFC1211_IDMA_NDES_WAIT_TRIG_3        (6<<0)  /* Wait for DMA trig #3    */
#define IFC1211_IDMA_NDES_WAIT_TRIG_4        (7<<0)  /* Wait for DMA trig #4    */

#define IFC1211_IDMA_DCNT_STATUS  	        (7<<29)  /* DMA Status                   */
#define IFC1211_IDMA_DCNT_IDLE  	        (0<<29)  /* DMA Status IDLE              */
#define IFC1211_IDMA_DCNT_INPROGRESS  	(3<<29)  /* DMA Status in progress       */
#define IFC1211_IDMA_DCNT_ENDED  	        (4<<29)  /* DMA Status ENDED             */

/*
 *  IDMA descriptors
 */
#define IFC1211_IDMA_DES0_                    (0<<0)  /*       */
#define IFC1211_IDMA_DES0_NOINC               (2<<1)  /* No address increment          */
#define IFC1211_IDMA_DES0_DMAERR              (1<<3)  /* Continue with next on error   */
#define IFC1211_IDMA_DES0_SNOOP               (1<<4)  /* Enable snooping (PCIe)        */
#define IFC1211_IDMA_DES0_RELAX               (1<<5)  /* Enable relax ordering (PCIe)  */
#define IFC1211_IDMA_DES0_TRIGOUT_NO         (0<<10)  /* dont't generate trig out      */
#define IFC1211_IDMA_DES0_TRIGOUT_START      (2<<10)  /* generate trig out at start    */
#define IFC1211_IDMA_DES0_TRIGOUT_END        (3<<10)  /* generate trig out at end      */
#define IFC1211_IDMA_DES0_UPDATE_TIME        (2<<12)  /* update status with nsec time  */
#define IFC1211_IDMA_DES0_UPDATE_WCNT        (3<<12)  /* update status with word cnt   */
#define IFC1211_IDMA_DES0_INTR_DIS           (0<<14)  /* Don't generate interrupt      */
#define IFC1211_IDMA_DES0_INTR_START         (1<<14)  /* Generate interrupt at start   */
#define IFC1211_IDMA_DES0_INTR_END           (2<<14)  /* Generate interrupt at end     */
#define IFC1211_IDMA_DES0_INTR_ERR           (3<<14)  /* Generate interrupt on error   */
#define IFC1211_IDMA_DES0_ADDBND_4k          (0<<16)  /* 4 KBytes address boundary     */
#define IFC1211_IDMA_DES0_ADDBND_2k          (1<<16)  /* 2 KBytes address boundary     */
#define IFC1211_IDMA_DES0_ADDBND_1k          (2<<16)  /* 1 KBytes address boundary     */
#define IFC1211_IDMA_DES0_ADDBND_256         (3<<16)  /* 256 Bytes address boundary    */
#define IFC1211_IDMA_DES0_PSIZE_128          (0<<18)  /* 128 Bytes payload size        */
#define IFC1211_IDMA_DES0_PSIZE_256          (1<<18)  /* 256 Bytes payload size        */
#define IFC1211_IDMA_DES0_PSIZE_512          (2<<18)  /* 512 Bytes payload size        */
#define IFC1211_IDMA_DES0_PSIZE_1k           (3<<18)  /* 1 KBytes payload size         */
#define IFC1211_IDMA_DES0_VME_SWAP_AUTO      (1<<26)  /* Automatic swapping            */
#define IFC1211_IDMA_DES0_VME_SWAP_DW        (2<<26)  /* Double word swapping          */
#define IFC1211_IDMA_DES0_VME_SWAP_QW        (3<<26)  /* Quad word swapping            */
#define IFC1211_IDMA_DES0_SHM_SWAP_AUTO      (1<<28)  /* Automatic swapping            */
#define IFC1211_IDMA_DES0_SHM_SWAP_DW        (2<<28)  /* Double word swapping          */
#define IFC1211_IDMA_DES0_SHM_SWAP_QW        (3<<28)  /* Quad word swapping            */

#define IFC1211_IDMA_DES1_WC_MASK       (0xfffffc<<0)  /* word count mask               */
#define IFC1211_IDMA_DES1_DES_PCIe            (0<<24)  /* remote space is PCIe          */
#define IFC1211_IDMA_DES1_DES_VME64x          (1<<24)  /* remote space is VME64x        */
#define IFC1211_IDMA_DES1_DES_SHM2            (2<<24)  /* remote space is SHM #2        */
#define IFC1211_IDMA_DES1_DES_USER            (3<<24)  /* remote space is USER          */
#define IFC1211_IDMA_DES1_AS_CRCSR            (0<<28)  /* remote mode VME configuration */
#define IFC1211_IDMA_DES1_AS_A16              (1<<28)  /* remote mode VME A16           */
#define IFC1211_IDMA_DES1_AS_A24              (2<<28)  /* remote mode VME A24           */
#define IFC1211_IDMA_DES1_AS_A32              (3<<28)  /* remote mode VME A32           */
#define IFC1211_IDMA_DES1_AS_A32_BLT          (4<<28)  /* remote mode VME A32_BLT       */
#define IFC1211_IDMA_DES1_AS_A32_MBLT         (5<<28)  /* remote mode VME A32_MBLT      */
#define IFC1211_IDMA_DES1_AS_A32_2eVME        (6<<28)  /* remote mode VME A32_2eVME     */
#define IFC1211_IDMA_DES1_AS_USER             (7<<28)  /* remote mode VME USER          */
#define IFC1211_IDMA_DES1_AS_A32_2eSST_160    (8<<28)  /* remote mode VME A32_2eSST_160 */
#define IFC1211_IDMA_DES1_AS_A32_2eSST_233    (9<<28)  /* remote mode VME A32_2eSST_233 */
#define IFC1211_IDMA_DES1_AS_A32_2eSST_320   (10<<28)  /* remote mode VME A32_2eSST_320 */
#define IFC1211_IDMA_DES1_AS_A32_2eSST_400   (11<<28)  /* remote mode VME A32_2eSST_400 */
#define IFC1211_IDMA_DES1_AS_A32_2eBCR       (12<<28)  /* remote mode VME A32_2eBCR     */
#define IFC1211_IDMA_DES1_AS_AS_A24_ADO      (13<<28)  /* remote mode VME A24_ADO       */
#define IFC1211_IDMA_DES1_AS_AS_A32_ADO      (14<<28)  /* remote mode VME A32_ADO       */
#define IFC1211_IDMA_DES1_AS_IACK            (15<<28)  /* remote mode VME IACK          */
#define IFC1211_IDMA_DES1_AS_MASK           (0xf<<28)  /* remote mode VME IACK          */

#define IFC1211_IDMA_DES2_BURST_2k             (0<<0)  /* burst size 2 KBytes           */
#define IFC1211_IDMA_DES2_BURST_1k             (1<<0)  /* burst size 1 KBytes           */
#define IFC1211_IDMA_DES2_BURST_512            (2<<0)  /* burst size 512 Bytes          */
#define IFC1211_IDMA_DES2_BURST_256            (3<<0)  /* burst size 256 Bytes          */
#define IFC1211_IDMA_DES2_ADDR_MASK   (0xfffffffc<<0)  /* SHM address mask              */

#define IFC1211_IDMA_DES3_START_NOW             (0<<0)  /* Start now       */
#define IFC1211_IDMA_DES3_START_GTS             (1<<0)  /* Wait Global Time Synchro      */
#define IFC1211_IDMA_DES3_START_EVT1            (2<<0)  /* Wait for Event #1             */
#define IFC1211_IDMA_DES3_START_EVT2            (3<<0)  /* Wait for Event #2             */
#define IFC1211_IDMA_DES3_START_IN1             (4<<0)  /* Wait for ON #1                */
#define IFC1211_IDMA_DES3_START_IN2             (5<<0)  /* Wait for ON #2                */
#define IFC1211_IDMA_DES3_START_IN3             (6<<0)  /* Wait for ON #3                */
#define IFC1211_IDMA_DES3_START_IN4             (7<<0)  /* Wait for ON #4                */
#define IFC1211_IDMA_DES3_LAST                  (1<<4)  /* Last descriptor               */
#define IFC1211_IDMA_DES3_NEXT_MASK    (0xffffffe0<<5)  /* Next Descriptor address mask   */

#define IFC1211_IDMA_DES4_       (1<<0)  /*       */

#define IFC1211_IDMA_DES5_       (1<<0)  /*       */

#define IFC1211_IDMA_DES6_       (1<<0)  /*       */

#define IFC1211_IDMA_DES7_       (1<<0)  /*       */

 
#define IFC1211_FIFO_CTL_WCNT_MAX	              255  /* FIFO word counter maximum       */
#define IFC1211_FIFO_CTL_WCNT_MASK	        0x000000ff  /* FIFO word counter mask       */
#define IFC1211_FIFO_CTL_WCNT(x)	          (x&0xff)  /* FIFO word counter            */
#define IFC1211_FIFO_CTL_WRPT_MASK	        0x0000ff00  /* FIFO write pointer mask      */
#define IFC1211_FIFO_CTL_WRPT(x)	     ((x>>8)&0xff)  /* FIFO write pointer           */
#define IFC1211_FIFO_CTL_RDPT_MASK	        0x00ff0000  /* FIFO read pointer mask       */
#define IFC1211_FIFO_CTL_RDPT(x)	    ((x>>16)&0xff)  /* FIFO read pointer            */
#define IFC1211_FIFO_CTL_NOEMPTY	           (1<<24)  /* FIFO not empty               */
#define IFC1211_FIFO_CTL_FULL	           (1<<25)  /* FIFO full                    */
#define IFC1211_FIFO_CTL_MBX	                   (1<<26)  /* FIFO mailbox mode            */
#define IFC1211_FIFO_CTL_ERRF	           (1<<27)  /* FIFO error                   */
#define IFC1211_FIFO_CTL_RESET	           (1<<28)  /* FIFO reset                   */
#define IFC1211_FIFO_CTL_REA	                   (1<<29)  /* FIFO read enable             */
#define IFC1211_FIFO_CTL_WEA	                   (1<<30)  /* FIFO write enable            */
#define IFC1211_FIFO_CTL_ENA	                   (1<<31)  /* FIFO enable                  */

#endif				/* _H_IFC1211 */
