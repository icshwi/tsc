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
#include "../../include/tscmasioctl.h"
#include "../../include/tscioctl.h"

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
  struct tsc_ioctl_mas_map mas_map;
  ulong rem_addr;
  uint size;
  uint offset;
  char yn[8];
  void *usr_addr;
  int cnt;

  if( argc < 3)
  {
    printf("Not enough parameters..\n");
    goto TSC_usage;
  }
  mas_map.ivec = MAS_MAP_NO_IVEC;
  ioctl_cmd = 0;
  ioctl_arg = NULL;

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
      sprintf( devname, "/dev/bus/vme/tsc_mas%d", idx);
      if( argc < 6)
      {
        printf("Not enough parameters..\n");
        goto TSC_usage;
      }
      if( sscanf( argv[3],"%lx", &rem_addr) != 1)
      {
        printf("Bad address parameter..\n");
        goto TSC_usage;
      }
      mas_map.rem_addr = rem_addr;
      if( sscanf( argv[4],"%x", &size) != 1)
      {
        printf("Bad size parameter..\n");
        goto TSC_usage;
      }
      mas_map.size = size;
      if( sscanf( argv[5],"%x", &mas_map.mode) != 1)
      {
        printf("Bad mode parameter..\n");
        goto TSC_usage;
      }
      ioctl_cmd = TSC_IOCTL_MAS_MAP_SET;
      ioctl_arg = (void *)&mas_map;
    }
    if( !strncmp( argv[2], "get", 3))
    {
      sprintf( devname, "/dev/bus/vme/tsc_mas%d", idx);
      ioctl_cmd = TSC_IOCTL_MAS_MAP_GET;
      ioctl_arg = (void *)&mas_map;
    }
  }
  else
  {
    printf("Operation not supported..\n");
    goto TSC_usage;
  }
  printf(" entering TSC master device %d ( %lx, %x, %x)\n", idx, mas_map.rem_addr, mas_map.size, mas_map.mode);
  fd = open( devname, O_RDWR);
  if( fd < 0)
  {
    printf("cannot open TSC master device %s\n", devname);
    printf( "Error -> %s\n", strerror(errno));
    exit(-1);
  }
  if( ioctl_cmd)
  {
    retval = ioctl( fd, ioctl_cmd, ioctl_arg);
    if( retval < 0)
    {
      if( ioctl_cmd == TSC_IOCTL_MAS_MAP_SET)
      {
        printf("FAILE TO SET MAPPING %s\n", devname);
      }
      printf("Error -> %s\n", strerror(errno));
    }
    if( mas_map.loc_addr)
    {
      char *p, *q;

      q = ubuf;
      offset = rem_addr-mas_map.rem_addr;
      printf("rem_addr = %lx : %lx -> offset = %x\n", mas_map.rem_addr, rem_addr, offset);
      printf("loc_addr = %lx\n", mas_map.loc_addr);
      printf("size = %x\n", mas_map.size);
      printf("mode = %x\n", mas_map.mode);
      usr_addr = mmap( NULL, size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, offset);
      if( usr_addr == MAP_FAILED)
      {
        printf("FAILED TO MMAP %s\n", devname);
      }
      else 
      {
        printf("usr_addr = %p\n", usr_addr);
        printf("usr_addr = %x\n", *(uint *)usr_addr);
        p = (char *)usr_addr;
        for( cnt = 0; cnt < 0x20; cnt+=4)
        {
	  *(int *)&q[cnt] =  *(int *)&p[cnt];
	}
        for( cnt = 0; cnt < 0x20; cnt+=4)
        {
 	  if( !(cnt&0xf)) printf("\n%06x : ", cnt);
	  printf("%08x ", *(int *)&q[cnt]);
	}
      }
      printf("\n");
      p = ubuf;
      for( cnt = 0; cnt < 0x100; cnt+=4)
      {
	*(int *)&q[cnt] = 0xdeadface;
      }
      cnt = 0;
      for( cnt = 0; cnt < 0x40; cnt+=4)
      {
 	if( !(cnt&0xf)) printf("\n%06x : ", cnt);
	printf("%08x ", *(int *)&q[cnt]);
      }
      printf("\n");
      lseek( fd, offset+1, SEEK_SET);
      p += 1;
      cnt = read( fd, p, 0x6);
      p += cnt;
      for( cnt = 0; cnt < 0x40; cnt+=4)
      {
 	if( !(cnt&0xf)) printf("\n%06x : ", cnt);
	printf("%08x ", *(int *)&q[cnt]);
      }
      printf("\n");
      cnt = read( fd, p, 0x7);
      p += cnt;
      for( cnt = 0; cnt < 0x40; cnt+=4)
      {
 	if( !(cnt&0xf)) printf("\n%06x : ", cnt);
	printf("%08x ", *(int *)&q[cnt]);
      }
      printf("\n");

#ifdef JFG
      for( cnt = 0; cnt < 0x10000; cnt++)
      {
	tbuf[cnt] = (char)cnt;
      }
      lseek( fd, offset, SEEK_SET);
      p = tbuf;
      cnt = write( fd, p, 0x40);
#endif
      if( usr_addr != MAP_FAILED)
      {
	munmap(usr_addr, 0x10000);
      }
    }
    else
    {
      printf("device %s need mapping..\n", devname);
    }
  }
  printf("continue: ");
  scanf("%s", yn);
  close( fd);
  exit(0);

TSC_usage:
  printf("usage: tscmas mapX set <rem_addr> <size> <mode>\n");

  exit(-1);
}
