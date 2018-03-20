/*=========================< begin file & file header >=======================
 *  References
 *  
 *    filename : buf.c
 *    author   : JFG, XP
 *    company  : IOxOS
 *    creation : Sept 15,2015
 *
 *----------------------------------------------------------------------------
 *  Description
 *
 *     That file contains a set of function called by TscMon to perform
 *     buffer control.
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

struct tsc_kbuf_ctl tsc_kbuf_ctl[TSC_NUM_KBUF];

char *
buf_rcsid()
{
  return( rcsid);
}

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * Function name : kbuf_init
 * Prototype     : int
 * Parameters    : none
 * Return        : error/success
 *----------------------------------------------------------------------------
 * Description   : initialize  resources neede by kbuf_xxx commands
 *
 *++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

void
kbuf_init( void)
{
  int i;

  for( i = 0; i < TSC_NUM_KBUF; i++)
  {
    tsc_kbuf_ctl[i].kbuf_p = NULL;
    tsc_kbuf_ctl[i].kbuf_p = NULL;
  }
}

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * Function name : kbuf_cmp
 * Prototype     : int
 * Parameters    : pointer to command parameter list
 * Return        : tsc_ok  if command executed
 *                 tsc_err if error
 *----------------------------------------------------------------------------
 * Description   : buffer compare
 *
 *++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

int
kbuf_cmp( struct cli_cmd_para *c) 
{
  int idx1, idx2, i;
  int off1, off2, size;
  unsigned char *buf1, *buf2;

  if( c->cnt < 2)
  {
      printf("Need size argument\n");
      return( TSC_ERR);
  }
  if( sscanf( c->para[1], "%x:%d", &off1, &idx1) != 2)
  {
    printf("Bad buffer#1 offset [%s]\n",  c->para[1]);
    return( TSC_ERR);
  }
  if( sscanf( c->para[2], "%x:%d", &off2, &idx2) != 2)
  {
    printf("Bad buffer#2 offset [%s]\n",  c->para[2]);
    return( TSC_ERR);
  }
  if( sscanf( c->para[3], "%x", &size) != 1)
  {
    printf("Bad compare size [%s]\n",  c->para[1]);
    return( TSC_ERR);
  }
  if( !tsc_kbuf_ctl[idx1].kbuf_p)
  {
     printf("Kernel buffer#%d not allocated\n",  idx1);
     return( TSC_ERR);
  }
  if( !tsc_kbuf_ctl[idx2].kbuf_p)
  {
     printf("Kernel buffer#%d not allocated\n",  idx2);
     return( TSC_ERR);
  }
  if( ( off1 + size) > tsc_kbuf_ctl[idx1].kbuf_p->size)
  {
     printf("Size overflows buffer #%d\n",  idx1);
     return( TSC_ERR);
  }
  if( ( off2 + size) > tsc_kbuf_ctl[idx2].kbuf_p->size)
  {
     printf("Size overflows buffer #%d\n",  idx1);
     return( TSC_ERR);
  }
  buf1 = (unsigned char *)malloc(size);
  if( !buf1)
  {
     return( TSC_ERR);
  }
  buf2 = (unsigned char *)malloc(size);
  if( !buf2)
  {
    free(buf1);
     return( TSC_ERR);
  }
  tsc_kbuf_read( tsc_kbuf_ctl[idx1].kbuf_p->k_base + off1, (char *)buf1, (uint)size);
  tsc_kbuf_read( tsc_kbuf_ctl[idx2].kbuf_p->k_base + off2, (char *)buf2, (uint)size);
  for( i = 0; i < size; i++)
  {
    if( buf1[i] != buf2[i])
    {
      int j, k;
      printf("Compare error at offset %x\n", i);
      j = i - 8;
      printf("%08x :", off1+j);
      for( k = 0; k < 0x10; k++)
      {
	printf(" %02x", buf1[j+k]);
      }
      printf("\n%08x :", off2+j);
      for( k = 0; k < 0x10; k++)
      {
	printf(" %02x", buf2[j+k]);
      }
      printf("\n");
      break;
    }
  }
  free( buf2);
  free( buf1);

  return( TSC_OK);
}
/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * Function name : kbuf_show
 * Prototype     : int
 * Parameters    : pointer to command parameter list
 * Return        : tsc_ok if command executed
 *                 tsc_err if error
 *----------------------------------------------------------------------------
 * Description   : free a previously allocated
 *
 *++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

int
kbuf_show( struct cli_cmd_para *c) 
{
  int idx, i;

  idx = -1;
  if( c->ext)
  {
    if( (c->ext[0] < '0') || c->ext[0] > '7')
    {
      printf("Buffer index out of range [0->7]\n");
      return( TSC_ERR);
    }
    idx = (int)(c->ext[0] - '0');
  }
  printf("+---+----------+----------+\n");
  printf("|idx|  b_addr  |   size   |\n");
  printf("+---+----------+----------+\n");
  if( idx == -1)
  {
    for( i = 0; i < TSC_NUM_KBUF; i++)
    {
      if( tsc_kbuf_ctl[i].kbuf_p)
      {
	printf("| %d | %08llx | %08x |\n", 
	       i, tsc_kbuf_ctl[i].kbuf_p->b_base, tsc_kbuf_ctl[i].kbuf_p->size);
      }
    }
  }
  else
  {
    if( tsc_kbuf_ctl[idx].kbuf_p )
    {
      printf("| %d | %08llx | %08x |\n", 
	     idx, tsc_kbuf_ctl[idx].kbuf_p->b_base, tsc_kbuf_ctl[idx].kbuf_p->size);
    }
  }
  printf("+---+----------+----------+\n");

  return( TSC_ERR);
}

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * Function name : alloc_kbuf
 * Prototype     : int
 * Parameters    : pointer to command parameter list
 * Return        : tsc_ok  if command executed
 *                 tsc_err if error
 *----------------------------------------------------------------------------
 * Description   : allocate buffer suitable for dma
 *
 *++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

int alloc_kbuf( int idx,
		int size)
{
  tsc_kbuf_ctl[idx].kbuf_p = malloc( sizeof(struct tsc_ioctl_kbuf_req));
  if( !tsc_kbuf_ctl[idx].kbuf_p)
  {
    printf("Cannot allocate kernel buffer\n");
    return( TSC_ERR);
  }
  tsc_kbuf_ctl[idx].kbuf_p->size = (uint)size;
  if( tsc_kbuf_alloc( tsc_kbuf_ctl[idx].kbuf_p))
  {
    printf("Cannot allocate kernel buffer\n");
    free(tsc_kbuf_ctl[idx].kbuf_p);
    tsc_kbuf_ctl[idx].kbuf_p = NULL;
    return( TSC_ERR);
  }
  return( TSC_OK);
}

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * Function name : kbuf_alloc
 * Prototype     : int
 * Parameters    : cli command parameter structure
 * Return        : success/error
 *----------------------------------------------------------------------------
 * Description   : allocate kernel buffer
 *
 *++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

int
kbuf_alloc( struct cli_cmd_para *c) 
{
  int idx;
  int size;
  int retval;

  if( c->cnt < 2)
  {
      printf("Need size argument\n");
      goto kbuf_alloc_exit;
  }
  if( c->ext)
  {
    if( (c->ext[0] < '0') || c->ext[0] > '7')
    {
      printf("Buffer index out of range [0->7]\n");
      return( TSC_ERR);
    }
    idx = (int)( c->ext[0] - '0');
  }
  else
  {
      printf("Need buffer index [0->7]\n");
      goto kbuf_alloc_exit;
  }
  if( tsc_kbuf_ctl[idx].kbuf_p)
  {
    printf("Buffer %d already allocated\n", idx);
    return( TSC_ERR);
  }
  if( sscanf( c->para[1], "%x", &size) != 1)
  {
    printf("Bad buffer size [%s]\n",  c->para[1]);
    return( TSC_ERR);
  }

  retval = alloc_kbuf(idx, size);
  if( retval == TSC_OK)
  {
    kbuf_show(c);
  }

  return( retval);

kbuf_alloc_exit:
  tsc_print_usage( c);
  return( TSC_ERR);
}

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * Function name : kbuf_free
 * Prototype     : int
 * Parameters    : pointer to command parameter list
 * Return        : tsc_ok if command executed
 *                 tsc_err if error
 *----------------------------------------------------------------------------
 * Description   : free a previously kernel buffer allocated
 *
 *++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

int
kbuf_free( struct cli_cmd_para *c) 
{
  int i, idx;

  idx = -1;
  if( c)
  {
    if( c->ext)
    {
      if( (c->ext[0] < '0') || c->ext[0] > '7')
      {
        printf("Buffer index out of range [0->7]\n");
        return( TSC_ERR);
      }
      idx = (int)(c->ext[0] - '0');
    }
  }
  if( idx == -1)
  {
    for( i = 0; i < TSC_NUM_KBUF; i++)
    {
      if( tsc_kbuf_ctl[i].kbuf_p)
      {
        if( tsc_kbuf_ctl[i].map_p)
        {
          tsc_map_free( tsc_kbuf_ctl[i].map_p);
	  free( tsc_kbuf_ctl[i].map_p);
          tsc_kbuf_ctl[i].map_p = NULL;
	}
        tsc_kbuf_free( tsc_kbuf_ctl[i].kbuf_p);
	free( tsc_kbuf_ctl[i].kbuf_p);
        tsc_kbuf_ctl[i].kbuf_p = NULL;
      }
    }
  }
  else
  {
    if( tsc_kbuf_ctl[idx].kbuf_p )
    {
      if( tsc_kbuf_ctl[idx].map_p)
      {
        tsc_map_free( tsc_kbuf_ctl[idx].map_p);
        free( tsc_kbuf_ctl[idx].map_p);
        tsc_kbuf_ctl[idx].map_p = NULL;
      }
      tsc_kbuf_free( tsc_kbuf_ctl[idx].kbuf_p);
      free( tsc_kbuf_ctl[idx].kbuf_p);
      tsc_kbuf_ctl[idx].kbuf_p = NULL;
    }
  }

  return( TSC_OK);
}

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * Function name : map_kbuf
 * Prototype     : int
 * Parameters    : pointer to command parameter list
 * Return        : tsc_ok  if command executed
 *                 tsc_err if error
 *----------------------------------------------------------------------------
 * Description   : map an allocated buffer in the slave address space
 *
 *++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

int 
map_kbuf( int idx,
	  int sg_id,
	  int offset)
{
  if( tsc_kbuf_ctl[idx].map_p)
  {
    printf("Kernel buffer already mapped\n");
    return( TSC_ERR);
  }
  tsc_kbuf_ctl[idx].map_p = malloc( sizeof(struct tsc_ioctl_map_win));
  if( !tsc_kbuf_ctl[idx].map_p)
  {
    printf("Cannot map kernel buffer\n");
    return( TSC_ERR);
  }

  tsc_kbuf_ctl[idx].map_p->req.mode.space = MAP_SPACE_PCIE;
  tsc_kbuf_ctl[idx].map_p->req.mode.sg_id = (char)sg_id;
  tsc_kbuf_ctl[idx].map_p->req.mode.am = (char)0;
  tsc_kbuf_ctl[idx].map_p->req.mode.flags = (char)0;
  tsc_kbuf_ctl[idx].map_p->req.size = tsc_kbuf_ctl[idx].kbuf_p->size;
  tsc_kbuf_ctl[idx].map_p->req.rem_addr = tsc_kbuf_ctl[idx].kbuf_p->b_base;
  tsc_kbuf_ctl[idx].map_p->req.loc_addr = 0;
  if( offset != -1)
  {
    tsc_kbuf_ctl[idx].map_p->req.loc_addr = offset;
    tsc_kbuf_ctl[idx].map_p->req.mode.flags = (char)MAP_FLAG_FORCE;
  }

  if( tsc_map_alloc( tsc_kbuf_ctl[idx].map_p))
  {
    printf("Cannot allocate kernel buffer\n");
    free(tsc_kbuf_ctl[idx].map_p);
    tsc_kbuf_ctl[idx].map_p = NULL;
    return( TSC_ERR);
  }
  return( TSC_OK);
}

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * Function name : kbuf_map
 * Prototype     : int
 * Parameters    : cli command parameter structure
 * Return        : success/error
 *----------------------------------------------------------------------------
 * Description   : map a kernel buffer
 *
 *++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

int
kbuf_map( struct cli_cmd_para *c) 
{
  int idx;
  int sg_id;
  int offset;
  int retval;

  if( c->cnt < 2)
  {
      printf("Need slave map identifier\n");
      goto kbuf_map_exit;
  }
  if( c->ext)
  {
    if( (c->ext[0] < '0') || c->ext[0] > '7')
    {
      printf("Buffer index out of range [0->7]\n");
      return( TSC_ERR);
    }
    idx = (int)( c->ext[0] - '0');
  }
  else
  {
      printf("Need buffer index [0->7]\n");
      goto kbuf_map_exit;
  }
  if( !tsc_kbuf_ctl[idx].kbuf_p)
  {
    printf("Buffer %d has not been allocated\n", idx);
    return( TSC_ERR);
  }

  printf("Mapping kernel buffer %d in %s\n", idx, c->para[1]);
  offset = -1;
  sg_id = MAP_ID_INVALID;
  if( sg_id == MAP_ID_INVALID)
  {
    printf("Bad map identifier %s\n", c->para[1]);
      return( TSC_ERR);
  }
  if( c->cnt > 2)
  {
    if( sscanf( c->para[2], "%x", &offset) != 1)
    {
      printf("Bad slave offset [%s]\n",  c->para[1]);
      return( TSC_ERR);
    }
  }
  retval = map_kbuf( idx, sg_id, offset);;
  printf("Mapping done at offset %lx\n", tsc_kbuf_ctl[idx].map_p->req.loc_addr);

  return( retval);

kbuf_map_exit:
  tsc_print_usage( c);
  return( TSC_ERR);
}

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * Function name : kbuf_unmap
 * Prototype     : int
 * Parameters    : cli command parameter structure
 * Return        : success/error
 *----------------------------------------------------------------------------
 * Description   : unmap kernel buffer
 *
 *++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

int
kbuf_unmap( struct cli_cmd_para *c) 
{
  int i, idx;

  idx = -1;
  if( c)
  {
    if( c->ext)
    {
      if( (c->ext[0] < '0') || c->ext[0] > '7')
      {
        printf("Buffer index out of range [0->7]\n");
        return( TSC_ERR);
      }
      idx = (int)(c->ext[0] - '0');
    }
  }
  if( idx == -1)
  {
    for( i = 0; i < TSC_NUM_KBUF; i++)
    {
      if( tsc_kbuf_ctl[i].map_p)
      {
        printf("Unmappins kernel buffer %d : %08llx\n", i, tsc_kbuf_ctl[i].kbuf_p->b_base);
        tsc_map_free( tsc_kbuf_ctl[i].map_p);
	free( tsc_kbuf_ctl[i].map_p);
        tsc_kbuf_ctl[i].map_p = NULL;
      }
    }
  }
  else
  {
    if( tsc_kbuf_ctl[idx].map_p )
    {
      printf("Unmapping kernel buffer %d : %08llx\n", idx, tsc_kbuf_ctl[idx].kbuf_p->b_base);
      tsc_map_free( tsc_kbuf_ctl[idx].map_p);
      free( tsc_kbuf_ctl[idx].map_p);
      tsc_kbuf_ctl[idx].map_p = NULL;
    }
  }

  return( TSC_OK);
}

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * Function name : tsc_buf
 * Prototype     : int
 * Parameters    : pointer to command parameter list
 * Return        : rdwr_ok  if command executed
 *                 rdwr_err if error
 *----------------------------------------------------------------------------
 * Description   : manage buffer dynamic allocation
 *
 *++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

int tsc_kbuf( struct cli_cmd_para *c){
	int cnt = c->cnt;

	if(cnt--) {
		if( !strcmp( "alloc", c->para[0])) {
			return( kbuf_alloc( c));
		}
		if( !strcmp( "free", c->para[0])) {
			return( kbuf_free( c));
		}
		if( !strcmp( "show", c->para[0])) {
			return( kbuf_show( c));
		}
		if( !strcmp( "cmp", c->para[0])) {
			return( kbuf_cmp( c));
		}
		if( !strcmp( "map", c->para[0])) {
			return( kbuf_map( c));
		}
		if( !strcmp( "unmap", c->para[0])) {
			return( kbuf_unmap( c));
		}
	    printf("Not enough arguments -> usage:\n");
	    tsc_print_usage(c);
		return( TSC_ERR);
	}
    printf("Not enough arguments -> usage:\n");
    tsc_print_usage(c);
	return( TSC_ERR);
}
