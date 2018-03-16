/*=========================< begin file & file header >=======================
 *  References
 *  
 *    filename : rdwr.c
 *    author   : JFG, XP
 *    company  : IOxOS
 *    creation : Sept 15,2015
 *
 *----------------------------------------------------------------------------
 *  Description
 *
 *    That file contains a set of function called by TscMon to perform read
 *    or write cycles through the TSC interface.
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
static char *rcsid = "$Id: rdwr.c,v 1.8 2015/12/11 15:32:26 ioxos Exp $";
#endif

#define DEBUGno
#include <debug.h>
#include <sys/types.h>
#include <sys/time.h>
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
#include <ctype.h>
#include <time.h>
#include "TscMon.h"

typedef unsigned char uchar;
struct cli_cmd_history pm_history;
extern struct aiocb aiocb;
extern char aio_buf[256];
struct rdwr_cycle_para last_csr_cycle;
struct rdwr_cycle_para last_shm_cycle[2];
struct rdwr_cycle_para last_shm2_cycle[2];
struct rdwr_cycle_para last_usr_cycle[2];
struct rdwr_cycle_para last_usr2_cycle[2];
struct rdwr_cycle_para last_pci1_cycle;
struct rdwr_cycle_para last_pci2_cycle;
struct rdwr_cycle_para last_kbuf_cycle[TSC_NUM_KBUF];
extern int script_exit;

extern struct tsc_kbuf_ctl tsc_kbuf_ctl[];

char *
rdwr_rcsid()
{
  return( rcsid);
}

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * Function name : rdwr_init
 * Prototype     : int
 * Parameters    : none
 * Return        : error/success
 *----------------------------------------------------------------------------
 * Description   : initialize  resources neede by rdwr_xxx commands
 *
 *++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

int 
rdwr_init( void)
{
  int i;

  cli_history_init( &pm_history);
  bzero( (char *)&aiocb, sizeof(struct aiocb) ); // Fill byte block with 0
  aiocb.aio_fildes = STDIN_FILENO;		 // Async IO
  aiocb.aio_buf    = aio_buf;
  aiocb.aio_nbytes = 1;

  bzero( &last_csr_cycle, sizeof(struct rdwr_cycle_para));
  last_csr_cycle.len = 0x40;
  last_csr_cycle.m.ads = 0x24;
  last_csr_cycle.m.space = 0x00;
  last_csr_cycle.m.swap = 0x00;
  last_csr_cycle.m.am = 0x00;
  last_csr_cycle.loop = 1;

  bzero( &last_shm_cycle[0], sizeof(struct rdwr_cycle_para));
  last_shm_cycle[0].len = 0x40;
  last_shm_cycle[0].m.ads = 0x44;
  last_shm_cycle[0].m.space = RDWR_SPACE_SHM;
  last_shm_cycle[0].m.swap = 0x00;
  last_shm_cycle[0].m.am = 0x00;
  last_shm_cycle[0].loop = 1;

  bzero( &last_shm2_cycle[0], sizeof(struct rdwr_cycle_para));
  last_shm2_cycle[0].len = 0x40;
  last_shm2_cycle[0].m.ads = 0x44;
  last_shm2_cycle[0].m.space = RDWR_SPACE_SHM2;
  last_shm2_cycle[0].m.swap = 0x00;
  last_shm2_cycle[0].m.am = 0x00;
  last_shm2_cycle[0].loop = 1;

  bzero( &last_usr_cycle[0], sizeof(struct rdwr_cycle_para));
  last_usr_cycle[0].len = 0x40;
  last_usr_cycle[0].m.ads = 0x44;
  last_usr_cycle[0].m.space = RDWR_SPACE_USR1;
  last_usr_cycle[0].m.swap = 0x00;
  last_usr_cycle[0].m.am = 0x00;
  last_usr_cycle[0].loop = 1;

  bzero( &last_usr2_cycle[0], sizeof(struct rdwr_cycle_para));
  last_usr2_cycle[0].len = 0x40;
  last_usr2_cycle[0].m.ads = 0x44;
  last_usr2_cycle[0].m.space = RDWR_SPACE_USR2;
  last_usr2_cycle[0].m.swap = 0x00;
  last_usr2_cycle[0].m.am = 0x00;
  last_usr2_cycle[0].loop = 1;

  bzero( &last_shm_cycle[1], sizeof(struct rdwr_cycle_para));
  last_shm_cycle[1].len = 0x40;
  last_shm_cycle[1].m.ads = 0x44;
  last_shm_cycle[1].m.space = RDWR_SPACE_SHM;
  last_shm_cycle[1].m.swap = 0x00;
  last_shm_cycle[1].m.am = 0x00;
  last_shm_cycle[1].loop = 1;

  bzero( &last_shm2_cycle[1], sizeof(struct rdwr_cycle_para));
  last_shm2_cycle[1].len = 0x40;
  last_shm2_cycle[1].m.ads = 0x44;
  last_shm2_cycle[1].m.space = RDWR_SPACE_SHM2;
  last_shm2_cycle[1].m.swap = 0x00;
  last_shm2_cycle[1].m.am = 0x00;
  last_shm2_cycle[1].loop = 1;

  bzero( &last_usr_cycle[1], sizeof(struct rdwr_cycle_para));
  last_usr_cycle[1].len = 0x40;
  last_usr_cycle[1].m.ads = 0x44;
  last_usr_cycle[1].m.space = RDWR_SPACE_USR1;
  last_usr_cycle[1].m.swap = 0x00;
  last_usr_cycle[1].m.am = 0x00;
  last_usr_cycle[1].loop = 1;

  bzero( &last_usr2_cycle[1], sizeof(struct rdwr_cycle_para));
  last_usr2_cycle[1].len = 0x40;
  last_usr2_cycle[1].m.ads = 0x44;
  last_usr2_cycle[1].m.space = RDWR_SPACE_USR2;
  last_usr2_cycle[1].m.swap = 0x00;
  last_usr2_cycle[1].m.am = 0x00;
  last_usr2_cycle[1].loop = 1;

  bzero( &last_pci1_cycle, sizeof(struct rdwr_cycle_para));
  last_pci1_cycle.len = 0x40;
  last_pci1_cycle.m.ads = 0x44;
  last_pci1_cycle.m.space = RDWR_SPACE_PCIE1;
  last_pci1_cycle.m.swap = 0x00;
  last_pci1_cycle.m.am = 0x00;
  last_pci1_cycle.loop = 1;

  bzero( &last_pci2_cycle, sizeof(struct rdwr_cycle_para));
  last_pci2_cycle.len = 0x40;
  last_pci2_cycle.m.ads = 0x44;
  last_pci2_cycle.m.space = RDWR_SPACE_PCIE2;
  last_pci2_cycle.m.swap = 0x00;
  last_pci2_cycle.m.am = 0x00;
  last_pci2_cycle.loop = 1;

  for( i = 0; i <  TSC_NUM_KBUF; i++)
  {
    bzero( &last_kbuf_cycle[i], sizeof(struct rdwr_cycle_para));
    last_kbuf_cycle[i].len = 0x40;
    last_kbuf_cycle[i].m.ads = 0x44;
    last_kbuf_cycle[i].m.space = RDWR_SPACE_KBUF;
    last_kbuf_cycle[i].m.swap = 0x00;
    last_kbuf_cycle[i].m.am = 0x00;
    last_kbuf_cycle[i].loop = 1;
    last_kbuf_cycle[i].kb_p = NULL;
  }

  if( !alloc_kbuf( 0, 0x100000))
  {
    //printf("kernel buffer allocated: %p %llx %x\n", tsc_kbuf_ctl[0].kbuf_p->k_base, tsc_kbuf_ctl[0].kbuf_p->b_base, tsc_kbuf_ctl[0].kbuf_p->size);
    last_kbuf_cycle[0].kb_p = tsc_kbuf_ctl[0].kbuf_p;
  }
  else
  {
    printf("Cannot allocate kernel buffer\n");
  }

  bzero( (char *)&aiocb, sizeof(struct aiocb) ); // Fill byte block with 0
  aiocb.aio_fildes = STDIN_FILENO;		 // Async IO
  aiocb.aio_buf    = aio_buf;
  aiocb.aio_nbytes = 1;

  return(0);
}

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * Function name : rdwr_exit
 * Prototype     : void
 * Parameters    : none
 * Return        : none
 *----------------------------------------------------------------------------
 * Description   : return resources allocated by rdwr_init()to OS
 *
 *++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

int 
rdwr_exit( void)
{
  return(0);
}

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * Function name : rdwr_get_cycle_xxx
 * Parameters    : pointer to command parameter string
 *                 pointer to cycle parameters data structure
 * Return        : void
 *----------------------------------------------------------------------------
 * Description   : extract address range parameters from command parameter
 *                 and store in cycle parameters data structure
 *
 *++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

struct rdwr_cycle_para * rdwr_get_cycle_space( char *cmd_p){
	int device = -1;

	// Get current device used to match correct command history
	device = tsc_get_device();
	printf("device: %d\n", device);

	if( cmd_p[1] == 'p'){
		if( strlen( cmd_p) > 2 ){
			if( cmd_p[2] == '2'){
				return( &last_pci2_cycle);
			}
		}
		return( &last_pci1_cycle);
	}
	if( cmd_p[1] == 's'){
		if( strlen( cmd_p) > 2 ){
			if( cmd_p[2] == '2'){
				return( &last_shm2_cycle[device]);
			}
		}
		return( &last_shm_cycle[device]);
	}
	if( cmd_p[1] == 'u'){
		if( strlen( cmd_p) > 2 ){
			if( cmd_p[2] == '2'){
				return( &last_usr2_cycle[device]);
			}
		}
		return( &last_usr_cycle[device]);
	}
	if( cmd_p[1] == 'm') {
		last_kbuf_cycle[0].kb_p = tsc_kbuf_ctl[0].kbuf_p;
		return( &last_kbuf_cycle[0]);
	}
	if( cmd_p[1] == 'k'){
		struct rdwr_cycle_para *cp;
		int idx;
		idx = (int)(cmd_p[2] - '0');
		if((idx < 0) || (idx > TSC_NUM_KBUF)){
			return( NULL);
		}
		cp = &last_kbuf_cycle[idx];
		cp->kb_p = tsc_kbuf_ctl[idx].kbuf_p;
		return(cp);
	}
	return( NULL);
}

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * Function name : rdwr_get_cycle_addr
 * Prototype     : int
 * Parameters    : address, rdwr cycle parameter
 * Return        : error/length
 *----------------------------------------------------------------------------
 * Description   : get cycle address
 *
 *++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

static int
rdwr_get_cycle_addr( char *addr_p,
		     struct rdwr_cycle_para *cp)
{
  int start, end;
  int len;
  int retval;

  len = cp->len;
  retval = sscanf( addr_p, "%x..%x", &start, &end);
  if( !retval)
  {
    return( RDWR_ERR);
  }
  cp->addr = (ulong)start;
  if( retval > 1)
  {
    len = end - start;
    if( len < 0) len = -len;
  }
  return( len);
}

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * Function name : rdwr_get_cycle_data
 * Prototype     : int
 * Parameters    : data, rdwr structure
 * Return        : success/error
 *----------------------------------------------------------------------------
 * Description   : get cycle data
 *
 *++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

static int
rdwr_get_cycle_data( char *data_p,
		     struct rdwr_cycle_para *cp)
{
  int retval;
  ulong data, para;
  char op;

  data = cp->data;
  retval = sscanf( data_p, "%lx", &data);
  if( !retval)
  {
    retval = sscanf( data_p, "%c:%lx..%lx", &op, &data, &para);
    if( (op=='d') ||  (op=='r'))
    {
      if( retval > 1)
      {
        cp->data = data;
        cp->operation = op;
	return(0);
      }
    }
    if( (op=='s') ||  (op=='w'))
    {
      if( retval > 2)
      {
        cp->data = data;
        cp->para = para;
        cp->operation = op;
	return(0);
      }
    }
  }
  else
  {
    cp->data = data;
    cp->operation = 'd';
    return(0);
  }
  return(-1);
}

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * Function name : rdwr_get_cycle_ds
 * Prototype     : void
 * Parameters    : ext parameter, rdwr structure
 * Return        : void
 *----------------------------------------------------------------------------
 * Description   : get cycle data size
 *
 *++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

static void
rdwr_get_cycle_ds( char *ext_p,
		   struct rdwr_cycle_para *cp)
{
  if( ext_p)
  {
    switch( ext_p[0])
    {
      case 'b':
      case 'c':
      {
	cp->m.ads = (char)RDWR_MODE_SET_DS( cp->m.ads, RDWR_SIZE_BYTE);
	break;
      }
      case 'h':
      case 's':
      {
	cp->m.ads = (char)RDWR_MODE_SET_DS( cp->m.ads, RDWR_SIZE_SHORT);
	break;
      }
      case 'i':
      case 'w':
      {
	cp->m.ads = (char)RDWR_MODE_SET_DS( cp->m.ads, RDWR_SIZE_INT);
	break;
      }
      case 'l':
      case 'd':
      {
	cp->m.ads = (char)RDWR_MODE_SET_DS( cp->m.ads, RDWR_SIZE_DBL);
	break;
      }
      default:
      {
	break;
      }
    }
  }
}

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * Function name : rdwr_get_cycle_swap
 * Prototype     : void
 * Parameters    : name, rdwr structure
 * Return        : void
 *----------------------------------------------------------------------------
 * Description   : get cycle swap
 *
 *++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

static void
rdwr_get_cycle_swap( char *name,
		     struct rdwr_cycle_para *cp)
{
  char tmp[4];

  if( !name) return;
  cp->m.swap = 0x0;
  if( name[1] == 's')
  {
    cp->m.swap = 0x80;
  }
#ifdef JFG
  bzero( tmp, 4);
  strncpy( tmp, name, 4);

  if(!strncmp("ns",  tmp, 2))
  {
    cp->m.swap = RDWR_SWAP_NO;
  }
  if(!strncmp("as",  tmp, 2))
  {
    cp->m.swap = RDWR_SWAP_AUTO;
  }
  if(!strncmp("ds",  tmp, 2))
  {
    cp->m.swap = RDWR_SWAP_DS;
  }
  if(!strncmp("qs",  tmp, 2))
  {
    cp->m.swap =RDWR_SWAP_QS;
  }
#endif
}

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * Function name : rdwr_get_cycle_loop
 * Prototype     : void
 * Parameters    : para, rdwr structure
 * Return        : void
 *----------------------------------------------------------------------------
 * Description   : get cycle loop
 *
 *++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

static void
rdwr_get_cycle_loop( char *para,
		     struct rdwr_cycle_para *cp)
{
  int loop;

  cp->loop = 10;
  if( sscanf( para, "l:%d", &loop) == 1)
  {
    cp->loop = loop;
  }
  return;
}

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * Function name : rdwr_set_prompt
 * Parameters    : pointer to prompt string
 *                 address to be displayed
 *                 display mode
 * Return        : void
 *----------------------------------------------------------------------------
 * Description   : and store in cycle parameters data structure
 *
 *++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

static char
*rdwr_set_prompt( char *prompt,
		  ulong addr,
		  int mode)
{
  int as;

  as = (mode >> 4)&0xf;
  switch( as)
  {
    case RDWR_MODE_A8: /* A8 */
    {
      sprintf( prompt, "0x%02x : ", (unsigned char)addr);
      break;
    }
    case RDWR_MODE_A16: /* A16 */
    {
      sprintf( prompt, "0x%04x : ", (ushort)addr);
      break;
    }
    case RDWR_MODE_A24: /* A24 */
    {
      sprintf( prompt, "0x%06x : ", (uint)addr);
      break;
    }
    case RDWR_MODE_A64: /* A64 */
    {
      sprintf( prompt, "0x%016lx : ", addr);
      break;
    }
    default: /* A32 */
    {
      sprintf( prompt, "0x%08x : ", (uint)addr);
      break;
    }
  }
  return( prompt);
}

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * Function name : rdwr_sprintf_bin
 * Prototype     : void
 * Parameters    : s, n, bit size
 * Return        : void
 *----------------------------------------------------------------------------
 * Description   : sprintf binary
 *
 *++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

