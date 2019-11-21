/*=========================< begin file & file header >=======================
 *  References
 *  
 *    filename : map.c
 *    author   : JFG, XP
 *    company  : IOxOS
 *    creation : Sept 15,2015
 *
 *----------------------------------------------------------------------------
 *  Description
 *
 *    That file contains a set of function called by TscMon to perform mapping.
 *
 *----------------------------------------------------------------------------
 *
 *  Copyright (C) IOxOS Technologies SA <ioxos@ioxos.ch>
 *  Copyright (C) 2019  European Spallation Source ERIC
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
static char *rcsid = "$Id: map.c,v 1.4 2015/12/02 08:26:51 ioxos Exp $";
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
#include "map.h"
#include "TscMon.h"

extern int tsc_fd;

char *
map_rcsid()
{
  return( rcsid);
}

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * Function name : map_get_id
 * Prototype     : int
 * Parameters    : name of map to get, map control structure
 * Return        : map id
 *----------------------------------------------------------------------------
 * Description   : get map id
 *
 *++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

int
map_get_id( char *name,
	    struct tsc_ioctl_map_ctl *map)
{
  map->sg_id =  MAP_ID_INVALID;
  if( !strcmp( "mas_mem", name))
  {
    map->sg_id =  MAP_ID_MAS_PCIE_MEM;
  }
  if( !strcmp( "mas_pmem", name))
  {
    map->sg_id =  MAP_ID_MAS_PCIE_PMEM;
  }
   return( map->sg_id);
}

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * Function name : map_show
 * Prototype     : int
 * Parameters    : name of map to show
 * Return        : success/error
 *----------------------------------------------------------------------------
 * Description   : show mapping information
 *
 *++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

int
map_show( char *name)
{
  struct tsc_ioctl_map_ctl map_ctl;
  struct tsc_map_blk *p;
  int j;
  int max_pg;


  if( map_get_id( name, &map_ctl) == MAP_ID_INVALID)
  {
    printf("wrong map name : %s\n", name);
    return( -1);
  } 
  map_ctl.map_p = (struct tsc_map_blk *)0;
  if( tsc_map_read(tsc_fd, &map_ctl) < 0)
  {
    printf("Cannot access map %s !!\n", name);
    return( -1);
  } 
  map_ctl.map_p = malloc( (uint)map_ctl.pg_num*(sizeof( struct tsc_map_blk)));
  if( tsc_map_read(tsc_fd, &map_ctl) < 0)
  {
    printf("Cannot read map %s !!\n", name);
    return( -1);
  } 

  p = map_ctl.map_p;
  max_pg = map_ctl.pg_num;
  printf("\n");
  printf("+=========================================================+\n");
  printf("+ Map Name : %s\n", name);
  if( map_ctl.pg_size > 0xfffff)
  {
    printf("+ Map Size : %d*%d MBytes\n", max_pg, map_ctl.pg_size/0x100000);
  }
  else if( map_ctl.pg_size > 0x3ff)
  {
    printf("+ Map Size : %d*%d KBytes\n", max_pg, map_ctl.pg_size/0x400);
  }
  else
  {
    printf("+ Map Size : %d*%d Bytes\n", max_pg, map_ctl.pg_size);
  }
  printf("+ Win Base : 0x%08lx\n", map_ctl.win_base);
  printf("+ Win Size : 0x%08lx [%ld pages]\n", map_ctl.win_size, map_ctl.win_size/(uint)map_ctl.pg_size);
  printf("+----------+----+----+----------+------------------+------+\n");
  printf("|  offset  |flag| usr|   size   |   remote address | mode |\n");
  printf("+----------+----+----+----------+------------------+------+\n");
  for( j = 0; j < map_ctl.pg_num; j++)
  {
    //if( p[j].flag)
    if( p[j].npg)
    {
      printf("| %08x | %02x | %02x | %08x | %016lx | %04x | ", j*map_ctl.pg_size,
	     p[j].flag, p[j].usr, p[j].npg*map_ctl.pg_size, (long)p[j].rem_addr, p[j].mode);
      if( !p[j].flag)
      {
	printf("FREE\n");
      }
      else
      { 
        switch( p[j].mode & 0xff00)
        {
 	  case 0x0000:
	  {
	    printf("PCIe\n");
	    break;
    	  }
          case 0x2000:
	  {
	    printf("SHM1\n");
	    break;
	  }
          case 0x3000:
	  {
	    printf("SHM2\n");
	    break;
	  }
          case 0x4000:
	  {
	    printf("USR1\n");
	    break;
	  }
           case 0x5000:
	  {
	    printf("USR2\n");
	    break;
	  }
         default:
	  {
	    printf("\n");
	    break;
	  }
	}
      }
    }
  }
  printf("+----------+----+----+----------+------------------+------+\n");
  free( map_ctl.map_p);

  return(0);
}

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * Function name : map_clear
 * Prototype     : int
 * Parameters    : name of map to clear
 * Return        : success/error
 *----------------------------------------------------------------------------
 * Description   : clear a mapping
 *
 *++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

int
map_clear( char *name)
{
  struct tsc_ioctl_map_ctl map_ctl;

  if( map_get_id( name, &map_ctl) == MAP_ID_INVALID)
  {
    printf("wrong map name : %s\n", name);
    return( -1);
  } 
  if( tsc_map_clear(tsc_fd, &map_ctl) < 0)
  {
    printf("Cannot access map %s !!\n", name);
    return( -1);
  } 

  return(0);
}

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * Function name : map_alloc
 * Prototype     : int
 * Parameters    : pointer to command parameter list
 * Return        : success/error
 *----------------------------------------------------------------------------
 * Description   : alloc a mapping
 *
 *++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

int 
map_alloc( struct cli_cmd_para *c)
{
  struct tsc_ioctl_map_ctl map_ctl;
  struct tsc_ioctl_map_win map_win;
  char *p;
  int retval;

  printf("Allocating mapping...\n");

  bzero( &map_win, sizeof(map_win));
  if( c->cnt < 4)
  {
    printf("missing parameter : %ld need at least 4\n",  c->cnt);
    goto map_alloc_usage;
  }

  map_win.req.mode.space = (unsigned char)MAP_SPACE_INVALID;
  if( c->ext)
  {
    switch( c->ext[0])
    {
      case'p':
      {
	map_win.req.mode.space = MAP_SPACE_PCIE;
	break;
      }
      case's':
      {
	map_win.req.mode.space = MAP_SPACE_SHM1;
	if( c->ext[1] == '2') map_win.req.mode.space = MAP_SPACE_SHM2;
	break;
      }
      case'u':
      {
	map_win.req.mode.space = MAP_SPACE_USR1;
	if( c->ext[1] == '2') map_win.req.mode.space = MAP_SPACE_USR2;
	break;
      }
    }
  }
  if( map_win.req.mode.space == (char)MAP_SPACE_INVALID)
  {
    if( c->ext)
    {
      printf("wrong remote space : %c\n",  c->ext[0]);
    }
    else 
    {
      printf("remote space shall be specified !!\n");
    }

    goto map_alloc_usage;
  }
  if( map_get_id( c->para[1], &map_ctl) == MAP_ID_INVALID)
  {
    printf("wrong map name : %s\n",  c->para[1]);
    return( -1);
  }
  map_ctl.map_p = (struct tsc_map_blk *)0;
  tsc_map_read(tsc_fd, &map_ctl);
  if( map_ctl.sg_id == MAP_ID_INVALID)
  {
    printf("map %s doesn't exist !!\n", c->para[1]);
    goto map_alloc_usage;
  } 

  map_win.req.mode.sg_id    = map_ctl.sg_id;
  map_win.req.rem_addr = strtoul( c->para[2], &p, 16);
  if( p == c->para[2])
  {
    printf("bad remote address : %s\n",  c->para[2]);
    goto map_alloc_usage;
    return(-1);
  }
  map_win.req.size = strtoul( c->para[3], &p, 16);
  if( p == c->para[3])
  {
    printf("bad window size : %s\n",  c->para[3]);
    goto map_alloc_usage;
  }
  map_win.req.mode.am     = 0x0;
  if( c->cnt > 4)
  {
    map_win.req.mode.am = (char)strtoul( c->para[4], &p, 16);
    if( p == c->para[4])
    {
      printf("bad mapping mode : %s\n",  c->para[4]);
      goto map_alloc_usage;
    }
  }
  map_win.req.mode.flags     = 0x0;
  if( c->cnt > 5)
  {
    map_win.req.loc_addr = strtoul( c->para[5], &p, 16);
    if( p == c->para[5])
    {
      printf("bad local address : %s\n",  c->para[5]);
      goto map_alloc_usage;
    }
    map_win.req.mode.flags = MAP_FLAG_FORCE;
  }
  printf("mapping mode: %x\n", map_win.req.mode.am);
  retval = tsc_map_alloc(tsc_fd, &map_win);
  if( !retval)
  {
    //printf("mapping done at offset [%x]%x\n", map_win.pg_idx*map_ctl.pg_size, map_win.req.win_size);
  }
  else
  {
    printf("Cannot perform mapping\n");
  }
  return( retval);

map_alloc_usage:
  printf("usage: map.<space> alloc <map_name> <rem_addr> <size> <mode> [<loc_addr>]\n");
  return(-1);
}

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * Function name : map_free
 * Prototype     : int
 * Parameters    : pointer to command parameter list
 * Return        : error/success
 *----------------------------------------------------------------------------
 * Description   : free a mapping
 *
 *++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

int 
map_free( struct cli_cmd_para *c)
{
  struct tsc_ioctl_map_ctl map_ctl;
  struct tsc_ioctl_map_win map_win;
  char *p;

  printf("Freeing mapping...\n");

  if( c->cnt < 3)
  {
    printf("missing parameter : %ld need at least 3\n",  c->cnt);
    goto map_free_usage;
  }
  if( map_get_id( c->para[1], &map_ctl) == MAP_ID_INVALID)
  {
    printf("wrong map name : %s\n",  c->para[1]);
    goto map_free_usage;
  }
  map_ctl.map_p = (struct tsc_map_blk *)0;
  tsc_map_read(tsc_fd, &map_ctl);
  if( map_ctl.sg_id == MAP_ID_INVALID)
  {
    printf("map %s doesn't exist !!\n", c->para[1]);
    goto map_free_usage;
  } 
  map_win.req.mode.sg_id    = map_ctl.sg_id;
  map_win.pg_idx = (int)strtol( c->para[2], &p, 16);
  if( p == c->para[2])
  {
    printf("bad window offset : %s\n",  c->para[2]);
    goto map_free_usage;
  }
  map_win.pg_idx = map_win.pg_idx/map_ctl.pg_size;
  return( tsc_map_free(tsc_fd, &map_win));

map_free_usage:
  printf("usage: map free <map_name> <offset>\n");
  return(-1);
}

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * Function name : tsc_map
 * Prototype     : int
 * Parameters    : pointer to command parameter list
 * Return        : ok
 *                 error
 *----------------------------------------------------------------------------
 * Description   : perform read/write acces to tsc csr registers
 *
 *++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

int tsc_map( struct cli_cmd_para *c){
	int cnt, i;

	cnt = c->cnt;
	i = 0;

	if(cnt--){
		if(!strcmp( "show", c->para[i])){
			i++;
			if(cnt){
				map_show( c->para[i]);
				printf("\n");
			}
			else{
				map_show( "mas_mem");
				map_show( "mas_pmem");
				printf("\n");
			}
			return( 0);
		}
		if(!strcmp( "alloc", c->para[i])){
			return( map_alloc( c));
		}
		if(!strcmp( "free", c->para[i])){
			return( map_free( c));
		}
		if(!strcmp( "clear", c->para[i])){
			i++;
			if(cnt){
				map_clear( c->para[1]);
				return( 0);
			}
			else {
				printf("Not enough arguments -> usage:\n");
				tsc_print_usage(c);
				return( -1);
			}
		}
	}
	printf("Not enough arguments -> usage:\n");
	tsc_print_usage(c);
	return(-1);
}
