/*=========================< begin file & file header >=======================
 *  References
 *  
 *    filename : alias.h
 *    author   : JFG, XP
 *    company  : IOxOS
 *    creation : Sept 30,2015
 *
 *----------------------------------------------------------------------------
 *  Description
 *
 *    This file contains the declarations of all exported functions defined in
 *    alias.c
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

#ifndef _H_ALIAS
#define _H_ALIAS

#define ALIAS_SIZE 80

struct tsc_alias
{
  char alias[ALIAS_SIZE][32];
  char cmd[ALIAS_SIZE][256];
} alias_ctl;

int tsc_alias( struct cli_cmd_para *);
void alias_init( void);
char *alias_find( char *alias);

#endif /* _H_ALIAS */
