/*=========================< begin file & file header >=======================
 *  References
 *  
 *    filename : tsc.h
 *    author   : JFG, XP
 *    company  : IOxOS
 *    creation : june 30,2008
 *
 *----------------------------------------------------------------------------
 *  Description
 *
 *    This file contains the declarations related to the TSC device
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

#ifndef _H_TSC
#define _H_TSC

#define TSC_VERSION "3.5.5"

#define	PCI_VENDOR_ID_IOXOS               0x7357   /* IOXOS PCI VENDOR ID         */
#define	PCI_DEVICE_ID_IOXOS_TSC_IO        0x1000   /* TSC PCI DEVICE ID IO        */
#define	PCI_DEVICE_ID_IOXOS_TSC_CENTRAL_1 0x1001   /* TSC PCI DEVICE ID CENTRAL 1 */
#define	PCI_DEVICE_ID_IOXOS_TSC_CENTRAL_2 0x1002   /* TSC PCI DEVICE ID CENTRAL 2 */
#define	PCI_DEVICE_ID_IOXOS_TSC_CENTRAL_3 0x1003   /* TSC PCI DEVICE ID CENTRAL 3 */

/*
 *  Define the number of each that the TSC supports.
 */
#define TSC_MAX_MASTER               64  /* Max Master Windows                   */
#define TSC_MAX_SLAVE                64  /* Max Slave Windows                    */
#define TSC_MAX_DMA                   2  /* Max DMA Controllers                  */
#define TSC_MAX_DMA_LEN        0xF00000  /* Max DMA size 15MB                    */
#define TSC_MAX_SG_DMA_LEN     0x700000  /* Max scatter-gather DMA size 7MB      */
#define TSC_MAX_MAILBOX               4  /* Max Mail Box registers               */
#define TSC_MAX_SEMAPHORE             8  /* Max Semaphores                       */
#define TSC_MAX_EVENT                16  /* Max Event Queues                     */
#define TSC_IRQ_NUM                  64  /* number of interrupt sources          */
#define TSC_AGENT_SW_NUM              6  /* number agent switches                */

#define TSC_IRQ_CTL_ILOC                0  /*  ILOC controler IRQ base           */
#define TSC_IRQ_CTL_IDMA               32  /*  IDMA controler IRQ base           */
#define TSC_IRQ_CTL_MBX                32  /*  FIFO controler IRQ base           */
#define TSC_IRQ_CTL_USER               48  /*  USER controler IRQ base           */

#define TSC_IRQ_SRC_I2C_CRIT            9  /*  I2C CRIT                           */
#define TSC_IRQ_SRC_I2C_OK             10  /*  I2C ended OK                       */
#define TSC_IRQ_SRC_I2C_ERR            11  /*  I2C Ended with error               */

#define TSC_IRQ_SRC_SYSFAIL             0  /*  SYSFAIL interrupt              */
#define TSC_IRQ_SRC_IRQ1                1  /*  IRQ1 interrupt                 */
#define TSC_IRQ_SRC_IRQ2                2  /*  IRQ2 interrupt                 */
#define TSC_IRQ_SRC_IRQ3                3  /*  IRQ3 interrupt                 */
#define TSC_IRQ_SRC_IRQ4                4  /*  IRQ4 interrupt                 */
#define TSC_IRQ_SRC_IRQ5                5  /*  IRQ5 interrupt                 */
#define TSC_IRQ_SRC_IRQ6                6  /*  IRQ6 interrupt                 */
#define TSC_IRQ_SRC_IRQ7                7  /*  IRQ7 interrupt                 */
#define TSC_IRQ_SRC_ACFAIL              8  /*  ACFAIL interrupt               */
#define TSC_IRQ_SRC_MASERR              9  /*  Master Error interrupt         */
#define TSC_IRQ_SRC_GTIM0              10  /*  Global Time interrupt 0        */
#define TSC_IRQ_SRC_GTIM1              11  /*  Global Time interrupt 1        */
#define TSC_IRQ_SRC_LM0                12  /*  Location Monitor 0 interrupt   */
#define TSC_IRQ_SRC_LM1                13  /*  Location Monitor 1 interrupt   */
#define TSC_IRQ_SRC_LM2                14  /*  Location Monitor 2 interrupt   */
#define TSC_IRQ_SRC_LM3                15  /*  Location Monitor 3 interrupt   */

#define TSC_IRQ_SRC_IDMA_RD0_END         0  /*  IDMA read channel #0 ended         */
#define TSC_IRQ_SRC_IDMA_RD0_ERR         1  /*  IDMA read channel #0 error         */
#define TSC_IRQ_SRC_IDMA_RD1_END         2  /*  IDMA read channel #1 ended         */
#define TSC_IRQ_SRC_IDMA_RD1_ERR         3  /*  IDMA read channel #1 error         */
#define TSC_IRQ_SRC_IDMA_WR0_END         4  /*  IDMA write channel #0 ended        */
#define TSC_IRQ_SRC_IDMA_WR0_ERR         5  /*  IDMA write channel #0 error        */
#define TSC_IRQ_SRC_IDMA_WR1_END         6  /*  IDMA write channel #1 ended        */
#define TSC_IRQ_SRC_IDMA_WR1_ERR         7  /*  IDMA write channel #1 error        */
#define TSC_IRQ_SRC_MBX_0                8  /*  Mailbox #0 not empty               */
#define TSC_IRQ_SRC_MBX_1                9  /*  Mailbox #0 not empty               */
#define TSC_IRQ_SRC_MBX_2               10  /*  Mailbox #0 not empty               */
#define TSC_IRQ_SRC_MBX_3               11  /*  Mailbox #0 not empty               */
#define TSC_IRQ_SRC_MBX_4               12  /*  Mailbox #0 not empty               */
#define TSC_IRQ_SRC_MBX_5               13  /*  Mailbox #0 not empty               */
#define TSC_IRQ_SRC_MBX_6               14  /*  Mailbox #0 not empty               */
#define TSC_IRQ_SRC_MBX_7               15  /*  Mailbos #7 not empty               */

#define TSC_ITC_IM_ALL            (0xffff)  /*  mask for  All interrupts           */
#define TSC_ITC_VEC( sts)       (sts&0xff)  /*  get irq ector from iack status     */
#define TSC_ITC_SRC( sts)   ((sts>>8)&0xf) /*  get irq source from iack status    */
#define TSC_ITC_CTL( ctl)  ((ctl>>12)&0x3)  /*  get irq controller from iack status   */


/*
 *  TSC ASIC register structure overlays and bit field definitions.
 *  refer to TSC user's manual for detailed description.
 *
 */
#define TSC_CSR_ILOC_BASE	             0x000
#define TSC_CSR_ILOC_STATIC	             0x000
#define TSC_CSR_ILOC_OPT_DYN_SEL         0x004
#define TSC_CSR_IPCIE                    0x004
#define TSC_CSR_ILOC_OPT_DYN_DAT         0x008
#define TSC_CSR_ILOC_PON_FSM             0x00c
#define TSC_CSR_ILOC_SPI                 0x010
#define TSC_CSR_ILOC_EFUSE_USR           0x014
#define TSC_CSR_ILOC_SIGN                0x018
#define TSC_CSR_ILOC_GENCTL              0x01c

#define TSC_CSR_PCIE_MMUADD              0x020
#define TSC_CSR_PCIE_MMUDAT              0x024

#define TSC_CSR_ILOC_TOSCA2_SIGN         0x034

#define TSC_CSR_A7_PCIE_BASE             0x000
#define TSC_CSR_A7_PCIE1_BASE            0x400

#define TSC_CSR_A7_PCIE_DSN_L            0x038
#define TSC_CSR_A7_PCIE_DSN_H            0x03c
#define TSC_CSR_A7_SMON_ADDPT            0x040
#define TSC_CSR_A7_SMON_DAT              0x044
#define TSC_CSR_A7_SMON_STA              0x048
#define TSC_CSR_A7_PCIE_REQCNT           0x050
#define TSC_CSR_A7_PCIE_ARMCNT           0x054
#define TSC_CSR_A7_PCIE_ACKCNT           0x058
#define TSC_CSR_A7_PCIE_CTL              0x05c
#define TSC_CSR_A7_PCIEP_ADDPT           0x060
#define TSC_CSR_A7_PCIEP_DRPDAT          0x064
#define TSC_CSR_A7_PCIEP_CFGDAT          0x068
#define TSC_CSR_A7_PCIEP_LINK_STA        0x06c
#define TSC_CSR_A7_PCIEP_CFG_STA_0       0x070
#define TSC_CSR_A7_PCIEP_CFG_STA_1       0x074
#define TSC_CSR_A7_PCIEP_CFG_STA_2       0x078
#define TSC_CSR_A7_PCIEP_CFG_STA_3       0x07c

#define TSC_CSR_ILOC_ITC_IACK            0x080
#define TSC_CSR_ILOC_ITC_CSR             0x084
#define TSC_CSR_ILOC_ITC_IMC             0x088
#define TSC_CSR_ILOC_ITC_IMS             0x08c

#define TSC_CSR_I2C_CTL                  0x10a0
#define TSC_CSR_I2C_CMD                  0x10a4
#define TSC_CSR_I2C_DATW                 0x10a8
#define TSC_CSR_I2C_DATR                 0x10ac

