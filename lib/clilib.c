/*=========================< begin file & file header >=======================
 *  References
 *  
 *    filename : clilib.c
 *    author   : JFG, XP
 *    company  : IOxOS
 *    creation : june 30,2008
 *
 *----------------------------------------------------------------------------
 *  Description
 *
 *    That library contains a set of function to implement a command line
 *    interpreter running in an Xterm.
 *    It keeps a command history and allows to recall previous command.
 *    Characters can be inserted/deleted anywhere in the current line using
 *    arrows and delete keys.
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
static char rcsid[] = "$Id: clilib.c,v 1.3 2015/12/03 15:13:06 ioxos Exp $";
#endif

#include <debug.h>
#include <stdio.h>
#include <string.h>
#include <cli.h>
#include <sys/types.h>

char cli_error_line[256] = {0,};

char *
cli_rcsid()
{
  return( rcsid);
}

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * Function name : cli_history_init
 * Prototype     : struct cli_cmd_history *
 * Parameters    : struct cli_cmd_history *h
 * Return        : structure with hisotry updated
 *----------------------------------------------------------------------------
 * Description   : set and manage cli command history
 *
 *++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

struct cli_cmd_history *
cli_history_init( struct cli_cmd_history *h)
{
  h->status = 0;
  h->rd_idx = 0;
  h->wr_idx = 0;
  h->cnt = 0;
  h->size = CLI_HISTORY_SIZE;
  h->end_idx = 0;
  h->insert_idx = 0;
  return(h);
}

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * Function name : cli_insert_char
 * Prototype     : long
 * Parameters    : c, cmdline, insert_idx
 * Return        : identifier of insertion
 *----------------------------------------------------------------------------
 * Description   : insert character
 *
 *++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

long
cli_insert_char( char c,
                 char *cmdline,
	         long insert_idx)
{
  long last_idx;
  long i;

  last_idx = strlen( cmdline);
  if( last_idx >= (CLI_COMMAND_SIZE - 2))
  {
    last_idx = (CLI_COMMAND_SIZE - 2);
    cmdline[CLI_COMMAND_SIZE - 1] = 0;
  }
  if( insert_idx > last_idx)
  {
    insert_idx = last_idx;
  }
  cmdline[last_idx+1] = 0;
  for( i = last_idx; i > insert_idx; i--)
  {
    cmdline[i] = cmdline[i-1];
  }
  cmdline[insert_idx] = c;
  return( insert_idx + 1);
}

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * Function name : cli_remove_char
 * Prototype     : long
 * Parameters    : cmdline structure and position of removing character
 * Return        : position of cursor
 *----------------------------------------------------------------------------
 * Description   : remove character from command line
 *
 *++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

long
cli_remove_char( char *cmdline,
		 long insert_idx)
{
  long last_idx;
  long i;

  if( !insert_idx)
  {
    return( 0);
  }
  last_idx = strlen( cmdline);
  if( insert_idx > last_idx)
  {
    insert_idx = last_idx;
  }
  for( i = insert_idx; i < last_idx; i++)
  {
    cmdline[i-1] = cmdline[i];
  }
  cmdline[last_idx - 1] = 0;
  return( insert_idx - 1);
}

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * Function name : cli_erase_line
 * Prototype     : void
 * Parameters    : prompt
 * Return        : void
 *----------------------------------------------------------------------------
 * Description   : erase a complete line on the command line
 *
 *++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

void
cli_erase_line( char *prompt)
{
  printf("%c%c%d%c%c%c%c", 0x1b, '[',  (int)strlen( prompt) + 1,'G',0x1b,'[','K');
}

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * Function name : cli_erase_end
 * Prototype     : void
 * Parameters    : void
 * Return        : void
 *----------------------------------------------------------------------------
 * Description   : end character of line
 *
 *++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

void
cli_erase_end()
{
  putchar(0x1b);
  putchar('[');
  putchar('K');
}

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * Function name : cli_print_line
 * Prototype     : long
 * Parameters    : cmdline, position of insertion
 * Return        : return the position where print line must be done
 *----------------------------------------------------------------------------
 * Description   : print a line
 *
 *++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

long
cli_print_line( char *cmdline,
		long insert_idx)
{
  long last_idx;
  long i;

  last_idx = strlen( cmdline);
  for( i = insert_idx; i < last_idx; i++)
  {
    putchar( cmdline[i]);
  }
  for( i = last_idx; i > insert_idx + 1; i--)
  {
    putchar( 0x8);
  }
  if( last_idx == CLI_COMMAND_SIZE - 1)
  {
    putchar( 0x8);
  }
  return( insert_idx);
}

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * Function name : cli_get_cmd
 * Prototype     : char *
 * Parameters    : cli command hisotry, prompt
 * Return        : cmdline
 *----------------------------------------------------------------------------
 * Description   : get command
 *
 *++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

char *
cli_get_cmd( struct cli_cmd_history *h,
	     char *prompt)
{
  char *cmdline, c;
  long iex;
  long i;

  cmdline = &h->bufline[h->wr_idx][0];
  bzero(cmdline, CLI_COMMAND_SIZE);
  printf("%s",prompt);
  h->end_idx = 0;
  h->insert_idx = 0;
  h->rd_idx = h->wr_idx;
  iex = 1;
  while( iex)
  {

    c = (char)getchar();
    switch( c)
    {
      case 0x7f:
      case 0x8:
      {
	if( h->insert_idx > 0)
	{
	  putchar('\b');
	  putchar(0x1b);
	  putchar('7');
	  cli_erase_line( prompt);
	  h->insert_idx = (short)cli_remove_char( cmdline, h->insert_idx);
	  printf("%s", cmdline);
	  putchar(0x1b);
	  putchar('8');
	  h->end_idx = (short)((int)h->end_idx - 1);
	}
	break;
      }
      case '\r':
      {
        putchar( c);
	break;
      }
      case '\n':
      {
        putchar( c);
        cmdline[h->end_idx] = 0;
	iex = 0;
        break;
      }
      case 0x1b: /* escape sequence */
      {
        char c1,c2;

	c1 = (char)getchar();
	switch( c1)
	{
	  case '[':
	  {
	    c2 = (char)getchar();
	    switch( c2)
	    {
	      case 'A': /* UP */
	      {
		if( h->rd_idx == 0)
		{
		  if( h->cnt >= h->size)
		  {
		    h->rd_idx = h->size - 1;
		  }
		}
                else
		{
	          h->rd_idx = (short)((int)h->rd_idx - 1);
		}
		cli_erase_line( prompt);
		cmdline = &h->bufline[h->rd_idx][0];
		h->end_idx = (short)strlen( cmdline);
		for( i = 0; i < h->end_idx; i++)
		{
		  putchar(cmdline[i]);
		}
		h->insert_idx = h->end_idx;
		break;
	      }
	      case 'B': /* DOWN */
	      {
		if( !(h->rd_idx == h->wr_idx))
		{
		  if( h->rd_idx == (short)((int)h->size - 1))
		  {
		    h->rd_idx = 0;
		  }
                  else
	 	  {
		    h->rd_idx = (short)((int) h->rd_idx + 1);
		  }
		}
		cli_erase_line(  prompt);
		cmdline = &h->bufline[h->rd_idx][0];
		h->end_idx = (short)strlen( cmdline);
		for( i = 0; i < h->end_idx; i++)
		{
		  putchar(cmdline[i]);
		}
		h->insert_idx = h->end_idx;
		break;
	      }
	      case 'C': /* RIGTH */
	      {
		if( h->insert_idx < h->end_idx)
		{
		  putchar( cmdline[h->insert_idx]);
		  h->insert_idx++;
		}
		break;
	      }
	      case 'D': /* LEFT */
	      {
		if( h->insert_idx > 0)
		{
		  h->insert_idx--;
		  putchar('\b');
		}
		break;
	      }
	    }
	    break;
	  }
	}
	break;
      }
      default:
      {
	if( ( c > 0x1f) && ( c < 0x80))
	{
	  h->insert_idx = (short)cli_insert_char( c, cmdline, h->insert_idx);
	  cli_print_line( cmdline, h->insert_idx - 1);
	  h->end_idx = (short)strlen( cmdline);
	}
	break;
      }
    }
  }

  if( strlen( cmdline))
  {
    int last_idx;

    if( cmdline[0] == '!')
    {
      return( cmdline);
    }
    last_idx = h->wr_idx -1;
    if( last_idx < 0)
    {
      last_idx = h->size;
    }
    if( !strcmp( cmdline, &h->bufline[last_idx][0]))
    {
      return( cmdline);
    }
    if( h->rd_idx != h->wr_idx)
    {
      strcpy( &h->bufline[h->wr_idx][0], cmdline);
    }
    h->wr_idx++;
    if( h->wr_idx >= h->size)
    {
      h->wr_idx = 0;
    }
    h->cnt++;
  }

  return( cmdline);

}

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * Function name : cli_cmd_parse
 * Prototype     : struct cli_cmd_para*
 * Parameters    : cmdline, parameter of cmdline
 * Return        :
 *----------------------------------------------------------------------------
 * Description   : parse command line
 *
 *++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

