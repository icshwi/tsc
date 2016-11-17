/*=========================< begin file & file header >=======================
 *  References
 *  
 *    filename : tst.h
 *    author   : JFG
 *    company  : IOxOS
 *    creation : may 3,2016
 *    version  : 0.0.1
 *
 *----------------------------------------------------------------------------
 *  Description
 *
 *    This file contain the declarations of all exported functions define in
 *    pevulib.c
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

#ifndef _H_TST
#define _H_TST

#define TST_OK     0
#define TST_ERR    -1

int tsc_tinit( struct cli_cmd_para *);
int tsc_tkill( struct cli_cmd_para *);
int tsc_tlist( struct cli_cmd_para *);
int tsc_tset( struct cli_cmd_para *);
int tsc_tstart( struct cli_cmd_para *);
int tsc_tstatus( struct cli_cmd_para *);
int tsc_tstop( struct cli_cmd_para *);
int tsc_twait( struct cli_cmd_para *);
void tst_init( void);
void tst_exit( void);

#endif /* _H_TST */
