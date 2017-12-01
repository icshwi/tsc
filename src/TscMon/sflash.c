/*=========================< begin file & file header >=======================
 *  References
 *  
 *    filename : sflash.c
 *    author   : JFG, XP
 *    company  : IOxOS
 *    creation : Sept 15,2015
 *
 *----------------------------------------------------------------------------
 *  Description
 *
 *    That file contains a set of function called by TscMon to handle the
 *    TSC SFLASH interface.
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
static char *rcsid = "$Id: sflash.c,v 1.2 2016/03/02 09:44:17 ioxos Exp $";
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

char *
sflash_rcsid()
{
  return( rcsid);
}

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * Function name : tsc_sflash_dump
 * Prototype     : int
 * Parameters    : pointer to command parameter list
 * Return        : TSC_OK  if command executed
 *                 TSC_ERR if error
 *----------------------------------------------------------------------------
 * Description   : perform SFLASH operation
 *
 *++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
int tsc_sflash_dump( struct cli_cmd_para *c){
  char *p, *buf_des;
  int i, j;
  int offset, size;

  if( c->cnt < 2){
    printf("missing parameters -> usage:\n");
    goto tsc_sflash_dump_usage;
  }
  if( sscanf( c->para[1],"%x", &offset) != 1){
    printf("Bad offset argument [%s] -> usage:\n", c->para[1]);
    goto tsc_sflash_dump_usage;
  }
  size = 0x100;
  if( c->cnt > 2){
    if( sscanf( c->para[2],"%x", &size) != 1){
      printf("Bad size argument [%s] -> usage:\n", c->para[2]);
      goto tsc_sflash_dump_usage;
    }
  }
      
  printf("Dumping SFLASH %d from offset %x [size %x] ...", offset >> 28, offset & 0xfffffff, size);
  buf_des = malloc( size);
  tsc_sflash_read( offset, buf_des, size);

  printf(" -> done\n");

  p = (char *)buf_des;
  for( j = 0; j < size; j += 16){
    char *pp;

    printf("%08x ", offset + j);
    pp = p;
    for( i = 0; i < 16; i++){
      printf("%02x ", *(unsigned char *)p++);
    }
    for( i = 0; i < 16; i++){
      char c;
      c = *pp++;
      if(isalpha(c)){
	    printf("%c", c);
      }
      else{
        printf(".");
      }
    }
    printf("\n");
  }
  printf("\n");

  free( buf_des);
  return(0);

tsc_sflash_dump_usage:
  printf("sflash dump <offset> <size>\n");
  return( -1);
}

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * Function name : tsc_sflash_read
 * Prototype     : int
 * Parameters    : pointer to command parameter list
 * Return        : TSC_OK  if command executed
 *                 TSC_ERR if error
 *----------------------------------------------------------------------------
 * Description   : perform SFLASH operation
 *
 *++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
static int sflash_read( int start, char *buf, int size, int blk){
  int n, last;
  int retval;

  if( blk > size){
    printf("%08x\b\b\b\b\b\b\b\b", start);
    fflush(stdout);
    usleep( 10000);
    retval = tsc_sflash_read( start, buf, size);
    if (retval < 0)
    {
      return( retval);
    }
    start += size;
    buf += size;
  }
  else
  {
    n = size/blk;
    last = size%blk;
    while( n--){
      printf("%08x\b\b\b\b\b\b\b\b", start);
      fflush( stdout);
      usleep( 10000);
      retval = tsc_sflash_read( start, buf, blk);
      if (retval < 0){
        return( retval);
      }
      start += blk;
      buf += blk;
    }
    if(last){
      printf("%08x\b\b\b\b\b\b\b\b", start);
      fflush( stdout);
      usleep( 10000);
      retval = tsc_sflash_read( start, buf, last);
      if (retval < 0){
        return( retval);
      }
      start += last;
      buf += last;
    }
  }
  printf("%08x", start);
  return( size);
}

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * Function name : tsc_sflash_write
 * Prototype     : int
 * Parameters    : pointer to command parameter list
 * Return        : TSC_OK  if command executed
 *                 TSC_ERR if error
 *----------------------------------------------------------------------------
 * Description   : perform SFLASH write operation
 *
 *++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
static int sflash_write(uint offset, char *buf_src, uint size){
  char *p, *buf_des;
  int i, n, nblk, blk_size;
  int start, first, last;
  int retval;

  blk_size = SFLASH_BLK_SIZE;
  blk_size = SFLASH_BLK_SIZE*4;
  first = offset & (blk_size -1);
  if( first){
    first = blk_size - first;
  }
  if( size <= first){
    first = size;
    nblk = 0;
    last = 0;
  }
  else{
    size -= first;
    nblk = size/blk_size;
    last = size - (nblk * blk_size);
  }
  printf("\n");

  start = offset;
  p = buf_src;
  printf("Writing device will take about %d seconds...", 1 + (nblk*3));
  printf("%08x\b\b\b\b\b\b\b\b", start);
  fflush( stdout);
  usleep(10000);
  retval = tsc_sflash_write( start, p, first);
  if (retval < 0){
    return( retval);
  }
  p += first;
  start += first;
  n = nblk;
  while( n--){
    printf("%08x\b\b\b\b\b\b\b\b", start);
    fflush( stdout);
    usleep(10000);
    retval = tsc_sflash_write( start, p, blk_size);
    if (retval < 0){
      return( retval);
    }
    p += blk_size;
    start += blk_size;
  }

  printf("%08x\b\b\b\b\b\b\b\b", start);
  fflush( stdout);
  usleep(10000);
  retval = tsc_sflash_write( start, p, last);
  if (retval < 0){
    return( retval);
  }
  p += last;
  start += last;
  printf("%08x ", start);

  printf(" -> done\n");

  printf("Verifying device will take about %d seconds...", 1 + (nblk*4));
  fflush( stdout);
  usleep(10000);
  buf_des = malloc( size);
  if( !buf_des){
    printf(" -> cannot allocate temporary buffer\n");
    return( TSC_ERR);
  }

  retval = sflash_read( offset, buf_des, size, blk_size);
  if (retval < 0){
    printf(" -> cannot read SFLASH\n");
    free( buf_des);
    return( retval);
  }

  retval = 0;
  for( i = 0; i < size; i++){
    if( buf_src[i] != buf_des[i]){
      printf(" -> compare error at offset 0x%x\n", i);
      retval = TSC_ERR;
      break;
    }
  }
  if( !retval){
    printf(" -> OK\n");
  }
  free( buf_des);

  return( retval);
}

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * Function name : tsc_sflash_load
 * Prototype     : int
 * Parameters    : pointer to command parameter list
 * Return        : TSC_OK  if command executed
 *                 TSC_ERR if error
 *----------------------------------------------------------------------------
 * Description   : perform SFLASH load operation
 *
 *++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
int tsc_sflash_load( struct cli_cmd_para *c){
  char *buf_src;
  int size, offset;
  FILE *file;
  int retval;

  if( c->cnt < 3){
    printf("missing parameters -> usage:\n");
    goto tsc_sflash_load_usage;
  }

  if( !strncmp( c->para[1], "golden", 3)){
    offset = 0x000000;
  }
  else if( !strncmp( c->para[1], "multi", 3)){
    offset = 0x400000;
  }
  else{
    if(sscanf( c->para[1],"%x", &offset) != 1){
      printf("bad offset parameters : %s -> usage:\n",  c->para[1]);
      goto tsc_sflash_load_usage;
    }
  }
  if(offset < 0x400000){
    char yn;

    printf("!! You are going to overwrite the FPGA backup bitstream [%x]!!!\n", offset);
    printf("!! That operation can kill the TSC interface...\n");
    printf("!! Do you want to continue [y/n] n ->  ");
    scanf("%c", &yn);
    printf("\n");
    if( yn != 'y'){
      return( TSC_OK);
    }
    printf("Are you sure  [y/n] n ->  ");
    scanf("%c", &yn);
    printf("\n");
    if( yn != 'y'){
      return( TSC_OK);
    }
  }
  printf("Loading SFLASH from file %s at offset 0x%x ", c->para[2], offset);
  file = fopen( c->para[2], "r");
  if( !file){
    printf("\nFile %s doesn't exist\n", c->para[2]);
    return( TSC_ERR);
  }
  fseek( file, 0, SEEK_END);
  size = ftell( file);
  printf("[size 0x%x]\n", size);

  buf_src = malloc( size);
  if( !buf_src){
    printf("Cannot allocate data buffer of size 0x%x\n", size);
    fclose( file);
    return( TSC_ERR);
  }
  fseek( file, 0, SEEK_SET);
  fread( buf_src, 1, size, file);
  fclose( file);

  retval = sflash_write( offset, buf_src, size);
  printf("\n");
  free( buf_src);
  if( retval < 0){
    perror("Cannot access SFLASH device:");
    if( retval == -EPERM){
      printf("Write protected by hardware !!");
    }
    return( TSC_ERR);
  }
  return( TSC_OK);

tsc_sflash_load_usage:
  printf("sflash load <offset> <file>\n");
  return( TSC_ERR);
}

struct sflash_dynopt{
  uint   magic;
  uint   statsw;
  ushort did;
  ushort vid;
  ushort sdid;
  ushort svid;
  uint   dsn_l;
  uint   dsn_h;
  uint   bcnt;
  uint   rsv;
};
struct cli_cmd_history sflash_history;

#define SFLASH_DYNOPT_OFF      0xff0000
#define SFLASH_DYNOPT_MAGIC  0x12345678

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * Function name : sflash_set_para_hex
 * Prototype     : int
 * Parameters    : command prompt and data
 * Return        : TSC_OK  if command executed
 *                 TSC_ERR if error
 *----------------------------------------------------------------------------
 * Description   : set hexadecimal parameter
 *
 *++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
static int sflash_set_para_hex( char *prompt, uint *data){
  char *para, *p;

  para = cli_get_cmd( &sflash_history, prompt);
  para = strtok(para,"\n\r");
  if(para){
    if( ( para[0] == '.') || ( para[0] == 'q')){
      return(TSC_QUIT);
    }
    *data = strtoul( para, &p, 16);
    if( p == para){
      printf("%s : Bad parameter value\n", para);
      return(TSC_ERR);
    }
  }
  return( TSC_OK);
}

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * Function name : sflash_set_para_yn
 * Prototype     : int
 * Parameters    : command prompt and data
 * Return        : TSC_OK  if command executed
 *                 TSC_ERR if error
 *----------------------------------------------------------------------------
 * Description   : set character parameter
 *
 *++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
static int sflash_set_para_yn(char *prompt, char *data){
  char *para;

  para = cli_get_cmd( &sflash_history, prompt);
  para = strtok(para,"\n\r");
  if( para){
    if( ( para[0] == '.') || ( para[0] == 'q')){
      return(TSC_QUIT);
    }
    if( (para[0] == 'y') || (para[0] == 'Y') || (para[0] == '1')){
      *data = 1;
    }
    if( (para[0] == 'n') || (para[0] == 'N') || (para[0] == '0')){
      *data = 0;
    }
    return( TSC_OK);
  }
  return( TSC_OK);
}

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * Function name : sflash_bit_cnt
 * Prototype     : int
 * Parameters    : buffer and size to count bit
 * Return        : TSC_OK  if command executed
 *                 TSC_ERR if error
 *----------------------------------------------------------------------------
 * Description   : bit count function
 *
 *++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
static int sflash_bit_cnt( char *buf, int size){
  int i, j;
  int bcnt;

  bcnt = 0;
  for( i = 0; i < size; i++){
    for( j = 0; j < 8; j++){
      if( buf[i] & (1<<j)){
	bcnt++;
      }
    }
  }
  return( bcnt);
}

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * Function name : tsc_sflash_dynopt
 * Prototype     : int
 * Parameters    : pointer to command parameter list
 * Return        : TSC_OK  if command executed
 *                 TSC_ERR if error
 *----------------------------------------------------------------------------
 * Description   : perform SFLASH operation
 *
 *++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
int tsc_sflash_dynopt(struct cli_cmd_para *c){
  struct sflash_dynopt dynopt;
  char prompt[80];   
  uint tmp, statsw;
  char yn;
  int retval;

  /* fill dynopt from SFLASH */
  retval = tsc_sflash_read( SFLASH_DYNOPT_OFF, (char *)&dynopt, sizeof(dynopt));
  if( retval < 0){
    perror("Cannot access SFLASH !!\n");
    return( TSC_ERR);
  }

  /* push statsw in data structure*/
  dynopt.magic = htole32(0x12345678);