struct cli_cmd_para    
*cli_cmd_parse( char *cmdline, 
		struct cli_cmd_para *c)
{
  char *p;
  long i;
  while( *cmdline == ' ') cmdline++;
  strcpy( c->cmdline, cmdline); 
  c->cmd = c->cmdline;
  p = strpbrk( c->cmd, ". \t");
  if( p)
  {
    if( *p == '.')
    {
      c->ext = p + strspn(p,". \t");
      *p = 0;
      p = strpbrk( c->ext, " \t");
    }
    else
    {
      c->ext = (char *)0;
    }
  }
  i = 0;
  while(p)
  {
    c->para[i] = p + strspn(p," \t");
    if( c->para[i][0] == '\"')
    {
      char *end;
      *p = 0;
      c->para[i] += 1;
      p = strpbrk( c->para[i], "\"");
      if( !p)
      {
	break;
      }
      end = p;
      p = strpbrk( p, " \t");
      *end = 0;
    }
    else
    {
      *p = 0;
      p = strpbrk( c->para[i], " \t");
    }
    i++;
    if( i > 11)
    {
      break;
    }
  }
  c->cnt = i;

  return( c);
}

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * Function name : cli_get_para_str
 * Prototype     : int
 * Parameters    : command line parameter, identifier, string, length
 * Return        : status
 *----------------------------------------------------------------------------
 * Description   : get string parameter
 *
 *++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