void rdwr_sprintf_bin( char *s, 
		       unsigned n,
		       unsigned int bit_size)
{
  unsigned int i;
  unsigned int j = 1;

  for( i = 1 << (bit_size - 1); i > 0; i = i / 2)
  {
    (n & i) ? sprintf(s++,"1"): sprintf(s++,"0");
    if (!(j % 4))
    {
      sprintf(s++,"'");
    }
    j++;
  }
  *(--s) = 0;
}

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * Function name : rdwr_patch_addr
 * Prototype     : char *
 * Parameters    : addr, data, mode, swap
 * Return        : cmdline
 *----------------------------------------------------------------------------
 * Description   : patch address
 *
 *++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

static char * 
rdwr_patch_addr( ulong addr, 
		 void *data_p,
		 int mode,
		 int swap)
{
  unsigned char *p;
  int ds;
  char pm_prompt[32];
  int idx;
  char bin_string[33];

  p = (unsigned char *)data_p;
  ds = mode & 0xf;
  rdwr_set_prompt( pm_prompt, addr, mode);
  idx = (int)strlen( pm_prompt);
  switch(ds)
  {
    case 1:
    {
      rdwr_sprintf_bin( bin_string, *p, 8);
      sprintf( &pm_prompt[idx], "%02x [%s] -> ", *p, bin_string);
      break;
    }
    case 2:
    {
      if( swap & 0x80)
      {
	rdwr_sprintf_bin( bin_string, tsc_swap_16( *(ushort *)p), 16);
	sprintf( &pm_prompt[idx], "%04x [%s] -> ", (ushort)tsc_swap_16( *(ushort *)p), bin_string);
      }
      else 
      {
	rdwr_sprintf_bin( bin_string, *(ushort *)p, 16);
	sprintf( &pm_prompt[idx], "%04x [%s] -> ", *(ushort *)p,  bin_string);
      }
      break;
    }
    case 4:
    {
      if( swap & 0x80)
      {
	rdwr_sprintf_bin( bin_string, tsc_swap_32( *(uint *)p), 32);
	sprintf( &pm_prompt[idx], "%08x [%s] -> ",  tsc_swap_32( *(uint *)p), bin_string);
      }
      else 
      {
	rdwr_sprintf_bin( bin_string, *(uint *)p, 32);
	sprintf( &pm_prompt[idx], "%08x [%s] -> ", *(uint *)p, bin_string);
      }
      break;
    }
    case 8:
    {
      sprintf( &pm_prompt[idx], "%016lx -> ", *(ulong *)p);
      break;
    }
  }
  return( cli_get_cmd( &pm_history, pm_prompt));
}

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * Function name : tsc_rdwr_pr
 * Prototype     : int
 * Parameters    : pointer to command parameter list
 * Return        : rdwr_ok  if command executed
 *                 rdwr_err if error
 *----------------------------------------------------------------------------
 * Description   : perform read/write access to csr registers
 *
 *++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

