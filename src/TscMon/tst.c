/*=========================< begin file & file header >=======================
 *  References
 *  
 *    filename : tst.c
 *    author   : JFG, XP
 *    company  : IOxOS
 *    creation : may 3,2016
 *
 *----------------------------------------------------------------------------
 *  Description
 *
 *    That file contains a set of function called by TscMon to drive test.
 *
 *----------------------------------------------------------------------------
 *
 *  Copyright (C) IOxOS Technologies SA <ioxos@ioxos.ch>
 *
 *    THIS SOFTWARE IS PROVIDED BY THE AUTHOR AND CONTRIBUTORS ``AS IS'' AND
 *    ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 *    IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 *    ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR OR CONTRIBUTORS BE LIABLE
 *    FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 *    DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 *    OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 *    HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 *    LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 *    OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 *    SUCH DAMAGE.
 *
 *    GPL license :
 *    This program is free software; you can redistribute it and/or
 *    modify it under the terms of the GNU General Public License
 *    as published by the Free Software Foundation; either version 2
 *    of the License, or (at your option) any later version.
 *
 *    This program is distributed in the hope that it will be useful,
 *    but WITHOUT ANY WARRANTY; without even the implied warranty of
 *    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *    GNU General Public License for more details.
 *
 *    You should have received a copy of the GNU General Public License
 *    along with this program; if not, write to the Free Software
 *    Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 *
 *=============================< end file header >============================*/

#ifndef lint
static char *rcsid = "$Id: $";
#endif
#define DEBUGno
#include <debug.h>
#include <sys/types.h>
#include <sys/time.h>
#include <termios.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <cli.h>
#include <tscioctl.h>
#include <tsculib.h>
#include <aio.h>
#include <errno.h>
#include <signal.h>
#include <ctype.h>
#include <time.h>
#include "TscMon.h"
#include <tsctst.h>
#ifdef JFG
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
#include <signal.h>
#include <time.h>
#include <sys/mman.h>
#include <cli.h>
#include <tscioctl.h>
#include <tsculib.h>
#include <sys/time.h>
#include <ctype.h>
#include "TscMon.h"
#include <tstlib.h>
#endif
#include "tst.h"
int debug;
#define TST_SHM_BASE 0x100000
#define TST_SHM_SIZE 0x100000
#define TST_KBUF_SIZE 0x100000

static int pid, A_pid, T_pid;
static char *tst_argv[16];
static int fd_p[2][2];
static char cfg_filename[64];
static char *cfg_default = "TscTst.cfg";

struct tsctst *at;

extern char *cmdline;
extern struct aiocb aiocb;

char *
Xtst_rcsid()
{
  return( rcsid);
}

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * Function name : tst_init
 * Prototype     : void
 * Parameters    : void
 * Return        : void
 *----------------------------------------------------------------------------
 * Description   : init test
 *
 *++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

void 
tst_init( void) 
{
  T_pid = 0;
  A_pid = 0;

  return;
}

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * Function name : tst_exit
 * Prototype     : void
 * Parameters    : void
 * Return        : void
 *----------------------------------------------------------------------------
 * Description   : exit test
 *
 *++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

void 
tst_exit( void) 
{
  if( T_pid)
  {
    tsc_tkill( 0);
  }
  free( at);
  return;
}

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * Function name : launch_test
 * Prototype     : int
 * Parameters    : test file and tty
 * Return        : success/error
 *----------------------------------------------------------------------------
 * Description   : launch test
 *
 *++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