//dynopt.magic = htole32(0xffffffff); //XP Erase Memory area

  /* get statsw from data structure*/
  statsw = le32toh(dynopt.statsw);
  tmp = statsw & 0x3;
  sprintf(prompt, "SYSCONMOD     [0x%01x] -> ", tmp);
  if(sflash_set_para_hex(prompt, &tmp) < 0){
    return( TSC_ERR);
  }
  statsw = (statsw & ~0x3) | (tmp & 0x3);

  yn =  (statsw&0x4)?1:0;
  sprintf(prompt, "SYSRST_ENA          [%c] -> ", (statsw&0x4)?'y':'n');
  if(sflash_set_para_yn(prompt, &yn) < 0){
    return( TSC_ERR);
  }
  statsw = (statsw & ~0x4) | (yn?0x4:0);

  yn =  (statsw&0x8)?1:0;
  sprintf(prompt, "AUTOID_ENA          [%c] -> ", (statsw&0x8)?'y':'n');
  if(sflash_set_para_yn(prompt, &yn) < 0){
    return( TSC_ERR);
  }
  statsw = (statsw & ~0x8) | (yn?0x8:0);

  yn =  (statsw&0x10)?1:0;
  sprintf(prompt, "PONFSM_DIS          [%c] -> ", (statsw&0x10)?'y':'n');
  if(sflash_set_para_yn(prompt, &yn) < 0){
    return( TSC_ERR);
  }
  statsw = (statsw & ~0x10) | (yn?0x10:0);

  yn =  (statsw&0x20)?1:0;
  sprintf(prompt, "PERST_ENA        [%c] -> ", (statsw&0x20)?'y':'n');
  if(sflash_set_para_yn(prompt, &yn) < 0){
    return( TSC_ERR);
  }
  statsw = (statsw & ~0x20) | (yn?0x20:0);

  /* clear SPI_WRPROT */
  statsw = (statsw & ~0x40);

  /* clear SPI_CFG_ENA */
  statsw = (statsw & ~0x80) ;
 
  yn =  (statsw&0x100)?1:0;
  sprintf(prompt, "PCIEP_MEMPF_A64     [%c] -> ", (statsw&0x100)?'y':'n');
  if(sflash_set_para_yn(prompt, &yn) < 0){
    return( TSC_ERR);
  }
  statsw = (statsw & ~0x100) | (yn?0x100:0);

  tmp =  (statsw & 0xe00) >> 9;
  sprintf(prompt, "PCIEP_MEMPF_SIZ   [0x%01x] -> ", tmp);
  if(sflash_set_para_hex(prompt, &tmp) < 0){
    return( TSC_ERR);
  }
  statsw = (statsw & ~0xe00) | ((tmp & 0x7) << 9);

  tmp = (statsw & 0x7000) >> 12;
  sprintf(prompt, "PCIEP_MEMNoPF_SIZ [0x%01x] -> ", tmp);
  if(sflash_set_para_hex(prompt, &tmp) < 0){
    return( TSC_ERR);
  }
  statsw = (statsw & ~0x7000) | ((tmp & 0x7) << 12);

  yn =  (statsw&0x8000)?1:0;
  sprintf(prompt, "PCIEP_IO4K          [%c] -> ", (statsw&0x8000)?'y':'n');
  if(sflash_set_para_yn(prompt, &yn) < 0){
    return( TSC_ERR);
  }
  statsw = (statsw & ~0x8000) | (yn?0x8000:0);

  tmp = (statsw & 0xff0000) >> 16;
  sprintf(prompt, "CRCSR_A24_MAP     [0x%02x] -> ", tmp);
  if(sflash_set_para_hex(prompt, &tmp) < 0){
    return( TSC_ERR);
  }
  statsw = (statsw & ~0xff0000) | ((tmp & 0xff) << 16);

  /* IOxOS reserved -> set to 0*/
  statsw = (statsw & ~0xff000000);