int
cli_get_para_str( struct cli_cmd_para *c,
	          int idx,
  	          char *str,
		  int len)
{
  if(idx < c->cnt)
  {
    if( len)
    {
      if( !strncmp( str, c->para[idx], len))
      {
        return( CLI_OK);
      }
    }
    else 
    {
      if( !strcmp( str, c->para[idx]))
      {
        return( CLI_OK);
      }
    }
    sprintf(cli_error_line, "Bad parameters %s\n", c->para[idx]);
  }
  else
  {
    sprintf(cli_error_line, "Not enough parameters..\n");
  }
  return( CLI_ERR);
}

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * Function name : cli_get_para_hex
 * Prototype     : int
 * Parameters    : command line parameter, identifier, value
 * Return        : status
 *----------------------------------------------------------------------------
 * Description   : get hexadecimal parameter
 *
 *++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

int
cli_get_para_hex( struct cli_cmd_para *c,
	          int idx,
  	          int *val_p)
{
  if(idx < c->cnt)
  {
    if( sscanf( c->para[idx], "%x\n", val_p) == 1)
    {
      return( CLI_OK);
    }
    sprintf(cli_error_line, "Bad parameters %s\n", c->para[idx]);
  }
  else
  {
    sprintf(cli_error_line, "Not enough parameters..\n");
  }
  return( CLI_ERR);
}

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * Function name : cli_get_para_dec
 * Prototype     : int
 * Parameters    : command line parameter, identifier, value
 * Return        : status
 *----------------------------------------------------------------------------
 * Description   : get decimal parameter
 *
 *++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