int 
launch_test( char *tst_file, char *tty) 
{
  char tmp[64];

  if( pipe( fd_p[0]) < 0)
  {
    printf("Cannot create communication pipe to write\n");
    return( -1);
  }
  //printf("write pipe = %d - %d\n", fd_p[0][0], fd_p[0][1]);
  if( pipe( fd_p[1]) < 0)
  {
    printf("Cannot create communication pipe to read\n");
    close(fd_p[0][0]);
    close(fd_p[0][1]);
    return( TST_ERR);
  }
  tst_argv[0] =  (char *)malloc( 64);
  strcpy(tst_argv[0], tst_file);
  tst_argv[1] =  "TscTst.cfg";
  //tst_argv[1] =  cfg_filename;
  tst_argv[2] =  (char *)malloc( 64);
  sprintf(tst_argv[2], "%d", fd_p[0][0]);
  tst_argv[3] =  (char *)malloc( 64);
  sprintf(tst_argv[3], "%d", fd_p[1][1]);
  //tst_argv[4] =  "/dev/pts/0";
  tst_argv[4] =  (char *)malloc( 64);
  strcpy(tst_argv[4], tty);
  tst_argv[5] =  0;
#ifdef JFG
  tst_argv[0] =  "/usr/bin/xterm";
  tst_argv[1] =  "-e";
  tst_argv[2] =  (char *)malloc( 64);
  tst_argv[3] =  0;
  sprintf( tst_argv[2], "%s %s %d %d", tst_file, cfg_filename, fd_p[0][0], fd_p[1][1]);
#endif
  pid = fork();
  if( pid < 0)
  {
    printf("Cannot fork\n");
    return( TST_ERR);
  }
  else
  {
    if( pid)
    {
      int n, i;
      /* parent process -> we stay in TscTst */
      A_pid = pid;
      close(fd_p[0][0]);
      close(fd_p[1][1]);
      n = read( fd_p[1][0], tmp, 63);  
      if( n <= 0)
      {
        printf("-> Error: Exiting: Connexion lost\n");
	return( -1);
      }
      else
      {
	tmp[n] = 0;
	i = strlen(tst_file);
	if( !strncmp( tst_file, tmp, i))
	{
	  printf("-> Done:%s\n", tmp);
	  sscanf( &tmp[i], "%d", &T_pid);
	}
      }
    }
    else
    {
      /* child process -> we execute TscTst */
      close( fd_p[0][1]);
      close( fd_p[1][0]);
      printf("TscTst-> Launching: %s", tst_argv[0]);
#ifdef JFG
      printf("TscTst-> Launching: %s", tst_argv[2]);
#endif
      fflush( stdout);
      if( execv( tst_argv[0], tst_argv) == -1)
      {
	printf("->ERROR: cannot execute %s\n", tst_argv[0]);
      }
      exit( -1);
    }
  }
  return( TST_OK);
}

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * Function name : tsc_tinit
 * Prototype     : int
 * Parameters    : command line parameter structure
 * Return        : status
 *----------------------------------------------------------------------------
 * Description   : init test environment
 *
 *++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

int tsc_tinit( struct cli_cmd_para *c)
{
  int retval;

  if(T_pid)
  {
    printf("Test program already running.\n");
    return( TST_ERR);
  }
  strcpy( cfg_filename, cfg_default);
  /* if(  cfg_filename) { */
  
  FILE *cfg_file;

  cfg_file = fopen( cfg_filename, "w");
  if(!cfg_file)
    {
      printf("Cannot create configuration file %s\n", cfg_filename);
      return( TST_ERR);
    }
  
  fclose( cfg_file);
    /*}*/

  switch( c->cnt)
  {
    case 0:
    {
      retval = launch_test("./TscTst",  "/dev/pts/0");
      break;
    }
    case 1:
    {
      retval = launch_test (c->para[0],  "/dev/pts/0");
      break;
    }
    default:
    {
      retval = launch_test(c->para[0],  c->para[1]);
    }
  }

  return( retval);
}

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * Function name : tsc_tkill
 * Prototype     : int
 * Parameters    : command line parameter structure
 * Return        : success/error
 *----------------------------------------------------------------------------
 * Description   : kill test
 *
 *++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

int tsc_tkill( struct cli_cmd_para *c)
{
  if( T_pid)
  {
    write( fd_p[0][1], "tstop", 6);
    kill( T_pid, SIGUSR1);
    sleep(1);
    write( fd_p[0][1], "exit", 6);
    kill( T_pid, SIGUSR1);
    close( fd_p[0][1]);
    close( fd_p[1][0]);
    kill( T_pid, SIGKILL);
    T_pid = 0;
#ifdef JFG
    tst_cpu_unmap_shm(&at->cpu_map_shm);
    tst_cpu_unmap_kbuf(&at->cpu_map_kbuf);
#endif
    return( 0);
  }
  else
  {
    printf("tkill -> no test program launched...\n");
    return( -1);
  }
  return( TST_OK);
}

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * Function name : tsc_tcmd
 * Prototype     : int
 * Parameters    : command line parameter structure
 * Return        : success/error
 *----------------------------------------------------------------------------
 * Description   : test command environment
 *
 *++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

int 
tsc_tcmd( struct cli_cmd_para *c)
{
  if( T_pid)
  {
    write( fd_p[0][1], cmdline, strlen( cmdline));
    kill( T_pid, SIGUSR1);
    return( 0);
  }
  else
  {
    printf("Test program not loaded... (see tinit command)\n");
    return( -1);
  }
  return( TST_OK);
}

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * Function name : tsc_tlist
 * Prototype     : int
 * Parameters    : command line parameter structure
 * Return        : status
 *----------------------------------------------------------------------------
 * Description   : list tests
 *
 *++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