//statsw = 0xffffffff; //XP Erase Memory area

  /* push statsw in data structure*/
  dynopt.statsw = htole32(statsw);

  /* get PCIE DID from data structure*/
  tmp = (uint)le16toh(dynopt.did);
  sprintf(prompt, "PCIE_DID         [0x%04x] -> ", (ushort)tmp);
  if(sflash_set_para_hex(prompt, &tmp) < 0){
    return( TSC_ERR);
  }
  dynopt.did = htole16((ushort)tmp);

  /* get PCIE VID from data structure*/
  tmp = (uint)le16toh(dynopt.vid);
  sprintf(prompt, "PCIE_VID         [0x%04x] -> ", (ushort)tmp);
  if(sflash_set_para_hex(prompt, &tmp) < 0){
    return( TSC_ERR);
  }
  dynopt.vid = htole16((ushort)tmp);

  /* get PCIE DID from data structure*/
  tmp = (uint)le16toh(dynopt.sdid);
  sprintf(prompt, "PCIE_SDID         [0x%04x] -> ", (ushort)tmp);
  if(sflash_set_para_hex(prompt, &tmp) < 0){
    return( TSC_ERR);
  }
  dynopt.sdid = htole16((ushort)tmp);

  /* get PCIE VID from data structure*/
  tmp = (uint)le16toh(dynopt.svid);
  sprintf(prompt, "PCIE_SVID         [0x%04x] -> ", (ushort)tmp);
  if(sflash_set_para_hex(prompt, &tmp) < 0){
    return( TSC_ERR);
  }
  dynopt.svid = htole16((ushort)tmp);

  /* get PCIE DSN_L from data structure*/
  tmp = (uint)le32toh(dynopt.dsn_l);
  sprintf(prompt, "PCIE_EP_DSN_L     [0x%08x] -> ", tmp);
  if(sflash_set_para_hex(prompt, &tmp) < 0){
    return( TSC_ERR);
  }
  dynopt.dsn_l = htole32(tmp);

  /* get PCIE DSN_H from data structure*/
  tmp = (uint)le32toh(dynopt.dsn_h);
  sprintf(prompt, "PCIE_EP_DSN_H     [0x%08x] -> ", tmp);
  if(sflash_set_para_hex(prompt, &tmp) < 0){
    return( TSC_ERR);
  }
  dynopt.dsn_h = htole32(tmp);

  dynopt.bcnt = 0;
  dynopt.rsv = 0;
