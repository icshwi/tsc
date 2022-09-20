/*=========================< begin file & file header >=======================
 *  References
 *  
 *    filename : scope1430lib.h
 *    author   : JFG, XP, CG
 *    company  : IOxOS
 *    creation : january 05, 2020
 *
 *----------------------------------------------------------------------------
 *  Description
 *
 *    That library contains a set of function to access the TOSCA II XUSER
 *    SCOPE DAQ1430 data acquisition logic.
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

#ifndef _H_SCOPE1430LIB
#define _H_SCOPE1430LIB

/* ------------------------------------------------------------------------------------------------------------------ */
/*  Definition                                                                                                        */
/* ------------------------------------------------------------------------------------------------------------------ */

#define SCOPE1430_SIGN        0x14110301    /* XUSER_SCOPE_1430 signature                                             */

#define SCOPE1430_CSR_BASE    0x00001180    /* CSR base address for XUSER scope                                       */

#define SCOPE1430_CSR_ADDR(csr)             (SCOPE1430_CSR_BASE |((csr&0x1f)<<2))

/* ------------------------------------------------------------------------------------------------------------------ */
/* Register Index
 */

#define SCOPE1430_CSR_SIGN          0x60    /* XUSER SCOPE signature                                                  */
#define SCOPE1430_CSR_ACQ1_CS       0x61    /* Acquisition Control / Status register #1 for channel 0-7               */
#define SCOPE1430_CSR_ACQ2_CS       0x62    /* Acquisition Control / Status register #2 for channel 8-9               */
#define SCOPE1430_CSR_CTL           0x63    /* Acquisition General Control / Status register                          */
#define SCOPE1430_CSR_DISCR1_CS     0x64    /* Discriminator Control / Status register for channel 0-7                */
#define SCOPE1430_CSR_DISCR2_CS     0x65    /* Discriminator Control / Status register for channel 8-9                */
              
#define SCOPE1430_CSR_INTR_STA      0x68    /* Interrupt Status                                                       */
#define SCOPE1430_CSR_INTR_CTL      0x69    /* interrupt Control                                                      */
              
#define SCOPE1430_CSR_SRAM1_CS      0x70    /* SRAM1 Acquisition Control / Status                                     */
#define SCOPE1430_CSR_SRAM1_TRIG    0x71    /* SRAM1 Trigger Control / Status                                         */
#define SCOPE1430_CSR_SRAM1_ADDR    0x72    /* SRAM1 Last address pre-trigger                                         */
              
#define SCOPE1430_CSR_SRAM2_CS      0x74    /* SRAM2 Acquisition Control / Status                                     */
#define SCOPE1430_CSR_SRAM2_TRIG    0x75    /* SRAM2 Trigger Control / Status                                         */
#define SCOPE1430_CSR_SRAM2_ADDR    0x76    /* SRAM2 Last address pre-trigger                                         */
              
#define SCOPE1430_CSR_SMEM1_CS      0x78    /* SMEM1 Acquisition Control / Status                                     */
#define SCOPE1430_CSR_SMEM1_TRIG    0x79    /* SMEM1 Trigger Control / Status                                         */
#define SCOPE1430_CSR_SMEM1_ADDR    0x7A    /* SMEM1 Last address pre-trigger                                         */
#define SCOPE1430_CSR_SMEM1_BASE    0x7B    /* SMEM1 Buffer base address & size                                       */
              
#define SCOPE1430_CSR_SMEM2_CS      0x7C    /* SMEM2 Acquisition Control / Status                                     */
#define SCOPE1430_CSR_SMEM2_TRIG    0x7D    /* SMEM2 Trigger Control / Status                                         */
#define SCOPE1430_CSR_SMEM2_ADDR    0x7E    /* SMEM2 Last address pre-trigger                                         */
#define SCOPE1430_CSR_SMEM2_BASE    0x7F    /* SMEM2 Buffer base address & size                                       */

/* ------------------------------------------------------------------------------------------------------------------ */
/* Scope Unit
 */

#define SCOPE1430_SMEM                0
#define SCOPE1430_SRAM                1

#define SCOPE1430_UNIT_SRAM1          0     /* SRAM1 Scope Unit (channel 0-7)                                         */
#define SCOPE1430_UNIT_SRAM2          1     /* SRAM2 Scope Unit (channel 8-9)                                         */
#define SCOPE1430_UNIT_SMEM1          2     /* SMEM1 Scope Unit (channel 0-7)                                         */
#define SCOPE1430_UNIT_SMEM2          3     /* SMEM2 Scope Unit (channel 8-9)                                         */