#define TSC_CSR_BASE                0x400
#define TSC_CSR_SLOT1               0x400
#define TSC_CSR_AGENT_SW_OFFSET     0x400
#define TSC_CSR_MASCSR              0x404
#define TSC_CSR_SLVCSR              0x408
#define TSC_CSR_SLVCSR_A32          0x408
#define TSC_CSR_INTG                0x40c
#define TSC_CSR_MMUADD              0x410
#define TSC_CSR_MMUDAT              0x414
#define TSC_CSR_ADDERR              0x418
#define TSC_CSR_STAERR              0x41c
#define TSC_CSR_LOCMON              0x420
#define TSC_CSR_LOCK                0x424
#define TSC_CSR_RMW_MODE            0x428
#define TSC_CSR_RMW_ADD             0x42c
#define TSC_CSR_RMW_DATCMP          0x430
#define TSC_CSR_RMW_DATUPT          0x434
#define TSC_CSR_GLTIM_CSR           0x1070
#define TSC_CSR_GLTIM_CNT2          0x1078
#define TSC_CSR_GLTIM_CNT1          0x107c

#define TSC_CSR_SLVCSR_A24          0x450
#define TSC_CSR_SLVCSR_A16          0x454
#define TSC_CSR_SLVCSR_2eBRC        0x458
                                                                        
#define TSC_CSR_ITC_IACK            0x480
#define TSC_CSR_ITC_CSR             0x484
#define TSC_CSR_ITC_IMC             0x488
#define TSC_CSR_ITC_IMS             0x48c

#define TSC_CSR_CSR_ADER0_3	        0x560
#define TSC_CSR_CSR_ADER0_2	        0x564
#define TSC_CSR_CSR_ADER0_1	        0x568
#define TSC_CSR_CSR_ADER0_0	        0x56c
#define TSC_CSR_CSR_ADER1_3	        0x570
#define TSC_CSR_CSR_ADER1_2	        0x574
#define TSC_CSR_CSR_ADER1_1	        0x578
#define TSC_CSR_CSR_ADER1_0	        0x57c
#define TSC_CSR_CSR_ADER2_3	        0x580
#define TSC_CSR_CSR_ADER2_2	        0x584
#define TSC_CSR_CSR_ADER2_1	        0x588
#define TSC_CSR_CSR_ADER2_0	        0x58c
#define TSC_CSR_CSR_ADER3_3	        0x590
#define TSC_CSR_CSR_ADER3_2	        0x594
#define TSC_CSR_CSR_ADER3_1	        0x598
#define TSC_CSR_CSR_ADER3_0	        0x59c
#define TSC_CSR_CSR_BCR	        	0x5f4
#define TSC_CSR_CSR_BSR	        	0x5f8
#define TSC_CSR_CSR_BAR	        	0x5fc
#define TSC_CSR_CSR_ROM	        	0x780

#define TSC_CSR_SMEM_BASE	        	0x800
#define TSC_CSR_SMEM_DDR3_CSR	        0x800
#define TSC_CSR_SMEM_DDR3_ERR	        0x804
#define TSC_CSR_SMEM_DDR3_DELQ	        0x808
#define TSC_CSR_SMEM_DDR3_IDEL	        0x80c
#define TSC_CSR_SMEM_SRAM_CSR	        0x810

#define TSC_CSR_SMEM2_BASE	        	0xc00
#define TSC_CSR_SMEM2_DDR3_CSR	        0xc00
#define TSC_CSR_SMEM2_DDR3_ERR	        0xc04
#define TSC_CSR_SMEM2_DDR3_DELQ	        0xc08
#define TSC_CSR_SMEM2_DDR3_IDEL	        0xc0c
#define TSC_CSR_SMEM2_SRAM_CSR	        0xc10

#define TSC_CSR_IDMA_BASE	       	 	0x800
#define TSC_CSR_IDMA_GCSR	        	0x840

#define TSC_CSR_IDMA_RD_0_PCSR	        0x850
#define TSC_CSR_IDMA_RD_1_PCSR	        0x854
#define TSC_CSR_IDMA_WR_0_PCSR	        0x858
#define TSC_CSR_IDMA_WR_1_PCSR	        0x85c

#define TSC_CSR_IDMA_ITC_IACK	        0x880
#define TSC_CSR_IDMA_ITC_CSR	        0x884
#define TSC_CSR_IDMA_ITC_IMC	        0x888
#define TSC_CSR_IDMA_ITC_IMS	        0x88c

#define TSC_CSR_IDMA2_BASE	       	 	0xc00
#define TSC_CSR_IDMA2_GCSR	        	0xc40

#define TSC_CSR_IDMA2_RD_0_PCSR	        0xc50
#define TSC_CSR_IDMA2_RD_1_PCSR	        0xc54
#define TSC_CSR_IDMA2_WR_0_PCSR	        0xc58
#define TSC_CSR_IDMA2_WR_1_PCSR	        0xc5c

#define TSC_CSR_IDMA2_ITC_IACK	        0xc80
#define TSC_CSR_IDMA2_ITC_CSR	        0xc84
#define TSC_CSR_IDMA2_ITC_IMC	        0xc88
#define TSC_CSR_IDMA2_ITC_IMS	        0xc8c

#define TSC_CSR_SEMAPHORE 	 		0x8b8

#define TSC_MBX_NUM                             8
#define TSC_CSR_MBX_0_CTL	        	0x8C0
#define TSC_CSR_MBX_1_CTL	        	0x8C4
#define TSC_CSR_MBX_2_CTL	        	0x8C8
#define TSC_CSR_MBX_3_CTL	        	0x8CC
#define TSC_CSR_MBX_4_CTL	        	0x8D0
#define TSC_CSR_MBX_5_CTL	        	0x8D4
#define TSC_CSR_MBX_6_CTL	        	0x8D8
#define TSC_CSR_MBX_7_CTL	        	0x8DC
static const int TSC_CSR_MBX_CTL[TSC_MBX_NUM] = {
                                                TSC_CSR_MBX_0_CTL, TSC_CSR_MBX_1_CTL,
						TSC_CSR_MBX_2_CTL, TSC_CSR_MBX_3_CTL,
						TSC_CSR_MBX_4_CTL, TSC_CSR_MBX_5_CTL,
						TSC_CSR_MBX_6_CTL, TSC_CSR_MBX_7_CTL };

#define TSC_CSR_MBX_0_PORT	        0x8E0
#define TSC_CSR_MBX_1_PORT	        0x8E4
#define TSC_CSR_MBX_2_PORT	        0x8E8
#define TSC_CSR_MBX_3_PORT	        0x8EC
#define TSC_CSR_MBX_4_PORT	        0x8F0
#define TSC_CSR_MBX_5_PORT	        0x8F4
#define TSC_CSR_MBX_6_PORT	        0x8F8
#define TSC_CSR_MBX_7_PORT	        0x8FC
static const int TSC_CSR_MBX_PORT[TSC_MBX_NUM] = {
                                                TSC_CSR_MBX_0_PORT, TSC_CSR_MBX_1_PORT,
			                        TSC_CSR_MBX_2_PORT, TSC_CSR_MBX_3_PORT,
		                              	TSC_CSR_MBX_4_PORT, TSC_CSR_MBX_5_PORT,
		                        	TSC_CSR_MBX_6_PORT, TSC_CSR_MBX_7_PORT };

#define TSC_FIFO_NUM                         8
#define TSC_CSR_FIFO_0_CTL	        0x8C0
#define TSC_CSR_FIFO_1_CTL	        0x8C4
#define TSC_CSR_FIFO_2_CTL	        0x8C8
#define TSC_CSR_FIFO_3_CTL	        0x8CC
#define TSC_CSR_FIFO_4_CTL	        0x8D0
#define TSC_CSR_FIFO_5_CTL	        0x8D4
#define TSC_CSR_FIFO_6_CTL	        0x8D8
#define TSC_CSR_FIFO_7_CTL	        0x8DC
static const int TSC_CSR_FIFO_CTL[TSC_FIFO_NUM] = {
                                                TSC_CSR_FIFO_0_CTL, TSC_CSR_FIFO_1_CTL,
			                        TSC_CSR_FIFO_2_CTL, TSC_CSR_FIFO_3_CTL,
		                              	TSC_CSR_FIFO_4_CTL, TSC_CSR_FIFO_5_CTL,
		                        	TSC_CSR_FIFO_6_CTL, TSC_CSR_FIFO_7_CTL };

#define TSC_CSR_FIFO_0_PORT	        0x8E0
#define TSC_CSR_FIFO_1_PORT	        0x8E4
#define TSC_CSR_FIFO_2_PORT	        0x8E8
#define TSC_CSR_FIFO_3_PORT	        0x8EC
#define TSC_CSR_FIFO_4_PORT	        0x8F0
#define TSC_CSR_FIFO_5_PORT	        0x8F4
#define TSC_CSR_FIFO_6_PORT	        0x8F8
#define TSC_CSR_FIFO_7_PORT	        0x8FC
static const int TSC_CSR_FIFO_PORT[TSC_FIFO_NUM] = {
                                                TSC_CSR_FIFO_0_PORT, TSC_CSR_FIFO_1_PORT,
			                        TSC_CSR_FIFO_2_PORT, TSC_CSR_FIFO_3_PORT,
		                              	TSC_CSR_FIFO_4_PORT, TSC_CSR_FIFO_5_PORT,
		                        	TSC_CSR_FIFO_6_PORT, TSC_CSR_FIFO_7_PORT };