int 
tsc_rdwr_pr( struct cli_cmd_para *c)
{
  int offset;
  int data;
  int retval;
  int iex;
  int as, ds;
  struct rdwr_cycle_para *cp;
  char *next, *p;

  cp = &last_csr_cycle;
  if( !c->cnt)
  {
    offset = (int)cp->addr;
  }
  else if( sscanf( c->para[0], "%x", &offset) != 1)
  {
    printf("Bad offset argument [%s] -> usage:\n", c->para[0]);
    tsc_print_usage( c);
    return( RDWR_ERR);
  }
  offset &= ~3;
  cp->addr = (ulong)offset;
  if( c->cnt > 1)
  {
    /* check for read and exit */
    if( c->para[1][0] == '.')
    {
      if( c->cmd[1] == 'c')
      {
        if( c->cmd[2] == '1')
        {
          retval =  tsc_pciep_read( TSC_A7_PCIEP1_ADDPT_CFG | (offset/4), &data);
	}
	else 
	{
          retval =  tsc_pciep_read( TSC_A7_PCIEP_ADDPT_CFG | (offset/4), &data);
	}
      }
      else if( c->cmd[1] == 'i')
      {
        retval =  tsc_pon_read( offset, &data);
      }
      else
      {
        retval =  tsc_csr_read( offset, &data);
      }
      if( retval < 0)
      {
        printf("cannot access TSC register %x -> error %d\n", offset, retval);
        return( RDWR_ERR);
      }
      printf("0x%04x : 0x%08x -> \n", offset, data);
      return( RDWR_OK);
    }
    if( sscanf( c->para[1], "%x", &data) != 1)
    {
      printf("Bad data argument [%s] -> usage:\n", c->para[1]);
      tsc_print_usage( c);
      return( RDWR_ERR);
    }
    if( c->cmd[1] == 'c')
    {
      if( c->cmd[2] == '1')
      {
        retval =  tsc_pciep_write( TSC_A7_PCIEP1_ADDPT_CFG | (offset/4), &data);
      }
      else 
      {
        retval =  tsc_pciep_write( TSC_A7_PCIEP_ADDPT_CFG | (offset/4), &data);
      }
    }
    else if( c->cmd[1] == 'i')
    {
      retval =  tsc_pon_write( offset, &data);
    }
    else
    {
      retval =  tsc_csr_write( offset, &data);
    }
    if( retval < 0)
    {
      printf("cannot access TSC register %x -> error %d\n", offset, retval);
      return( RDWR_ERR);
    }
    return( RDWR_OK);
  }
  iex = 1;
  ds = RDWR_SIZE_INT;
  as = RDWR_MODE_A16;
  cp->m.ads = (char)RDWR_MODE_SET_DS( cp->m.ads, ds);
  cp->m.ads = (char)RDWR_MODE_SET_AS( cp->m.ads, as);
  while( iex)
  {
    if( c->cmd[1] == 'c')
    {
      if( c->cmd[2] == '1')
      {
        retval =  tsc_pciep_read( TSC_A7_PCIEP1_ADDPT_CFG | (offset/4), &data);
      }
      else 
      {
        retval =  tsc_pciep_read( TSC_A7_PCIEP_ADDPT_CFG | (offset/4), &data);
      }
    }
    else if( c->cmd[1] == 'i')
    {
      retval =  tsc_pon_read( offset, &data);
    }
    else
    {
      retval =  tsc_csr_read( offset, &data);
    }
    if( retval < 0)
    {
      printf("cannot access TSC register 0x%x -> error %d\n", offset, retval);
      return( RDWR_ERR);
    }
    next = rdwr_patch_addr( (ulong)offset, (void *)&data, cp->m.ads, 0);
    switch( next[0])
    {
      case 0:
      {
	offset += ds;
        break;
      }
      case '.':
      {
	iex = 0;
        break;
      }
      case '-':
      {
	offset -= ds;
        break;
      }
      case '=':
      {
        break;
      }
      default:
      {
	data = (int)strtoul( next, &p, 16);
	if( p == next)
        {
	  printf("Format error\n");
	}
	else
	{
          if( c->cmd[1] == 'c')
          {
            if( c->cmd[2] == '1')
            {
	      retval =  tsc_pciep_write( TSC_A7_PCIEP1_ADDPT_CFG | (offset/4), &data);
            }
            else 
            {
	      retval =  tsc_pciep_write( TSC_A7_PCIEP_ADDPT_CFG | (offset/4), &data);
	    }
	  }
	  else if( c->cmd[1] == 'i')
          {
	    retval =  tsc_pon_write( offset, &data);
	  }
	  else 
          {
	    retval =  tsc_csr_write( offset, &data);
	  }
          if( retval < 0)
          {
            printf("cannot access TSC register %x -> error %d\n", offset, retval);
            return( RDWR_ERR);
          }
	  offset += ds;
	}
        break;
      }
    }
    cp->addr = (ulong)offset;
  }
  return( RDWR_OK);
}

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * Function name : tsc_rdwr_dr
 * Prototype     : int
 * Parameters    : pointer to command parameter list
 * Return        : rdwr_ok  if command executed
 *                 rdwr_err if error
 *----------------------------------------------------------------------------
 * Description   : display csr registers
 *
 *++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

