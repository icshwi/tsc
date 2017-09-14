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
  fd_set rfds;
  int retval;
  struct tsc_ioctl_mas_map mas_map;
  void *usr_addr;
  char yn[8];
  struct timeval tv;
  int ivec;

  ivec = 0xaa;
  fd = open( "/dev/bus/vme/tsc_mas0", O_RDWR);
  if( fd < 0)
  {
    printf("cannot open IFC1211 master device %s\n", "/dev/bus/vme/tsc_mas0");
    printf( "Error -> %s\n", strerror(errno));
    exit(-1);
  }
  mas_map.rem_addr = 0;
  mas_map.size = 0x400000;
  mas_map.mode = VME_AM_A32 | VME_AM_DATA;
  mas_map.ivec = ivec;
  retval = ioctl( fd, TSC_IOCTL_MAS_MAP_SET, (void *)&mas_map);
  if( retval < 0)
  {
    printf("Error -> %s\n", strerror(errno));
  }
  if( mas_map.loc_addr)
  {
    usr_addr = mmap( NULL, mas_map.size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if( usr_addr == MAP_FAILED)
    {
      printf("FAILED TO MMAP %s\n", "/dev/bus/vme/tsc_mas0");
    }
    else
    {
      printf("usr_addr = %p - %x\n", usr_addr, *(uint *)usr_addr);
    }

    /* ================>
     *
     */

    printf("continue: ");
    scanf("%s", yn);

    FD_ZERO(&rfds);
    FD_SET( fd, &rfds);

    tv.tv_sec = 5;
    tv.tv_usec = 0;

    printf("waiting for VME IRQ...\n");
    do
    {
      retval = select( fd+1, &rfds, NULL, NULL, &tv);
    } while( FD_ISSET( fd, &rfds));

    printf("continue: ");
    scanf("%s", yn);

    if( usr_addr != MAP_FAILED)
    {
      munmap(usr_addr, mas_map.size);
    }
  }
  close( fd);
  exit(0);

}