#define TSC_CSR_IDMA_RD_0_CSR	        0x900
#define TSC_CSR_IDMA_RD_0_NDES	        0x904
#define TSC_CSR_IDMA_RD_0_CDES	        0x908
#define TSC_CSR_IDMA_RD_0_DCNT	        0x90c

#define TSC_CSR_IDMA_RD_1_CSR	        0x940
#define TSC_CSR_IDMA_RD_1_NDES	        0x944
#define TSC_CSR_IDMA_RD_1_CDES	        0x948
#define TSC_CSR_IDMA_RD_1_DCNT	        0x94c

#define TSC_CSR_IDMA_WR_0_CSR	        0xa00
#define TSC_CSR_IDMA_WR_0_NDES	        0xa04
#define TSC_CSR_IDMA_WR_0_CDES	        0xa08
#define TSC_CSR_IDMA_WR_0_DCNT	        0xa0c

#define TSC_CSR_IDMA_WR_1_CSR	        0xa40
#define TSC_CSR_IDMA_WR_1_NDES	        0xa44
#define TSC_CSR_IDMA_WR_1_CDES	        0xa48
#define TSC_CSR_IDMA_WR_1_DCNT	        0xa4c

static const int TSC_CSR_IDMA_CSR[4] = { TSC_CSR_IDMA_RD_0_CSR, TSC_CSR_IDMA_RD_1_CSR,
					        TSC_CSR_IDMA_WR_0_CSR, TSC_CSR_IDMA_WR_1_CSR };
static const int TSC_CSR_IDMA_NDES[4] = { TSC_CSR_IDMA_RD_0_NDES, TSC_CSR_IDMA_RD_1_NDES,
					         TSC_CSR_IDMA_WR_0_NDES, TSC_CSR_IDMA_WR_1_NDES };
static const int TSC_CSR_IDMA_CDES[4] = { TSC_CSR_IDMA_RD_0_CDES, TSC_CSR_IDMA_RD_1_CDES,
					         TSC_CSR_IDMA_WR_0_CDES, TSC_CSR_IDMA_WR_1_CDES };
static const int TSC_CSR_IDMA_DCNT[4] = { TSC_CSR_IDMA_RD_0_DCNT, TSC_CSR_IDMA_RD_1_DCNT,
					         TSC_CSR_IDMA_WR_0_DCNT, TSC_CSR_IDMA_WR_1_DCNT };

#define TSC_CSR_IDMA2_RD_0_CSR	        0xd00
#define TSC_CSR_IDMA2_RD_0_NDES	        0xd04
#define TSC_CSR_IDMA2_RD_0_CDES	        0xd08
#define TSC_CSR_IDMA2_RD_0_DCNT	        0xd0c

#define TSC_CSR_IDMA2_RD_1_CSR	        0xd40
#define TSC_CSR_IDMA2_RD_1_NDES	        0xd44
#define TSC_CSR_IDMA2_RD_1_CDES	        0xd48
#define TSC_CSR_IDMA2_RD_1_DCNT	        0xd4c

#define TSC_CSR_IDMA2_WR_0_CSR	        0xe00
#define TSC_CSR_IDMA2_WR_0_NDES	        0xe04
#define TSC_CSR_IDMA2_WR_0_CDES	        0xe08
#define TSC_CSR_IDMA2_WR_0_DCNT	        0xe0c

#define TSC_CSR_IDMA2_WR_1_CSR	        0xe40
#define TSC_CSR_IDMA2_WR_1_NDES	        0xe44
#define TSC_CSR_IDMA2_WR_1_CDES	        0xe48
#define TSC_CSR_IDMA2_WR_1_DCNT	        0xe4c

static const int TSC_CSR_IDMA2_CSR[4] = { TSC_CSR_IDMA2_RD_0_CSR, TSC_CSR_IDMA2_RD_1_CSR,
					        TSC_CSR_IDMA2_WR_0_CSR, TSC_CSR_IDMA2_WR_1_CSR };
static const int TSC_CSR_IDMA2_NDES[4] = { TSC_CSR_IDMA2_RD_0_NDES, TSC_CSR_IDMA2_RD_1_NDES,
					         TSC_CSR_IDMA2_WR_0_NDES, TSC_CSR_IDMA2_WR_1_NDES };
static const int TSC_CSR_IDMA2_CDES[4] = { TSC_CSR_IDMA2_RD_0_CDES, TSC_CSR_IDMA2_RD_1_CDES,
					         TSC_CSR_IDMA2_WR_0_CDES, TSC_CSR_IDMA2_WR_1_CDES };
static const int TSC_CSR_IDMA2_DCNT[4] = { TSC_CSR_IDMA2_RD_0_DCNT, TSC_CSR_IDMA2_RD_1_DCNT,
					         TSC_CSR_IDMA2_WR_0_DCNT, TSC_CSR_IDMA2_WR_1_DCNT };
#define TSC_CSR_IDMA_ROM 	            0xb80

#define TSC_CSR_USER_BASE	            0xc00
#define TSC_CSR_GPIO_OUT	            0xc00
#define TSC_CSR_GPIO_ENA	            0xc04
#define TSC_CSR_GPIO_IN	            0xc08
#define TSC_CSR_GPIO_IPOL	            0xc0c

#define TSC_CSR_UART_CTL	            0xc10
#define TSC_CSR__SERIAL	            0xc14

#define TSC_CSR_USER_ITC_IACK	    0xc80
#define TSC_CSR_USER_ITC_CSR	    0xc84
#define TSC_CSR_USER_ITC_IMC	    0xc88
#define TSC_CSR_USER_ITC_IMS	    0xc8c

#define TSC_CSR_USER_ROM	            0xf80

#define TSC_CSR_OFFSET_MAX	       0xfff

#define TSC_CSR_ITC_OFFSET         0x0080

#define TSC_CSR_ITC_IACK_OFFSET    0x0000
#define TSC_CSR_ITC_CSR_OFFSET     0x0004
#define TSC_CSR_ITC_IMC_OFFSET     0x0008
#define TSC_CSR_ITC_IMS_OFFSET     0x000C

/*
 *  SPI FLASH control bits (CSR + $010)
 */
#define TSC_ILOC_SPI_END                0x00
#define TSC_ILOC_SPI_CLK                0x01
#define TSC_ILOC_SPI_DO                 0x02
#define TSC_ILOC_SPI_DI                 0x04
#define TSC_ILOC_SPI_CS                 0x08
#define TSC_ILOC_SPI_WRPROT_PIN       0x4000
#define TSC_ILOC_SPI_WRPROT_SW        0x8000
#define TSC_ILOC_SPI_WRPROT(x)   ((x>>14)&3)
#define TSC_ILOC_SPI_WPOST_BUSY     0x100000

/*
 *  TOSCA2 Signature (CSR + $034)
 */
#define TSC_TOSCA2_SIGN	        0x20000000

/*
 *  Master MMU parameters
 */
#define TSC_PCIE_MMU_PG_NUM                 1024  /* number of pages 1024 pages  */
#define TSC_PCIE_MMU_PG_4M              0x400000  /* page size 4 MBytes          */
#define TSC_PCIE_MMU_PG_1M              0x100000  /* page size 1 MBytes          */
#define TSC_PCIE_MMU_PG_64K              0x10000  /* page size 64 KBytes         */

/*
 *  Master MMU address pointer(CSR + $20)
 */
#define TSC_PCIE_MMUADD_PTR_MASK                  (0x1ffe<<0)  /* Mask for MMU pointer (1024 pages) */
#define TSC_PCIE_MMUADD_PMEM_SEL                      (0<<16)  /* Select MMU for A64 PMEM           */
#define TSC_PCIE_MMUADD_MEM_SEL                       (1<<16)  /* Select MMU for A32                */
#define TSC_PCIE_MMUADD_MEM_SIZE(x)   (0x200000<<((x>>18)&7))  /* calculate PCI MEM size            */
#define TSC_PCIE_MMUADD_MEM_ENA                       (1<<23)  /* PCI MEM enable                    */
#define TSC_PCIE_MMUADD_PMEM_SIZE(x) (0x2000000<<((x>>26)&7))  /* calculate PCI MEM size            */
#define TSC_PCIE_MMUADD_PMEM_A64                      (1<<30)  /* PCI PMEM A64                      */
#define TSC_PCIE_MMUADD_PMEM_ENA                      (1<<31)  /* PCI PMEM enable                   */

/*
 *  Master MMU data register(CSR + $24)
 */