int 
tsc_rdwr_dr( struct cli_cmd_para *c)
{
  int start, end, offset;
  int i, len, inc;
  int data;
  int retval;
  struct rdwr_cycle_para *cp;

  cp = &last_csr_cycle;
  if( c->cnt)
  {
    retval = sscanf( c->para[0], "%x..%x", &start, &end);
    if( !retval)
    {
      printf("Bad offset argument [%s] -> usage:\n", c->para[0]);
      tsc_print_usage( c);
      return( RDWR_ERR);
    }
    start &= ~3;
    if( retval == 1)
    {
      len = cp->len;
    }
    else
    {
      len = ((end+3)&~3) - start;
      if( len < 0) len = -len;
      cp->len = len;
    }
  }
  else
  {
    start = (int)cp->addr;
    len = cp->len;
  }
  inc = 4;
  if( c->cmd[1] == 'c')
  {
    inc = 1;
    len = len/4;
  }
  for( i = 0; i < len; i += inc)
  {
    offset = start + i;
    if( c->cmd[1] == 'c')
    {
      retval = tsc_pciep_read( TSC_A7_PCIEP_ADDPT_CFG | offset, &data);
      if( retval < 0)
      {
        printf("\ncannot access TSC PCI CFG register 0x%x -> error %d\n", offset*4, retval);
        return( RDWR_ERR);
      }
      if( !((i*4)&0xf)) printf("\n%08x : ", 4*offset);
      printf("%08x ", data);
    }
    else if( c->cmd[1] == 'i')
    {
      retval = tsc_pon_read( offset, &data);
      if( retval < 0)
      {
        printf("\ncannot access PON register 0x%x -> error %d\n", offset, retval);
        return( RDWR_ERR);
      }
      if( !(i&0xf)) printf("\n%08x : ", offset);
      printf("%08x ", data);
    }
    else
    {
      retval = tsc_csr_read( offset, &data);
      if( retval < 0)
      {
        printf("\ncannot access TSC register 0x%x -> error %d\n", offset, retval);
        return( RDWR_ERR);
      }
      if( !(i&0xf)) printf("\n%08x : ", offset);
      printf("%08x ", data);
    }
  }
  printf("\n\n");
  cp->addr = (ulong)(offset+4);
  return( RDWR_OK);
}

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * Function name : tsc_show_addr
 * Prototype     : int
 * Parameters    : pointer to command parameter list
 * Return        : rdwr_ok  if command executed
 *                 rdwr_err if error
 *----------------------------------------------------------------------------
 * Description   : display address range from tsc remote resource (shm)
 *
 *++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

static void
rdwr_show_addr( ulong addr,
		int mode)
{
  int as;

  as = (mode >> 8)&0xff;
  switch( as)
  {
    case 1: /* A8 */
    {
      printf("0x%02x : ", (uchar)addr);
      break;
    }
    case 2: /* A16 */
    {
      printf("0x%04x : ", (ushort)addr);
      break;
    }
    case 3: /* A24 */
    {
      printf("0x%06x : ", (uint)addr);
      break;
    }
    case 8: /* A64 */
    {
      printf("0x%016lx : ", addr);
      break;
    }
    default: /* A32 */
    {
      printf("0x%08x : ", (uint)addr);
      break;
    }
  }
  return;
}

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * Function name : rdwr_show_buf
 * Prototype     : int
 * Parameters    : addr, buf, len, mode
 * Return        : success/error
 *----------------------------------------------------------------------------
 * Description   : show buffer
 *
 *++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

static int 
rdwr_show_buf( ulong addr, 
	       void *buf,
	       int len,
	       int mode)
{
  unsigned char *p;
  int i, j;
  int ds, swap;

  p = (unsigned char *)buf;
  ds = mode & 0xf;
  swap = mode & 0x80;
  for( i = 0; i < len; i += 16)
  {
    rdwr_show_addr( addr, mode);
    for( j = 0; j < 16; j += ds)
    {
      if( ( i + j) >= len)
      {
	long n;
	n = ((2*ds)+1)*((16-j)/ds);
	while( n--)
	{
	  putchar(' ');
	}
	break;
      }
      switch( ds)
      {
        case 1:
	{
	  printf("%02x ", p[j]);
	  break;
	}
        case 2:
	{
	  if(swap) printf("%04x ", (ushort)tsc_swap_16( *(short *)&p[j]));
	  else printf("%04x ", *(ushort *)&p[j]);
	  break;
	}
        case 4:
	{
	  if(swap) printf("%08x ", (uint)tsc_swap_32( *(int *)&p[j]));
	  else printf("%08x ", *(uint *)&p[j]);
	  break;
	}
        case 8:
	{
	  if(swap) printf("%016llx ", (uint64_t)tsc_swap_64( *(uint64_t *)&p[j]));
	  printf("%016llx ", *(uint64_t *)&p[j]);
	  break;
	}
      }
    }
    printf(" ");
    for( j = 0; j < 16; j++)
    {
      char c;
      if( ( i + j) >= len)
      {
	break;
      }

      c = p[j];
      if( !isprint(c)) c = '.';
      printf("%c", c);
    }
    printf("\n");
    p += 16;
    addr += 16;
  }
  return(0);
}

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * Function name : tsc_rdwr_dx
 * Prototype     : int
 * Parameters    : cli command parameter structure
 * Return        : error/success
 *----------------------------------------------------------------------------
 * Description   : perform display data
 *
 *++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

int 
tsc_rdwr_dx( struct cli_cmd_para *c)
{
  struct rdwr_cycle_para *cp;
  char *buf;
  int len;

  cp = rdwr_get_cycle_space( c->cmd);
  if( !cp)
  {
    printf("Bad space argument [%s] -> usage:\n", c->para[0]);
    goto tsc_rdwr_dx_error;
  }
  len = cp->len;
  if( c->cnt)
  {
    len = rdwr_get_cycle_addr(  c->para[0], cp);
    if( len < 0)
    {
      printf("Bad address argument [%s] -> usage:\n", c->para[0]);
      goto tsc_rdwr_dx_error;
    }
    cp->len = len;
  }
  if( c->ext)
  {
    rdwr_get_cycle_ds( c->ext, cp);
    rdwr_get_cycle_swap( c->ext, cp);
  }
  buf = (char *)malloc( (size_t)cp->len);
  if( (cp->m.space & RDWR_SPACE_MASK) == RDWR_SPACE_KBUF)
  {
    if( !cp->kb_p)
    {
      printf("kernel buffer not available\n");
      free(buf);
      return( RDWR_ERR);
    }
    tsc_kbuf_read( cp->kb_p->k_base + cp->addr, buf, (uint)cp->len);
  }
  else 
  {
    tsc_read_blk( cp->addr, buf, cp->len, cp->mode);
  }
  rdwr_show_buf( cp->addr, buf, cp->len, cp->m.swap | RDWR_MODE_GET_DS( cp->m.ads));
  cp->addr += (ulong)cp->len;
  free(buf);

  return(RDWR_OK);

tsc_rdwr_dx_error:
  tsc_print_usage( c);
  return( RDWR_ERR);
}

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * Function name : rdwr_fill_buf
 * Prototype     : int
 * Parameters    : pointer to command parameter list
 * Return        : rdwr_ok  if command executed
 *                 rdwr_err if error
 *----------------------------------------------------------------------------
 * Description   : fill tsc buffer with data
 *
 *++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

int 
rdwr_fill_buf( void *buf,
	       int len,
	       struct rdwr_cycle_para *cp)
{
  int i, ds, retval;

  ds = RDWR_MODE_GET_DS( cp->m.ads);
  retval = (int)cp->data;
  if( cp->operation == 'r') srandom( (uint)cp->data);
  switch( ds)
  {
    case RDWR_SIZE_BYTE:
    {
      char *p;
      char data;

      p = (char *)buf;
      data = (char)cp->data;
      for( i = 0; i < len; i+=1)
      {
	if( cp->operation == 'w')
	{ 
	  char para;
	  int shift;

	  shift = i & 0x7;
	  para = ( (char)(cp->para << shift)) | ( (char)(cp->para >> ( 0x8 - shift)));
	  data =  (char)cp->data ^ para;
	}
        *p++ = data;
	if( cp->operation == 'r') data =  (char)random();
	if( cp->operation == 's') data = (char)((int)data + (int)cp->para);
	if( cp->operation == 'w') data =  (char)cp->data;
      }
      retval = (int)data;
      break;
    }
    case RDWR_SIZE_SHORT:
    {
      ushort *p;
      ushort data;

      p = (ushort *)buf;
      data = (ushort)cp->data;
      for( i = 0; i < len; i+=2)
      {
	if( cp->operation == 'w')
	{ 
	  ushort para;
	  int shift;

	  shift = (i/2) & 0xf;
	  para = ((ushort)(cp->para << shift)) | ((ushort)(cp->para >> ( 0x10 - shift)));
	  data =  (ushort)cp->data ^ para;
	}
	if( cp->m.swap & 0x80) *p++ = tsc_swap_16( data);
        else *p++ = data;
	if( cp->operation == 'r') data =  (ushort)random();
	if( cp->operation == 's') data =  (ushort)((int)data + (int)cp->para);
	if( cp->operation == 'w') data =  (ushort)cp->data;
      }
      retval = (int)data;
      break;
    }
    case RDWR_SIZE_INT:
    {
      uint *p;
      uint data;

      p = (uint *)buf;
      data = (uint)cp->data;
      for( i = 0; i < len; i+=4)
      {
	if( cp->operation == 'w')
	{ 
	  uint para;
	  int shift;

	  shift = (i/4) & 0x1f;
	  para = ((uint)cp->para << shift) | ((uint)cp->para >> ( 0x20 - shift));
	  data =  (uint)cp->data ^ para;
	}
	if( cp->m.swap & 0x80) *p++ = tsc_swap_32( data);
        else *p++ = data;
	if( cp->operation == 'r') data =  (uint)((random()<<24) + random());
	if( cp->operation == 's') data +=  (uint)cp->para;
	if( cp->operation == 'w') data =  (uint)cp->data;
      }
      retval = (int)data;
      break;
    }
  }
  return(retval);
}

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * Function name : tsc_rdwr_fx
 * Prototype     : int
 * Parameters    : pointer to command parameter list
 * Return        : rdwr_ok  if command executed
 *                 rdwr_err if error
 *----------------------------------------------------------------------------
 * Description   : fill tsc remote resource address range shm
 *                 with data
 *
 *++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

