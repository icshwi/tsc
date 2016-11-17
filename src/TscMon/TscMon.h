/*=========================< begin file & file header >=======================
 *  References
 *  
 *    filename : TscMon.h
 *    author   : JFG
 *    company  : IOxOS
 *    creation : Sept 14,2015
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
 *=============================< end file header >============================*/

#ifndef _H_TSCMON
#define _H_TSCMON

#define TSC_OK    0
#define TSC_ERR  -1
#define TSC_QUIT -2

#include "conf.h"   
#include "rdwr.h"   
#include "map.h"
#include "dma.h"   
#include "timer.h"   
#include "sflash.h"   
#include "script.h"   
#include "buf.h"   
#include "tst.h"
#include "semaphore.h"
#include "fifo.h"
#include "lmk.h"

int tsc_print_usage(struct cli_cmd_para *c);

#endif /* _H_TSCMON */
