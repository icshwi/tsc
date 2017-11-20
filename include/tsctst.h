/*=========================< begin file & file header >=======================
 *  References
 *
 *    filename : tsctst.h
 *    author   : XP
 *    company  : IOxOS
 *    creation : june 30,2016
 *    version  : 0.0.1
 *
 *----------------------------------------------------------------------------
 *  Description
 *
 *    This file contains the definition of all structures and define used
 *    in the TSC environment test
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

struct tsc_para{
	int crate;
	int serial;
};

// Structure with all configuration parameters
struct tscTst{
	/* Use TSI148 board */
	int use_tsi148;
	/* Local board 0 */
	int shm_base_0;
	int shm_size_0;
	int shm_offset_0;
	/* Remote board 1 */

	int shm_base_1;
	int shm_size_1;
	int shm_offset_1;
};

struct tst_ctl{
	int    status;
	int    test_idx;
	int    loop_mode;
	int    loop_cnt;
	int    log_mode;
	int    err_mode;
	char   *log_filename;
	FILE   *log_file;
	struct tscTst *at;
	int    para_cnt;
	char   **para_p;
	int    exec_mode;
	int    fd;
};

#define TST_DMA_WAIT_NO     0x00000000
#define TST_DMA_WAIT_POLL   0x01000000
#define TST_DMA_WAIT_INTR   0x02000000
#define TST_DMA_WAIT_MASK   0x0f000000
#define TST_DMA_CHAN_0      0x00000000
#define TST_DMA_CHAN_1      0x10000000
#define TST_DMA_CHAN_MASK   0x10000000

#define TST_LOG_OFF    		0x00000000
#define TST_LOG_NEW    		0x01000000
#define TST_LOG_ADD    		0x02000000

#define TST_STS_IDLE      	0x00000000
#define TST_STS_STARTED   	0x00000001
#define TST_STS_DONE      	0x00000002
#define TST_STS_STOPPED   	0x00000004
#define TST_STS_ERR       	0x00000008

#define TST_ERR_CONT        0x00000000
#define TST_ERR_HALT        0x00000001

#define TST_EXEC_FAST       0x00000001

char logline[0x101];
#define TST_LOG( x, y) \
sprintf y;\
printf( "%s", logline);fflush( stdout);\
if( x->log_mode){fprintf( x->log_file, "%s", logline);fflush( x->log_file);}