int tsc_rdwr_fx( struct cli_cmd_para *c){
	struct rdwr_cycle_para *cp;
	char *buf;
	int i, nblk, last, blk, len;
	ulong addr;
	int cnt = c->cnt;

	if(cnt--){
		cp = rdwr_get_cycle_space( c->cmd);
		if( !cp){
			printf("Bad space argument [%s] -> usage:\n", c->para[0]);
			tsc_print_usage( c);
			return( RDWR_ERR);
		}
		len = cp->len;
		if( c->cnt){
			len = rdwr_get_cycle_addr(  c->para[0], cp);
			if( len < 0){
				printf("Bad offset argument [%s] -> usage:\n", c->para[0]);
				tsc_print_usage( c);
				return( RDWR_ERR);
			}
			if( c->cnt > 1){
				if( rdwr_get_cycle_data(  c->para[1], cp) < 0){
					printf("Bad data argument [%s] -> usage:\n", c->para[0]);
					tsc_print_usage( c);
					return( RDWR_ERR);
				}
			}
		}
		if( c->ext){
			rdwr_get_cycle_ds( c->ext, cp);
			rdwr_get_cycle_swap( c->ext, cp);
		}
		blk = 0x100000;
		nblk = len/blk;
		last = len%blk;

		addr = cp->addr;
		for( i = 0; i < nblk; i++){
			buf = (char *)malloc( (size_t)blk);
			cp->data = (ulong)rdwr_fill_buf( buf, blk, cp);
			if( (cp->m.space & RDWR_SPACE_MASK) == RDWR_SPACE_KBUF){
				if( !cp->kb_p){
					printf("kernel buffer not available\n");
					return( RDWR_ERR);
				}
				tsc_kbuf_write( cp->kb_p->k_base + addr, buf, (uint)blk);
			}
			else {
				tsc_write_blk( addr, buf, blk, cp->mode);
			}
			addr += blk;

			free(buf);
		}
		if( last){
			buf = (char *)malloc( (size_t)last);
			cp->data = (ulong)rdwr_fill_buf( buf, last, cp);
			if( (cp->m.space & RDWR_SPACE_MASK) == RDWR_SPACE_KBUF){
				if( !cp->kb_p){
					printf("kernel buffer not available\n");
					return( RDWR_ERR);
				}
				tsc_kbuf_write( cp->kb_p->k_base + addr, buf, (uint)last);
			}
			else {
				tsc_write_blk( addr, buf, last, cp->mode);
			}
			free(buf);
		}

		return(RDWR_OK);
	}
	else {
	    printf("Not enough arguments -> usage:\n");
	    tsc_print_usage(c);
		return( RDWR_ERR);
	}
}

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * Function name : rdwr_cmp_buf
 * Prototype     : int
 * Parameters    : pointer to command parameter list
 * Return        : rdwr_ok  if command executed
 *                 rdwr_err if error
 *----------------------------------------------------------------------------
 * Description   : test tsc remote resource address range SHM
 *                 with data (compare)
 *
 *++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

int 
rdwr_cmp_buf( void *buf1,
	      void *buf2,
	      int len,
	      long mask)
{
  long *p1, *p2;
  int i;
  long in, out;

  p1 = (long *)buf1;
  p2 = (long *)buf2;
  i = 0;
  while( i < len)
  {
    in = *p1++;
    out = *p2++;
    if( (in & mask) != (out & mask))
    {
      break;
    }
    i += sizeof( long);
  }
  return( i);
}

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * Function name : rdwr_tx_error
 * Prototype     : int
 * Parameters    : buffer in, buffer out, idt, ds, base
 * Return        : 0
 *----------------------------------------------------------------------------
 * Description   : error management
 *
 *++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

int
rdwr_tx_error( void *buf_in,
	       void *buf_out,
	       uint idx,
	       int ds,
	       ulong base)
{
  printf("Data error at address %x \n", idx);
  script_exit = 1;
  idx &= 0xffff8;
  switch( ds)
  {
    case RDWR_SIZE_BYTE:
    {
      unsigned char *p;

      p = (unsigned char *)buf_in + idx;
      printf("Expected pattern -> %08lx : %02x %02x %02x %02x %02x %02x %02x %02x\n",
	     base + idx, p[0], p[1], p[2], p[3], p[4], p[5], p[6], p[7]);
      p = (unsigned char *)buf_out + idx;
      printf("Actual pattern   -> %08lx : %02x %02x %02x %02x %02x %02x %02x %02x\n",
	     base + idx, p[0], p[1], p[2], p[3], p[4], p[5], p[6], p[7]);
      break;
    }
    case RDWR_SIZE_SHORT:
    {
      unsigned short *p;

      p = (unsigned short *)buf_in + idx/sizeof(short);
      printf("Expected pattern -> %08lx : %02x %02x %02x %02x\n",
	     base + idx, p[0], p[1], p[2], p[3]);
      p = (unsigned short *)buf_out + idx/sizeof(short);
      printf("Actual pattern   -> %08lx : %02x %02x %02x %02x\n",
	     base + idx, p[0], p[1], p[2], p[3]);
      break;
    }
    case RDWR_SIZE_INT:
    {
      unsigned int *p;

      p = (unsigned int *)buf_in + idx/sizeof(int);
      printf("Expected pattern -> %06x : %08x %08x\n", 
	     idx, p[0], p[1]);
      p = (unsigned int *)buf_out + idx/sizeof(int);
      printf("Actual pattern   -> %06x : %08x %08x\n", 
	     idx, p[0], p[1]);
      break;
    }
    case RDWR_SIZE_DBL:
    {
      unsigned long *p;

      p = (unsigned long *)buf_in + idx/sizeof(long);
      printf("Expected pattern -> %06x : %016lx\n", idx, *p);
      p = (unsigned long *)buf_out + idx/sizeof(long);
      printf("Actual pattern   -> %06x : %016lx\n", idx, *p);
      break;
    }
  }
  return( 0);
}

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * Function name : rdwr_test
 * Prototype     : int
 * Parameters    : addr, len, rdwr structure
 * Return        : success/error
 *----------------------------------------------------------------------------
 * Description   : read write test
 *
 *++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

int 
rdwr_test( ulong addr,
	   int len,
	   struct rdwr_cycle_para *cp)
{
  char *buf_in, *buf_out;
  int retval, offset;

  retval = RDWR_OK;
  buf_in = (char *)malloc( (size_t)len);
  buf_out = (char *)malloc( (size_t)len);

  cp->data = (ulong)rdwr_fill_buf( buf_in, len, cp);
  tsc_write_blk( addr, buf_in, len, cp->mode);
  tsc_read_blk( addr, buf_out, len, cp->mode);
  offset = rdwr_cmp_buf( buf_in, buf_out, len, -1);
  if( offset < len)
  {
    rdwr_tx_error( buf_in, buf_out, offset, RDWR_MODE_GET_DS( cp->m.ads), addr);
    retval = RDWR_ERR;
  }
  free(buf_in);
  free(buf_out);
  return( retval);
}

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * Function name : tsc_rdwr_tx
 * Prototype     : int
 * Parameters    : cli command parameter structure
 * Return        : success/error
 *----------------------------------------------------------------------------
 * Description   : read write memory test
 *
 *++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