#define TSC_PCIE_MMUDAT_PG_ENA               (1<<0)  /* Enable page                       */
#define TSC_PCIE_MMUDAT_WR_ENA               (1<<1)  /* Enable write                      */
#define TSC_PCIE_MMUDAT_D16                  (1<<3)  /* D16 access                        */
#define TSC_PCIE_MMUDAT_PROG                 (1<<4)  /* Program access                    */
#define TSC_PCIE_MMUDAT_SUPER                (1<<5)  /* Supervisor access                 */
#define TSC_PCIE_MMUDAT_SWAP_AUTO            (1<<6)  /* set auto swap be <-> le           */
#define TSC_PCIE_MMUDAT_SWAP_DW              (2<<6)  /* set double word swap              */
#define TSC_PCIE_MMUDAT_SWAP_QW              (3<<6)  /* set qwad word swap                */
#define TSC_PCIE_MMUDAT_SWAP_MASK            (3<<6)  /* set double word swap              */
#define TSC_PCIE_MMUDAT_AS_CRCSR             (0<<8)  /* address space CRCSR               */
#define TSC_PCIE_MMUDAT_AS_A16               (1<<8)  /* address space A16                 */
#define TSC_PCIE_MMUDAT_AS_A24               (2<<8)  /* address space A24                 */
#define TSC_PCIE_MMUDAT_AS_A32               (3<<8)  /* address space A32                 */
#define TSC_PCIE_MMUDAT_AS_A32_BLT           (4<<8)  /* address space A32 BLT             */
#define TSC_PCIE_MMUDAT_AS_A32_MBLT          (5<<8)  /* address space A32 MBLT            */
#define TSC_PCIE_MMUDAT_AS_A32_2eVME         (6<<8)  /* address space A32 2eVME           */
#define TSC_PCIE_MMUDAT_AS_USER              (7<<8)  /* address space USER                */
#define TSC_PCIE_MMUDAT_AS_A32_2eSST_160     (8<<8)  /* address space A32 2eSST 160       */
#define TSC_PCIE_MMUDAT_AS_A32_2eSST_233     (9<<8)  /* address space A32 2eSST 233       */
#define TSC_PCIE_MMUDAT_AS_A32_2eSST_320    (10<<8)  /* address space A32 2eSST 320       */
#define TSC_PCIE_MMUDAT_AS_A32_2eSST_400    (11<<8)  /* address space A32 2eSST 400       */
#define TSC_PCIE_MMUDAT_AS_A32_2eBCR        (12<<8)  /* address space A32 2e broadcast    */
#define TSC_PCIE_MMUDAT_AS_A24_ADO          (13<<8)  /* address space A24 Address Only    */
#define TSC_PCIE_MMUDAT_AS_A32_ADO          (14<<8)  /* address space A32 Address Only    */
#define TSC_PCIE_MMUDAT_AS_IACK             (15<<8)  /* address space IACK                */
#define TSC_PCIE_MMUDAT_AS_MASK             (15<<8)  /* address space mask                */
#define TSC_PCIE_MMUDAT_DES_                (1<<12)  /* destination bus                   */
#define TSC_PCIE_MMUDAT_DES_SHM             (2<<12)  /* destination Shared Memory         */
#define TSC_PCIE_MMUDAT_DES_SHM1            (2<<12)  /* destination Shared Memory 1       */
#define TSC_PCIE_MMUDAT_DES_SHM2            (3<<12)  /* destination Shared Memory 2       */
#define TSC_PCIE_MMUDAT_DES_USR             (3<<12)  /* destination User Block            */
#define TSC_PCIE_MMUDAT_DES_USR1            (4<<12)  /* destination User Block 1          */
#define TSC_PCIE_MMUDAT_DES_USR2            (5<<12)  /* destination User Block 2          */

#define TSC_PCIE_MMUDAT_DES_MASK          (0xf<<12)  /* destination Mask                  */

#define TSC_PCIE_MMUDAT_AS_INV                   -1
#define TSC_PCIE_MMUDAT_AM_MASK                0xff
static const int TSC_PCIE_MMUDAT_AM[0x40] = {
                 TSC_PCIE_MMUDAT_AS_INV, /* 0x00 */
                 TSC_PCIE_MMUDAT_AS_INV, /* 0x01 */
                 TSC_PCIE_MMUDAT_AS_INV, /* 0x02 */
                 TSC_PCIE_MMUDAT_AS_INV, /* 0x03 */
                 TSC_PCIE_MMUDAT_AS_INV, /* 0x04 */
                 TSC_PCIE_MMUDAT_AS_INV, /* 0x05 */
                 TSC_PCIE_MMUDAT_AS_INV, /* 0x06 */
                 TSC_PCIE_MMUDAT_AS_INV, /* 0x07 */
                 TSC_PCIE_MMUDAT_AS_A32_MBLT,                                                         /* 0x08 */
                 TSC_PCIE_MMUDAT_AS_A32,                                                              /* 0x09 */
                 TSC_PCIE_MMUDAT_AS_A32 | TSC_PCIE_MMUDAT_PROG,                                /* 0x0a */
                 TSC_PCIE_MMUDAT_AS_A32_BLT,                                                          /* 0x0b */
                 TSC_PCIE_MMUDAT_AS_A32_MBLT | TSC_PCIE_MMUDAT_SUPER,                          /* 0x0c */
                 TSC_PCIE_MMUDAT_AS_A32 | TSC_PCIE_MMUDAT_SUPER,                               /* 0x0d */
                 TSC_PCIE_MMUDAT_AS_A32 | TSC_PCIE_MMUDAT_SUPER | TSC_PCIE_MMUDAT_PROG, /* 0x0e */
                 TSC_PCIE_MMUDAT_AS_A32_BLT | TSC_PCIE_MMUDAT_SUPER,                           /* 0x0f */
                 TSC_PCIE_MMUDAT_AS_USER,                                                             /* 0x10 */
                 TSC_PCIE_MMUDAT_AS_INV,                                                              /* 0x11 */
                 TSC_PCIE_MMUDAT_AS_USER | TSC_PCIE_MMUDAT_SUPER,                              /* 0x12 */
                 TSC_PCIE_MMUDAT_AS_INV,                                                              /* 0x13 */
                 TSC_PCIE_MMUDAT_AS_USER,                                                             /* 0x14 */
                 TSC_PCIE_MMUDAT_AS_INV,                                                              /* 0x15 */
                 TSC_PCIE_MMUDAT_AS_USER | TSC_PCIE_MMUDAT_SUPER,                              /* 0x16 */
                 TSC_PCIE_MMUDAT_AS_INV,                                                              /* 0x17 */
                 TSC_PCIE_MMUDAT_AS_USER,                                                             /* 0x18 */
                 TSC_PCIE_MMUDAT_AS_INV,                                                              /* 0x19 */
                 TSC_PCIE_MMUDAT_AS_USER | TSC_PCIE_MMUDAT_SUPER,                              /* 0x1a */
                 TSC_PCIE_MMUDAT_AS_INV,                                                              /* 0x1b */
                 TSC_PCIE_MMUDAT_AS_USER,                                                             /* 0x1c */
                 TSC_PCIE_MMUDAT_AS_INV,                                                              /* 0x1d */
                 TSC_PCIE_MMUDAT_AS_USER | TSC_PCIE_MMUDAT_SUPER,                              /* 0x1e */
                 TSC_PCIE_MMUDAT_AS_INV,                                                              /* 0x1f */
                 TSC_PCIE_MMUDAT_AS_INV,                                                              /* 0x20 */
                 TSC_PCIE_MMUDAT_AS_INV,                                                              /* 0x21 */
                 TSC_PCIE_MMUDAT_AS_INV,                                                              /* 0x22 */
                 TSC_PCIE_MMUDAT_AS_INV,                                                              /* 0x23 */
                 TSC_PCIE_MMUDAT_AS_INV,                                                              /* 0x24 */
                 TSC_PCIE_MMUDAT_AS_INV,                                                              /* 0x25 */
                 TSC_PCIE_MMUDAT_AS_INV,                                                              /* 0x26 */
                 TSC_PCIE_MMUDAT_AS_INV,                                                              /* 0x27 */
                 TSC_PCIE_MMUDAT_AS_INV,                                                              /* 0x28 */
                 TSC_PCIE_MMUDAT_AS_A16,                                                              /* 0x29 */
                 TSC_PCIE_MMUDAT_AS_INV,                                                              /* 0x2a */
                 TSC_PCIE_MMUDAT_AS_INV,                                                              /* 0x2b */
                 TSC_PCIE_MMUDAT_AS_INV,                                                              /* 0x2c */
                 TSC_PCIE_MMUDAT_AS_A16|TSC_PCIE_MMUDAT_SUPER,                                 /* 0x2d */
                 TSC_PCIE_MMUDAT_AS_INV,                                                              /* 0x2e */
                 TSC_PCIE_MMUDAT_AS_CRCSR,                                                            /* 0x2f */
                 TSC_PCIE_MMUDAT_AS_INV, /* 0x30 */
                 TSC_PCIE_MMUDAT_AS_INV, /* 0x31 */
                 TSC_PCIE_MMUDAT_AS_INV, /* 0x32 */
                 TSC_PCIE_MMUDAT_AS_INV, /* 0x33 */
                 TSC_PCIE_MMUDAT_AS_INV, /* 0x34 */
                 TSC_PCIE_MMUDAT_AS_INV, /* 0x35 */
                 TSC_PCIE_MMUDAT_AS_INV, /* 0x36 */
                 TSC_PCIE_MMUDAT_AS_INV, /* 0x37 */
                 TSC_PCIE_MMUDAT_AS_INV, /* 0x38 */
                 TSC_PCIE_MMUDAT_AS_A24,                                                              /* 0x39 */
                 TSC_PCIE_MMUDAT_AS_A24| TSC_PCIE_MMUDAT_PROG,                                 /* 0x3a */
                 TSC_PCIE_MMUDAT_AS_INV,                                                              /* 0x3b */
                 TSC_PCIE_MMUDAT_AS_INV,                                                              /* 0x3c */
                 TSC_PCIE_MMUDAT_AS_A24 | TSC_PCIE_MMUDAT_SUPER,                               /* 0x3d */
                 TSC_PCIE_MMUDAT_AS_A24 | TSC_PCIE_MMUDAT_SUPER | TSC_PCIE_MMUDAT_PROG, /* 0x3e */
                 TSC_PCIE_MMUDAT_AS_IACK
};                                                                                                           /* 0x3f */