int 
tsc_tlist( struct cli_cmd_para *c)
{
  return( tsc_tcmd( c));
}

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * Function name : tsc_tset
 * Prototype     : int
 * Parameters    : command line parameter structure
 * Return        : status
 *----------------------------------------------------------------------------
 * Description   : set specific variable for test
 *
 *++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

int 
tsc_tset( struct cli_cmd_para *c)
{
  return( tsc_tcmd( c));
}

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * Function name : tsc_tstart
 * Prototype     : int
 * Parameters    : command line parameter structure
 * Return        : status
 *----------------------------------------------------------------------------
 * Description   : start test
 *
 *++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

int tsc_tstart( struct cli_cmd_para *c) {
  return( tsc_tcmd( c));
}

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * Function name : tsc_tstop
 * Prototype     : int
 * Parameters    : command line parameter structure
 * Return        : status
 *----------------------------------------------------------------------------
 * Description   : stop test
 *
 *++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

int 
tsc_tstop( struct cli_cmd_para *c)
{
  return( tsc_tcmd( c));
}

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * Function name : tsc_tstatus
 * Prototype     : int
 * Parameters    : command line parameter structure
 * Return        : success/error
 *----------------------------------------------------------------------------
 * Description   : get status of test
 *
 *++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

int 
tsc_tstatus( struct cli_cmd_para *c)
{
  struct tst_ctl tc;
  char tmp[64];
  int n;

  if( !tsc_tcmd( c))
  {
    bzero( tmp, 64);
    n = read( fd_p[1][0], &tc, sizeof( tc));  
    if( n <= 0)
    {
      printf("Connexion lost with test program\n");
      return( -1);
    }
    if( !tc.status)
    {
      printf("TscTST -> IDLE\n");
      return(0);
    }
    if( tc.status & TST_STS_STARTED)
    {
      printf("Tst:%02x -> STARTED\n", tc.test_idx);
      return(0);
    }
    if( tc.status & TST_STS_DONE)
    {
      printf("Tst:%02x -> DONE:", tc.test_idx);
      if( tc.status & TST_STS_STOPPED)
      {
	printf("STOPPED\n");
      }
      else if( tc.status & TST_STS_ERR)
      {
	printf("ERROR\n");
      }
      else
      {
	printf("OK\n");
      }
    }

    return( TST_OK);
  }
  return( TST_ERR);
}

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * Function name : tsc_twait
 * Prototype     : int
 * Parameters    : pointer to command parameter list
 * Return        : tst_ok  if command executed
 *                 tst_err if error
 *----------------------------------------------------------------------------
 * Description   : wait for a test to complete
 *
 *++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

int 
tsc_twait( struct cli_cmd_para *c)
{
  struct tst_ctl tc;
  char tmp[64];
  int n;
  int retval, ret;

  printf("Waiting for test to complete -> ");
  fflush( stdout);
  retval = -1;
  if( aio_error( &aiocb) != EINPROGRESS)
  {
    ret = aio_read( &aiocb);
    if( ret < 0)
    {
      perror("aio_read");
      goto tsc_twait_exit;
    }
  }
  while( !tsc_tcmd( c))
  {
    bzero( tmp, 64);
    n = read( fd_p[1][0], &tc, sizeof( tc));  
    if( n <= 0)
    {
      printf("Connexion lost with test program\n");
      break;
    }
    if( !tc.status)
    {
      printf("TscTST -> IDLE\n");
      retval = 0;
      break;;
    }
    if( tc.status & TST_STS_DONE)
    {
      printf("Tst: %02x -> DONE:", tc.test_idx);
      if( tc.status & TST_STS_STOPPED)
      {
	printf("STOPPED\n");
      }
      else if( tc.status & TST_STS_ERR)
      {
	printf("ERROR\n");
      }
      else
      {
	printf("OK\n");
      }
      retval = 0;
      break;
    }
    if( aio_error( &aiocb) != EINPROGRESS)
    {
      aio_return( &aiocb);
      printf(" Exiting\n");
      goto tsc_twait_exit;
    }
    sleep(1);
  }
  if( aio_error( &aiocb) == EINPROGRESS)
  {
    ret = aio_cancel( aiocb.aio_fildes, &aiocb);
  }
tsc_twait_exit:
  return( retval);
}