int 
tsc_rdwr_tx( struct cli_cmd_para *c)
{
  struct rdwr_cycle_para *cp;
  int len, cnt;
  time_t tm, tm_start, tm_end;

  cp = rdwr_get_cycle_space( c->cmd);
  if( !cp)
  {
    printf("Bad space argument [%s] -> usage:\n", c->para[0]);
    goto tsc_rdwr_tx_error;
  }
  if( (cp->m.space & RDWR_SPACE_MASK) == RDWR_SPACE_KBUF)
  {
    printf("Bad space argument [%s] -> usage:\n", c->para[0]);
    goto tsc_rdwr_tx_error;
  }
  len = cp->len;
  if( c->cnt)
  {
    len = rdwr_get_cycle_addr(  c->para[0], cp);
    if( len < 0)
    {
      printf("Bad offset argument [%s] -> usage:\n", c->para[0]);
      goto tsc_rdwr_tx_error;
    }
    if( c->cnt > 1)
    {
      if( rdwr_get_cycle_data(  c->para[1], cp) < 0)
      {
         printf("Bad data argument [%s] -> usage:\n", c->para[0]);
	 goto tsc_rdwr_tx_error;
      }
    }
    if( c->cnt > 2)
    {
   	rdwr_get_cycle_loop( c->para[2], cp);
    }
  }
  if( c->ext)
  {
    rdwr_get_cycle_ds( c->ext, cp);
    rdwr_get_cycle_swap( c->ext, cp);
  }
  cnt = 0;
  printf("Enter any key to stop memory test\n");
  printf("Loop [%d]: %d\r", cp->loop, cnt);
  fflush(stdout);
  tm = time(0);
  tm_start = tm;
  if( aio_error( &aiocb) != EINPROGRESS)
  {
    if( aio_read( &aiocb) < 0)
    {
      perror("aio_read");
      goto xprs_rdwr_tx_exit;
    }
  }
  while( (cnt < cp->loop) || !cp->loop)
  {
    int i, nblk, last, blk;
    ulong addr;

    blk = 0x100000;
    nblk = len/blk;
    last = len%blk;

    addr = cp->addr;
    for( i = 0; i < nblk; i++)
    {
      printf("Loop [%d]: %d - %08x\r", cp->loop, cnt, (uint)addr);
      fflush(stdout);
      if( rdwr_test( addr, blk, cp) == RDWR_ERR)
      {
	goto xprs_rdwr_tx_exit;
      }
      addr += blk;
      if( aio_error( &aiocb) != EINPROGRESS)
      {
        aio_return( &aiocb);
	goto xprs_rdwr_tx_exit;
      }
    }
    if( last)
    {
      printf("Loop [%d]: %d - %08x\r", cp->loop, cnt, (uint)addr);
      fflush(stdout);
      if( rdwr_test( addr, last, cp) == RDWR_ERR)
      {
	goto xprs_rdwr_tx_exit;
      }
      if( aio_error( &aiocb) != EINPROGRESS)
      {
        aio_return( &aiocb);
	goto xprs_rdwr_tx_exit;
      }
    }
    cnt += 1;
  }
  if( aio_error( &aiocb) == EINPROGRESS)
  {
    aio_cancel( aiocb.aio_fildes, &aiocb);
  }

xprs_rdwr_tx_exit:
  tm_end = time(0);
  printf("Loop [%d]: %d - %08x    elapsed time = %ld sec\n", cp->loop, cnt, (uint)cp->addr, tm_end - tm_start);

  return(RDWR_OK);

tsc_rdwr_tx_error:
  tsc_print_usage( c);
  return( RDWR_ERR);
}

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * Function name : tsc_rdwr_px
 * Prototype     : int
 * Parameters    : pointer to command parameter list
 * Return        : rdwr_ok  if command executed
 *                 rdwr_err if error
 *----------------------------------------------------------------------------
 * Description   : perform read/write access to tsc remote resources
 *
 *++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

int 
tsc_rdwr_px( struct cli_cmd_para *c)
{
  ulong offset;
  int data;
  char buf[8];
  int retval;
  int iex;
  int ds;
  struct rdwr_cycle_para *cp;
  char *next, *p;
  char bin_string[33];

  cp = rdwr_get_cycle_space( c->cmd);
  if( !cp)
  {
    printf("Bad space argument [%s] -> usage:\n", c->para[0]);
    goto tsc_rdwr_px_error;
  }
  rdwr_get_cycle_ds( c->ext, cp);
  ds = RDWR_MODE_GET_DS(cp->m.ads);
  rdwr_get_cycle_swap( c->ext, cp);
  if( !c->cnt)
  {
    offset = cp->addr;
  }
  else 
  {
    if( sscanf( c->para[0], "%lx", &offset) != 1)
    {
      printf("Bad address argument [%s] -> usage:\n", c->para[0]);
      goto tsc_rdwr_px_error;
    }
    cp->addr = (ulong)offset;
  }
  if( c->cnt > 1)
  {
    /* check for read and exit */
    if( c->para[1][0] == '.')
    {
      if( (cp->m.space & RDWR_SPACE_MASK) == RDWR_SPACE_KBUF)
      {
        if( !cp->kb_p)
        {
          printf("kernel buffer not available\n");
          return( RDWR_ERR);
        }
	retval =  tsc_kbuf_read( cp->kb_p->k_base + offset, buf, (uint)ds);
      }
      else 
      {
	retval =  tsc_read_sgl( offset, buf, cp->mode);
      }
      if( retval < 0)
      {
        printf("cannot access TSC resoource at offfset %lx -> error %d\n", offset, retval);
        return( RDWR_ERR);
      }
      switch( ds)
      {
        case RDWR_SIZE_BYTE:
        {
	  rdwr_sprintf_bin( bin_string,  *(unsigned char *)buf, 8);
          printf("0x%08lx : 0x%02x [0b%8b] -> \n", offset, *(unsigned char *)buf, bin_string);
          break;
        }
        case RDWR_SIZE_SHORT:
        {
	  unsigned short data_s;

	  data_s =  *(unsigned short *)buf;
	  if( cp->m.swap & 0x80)
	  {
	    data_s = (unsigned short)tsc_swap_16( data_s);
	  }
	  rdwr_sprintf_bin( bin_string,  data_s, 16);
	  printf("0x%08lx : 0x%04x [%s] -> \n", offset, data_s, bin_string);
          break;
        }
        case RDWR_SIZE_INT:
        {
	  unsigned int data_i;

	  data_i =  *(unsigned int *)buf;
	  if( cp->m.swap & 0x80)
	  {
	    data_i = (unsigned int)tsc_swap_32( data_i);
	  }
	  rdwr_sprintf_bin( bin_string,  data_i, 32);
	  printf("0x%08lx : 0x%08x [%s] -> \n", offset, data_i, bin_string);
          break;
        }
      }
      return( RDWR_OK);
    }
    /* check for read and continue */
    if( c->para[1][0] != '?')
    {
      if( sscanf( c->para[1], "%x", &data) != 1)
      {
        printf("Bad data argument [%s] -> usage:\n", c->para[1]);
        goto tsc_rdwr_px_error;
      }
      if( ds == RDWR_SIZE_BYTE)  *(char *)buf = (char)data;
      if( ds == RDWR_SIZE_SHORT)
      {
	if( cp->m.swap & 0x80)
	{
	  *(short *)buf = tsc_swap_16( (short)data);
	}
	else 
	{
	  *(short *)buf = (short)data;
	}
      }
      if( ds == RDWR_SIZE_INT)   
      {
	if( cp->m.swap & 0x80)
	{
	  *(int *)buf = tsc_swap_32( data);
	}
	else 
	{
	  *(int *)buf = data;
	}
      }
      if( (cp->m.space & RDWR_SPACE_MASK) == RDWR_SPACE_KBUF)
      {
        if( !cp->kb_p)
        {
          printf("kernel buffer not available\n");
          return( RDWR_ERR);
        }
 	retval =  tsc_kbuf_write( cp->kb_p->k_base + offset, buf, (uint)ds);
      }
      else 
      {
	retval =  tsc_write_sgl( offset, buf, cp->mode);
      }
      if( retval < 0)
      {
        printf("cannot access TSC register %lx -> error %d\n", offset, retval);
        return( RDWR_ERR);
      }
      return( RDWR_OK);
    }
  }
  iex = 1;
  while( iex)
  {
    if( (cp->m.space & RDWR_SPACE_MASK) == RDWR_SPACE_KBUF)
    {
      if( !cp->kb_p)
      {
        printf("kernel buffer not available\n");
        return( RDWR_ERR);
      }
      retval =  tsc_kbuf_read( cp->kb_p->k_base + offset, buf, (uint)ds);
    }
    else 
    {
      retval =  tsc_read_sgl( offset, buf, cp->mode);
    }
    if( retval < 0)
    {
      printf("cannot access TSC register 0x%lx -> error %d\n", offset, retval);
      return( RDWR_ERR);
    }
    next = rdwr_patch_addr( offset, (void *)buf, cp->m.ads, cp->m.swap);
    switch( next[0])
    {
      case 0:
      {
	offset += (ulong)ds;
        break;
      }
      case '.':
      {
	iex = 0;
        break;
      }
      case '-':
      {
	offset -= (ulong)ds;
        break;
      }
      case '=':
      {
        break;
      }
      default:
      {
	data = (int)strtoul( next, &p, 16);
	if( p == next)
        {
	  printf("Format error\n");
	}
	else
	{
          if( ds == RDWR_SIZE_BYTE)  *(char *)buf = (char)data;
          if( ds == RDWR_SIZE_SHORT)
	  {
	    if( cp->m.swap & 0x80)
	    {
	      *(short *)buf = tsc_swap_16( (short)data);
	    }
	    else 
	    {
	      *(short *)buf = (short)data;
	    }
	  }
          if( ds == RDWR_SIZE_INT)   
	  {
	    if( cp->m.swap & 0x80)
	    {
	      *(int *)buf = tsc_swap_32( data);
	    }
	    else 
	    {
	      *(int *)buf = data;
	    }
	  }
          if( (cp->m.space & RDWR_SPACE_MASK) == RDWR_SPACE_KBUF)
          {
            if( !cp->kb_p)
            {
              printf("kernel buffer not available\n");
              return( RDWR_ERR);
            }
	    retval =  tsc_kbuf_write( cp->kb_p->k_base + offset, buf, (uint)ds);
	  }
	  else  
          {
	    retval =  tsc_write_sgl( offset, buf, cp->mode);
          }
          if( retval < 0)
          {
            printf("cannot access TSC register %lx -> error %d\n", offset, retval);
            return( RDWR_ERR);
          }
	  offset += (ulong)ds;
	}
        break;
      }
    }
    cp->addr = offset;
  }
  return( RDWR_OK);

tsc_rdwr_px_error:
  tsc_print_usage( c);
  return( RDWR_ERR);
}

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * Function name : tsc_rdwr_cr
 * Prototype     : int
 * Parameters    : pointer to command parameter list
 * Return        : rdwr_ok  if command executed
 *                 rdwr_err if error
 *----------------------------------------------------------------------------
 * Description   : compare tsc csr registers with expected content
 *
 *++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

