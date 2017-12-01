/*=========================< begin file & file header >=======================
 *  References
 *  
 *    filename : sflash.h
 *    author   : JFG, XP
 *    company  : IOxOS
 *    creation : Sept 30,2015
 *
 *----------------------------------------------------------------------------
 *  Description
 *
 *    This file contains the declarations of all exported functions defined in
 *    sflash.c
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

#ifndef _H_SFLASH
#define _H_SFLASH

#define SFLASH_BLK_SIZE         0x10000

int tsc_sflash( struct cli_cmd_para *);

#endif /* _H_SFLASH */
