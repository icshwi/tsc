/*=========================< begin file & file header >=======================
 *  References
 *  
 *    filename : fscope3112lib.h
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

#define FSCOPE3112_CSR_BASE   0x00001000
#define FSCOPE3112_CSR_SIGN         0x60
#define FSCOPE3112_CSR_CTL          0x63
#define FSCOPE3112_CSR_INTR         0x68
#define FSCOPE3112_CSR_VME_Px       0x69
#define FSCOPE3112_CSR_ACQ1_CTL     0x70
#define FSCOPE3112_CSR_TRIG1_STS    0x71
#define FSCOPE3112_CSR_TRIG1_ADDR   0x72
#define FSCOPE3112_CSR_TRIG1_TIM    0x73
#define FSCOPE3112_CSR_ACQ2_CTL     0x74
#define FSCOPE3112_CSR_TRIG2_STS    0x75
#define FSCOPE3112_CSR_TRIG2_ADDR   0x76
#define FSCOPE3112_CSR_TRIG2_TIM    0x77
#define FSCOPE3112_CSR_ADDR( csr)      (FSCOPE3112_CSR_BASE | (csr<<2))

#define FSCOPE3112_MAP_USR_OFF  0x100000
#define FSCOPE3112_MAP_USR_SIZE 0x100000

#define FSCOPE3112_ACQ_SIZE      0x10000
#define FSCOPE3112_BUF_SIZE_1k         5
#define FSCOPE3112_BUF_SIZE_2k         4
#define FSCOPE3112_BUF_SIZE_4k         3
#define FSCOPE3112_BUF_SIZE_8k         2
#define FSCOPE3112_BUF_SIZE_16k        2
#define FSCOPE3112_BUF_SIZE_32k        0

#define FSCOPE3112_TRIG_POS_0          0

#define FSCOPE3112_TRIG_MODE_MASK     0x80030000
#define FSCOPE3112_TRIG_MODE_AUTO     0x00000000
#define FSCOPE3112_TRIG_MODE_TTRIG    0x88010000
#define FSCOPE3112_TRIG_MODE_GPIO     0x88020000
#define FSCOPE3112_TRIG_MODE_ADC      0x88000001
#define FSCOPE3112_TRIG_MODE_ADC0(x)     (0x88000001 | (x<<4))
#define FSCOPE3112_TRIG_MODE_ADC1(x)     (0x98000001 | (x<<4))
#define FSCOPE3112_TRIG_MODE_ADC2(x)     (0xa8000001 | (x<<4))
#define FSCOPE3112_TRIG_MODE_ADC3(x)     (0xb8000001 | (x<<4))
#define FSCOPE3112_TRIG_POS_1_8          1
#define FSCOPE3112_TRIG_POS_1_4          2
#define FSCOPE3112_TRIG_POS_3_8          3
#define FSCOPE3112_TRIG_POS_1_2          4
#define FSCOPE3112_TRIG_POS_5_8          5
#define FSCOPE3112_TRIG_POS_3_4          6
#define FSCOPE3112_TRIG_POS_7_8          7

int fscope3112_identify(int fd);
struct tsc_ioctl_map_win *fscope3112_map_usr(int fd, int fmc, char **buf, int offset, int size);
void fscope3112_unmap_usr(int fd, struct tsc_ioctl_map_win *map, char *u_addr);
void fscope3112_acq_init(int fd);
void fscope3112_acq_arm(int fd, int fmc, int trig_mode, int trig_pos, int buf_size);
int fscope3112_acq_rearm(int fd, int fmc);
int fscope3112_acq_stop(int fd, int fmc);
void fscope3112_acq_clear(int fd, int fmc, int chan); 
void fscope3112_acq_load(int fd, int fmc, int chan, char *buf, int size);
int fscope3112_acq_read(int fd, int fmc, int chan, unsigned short *buf, int size);
int fscope3112_acq_save(int fd, int fmc,  int chan,  char *filename);
int fscope3112_trig_time(int fd, int fmc, int *msec_p, int *nsec_p);
int fscope3112_intr_status(int fd);