int
cli_get_para_dec( struct cli_cmd_para *c,
	          int idx,
  	          int *val_p)
{
  if(idx < c->cnt)
  {
    if( sscanf( c->para[idx], "%i\n", val_p) == 1)
    {
      return( CLI_OK);
    }
    sprintf(cli_error_line, "Bad parameters %s\n", c->para[idx]);
  }
  else
  {
    sprintf(cli_error_line, "Not enough parameters..\n");
  }
  return( CLI_ERR);
}

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * Function name : cli_error_print
 * Prototype     : void
 * Parameters    : void
 * Return        : void
 *----------------------------------------------------------------------------
 * Description   : print error
 *
 *++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

void
cli_error_print( void)
{
  printf("%s", cli_error_line);
  return;
}

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * Function name : cli_history_print
 * Prototype     : void
 * Parameters    : structure of history command line
 * Return        : void
 *----------------------------------------------------------------------------
 * Description   : print history of commands
 *
 *++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

void
cli_history_print( struct cli_cmd_history *h)
{
  int i;

  if( h->cnt < h->size)
  {
    for( i = 0; i < h->wr_idx-1; i++)
    {
      printf("%4d %s\n", i, &h->bufline[i][0]);
    }
  }
  else
  {
    int n;

    n =  h->cnt - h->size;
    for( i = h->wr_idx; i < h->size; i++)
    {
      printf("%4d %s\n", n++, &h->bufline[i][0]);
    }
    for( i = 0; i < h->wr_idx; i++)
    {
      printf("%4d %s\n", n++, &h->bufline[i][0]);
    }
  }
  return;
}

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * Function name : cli_history_find_idx
 * Prototype     : char *
 * Parameters    : command line history structure, position
 * Return        : position of history element to find
 *----------------------------------------------------------------------------
 * Description   : find a history element (position)
 *
 *++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

char *
cli_history_find_idx( struct cli_cmd_history *h,
		      int idx)
{
  int i;

  if( h->cnt < h->size)
  {
    for( i = 0; i < h->wr_idx; i++)
    {
      //printf("%4d %s\n", i, &h->bufline[i][0]);
      if( i == idx)
      {
	return( &h->bufline[i][0]);
      }
    }
  }
  else
  {
    int n;

    n =  h->cnt - h->size;
    for( i = h->wr_idx; i < h->size; i++)
    {
      //printf("%s\n", n++, &h->bufline[i][0]);
      if( n == idx)
      {
	return( &h->bufline[i][0]);
      }
    }
    for( i = 0; i < h->wr_idx; i++)
    {
      //printf("%4d %s\n", n++, &h->bufline[i][0]);
      if( n == idx)
      {
	return( &h->bufline[i][0]);
      }
    }
  }
  return(NULL);
}

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * Function name : cli_history_find_str
 * Prototype     : char *
 * Parameters    : command line history structure, command
 * Return        : element of history element to find
 *----------------------------------------------------------------------------
 * Description   : find a history element (string)
 *
 *++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

char *
cli_history_find_str( struct cli_cmd_history *h,
		      char *cmd)
{
  int i;

  if( h->cnt < h->size)
  {
    for( i = h->wr_idx; i >= 0; i--)
    {
      //printf("%4d %s\n", i, &h->bufline[i][0]);
      if( !strncmp( cmd,  &h->bufline[i][0], strlen(cmd)))
      {
	return( &h->bufline[i][0]);
      }
    }
  }
  else
  {
    for(i = h->size - 1;  i == h->wr_idx; i--)
    {
      if( !strncmp( cmd,  &h->bufline[i][0], strlen(cmd)))
      {
	return( &h->bufline[i][0]);
      }
    }
    for( i = h->wr_idx; i >= 0; i--)
    {
      if( !strncmp( cmd,  &h->bufline[i][0], strlen(cmd)))
      {
	return( &h->bufline[i][0]);
      }
    }
  }
  return(NULL);
}
