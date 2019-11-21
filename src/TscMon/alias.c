/*=========================< begin file & file header >=======================
 *  References
 *  
 *    filename : alias.c
 *    author   : JFG, XP
 *    company  : IOxOS
 *    creation : Sept 15,2015
 *
 *----------------------------------------------------------------------------
 *  Description
 *
 *     That file contains a set of function called by TscMon to handle the
 *     alias command substitution.
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
#include "TscMon.h"

char line[512];

char *
alias_rcsid()
{
  return( rcsid);
}

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * Function name : alias_init
 * Prototype     : void
 * Parameters    : void
 * Return        : void
 *----------------------------------------------------------------------------
 * Description   : initialize alias
 *
 *++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

void
alias_init( void)
{
  int i;

  for( i = 0; i < ALIAS_SIZE; i++)
  {
    alias_ctl.alias[i][0] = 0;
    alias_ctl.cmd[i][0] = 0;
  }
}

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * Function name : alias_show
 * Prototype     : void
 * Parameters    : void
 * Return        : void
 *----------------------------------------------------------------------------
 * Description   : show alias
 *
 *++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

void
alias_show( void)
{
  int i;

  printf("alias list:\n");
  for( i = 0; i < ALIAS_SIZE; i++)
  {
    if( alias_ctl.alias[i][0] != 0)
    {
      printf("%s \"%s\"\n", &alias_ctl.alias[i][0], &alias_ctl.cmd[i][0]);
    }
  }
  return;
}

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * Function name : alias_save
 * Prototype     : int
 * Parameters    : filename
 * Return        : success/error
 *----------------------------------------------------------------------------
 * Description   : save an alias
 *
 *++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

int
alias_save( char *filename)
{
  int i;
  FILE *file;

  printf("saving alias list in %s\n", filename);
  file = fopen( filename, "w");
  if( !file)
  {
    printf("cannot create file %s\n", filename);
    return( TSC_ERR);
  }
  fprintf(file, "# TscMon alias list\n");
  for( i = 0; i < ALIAS_SIZE; i++)
  {
    if( alias_ctl.alias[i][0] != 0)
    {
      fprintf(file, "%s \"%s\"\n", &alias_ctl.alias[i][0], &alias_ctl.cmd[i][0]);
    }
  }
  fprintf(file, "#");
  fclose( file);
  return( TSC_OK);
}

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * Function name : alias_load
 * Prototype     : int
 * Parameters    : filename
 * Return        : success/error
 *----------------------------------------------------------------------------
 * Description   : load alias
 *
 *++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

int
alias_load( char *filename)
{
  int i;
  FILE *file;
  char *alias, *cmd;
  int next;

  printf("loading alias list from file \"%s\"\n", filename);
  file = fopen( filename, "r");
  if( !file)
  {
    printf("file \"%s\" does not exist\n", filename);
    return( TSC_ERR);
  }
  i = 0;
  while( fgets( line, 512, file))
  {
    strtok( line, "\n\r#");
    printf("%s\n", line);
    alias = strtok( line, " ");
    cmd = strtok( NULL,"\"");
    if( (alias[0]) != '#' && cmd)
    {
      next = 0;
      //printf("registering alias %s \"%s\"\n", alias, cmd);
      for( i = 0; i < ALIAS_SIZE; i++)
      {
        if( alias_ctl.alias[i][0] != 0)
        {
          if( !strncmp( alias,  &alias_ctl.alias[i][0], strlen( alias)))
          {
  	    //printf("updating alias %s\n", alias);
	    strcpy( &alias_ctl.cmd[i][0], cmd);
	    next = 1;
	    break;
          }
	}
      }
      if( !next)
      {
        for( i = 0; i < ALIAS_SIZE; i++)
        {
          if( alias_ctl.alias[i][0] == 0)
          {
  	    //printf("creating alias %s\n", alias);
  	    strcpy( &alias_ctl.alias[i][0], alias);
	    strcpy( &alias_ctl.cmd[i][0], cmd);
	    next = 1;
	    break;
	  }
        }
      }
      if( !next)
      {
        printf("No more space in alias list\n");
        break;
      }
    }
  }
  fclose( file);
  return( TSC_OK);
}

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * Function name : alias_find
 * Prototype     : char *
 * Parameters    : alias
 * Return        : null
 *----------------------------------------------------------------------------
 * Description   : find an alias
 *
 *++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

char *
alias_find( char *alias)
{
  int i;

  for( i = 0; i < ALIAS_SIZE; i++)
  {
    if( alias_ctl.alias[i][0] != 0)
    {
      if( !strncmp( alias,  &alias_ctl.alias[i][0], strlen( alias)))
      {
	//printf("%s \"%s\"\n", &alias_ctl.alias[i][0], &alias_ctl.cmd[i][0]);
	return( &alias_ctl.cmd[i][0]);
      }
    }
  }
  return(NULL);
}

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * Function name : alias_set
 * Prototype     : int
 * Parameters    : command line parameter structure
 * Return        : success/error
 *----------------------------------------------------------------------------
 * Description   : set an alias
 *
 *++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

int
alias_set( struct cli_cmd_para *c)
{
  int i;
  if( c->cnt < 2)
  {
    printf("%s need more arguments...\n", c->cmd);
    printf("usage: alias set <alias> \"cmd\"\n");
    return( TSC_ERR);
  }
  if( strlen( c->para[1]) > 30)
  {
    printf("alias name too long [must be < 30]...\n");
    return( TSC_ERR);
  }
  if( c->cnt == 2)
  {
    //printf("%s %s %s -> clearing entry\n", c->cmd, c->para[0], c->para[1]);
    for( i = 0; i < ALIAS_SIZE; i++)
    {
      if( !strcmp( &alias_ctl.alias[i][0],  c->para[1]))
      {
	alias_ctl.alias[i][0] = 0;
	alias_ctl.cmd[i][0] = 0;
	return( TSC_OK);
      }
    }
  }
  else
  {
    //printf("%s %s %s \"%s\"\n", c->cmd, c->para[0], c->para[1], c->para[2]);
    for( i = 0; i < ALIAS_SIZE; i++)
    {
      if( alias_ctl.alias[i][0] != 0)
      {
        if( !strncmp( c->para[1],  &alias_ctl.alias[i][0], strlen( c->para[1])))
        {
	  //printf("registering alias %s\n", c->para[1]);
	  strcpy( &alias_ctl.cmd[i][0], c->para[2]);
	  return( TSC_OK);
	}
      }
    }
    for( i = 0; i < ALIAS_SIZE; i++)
    {
      if( alias_ctl.alias[i][0] == 0)
      {
	//printf("registering alias %s\n", c->para[1]);
	strcpy( &alias_ctl.alias[i][0], c->para[1]);
	strcpy( &alias_ctl.cmd[i][0], c->para[2]);
	return( TSC_OK);
      }
    }
    printf("no space to register alias %s\n", c->para[1]);
  }

  return( TSC_ERR);
}

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * Function name : tsc_alias
 * Prototype     : int
 * Parameters    : pointer to command parameter list
 * Return        : ok  if command executed
 *                 error if error
 *----------------------------------------------------------------------------
 * Description   : perform alias operation
 *
 *++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

int 
tsc_alias( struct cli_cmd_para *c) 
{
  if( c->cnt == 0)
  {
    alias_show();
    return( TSC_OK);
  }
  if( !strcmp( "show", c->para[0])) 
  {
    alias_show();
    return( TSC_OK);
  }
  if( !strcmp( "clear", c->para[0])) 
  {
    alias_init();
    return( TSC_OK);
  }
  else if( !strcmp( "set", c->para[0])) 
  {
    return( alias_set( c));
  }
  else if( !strcmp( "load", c->para[0])) 
  {
    if( c->cnt < 2)
    {
      return( TSC_ERR);
    }
    return( alias_load( c->para[1]));
  }
  else if( !strcmp( "save", c->para[0])) 
  {
    if( c->cnt < 2)
    {
      return( TSC_ERR);
    }
    return( alias_save( c->para[1]));
  }
  else
  {
    return( TSC_ERR);
  }
}
