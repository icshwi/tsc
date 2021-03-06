/*=========================< begin file & file header >=======================
 *  References
 *
 *    filename : TscTst.c
 *    author   : JFG, XP
 *    company  : IOxOS
 *    creation : june 30,2009
 *
 *----------------------------------------------------------------------------
 *  Description
 *
 *	  Main test program.
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

#include <debug.h>

#include <sys/types.h>
#include <sys/time.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <cli.h>
#include <aio.h>
#include <errno.h>
#include <ctype.h>
#include <time.h>
#include <signal.h>

#include <tsctst.h>
#include <tstlib.h>
#include "tstlist.h"

#define DEBUGno

void tst_signal( int);

struct cli_cmd_para cmd_para;
char cmdline[0x101];
int fd_in, fd_out;
int cmd_pending = 0;
struct tst_ctl tst_ctl;
char log_filename[0x101];
int debug = 0;
struct tsc_reg_remap *reg_remap;
char TscTst_version[] = "1.00";

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * Function name : tst_signal
 * Prototype     : void
 * Parameters    : signal number
 * Return        : void
 *----------------------------------------------------------------------------
 * Description   : send test signal to another window
 *
 *++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

void tst_signal( int signum){
	int n;

	n = read( fd_in, cmdline, 0x100);
	if(n <= 0){
		printf("TscTst->Exiting:Connexion lost with TscTst");
		strcpy( cmdline, "exit");
	}
	else{
		cmdline[n] = 0;
		cmd_pending = 1;
	}
}

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * Function name : tst_init
 * Prototype     : int
 * Parameters    : void
 * Return        : 0
 *----------------------------------------------------------------------------
 * Description   : init test environment
 *
 *++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

int tst_init(){
	struct tst_ctl *tc;
	int fd = 0;
	tc = &tst_ctl;

	fd = tsc_init();
	if(fd < 0){
		printf("Cannot find interface\n");
		return( -1);
	}
	tc->fd = fd;

	return( 0);
}

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * Function name : tst_status
 * Prototype     : int
 * Parameters    : commande line parameter structure
 * Return        : 0
 *----------------------------------------------------------------------------
 * Description   : get status of test
 *
 *++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

int tst_status( struct cli_cmd_para *c){
	write( fd_out, &tst_ctl, sizeof( tst_ctl));
	return(0);
}

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * Function name : tst_set
 * Prototype     : int
 * Parameters    : command line parameter structure
 * Return        : 0
 *----------------------------------------------------------------------------
 * Description   : set specific variable to environment
 *
 *++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

int tst_set( struct cli_cmd_para *c){
	int i, cnt;
	struct tst_ctl *tc;

	tc = &tst_ctl;

	cnt = c->cnt;
	i = 0;
	while( cnt--){
		int tmp;
		printf("%s\n", c->para[i]);
		if( sscanf( c->para[i], "loop=%d", &tmp) == 1){
			tc->loop_mode = tmp;
		}
		if( !strncmp( c->para[i], "log=off", 7) == 1){
			if( tc->log_mode != TST_LOG_OFF){
				TST_LOG( tc, (logline, "TscTst->LoggingOff:%s\n", tc->log_filename));
				fclose( tc->log_file);
			}
			tc->log_mode = TST_LOG_OFF;
		}
		if( !strncmp( c->para[i], "log=new", 7) == 1){
			if( tc->log_mode != TST_LOG_OFF){
				fclose( tc->log_file);
			}
			tc->log_file = fopen( tc->log_filename, "w");
			if( !tc->log_file){
				tc->log_mode = TST_LOG_OFF;
			}
			else{
				TST_LOG( tc, (logline, "TscTst->LoggingNew:%s\n", tc->log_filename));
			}
			tc->log_mode = TST_LOG_NEW;
		}
		if( !strncmp( c->para[i], "log=add", 7) == 1){
			if( tc->log_mode == TST_LOG_OFF){
				tc->log_file = fopen( tc->log_filename, "a");
				if( !tc->log_file){
					tc->log_mode = TST_LOG_OFF;
				}
				else{
					TST_LOG( tc, (logline, "TscTst->LoggingAdd:%s\n", tc->log_filename));
				}
			}
			tc->log_mode = TST_LOG_ADD;
		}
		if( sscanf( c->para[i], "logfile=%s", log_filename) == 1){
			strcpy( log_filename, &c->para[i][8]);
			printf("log_filename=%s\n", log_filename);
		}
		if( !strncmp( c->para[i], "err=cont", 6) == 1){
			tc->err_mode = TST_ERR_CONT;
		}
		if( !strncmp( c->para[i], "err=halt", 6) == 1){
			tc->err_mode = TST_ERR_HALT;
		}
		if( !strncmp( c->para[i], "exec=fast", 8) == 1){
			tc->exec_mode = TST_EXEC_FAST;
		}
		if( !strncmp( c->para[i], "exec=val", 8) == 1){
			tc->exec_mode = TST_EXEC_VAL;
		}
		i++;
	}
	return(0);
}

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * Function name : tst_get_range
 * Prototype     : int
 * Parameters    : parameter, first test, last test
 * Return        : first test range to execute
 *----------------------------------------------------------------------------
 * Description   : get test range
 *
 *++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

static int tst_get_range( char *para, int *first, int *last){
	char *p;

	*first = strtoul( para, &p, 16);
	*last = *first;
	p = strpbrk( para,".");
	if(p){
		para = p + strspn(p,".");
		*last =  strtoul( para, &p, 16);
	}
	return( *first);
}

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * Function name : tst_start
 * Prototype     : int
 * Parameters    : commande line parameter structure
 * Return        : 0
 *----------------------------------------------------------------------------
 * Description   : start test
 *
 *++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

int tst_start( struct cli_cmd_para *c){
	struct tst_list *t;
	int first, last;
	int i, cnt, loop;
	int iex;

	cnt = c->cnt;
	i = 0;
	if( cnt > 0){
		tst_get_range( c->para[0], &first, &last);
		i++;
		cnt--;
	}
	else{
		first = 1;
		last = 0xff;
	}
	tst_ctl.para_cnt = cnt;
	tst_ctl.para_p = &c->para[i];

	printf("TscTst->Starting test %x..%x\n", first, last);
	iex = 0;
	loop = tst_ctl.loop_mode;
	do{
		t = &tst_list[0];
		while( t->idx){
			if( ( t->idx >= first) && ( t->idx <= last)){
				tst_ctl.test_idx = t->idx;
				tst_ctl.status = TST_STS_STARTED;
				tst_ctl.status = t->func( &tst_ctl);
				t->status = tst_ctl.status;
				if( ( tst_ctl.status & TST_STS_ERR) && ( tst_ctl.err_mode & TST_ERR_HALT)){
					iex = 1;
					break;
				}
			}
			if( cmd_pending){
				if( !strncmp( cmdline, "tstop", 5)){
					printf("TscTst->Stopping test %x\n", t->idx);
					iex = 1;
					break;
				}
			}
			t++;
		}
	} while( --loop && !iex);
	return(0);
}

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * Function name : tst_tlist
 * Prototype     : int
 * Parameters    : command line parameter
 * Return        : 0
 *----------------------------------------------------------------------------
 * Description   : list tests
 *
 *++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

int tst_tlist( struct cli_cmd_para *c){
	struct tst_list *t;
	int first, last;
	int i, cnt;

	cnt = c->cnt;
	i = 0;
	if( cnt > 0){
		tst_get_range( c->para[0], &first, &last);
		i++;
		cnt--;
	}
	else{
		first = 1;
    	last = 0xff;
	}

	i = 0;
	t = &tst_list[0];
	while( t->idx){
		if( ( t->idx >= first) && ( t->idx <= last)){
			printf( "Tst:%02x ", t->idx);
			if( t->status){
				if( t->status & TST_STS_STOPPED){
					printf( "[STP]");
				}
				else if( t->status & TST_STS_ERR){
					printf( "[NOK]");
				}
				else{
					printf( "[ OK]");
				}
			}
			else{
				printf( "[NEX]");
			}
			printf( " -> %s\n", t->msg[0]);
		}
		t++;
	}
	return(0);
}

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * Function name : tst_exit
 * Prototype     : void
 * Parameters    : void
 * Return        : void
 *----------------------------------------------------------------------------
 * Description   : exit test environment
 *
 *++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

void tst_exit(){
	tsc_exit();
}

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * Function name : tst_get_cmd_pending
 * Prototype     : int
 * Parameters    : void
 * Return        : command pending
 *----------------------------------------------------------------------------
 * Description   : get command pending
 *
 *++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

int tst_get_cmd_pending(){
	return( cmd_pending);
}

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * Function name : tst_get_cmdline
 * Prototype     : char
 * Parameters    : void
 * Return        : character get
 *----------------------------------------------------------------------------
 * Description   : get command line
 *
 *++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

char * tst_get_cmdline(){
	return( cmdline);
}

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * Function name : tst_check_cmd_tstop
 * Prototype     : int
 * Parameters    : void
 * Return        : error/success
 *----------------------------------------------------------------------------
 * Description   : stop test execution
 *
 *++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

int tst_check_cmd_tstop(){
	if( cmd_pending){
		if( !strncmp( cmdline, "tstop", 5)){
			return( 1);
		}
		if( !strncmp( cmdline, "tstatus", 7) || !strncmp( cmdline, "twait", 5)){
			tst_status( 0);
			cmd_pending = 0;
			return( 0);
		}
	}
	return( 0);
}

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * Function name : tst_get_config
 * Prototype     : void
 * Parameters    : test structure
 * Return        : void
 *----------------------------------------------------------------------------
 * Description   : acquire configuration
 *
 *++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

void tst_get_config(struct tscTst *at){
    FILE *fp;
    char buf[100];
    char prm_name[20];
    int val = 0;
    if((fp=fopen("tscTst.conf", "r")) == NULL) {
    	fprintf(stderr, "Failed to open config file");
        exit(EXIT_FAILURE);
    }

    while(! feof(fp)) {
        fgets(buf, 100, fp);
        if (buf[0] == '#') {
            continue;
        }
        if (strstr(buf, "SHM_BASE_0 ")) {
        	if(sscanf(buf, "%s %x\n", prm_name, &val)!= 2){
        		printf("Error ! \n");
        		break;
        	}
        	at->shm_base_0 = val;
        }
        if (strstr(buf, "SHM_SIZE_0 ")) {
        	if(sscanf(buf, "%s %x\n", prm_name, &val)!= 2){
        		printf("Error ! \n");
        		break;
        	}
        	at->shm_size_0 = val;
        }
        if (strstr(buf, "SHM_OFFSET_0 ")) {
        	if(sscanf(buf, "%s %x\n", prm_name, &val)!= 2){
        		printf("Error ! \n");
        		break;
        	}
        	at->shm_offset_0 = val;
        }
        if (strstr(buf, "SRAM_OFFSET_0 ")) {
        	if(sscanf(buf, "%s %x\n", prm_name, &val)!= 2){
        		printf("Error ! \n");
        		break;
        	}
        	at->sram_offset_0 = val;
        }
        if (strstr(buf, "USR_BASE_0 ")) {
        	if(sscanf(buf, "%s %x\n", prm_name, &val)!= 2){
        		printf("Error ! \n");
                break;
            }
            at->usr_base_0 = val;
        }
        if (strstr(buf, "USR_SIZE_0 ")) {
        	if(sscanf(buf, "%s %x\n", prm_name, &val)!= 2){
        		printf("Error ! \n");
                break;
            }
            at->usr_size_0 = val;
        }
        if (strstr(buf, "USR_OFFSET_0 ")) {
        	if(sscanf(buf, "%s %x\n", prm_name, &val)!= 2){
        		printf("Error ! \n");
            break;
            }
            at->usr_offset_0 = val;
        }
    }
}

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * Function name : main
 * Prototype     : int
 * Parameters    : argc, argv
 * Return        : error/success
 *----------------------------------------------------------------------------
 * Description   : main program for test
 *
 *++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

int main( int argc, char *argv[]){
	char data[64];
	int pid;
	time_t tm;
	struct tst_ctl *tc;
	struct tscTst *at;
#ifdef DEBUG
	debug = 1;
#endif

	if( argv[4]){
		freopen( argv[4], "r+", stdout);
	}

	tm = time(0);
	signal(SIGUSR1, tst_signal);

	at = (struct tscTst *)malloc( sizeof( struct tscTst));
	tc = &tst_ctl;
	tc->at = at;
	printf("TscTst->Entering:%s", ctime(&tm));
	printf("TscTst->Initialization");

	fd_in = atoi( argv[2]);
  	fd_out = atoi( argv[3]);

  	tst_init();
  	pid = getpid();
  	sprintf( data, "%s %d", argv[0], pid);
  	write( fd_out, data, strlen( data));
  	printf("->Done\n");
  	printf("TscTst->Version %s - %s %s\n", TscTst_version, __DATE__, __TIME__);
    tst_ctl.loop_mode = 1;
    tst_ctl.log_mode = TST_LOG_OFF;
    tst_ctl.err_mode = TST_ERR_CONT;
    strcpy(log_filename, "TscTst.log");
    tc->log_filename = log_filename;
    tst_ctl.exec_mode = TST_EXEC_VAL;

    // Parse configuration file
    tst_get_config(at);

#ifdef DEBUG
    printf("Extraction from configuration file \n");
    printf("---------------------------------- \n");

    printf("shm_base_0       : %x \n", tc->at->shm_base_0);
    printf("shm_size_0       : %x \n", tc->at->shm_size_0);
    printf("shm_offset_0     : %x \n", tc->at->shm_offset_0);
    printf("shm_base_1       : %x \n", tc->at->shm_base_1);
    printf("shm_size_1       : %x \n", tc->at->shm_size_1);
    printf("shm_offset_1     : %x \n", tc->at->shm_offset_1);
#endif

    while(1){
    	if(!cmd_pending){
    		pause();
    	}
    	cmd_pending = 0;
    	TST_LOG( tc, (logline, "TscTst->Command:%s\n", cmdline));
    	cli_cmd_parse( cmdline, &cmd_para);
    	if(!strncmp( cmd_para.cmd, "exit", 4)){
    		TST_LOG( tc, (logline, "TscTst->Exiting:TscTst command"));
    		break;
    	}

    	if(!strncmp( cmd_para.cmd, "tstart", 6)){
    		tst_start( &cmd_para);
    	}
    	if(!strncmp( cmd_para.cmd, "tset", 6)){
    		tst_set( &cmd_para);
    	}
    	if(!strncmp( cmd_para.cmd, "tlist", 6)){
    		tst_tlist( &cmd_para);
    	}
    	if(!strncmp( cmd_para.cmd, "tstatus", 7)){
    		tst_status( &cmd_para);
    	}
    	if(!strncmp( cmd_para.cmd, "twait", 5)){
    		tst_status( &cmd_para);
    	}
    }
    tst_exit();
    TST_LOG( tc, (logline, "->Done\n"));
    close( fd_in);
    close( fd_out);
    if(tc->log_file){
    	fclose( tc->log_file);
    }
    exit(0);
}