/*
 *  PCIe End Point register address pointer
 */
#define TSC_A7_PCIEP_ADDPT_DRP           0x10000
#define TSC_A7_PCIEP_ADDPT_CFG           0x00000
#define TSC_A7_PCIEP1_ADDPT_DRP          0x11000
#define TSC_A7_PCIEP1_ADDPT_CFG          0x01000


/*
 *  I2C controller registers (CSR + $100/104/108/10c)
 */
#define TSC_I2C_CTL_ADD(a)      ((a&0x7f) | ((a&0x380) << 1))   /* set I2C device address  */
#define TSC_I2C_CTL_BTO_FL                            (1<<13)   /* Bus timeout flag         */
#define TSC_I2C_CTL_KILL                              (1<<14)   /* kill current transaction */
#define TSC_I2C_CTL_STR_REPEAT                        (1<<15)   /* restart transaction      */
#define TSC_I2C_CTL_CMDSIZ(x)                 (((x-1)&3)<<16)   /* command size (1-4)       */
#define TSC_I2C_CTL_DATSIZ(x)                 (((x-1)&3)<<18)   /* data size (1-4)          */
#define TSC_I2C_CTL_EXEC_IDLE                         (0<<20)
#define TSC_I2C_CTL_EXEC_RUN                          (1<<20)
#define TSC_I2C_CTL_EXEC_DONE                         (2<<20)
#define TSC_I2C_CTL_EXEC_ERR                          (3<<20)
#define TSC_I2C_CTL_EXEC_MASK                         (3<<20)
#define TSC_I2C_CTL_TRIG_CMD                          (1<<22)
#define TSC_I2C_CTL_TRIG_DATW                         (2<<22)
#define TSC_I2C_CTL_TRIG_DATR                         (3<<22)
#define TSC_I2C_CTL_TRIG_MASK                         (3<<22)
#define TSC_I2C_CTL_SPEED_100k                        (0<<24)
#define TSC_I2C_CTL_SPEED_400k                        (1<<24)
#define TSC_I2C_CTL_SPEED_1M                          (2<<24)
#define TSC_I2C_CTL_PORT(port)               ((port&0x7)<<29)

/*
 *  Interrupt Controller Control and Status(CSR + $84/$484/$884/$c84)
 */
#define TSC_ALL_ITC_CSR_GLENA           (1<<0)   /* Interrupt Controller global enable */
#define TSC_ALL_ITC_CSR_CLEARIP         (1<<1)   /* Clear all pending interrupts       */
#define TSC_ITC_CSR_AUTOIACK       (1<<2)   /* Enable auto IACK mechanism     */

#define TSC_ITC_IACK_VEC(iack)            (iack&0xff)        /* extract vector from iack */
#define TSC_ITC_IACK_BASE(iack)           ((iack>>2)&0x1c00) /* extract ITC base from iack */
#define TSC_ALL_ITC_IACK_SRC(iack)        ((iack>>8)&0x7f)   /* extract source from iack */
#define TSC_ALL_ITC_IACK_IP(iack)         ((iack>>8)&0xf)    /* extract controller from iack */
#define TSC_ALL_ITC_IACK_ITC(iack)        ((iack>>12)&0x7)   /* extract controller from iack */

/*
 *  Interrupt Controller Mask Set/Clear(CSR + $84/$484/$884/$c84)
 */
#define TSC_ALL_ITC_MASK_ALL            0xffff   /* Mask for all interrupt sources     */
#define TSC_ALL_ITC_IM( src)    (1<<(src&0xf))   /* get interrupt mask from source     */

/*
 *  VME master & General Control and Status(CSR + $404)
 */
#define TSC_MASCSR_REQMOD_RWD            0  /* ARB Request mode Release When Done     */
#define TSC_MASCSR_REQMOD_ROR            1  /* ARB Request mode Release When Done     */
#define TSC_MASCSR_REQMOD_FAIR           2  /* ARB Request mode Release When Done     */
#define TSC_MASCSR_REQMOD_NOREL          3  /* ARB Request mode Release When Done     */
#define TSC_MASCSR_REQMOD_MASK           3  /* mask for ARB Request mode              */
#define TSC_MASCSR_REQLEV_0         (0<<2)  /* ARB Request level 0                    */
#define TSC_MASCSR_REQLEV_1         (1<<2)  /* ARB Request level 1                    */
#define TSC_MASCSR_REQLEV_2         (2<<2)  /* ARB Request level 2                    */
#define TSC_MASCSR_REQLEV_3         (3<<2)  /* ARB Request level 3                    */
#define TSC_MASCSR_REQLEV_MASK      (3<<2)  /* mask ARB Request level                 */
#define TSC_MASCSR_BACKOFF          (1<<4)  /* Retry acknowledge force bus release    */
#define TSC_MASCSR_SUPER            (1<<5)  /* VME AM assign to supervisor            */
#define TSC_MASCSR_MAXRETRY         (1<<6)  /* limit number of retry to 1024          */
#define TSC_MASCSR_READERR_DIS      (1<<7)  /* VME Read BERR -> PCIe status OK        */
#define TSC_MASCSR_REJ_FL           (1<<8)  /* VME cycle rejected due to MAS disabled */
#define TSC_MASCSR_HW16_SW          (1<<9)  /* D16 swapping enable                    */
#define TSC_MASCSR_IACK_DW         (1<<10)  /* VME IACK DW status_ID                  */
#define TSC_MASCSR_READ_SMPL_0     (0<<12)  /* Read sampling point +0ns               */
#define TSC_MASCSR_READ_SMPL_6     (1<<12)  /* Read sampling point +6ns               */
#define TSC_MASCSR_READ_SMPL_12    (2<<12)  /* Read sampling point +12ns              */
#define TSC_MASCSR_READ_SMPL_18    (3<<12)  /* Read sampling point +18ns              */
#define TSC_MASCSR_RMW_ENA         (1<<22)  /* Enable Read Modify Write               */
#define TSC_MASCSR_VTON(x)     ((x-2)<<25)  /* VME Master Timer ON                    */
#define TSC_MASCSR_VTON_MASK       (7<<25)  /* VME Master Timer ON                    */
#define TSC_MASCSR_VTOFF(x)        (x<<28)  /* VME Master Timer OFF                   */
#define TSC_MASCSR_VTOFF_MASK      (7<<28)  /* VME Master Timer OFF                   */
#define TSC_MASCSR_ENA             (1<<31)  /* Enable Master port                     */

#define TSC_MASCSR_REQMOD_GET(x)       (x&3)  /* get ARB Request mode                  */
#define TSC_MASCSR_REQMOD_SET(x)       (x&3)  /* set ARB Request mode                  */
#define TSC_MASCSR_REQLEV_GET(x)  ((x>>2)&3)  /* get ARB Request level                 */
#define TSC_MASCSR_REQLEV_SET(x)  ((x&3)<<2)  /* set ARB Request level                 */

/*
 *  VME slave A32 Port Control and Status(CSR + $408)
 */
#define TSC_SLVCSR_SIZE_MASK     (0xf<<0)  /* Mask for window size bitfield      */
#define TSC_SLVCSR_NORETRY         (1<<4)  /* Don't perform slave retry (legacy) */
#define TSC_SLVCSR_WP_1k           (0<<6)  /* Write postion burst size 1k        */
#define TSC_SLVCSR_WP_512          (1<<6)  /* Write postion burst size 512       */
#define TSC_SLVCSR_WP_256          (2<<6)  /* Write postion burst size 256       */
#define TSC_SLVCSR_WP_128          (3<<6)  /* Write postion burst size 128       */
#define TSC_SLVCSR_RRSP_INV        (1<<8)  /* Read Response Invalid              */
#define TSC_SLVCSR_RRSP_UEX        (1<<9)  /* Read Response Unexpected           */
#define TSC_SLVCSR_RRSP_WA        (1<<10)  /* Read Response Wrong Acknowledge    */
#define TSC_SLVCSR_RRSP_OOD       (1<<11)  /* Read Response Out Of Date          */
#define TSC_SLVCSR_ROT_MASK    (0xff<<16)  /* Mask for rotary encoder            */
#define TSC_SLVCSR_AUTO_ADER      (1<<24)  /* use Rotary encode to set vme base  */
#define TSC_SLVCSR_ENA            (1<<31)  /* Enable Master port      */

#define TSC_SLVCSR_SIZE(x)    ((x&8)?(0x100000<<(x&7)):(0x1000000<<(x&7))) /*VME A32 slave size */
#define TSC_SLVCSR_A32_PG        0x100000  /* Page size 1 MByte                 */
#define TSC_SLVCSR_A32_SIZE_MAX  0x80000000  /* Max slave size 4*512 MBytes       */