#define SCOPE1430_UNIT(chan, type)    (((type!=0) ? 0 : 2)+ ((chan<8) ? 0 : 1))

/* ------------------------------------------------------------------------------------------------------------------ */
/* Scope SMEM Burst Size 
 */
#define SCOPE1430_SMEM_BURST_512    (0<<8)    /*  512 bytes */
#define SCOPE1430_SMEM_BURST_1K     (1<<8)    /* 1024 bytes */
#define SCOPE1430_SMEM_BURST_2K     (2<<8)    /* 2048 bytes */
#define SCOPE1430_SMEM_BURST_4K     (3<<8)    /* 4096 bytes */

/* ------------------------------------------------------------------------------------------------------------------ */
/* Scope SRAM Buffer Size 
 */

#define SCOPE1430_BUF_SIZE_64K        0     /* 64 KBytes / 32 KSamples                                                */
#define SCOPE1430_BUF_SIZE_32K        1     /* 32 KBytes / 16 KSamples                                                */
#define SCOPE1430_BUF_SIZE_16K        2     /* 16 KBytes /  8 KSamples                                                */
#define SCOPE1430_BUF_SIZE_8K         3     /*  8 KBytes /  4 KSamples                                                */
#define SCOPE1430_BUF_SIZE_4K         4     /*  4 KBytes /  2 KSamples                                                */
#define SCOPE1430_BUF_SIZE_2K         5     /*  2 KBytes /  1 KSamples                                                */

/* ------------------------------------------------------------------------------------------------------------------ */
/* Scope SRAM Buffer Offset & Size 
 */

#define SCOPE1430_MAP_USR_OFF     0x100000  /* Buffer start address in USR1 or USR2 space for SRAM1 & SRAM2           */
#define SCOPE1430_MAP_USR_SIZE    0x100000  /* Maximum Buffer size in USR1 or USR2 space for SRAM1 & SRAM2            */

/* ------------------------------------------------------------------------------------------------------------------ */
/* Down-Sampling Factor 
 */

/* used for SMEM legacy mode & SRAM mode */
#define SCOPE1430_DS_1_1      0   /* 1:  1                        */
#define SCOPE1430_DS_1_2      1   /* 1:  2                        */
#define SCOPE1430_DS_1_5      2   /* 1:  5                        */
#define SCOPE1430_DS_1_10     3   /* 1: 10                        */
#define SCOPE1430_DS_1_20     4   /* 1: 20                        */
#define SCOPE1430_DS_1_50     5   /* 1: 50                        */
#define SCOPE1430_DS_1_100    6   /* 1:100                        */
#define SCOPE1430_DS_1_200    7   /* 1:200                        */

/* used for SMEM new mode & SRAM mode */
#define SCOPE1430_DS_1_4      9   /* 1:  4 averaging              */
#define SCOPE1430_DS_1_8     10   /* 1:  8 averaging              */
#define SCOPE1430_DS_1_16    11   /* 1: 15 averaging              */
#define SCOPE1430_DS_1_32    12   /* 1: 32 averaging              */
#define SCOPE1430_DS_1_64    13   /* 1: 64 averaging              */
#define SCOPE1430_DS_1_128   14   /* 1:128 averaging              */
#define SCOPE1430_DS_1_256   15   /* 1:256 averaging              */


/* ------------------------------------------------------------------------------------------------------------------ */
/* Scope Status
 */
#define SCOPE1430_STATUS_IDLE       (0<<28)   /* Acquisition is stopped               */
#define SCOPE1430_STATUS_RUN_PRE    (1<<28)   /* Acquisition is running pre-trigger   */
#define SCOPE1430_STATUS_RUN_POST   (2<<28)   /* Acquisition is running post-trigger  */
#define SCOPE1430_STATUS_DONE       (3<<28)   /* Acquisition has ended                */
#define SCOPE1430_STATUS_MASK       (3<<28)   /* Acquisition status mask              */

/* ------------------------------------------------------------------------------------------------------------------ */
/* Scope Control
 */
