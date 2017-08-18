/*=========================< begin file & file header >=======================
 *  References
 *  
 *    filename : TscMon.c
 *    author   : JFG
 *    company  : IOxOS
 *    creation : Sept 14,2015
 *    version  : 0.0.1
 *
 *----------------------------------------------------------------------------
 *  Description
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
static char rcsid[] = "$Id: TscMon.c,v 1.3 2016/01/26 13:00:40 ioxos Exp $";
#endif

#include <termios.h>
#include <stdlib.h>
#include <stdio.h>
#include <pty.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <aio.h>
#include <errno.h>
#include <time.h>

#include <tscioctl.h>
#include <cli.h>
#include <tsculib.h>
#include "cmdlist.h"

char TscMon_version[] = "1.00";

int tsc_cmd_exec( struct cli_cmd_list *, struct cli_cmd_para *);

struct termios termios_old;
struct termios termios_new;
char cli_prompt[16];
struct cli_cmd_para cmd_para;
struct cli_cmd_history cmd_history;
struct aiocb aiocb;
char aio_buf[256];
char *cmdline;
int script_exit = 0;
int ifc1211_sign;
int ifc1211_date;

static char *month[16] ={ NULL,"Jan","Feb","Mar","Apr","May","Jun","Jul","Aug","Sep","Oct","Nov","Dec",NULL,NULL};

char *
TscMon_rcsid()
{
  return( rcsid);
}

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * Function name : main
 * Prototype     : int
 * Parameters    : argument count
 *                 pointer tor argument list
 * Return        : 0
 *----------------------------------------------------------------------------
 * Description   : main entry for TscMon application
 *
 *++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
int
main( int argc,
      char *argv[])
{
  struct cli_cmd_history *h;
  struct winsize winsize;
  int iex, retval;
  int cmd_cnt;
  int mm,dd,yy,hh,mn,ss;

  if( tsc_init() < 0)
  {
    printf("Cannot find interface\n");
    exit( -1);
  }

  retval = tsc_csr_read( 0x18, &ifc1211_sign);
  if( retval < 0)
  {
    printf("  ERROR -> cannot access ILOC_SIGN register !!\n");
  }
  tsc_csr_read( 0x28, &ifc1211_date);
  ss = ifc1211_date & 0x3f;
  mn = (ifc1211_date>>6) & 0x3f;
  hh = (ifc1211_date>>12) & 0x1f;
  yy = (ifc1211_date>>17) & 0x3f;
  mm = (ifc1211_date>>23) & 0xf;
  dd = (ifc1211_date>>27) & 0x1f;

  /* configure the terminal in canonical mode with echo */
  ioctl( 0, TIOCGWINSZ, &winsize);
  tcgetattr( 0, &termios_old);
  memcpy( &termios_new, &termios_old, sizeof( struct termios)); 
  termios_new.c_lflag &=  (uint)(~(ECHOCTL | ECHO | ICANON));
  tcsetattr( 0, TCSANOW, &termios_new);

  /* initialize command list history */
  h = cli_history_init( &cmd_history);


  rdwr_init();
  tst_init();
  dma_init();

  if( argc > 1) 
  {
    struct cli_cmd_para script_para;

    /* check for script execution */
    if( argv[1][0] == '@') 
    {
      cli_cmd_parse( &argv[1][1], &script_para);
      iex = tsc_script( &argv[1][1], &script_para);
      if( iex == 2)
      {
        goto TscMon_exit;
      }
    }
    /* if not, interpret argument as command and execute it */
    else 
    {
      cli_cmd_parse(  argv[1], &cmd_para);
      tsc_cmd_exec( &cmd_list[0], &cmd_para);
      goto TscMon_exit;
    }
  }
  printf("\n");
  printf("     +-----------------------------------------+\n");
  if(tsc_get_device_id() == 0x1000){
	  printf("     |  TscMon - %s %04x diagnostic tool        |\n", "IO", tsc_get_device_id());
  }
  else if(tsc_get_device_id() == 0x1001){
	  printf("     |  TscMon - %s %04x diagnostic tool  |\n", "CENTRAL", tsc_get_device_id());
  }
  printf("     |  IOxOS Technologies Copyright 2015-2016 |\n");
  printf("     |  Version %s - %s %s    |\n", TscMon_version, __DATE__, __TIME__);
  printf("     |  FPGA Built %s %2d 20%02d %02d:%02d:%02d     |\n", month[mm], dd, yy, hh, mn, ss);
  printf("     |  FPGA Sign  %08x                    |\n", ifc1211_sign);
  printf("     |  Driver %s  Version %s   |\n", tsc_get_drv_name(), tsc_get_drv_version());
  printf("     +-----------------------------------------+\n");
  printf("\n");

  cmd_cnt = 0;
  while(1)
  {
    sprintf(cli_prompt, "%d:TscMon>", ++cmd_cnt);
    cmdline = cli_get_cmd( h, cli_prompt);

    if( cmdline[0] == 'q')
    {
      break;
    }
  /* Check for script execution */
    if( cmdline[0] == '@')
    {
      struct cli_cmd_para script_para;

      cli_cmd_parse(&cmdline[1], &script_para);
      iex = tsc_script( &cmdline[1], &script_para);
      if( iex == 2)
      {
	break;
      }
      continue;
    }   
    if( cmdline[0] == '&')
    {
      printf("Entering loop mode [enter any character to stop loop]...\n");
      if( aio_error( &aiocb) != EINPROGRESS)
      {
        retval = aio_read( &aiocb);
        if( retval < 0)
        {
          perror("aio_read");
          goto TscMon_end_loop;
        }
      }
      while( 1)
      {
        if( cmdline[1] == '@')
        {
          struct cli_cmd_para script_para;

          cli_cmd_parse(  &cmdline[2], &script_para);
          iex = tsc_script( &cmdline[2], &script_para);
          if(iex == 2)
          {
  	    break;
          }
        }
        else
	{
	  cli_cmd_parse( &cmdline[1], &cmd_para);
          if( tsc_cmd_exec( &cmd_list[0], &cmd_para) < 0)
	  {
            if( aio_error( &aiocb) == EINPROGRESS)
            {
              retval = aio_cancel( aiocb.aio_fildes, &aiocb);
	    }
	    goto TscMon_end_loop;
  	  }
	}
        if(aio_error( &aiocb) != EINPROGRESS)
        {
          aio_return( &aiocb);
	  goto TscMon_end_loop;
	}
      }
TscMon_end_loop:
      continue;
    }
    cli_cmd_parse( cmdline, &cmd_para);
    if( cmdline[0] == '?')
    {
      tsc_func_help( &cmd_para);
      continue;
    }
    if( cmdline[0] == '!')
    {
      char *new_cmd;
      int idx;

      cmd_para.cmd = NULL;
      if( sscanf( &cmdline[1], "%d", &idx) == 1)
      {
        new_cmd = cli_history_find_idx( &cmd_history, idx);
	if( new_cmd)
	{
	  printf("%s\n", new_cmd);
	  cli_cmd_parse( new_cmd, &cmd_para);
	}
      }
      else 
      {
        new_cmd = cli_history_find_str( &cmd_history, &cmdline[1]);
	if( new_cmd)
	{
	  printf("%s\n", new_cmd);
	  cli_cmd_parse( new_cmd, &cmd_para);
	}
      }
    }
    if( cmdline[0] == '$')
    {
      char *new_cmd;

      new_cmd = alias_find( &cmdline[1]);
      cmd_para.cmd = NULL;
      if( new_cmd)
      {
	printf("%s\n", new_cmd);
	cli_cmd_parse( new_cmd, &cmd_para);
      }
    }
    if( cmd_para.cmd)
    {
      tsc_cmd_exec( &cmd_list[0], &cmd_para);
    }
  }

