/*=========================< begin file & file header >=======================
 *  References
 *  
 *    filename : gscope3110lib.h
 *    author   : JFG, XP
 *    company  : IOxOS
 *    creation : october 18,2018
 *
 *----------------------------------------------------------------------------
 *  Description
 *
 *    That library contains a set of function to access the TOSCA II XUSER
 *    FASTSCOPE ADC3112 data acquisition logic.
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

#define GSCOPE3110_CSR_BASE   0x00001000
#define GSCOPE3110_CSR_ADDR( csr)      (GSCOPE3110_CSR_BASE | (csr<<2))
#define GSCOPE3110_CSR_SIGN1        0x60
#define GSCOPE3110_CSR_MAP1         0x61
#define GSCOPE3110_CSR_RWT1         0x63
#define GSCOPE3110_CSR_SIGN2        0x64
#define GSCOPE3110_CSR_MAP2         0x65
#define GSCOPE3110_CSR_RWT2         0x67

#define GSCOPE3110_CSR_FE1_CSR1     0x68
#define GSCOPE3110_CSR_FE1_CSR2     0x69 
#define GSCOPE3110_CSR_FE1_CSR3     0x6a 
#define GSCOPE3110_CSR_FE1_TRIG     0x6b 
#define GSCOPE3110_CSR_FE2_CSR1     0x6c
#define GSCOPE3110_CSR_FE2_CSR2     0x6d
#define GSCOPE3110_CSR_FE2_CSR3     0x6e 
#define GSCOPE3110_CSR_FE2_TRIG     0x6f
 
#define GSCOPE3110_CSR_SWR1_RGB_CFG       0x70
#define GSCOPE3110_CSR_SWR1_RGB_BAS       0x71
#define GSCOPE3110_CSR_SWR1_ACQ_MGT       0x72
#define GSCOPE3110_CSR_SWR1_TRIG_MKT      0x73
#define GSCOPE3110_CSR_SWR1_GEN_CTL       0x74
#define GSCOPE3110_CSR_SWR1_FMC_STA       0x75

#define GSCOPE3110_CSR_SWR2_RGB_CFG       0x78
#define GSCOPE3110_CSR_SWR2_RGB_BAS       0x79
#define GSCOPE3110_CSR_SWR2_ACQ_MGT       0x7a
#define GSCOPE3110_CSR_SWR2_TRIG_MKT      0x7b
#define GSCOPE3110_CSR_SWR2_GEN_CTL       0x7c
#define GSCOPE3110_CSR_SWR2_FMC_STA       0x7d

#define GSCOPE3110_MAP_USR_OFF  0x100000
#define GSCOPE3110_MAP_USR_SIZE 0x100000

#define GSCOPE3110_FEM_CHAN_0           0x01
#define GSCOPE3110_FEM_CHAN_1           0x02
#define GSCOPE3110_FEM_CHAN_2           0x04
#define GSCOPE3110_FEM_CHAN_3           0x08
#define GSCOPE3110_FEM_CHAN_4           0x10
#define GSCOPE3110_FEM_CHAN_5           0x20
#define GSCOPE3110_FEM_CHAN_6           0x40
#define GSCOPE3110_FEM_CHAN_7           0x80
#define GSCOPE3110_FEM_CHAN_ALL         0xff
#define GSCOPE3110_FEM_ENA        0x80000000
#define GSCOPE3110_FEM_FCT_1      0x10000000

#define GSCOPE3110_DPRAM_SIZE_32K      0x000
#define GSCOPE3110_DPRAM_SIZE_64K      0x400
#define GSCOPE3110_DPRAM_SIZE_128K     0x800
#define GSCOPE3110_DPRAM_SIZE_256K     0xc00

#define GSCOPE3110_SMEM_INIT_BUSY_MASK 0x40000000

#define GSCOPE3110_RGBUF_SIZE_MASK     0x1fff0000
#define GSCOPE3110_RGBUF_BASE_MASK     0xffff0000
#define GSCOPE3110_RGBUF_MODE_SINGLE   0
#define GSCOPE3110_RGBUF_MODE_DUAL     1
#define GSCOPE3110_RGBUF_NUM_MAX       8
#define GSCOPE3110_RGBUF_SIZE_MAX      0x2000000
#define GSCOPE3110_RGBUF_ADDR_MAX	   0x20000000
#define GSCOPE3110_RGBUF_ADDR_START	   0x10000000

#define GSCOPE3110_TRIG_POS_0_0          0
#define GSCOPE3110_TRIG_POS_1_8          1
#define GSCOPE3110_TRIG_POS_1_4          2
#define GSCOPE3110_TRIG_POS_3_8          3
#define GSCOPE3110_TRIG_POS_1_2          4
#define GSCOPE3110_TRIG_POS_5_8          5
#define GSCOPE3110_TRIG_POS_3_4          6
#define GSCOPE3110_TRIG_POS_7_8          7
#define GSCOPE3110_TRIG_POS_MASK         7

#define GSCOPE3110_TRIG_MAN         0x40000000
#define GSCOPE3110_TRIG_GPIO_MASK    0xA28C0000
#define GSCOPE3110_TRIG_ADC         0x0
#define GSCOPE3110_TRIG_GPIO        0x1
#define GSCOPE3110_TRIG_LVL         0x0
#define GSCOPE3110_TRIG_EDGE        0x1
#define GSCOPE3110_TRIG_UP          0x0
#define GSCOPE3110_TRIG_DOWN        0x1
#define GSCOPE3110_TRIG_UNSIGN      0
#define GSCOPE3110_TRIG_SIGN        1
#define GSCOPE3110_TRIG_HYST        1

#define GSCOPE3110_TRIG_HYST_MASK   0x8000000
#define GSCOPE3110_TRIG_ENG_MASK    0xa0000000
#define GSCOPE3110_TRIG_HYST_OVF	0xFFFF
#define GSCOPE3110_TRIG_OFFSET_OVF  0xFFFF

#define GSCOPE3110_TRIG_CHAN_START       16
#define GSCOPE313_TRIG_ENG_TYPE_START    23
#define GSCOPE313_TRIG_DIR_START         24
#define GSCOPE313_TRIG_LVL_START         25
#define GSCOPE3110_TRIG_HYST_START   	 26
#define GSCOPE313_TRIG_SIGN_START        28

#define GSCOPE3110_TRIG_CODE_MAIN_MASK       0x30
#define GSCOPE3110_TRIG_CODE_LVL_MASK        0x4
#define GSCOPE3110_TRIG_CODE_DIR_MASK        0x2
#define GSCOPE3110_TRIG_CODE_SIGN_MASK        0x1

#define GSCOPE3110_TRIG_CODE_MAN             0x0
#define GSCOPE3110_TRIG_CODE_GPIO            0x20
#define GSCOPE3110_TRIG_CODE_ADC             0x30

#define GSCOPE3110_TRIG_CODE_LVL             0x0
#define GSCOPE3110_TRIG_CODE_EDGE            0x4
#define GSCOPE3110_TRIG_CODE_UP              0x0
#define GSCOPE3110_TRIG_CODE_DOWN            0x2

#define GSCOPE3110_TRIG_CODE_UNSIGN          0x0
#define GSCOPE3110_TRIG_CODE_SIGN            0x1

#define GSCOPE3110_ARM_CONT             0
#define GSCOPE3110_ARM_SINGLE           1
#define GSCOPE3110_ARM_NORMAL           0
#define GSCOPE3110_ARM_AUTO             1
#define GSCOPE3110_ARM_MASTER           0
#define GSCOPE3110_ARM_SLAVE            1

#define GSCOPE3110_ARM_MODE_START       0
#define GSCOPE3110_ARM_TRIG_START       1
#define GSCOPE3110_ARM_SYNC_START       2
#define GSCOPE3110_ARM_TRIG_POS_START   5
#define GSCOPE3110_ARM_TRIG_POS_MAX     8
#define GSCOPE3110_ARM_TRIG_POS_SIZE    3

#define GSCOPE3110_ARM_MASK                  0x10000000
#define GSCOPE3110_ARM_CODE_MODE_MASK         0x1
#define GSCOPE3110_ARM_CODE_TRIG_MASK         0x2
#define GSCOPE3110_ARM_CODE_SYNC_MASK         0x4

#define GSCOPE3110_ARM_CODE_CONT             0
#define GSCOPE3110_ARM_CODE_SINGLE           0x1
#define GSCOPE3110_ARM_CODE_NORMAL           0
#define GSCOPE3110_ARM_CODE_AUTO             0x2
#define GSCOPE3110_ARM_CODE_MASTER           0
#define GSCOPE3110_ARM_CODE_SLAVE            0x4

#define GSCOPE3110_SAVE_CHAN_MASK            0xFFFFFFF8

#define GSCOPE3110_MIXED_FMC                 3





int gscope3110_identify(int fd);
struct tsc_ioctl_map_win *gscope3110_map_usr(int fd, int fmc, char **buf, int offset, int size);
void gscope3110_unmap_usr(int fd, struct tsc_ioctl_map_win *map, char *u_addr);
int gscope3110_acq_smem_init(int fd, int chan_set, int rgbuf_base, int rgbuf_size, int mode);
int gscope3110_acq_dpram_init(int fd, int fmc, int chan_set, int size);
int gscope3110_acq_status(int fd, int fmc);
int gscope3110_acq_arm(int fd, int fmc, int trig_mode, int trig_pos, int buf_size);
int gscope3110_acq_abort(int fd, int fmc);
int gscope3110_acq_rearm( int fmc);
int gscope3110_acq_stop( int fmc);
void gscope3110_acq_trig(int fd, int fmc, int chan, int mode, int offset, int hyst);
void gscope3110_acq_clear(int fd, int fmc, int chan); 
void gscope3110_acq_load(int fd, int fmc, char *buf, int size);
int gscope3110_acq_read(int fd, int fmc, int chan, unsigned short *buf, int size);
char* gscope3110_save(int fd, int chan,  char* str, int mode, int new_size);
char* gscope3110_filename_generator(char* filename, int chan);
int gscope3110_trig_time(int fmc, int *msec_p, int *nsec_p);
int gscope3110_intr_status( void);
int gscope3110_acq_dpram_move(int fd, int fmc,  int chan,  uint64_t pci_addr,  uint64_t dpram_off, int size);
void gscope_acq_dis(int fd, int fmc);
