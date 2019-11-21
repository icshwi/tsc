/*=========================< begin file & file header >=======================
 *  References
 *  
 *    filename : script.c
 *    author   : JFG, XP
 *    company  : IOxOS
 *    creation : Sept 17,2015
 *
 *----------------------------------------------------------------------------
 *  Description
 *
 *    That file contains a set of function called by TscMon to handle
 *    script files.
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
static char *rcsid = "$Id: script.c,v 1.2 2015/12/03 15:15:21 ioxos Exp $";
#endif

#define DEBUGno
#include <debug.h>
#include <sys/types.h>
#include <sys/time.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <cli.h>
#include <tscioctl.h>
#include <tsculib.h>

char line[256];
extern struct cli_cmd_list cmd_list[];
extern char cli_prompt[];
extern struct cli_cmd_para cmd_para;
extern int script_exit;

int tsc_cmd_exec( struct cli_cmd_list *, struct cli_cmd_para *);

char *
script_rcsid()
{
  return( rcsid);
}

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * Function name : tsc_script
 * Prototype     : int
 * Parameters    : filename of script, command line parameter structure
 * Return        : error/success
 *----------------------------------------------------------------------------
 * Description   : manage script execution
 *
 *++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

int tsc_script( char *filename,
	     struct cli_cmd_para *s)

{
  FILE *file;
  int retval;

  filename = strtok( filename, " \t");
  retval = 0;
  file = fopen( filename, "r");
  
  if( !file)
  {
    printf("Cannot open script file %s\n", filename);
    return( -1);
  }
  printf("Start execution of script file %s\n", s->cmd);
  
  while( fgets( line, 256, file))
  {
    strtok( line, "\n\r#");
    if( line[0] == '#') continue;
    if( line[0] == '\n') continue;
    if( line[0] == '\r') continue;
    if( line[0] == '$')
    {
      uint para;

      if( !strncmp( "exit", &line[1], 4))
      {
	retval = 2;
	break;
      }
      if( !strncmp( "sleep", &line[1], 5))
      {
	sscanf(line, "$sleep %d", &para);
	sleep( para);
      }
      if( !strncmp( "usleep", &line[1], 5))
      {
	sscanf(line, "$usleep %d", &para);
	usleep( para);
      }
      if( !strncmp( "echo", &line[1], 4))
      {
	printf("%s\n", &line[6]);
      }
      continue;
    }
    
    if( line[0] == 'q') break;
    
    if( line[0] == '@') {
      struct cli_cmd_para script_para;

      printf("%s%s\n", cli_prompt, line);
      cli_cmd_parse(  &line[1], &script_para);
      if( (retval = tsc_script( &line[1], &script_para)))
      {
	break;
      }
      continue;
    }
    
    if( line[0]) {
      int i, j;
      char arg[3];

      printf("%s%s\n", cli_prompt, line);
      cli_cmd_parse( line, &cmd_para);
      arg[0] = '$';
      arg[2] = 0;
      for( i = 0; i < cmd_para.cnt; i++)
      {
        for( j = 0; j < s->cnt; j++)
        {
	  arg[1] = (char)((int)'0'+ j);
          if( !strncmp( arg, cmd_para.para[i],2))
          {
	    cmd_para.para[i] = s->para[j];
	  }
        }
      }
      script_exit = 0;
      tsc_cmd_exec( &cmd_list[0], &cmd_para);
      if( script_exit)
      {
	retval = 1;
	break;
      }
    }
  }
  fclose( file);
  return( retval);
}