/*
 *  VME Interrupt Generator(CSR + $40c)
 */
#define TSC_INTG_STATUS_MASK     (0xff<<0)   /* Mask for interrupt vector     */
#define TSC_INTG_LEVEL_MASK       (0x7<<8)   /* Mask for interrupt level      */
#define TSC_INTG_LEVEL_1          (0x1<<8)   /* Interrupt level 1             */
#define TSC_INTG_LEVEL_2          (0x2<<8)   /* Interrupt level 2             */
#define TSC_INTG_LEVEL_3          (0x3<<8)   /* Interrupt level 3             */
#define TSC_INTG_LEVEL_4          (0x4<<8)   /* Interrupt level 4             */
#define TSC_INTG_LEVEL_5          (0x5<<8)   /* Interrupt level 5             */
#define TSC_INTG_LEVEL_6          (0x6<<8)   /* Interrupt level 6             */
#define TSC_INTG_LEVEL_7          (0x7<<8)   /* Interrupt level 7             */
#define TSC_INTG_IP               (0xf<<11)  /* Interrupt pending             */
#define TSC_INTG_CMD_SET          (0x1<<12)  /* Set VME interrupt             */
#define TSC_INTG_CMD_CLEAR        (0x2<<12)  /* Clear VME interrupt           */
#define TSC_INTG_CMD_ID1          (0x9<<12)  /* Init Status-ID[15:8]          */
#define TSC_INTG_CMD_ID2          (0xa<<12)  /* Init Status-ID[15:8]          */
#define TSC_INTG_CMD_ID3          (0xb<<12)  /* Init Status-ID[15:8]          */
#define TSC_INTG_CMD_ID3          (0xb<<12)  /* Init Status-ID[15:8]          */
#define TSC_INTG_RORA               (1<<22)  /* Interrupt Aknowledge RORA     */
#define TSC_INTG_INTPD_MASK      (0xfe<<25)  /* Mask for interrupt pending    */
#define TSC_INTG_STATUS_SET(x)     (x&0xff)  /* Set Interrupt vector x        */
#define TSC_INTG_STATUS_GET(x)     (x&0xff)  /* Set Interrupt vector x        */
#define TSC_INTG_LEVEL_SET(x)    ((x&7)<<8)  /* Set Interrupt level x         */
#define TSC_INTG_LEVEL_GET(x)    ((x>>8)&7)  /* Get Interrupt level x         */
#define TSC_INTG_INTPD_GET(x)((x&0xfe)>>25)  /* get interrupt pending status  */

static const int TSC_INTG_LEVEL[8] = { 0, TSC_INTG_LEVEL_1,
			TSC_INTG_LEVEL_2, TSC_INTG_LEVEL_3,
			TSC_INTG_LEVEL_4, TSC_INTG_LEVEL_5,
			TSC_INTG_LEVEL_6, TSC_INTG_LEVEL_7 };
/*
 *  VME Slave MMU parameters
 */      
#define TSC_MMU_A32_PG_NUM                0x200  /* 512 MMU pages for VME A32 Slave            */
#define TSC_MMU_A32_PG_SIZE            0x100000  /* 1 MBytes page size for VME for A32 Slave   */
#define TSC_MMU_A24_PG_NUM                0x200  /* 512 MMU pages for VME A24 Slave            */
#define TSC_MMU_A24_PG_SIZE              0x1000  /* 4 KBytes page size for VME for A24 Slave   */
#define TSC_MMU_A16_PG_NUM                0x200  /* 512 MMU pages for VME A16 Slave            */
#define TSC_MMU_A16_PG_SIZE               0x100  /* 256 Bytes page size for VME for A16 Slave  */
#define TSC_MMU_2eBRC_PG_NUM              0x200  /* 512 MMU pages for VME 2eBRC Slave          */
#define TSC_MMU_2eBRC_PG_SIZE          0x400000  /* 4 MBytes page size for VME for 2eBRC Slave */

/*
 *  VME Slave MMU address pointer(CSR + $410)
 */
#define TSC_MMUADD_PTR_MASK               (0xff8<<0)  /* Mask for MMU pointer (512 pages) */
#define TSC_MMUADD_A32_SEL                   (0<<16)  /* Select MMU for A32               */
#define TSC_MMUADD_A24_SEL                   (1<<16)  /* Select MMU for A32               */
#define TSC_MMUADD_A16_SEL                   (2<<16)  /* Select MMU for A32               */
#define TSC_MMUADD_2eBRC_SEL                 (3<<16)  /* Select MMU for BCR               */

/*
 *  VME Slave MMU data register(CSR + $410)
 */
#define TSC_MMUDAT_PG_ENA               (1<<0)  /* Enable page               */
#define TSC_MMUDAT_WR_ENA               (1<<1)  /* Enable write              */
#define TSC_MMUDAT_TC_MASK              (3<<2)  /* Mask for traffic class    */
#define TSC_MMUDAT_SP                   (1<<4)  /* Accept only spervisory AM */
#define TSC_MMUDAT_SWAP_AUTO            (1<<6)  /* set auto swap be <-> le   */
#define TSC_MMUDAT_SWAP_DW              (2<<6)  /* set double word swap      */
#define TSC_MMUDAT_SWAP_MASK            (3<<6)  /* set double word swap      */
#define TSC_MMUDAT_DES_PCIE            (0<<12)  /* destination PCI Express   */
#define TSC_MMUDAT_DES_SHM             (2<<12)  /* destination Shared Memory */
#define TSC_MMUDAT_DES_USR             (3<<12)  /* destination User Block    */
#define TSC_MMUDAT_DES_MASK            (f<<12)  /* destination User Block    */
#define TSC_MMUDAT_RPF_32               (0<<0)  /* Read Prefetch 32 bytes    */
#define TSC_MMUDAT_RPF_64               (1<<0)  /* Read Prefetch 64 bytes    */
#define TSC_MMUDAT_RPF_128              (2<<0)  /* Read Prefetch 128 bytes   */
#define TSC_MMUDAT_RPF_256              (3<<0)  /* Read Prefetch 256 bytes   */
#define TSC_MMUDAT_RPF_MASK             (3<<0)  /* Read Prefetch mask        */
#define TSC_MMUDAT_SNOOP_NO             (1<<2)  /* No snoop                  */
#define TSC_MMUDAT_RELAX                (1<<3)  /* Relax Ordering            */
#define TSC_MMUDAT_ADD_MASK         (0xfff<<4)  /* destination address mask  */

/*
 *  VME Master Error Status (CSR + $420)
 */
#define TSC_STAERR_BTO                 (1<<29)   /* VME master error Bus Timeout       */
#define TSC_STAERR_WR                  (1<<30)   /* VME master error write             */
#define TSC_STAERR_VEOF                (1<<31)   /* VME master error overflow          */

/*
 *  VME Location Monitor (CSR + $420)
 */
#define TSC_LOCMON_AS_MASK                (3<<0)  /*  Location Monitor Mask                  */
#define TSC_LOCMON_AS_A16                 (1<<0)  /*  Enable Location Monitor A16            */
#define TSC_LOCMON_AS_A24                 (2<<0)  /*  Enable Location Monitor A24            */
#define TSC_LOCMON_AS_A32                 (3<<0)  /*  Enable Location Monitor A32            */
#define TSC_LOCMON_ADDR_MASK     (0xffffffe0<<0)  /*  Location Monitor Address Masl          */

/*
 *  Global Timer (CSR + $440)
 */
#define TSC_GLTIM_1MHZ         (0<<0)  /* timer frequency 1 MHz                  */
#define TSC_GLTIM_5MHZ         (1<<0)  /* timer frequency 5 MHz                  */
#define TSC_GLTIM_25MHZ        (2<<0)  /* timer frequency 25 MHz                 */
#define TSC_GLTIM_100MHZ       (3<<0)  /* timer frequency 100 MHz                */
#define TSC_GLTIM_BASE_1000    (0<<3)  /* timer period 1000 usec                 */
#define TSC_GLTIM_BASE_1024    (1<<3)  /* timer period 1024 usec                 */
#define TSC_GLTIM_SYNC_LOC     (0<<4)  /* timer synchronization local            */
#define TSC_GLTIM_SYNC_USR1    (1<<4)  /* timer synchronization user signal #1   */
#define TSC_GLTIM_SYNC_USR2    (2<<4)  /* timer synchronization user signal #2   */
#define TSC_GLTIM_SYNC_SYSFAIL (4<<4)  /* timer synchronization sysfail          */
#define TSC_GLTIM_SYNC_IRQ1    (5<<4)  /* timer synchronization IRQ#1            */
#define TSC_GLTIM_SYNC_IRQ2    (6<<4)  /* timer synchronization IRQ#2            */
#define TSC_GLTIM_SYNC_ENA     (1<<7)  /* timer synchronization enable           */
#define TSC_GLTIM_OUT_SYSFAIL  (1<<8)  /* issue sync signal on sysfail           */
#define TSC_GLTIM_OUT_IRQ1     (2<<8)  /* issue sync signal on IRQ#1             */
#define TSC_GLTIM_OUT_IRQ2     (3<<8)  /* issue sync signal on IRQ#2             */
#define TSC_GLTIM_SYNC_ERR    (1<<16)  /* timer synchronization error            */
#define TSC_GLTIM_ENA         (1<<31)  /* timer global enable                    */

