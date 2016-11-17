/*=========================< begin file & file header >=======================
 *  References
 *  
 *    filename : buf.h
 *    author   : JFG
 *    company  : IOxOS
 *    creation : Sept 30,2015
 *    version  : 0.0.1
 *
 *----------------------------------------------------------------------------
 *  Description
 *
 *    This file contain the declarations of all exported functions defined in
 *    buf.c
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

#ifndef _H_BUF
#define _H_BUF

#define TSC_NUM_KBUF 8
struct tsc_kbuf_ctl
{
  struct tsc_ioctl_kbuf_req *kbuf_p;
  struct tsc_ioctl_map_win *map_p;
};

int alloc_kbuf( int idx, int size);
int kbuf_show( struct cli_cmd_para *);
int kbuf_alloc( struct cli_cmd_para *);
int kbuf_free( struct cli_cmd_para *);
int tsc_kbuf( struct cli_cmd_para *);

#endif /* _H_BUF */
