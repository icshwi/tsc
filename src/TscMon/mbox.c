/*=========================< begin file & file header >=======================
 *  References
 *  
 *    filename : mbox.c
 *    author   : RH
 *    company  : IOxOS
 *    creation : Sept 28,2017
 *    version  : 0.0.1
 *
 *----------------------------------------------------------------------------
 *  Description
 *
 *     That file contains a 
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

#ifndef lint
static char *rcsid = "$Id: mbox.c,v 1.4 2015/12/02 08:26:51 ioxos Exp $";
#endif

#define DEBUGno
#include <debug.h>
#include <sys/types.h>
#include <sys/time.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <cli.h>
#include <tscioctl.h>
#include <tsculib.h>
#include <aio.h>
#include <errno.h>
#include <ctype.h>
#include <time.h>
#include "mbox.h"

char *
mbox_rcsid()
{
  return( rcsid);
}


int 
mbox_read( struct cli_cmd_para *c)
{
  int retval;
  int offset = 0;
  int data = 0;

  if (c->cnt != 2)
  {
    tsc_print_usage( c);
    return( CLI_ERR);
  }
  else if( sscanf( c->para[1], "%x", &offset) != 1)
  {
    printf("Bad offset argument [%s] -> usage:\n", c->para[0]);
    tsc_print_usage( c);
    return( CLI_ERR);
  }

  offset >>= 2;
  retval = tsc_pon_write( 0xd0, &offset);
  if( retval < 0)
  {
    puts("cannot write to mailbox address register");
    return( CLI_ERR);
  }
  retval = tsc_pon_read( 0xd4, &data);
  if( retval < 0)
  {
    puts("cannot read from mailbox data register");
    return( CLI_ERR);
  }

  printf("0x%04x : 0x%08x\n", offset, data);
  return CLI_OK;
}

int 
mbox_write( struct cli_cmd_para *c)
{
  int retval;
  int offset = 0;
  int data = 0;

  if (c->cnt != 3)
  {
    tsc_print_usage( c);
    return( CLI_ERR);
  }
  else if( sscanf( c->para[1], "%x", &offset) != 1)
  {
    printf("Bad offset argument [%s] -> usage:\n", c->para[0]);
    tsc_print_usage( c);
    return( CLI_ERR);
  }
  else if( sscanf( c->para[2], "%x", &data) != 1)
  {
    printf("Bad data argument [%s] -> usage:\n", c->para[0]);
    tsc_print_usage( c);
    return( CLI_ERR);
  }

  offset >>= 2;
  retval = tsc_pon_write( 0xd0, &offset);
  if( retval < 0)
  {
    puts("cannot write to mailbox address register");
    return( CLI_ERR);
  }
  retval = tsc_pon_write( 0xd4, &data);
  if( retval < 0)
  {
    puts("cannot write to mailbox data register");
    return( CLI_ERR);
  }

  return CLI_OK;
}

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * Function name : tsc_mbox
 * Prototype     : int
 * Parameters    : pointer to command parameter list
 * Return        : RDWR_OK  if command executed
 *                 RDWR_ERR if error
 *----------------------------------------------------------------------------
 * Description   : perform read/write acces to IFC1211 CSR registers
 *
 *++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
int 
tsc_mbox( struct cli_cmd_para *c)
{
  int cnt, i;

  cnt = c->cnt;
  i = 0;
  if( cnt--)
  {
    if( !strcmp( "read", c->para[i]))
    {
      return( mbox_read( c));
    }
    if( !strcmp( "write", c->para[i]))
    {
      return( mbox_write( c));
    }
  }
  return(-1);

}
