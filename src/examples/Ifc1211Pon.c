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

char buf[0x100];
int
main( int argc, char *argv[])
{
  int fd;
  int data;
  int retval;
  off_t offset;

  printf("Hello from %s\n", argv[0]);

  fd = open("/dev/mem", O_RDWR);
  if( fd < 0)
  {
    perror("Cannot open /dev/mem ");
    exit(-1);
  }

  data = 0x12345678;
  offset = (off_t)0;
  retval = pread64( fd, (void *)&buf, 4, offset);
  if( retval < 0)
  {
    perror("Cannot read /dev/mem");
  }
  data = *(int *)buf;
  printf("data = %x\n", data);
  close( fd);


  exit(0);
}