#define SCOPE1430_CMD_ARM           (1<<30)   /* ARM command              */
#define SCOPE1430_CMD_STOP          (2<<30)   /* STOP command             */
#define SCOPE1430_CMD_REARM         (3<<30)   /* REARM command            */
#define SCOPE1430_CMD_MASK          (3<<30)   /* Command mask             */

#define SCOPE1430_MODE_SINGLE       (1<<0)    /* Continous / Single       */
#define SCOPE1430_MODE_AUTO         (1<<1)    /* Normal / Auto            */
#define SCOPE1430_MODE_MASK         (3<<0) 

/* ------------------------------------------------------------------------------------------------------------------ */
/* Trigger Position (Pre-/Post-trigger)
 */
#define SCOPE1430_TRIG_POS_0_0  0   /* 0/8 pre-trigger / 8/8 post-trigger */
#define SCOPE1430_TRIG_POS_1_8  1   /* 1/8 pre-trigger / 7/8 post-trigger */
#define SCOPE1430_TRIG_POS_1_4  2   /* 2/8 pre-trigger / 6/8 post-trigger */
#define SCOPE1430_TRIG_POS_3_8  3   /* 3/8 pre-trigger / 5/8 post-trigger */
#define SCOPE1430_TRIG_POS_1_2  4   /* 4/8 pre-trigger / 4/8 post-trigger */
#define SCOPE1430_TRIG_POS_5_8  5   /* 5/8 pre-trigger / 3/8 post-trigger */
#define SCOPE1430_TRIG_POS_3_4  6   /* 6/8 pre-trigger / 2/8 post-trigger */
#define SCOPE1430_TRIG_POS_7_8  7   /* 7/8 pre-trigger / 1/8 post-trigger */

#define SCOPE1430_TRIG_POS_MASK 7

/* ------------------------------------------------------------------------------------------------------------------ */
/* Trigger Mode
 */
 
#define SCOPE1430_TRIG_MAN          0x40000000        /* Manual trigger in xxx_ADDR registers                         */

#define SCOPE1430_TRIG_ENABLE       (1<<31)           /* Enable Trigger Unit                                          */
#define SCOPE1430_TRIG_ADC(ch)      ((ch&7)<<28)      /* Trigger ADC channel source selection                         */
#define SCOPE1430_TRIG_POL_NEG      (0<<27)           /* Trigger polarity negative                                    */
#define SCOPE1430_TRIG_POL_POS      (1<<27)           /* Trigger polarity positive                                    */
#define SCOPE1430_TRIG_GPIO(x)      ((x&0x3F)<<20)    /* Trigger GPIO source selection                                */
#define SCOPE1430_TRIG_ADC_SEL      (0<<16)           /* Trigger source selection is ADC                              */
#define SCOPE1430_TRIG_GPIO_SEL     (3<<16)           /* Trigger source selection is GPIO                             */
#define SCOPE1430_TRIG_LEVEL(l)     (l&0xffff)        /* Trigger level (when ADC is used as a trigger source)         */

/* ------------------------------------------------------------------------------------------------------------------ */
/* ADC Channels
 */
#define SCOPE1430_TRIG_ADC_0        0 /* ADC channel #0 (on SRAM1 or SMEM1)                                           */
#define SCOPE1430_TRIG_ADC_1        1 /* ADC channel #1 (on SRAM1 or SMEM1)                                           */
#define SCOPE1430_TRIG_ADC_2        2 /* ADC channel #2 (on SRAM1 or SMEM1)                                           */
#define SCOPE1430_TRIG_ADC_3        3 /* ADC channel #3 (on SRAM1 or SMEM1)                                           */
#define SCOPE1430_TRIG_ADC_4        4 /* ADC channel #4 (on SRAM1 or SMEM1)                                           */
#define SCOPE1430_TRIG_ADC_5        5 /* ADC channel #5 (on SRAM1 or SMEM1)                                           */
#define SCOPE1430_TRIG_ADC_6        6 /* ADC channel #6 (on SRAM1 or SMEM1)                                           */
#define SCOPE1430_TRIG_ADC_7        7 /* ADC channel #7 (on SRAM1 or SMEM1)                                           */

#define SCOPE1430_TRIG_ADC_8        0 /* ADC channel #8 (on SRAM2 or SMEM2)                                           */
#define SCOPE1430_TRIG_ADC_9        1 /* ADC channel #9 (on SRAM2 or SMEM2)                                           */

/* ------------------------------------------------------------------------------------------------------------------ */
/* MTCA.4 AMC & ADF I/Os
 */