/*
 *  interrupt IACK (CSR + $480)
 */

/*
 *  interrupt CSR (CSR + $484)
 */
#define TSC_ITC_CSR_             0x484


/*
 *  interrupt MASK (CSR + $488/C)
 */
#define TSC_ITC_IM_SYSFAIL             (1<<0)  /*  SYSFAIL interrupt              */
#define TSC_ITC_IM_IRQ1                (1<<1)  /*  IRQ1 interrupt                 */
#define TSC_ITC_IM_IRQ2                (1<<2)  /*  IRQ2 interrupt                 */
#define TSC_ITC_IM_IRQ3                (1<<3)  /*  IRQ3 interrupt                 */
#define TSC_ITC_IM_IRQ4                (1<<4)  /*  IRQ4 interrupt                 */
#define TSC_ITC_IM_IRQ5                (1<<5)  /*  IRQ5 interrupt                 */
#define TSC_ITC_IM_IRQ6                (1<<6)  /*  IRQ6 interrupt                 */
#define TSC_ITC_IM_IRQ7                (1<<7)  /*  IRQ7 interrupt                 */
#define TSC_ITC_IM_ACFAIL              (1<<8)  /*  ACFAIL interrupt               */
#define TSC_ITC_IM_MASERR              (1<<9)  /*  Master Error interrupt         */
#define TSC_ITC_IM_GTIM0              (1<<10)  /*  Global Time interrupt 0        */
#define TSC_ITC_IM_GTIM1              (1<<11)  /*  Global Time interrupt 1        */
#define TSC_ITC_IM_LM0                (1<<12)  /*  Location Monitor 0 interrupt   */
#define TSC_ITC_IM_LM1                (1<<13)  /*  Location Monitor 1 interrupt   */
#define TSC_ITC_IM_LM2                (1<<14)  /*  Location Monitor 2 interrupt   */
#define TSC_ITC_IM_LM3                (1<<15)  /*  Location Monitor 3 interrupt   */
#define TSC_ITC_IM_LM_ALL           (0xf<<12)  /*  Location Monitor all interrupt */
#define TSC_ITC_IM_VME_ALL             0xffff  /*  All interrupts                 */

static const int TSC_ITC_IM_LM[4] = { TSC_ITC_IM_LM0,
						  TSC_ITC_IM_LM1,
						  TSC_ITC_IM_LM2,
						  TSC_ITC_IM_LM3};

static const int TSC_ITC_IM_IRQ[7] = { TSC_ITC_IM_IRQ1,
						   TSC_ITC_IM_IRQ2,
						   TSC_ITC_IM_IRQ3,
						   TSC_ITC_IM_IRQ4,
						   TSC_ITC_IM_IRQ5,
						   TSC_ITC_IM_IRQ6,
						   TSC_ITC_IM_IRQ7};


/*
 *  CRCSR BCR/BSR (CSR + $5F4/8)
 */
#define TSC_CRCSR_RESET             (1<<0)
#define TSC_CRCSR_SYSFAIL_ENA       (1<<1)
#define TSC_CRCSR_SYSFAIL           (1<<2)
#define TSC_CRCSR_BERR              (1<<3)
#define TSC_CRCSR_SLV_ENA           (1<<4)

/*
 *  DDR3 CSR (CSR + $800/C)
 */
#define TSC_SMEM_DDR3_SIZE(x) ((x&0xc)?(0x8000000<<((x>>2)&3)):0)  /* calculate SMEM DDR3 size            */

/*
 *  SRAM CSR (CSR + $810/C)
 */
//#define TSC_SMEM_RAM_SIZE(x) ((x&0xc)?(0x10000<<((x>>2)&3)):0)  /* calculate SMEM RAM size            */
#define TSC_SMEM_RAM_SIZE(x) (0x10000<<((x>>2)&3))  /* calculate SMEM RAM size            */

/*
 *  IDMA Pipeline control bits (CSR + $850/C)
 */
#define TSC_IDMA_PCSR_PIPE_CNT(x)	        (x&0x3f)  /* PIPE counter (read only) */
#define TSC_IDMA_PCSR_PIPE_MAX_4 	          (0<<6)  /*  4 ring buffer entries   */
#define TSC_IDMA_PCSR_PIPE_MAX_8 	          (1<<6)  /*  8 ring buffer entries   */
#define TSC_IDMA_PCSR_PIPE_MAX_16 	          (2<<6)  /* 16 ring buffer entries   */
#define TSC_IDMA_PCSR_PIPE_MAX_32 	          (3<<6)  /* 32 ring buffer entries   */
#define TSC_IDMA_PCSR_TRIG_IN_RD0 	          (0<<8)  /* Trig by read engine 0    */
#define TSC_IDMA_PCSR_TRIG_IN_RD1 	          (1<<8)  /* Trig by read engine 1    */
#define TSC_IDMA_PCSR_TRIG_IN_WR0 	          (2<<8)  /* Trig by write engine 0   */
#define TSC_IDMA_PCSR_TRIG_IN_WR1 	          (3<<8)  /* Trig by write engine 1   */
#define TSC_IDMA_PCSR_PIPE_RESET 	         (1<<13)  /* Reset PIPE               */
#define TSC_IDMA_PCSR_PIPE_MODE_1 	         (0<<14)  /* select PIPE mode 1       */
#define TSC_IDMA_PCSR_PIPE_MODE_2 	         (1<<14)  /* select PIPE mode 2       */
#define TSC_IDMA_PCSR_PIPE_ENABLE 	         (1<<15)  /* enable PIPE mode          */

/*
 *  IDMA interrupt MASK (CSR + $888/C)
 */
#define TSC_IDMA_ITC_IM_RD0_END          (1<<0)  /* Read engine #0 ended              */
#define TSC_IDMA_ITC_IM_RD0_ERR          (1<<1)  /* Read engine #0 error              */
#define TSC_IDMA_ITC_IM_RD0              (3<<0)  /* Read engine #0                    */
#define TSC_IDMA_ITC_IM_RD1_END          (1<<2)  /* Read engine #1 ended              */
#define TSC_IDMA_ITC_IM_RD1_ERR          (1<<3)  /* Read engine #1 error              */
#define TSC_IDMA_ITC_IM_RD1              (3<<2)  /* Read engine #1                    */
#define TSC_IDMA_ITC_IM_WR0_END          (1<<4)  /* Write engine #0 ended             */
#define TSC_IDMA_ITC_IM_WR0_ERR          (1<<5)  /* Write engine #0 error             */
#define TSC_IDMA_ITC_IM_WR0              (3<<4)  /* Write engine #0                   */
#define TSC_IDMA_ITC_IM_WR1_END          (1<<6)  /* Write engine #1 ended             */
#define TSC_IDMA_ITC_IM_WR1_ERR          (1<<7)  /* Write engine #1 error             */
#define TSC_IDMA_ITC_IM_WR1              (3<<6)  /* Write engine #1                   */
#define TSC_IDMA_ITC_IM_CHAN0         (0x33<<0)  /* Channel #0 event                  */
#define TSC_IDMA_ITC_IM_CHAN1         (0xcc<<0)  /* Channel #1 event                  */

/*
 *  IDMA Control Registers (CSR + $900/$940/$a00/$a40)
 */
#define TSC_IDMA_CSR_CACHE_ENA	       (1<<27)  /* DMA Engine enabled              */
#define TSC_IDMA_CSR_SUSPEND	       (1<<28)  /* DMA Engine enabled              */
#define TSC_IDMA_CSR_ABORT	       (1<<29)  /* DMA Engine enabled              */
#define TSC_IDMA_CSR_KILL	       (1<<30)  /* DMA Engine enabled              */
#define TSC_IDMA_CSR_ENA	               (1<<31)  /* DMA Engine enabled              */

#define TSC_IDMA_NDES_START_NOW          (0<<0)  /* Start now               */
#define TSC_IDMA_NDES_WAIT_TIMER         (1<<0)  /* Wait for timer sync     */
#define TSC_IDMA_NDES_WAIT_GPIO_1        (2<<0)  /* Wait for GPIO #1        */
#define TSC_IDMA_NDES_WAIT_GPIO_2        (3<<0)  /* Wait for GPIO #2        */
#define TSC_IDMA_NDES_WAIT_TRIG_1        (4<<0)  /* Wait for DMA trig #1    */
#define TSC_IDMA_NDES_WAIT_TRIG_2        (5<<0)  /* Wait for DMA trig #2    */
#define TSC_IDMA_NDES_WAIT_TRIG_3        (6<<0)  /* Wait for DMA trig #3    */
#define TSC_IDMA_NDES_WAIT_TRIG_4        (7<<0)  /* Wait for DMA trig #4    */

#define TSC_IDMA_DCNT_STATUS  	        (7<<29)  /* DMA Status                   */
#define TSC_IDMA_DCNT_IDLE  	        (0<<29)  /* DMA Status IDLE              */
#define TSC_IDMA_DCNT_INPROGRESS  	(3<<29)  /* DMA Status in progress       */
#define TSC_IDMA_DCNT_ENDED  	        (4<<29)  /* DMA Status ENDED             */

