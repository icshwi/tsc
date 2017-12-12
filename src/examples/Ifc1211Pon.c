/*=========================< begin file & file header >=======================
 *  References
 *
 *    filename : ifc1211Pon.c
 *    author   : JFG, XP
 *    company  : IOxOS
 *    creation : june 30,2008
 *
 *----------------------------------------------------------------------------
 *  Description
 *
 *    Access pon driver.
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

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * Function name : main
 * Prototype     : int
 * Parameters    : arc, argv
 * Return        : 0
 *----------------------------------------------------------------------------
 * Description   : test pon driver
 *
 *++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

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