/* MTCA.4 AMC */
#define SCOPE1430_TRIG_MTCA_AMC_RX_17     0
#define SCOPE1430_TRIG_MTCA_AMC_RX_18     1
#define SCOPE1430_TRIG_MTCA_AMC_RX_19     2
#define SCOPE1430_TRIG_MTCA_AMC_RX_20     3
#define SCOPE1430_TRIG_MTCA_AMC_TX_17     4
#define SCOPE1430_TRIG_MTCA_AMC_TX_18     5
#define SCOPE1430_TRIG_MTCA_AMC_TX_19     6
#define SCOPE1430_TRIG_MTCA_AMC_TX_20     7
#define SCOPE1430_TRIG_MTCA_AMC_RX_12     8
#define SCOPE1430_TRIG_MTCA_AMC_RX_13     9
#define SCOPE1430_TRIG_MTCA_AMC_RX_14    10
#define SCOPE1430_TRIG_MTCA_AMC_RX_15    11
#define SCOPE1430_TRIG_MTCA_AMC_TX_12    12
#define SCOPE1430_TRIG_MTCA_AMC_TX_13    13
#define SCOPE1430_TRIG_MTCA_AMC_TX_14    14
#define SCOPE1430_TRIG_MTCA_AMC_TX_15    15

/* uRTM ADF */
#define SCOPE1430_TRIG_MTCA_ADF_IO_4     20
#define SCOPE1430_TRIG_MTCA_ADF_IO_5     21
#define SCOPE1430_TRIG_MTCA_ADF_IO_6     22
#define SCOPE1430_TRIG_MTCA_ADF_IO_7     23
#define SCOPE1430_TRIG_MTCA_ADF_IO_8     27

#define SCOPE1430_TRIG_MTCA_ADF_OUT_0    24
#define SCOPE1430_TRIG_MTCA_ADF_OUT_1    25

/* ------------------------------------------------------------------------------------------------------------------ */
/*  Function Prototypes                                                                                               */
/* ------------------------------------------------------------------------------------------------------------------ */

void scope1430_cleanup               (int fd);
int  scope1430_csr_rd                (int fd, int csr, int *data);
int  scope1430_csr_wr                (int fd, int csr, int data);
int  scope1430_identify              (int fd);

int  scope1430_smem_init             (int fd, int unit, int base_addr, int buf_size, int burst_size);
int  scope1430_smem_set_legacy_mode  (int fd, int unit, int ch_a, int ch_b, int down_sampling);
int  scope1430_smem_set_new_mode     (int fd, int unit, int down_sampling);

int  scope1430_dpram_init            (int fd, int unit, int buf_size);
int  scope1430_dpram_set_mode        (int fd, int unit, int down_sampling);

int  scope1430_map_hwbuf             (int fd, int unit);
void scope1430_unmap_hwbuf           (int fd, int unit);

int  scope1430_map                   (int fd, int unit, struct tsc_ioctl_map_win *map, void **buf, int offset, int size);
void scope1430_unmap                 (int fd, struct tsc_ioctl_map_win *map, void *u_addr);

int  scope1430_intr_status           (int fd, int *status);
int  scope1430_acq_status            (int fd, int unit, int * status);

int  scope1430_trigger_set_adc_mode  (int fd, int unit, int channel, int level, int pol);
int  scope1430_trigger_set_gpio_mode (int fd, int unit, int gpio, int pol);
int  scope1430_trigger_disable       (int fd, int unit);
int  scope1430_trigger_set_pos       (int fd, int unit, int pos);
int  scope1430_pretrigger_get_pos    (int fd, int unit);
int  scope1430_manual_trigger        (int fd, int unit);

int  scope1430_acq_arm               (int fd, int unit);
int  scope1430_acq_set_mode          (int fd, int unit, int mode);
int  scope1430_acq_stop              (int fd, int unit);
int  scope1430_acq_clear             (int fd, int unit);

int  scope1430_hwbuf_get_size        (int fd, int unit);
int  scope1430_sbuf_get_size         (int fd, int unit);

int  scope1430_acq_read              (int fd, int unit, int chan, uint16_t ** sbuf_ptr, int *sbuf_size, int mode);

struct tsc_ioctl_map_win *scope1430_hwbuf_get_map_win(int unit);

#endif /* _H_SCOPE1430LIB */