/*
 *  IDMA descriptors
 */
#define TSC_IDMA_DES0_                    (0<<0)  /*       */
#define TSC_IDMA_DES0_NOINC               (2<<1)  /* No address increment          */
#define TSC_IDMA_DES0_DMAERR              (1<<3)  /* Continue with next on error   */
#define TSC_IDMA_DES0_SNOOP               (1<<4)  /* Enable snooping (PCIe)        */
#define TSC_IDMA_DES0_RELAX               (1<<5)  /* Enable relax ordering (PCIe)  */
#define TSC_IDMA_DES0_SMEM_ADD_NORMAL     (0<<8)  /* Normal Update                 */
#define TSC_IDMA_DES0_SMEM_ADD_NONE       (1<<8)  /* No Update, use current        */
#define TSC_IDMA_DES0_SMEM_ADD_ADD        (2<<8)  /* Add Update, current + SMEM ADD*/
#define TSC_IDMA_DES0_TRIGOUT_NO         (0<<10)  /* dont't generate trig out      */
#define TSC_IDMA_DES0_TRIGOUT_START      (2<<10)  /* generate trig out at start    */
#define TSC_IDMA_DES0_TRIGOUT_END        (3<<10)  /* generate trig out at end      */
#define TSC_IDMA_DES0_UPDATE_TIME        (2<<12)  /* update status with nsec time  */
#define TSC_IDMA_DES0_UPDATE_WCNT        (3<<12)  /* update status with word cnt   */
#define TSC_IDMA_DES0_INTR_DIS           (0<<14)  /* Don't generate interrupt      */
#define TSC_IDMA_DES0_INTR_ERR           (1<<14)  /* Generate interrupt at start   */
#define TSC_IDMA_DES0_INTR_END           (2<<14)  /* Generate interrupt at end     */
#define TSC_IDMA_DES0_INTR_ERR_END       (3<<14)  /* Generate interrupt on error   */
#define TSC_IDMA_DES0_ADDBND_4k          (0<<16)  /* 4 KBytes address boundary     */
#define TSC_IDMA_DES0_ADDBND_2k          (1<<16)  /* 2 KBytes address boundary     */
#define TSC_IDMA_DES0_ADDBND_1k          (2<<16)  /* 1 KBytes address boundary     */
#define TSC_IDMA_DES0_ADDBND_256         (3<<16)  /* 256 Bytes address boundary    */
#define TSC_IDMA_DES0_PSIZE_128          (0<<18)  /* 128 Bytes payload size        */
#define TSC_IDMA_DES0_PSIZE_256          (1<<18)  /* 256 Bytes payload size        */
#define TSC_IDMA_DES0_PSIZE_512          (2<<18)  /* 512 Bytes payload size        */
#define TSC_IDMA_DES0_PSIZE_1k           (3<<18)  /* 1 KBytes payload size         */
#define TSC_IDMA_DES0_SWAP_AUTO      	(1<<26)  /* Automatic swapping            */
#define TSC_IDMA_DES0_SWAP_DW       	 (2<<26)  /* Double word swapping          */
#define TSC_IDMA_DES0_SWAP_QW   	     (3<<26)  /* Quad word swapping            */
#define TSC_IDMA_DES0_SHM_SWAP_AUTO      (1<<28)  /* Automatic swapping            */
#define TSC_IDMA_DES0_SHM_SWAP_DW        (2<<28)  /* Double word swapping          */
#define TSC_IDMA_DES0_SHM_SWAP_QW        (3<<28)  /* Quad word swapping            */

#define TSC_IDMA_DES1_WC_MASK       (0xfffffc<<0)  /* word count mask               */
#define TSC_IDMA_DES1_DES_PCIe            (0<<24)  /* remote space is PCIe          */
#define TSC_IDMA_DES1_DES_SHM2            (2<<24)  /* remote space is SHM #2        */
#define TSC_IDMA_DES1_DES_USER            (3<<24)  /* remote space is USER          */
#define TSC_IDMA_DES1_AS_CRCSR            (0<<28)  /* remote mode configuration */
#define TSC_IDMA_DES1_AS_USER             (7<<28)  /* remote mode USER          */
#define TSC_IDMA_DES1_AS_IACK            (15<<28)  /* remote mode IACK          */
#define TSC_IDMA_DES1_AS_MASK           (0xf<<28)  /* remote mode IACK          */

#define TSC_IDMA_DES2_BURST_2k             (0<<0)  /* burst size 2 KBytes           */
#define TSC_IDMA_DES2_BURST_1k             (1<<0)  /* burst size 1 KBytes           */
#define TSC_IDMA_DES2_BURST_512            (2<<0)  /* burst size 512 Bytes          */
#define TSC_IDMA_DES2_BURST_256            (3<<0)  /* burst size 256 Bytes          */
#define TSC_IDMA_DES2_ADDR_MASK   (0xfffffffc<<0)  /* SHM address mask              */

#define TSC_IDMA_DES3_START_NOW             (0<<0)  /* Start now       */
#define TSC_IDMA_DES3_START_GTS             (1<<0)  /* Wait Global Time Synchro      */
#define TSC_IDMA_DES3_START_EVT1            (2<<0)  /* Wait for Event #1             */
#define TSC_IDMA_DES3_START_EVT2            (3<<0)  /* Wait for Event #2             */
#define TSC_IDMA_DES3_START_IN1             (4<<0)  /* Wait for ON #1                */
#define TSC_IDMA_DES3_START_IN2             (5<<0)  /* Wait for ON #2                */
#define TSC_IDMA_DES3_START_IN3             (6<<0)  /* Wait for ON #3                */
#define TSC_IDMA_DES3_START_IN4             (7<<0)  /* Wait for ON #4                */
#define TSC_IDMA_DES3_LAST                  (1<<4)  /* Last descriptor               */
#define TSC_IDMA_DES3_NEXT_MASK    (0xffffffe0<<5)  /* Next Descriptor address mask   */

#define TSC_IDMA_DES4_       (1<<0)  /*       */

#define TSC_IDMA_DES5_       (1<<0)  /*       */

#define TSC_IDMA_DES6_       (1<<0)  /*       */

#define TSC_IDMA_DES7_       (1<<0)  /*       */

/*
 *  USER interrupt source
 */
#define TSC_ITC_SRC_USER1_0      0x40
#define TSC_ITC_SRC_USER1_1      0x41
#define TSC_ITC_SRC_USER1_2      0x42
#define TSC_ITC_SRC_USER1_3      0x43
#define TSC_ITC_SRC_USER1_4      0x44
#define TSC_ITC_SRC_USER1_5      0x45
#define TSC_ITC_SRC_USER1_6      0x46
#define TSC_ITC_SRC_USER1_7      0x47

/*
 *  USER interrupt MASK (CSR + $1088/1488)
 */
#define TSC_USER_ITC_IM_0          (1<<0)  /* User interrupt 0 */
#define TSC_USER_ITC_IM_1          (1<<1)  /* User interrupt 1 */
#define TSC_USER_ITC_IM_2          (1<<2)  /* User interrupt 2 */
#define TSC_USER_ITC_IM_3          (1<<3)  /* User interrupt 3 */
#define TSC_USER_ITC_IM_4          (1<<4)  /* User interrupt 4 */
#define TSC_USER_ITC_IM_5          (1<<5)  /* User interrupt 5 */
#define TSC_USER_ITC_IM_6          (1<<6)  /* User interrupt 6 */
#define TSC_USER_ITC_IM_7          (1<<7)  /* User interrupt 7 */

/*
 *  FIFO CTL
 */
#define TSC_FIFO_CTL_WCNT_MAX	              255  /* FIFO word counter maximum       */
#define TSC_FIFO_CTL_WCNT_MASK	        0x000000ff  /* FIFO word counter mask       */
#define TSC_FIFO_CTL_WCNT(x)	          (x&0xff)  /* FIFO word counter            */
#define TSC_FIFO_CTL_WRPT_MASK	        0x0000ff00  /* FIFO write pointer mask      */
#define TSC_FIFO_CTL_WRPT(x)	     ((x>>8)&0xff)  /* FIFO write pointer           */
#define TSC_FIFO_CTL_RDPT_MASK	        0x00ff0000  /* FIFO read pointer mask       */
#define TSC_FIFO_CTL_RDPT(x)	    ((x>>16)&0xff)  /* FIFO read pointer            */
#define TSC_FIFO_CTL_NOEMPTY	           (1<<24)  /* FIFO not empty               */
#define TSC_FIFO_CTL_FULL	            	(1<<25)  /* FIFO full                    */
#define TSC_FIFO_CTL_MBX	                (1<<26)  /* FIFO mailbox mode            */
#define TSC_FIFO_CTL_ERRF	           		(1<<27)  /* FIFO error                   */
#define TSC_FIFO_CTL_RESET	           		(1<<28)  /* FIFO reset                   */
#define TSC_FIFO_CTL_REA	                   (1<<29)  /* FIFO read enable             */
#define TSC_FIFO_CTL_WEA	                   (1<<30)  /* FIFO write enable            */
#define TSC_FIFO_CTL_ENA	                   (1<<31)  /* FIFO enable                  */

#endif	/* _H_TSC */