int 
tsc_rdwr_cr( struct cli_cmd_para *c)
{
  int offset, cmp, mask;
  int data;
  int retval;

  if( c->cnt < 3)
  {
    printf("Not enough arguments -> usage:\n");
    tsc_print_usage( c);
    return( RDWR_ERR);
  }
  if( sscanf( c->para[0],"%x", &offset) != 1)
  {
    printf("Bad offset argument [%s] -> usage:\n", c->para[0]);
    tsc_print_usage( c);
    return( RDWR_ERR);
  }
  if( sscanf( c->para[1],"%x", &cmp) != 1)
  {
    printf("Bad data compare argument [%s] -> usage:\n", c->para[1]);
    tsc_print_usage( c);
    return( RDWR_ERR);
  }
  if( sscanf( c->para[2],"%x", &mask) != 1)
  {
    printf("Bad mask argument [%s] -> usage:\n", c->para[2]);
    tsc_print_usage( c);
    return( RDWR_ERR);
  }

  script_exit = 0;
  if( c->cmd[1] == 'i')
  {
    retval = tsc_pon_read( offset, &data);
  }
  else 
  {
    retval = tsc_csr_read( offset, &data);
  }
  if( retval < 0)
  {
    printf("Cannot access register %x\n", offset);
    script_exit = 1;
    return(RDWR_ERR);
  }
  printf("%04x :  %08x - %08x [%08x]", offset, cmp, data, mask);
  if( (data & mask) != (cmp & mask)) script_exit = 1;
  if( script_exit) printf(" -> ERROR !!!\n");
  else  printf("\n");

  return( RDWR_OK);
}

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * Function name : tsc_rdwr_cx
 * Prototype     : int
 * Parameters    : pointer to command parameter list
 * Return        : rdwr_ok  if command executed
 *                 rdwr_Err if error
 *----------------------------------------------------------------------------
 * Description   : compare tsc csr registers with expected content
 *
 *++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

int 
tsc_rdwr_cx( struct cli_cmd_para *c)
{
  int addr, cmp, mask;
  int data, ds;
  char buf[8];
  int retval;
  struct rdwr_cycle_para *cp;


  if( c->cnt < 3)
  {
    printf("Not enough arguments -> usage:\n");
    tsc_print_usage( c);
    return( RDWR_ERR);
  }
  if( sscanf( c->para[0],"%x", &addr) != 1)
  {
    printf("Bad addr argument [%s] -> usage:\n", c->para[0]);
    tsc_print_usage( c);
    return( RDWR_ERR);
  }
  if( sscanf( c->para[1],"%x", &cmp) != 1)
  {
    printf("Bad data compare argument [%s] -> usage:\n", c->para[1]);
    tsc_print_usage( c);
    return( RDWR_ERR);
  }
  if( sscanf( c->para[2],"%x", &mask) != 1)
  {
    printf("Bad mask argument [%s] -> usage:\n", c->para[2]);
    tsc_print_usage( c);
    return( RDWR_ERR);
  }
  cp = rdwr_get_cycle_space( c->cmd);
  if( !cp)
  {
    printf("Bad space argument [%s] -> usage:\n", c->para[0]);
    tsc_print_usage( c);
    return( RDWR_ERR);
  }
  rdwr_get_cycle_ds( c->ext, cp);
  ds = RDWR_MODE_GET_DS(cp->m.ads);
  rdwr_get_cycle_swap( c->ext, cp);
  script_exit = 0;
  retval =  tsc_read_sgl( (ulong)addr, buf, cp->mode);
  if( retval < 0)
  {
    printf("Cannot access address %x\n", addr);
    script_exit = 1;
    return(RDWR_ERR);
  }
  if( ds == 1)
  {
    data = (int)buf[0];
    printf("%08x :  %02x - %02x [%02x]", addr, (uchar)cmp,  (uchar)data,  (uchar)mask);
    if( (char)(data & mask) != (char)(cmp & mask)) script_exit = 1;
  }
  if( ds == 2)
  {
    data = (int)(*(short *)buf);
    printf("%08x :  %04x - %04x [%04x]", addr, (ushort)cmp,  (ushort)data,  (ushort)mask);
    if( (short)(data & mask) != (short)(cmp & mask)) script_exit = 1;
  }
  if( ds == 4)
  {
    data = *(int *)buf;
    printf("%08x :  %08x - %08x [%08x]", addr, cmp, data, mask);
    if( (data & mask) != (cmp & mask)) script_exit = 1;
  }
  if( (data & mask) != (cmp & mask)) script_exit = 1;
  if( script_exit) printf(" -> ERROR !!!\n");
  else  printf("\n");

  return( RDWR_OK);
}

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * Function name : tsc_rdwr_cmp
 * Prototype     : int
 * Parameters    : pointer to command parameter list
 * Return        : rdwr_ok  if command executed
 *                 rdwr_err if error
 *----------------------------------------------------------------------------
 * Description   : compare compare two data buffers
 *
 *++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