//dynopt.rsv = 0xffffffff; //XP Erase Memory area
  tmp = sflash_bit_cnt( (char *)&dynopt, sizeof(dynopt));
//tmp = 0xffffffff; //XP Erase Memory area
  dynopt.bcnt = htole32(tmp);

  /* load dynamic options in SFLASH */
  printf("Loading dynamic options in SFLASH [%08x]...", dynopt.bcnt);
  fflush( stdout);
  retval = tsc_sflash_write( SFLASH_DYNOPT_OFF, (char *)&dynopt, sizeof(dynopt));
  if( retval < 0){
    perror("Cannot overwrite dynamic options in SFLASH !!\n");
    return( TSC_ERR);
  }
  printf(" -> DONE\n");

  return( TSC_OK);
}

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * Function name : tsc_sflash
 * Prototype     : int
 * Parameters    : pointer to command parameter list
 * Return        : TSC_OK  if command executed
 *                 TSC_ERR if error
 *----------------------------------------------------------------------------
 * Description   : perform SFLASH operation
 *
 *++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
int tsc_sflash( struct cli_cmd_para *c){
  if(!strcmp( "rdid", c->para[0])){
    char id[4];

    tsc_sflash_rdid(id);
    printf("SFLASH IDENTIFIER = %02x.%02x.%02x\n", id[0], id[1], id[2]);
    return(TSC_OK);
  }
  else if( !strcmp( "rdsr", c->para[0])){
    short sr;

    tsc_sflash_rdsr( (char *)&sr);
    printf("SFLASH SR = %04x\n", sr);
    return(TSC_OK);
  }
  else if(!strcmp("wrsr", c->para[0])) {
    short sr;
    int tmp;

    if( sscanf( c->para[1],"%x", &tmp) != 1){
      printf("bad SR value : %s -> usage:\n",  c->para[1]);
      printf("sflash wrsr <sr>\n");
      return(TSC_ERR);
    }
    sr = (short)tmp;
    tsc_sflash_wrsr( (char *)&sr);
    tsc_sflash_rdsr( (char *)&sr);
    printf("SFLASH SR = %04x\n", sr);
    return(TSC_OK);
  }
  else if(!strcmp( "load", c->para[0])){
    return( tsc_sflash_load( c));
  }
  else if(!strcmp( "dump", c->para[0])){
    return( tsc_sflash_dump( c));
  }
  else if(!strcmp( "dynopt", c->para[0])){
    return(tsc_sflash_dynopt( c));
  }
  else{
    return(TSC_ERR);
  }
}