TscMon_exit:
  kbuf_free( NULL);
  tst_exit();
  rdwr_exit();

  /* restore previous terminal setting */
  tcsetattr( 0, TCSANOW, &termios_old);
  tsc_exit();
  exit(0);
}

int
tsc_cmd_exec( struct cli_cmd_list *l,
	       struct cli_cmd_para *c)
{
  long i;

  i = 0;
  if(strlen(c->cmd)) {
    while(1) {
      if( !l->cmd) // Command doesn't exist, break 
      {
        break;
      }
      // Check that the user enter the correct function name !
      if(!strncmp(l->cmd, c->cmd, strlen(l->cmd)))
      {
        c->idx = i;
        return( l->func( c));
      }
      i++; l++;
    }
    printf("%s -> Invalid command name ", c->cmd);
    printf("(\'help\' or \'?\' displays a list of valid commands)\n");
  }
  return(-1);
}

int 
tsc_print_usage( struct cli_cmd_para *c)
{
  long i;

  i = 0;
  while( cmd_list[c->idx].msg[i])
  {
    printf("%s\n", cmd_list[c->idx].msg[i]);
    i++;
  }
  return(0);
}

int 
tsc_func_help( struct cli_cmd_para *c)
{
  char *cmd;
  long i, j;

  i = 0;
  if( c->cnt > 0)
  {
    while(1)
    {
      cmd = cmd_list[i].cmd;
      if( cmd)
      {
        if( !strcmp( c->para[0], cmd))
        {
	  long j;

	  j = 0;
	  while( cmd_list[i].msg[j])
	  {
	    printf("%s\n", cmd_list[i].msg[j]);
	    j++;
	  }
	  return(0);
        }
	i++;
      }
      else
      {
	printf("%s -> Invalid command name ", c->para[0]);
	printf("(\'help\' or \'?\' displays a list of valid commands)\n");
	return(-1);
      }
    }
  }

  while(1)
  {
    cmd = cmd_list[i++].cmd;
    if( cmd)
    {
      printf("%s", cmd);
      if( i&3)
      {
	j = 10 - (int)strlen( cmd);
	while(j--)
	{
	  putchar(' ');
	}
      }
      else
      {
	putchar('\n');
      }
    }
    else
    {
      putchar('\n');
      break;
    }
  }
  return(0);
}


int 
tsc_func_history( struct cli_cmd_para *c)
{
  cli_history_print( &cmd_history);

  return(0);
}

int 
tsc_wait(struct cli_cmd_para *c)
{
  int retval, ret;
  int tmo;
  char ch;

  printf("Continue [y/n] -> ");
  fflush( stdout);
  retval = 0;
  ch = 'y';
  if( aio_error( &aiocb) != EINPROGRESS) {
    ret = aio_read( &aiocb);
    if( ret < 0)
    {
      perror("aio_read");
      goto tsc_wait_exit;
    }
  }
  tmo = -1;
  if( c->cnt > 0)
  {
    sscanf( c->para[0], "%d", &tmo);
    tmo = tmo * 10;
  }
  while( tmo--)
  {
    if( aio_error( &aiocb) != EINPROGRESS)
    {
      retval = 0;
      ch = *(char *)aiocb.aio_buf;
      if( ch == 'n')
      {
	retval = -1;
      }
      aio_return( &aiocb);
      goto tsc_wait_exit;
    }
    usleep(100000);
  }

  if( aio_error( &aiocb) == EINPROGRESS)
  {
    ret = aio_cancel( aiocb.aio_fildes, &aiocb);
  }

tsc_wait_exit:

  printf("%c\n", ch);
  return( retval);
}
