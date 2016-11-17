/*=========================< begin file & file header >=======================
 *  References
 *  
 *    filename : rdwr.h
 *    author   : JFG
 *    company  : IOxOS
 *    creation : Sept 30,2015
 *    version  : 0.0.1
 *
 *----------------------------------------------------------------------------
 *  Description
 *
 *    This file contain the declarations of all exported functions defined in
 *    rdwr.c
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

#ifndef _H_RDWR
#define _H_RDWR

struct rdwr_cycle_para
{
  ulong addr;
  ulong data;
  int len;
  union
  {
    uint mode;
    struct rdwr_mode m;
  };
  ulong para; 
  uint operation;
  int loop;
  struct tsc_ioctl_kbuf_req *kb_p;
};

#define RDWR_OK     0
#define RDWR_ERR    -1

int  rdwr_init( void);
int  rdwr_exit( void);
int tsc_rdwr_pr( struct cli_cmd_para *);
int tsc_rdwr_dr( struct cli_cmd_para *);
int tsc_rdwr_dx( struct cli_cmd_para *);
int tsc_rdwr_fx( struct cli_cmd_para *);
int tsc_rdwr_tx( struct cli_cmd_para *);
int tsc_rdwr_px( struct cli_cmd_para *);
int tsc_rdwr_cr( struct cli_cmd_para *);
int tsc_rdwr_cx( struct cli_cmd_para *);
int tsc_rdwr_cmp( struct cli_cmd_para *c);

#endif /* _H_RDWR */
