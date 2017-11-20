#include <stdlib.h>
#include <stdio.h>
#include <pty.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <sys/mman.h>
#include "../../include/tscioctl.h"

main( int argc, char *argv[])
{
  int fd;
  int retval;
  int cmd;
  struct tsc_ioctl_csr_op csr_op;

  if( argc < 3)
  {
    printf("Not enough parameters..\n");
    goto TSC_usage;
  }
  if( !strcmp( argv[1], "rd"))
  {
    cmd = TSC_IOCTL_CSR_RD;
  }
  else
  {
    if( !strcmp( argv[1], "wr"))
    {
      cmd = TSC_IOCTL_CSR_WR;
    }
    else if( !strcmp( argv[1], "or"))
    {
      cmd = TSC_IOCTL_CSR_OR;
    }
    else if( !strcmp( argv[1], "xor"))
    {
      cmd = TSC_IOCTL_CSR_XOR;
    }
    else if( !strcmp( argv[1], "and"))
    {
      cmd = TSC_IOCTL_CSR_AND;
    }
    else
    {
      printf("Operation not supported..\n");
      goto TSC_usage;
    }
    if( argc < 4)
    {
      printf("Not enough parameters..\n");
      goto TSC_usage;
    }
    if( sscanf( argv[3],"%x", &csr_op.data) != 1)
    {
      printf("Bad data parameter..\n");
      goto TSC_usage;
    }
  }
  if( sscanf( argv[2],"%x", &csr_op.offset) != 1)
  {
    printf("Bad offset parameter..\n");
    goto TSC_usage;
  }
  printf(" entering TSC test...\n");
  fd = open("/dev/bus/bridge/tsc_ctl", O_RDWR);
  if( fd < 0)
  {
    printf("cannot open TSC control device\n");
    exit(-1);
  }
  retval = ioctl( fd, cmd, &csr_op);
  if( retval < 0)
  {
    printf("Error accessing register %x\n", csr_op.offset);
  }
  else
  {
    printf("reg %x -> %08x\n", csr_op.offset, csr_op.data);
  }
  close( fd);
  exit(0);

TSC_usage:
  printf("usage: TSC <op> <off> [<data>]\n");

  exit(-1);
}
