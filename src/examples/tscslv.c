#include <stdlib.h>
#include <stdio.h>
#include <pty.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <sys/mman.h>
#include <errno.h>
#include "../../include/tscioctl.h"
#include "../../include/tscslvioctl.h"

char ubuf[0x10000];
char tbuf[0x10000];

int
main( int argc, char *argv[])
{
  int fd;
  int retval;
  int ioctl_cmd;
  void *ioctl_arg;
  int idx;
  char devname[32];
  struct tsc_ioctl_map_req slv_map, *r;
  char yn[8];
  void *usr_addr;
  int cnt;
  char space, sg_id;

  if( argc < 3)
  {
    printf("Not enough parameters..\n");
    goto TSC_usage;
  }
  ioctl_cmd = 0;
  ioctl_arg = NULL;

  bzero( &slv_map, sizeof( slv_map));
  r = (struct tsc_ioctl_map_req *)&slv_map;
  sg_id = MAP_ID_INVALID;


  if( !strncmp( argv[1], "map", 3))
  {
    idx = (int)( argv[1][3] - '0');
    if( (idx < 0) || (idx > 15))
    {
      printf("Device index not valid..[%d]\n", idx);
      goto TSC_usage;
    }
    if( !strncmp( argv[2], "set", 3))
    {
      sprintf( devname, "/dev/bus/vme/tsc_slv%d", idx);
      if( argc < 6)
      {
        printf("Not enough parameters..\n");
        goto TSC_usage;
      }
      if( sscanf( argv[3],"%lx.%c", &r->rem_addr, &space) != 2)
      {
        printf("Bad address parameter..\n");
        goto TSC_usage;
      }
      if( sscanf( argv[4],"%x", &r->size) != 1)
      {
        printf("Bad size parameter..\n");
        goto TSC_usage;
      }
      if( !strncmp( argv[5], "A16", 3)) sg_id = MAP_ID_SLV_VME_A16;
      if( !strncmp( argv[5], "A24", 3)) sg_id = MAP_ID_SLV_VME_A24;
      if( !strncmp( argv[5], "A32", 3)) sg_id = MAP_ID_SLV_VME_A32;
      if( !strncmp( argv[5], "BRC", 3)) sg_id = MAP_ID_SLV_VME_BRC;
      if( sg_id == MAP_ID_INVALID)
      {
	printf("Bad SG identifier..\n");
	goto TSC_usage;
      }
      r->loc_addr = MAP_LOC_ADDR_AUTO;
      if( argc > 6)
      {
        if( sscanf( argv[6],"%lx", &r->loc_addr) != 1)
        {
	  printf("Bad local address parameter..\n");
	  goto TSC_usage;
	}
        r->mode.flags = MAP_FLAG_FORCE;
      }
      ioctl_cmd = TSC_IOCTL_SLV_MAP_SET;
      ioctl_arg = (void *)&slv_map;
    }
    else if( !strncmp( argv[2], "get", 3))
    {
      sprintf( devname, "/dev/bus/vme/tsc_slv%d", idx);
      ioctl_cmd = TSC_IOCTL_SLV_MAP_GET;
      ioctl_arg = (void *)&slv_map;
    }
    else 
    {
      printf("command not valid..[%s]\n", argv[2]);
      goto TSC_usage;
    }
  }
  else
  {
    printf("Operation not supported..\n");
    goto TSC_usage;
  }
  r->mode.sg_id = sg_id;
  if( space == 'p') r->mode.space = MAP_SPACE_PCIE;
  else if( space == 's') r->mode.space = MAP_SPACE_SHM;
  else if( space == 'u') r->mode.space = MAP_SPACE_USR;
  else if( space == 'k')
  {
    r->mode.space = MAP_SPACE_PCIE;
    r->rem_addr = MAP_REM_ADDR_AUTO;
  }
  else 
  {
    printf("Bad remote space [%c]...\n", space);
    goto TSC_usage;
  }
  printf(" entering TSC slave device %d ( %lx, %x, %x)\n", idx, r->rem_addr, r->size, *(int *)&r->mode);
  fd = open( devname, O_RDWR);
  if( fd < 0)
  {
    printf("cannot open TSC slave device %s\n", devname);
    printf( "Error -> %s\n", strerror(errno));
    exit(-1);
  }
  if( ioctl_cmd)
  {
    retval = ioctl( fd, ioctl_cmd, ioctl_arg);
    if( retval < 0)
    {
      if( ioctl_cmd == TSC_IOCTL_SLV_MAP_SET)
      {
        printf("FAIL TO SET MAPPING %s\n", devname);
      }
      printf("Error -> %s\n", strerror(errno));
    }
  }
  printf("continue: ");
  scanf("%s", yn);
  if( space == 'k')
  {
    char *p, *q;
      usr_addr = mmap( NULL, 0x40, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
      if( usr_addr == MAP_FAILED)
      {
        printf("FAILED TO MMAP %s\n", devname);
      }
      else 
      {
        printf("usr_addr = %p\n", usr_addr);
        printf("usr_addr = %x\n", *(uint *)usr_addr);
        p = (char *)usr_addr;
        q = (char *)ubuf;
        for( cnt = 0; cnt < 0x40; cnt+=4)
        {
	  *(int *)(&q[cnt]) =  *(int *)&p[cnt];
	}
        for( cnt = 0; cnt < 0x40; cnt+=4)
        {
 	  if( !(cnt&0xf)) printf("\n%06x : ", cnt);
	  printf("%08x ", *(int *)&q[cnt]);
	}
      }
      printf("\n");
  printf("continue: ");
  scanf("%s", yn);

    printf("\n");
    for( cnt = 0; cnt < 0x40; cnt+=4)
    {
      *(int *)&q[cnt] = 0xdeadface;
    }
    lseek( fd, 0, SEEK_SET);
    cnt = write( fd, ubuf, 0x40);
    printf("\n");

    printf("continue: ");
    scanf("%s", yn);
    lseek( fd, 0, SEEK_SET);
    cnt = read( fd, ubuf, 0x40);
    for( cnt = 0; cnt < 0x40; cnt+=4)
    {
      if( !(cnt&0xf)) printf("\n%06x : ", cnt);
      printf("%08x ", *(int *)&q[cnt]);
    }
    printf("\n");
  } 
  close( fd);
  exit(0);

TSC_usage:
  printf("usage: tscslv map<X> <cmd> <rem_addr>.<space> <size> <sg_id>\n");

  exit(-1);
}
