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

#define SHM_OFFSET   0x1c0000		/* SHM offset       */
#define SHM_SIZE     0x80000		/* SHM window size  */
#define DEV_NAME     "/dev/bus/vme/tsc_slv2"

char *devname = DEV_NAME;

int
main( int argc, char *argv[])
{
  int fd;
  struct tsc_ioctl_map_req slv_req;
  struct tsc_ioctl_map_req slv_sts;
  int retval;
  ulong shm_offset;
  int shm_size;
  ulong vme_addr;

  fd = open( devname, O_RDWR);
  if( fd < 0)
  {
    printf("cannot open TSC slave device %s\n", devname);
    printf( "Error -> %s\n", strerror(errno));
    exit(-1);
  }

  /* open slave in VME A32 to access SHM at offset 0x1c0000 device */
  shm_offset =  SHM_OFFSET;
  shm_size =  SHM_SIZE;
  slv_req.rem_addr = shm_offset;
  slv_req.loc_addr = MAP_LOC_ADDR_AUTO;
  slv_req.size = shm_size;
  slv_req.mode.space = MAP_SPACE_SHM;
  slv_req.mode.sg_id = MAP_ID_SLV_VME_A32;
  slv_req.mode.flags = 0;

  retval = ioctl( fd, TSC_IOCTL_SLV_MAP_SET, &slv_req);
  if( retval < 0)
  {
    if( retval == -EBUSY)
    {
      /* device already mapped */ 
      printf("FAIL TO SET MAPPING %s\n", devname);
    }
    printf("Error -> %s\n", strerror(errno));
  
  }
  /* calulate actual VME adress to access SHM_ADDR */
  retval = ioctl( fd, TSC_IOCTL_SLV_MAP_GET, &slv_sts);
  if( retval < 0)
  {
    printf("Error -> %s\n", strerror(errno));  
  }
  vme_addr = slv_sts.loc_addr + shm_offset - slv_sts.rem_addr;

  printf("vme_addr = %lx\n", vme_addr);

  if(1)
  {
    char yn[8];
    printf("continue: ");
    scanf("%s", yn);
  }
  close( fd);

  return(0);

}