int
tsc_rdwr_cmp( struct cli_cmd_para *c)
{
  int off1, off2, offset;
  char sp1, sp2;
  int idx1, idx2;
  int len;
  char *buf1, *buf2;
  int mode1, mode2;
  int retval;

  retval = RDWR_ERR;

  if( c->cnt < 3)
  {
    printf("Not enough arguments -> usage:\n");
    tsc_print_usage( c);
    return( retval);
  }
  if( sscanf( c->para[0],"%x:%c%d", &off1, &sp1, &idx1) < 2)
  {
    printf("Bad offset argument [%s] -> usage:\n", c->para[0]);
    tsc_print_usage( c);
    return( retval);
  }
  if( sscanf( c->para[1],"%x:%c%d", &off2, &sp2, &idx2) < 2)
  {
    printf("Bad offset argument [%s] -> usage:\n", c->para[0]);
    tsc_print_usage( c);
    return( retval);
  }
  if( sscanf( c->para[2],"%x", &len) < 1)
  {
    printf("Bad length argument [%s] -> usage:\n", c->para[0]);
    tsc_print_usage( c);
    return( retval);
  }
  printf("Comparing data buffers..");

  mode1 = 0;
  buf1 = (char *)malloc( len);
  buf2 = NULL;
  if( !buf1)
  {
    printf("Cannot allocate memory\n");
    return( retval);
  }
  if( (sp1 == 's') || (sp1 == 'u'))
  {
    if( (idx1 < 1) || (idx1 > 2))
    {
      printf("Bad space identifier: %c%d\n", sp1, idx1);
      goto tsc_rdwr_cmp_err;
    }
    if( (sp1 == 's') && (idx1 == 1)) mode1 = RDWR_MODE_SET( 0x44, RDWR_SPACE_SHM1, 0);
    if( (sp1 == 's') && (idx1 == 2)) mode1 = RDWR_MODE_SET( 0x44, RDWR_SPACE_SHM2, 0);
    if( (sp1 == 'u') && (idx1 == 1)) mode1 = RDWR_MODE_SET( 0x44, RDWR_SPACE_USR1, 0);
    if( (sp1 == 'u') && (idx1 == 2)) mode1 = RDWR_MODE_SET( 0x44, RDWR_SPACE_USR2, 0);
    tsc_read_blk( off1, buf1, len, mode1);
  }
  else if( sp1 == 'k')
  {
    if( (idx1 < 0) || (idx1 >= TSC_NUM_KBUF))
    {
      printf("Bad kernel buffer index %d\n", idx1);
      goto tsc_rdwr_cmp_err;
    }
    if( !last_kbuf_cycle[idx1].kb_p)
    {
      printf("Kernel buffer #%d not allocated\n", idx1);
      goto tsc_rdwr_cmp_err;
    }
    tsc_kbuf_read( last_kbuf_cycle[idx1].kb_p->k_base + off1, buf1, len);
  }
  else
  {
    printf("Bad space identifier: %c%d\n");
    goto tsc_rdwr_cmp_err;
  }
  mode2 = 0;
  buf2 = (char *)malloc( len);
  if( !buf2)
  {
    printf("Cannot allocate memory\n");
    return( retval);
  }
  if( (sp2 == 's') || (sp2 == 'u'))
  {
    if( (idx2 < 1) || (idx2 > 2))
    {
      printf("Bad space identifier: %c%d\n", sp2, idx2);
      goto tsc_rdwr_cmp_err;
    }
    if( (sp2 == 's') && (idx2 == 1)) mode2 = RDWR_MODE_SET( 0x44, RDWR_SPACE_SHM1, 0);
    if( (sp2 == 's') && (idx2 == 2)) mode2 = RDWR_MODE_SET( 0x44, RDWR_SPACE_SHM2, 0);
    if( (sp2 == 'u') && (idx2 == 1)) mode2 = RDWR_MODE_SET( 0x44, RDWR_SPACE_USR1, 0);
    if( (sp2 == 'u') && (idx2 == 2)) mode2 = RDWR_MODE_SET( 0x44, RDWR_SPACE_USR2, 0);
    tsc_read_blk( off2, buf2, len, mode2);
  }
  else if( sp2 == 'k')
  {
    if( (idx2 < 0) || (idx2 >= TSC_NUM_KBUF))
    {
      printf("Bad kernel buffer index %d\n", idx2);
      goto tsc_rdwr_cmp_err;
    }
    if( !last_kbuf_cycle[idx2].kb_p)
    {
      printf("Kernel buffer #%d not allocated\n", idx2);
      goto tsc_rdwr_cmp_err;
    }
    tsc_kbuf_read( last_kbuf_cycle[idx2].kb_p->k_base + off2, buf2, len);
  }
  else
  {
    printf("Bad space identifier: %c%d\n");
    goto tsc_rdwr_cmp_err;
  }
  offset = rdwr_cmp_buf( buf1, buf2, len, -1);
  if( offset < len)
  {
    printf(" -> NOK\n");
    rdwr_tx_error( buf1, buf2, offset,  RDWR_SIZE_INT, 0);
    retval = RDWR_ERR;
  }
  else
  {
    printf(" -> OK\n");
    retval = RDWR_OK;
  }

tsc_rdwr_cmp_err:
  if( buf1) free(buf1);
  if( buf2) free(buf2);
  return( retval);
}

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * Function name : tsc_rdwr_lx
 * Prototype     : int
 * Parameters    : pointer to command parameter list
 * Return        : rdwr_ok  if command executed
 *                 rdwr_err if error
 *----------------------------------------------------------------------------
 * Description   : loop read/write on remote resource
 *
 *++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

int tsc_rdwr_lx( struct cli_cmd_para *c){
	ulong offset;
	ulong data;
	char buf[8];
	int retval;
	int ds;
	struct rdwr_cycle_para *cp;
	int rdwr;
	int cnt = c->cnt;

	if(cnt--) {
		cp = rdwr_get_cycle_space( c->cmd);
		if( !cp){
			printf("Bad space argument [%s] -> usage:\n", c->para[0]);
			tsc_print_usage( c);
			return( RDWR_ERR);
		}
		if( (cp->m.space & RDWR_SPACE_MASK) == RDWR_SPACE_KBUF){
			printf("Bad space argument [%s] -> usage:\n", c->para[0]);
			tsc_print_usage( c);
			return( RDWR_ERR);
		}
		if( !c->cnt){
			offset = cp->addr;
		}
		else {
			if( sscanf( c->para[0], "%lx", &offset) != 1){
				printf("Bad address argument [%s] -> usage:\n", c->para[0]);
				tsc_print_usage( c);
				return( RDWR_ERR);
			}
			cp->addr = (ulong)offset;
		}
		rdwr = 0;
		if( c->cnt > 1){
			if( sscanf( c->para[1], "%lx", &data) != 1){
				printf("Bad data argument [%s] -> usage:\n", c->para[1]);
				tsc_print_usage( c);
				return( RDWR_ERR);
			}
			rdwr = 1;
			printf("data = %lx\n", data);
		}
		if( c->ext){
			rdwr_get_cycle_ds( c->ext, cp);
			rdwr_get_cycle_swap( c->ext, cp);
		}
		ds = RDWR_MODE_GET_DS(cp->m.ads);
		if( aio_error( &aiocb) != EINPROGRESS){
			if( aio_read( &aiocb) < 0){
				perror("aio_read");
				return(RDWR_OK);
			}
		}
		while(1){
			if( rdwr){
				if( ds == RDWR_SIZE_BYTE)  *(char *)buf = (char)data;
				if( ds == RDWR_SIZE_SHORT){
					if( cp->m.swap & 0x80){
						*(short *)buf = tsc_swap_16( (short)data);
					}
					else {
						*(short *)buf = (short)data;
					}
				}
				if( ds == RDWR_SIZE_INT) {
					if( cp->m.swap & 0x80){
						*(int *)buf = tsc_swap_32( data);
					}
					else {
						*(int *)buf = data;
					}
				}
				if( ds == RDWR_SIZE_DBL)   {
					if( cp->m.swap & 0x80){
						*(uint64_t *)buf = tsc_swap_64( data);
					}
					else {
						*(uint64_t *)buf = data;
					}
				}
				tsc_write_loop( offset, buf, 1000, cp->mode);
			}
			else {
				tsc_read_loop( offset, buf, 1000, cp->mode);
			}
			if( aio_error( &aiocb) != EINPROGRESS){
				aio_return( &aiocb);
				return(RDWR_OK);
			}
		}
		if( aio_error( &aiocb) == EINPROGRESS){
			aio_cancel( aiocb.aio_fildes, &aiocb);
		}
	}
    printf("Not enough arguments -> usage:\n");
    tsc_print_usage(c);
    return( RDWR_ERR);
}
