/*=========================< begin file & file header >=======================
 *  References
 *  
 *    filename : tsculib.c
 *    author   : JFG, XP
 *    company  : IOxOS
 *    creation : october 18,2015
 *
 *----------------------------------------------------------------------------
 *  Description
 *
 *    That library contains a set of function to access the TSC driver.
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
#include <errno.h>
#include <sys/mman.h>
#include "../include/mtca4rtmlib.h"
#include "../include/ponmboxlib.h"

#include "../include/tscioctl.h"
#include "../include/i2c-dev.h"

char tsc_lib_version[] = "1.30";
int tsc_fd         = -1;
int tsc_fd_io      = -1;
int tsc_fd_central = -1;
static char tsc_drv_name[16] = {0,};
static char tsc_drv_version[16] = {0,};
static unsigned short tsc_vendor_id;
static unsigned short tsc_device_id;

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * Function name : CheckByteOrder
 * Prototype     : int
 * Parameters    : void
 * Return        : endianness
 *----------------------------------------------------------------------------
 * Description   : Check byte order
 *
 *++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

int CheckByteOrder(void){
	unsigned int i = 1;
	char *c = (char*)&i;
	if (*c){
		// Little endian x86
		return 0;
	}
	else{
		// Big endian ppc
		return 1;
	}
}

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * Function name : tsc_swap_64
 * Prototype     : uint64_t
 * Parameters    : uint64_t data
 * Return        : data swapped
 *----------------------------------------------------------------------------
 * Description   : return data swapped for 64 bits
 *
 *++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

uint64_t
tsc_swap_64( uint64_t data)
{
  char ci[8];
  char co[8];

  *(uint64_t *)ci = data;
  co[0] = ci[7];
  co[1] = ci[6];
  co[2] = ci[5];
  co[3] = ci[4];
  co[4] = ci[3];
  co[5] = ci[2];
  co[6] = ci[1];
  co[7] = ci[0];

  return( *(uint64_t *)co);
}

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * Function name : tsc_swap_32
 * Prototype     : int
 * Parameters    : int data
 * Return        : data swapped
 *----------------------------------------------------------------------------
 * Description   : return data swapped for 32 bits
 *
 *++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

int
tsc_swap_32( int data)
{
  char ci[4];
  char co[4];

  *(int *)ci = data;
  co[0] = ci[3];
  co[1] = ci[2];
  co[2] = ci[1];
  co[3] = ci[0];

  return( *(int *)co);
}

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * Function name : tsc_swap_16
 * Prototype     : short
 * Parameters    : short data
 * Return        : data swapped
 *----------------------------------------------------------------------------
 * Description   : return data swapped for 16 bits
 *
 *++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

short
tsc_swap_16( short data)
{
  char ci[2];
  char co[2];

  *(short *)ci = data;
  co[0] = ci[1];
  co[1] = ci[0];

  return( *(short *)co);
}

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * Function name : tsc_get_device
 * Prototype     : int
 * Parameters    : void
 * Return        : device id : central = 0, io = 1
 *----------------------------------------------------------------------------
 * Description   : return device id
 *
 *++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

int tsc_get_device(void){
	int device = -1;

	if (tsc_fd == tsc_fd_central){
		device = 0;
	}
	else if (tsc_fd == tsc_fd_io){
		device = 1;
	}

	return device;
}

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * Function name : tsc_get_lib_version
 * Prototype     : char
 * Parameters    : void
 * Return        : pointer to library version string
 *----------------------------------------------------------------------------
 * Description   : return the library version string 
 *
 *++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

char *
tsc_get_lib_version()
{
  return( tsc_lib_version);
}

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * Function name : tsc_get_drv_name
 * Prototype     : char *
 * Parameters    : none
 * Return        : driver name identifier
 *----------------------------------------------------------------------------
 * Description   : return a pointer to the driver name identifier string
 *
 *++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

char *
tsc_get_drv_name()
{
  return( tsc_drv_name);
}

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * Function name : tsc_drv_version
 * Prototype     : char *
 * Parameters    : none
 * Return        : driver version identifier
 *----------------------------------------------------------------------------
 * Description   : return a pointer to the driver version identifier string
 *
 *++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

char *
tsc_get_drv_version()
{
  return( tsc_drv_version);
}

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * Function name : tsc_get_vendor_id
 * Prototype     : unsigned short
 * Parameters    : none
 * Return        : tsc vendor id
 *----------------------------------------------------------------------------
 * Description   : return vendor id
 *
 *++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

unsigned short
tsc_get_vendor_id()
{
  return( tsc_vendor_id);
}

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * Function name : tsc_get_device_id
 * Prototype     : unsigned short
 * Parameters    : none
 * Return        : tsc device id
 *----------------------------------------------------------------------------
 * Description   : return device id
 *
 *++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

unsigned short
tsc_get_device_id()
{
  return( tsc_device_id);
}

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * Function name : tsc_init
 * Prototype     : int
 * Parameters    : none
 * Return        : file descriptor for tsc control device
 *----------------------------------------------------------------------------
 * Description   : file descriptor
 *
 *++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

int
tsc_init()
{
  if(tsc_fd < 0 )
  {
	// Open both devices
    tsc_fd_central = open("/dev/bus/bridge/tsc_ctl_central", O_RDWR);
    tsc_fd_io      = open("/dev/bus/bridge/tsc_ctl_io", O_RDWR);

    // CENTRAL device must be accessible, IO device is present only on IFC1211 board
    if( tsc_fd_central >= 0)
    {
      // By default CENTRAL devices is used
      tsc_fd = tsc_fd_central;

      ioctl(tsc_fd, TSC_IOCTL_ID_NAME, tsc_drv_name);
      ioctl(tsc_fd, TSC_IOCTL_ID_VERSION, tsc_drv_version);
      ioctl(tsc_fd, TSC_IOCTL_ID_VENDOR, &tsc_vendor_id);
      ioctl(tsc_fd, TSC_IOCTL_ID_DEVICE, &tsc_device_id);
    }
  }
  return( tsc_fd);
}

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * Function name : tsc_exit
 * Prototype     : int
 * Parameters    : none
 * Return        : 0 on success
 *                 < 0 on error
 *----------------------------------------------------------------------------
 * Description   : return allocated resources to os
 *
 *++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

int
tsc_exit()
{
  int retval;

  retval = -EBADF;
  if( tsc_fd >= 0)
  {
    retval = close(tsc_fd);
    retval = close(tsc_fd_central);
    retval = close(tsc_fd_io);
  }
  tsc_fd         = -1;
  tsc_fd_central = -1;
  tsc_fd_io      = -1;
  return( retval);
}

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * Function name : tsc_csr_write
 * Prototype     : int
 * Parameters    : register index
 *                 data pointer
 * Return        : status of write operation
 *----------------------------------------------------------------------------
 * Description   : write the tsc csr pointed by idx with the content of
 *                 data_p. The status of the write operation is returned.
 *
 *++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

int
tsc_csr_write( int idx,
	       int *data_p)
{
  struct tsc_ioctl_csr_op csr_op;

  if( tsc_fd < 0) return(-EBADF);
  csr_op.offset = idx;
  csr_op.data = *data_p;
  return( ioctl( tsc_fd, TSC_IOCTL_CSR_WR, &csr_op));
}

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * Function name : tsc_csr_read
 * Prototype     : int
 * Parameters    : register index
 *                 data pointer
 * Return        : status of read operation
 *----------------------------------------------------------------------------
 * Description   : read the content of the tsc csr pointed by idx and return
 *                 its current value in data_p. The status of the read is
 *                 returned
 *
 *++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

int
tsc_csr_read( int idx,
	      int *data_p)
{
  struct tsc_ioctl_csr_op csr_op;
  int retval;

  if( tsc_fd < 0) return(-EBADF);
  csr_op.offset = idx;
  csr_op.data = -1;
  retval = ioctl( tsc_fd, TSC_IOCTL_CSR_RD, &csr_op);
  *data_p = csr_op.data;

  return( retval);
}

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * Function name : tsc_csr_set
 * Prototype     : int
 * Parameters    : register index
 *                 data pointer
 * Return        : status of write operation
 *----------------------------------------------------------------------------
 * Description   : set bit in the tsc csr pointed by idx with the content of
 *                 data_p. The status of the write operation is returned.
 *
 *++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

int
tsc_csr_set( int idx,
	     int *data_p)
{
  struct tsc_ioctl_csr_op csr_op;

  if( tsc_fd < 0) return(-EBADF);
  csr_op.offset = idx;
  csr_op.data = *data_p;
  csr_op.mask = *data_p;
  return( ioctl( tsc_fd, TSC_IOCTL_CSR_WRm, &csr_op));
}

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * Function name : tsc_smon_write
 * Prototype     : int
 * Parameters    : register index
 *                 data pointer
 * Return        : status of write operation
 *----------------------------------------------------------------------------
 * Description   : write the tsc smon register pointed by idx with the content
 *                 of data_p. The status of the write operation is returned.
 *
 *++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

int
tsc_smon_write( int idx,
	       int *data_p)
{
  struct tsc_ioctl_csr_op smon_op;

  if( tsc_fd < 0) return(-EBADF);
  smon_op.offset = idx;
  smon_op.data = *data_p;
  return( ioctl( tsc_fd, TSC_IOCTL_CSR_SMON_WR, &smon_op));
}


/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * Function name : tsc_smon_read
 * Prototype     : int
 * Parameters    : register index
 *                 data pointer
 * Return        : status of read operation
 *----------------------------------------------------------------------------
 * Description   : read the content of the tsc smon register pointed by idx
 *                 and return its current value in data_p. The status of the
 *                 operation is returned
 *
 *++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

int
tsc_smon_read( int idx,
	      int *data_p)
{
  struct tsc_ioctl_csr_op smon_op;
  int retval;

  if( tsc_fd < 0) return(-EBADF);
  smon_op.offset = idx;
  smon_op.data = -1;
  retval = ioctl( tsc_fd, TSC_IOCTL_CSR_SMON_RD, &smon_op);
  *data_p = smon_op.data;

  return( retval);
}

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * Function name : tsc_pon_write
 * Prototype     : int
 * Parameters    : register index
 *                 data pointer
 * Return        : status of write operation
 *----------------------------------------------------------------------------
 * Description   : write the tsc pon register pointed by idx with the content
 *                 of data_p. The status of the write operation is returned.
 *
 *++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

int
tsc_pon_write( int idx,
	       int *data_p)
{
  struct tsc_ioctl_csr_op pon_op;

  if( tsc_fd < 0) return(-EBADF);
  pon_op.offset = idx;
  pon_op.data = *data_p;
  return( ioctl( tsc_fd, TSC_IOCTL_CSR_PON_WR, &pon_op));
}


/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * Function name : tsc_pon_read
 * Prototype     : int
 * Parameters    : register index
 *                 data pointer
 * Return        : status of read operation
 *----------------------------------------------------------------------------
 * Description   : read the content of the tsc pon register pointed by idx
 *                 and return its current value in data_p. The status of the
 *                 operation is returned
 *
 *++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

int
tsc_pon_read( int idx,
	      int *data_p)
{
  struct tsc_ioctl_csr_op pon_op;
  int retval;

  if( tsc_fd < 0) return(-EBADF);
  pon_op.offset = idx;
  pon_op.data = -1;
  retval = ioctl( tsc_fd, TSC_IOCTL_CSR_PON_RD, &pon_op);
  *data_p = pon_op.data;

  return( retval);
}

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * Function name : tsc_pciep_write
 * Prototype     : int
 * Parameters    : register index
 *                 data pointer
 * Return        : status of write operation
 *----------------------------------------------------------------------------
 * Description   : write the tsc pcie register pointed by idx with the content
 *                 of data_p. The status of the write operation is returned.
 *
 *++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

int
tsc_pciep_write( int idx,
	         int *data_p)
{
  struct tsc_ioctl_csr_op pciep_op;

  if( tsc_fd < 0) return(-EBADF);
  pciep_op.offset = idx;
  pciep_op.data = *data_p;
  return( ioctl( tsc_fd, TSC_IOCTL_CSR_PCIEP_WR, &pciep_op));
}

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * Function name : tsc_pciep_read
 * Prototype     : int
 * Parameters    : register index
 *                 data pointer
 * Return        : status of read operation
 *----------------------------------------------------------------------------
 * Description   : read the content of the tsc pcie register pointed by idx
 *                 and return its current value in data_p. The status of the
 *                 operation is returned
 *
 *++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

int
tsc_pciep_read( int idx,
	      int *data_p)
{
  struct tsc_ioctl_csr_op pciep_op;
  int retval;

  if( tsc_fd < 0) return(-EBADF);
  pciep_op.offset = idx;
  pciep_op.data = -1;
  retval = ioctl( tsc_fd, TSC_IOCTL_CSR_PCIEP_RD, &pciep_op);
  *data_p = pciep_op.data;

  return( retval);
}

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * Function name : tsc_write_blk
 * Prototype     : int
 * Parameters    : remote address
 *                 data buffer pointer
 *                 transfer size (in bytes)
 *                 transfer mode (am,ds,..)
 * Return        : status of write operation
 *----------------------------------------------------------------------------
 * Description   : copy <len> bytes from buffer <buf> to remote space 
 *                 the addressing mode and data size are specified in <mode>
 *
 *++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

int
tsc_write_blk( uint64_t rem_addr,
	       char *buf,
	        int len,
	        uint mode)
{
  struct tsc_ioctl_rdwr rdwr;
  int retval;

  if( len <= 0) return(-EINVAL);
  if( tsc_fd < 0) return(-EBADF);
  rdwr.rem_addr = rem_addr;
  rdwr.buf = buf;
  rdwr.len = len;
  rdwr.mode = mode;
  retval = ioctl( tsc_fd, TSC_IOCTL_RDWR_WRITE, &rdwr);

  return( retval);
}

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * Function name : tsc_read_blk
 * Prototype     : int
 * Parameters    : remote address
 *                 data buffer pointer
 *                 transfer size (in bytes)
 *                 transfer mode (am,ds,..)
 * Return        : status of read operation
 *----------------------------------------------------------------------------
 * Description   : read data block
 *
 *++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

int
tsc_read_blk( uint64_t rem_addr,
	      char *buf,
	      int len,
	      uint mode)
{
  struct tsc_ioctl_rdwr rdwr;
  int retval;

  if( len <= 0) return(-EINVAL);
  if( tsc_fd < 0) return(-EBADF);
  rdwr.rem_addr = rem_addr;
  rdwr.buf = buf;
  rdwr.len = len;
  rdwr.mode = mode;
  retval = ioctl( tsc_fd, TSC_IOCTL_RDWR_READ, &rdwr);

  return( retval);
}
/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * Function name : tsc_write_loop
 * Prototype     : int
 * Parameters    : remote address
 *                 data buffer pointer
 *                 transfer size (in bytes)
 *                 transfer mode (am,ds,..)
 * Return        : status of write operation
 *----------------------------------------------------------------------------
 * Description   : copy <len> bytes from buffer <buf> to remote space 
 *                 the addressing mode and data size are specified in <mode>
 *
 *++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

int
tsc_write_loop( uint64_t rem_addr,
	        char *buf,
	        int len,
	        uint mode)
{
  struct tsc_ioctl_rdwr rdwr;
  int retval;

  if( len <= 0) return(-EINVAL);
  if( tsc_fd < 0) return(-EBADF);
  rdwr.rem_addr = rem_addr;
  rdwr.buf = buf;
  rdwr.len = len | RDWR_LOOP;
  rdwr.mode = mode;
  retval = ioctl( tsc_fd, TSC_IOCTL_RDWR_WRITE, &rdwr);

  return( retval);
}

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * Function name : tsc_read_loop
 * Prototype     : int
 * Parameters    : remote address
 *                 data buffer pointer
 *                 transfer size (in bytes)
 *                 transfer mode (am,ds,..)
 * Return        : status of read operation
 *----------------------------------------------------------------------------
 * Description   : read loop
 *
 *++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

int
tsc_read_loop( uint64_t rem_addr,
	      char *buf,
	      int len,
	      uint mode)
{
  struct tsc_ioctl_rdwr rdwr;
  int retval;

  if( len <= 0) return(-EINVAL);
  if( tsc_fd < 0) return(-EBADF);
  rdwr.rem_addr = rem_addr;
  rdwr.buf = buf;
  rdwr.len = len | RDWR_LOOP;
  rdwr.mode = mode;
  retval = ioctl( tsc_fd, TSC_IOCTL_RDWR_READ, &rdwr);

  return( retval);
}

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * Function name : tsc_write_sgl
 * Prototype     : int
 * Parameters    : remote address
 *                 data  pointer
 *                 transfer mode (am,ds,..)
 * Return        : status of read operation
 *----------------------------------------------------------------------------
 * Description   : perform a single write in a remote space (shm)
 *
 *++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

int
tsc_write_sgl( uint64_t rem_addr,
	       char *data_p,
	       uint mode)
{
  struct tsc_ioctl_rdwr rdwr;
  int retval;

  if( tsc_fd < 0) return(-EBADF);
  rdwr.rem_addr = rem_addr;
  rdwr.buf = data_p;
  rdwr.len = 0;
  rdwr.mode = mode;
  retval = ioctl( tsc_fd, TSC_IOCTL_RDWR_WRITE, &rdwr);

  return( retval);
}

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * Function name : tsc_read_sgl
 * Prototype     : int
 * Parameters    : remote address
 *                 data pointer
 *                 transfer mode (am,ds,..)
 * Parameters    : register index
 *                 data pointer
 * Return        : status of read operation
 *----------------------------------------------------------------------------
 * Description   : perform a single read in a remote space (shm)
 *
 *++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

int
tsc_read_sgl( uint64_t rem_addr,
	      char *data_p,
	      uint mode)
{
  struct tsc_ioctl_rdwr rdwr;
  int retval;

  if( tsc_fd < 0) return(-EBADF);
  rdwr.rem_addr = rem_addr;
  rdwr.buf = data_p;
  rdwr.len = 0;
  rdwr.mode = mode;
  retval = ioctl( tsc_fd, TSC_IOCTL_RDWR_READ, &rdwr);

  return( retval);
}

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * Function name : tsc_shm_write
 * Prototype     : int
 * Parameters    : shm address
 *                 pointer to data buffer
 *                 transfer size (0 for single data)
 *                 data size
 *                 hardware swapping mode
 *                 Select smem1 or smem2
 * Return        : status of write operation
 *----------------------------------------------------------------------------
 * Description   : copy <len> bytes from buffer <buf> to SHM address space
 *
 *++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

int tsc_shm_write(uint shm_addr, char *buf, int len, int ds, int swap, int mem){
	struct tsc_ioctl_rdwr rdwr;
	int retval;

	if(len < 0) return(-EINVAL);
	if(tsc_fd < 0) return(-EBADF);
	rdwr.rem_addr = (uint64_t)shm_addr;
	rdwr.buf = buf;
	rdwr.len = len;
	if (mem == 1) {
		rdwr.m.space = RDWR_SPACE_SHM1;
	}
	else if (mem == 2){
		rdwr.m.space = RDWR_SPACE_SHM2;
	}
	rdwr.m.am = 0;
	rdwr.m.ads = (char)RDWR_MODE_SET_DS( rdwr.m.ads,(char)ds);
	rdwr.m.swap = (char)swap;
	retval = ioctl( tsc_fd, TSC_IOCTL_RDWR_WRITE, &rdwr);

	return( retval);
}

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * Function name : tsc_shm_read
 * Prototype     : int
 * Parameters    : shm address
 *                 pointer to data buffer
 *                 transfer size (0 for single data)
 *                 data size
 *                 hardware swapping mode
 *                 Select smem1 or smem2
 * Return        : status of read operation
 *----------------------------------------------------------------------------
 * Description   : Read a block of data from shm address space
 *
 *++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

int tsc_shm_read(uint shm_addr, char *buf, int len, int ds, int swap, int mem){
	struct tsc_ioctl_rdwr rdwr;
	int retval;

	if(len < 0) return(-EINVAL);
	if(tsc_fd < 0) return(-EBADF);
	rdwr.rem_addr = (uint64_t)shm_addr;
	rdwr.buf = buf;
	rdwr.len = len;
	if (mem == 1) {
		rdwr.m.space = RDWR_SPACE_SHM1;
	}
	else if (mem == 2){
		rdwr.m.space = RDWR_SPACE_SHM2;
	}
	rdwr.m.am = 0;
	rdwr.m.ads = (char)RDWR_MODE_SET_DS( rdwr.m.ads,(char)ds);
	rdwr.m.swap = (char)swap;
	retval = ioctl( tsc_fd, TSC_IOCTL_RDWR_READ, &rdwr);

	return(retval);
}

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * Function name : tsc_usr_write
 * Prototype     : int
 * Parameters    : shm address
 *                 pointer to data buffer
 *                 transfer size (0 for single data)
 *                 data size
 *                 hardware swapping mode
 *                 Select usr1 or usr2
 * Return        : status of write operation
 *----------------------------------------------------------------------------
 * Description   : copy <len> bytes from buffer <buf> to usr address space
 *
 *++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

int tsc_usr_write(uint usr_addr, char *buf, int len, int ds, int swap, int mem){
	struct tsc_ioctl_rdwr rdwr;
	int retval;

	if(len < 0) return(-EINVAL);
	if(tsc_fd < 0) return(-EBADF);
	rdwr.rem_addr = (uint64_t)usr_addr;
	rdwr.buf = buf;
	rdwr.len = len;
	if (mem == 1) {
		rdwr.m.space = RDWR_SPACE_USR1;
	}
	else if (mem == 2){
		rdwr.m.space = RDWR_SPACE_USR2;
	}
	rdwr.m.am = 0;
	rdwr.m.ads = (char)RDWR_MODE_SET_DS( rdwr.m.ads,(char)ds);
	rdwr.m.swap = (char)swap;
	retval = ioctl( tsc_fd, TSC_IOCTL_RDWR_WRITE, &rdwr);

	return( retval);
}

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * Function name : tsc_usr_read
 * Prototype     : int
 * Parameters    : shm address
 *                 pointer to data buffer
 *                 transfer size (0 for single data)
 *                 data size
 *                 hardware swapping mode
 *                 Select usr1 or usr2
 * Return        : status of read operation
 *----------------------------------------------------------------------------
 * Description   : Read a block of data from usr address space
 *
 *++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

int tsc_usr_read(uint usr_addr, char *buf, int len, int ds, int swap, int mem){
	struct tsc_ioctl_rdwr rdwr;
	int retval;

	if(len < 0) return(-EINVAL);
	if(tsc_fd < 0) return(-EBADF);
	rdwr.rem_addr = (uint64_t)usr_addr;
	rdwr.buf = buf;
	rdwr.len = len;
	if (mem == 1) {
		rdwr.m.space = RDWR_SPACE_USR1;
	}
	else if (mem == 2){
		rdwr.m.space = RDWR_SPACE_USR2;
	}
	rdwr.m.am = 0;
	rdwr.m.ads = (char)RDWR_MODE_SET_DS( rdwr.m.ads,(char)ds);
	rdwr.m.swap = (char)swap;
	retval = ioctl( tsc_fd, TSC_IOCTL_RDWR_READ, &rdwr);

	return(retval);
}

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * Function name : tsc_map_alloc
 * Prototype     : int
 * Parameters    : pointer to mapping control structure
 * Return        : status of map operation
 *----------------------------------------------------------------------------
 * Description   : allocate mapping
 *
 *++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

int
tsc_map_alloc( struct tsc_ioctl_map_win *w)
{
  if( tsc_fd < 0) return(-EBADF);
  switch( w->req.mode.sg_id)
  {
    case MAP_ID_MAS_PCIE_MEM:
    case MAP_ID_MAS_PCIE_PMEM:
    {
      return( ioctl( tsc_fd, TSC_IOCTL_MAP_MAS_ALLOC, w));
    }
    default:
    {
      return(-1);
    }
  }
}

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * Function name : tsc_map_get
 * Prototype     : int
 * Parameters    : pointer to mapping control structure
 * Return        : status of get operation
 *----------------------------------------------------------------------------
 * Description   : get the map
 *
 *++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

int
tsc_map_get( struct tsc_ioctl_map_win *w)
{
  if( tsc_fd < 0) return(-EBADF);
  switch( w->sts.mode.sg_id)
  {
    case MAP_ID_MAS_PCIE_MEM:
    case MAP_ID_MAS_PCIE_PMEM:
    {
      return( ioctl( tsc_fd, TSC_IOCTL_MAP_MAS_GET, w));
    }
    default:
    {
      return(-1);
    }
  }
}

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * Function name : tsc_map_free
 * Prototype     : int
 * Parameters    : pointer to mapping control structure
 * Return        : status of free operation
 *----------------------------------------------------------------------------
 * Description   : free the map
 *
 *++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

int
tsc_map_free( struct tsc_ioctl_map_win *w)
{
  if( tsc_fd < 0) return(-EBADF);
  switch( w->req.mode.sg_id)
  {
    case MAP_ID_MAS_PCIE_MEM:
    case MAP_ID_MAS_PCIE_PMEM:
    {
      return( ioctl( tsc_fd, TSC_IOCTL_MAP_MAS_FREE, w));
    }
    default:
    {
      return(-1);
    }
  }
  return( 0);
}

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * Function name : tsc_map_modify
 * Prototype     : int
 * Parameters    : pointer to mapping control structure
 * Return        : status of modify operation
 *----------------------------------------------------------------------------
 * Description   : map modify
 *
 *++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

int
tsc_map_modify( struct tsc_ioctl_map_win *w)
{
  if( tsc_fd < 0) return(-EBADF);
  switch( w->req.mode.sg_id)
  {
    case MAP_ID_MAS_PCIE_MEM:
    case MAP_ID_MAS_PCIE_PMEM:
    {
      return( ioctl( tsc_fd, TSC_IOCTL_MAP_MAS_MODIFY, w));
    }
    default:
    {
      return(-1);
    }
  }
  return( 0);
}

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * Function name : tsc_map_read
 * Prototype     : int
 * Parameters    : pointer to mapping control structure
 * Return        : status of read map operation
 *----------------------------------------------------------------------------
 * Description   : map read
 *
 *++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

int
tsc_map_read( struct tsc_ioctl_map_ctl *m)
{
  if( tsc_fd < 0) return(-EBADF);
  return( ioctl( tsc_fd, TSC_IOCTL_MAP_READ, m));
}

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * Function name : tsc_map_clear
 * Prototype     : int
 * Parameters    : pointer to mapping control structure
 * Return        : status of clear operation
 *----------------------------------------------------------------------------
 * Description   : clear the mapping
 *
 *++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

int
tsc_map_clear( struct tsc_ioctl_map_ctl *m)
{
  if( tsc_fd < 0) return(-EBADF);
  return( ioctl( tsc_fd, TSC_IOCTL_MAP_CLEAR, m));
}

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * Function name : tsc_pci_mmap
 * Prototype     : int
 * Parameters    : pci address and mapping size
 * Return        : status of mmap operation
 *----------------------------------------------------------------------------
 * Description   : pci map
 *
 *++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

void *
tsc_pci_mmap( off_t pci_addr,
	      size_t size)
{
  if( tsc_fd < 0) return(NULL);
  return( mmap( NULL, size, PROT_READ | PROT_WRITE, MAP_SHARED, tsc_fd, pci_addr));
}

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * Function name : tsc_pci_munmap
 * Prototype     : int
 * Parameters    : pci address and mapping size
 * Return        : status of munmap operation
 *----------------------------------------------------------------------------
 * Description   : pci unmap
 *++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

int
tsc_pci_munmap( void *addr,
	        size_t size)
{
  if( tsc_fd < 0) return(-EBADF);
  return( munmap( addr, size));
}

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * Function name : tsc_kbuf_alloc
 * Prototype     : int
 * Parameters    : pointer to kernel buffer request data structure
 * Return        : status of operation
 *----------------------------------------------------------------------------
 * Description   : allocate a kernel buffer suitable for DMA operation
 *
 *++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

int
tsc_kbuf_alloc( struct tsc_ioctl_kbuf_req *kr_p)
{
  if( tsc_fd < 0) return(-EBADF);
  return( ioctl( tsc_fd, TSC_IOCTL_KBUF_ALLOC, kr_p));
}
 
/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * Function name : tsc_kbuf_free
 * Prototype     : int
 * Parameters    : pointer to kernel buffer request data structure
 * Return        : status of operation
 *----------------------------------------------------------------------------
 * Description   : desallocate a kernel buffer suitable for DMA operation
 *
 *++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

int
tsc_kbuf_free( struct tsc_ioctl_kbuf_req *kr_p)
{
  if( tsc_fd < 0) return(-EBADF);
  return( ioctl( tsc_fd, TSC_IOCTL_KBUF_FREE, kr_p));
}

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * Function name : tsc_kbuf_mmap
 * Prototype     : int
 * Parameters    : pointer to kernel buffer request data structure
 * Return        : status of operation
 *----------------------------------------------------------------------------
 * Description   : map kernel buffer in user's space
 *
 *++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

void *
tsc_kbuf_mmap( struct tsc_ioctl_kbuf_req *kr_p)
{
  if( tsc_fd < 0) return(NULL);
  kr_p->u_base =  mmap( NULL, kr_p->size, PROT_READ | PROT_WRITE, MAP_SHARED, tsc_fd, (off_t)kr_p->b_base);
  return( kr_p->u_base);
}

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * Function name : tsc_kbuf_munmap
 * Prototype     : int
 * Parameters    : pointer to kernel buffer request data structure
 * Return        : status of operation
 *----------------------------------------------------------------------------
 * Description   : unmap kernel buffer
 *
 *++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

int
tsc_kbuf_munmap( struct tsc_ioctl_kbuf_req *kr_p)
{
  if( tsc_fd < 0) return(-EBADF);
  return( munmap( kr_p->u_base, kr_p->size));
}

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * Function name : tsc_kbuf_read
 * Prototype     : int
 * Parameters    : pointer to user buffer
 *                 kernel address
 *                 transfer size
 * Return        : status of the read operation
 *----------------------------------------------------------------------------
 * Description   : read data from a kernel buffer
 *
 *++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

int
tsc_kbuf_read( void *k_addr,
	       char *buf,
	       uint size)
{
  struct tsc_ioctl_kbuf_rw rw;

  if( tsc_fd < 0) return(-EBADF);
  rw.buf = (void *)buf;
  rw.k_addr = k_addr;
  rw.size = size;
  return( ioctl( tsc_fd, TSC_IOCTL_KBUF_READ, &rw));
}
 
/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * Function name : tsc_kbuf_write
 * Prototype     : int
 * Parameters    : pointer to user buffer
 *                 kernel address
 *                 transfer size
 * Return        : status of the write operation
 *----------------------------------------------------------------------------
 * Description   : copy data to a kernel buffer
 *
 *++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

int
tsc_kbuf_write( void *k_addr,
	        char *buf,
	        uint size)
{
  struct tsc_ioctl_kbuf_rw rw;

  if( tsc_fd < 0) return(-EBADF);
  rw.buf = (void *)buf;
  rw.k_addr = k_addr;
  rw.size = size;
  return( ioctl( tsc_fd, TSC_IOCTL_KBUF_WRITE, &rw));
}
  
/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * Function name : tsc_dma_move
 * Prototype     : int
 * Parameters    : pointer to dma  request data structure
 * Return        : status of DMA operation
 *----------------------------------------------------------------------------
 * Description   : perform a DMA transfer
 *
 *++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

int
tsc_dma_move( struct tsc_ioctl_dma_req *dr_p)
{
  if( tsc_fd < 0) return(-EBADF);
  return( ioctl( tsc_fd, TSC_IOCTL_DMA_MOVE, dr_p));
}
 
/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * Function name : tsc_dma_wait
 * Prototype     : int
 * Parameters    : pointer to dma  request data structure
 * Return        : status of DMA operation
 *----------------------------------------------------------------------------
 * Description   : wait for DMA transfer to complete
 *
 *++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

int
tsc_dma_wait( struct tsc_ioctl_dma_req *dr_p)
{
  if( tsc_fd < 0) return(-EBADF);
  return( ioctl( tsc_fd, TSC_IOCTL_DMA_WAIT, dr_p));
}
 
/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * Function name : tsc_dma_status
 * Prototype     : int
 * Parameters    : pointer to dma status control structure
 * Return        : status of status operation
 *----------------------------------------------------------------------------
 * Description   : update dma status control structure with current status
 *
 *++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

int
tsc_dma_status( struct tsc_ioctl_dma_sts *ds_p)
{
  if( tsc_fd < 0) return(-EBADF);
  return( ioctl( tsc_fd, TSC_IOCTL_DMA_STATUS, ds_p));
}
 
/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * Function name : tsc_dma_mode
 * Prototype     : int
 * Parameters    : pointer to dma mode control structure
 * Return        : current DMA  mode of operation
 *----------------------------------------------------------------------------
 * Description   : allow to set/get dma mode of operation
 *
 *++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

int
tsc_dma_mode( struct tsc_ioctl_dma_sts *dm_p)
{
  if( tsc_fd < 0) return(-EBADF);
  return( ioctl( tsc_fd, TSC_IOCTL_DMA_MODE, dm_p));
}
 
/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * Function name : tsc_dma_alloc
 * Prototype     : int
 * Parameters    : channel index
 * Return        : status of alloc operation
 *----------------------------------------------------------------------------
 * Description   : request DMA channel allocation
 *
 *++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

int
tsc_dma_alloc( int chan)
{
  struct tsc_ioctl_dma dma;

  dma.chan = (char)chan;
  if( tsc_fd < 0) return(-EBADF);
  return( ioctl( tsc_fd, TSC_IOCTL_DMA_ALLOC, &dma));
} 

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * Function name : tsc_dma_free
 * Prototype     : int
 * Parameters    : dma channel
 * Return        : status of free operation
 *----------------------------------------------------------------------------
 * Description   : free allocated DMA channel
 *
 *++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

int
tsc_dma_free( int chan)
{
  struct tsc_ioctl_dma dma;

  dma.chan = (char)chan;
  if( tsc_fd < 0) return(-EBADF);
  return( ioctl( tsc_fd, TSC_IOCTL_DMA_FREE, &dma));
}
 
/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * Function name : tsc_dma_clear
 * Prototype     : int
 * Parameters    : dma channel
 * Return        : status of clear operation
 *----------------------------------------------------------------------------
 * Description   : dma clear operation
 *
 *++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

int
tsc_dma_clear( int chan)
{
  struct tsc_ioctl_dma dma;

  dma.chan = (char)chan;
  if( tsc_fd < 0) return(-EBADF);
  return( ioctl( tsc_fd, TSC_IOCTL_DMA_CLEAR, &dma));
} 

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * Function name : tsc_sflash_rdid
 * Prototype     : int
 * Parameters    : pointer to 4 byte string to hold sflash id
 * Return        : status of operation
 *----------------------------------------------------------------------------
 * Description   : read sflash identifiers (3 bytes)
 *                 id[0]   -> manufacturer ID
 *                 id[1,2] -> devices ID (15 bits)] 
 *
 *++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

int
tsc_sflash_rdid( char *id)
{
  if( tsc_fd < 0) return(-EBADF);
  return( ioctl( tsc_fd, TSC_IOCTL_SFLASH_RDID, id));
}

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * Function name : tsc_sflash_rdsr
 * Prototype     : int
 * Parameters    : pointer to  hold current value of SR register
 * Return        : status of operation
 *----------------------------------------------------------------------------
 * Description   : read sr
 *
 *++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

int
tsc_sflash_rdsr( char *sr)
{
  if( tsc_fd < 0) return(-EBADF);
  return( ioctl( tsc_fd, TSC_IOCTL_SFLASH_RDSR, sr));
} 

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * Function name : tsc_sflash_wrsr
 * Prototype     : int
 * Parameters    : pointer to char to hold value of SR to be written
 * Return        : status of operation
 *----------------------------------------------------------------------------
 * Description   : write sflash status register
 *
 *++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

int
tsc_sflash_wrsr( char *sr)
{
  if( tsc_fd < 0) return(-EBADF);
  return( ioctl( tsc_fd, TSC_IOCTL_SFLASH_WRSR, sr));
} 

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * Function name : tsc_sflash_read
 * Prototype     : int
 * Parameters    : offset in SFLASH from where to read
 *                 pointer to destination buffer
 *                 number of byte to read
 * Return        : status of operation
 *----------------------------------------------------------------------------
 * Description   : read sflash
 *
 *++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

int
tsc_sflash_read( int offset, char *buf, int len)
{
  struct tsc_ioctl_rdwr rw;

  if( tsc_fd < 0) return(-EBADF);
  rw.rem_addr = offset;
  rw.buf = buf;
  rw.len = len;
  return( ioctl( tsc_fd, TSC_IOCTL_SFLASH_READ, &rw));
} 

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * Function name : tsc_sflash_write
 * Prototype     : int
 * Parameters    : offset in sflash from where to write
 *                 pointer to destination buffer
 *                 number of byte to read
 * Return        : status of operation
 *----------------------------------------------------------------------------
 * Description   : write sflash
 *
 *++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

int
tsc_sflash_write( int offset, char *buf, int len)
{
  struct tsc_ioctl_rdwr rw;

  if( tsc_fd < 0) return(-EBADF);
  rw.rem_addr = offset;
  rw.buf = buf;
  rw.len = len;
  return( ioctl( tsc_fd, TSC_IOCTL_SFLASH_WRITE, &rw));
} 

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * Function name : tsc_timer_start
 * Prototype     : int
 * Parameters    : timer operation mode
 *                 start time (msec)
 * Return        : status of operation
 *----------------------------------------------------------------------------
 * Description   : start the global timer
 *
 *++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

int
tsc_timer_start( int mode,
		 int msec)
{
  struct tsc_ioctl_timer tmr;

  tmr.mode = mode;
  tmr.time.msec = msec;
  tmr.time.usec = 0;

  if( tsc_fd < 0) return(-EBADF);
  return( ioctl( tsc_fd, TSC_IOCTL_TIMER_START, &tmr));
}

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * Function name : tsc_timer_restart
 * Prototype     : int
 * Parameters    : none
 * Return        : status of  operation
 *----------------------------------------------------------------------------
 * Description   : restart the tsc global timer
 *
 *++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

int
tsc_timer_restart( void)
{
  if( tsc_fd < 0) return(-EBADF);
  return( ioctl( tsc_fd, TSC_IOCTL_TIMER_RESTART, NULL));
} 

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * Function name : tsc_timer_stop
 * Prototype     : int
 * Parameters    : none
 * Return        : status of operation
 *----------------------------------------------------------------------------
 * Description   : stop the tsc global timer
 *
 *++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

int
tsc_timer_stop( )
{
  if( tsc_fd < 0) return(-EBADF);
  return( ioctl( tsc_fd, TSC_IOCTL_TIMER_STOP, NULL));
} 

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * Function name : tsc_timer_read
 * Prototype     : int
 * Parameters    : pointer to tsc_time data structure
 * Return        : status of operation
 *----------------------------------------------------------------------------
 * Description   : return the current value og the tsc global timer
 *                 in the tsc_time data structure
 *
 *++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

int
tsc_timer_read( struct tsc_time *tm)
{
  if( tsc_fd < 0) return(-EBADF);
  return( ioctl( tsc_fd, TSC_IOCTL_TIMER_READ, tm));
} 

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * Function name : tsc_fifo_init
 * Prototype     : int
 * Parameters    : fifo index ( 0 -> 7)
 *                 operating mode
 * Return        : status of operation
 *----------------------------------------------------------------------------
 * Description   : initialize fifo according to mode parameter
 *                 
 *++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

int 
tsc_fifo_init( int idx,
               int mode)
{
  struct tsc_ioctl_fifo fifo;
  int retval;
  
  if( tsc_fd < 0) return(-EBADF);
  fifo.idx  = idx;
  fifo.mode = mode;
  retval = ioctl( tsc_fd, TSC_IOCTL_FIFO_INIT, &fifo);
  return( retval);
}

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * Function name : tsc_fifo_status
 * Prototype     : int
 * Parameters    : fifo index ( 0 -> 7)
 *                 pointer to hold current status (uint *)
 * Return        : status of operation
 *----------------------------------------------------------------------------
 * Description   : returns the current status of fifo idx
 *                 
 *++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

int 
tsc_fifo_status( uint idx,
		 uint *sts)
{
  struct tsc_ioctl_fifo fifo;
  int retval;
  
  fifo.idx = idx;
  fifo.sts = 0;

  retval = ioctl( tsc_fd, TSC_IOCTL_FIFO_STATUS, &fifo);
  if(sts)
  {
    *sts = fifo.sts;
  }
  else
  {
    if (! retval)
	{
	  retval = fifo.sts;
	}
  }
  return(retval);
}

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * Function name : tsc_fifo_clear
 * Prototype     : int
 * Parameters    : fifo index ( 0 -> 7)
 *                 pointer to hold current status (uint *)
 * Return        : status of operation
 *----------------------------------------------------------------------------
 * Description   : clear fifo idx
 *                 
 *++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

int 
tsc_fifo_clear( uint idx,
		uint *sts)
{
  struct tsc_ioctl_fifo fifo;
  int retval;
  
  fifo.idx = idx;
  fifo.sts = 0;
  retval = ioctl( tsc_fd, TSC_IOCTL_FIFO_CLEAR, &fifo);
  if(sts)
  {
    *sts = fifo.sts;
  }
  return( retval);
}

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * Function name : tsc_fifo_wait_ef
 * Prototype     : int
 * Parameters    : fifo index ( 0 -> 7)
 *                 pointer to hold status (uint *)
 *                 timeout
 * Return        : status of operation
 *----------------------------------------------------------------------------
 * Description   : wait for fifo empty -> not empty
 *                 
 *++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

int
tsc_fifo_wait_ef( uint idx,
		  uint *sts,
		  uint tmo)
{
  struct tsc_ioctl_fifo fifo;
  int retval;
  
  fifo.idx = idx;
  fifo.tmo = tmo;
  fifo.sts = 0;
  retval = ioctl( tsc_fd, TSC_IOCTL_FIFO_WAIT_EF, &fifo);
  if(sts)
  {
    *sts = fifo.sts;
  }
  return( retval);
}

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * Function name : tsc_fifo_wait_ff
 * Prototype     : int
 * Parameters    : fifo index ( 0 -> 7)
 *                 pointer to hold status (uint *)
 *                 timeout
 * Return        : status of operation
 *----------------------------------------------------------------------------
 * Description   : wait for fifo full -> not full
 *                 
 *++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

int
tsc_fifo_wait_ff( uint idx,
		  uint *sts,
		  uint tmo)
{
  struct tsc_ioctl_fifo fifo;
  int retval;
  
  fifo.idx = idx;
  fifo.tmo = tmo;
  fifo.sts = 0;
  retval = ioctl( tsc_fd, TSC_IOCTL_FIFO_WAIT_FF, &fifo);
  if(sts)
  {
    *sts = fifo.sts;
  }
  return( retval);
}

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * Function name : tsc_fifo_read
 * Prototype     : int
 * Parameters    : fifo index ( 0 -> 7)
 *                 point to buffer to hold data (uint *)
 *                 word count
 *                 pointer to hold status (uint *)
 *                 timeout
 * Return        : status of operation
 *----------------------------------------------------------------------------
 * Description   : read up to <wcnt> data from fifo <idx> and store in buffer
 *                 pointed by <data>.
 *                 return current status in <sts>
 *                 
 *++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

int 
tsc_fifo_read( uint idx,
	       uint *data,
	       uint wcnt,
	       uint *sts)
{
  struct tsc_ioctl_fifo fifo;
  int retval;
  
  fifo.idx  = idx;
  fifo.data = data;
  fifo.cnt  = wcnt;
  fifo.sts  = 0;
  retval = ioctl( tsc_fd, TSC_IOCTL_FIFO_READ, &fifo);
  if(sts)
  {
    *sts = fifo.sts;
  }
  return( retval);
}

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * Function name : tsc_fifo_write
 * Prototype     : int
 * Parameters    : fifo index ( 0 -> 7)
 *                 point to buffer to hold data (uint *)
 *                 word count
 *                 pointer to hold status (uint *)
 *                 timeout
 * Return        : status of  operation
 *----------------------------------------------------------------------------
 * Description   : write up to <wcnt> data to fifo <idx> from  buffer pointed
 *                 by <data>.
 *                 return current status in <sts>
 *                 
 *++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

int 
tsc_fifo_write( uint idx,
	        uint *data,
	        uint wcnt,
	        uint *sts)
{
  struct tsc_ioctl_fifo fifo;
  int retval;
  
  fifo.idx  = idx;
  fifo.data = data;
  fifo.cnt  = wcnt;
  fifo.sts  = 0;
  retval = ioctl( tsc_fd, TSC_IOCTL_FIFO_WRITE, &fifo);
  if(sts)
  {
    *sts = fifo.sts;
  }
  return( retval);
}

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * Function name : i2c_set_dev
 * Prototype     : int
 * Parameters    : i2c device
 * Return        : status of operation
 *----------------------------------------------------------------------------
 * Description   : Set i2c device
 *
 *++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

static int
i2c_set_dev( int dev)
{
  int fd;
  int addr;
  char filename[16];

  switch( (dev >> 29) & 3)
  {
    case 0:
    {
      sprintf(filename,"/dev/i2c-0");
      break;
    }
    case 1:
    {
      sprintf(filename,"/dev/i2c-1");
      break;
    }
    case 2:
    {
      sprintf(filename,"/dev/i2c-2");
      break;
    }
    default:
    {
      return(-1);
    }
  }
  fd = open(filename,O_RDWR);
  if( fd < 0)
  {
    printf("cannot open %s\n", filename);
    return( -1);
  }
  addr = dev & 0x7f;
  if (ioctl( fd, I2C_SLAVE, addr) < 0) 
  {
    printf("cannot select I2C_SLVE %x\n", addr);
    close( fd);
    return( -1);
  }
  return(fd);
}

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * Function name : tsc_i2c_read
 * Prototype     : int
 * Parameters    : i2c device
 *                 register index
 *                 data
 * Return        : status of read operation
 *----------------------------------------------------------------------------
 * Description   : read register <reg> of i2c device <dev> and returns the
 *                 result in <data>
 *                 by <data>.
 *
 *++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

int
tsc_i2c_read( uint dev,
	      uint reg,
	      uint *data)
{
  if( dev & 0x80000000)
  {
    struct tsc_ioctl_i2c i2c;

    i2c.device = dev & ~0x80000000;
    i2c.cmd = reg;
    i2c.data = 0;
    ioctl( tsc_fd, TSC_IOCTL_I2C_READ, &i2c);
    *data = i2c.data;

    return( i2c.status);
  }
  else 
  {
    int fd;
    int cmd_size;
    int data_size;

    fd = i2c_set_dev( dev);
    if( fd < 0)
    {
      return(-1);
    }
    cmd_size  = ((dev>>16)&3)+1;
    data_size = ((dev>>18)&3)+1;
    if( cmd_size == 2)
    {
      i2c_smbus_write_byte_data(fd, (reg>>8)&0xff, reg&0xff);
      *data = i2c_smbus_read_byte(fd);
    }
    if( cmd_size == 1)
    {
      if( data_size == 1) *data = i2c_smbus_read_byte_data(fd, reg);
      if( data_size == 2) *data = i2c_smbus_read_word_data(fd, reg);
    }
    close(fd);
  }
  return(0);
}

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * Function name : tsc_i2c_cmd
 * Prototype     : int
 * Parameters    : i2c device
 *                 command
 * Return        : status of command operation
 *----------------------------------------------------------------------------
 * Description   : execute a command operation
 *                 return current status in <sts>
 *                 
 *++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

int
tsc_i2c_cmd( uint dev,
	     uint cmd)
{
  int fd;

  if( dev & 0x80000000)
  {
    struct tsc_ioctl_i2c i2c;

    i2c.device = dev & ~0x80000000;
    i2c.cmd = cmd;
    i2c.data = 0;
    ioctl( tsc_fd, TSC_IOCTL_I2C_CMD, &i2c);

    return( i2c.status);
  }
  else 
  {
    fd = i2c_set_dev( dev);
    if( fd < 0)
    {
      return(-1);
    }
    if( i2c_smbus_write_byte(fd, cmd) < 0)
    {
      return(-1);
    }
    close(fd);
  }
}

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * Function name : tsc_i2c_write
 * Prototype     : int
 * Parameters    : i2c device
 *                 register index
 *                 data
 * Return        : status of write operation
 *----------------------------------------------------------------------------
 * Description   : write  <data> to register <reg> of i2c device <dev>
 *                 return current status in <sts>
 *                 
 *++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

int
tsc_i2c_write( uint dev,
	       uint reg,
	       uint data)
{
  if( dev & 0xe0000000)
  {
    struct tsc_ioctl_i2c i2c;

    i2c.device = dev & ~0x80000000;
    i2c.cmd = reg;
    i2c.data = data;
    ioctl( tsc_fd, TSC_IOCTL_I2C_WRITE, &i2c);

    return( i2c.status);
  }
  else 
  {
    int fd;
    int cmd_size;
    int data_size;

    fd = i2c_set_dev( dev);
    if( fd < 0)
    {
      return(-1);
    }
    cmd_size  = ((dev>>16)&3)+1;
    data_size = ((dev>>18)&3)+1;
    if( cmd_size == 2)
    {
      i2c_smbus_write_word_data(fd, (reg>>8)&0xff, (reg&0xff) | ((data&0xff)<<8));
    }
    if( cmd_size == 1)
    {
      if( data_size == 1) i2c_smbus_write_byte_data(fd, reg, data);
      if( data_size == 2) i2c_smbus_write_word_data(fd, reg, data);
    }
    close(fd);
  }
  return(0);
}

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * Function name : tsc_i2c_reset
 * Prototype     : int
 * Parameters    : i2c device
 * Return        : status of reset operation
 *----------------------------------------------------------------------------
 * Description   : reset i2c controller
 *                 return current status in <sts>
 *                 
 *++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

int
tsc_i2c_reset( uint dev)
{
  return(0);
}

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * Function name : tsc_semaphore_status
 * Prototype     : int
 * Parameters    : status
 * Return        : status of semaphore
 *----------------------------------------------------------------------------
 * Description   : acquire status of semaphore
 *
 *++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

int tsc_semaphore_status(uint *sts){
	struct tsc_ioctl_semaphore semaphore;
	int retval = 0;

	semaphore.sts = 0;

	retval = ioctl(tsc_fd, TSC_IOCTL_SEMAPHORE_STATUS, &semaphore);
	if(sts){
		*sts = semaphore.sts;
	}
	if(!retval){
		retval = semaphore.sts;
	}
	return(retval);
}

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * Function name : tsc_semaphore_release
 * Prototype     : int
 * Parameters    : semaphore identifier
 * Return        : status of operation
 *----------------------------------------------------------------------------
 * Description   : release semaphore
 *
 *++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

int tsc_semaphore_release(uint idx, uint tag){
	struct tsc_ioctl_semaphore semaphore;
	int retval = 0;

	semaphore.idx = idx;
	semaphore.tag = tag;
	semaphore.sts = 0;

	retval = ioctl(tsc_fd, TSC_IOCTL_SEMAPHORE_RELEASE, &semaphore);
	return(retval);
}

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * Function name : tsc_semaphore_get
 * Prototype     : int
 * Parameters    : semaphore identifier and semaphore tag
 * Return        : status of operation
 *----------------------------------------------------------------------------
 * Description   : get semaphore
 *
 *++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

int tsc_semaphore_get(uint idx, uint tag){
	struct tsc_ioctl_semaphore semaphore;
	int retval = 0;

	semaphore.idx = idx;
	semaphore.tag = tag;

	retval = ioctl(tsc_fd, TSC_IOCTL_SEMAPHORE_GET, &semaphore);
	return(retval);
}

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * Function name : set_device
 * Prototype     : int
 * Parameters    : device number
 * Return        : status of operation
 *----------------------------------------------------------------------------
 * Description   : change between central and io device if available
 *
 *++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

int set_device(int device){
	int retval = 0;

	// 0 -> CENTRAL device
	// 1 -> IO      device
	if((device == 0) && (tsc_fd_central >= 0)) {
		tsc_fd = tsc_fd_central;
	}
	else if ((device == 1) && (tsc_fd_io >= 0)){
		tsc_fd = tsc_fd_io;
	}
	// CENTRAL (never happens) or IO not available (IFC1410)
	else{
		return(-1);
	}

	return(retval);
}

/*
User I2C devices
 -----------------------------------------------------------------------------
| PCB    | part       | device   | 7-bit   |                                  |
| refdes | name       | function | address | I2C access                       |
|--------+------------+----------+---------+----------------------------------|
| U111   | TCA9548A   | switch   | 0x70    | direct                           |
|--------+------------+----------+---------+----------------------------------|
| U107   | PCA9539BS  | I/O      | 0x74    | direct                           |
|--------+------------+----------+---------+----------------------------------|
| U108   | PCA9539BS  | I/O      | 0x75    | direct                           |
|--------+------------+----------+---------+----------------------------------|
| U120   | PCA9539BS  | I/O      | 0x76    | direct                           |
|--------+------------+----------+---------+----------------------------------|
| U121   | PCA9539BS  | I/O      | 0x77    | direct                           |
|--------+------------+----------+---------+----------------------------------|
| U100   | DS110DF111 | CDR      | 0x18    | switch channel 1 (PCB SFP7)      |
|--------+------------+----------+---------+----------------------------------|
| U102   | DS110DF111 | CDR      | 0x18    | switch channel 2 (PCB SFP6)      |
 -----------------------------------------------------------------------------

U111 I2C switch channels assignation
--------------------------------------------------------------------------------

  * channel 0: PCB SFP5
  * channel 1: PCB SFP7
  * channel 2: PCB SFP6
  * channel 3: PCB SFP1
  * channel 4: PCB SFP2
  * channel 5: PCB SFP3
  * channel 6: PCB SFP4
  * channel 7: extension board (EXT)

U107 I/O expander I/O assignation
 -----------------------------------------------------------------------------
| port | pin | direction | usage                                              |
|------+-----+-----------+----------+-----------------------------------------|
| [0]  | [0] | n/a       | no connect                                         |
|      | [1] | n/a       | no connect                                         |
|      | [2] | n/a       | no connect                                         |
|      | [3] | input     | EXT presence: 0 = present, 1 = absent              |
|      | [4] | n/a       | no connect                                         |
|      | [5] | n/a       | no connect                                         |
|      | [6] | in/out/Z  | EXT I/O 5 (application specific)                   |
|      | [7] | in/out/Z  | EXT I/O 6 (application specific)                   |
|------+-----+-----------+----------+-----------------------------------------|
| [1]  | [0] | output    | SFP5 TX disable: 0 = enable, 1 = disable           |
|      | [1] | output    | SFP5 rate select 0                                 |
|      | [2] | output    | SFP5 rate select 1                                 |
|      | [3] | input     | SFP5 presence: 0 = present, 1 = absent             |
|      | [4] | input     | SFP5 TX fault: 0 = ok, 1 = fault                   |
|      | [5] | input     | SFP5 LOS (loss of signal): 0 = ok, 1 = fault       |
|      | [6] | in/out/Z  | EXT I/O 3 (application specific)                   |
|      | [7] | in/out/Z  | EXT I/O 4 (application specific)                   |
 -----------------------------------------------------------------------------

U108 I/O expander I/O assignation
 -----------------------------------------------------------------------------
| port | pin | direction | usage                                              |
|------+-----+-----------+----------+-----------------------------------------|
| [0]  | [0] | output    | SFP7 TX disable: 0 = enable, 1 = disable           |
|      | [1] | output    | SFP7 rate select 0                                 |
|      | [2] | output    | SFP7 rate select 1                                 |
|      | [3] | input     | SFP7 presence: 0 = present, 1 = absent             |
|      | [4] | input     | SFP7 TX fault: 0 = ok, 1 = fault                   |
|      | [5] | input     | SFP7 LOS (loss of signal): 0 = ok, 1 = fault       |
|      | [6] | in/out/Z  | EXT I/O 0 (application specific)                   |
|      | [7] | in/out/Z  | EXT I/O 1 (application specific)                   |
|------+-----+-----------+----------+-----------------------------------------|
| [1]  | [0] | output    | SFP6 TX disable: 0 = enable, 1 = disable           |
|      | [1] | output    | SFP6 rate select 0                                 |
|      | [2] | output    | SFP6 rate select 1                                 |
|      | [3] | input     | SFP6 presence: 0 = present, 1 = absent             |
|      | [4] | input     | SFP6 TX fault: 0 = ok, 1 = fault                   |
|      | [5] | input     | SFP6 LOS (loss of signal): 0 = ok, 1 = fault       |
|      | [6] | in/out/Z  | EXT I/O 2 (application specific)                   |
|      | [7] | n/a       | no connect                                         |
 -----------------------------------------------------------------------------

U120 I/O expander I/O assignation
 -----------------------------------------------------------------------------
| port | pin | direction | usage                                              |
|------+-----+-----------+----------+-----------------------------------------|
| [0]  | [0] | output    | SFP1 TX disable: 0 = enable, 1 = disable           |
|      | [1] | output    | SFP1 rate select 0                                 |
|      | [2] | output    | SFP1 rate select 1                                 |
|      | [3] | input     | SFP1 presence: 0 = present, 1 = absent             |
|      | [4] | input     | SFP1 TX fault: 0 = ok, 1 = fault                   |
|      | [5] | input     | SFP1 LOS (loss of signal): 0 = ok, 1 = fault       |
|      | [6] | output    | LED126 green: 0 = off, 1 = on                      |
|      | [7] | output    | LED126 red: 0 = off, 1 = on                        |
|------+-----+-----------+----------+-----------------------------------------|
| [1]  | [0] | output    | SFP2 TX disable: 0 = enable, 1 = disable           |
|      | [1] | output    | SFP2 rate select 0                                 |
|      | [2] | output    | SFP2 rate select 1                                 |
|      | [3] | input     | SFP2 presence: 0 = present, 1 = absent             |
|      | [4] | input     | SFP2 TX fault: 0 = ok, 1 = fault                   |
|      | [5] | input     | SFP2 LOS (loss of signal): 0 = ok, 1 = fault       |
|      | [6] | output    | LED125 green: 0 = off, 1 = on                      |
|      | [7] | output    | LED125 red: 0 = off, 1 = on                        |
 -----------------------------------------------------------------------------

U121 I/O expander I/O assignation
 -----------------------------------------------------------------------------
| port | pin | direction | usage                                              |
|------+-----+-----------+----------+-----------------------------------------|
| [0]  | [0] | output    | SFP3 TX disable: 0 = enable, 1 = disable           |
|      | [1] | output    | SFP3 rate select 0                                 |
|      | [2] | output    | SFP3 rate select 1                                 |
|      | [3] | input     | SFP3 presence: 0 = present, 1 = absent             |
|      | [4] | input     | SFP3 TX fault: 0 = ok, 1 = fault                   |
|      | [5] | input     | SFP3 LOS (loss of signal): 0 = ok, 1 = fault       |
|      | [6] | output    | LED124 green: 0 = off, 1 = on                      |
|      | [7] | output    | LED124 red: 0 = off, 1 = on                        |
|------+-----+-----------+----------+-----------------------------------------|
| [1]  | [0] | output    | SFP4 TX disable: 0 = enable, 1 = disable           |
|      | [1] | output    | SFP4 rate select 0                                 |
|      | [2] | output    | SFP4 rate select 1                                 |
|      | [3] | input     | SFP4 presence: 0 = present, 1 = absent             |
|      | [4] | input     | SFP4 TX fault: 0 = ok, 1 = fault                   |
|      | [5] | input     | SFP4 LOS (loss of signal): 0 = ok, 1 = fault       |
|      | [6] | output    | LED123 green: 0 = off, 1 = on                      |
|      | [7] | output    | LED123 red: 0 = off, 1 = on                        |
 -----------------------------------------------------------------------------
*/

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * Function name : rsp1461_presence
 * Prototype     : int
 * Parameters    : int *present
 * Return        : status of operation
 *----------------------------------------------------------------------------
 * Description   : RSP1461 board support function, check presence
 *
 *++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

int rsp1461_presence(void) {
	int retval = 0;
	int offset = 0;
	unsigned char magic_byte;

	// Get mbox info for RSP presence
	mbox_info_t *info = alloc_mbox_info();
	if (!info){
		printf("Allocation of mbox info structure failed \n");
		return (-1);
	}

	if (pop_mbox_byte(&offset, &magic_byte)){
		printf("Mbox magic get failed\n");
		return (-1);
	}
	if (magic_byte != MAGIC_BYTE_VALUE_VALID){
		printf("Mbox magic check failed \n");
		return (-1);
	}

    pop_mbox_byte(&offset, &info->rtm_status);
    pop_mbox_tribyte(&offset, &info->rtm_manufacturer_id);
    pop_mbox_int(&offset, &info->rtm_zone3_interface_designator);

	return retval;
}

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * Function name : rsp1461_init
 * Prototype     : int
 * Parameters    : void
 * Return        : status of operation
 *----------------------------------------------------------------------------
 * Description   : Initialization function
 *
 *++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

int rsp1461_init(void){
	int retval = 0;
	int addr   = 0;
	int bus    = 4;
	int reg    = 0;
	int rs     = 1;
	int ds     = 1;
	int device = 0;
	int data   = 0;

// ----- Init U120 -----
	addr = 0x76;
	data = 0x38; // 0011'1000 out->0, in->1
	reg  = 6; // Port[0]
	device = (bus & 7) << 29; device |= addr & 0x7f; device |= ((rs - 1) & 3) << 16; device |= ((ds - 1) & 3) << 18;
	retval = tsc_i2c_write(device, reg, data);
	reg  = 7; // Port[1]
	data = 0x38; // 0011'1000 out->0, in->1
	retval = tsc_i2c_write(device, reg, data);

// ----- Init U121 -----
	addr = 0x77;
	reg  = 6; // Port[0]
	data = 0x38; // 0011'1000 out->0, in->1
	device = (bus & 7) << 29; device |= addr & 0x7f; device |= ((rs - 1) & 3) << 16; device |= ((ds - 1) & 3) << 18;
	retval = tsc_i2c_write(device, reg, data);
	reg  = 7; // Port[1]
	data = 0x38; // 0011'1000 out->0, in->1
	retval = tsc_i2c_write(device, reg, data);

// ----- Init U107 -----
	addr = 0x74;
	reg  = 6; // Port[0]
	data = 0xff; // 1111'1111 out->0, in->1
	device = (bus & 7) << 29; device |= addr & 0x7f; device |= ((rs - 1) & 3) << 16; device |= ((ds - 1) & 3) << 18;
	retval = tsc_i2c_write(device, reg, data);
	reg  = 7; // Port[1]
	data = 0xf8; // 1111'1000 out->0, in->1
	retval = tsc_i2c_write(device, reg, data);

// ----- Init U108 -----
	addr = 0x75;
	reg  = 6; // Port[0]
	data = 0xf8; // 1111'1000 out->0, in->1
	device = (bus & 7) << 29; device |= addr & 0x7f; device |= ((rs - 1) & 3) << 16; device |= ((ds - 1) & 3) << 18;
	retval = tsc_i2c_write(device, reg, data);
	reg  = 7; // Port[1]
	data = 0xf8; // 1111'1000 out->0, in->1
	retval = tsc_i2c_write(device, reg, data);

	//check on-board devices presence
	//set I/O expanders pins direction and default state
	// Blink MMC LED
	//set_mtca4_rtm_led_state(......);
	return retval;
}

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * Function name : rsp1461_extension_presence
 * Prototype     : int
 * Parameters    : int *present
 * Return        : status of operation
 *----------------------------------------------------------------------------
 * Description   : Extension board support function, check presence
 *
 *++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

int rsp1461_extension_presence(int *present) {
	int retval = 0;
	int addr   = 0x74; //U107
	int bus    = 4;
	int reg    = 0;
	int rs     = 1;
	int ds     = 1;
	int device = 0;
	int data   = 0;

    // Check U107 EXT presence bit at port 0
	device = (bus&7)<<29; device |= addr & 0x7f;device |= ((rs-1)&3)<<16; device |= ((ds-1)&3)<<18;
	retval = tsc_i2c_read(device, reg, &data);
	*present = (data & 0x8) >> 3;

	return retval;
}

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * Function name : rsp1461_extension_set_pin_state
 * Prototype     : int
 * Parameters    : int index, rsp1461_ext_pin_state_t state
 * Return        : status of operation
 *----------------------------------------------------------------------------
 * Description   : Extension board set pin state
 *
 *++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

int rsp1461_extension_set_pin_state(int index, rsp1461_ext_pin_state_t state) {
	int retval = 0;
	int addr     = 0;
	int bus      = 4;
	int reg      = 0;
	int rs       = 1;
	int ds       = 1;
	int device   = 0;
	int data     = 0;
	int data_org = 0;

	switch(index){
		// U108 PORT#0
	    case 0:
	    case 1:
	    	addr = 0x75;
	    	device = (bus & 7) << 29; device |= addr & 0x7f; device |= ((rs - 1) & 3) << 16; device |= ((ds - 1) & 3) << 18;
	    	if(index == 0){
	    		if((state == RSP1461_EXT_PIN_LOW) || (state == RSP1461_EXT_PIN_HIGH)) {
	    			// Write to the output the value
	    			reg = 2;
	    			// Read pin value
	    			retval = tsc_i2c_read(device, reg, &data);
	    			data ^= (-state ^ data) & (1 << 6); // Bit n will be set if x is 1, and cleared if x is 0. // number ^= (-x ^ number) & (1 << n);
	    			retval = tsc_i2c_write(device, reg, data);
	    			// Configure pin as an output
	    	    	reg = 6;
	    	    	// Read pin direction
	    	    	retval = tsc_i2c_read(device, reg, &data);
	    	    	// Set specific pin as an output direction (clear bit)
	    	    	data &= ~(1 << 6);
	    	    	retval = tsc_i2c_write(device, reg, data);
	    		}
	    		else if (state == RSP1461_EXT_PIN_Z){
	    			// Configure pin as an input
	    	    	reg = 6;
	    	    	// Read pin direction
	    	    	retval = tsc_i2c_read(device, reg, &data);
	    	    	// Set specific pin as an input direction (set bit)
	    	    	data |= 1 << 6;
	    	    	retval = tsc_i2c_write(device, reg, data);
	    		}
	    	}
	    	else if (index == 1){
	    		if((state == RSP1461_EXT_PIN_LOW) || (state == RSP1461_EXT_PIN_HIGH)) {
	    			// Write to the output the value
	    			reg = 2;
	    			// Read pin value
	    			retval = tsc_i2c_read(device, reg, &data);
	    			data ^= (-state ^ data) & (1 << 7); // Bit n will be set if x is 1, and cleared if x is 0. // number ^= (-x ^ number) & (1 << n);
	    			retval = tsc_i2c_write(device, reg, data);
	    			// Configure pin as an output
	    	    	reg = 6;
	    	    	// Read pin direction
	    	    	retval = tsc_i2c_read(device, reg, &data);
	    	    	// Set specific pin as an output direction (clear bit)
	    	    	data &= ~(1 << 7);
	    	    	retval = tsc_i2c_write(device, reg, data);
	    		}
	    		else if (state == RSP1461_EXT_PIN_Z){
	    			// Configure pin as an input
	    	    	reg = 6;
	    	    	// Read pin direction
	    	    	retval = tsc_i2c_read(device, reg, &data);
	    	    	// Set specific pin as an input direction (set bit)
	    	    	data |= 1 << 7;
	    	    	retval = tsc_i2c_write(device, reg, data);
	    		}
	    	}
	    	break;
	    // U108 PORT#1
	    case 2:
	    	addr = 0x75;
	    	device = (bus & 7) << 29; device |= addr & 0x7f; device |= ((rs - 1) & 3) << 16; device |= ((ds - 1) & 3) << 18;
	    	if((state == RSP1461_EXT_PIN_LOW) || (state == RSP1461_EXT_PIN_HIGH)) {
	    		// Write to the output the value
	    		reg = 3;
	    		// Read pin value
	    		retval = tsc_i2c_read(device, reg, &data);
	    		data ^= (-state ^ data) & (1 << 6); // Bit n will be set if x is 1, and cleared if x is 0. // number ^= (-x ^ number) & (1 << n);
	    		retval = tsc_i2c_write(device, reg, data);
	    		// Configure pin as an output
	    	   	reg = 7;
	    	   	// Read pin direction
	    	   	retval = tsc_i2c_read(device, reg, &data);
	    	   	// Set specific pin as an output direction (clear bit)
	    	   	data &= ~(1 << 6);
	    	   	retval = tsc_i2c_write(device, reg, data);
	    	}
	    	else if (state == RSP1461_EXT_PIN_Z){
	    		// Configure pin as an input
	    	   	reg = 7;
	    	   	// Read pin direction
	    	   	retval = tsc_i2c_read(device, reg, &data);
	    	   	// Set specific pin as an input direction (set bit)
	    	   	data |= 1 << 6;
	    	   	retval = tsc_i2c_write(device, reg, data);
	    	}
	        break;
	    // U107 PORT#1
	    case 3:
	    case 4:
	    	addr = 0x74;
	    	device = (bus & 7) << 29; device |= addr & 0x7f; device |= ((rs - 1) & 3) << 16; device |= ((ds - 1) & 3) << 18;
	    	if(index == 0){
	    		if((state == RSP1461_EXT_PIN_LOW) || (state == RSP1461_EXT_PIN_HIGH)) {
	    			// Write to the output the value
	    			reg = 3;
	    			// Read pin value
	    			retval = tsc_i2c_read(device, reg, &data);
	    			data ^= (-state ^ data) & (1 << 6); // Bit n will be set if x is 1, and cleared if x is 0. // number ^= (-x ^ number) & (1 << n);
	    			retval = tsc_i2c_write(device, reg, data);
	    			// Configure pin as an output
	    			reg = 7;
	    			// Read pin direction
	    			retval = tsc_i2c_read(device, reg, &data);
	    			// Set specific pin as an output direction (clear bit)
	    			data &= ~(1 << 6);
	    			retval = tsc_i2c_write(device, reg, data);
	    		}
	    		else if (state == RSP1461_EXT_PIN_Z){
	    			// Configure pin as an input
	    			reg = 7;
	    			// Read pin direction
	    			retval = tsc_i2c_read(device, reg, &data);
	    			// Set specific pin as an input direction (set bit)
	    			data |= 1 << 6;
	    			retval = tsc_i2c_write(device, reg, data);
	    		}
	    	}
	    	else if (index == 1){
	    		if((state == RSP1461_EXT_PIN_LOW) || (state == RSP1461_EXT_PIN_HIGH)) {
	    			// Write to the output the value
	    			reg = 3;
	    			// Read pin value
	    			retval = tsc_i2c_read(device, reg, &data);
	    			data ^= (-state ^ data) & (1 << 7); // Bit n will be set if x is 1, and cleared if x is 0. // number ^= (-x ^ number) & (1 << n);
	    			retval = tsc_i2c_write(device, reg, data);
	    			// Configure pin as an output
	    			reg = 7;
	    			// Read pin direction
	    			retval = tsc_i2c_read(device, reg, &data);
	    			// Set specific pin as an output direction (clear bit)
	    			data &= ~(1 << 7);
	    			retval = tsc_i2c_write(device, reg, data);
	    		}
	    		else if (state == RSP1461_EXT_PIN_Z){
	    			// Configure pin as an input
	    			reg = 7;
	    			// Read pin direction
	    			retval = tsc_i2c_read(device, reg, &data);
	    			// Set specific pin as an input direction (set bit)
	    			data |= 1 << 7;
	    			retval = tsc_i2c_write(device, reg, data);
	    		}
	    	}
	    	break;
	    // U107 PORT#0
	    case 5:
	    case 6:
	    	addr = 0x74;
	    	device = (bus & 7) << 29; device |= addr & 0x7f; device |= ((rs - 1) & 3) << 16; device |= ((ds - 1) & 3) << 18;
	    	if(index == 0){
	    		if((state == RSP1461_EXT_PIN_LOW) || (state == RSP1461_EXT_PIN_HIGH)) {
	    			// Write to the output the value
	    			reg = 2;
	    			data ^= (-state ^ data) & (1 << 6); // Bit n will be set if x is 1, and cleared if x is 0. // number ^= (-x ^ number) & (1 << n);
	    			retval = tsc_i2c_write(device, reg, data);
	    			// Configure pin as an output
	    			reg = 6;
	    			// Read pin direction
	    			retval = tsc_i2c_read(device, reg, &data);
	    			// Set specific pin as an output direction (clear bit)
	    			data &= ~(1 << 6);
	    			retval = tsc_i2c_write(device, reg, data);
	    		}
	    		else if (state == RSP1461_EXT_PIN_Z){
	    			// Configure pin as an input
	    			reg = 6;
	    			// Read pin direction
	    			retval = tsc_i2c_read(device, reg, &data);
	    			// Set specific pin as an input direction (set bit)
	    			data |= 1 << 6;
	    			retval = tsc_i2c_write(device, reg, data);
	    		}
	    	}
	    	else if (index == 1){
	    		if((state == RSP1461_EXT_PIN_LOW) || (state == RSP1461_EXT_PIN_HIGH)) {
	    			// Write to the output the value
	    			reg = 2;
	    			data ^= (-state ^ data) & (1 << 7); // Bit n will be set if x is 1, and cleared if x is 0. // number ^= (-x ^ number) & (1 << n);
	    			retval = tsc_i2c_write(device, reg, data);
	    			// Configure pin as an output
	    			reg = 6;
	    			// Read pin direction
	    			retval = tsc_i2c_read(device, reg, &data);
	    			// Set specific pin as an output direction (clear bit)
	    			data &= ~(1 << 7);
	    			retval = tsc_i2c_write(device, reg, data);
	    		}
	    		else if (state == RSP1461_EXT_PIN_Z){
	    			// Configure pin as an input
	    			reg = 6;
	    			// Read pin direction
	    			retval = tsc_i2c_read(device, reg, &data);
	    			// Set specific pin as an input direction (set bit)
	    			data |= 1 << 7;
	    			retval = tsc_i2c_write(device, reg, data);
	    		}
	    	}
	        break;
	    default :
			printf("Bad pin index ! \n");
			printf("Available id is 0 to 6 \n");
			return(-1);
		}

	return retval;
}

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * Function name : rsp1461_extension_get_pin_state
 * Prototype     : int
 * Parameters    : int index, int *state
 * Return        : status of operation
 *----------------------------------------------------------------------------
 * Description   : Extension board get pin state
 *
 *++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

int rsp1461_extension_get_pin_state(int index, int *state) {
	int retval   = 0;
	int addr     = 0;
	int bus      = 4;
	int reg      = 0;
	int rs       = 1;
	int ds       = 1;
	int device   = 0;
	int data     = 0;
	int data_org = 0;

switch(index){
	// U108 PORT#0
    case 0:
    case 1:
    	addr = 0x75;
    	reg = 6;
    	device = (bus & 7) << 29; device |= addr & 0x7f; device |= ((rs - 1) & 3) << 16; device |= ((ds - 1) & 3) << 18;
    	// Read pin direction
    	retval = tsc_i2c_read(device, reg, &data_org);
    	// Set specific pin as an input direction
    	data = data_org;
    	if(index == 0){
    		data |= 1 << 6;
    	}
    	else if (index == 1){
    		data |= 1 << 7;
    	}
    	retval = tsc_i2c_write(device, reg, data);
    	// Get pin value
    	reg = 0;
    	retval = tsc_i2c_read(device, reg, &data);
    	if(index == 0){
    		*state = (data & 0x40) >> 6;
    	}
    	else if (index == 1){
    		*state = (data & 0x80) >> 7;
    	}
    	// Re-set pin direction as original
    	reg = 6;
    	retval = tsc_i2c_write(device, reg, data_org);
        break;
    // U108 PORT#1
    case 2:
    	addr = 0x75;
    	reg = 7;
    	device = (bus & 7) << 29; device |= addr & 0x7f; device |= ((rs - 1) & 3) << 16; device |= ((ds - 1) & 3) << 18;
    	// Read pin direction
    	retval = tsc_i2c_read(device, reg, &data_org);
    	// Set specific pin as an input direction
    	data = data_org;
    	data |= 1 << 6;
    	retval = tsc_i2c_write(device, reg, data);
    	// Get pin value
    	reg = 1;
    	retval = tsc_i2c_read(device, reg, &data);
    	*state = (data & 0x40) >> 6;
    	// Re-set pin direction as original
    	reg = 7;
    	retval = tsc_i2c_write(device, reg, data_org);
        break;
    // U107 PORT#1
    case 3:
    case 4:
    	addr = 0x74;
    	reg = 7;
    	device = (bus & 7) << 29; device |= addr & 0x7f; device |= ((rs - 1) & 3) << 16; device |= ((ds - 1) & 3) << 18;
    	// Read pin direction
    	retval = tsc_i2c_read(device, reg, &data_org);
    	// Set specific pin as an input direction
    	data = data_org;
    	if(index == 3){
    		data |= 1 << 6;
    	}
    	else if (index == 4){
    		data |= 1 << 7;
    	}
    	retval = tsc_i2c_write(device, reg, data);
    	// Get pin value
    	reg = 1;
    	retval = tsc_i2c_read(device, reg, &data);
    	if(index == 3){
    		*state = (data & 0x40) >> 6;
    	}
    	else if (index == 4){
    		*state = (data & 0x80) >> 7;
    	}
    	// Re-set pin direction as original
    	reg = 7;
    	retval = tsc_i2c_write(device, reg, data_org);
        break;
    // U107 PORT#0
    case 5:
    case 6:
    	addr = 0x74;
    	reg = 6;
    	device = (bus & 7) << 29; device |= addr & 0x7f; device |= ((rs - 1) & 3) << 16; device |= ((ds - 1) & 3) << 18;
    	// Read pin direction
    	retval = tsc_i2c_read(device, reg, &data_org);
    	// Set specific pin as an input direction
    	data = data_org;
    	if(index == 5){
    		data |= 1 << 6;
    	}
    	else if (index == 6){
    		data |= 1 << 7;
    	}
    	retval = tsc_i2c_write(device, reg, data);
    	// Get pin value
    	reg = 0;
    	retval = tsc_i2c_read(device, reg, &data);
    	if(index == 5){
    		*state = (data & 0x40) >> 6;
    	}
    	else if (index == 6){
    		*state = (data & 0x80) >> 7;
    	}
    	// Re-set pin direction as original
    	reg = 6;
    	retval = tsc_i2c_write(device, reg, data_org);
        break;
    default :
		printf("Bad pin index ! \n");
		printf("Available id is 0 to 6 \n");
		return(-1);
	}
	return retval;
}

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * Function name : rsp1461_led_turn_on
 * Prototype     : int
 * Parameters    : rsp1461_led_t led_id
 * Return        : status of operation
 *----------------------------------------------------------------------------
 * Description   : On-board LEDs support functions ON
 *
 *++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

int rsp1461_led_turn_on(rsp1461_led_t led_id) {
	int retval = 0;
	int addr   = 0;
	int bus    = 4;
	int reg    = 0;
	int rs     = 1;
	int ds     = 1;
	int device = 0;
	int data   = 0;

// ----- U121 -----
	if ((int)led_id < 4){
		addr = 0x77;
		device = (bus & 7) << 29; device |= addr & 0x7f; device |= ((rs - 1) & 3) << 16; device |= ((ds - 1) & 3) << 18;
		// PORT#1 RSP1461_LED123_GREEN[0] & RSP1461_LED123_RED[1]
		if ((int)led_id < 2){
			reg = 1;
			retval = tsc_i2c_read(device, reg, &data);

			if((int)led_id == 0){
				data |= 1 << 6;
			}
			else if ((int)led_id == 1){
				data |= 1 << 7;
			}
			reg = 3;
			retval = tsc_i2c_write(device, reg, data);
		}
		// PORT#0 RSP1461_LED124_GREEN[2] & RSP1461_LED124_RED[3]
		else if ((int)led_id > 1){
			reg = 0;
			retval = tsc_i2c_read(device, reg, &data);

			if((int)led_id == 2){
				data |= 1 << 6;
			}
			else if ((int)led_id == 3){
				data |= 1 << 7;
			}
			reg = 2;
			retval = tsc_i2c_write(device, reg, data);
		}
	}
// ----- U120 -----
	else if ((int)led_id > 3){
		addr = 0x76;
		device = (bus & 7) << 29; device |= addr & 0x7f; device |= ((rs - 1) & 3) << 16; device |= ((ds - 1) & 3) << 18;
		// PORT#1 RSP1461_LED125_GREEN[4] & RSP1461_LED125_RED[5]
		if ((int)led_id < 6){
			reg = 1;
			retval = tsc_i2c_read(device, reg, &data);

			if((int)led_id == 4){
				data |= 1 << 6;
			}
			else if ((int)led_id == 5){
				data |= 1 << 7;
			}
			reg = 3;
			retval = tsc_i2c_write(device, reg, data);
		}
		// PORT#0 SP1461_LED126_GREEN[6] & RSP1461_LED126_RED[7]
		else if ((int)led_id > 5){
			reg = 0;
			retval = tsc_i2c_read(device, reg, &data);

			if((int)led_id == 6){
				data |= 1 << 6;
			}
			else if ((int)led_id == 7){
				data |= 1 << 7;
			}
			reg = 2;
			retval = tsc_i2c_write(device, reg, data);
		}
	}
	return retval;
}

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * Function name : rsp1461_led_turn_off
 * Prototype     : int
 * Parameters    : rsp1461_led_t led_id
 * Return        : status of operation
 *----------------------------------------------------------------------------
 * Description   : On-board LEDs support functions OFF
 *
 *++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

int rsp1461_led_turn_off(rsp1461_led_t led_id) {
	int retval = 0;
	int addr   = 0;
	int bus    = 4;
	int reg    = 0;
	int rs     = 1;
	int ds     = 1;
	int device = 0;
	int data   = 0;

// ----- U121 -----
	if ((int)led_id < 4){
		addr = 0x77;
		device = (bus & 7) << 29; device |= addr & 0x7f; device |= ((rs - 1) & 3) << 16; device |= ((ds - 1) & 3) << 18;
		// PORT#1 RSP1461_LED123_GREEN[0] & RSP1461_LED123_RED[1]
		if ((int)led_id < 2){
			reg = 1;
			retval = tsc_i2c_read(device, reg, &data);
			if((int)led_id == 0){
				data &= ~(1 << 6);
			}
			else if ((int)led_id == 1){
				data &= ~(1 << 7);
			}
			reg = 3;
			retval = tsc_i2c_write(device, reg, data);
		}
		// PORT#0 RSP1461_LED124_GREEN[2] & RSP1461_LED124_RED[3]
		else if ((int)led_id > 1){
			reg = 0;
			retval = tsc_i2c_read(device, reg, &data);
			if((int)led_id == 2){
				data &= ~(1 << 6);
			}
			else if ((int)led_id == 3){
				data &= ~(1 << 7);
			}
			reg = 2;
			retval = tsc_i2c_write(device, reg, data);
		}
	}
// ----- U120 -----
	else if ((int)led_id > 3){
		addr = 0x76;
		device = (bus & 7) << 29; device |= addr & 0x7f; device |= ((rs - 1) & 3) << 16; device |= ((ds - 1) & 3) << 18;
		// PORT#1 RSP1461_LED125_GREEN[4] & RSP1461_LED125_RED[5]
		if ((int)led_id < 6){
			reg = 1;
			retval = tsc_i2c_read(device, reg, &data);
			if((int)led_id == 4){
				data &= ~(1 << 6);
			}
			else if ((int)led_id == 5){
				data &= ~(1 << 7);
			}
			reg = 3;
			retval = tsc_i2c_write(device, reg, data);
		}
		// PORT#0 SP1461_LED126_GREEN[6] & RSP1461_LED126_RED[7]
		else if ((int)led_id > 5){
			reg = 0;
			retval = tsc_i2c_read(device, reg, &data);

			if((int)led_id == 6){
				data &= ~(1 << 6);
			}
			else if ((int)led_id == 7){
				data &= ~(1 << 7);
			}
			reg = 2;
			retval = tsc_i2c_write(device, reg, data);
		}
	}
	return retval;
}

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * Function name : rsp1461_sfp_status
 * Prototype     : int
 * Parameters    : rsp1461_sfp_id_t id, rsp1461_sfp_status_t *status
 * Return        : status of operation
 *----------------------------------------------------------------------------
 * Description   : SFP slots support functions status
 *
 *++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

int rsp1461_sfp_status(rsp1461_sfp_id_t id, uint8_t *status){
	int retval   = 0;
	int addr     = 0;
	int bus      = 4;
	int reg      = 0;
	int rs       = 1;
	int ds       = 1;
	int device   = 0;
	int data     = 0;

	switch(id){

	// U120 PORT#0 & PORT#1
	case 0:
	case 1:
    	addr = 0x76;
    	device = (bus & 7) << 29; device |= addr & 0x7f; device |= ((rs - 1) & 3) << 16; device |= ((ds - 1) & 3) << 18;
    	if (id == 0){
    		reg = 0; // PORT#0
    	}
    	else if (id == 1){
    		reg = 1; // PORT#1
    	}
    	retval = tsc_i2c_read(device, reg, &data);
    	*status = (data & 0x3f);
		break;

	// U121 PORT#0 & PORT#1
	case 2:
	case 3:
    	addr = 0x77;
    	device = (bus & 7) << 29; device |= addr & 0x7f; device |= ((rs - 1) & 3) << 16; device |= ((ds - 1) & 3) << 18;
    	if (id == 2){
    		reg = 0; // PORT#0
    	}
    	else if (id == 3){
    		reg = 1; // PORT#1
    	}
    	retval = tsc_i2c_read(device, reg, &data);
    	*status = (data & 0x3f);
		break;

	// U107 PORT#1
	case 4:
    	addr = 0x74;
    	device = (bus & 7) << 29; device |= addr & 0x7f; device |= ((rs - 1) & 3) << 16; device |= ((ds - 1) & 3) << 18;
    	reg = 1; // PORT#1
    	retval = tsc_i2c_read(device, reg, &data);
    	*status = (data & 0x3f);
		break;

	// U108 PORT#1 & PORT#0
	case 5:
	case 6:
    	addr = 0x75;
    	device = (bus & 7) << 29; device |= addr & 0x7f; device |= ((rs - 1) & 3) << 16; device |= ((ds - 1) & 3) << 18;
    	if (id == 5){
    		reg = 1; // PORT#1
    	}
    	else if (id == 6){
    		reg = 0; // PORT#0
    	}
    	retval = tsc_i2c_read(device, reg, &data);
    	*status = (data & 0x3f);
		break;

	default :
		printf("Bad index ! \n");
		printf("Available id is 0 to 6 \n");
		return(-1);
	}
return retval;
}

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * Function name : rsp1461_sfp_control
 * Prototype     : int
 * Parameters    : rsp1461_sfp_id_t id, rsp1461_sfp_status_t control
 * Return        : status of operation
 *----------------------------------------------------------------------------
 * Description   : SFP slots support functions control
 *
 *++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

int rsp1461_sfp_control(rsp1461_sfp_id_t id, int sfp_enable, int sfp_rate){
	int retval   = 0;
	int addr     = 0;
	int bus      = 4;
	int reg      = 0;
	int rs       = 1;
	int ds       = 1;
	int device   = 0;
	int data     = 0;

	switch(id){

	// U120 PORT#0 & PORT#1
	case 0:
	case 1:
	   	addr = 0x76;
	   	device = (bus & 7) << 29; device |= addr & 0x7f; device |= ((rs - 1) & 3) << 16; device |= ((ds - 1) & 3) << 18;
	   	if (id == 0){
	   		reg = 0; // PORT#0
	   		retval = tsc_i2c_read(device, reg, &data); // Get current state
	   		// Set enable
	   		data ^= (-sfp_enable ^ data) & (1 << 0); // Bit n will be set if x is 1, and cleared if x is 0. // number ^= (-x ^ number) & (1 << n);
	   		switch(sfp_rate) {
				// Set rate[00]
				case 0 :
					data ^= (-(0) ^ data) & (1 << 1);
					data ^= (-(0) ^ data) & (1 << 2);
					break;
				// Set rate[01]
				case 1  :
					data ^= (-(1) ^ data) & (1 << 1);
					data ^= (-(0) ^ data) & (1 << 2);
					break;
				// Set rate[10]
				case 2  :
					data ^= (-(0) ^ data) & (1 << 1);
					data ^= (-(1) ^ data) & (1 << 2);
					break;
				// Set rate[11]
				case 3  :
					data ^= (-(1) ^ data) & (1 << 1);
					data ^= (-(1) ^ data) & (1 << 2);
					break;
				default :
					NULL;
	   		}
	   		reg = 2;
	retval = tsc_i2c_write(device, reg, data);	// Set state
	   	}
	   	else if (id == 1){
	   		reg = 1; // PORT#1
	   		retval = tsc_i2c_read(device, reg, &data); // Get current state
	   		// Set enable
	   		data ^= (-sfp_enable ^ data) & (1 << 0); // Bit n will be set if x is 1, and cleared if x is 0. // number ^= (-x ^ number) & (1 << n);
	   		switch(sfp_rate) {
				// Set rate[00]
				case 0 :
					data ^= (-(0) ^ data) & (1 << 1);
					data ^= (-(0) ^ data) & (1 << 2);
					break;
				// Set rate[01]
				case 1  :
					data ^= (-(1) ^ data) & (1 << 1);
					data ^= (-(0) ^ data) & (1 << 2);
					break;
				// Set rate[10]
				case 2  :
					data ^= (-(0) ^ data) & (1 << 1);
					data ^= (-(1) ^ data) & (1 << 2);
					break;
				// Set rate[11]
				case 3  :
					data ^= (-(1) ^ data) & (1 << 1);
					data ^= (-(1) ^ data) & (1 << 2);
					break;
				default :
					NULL;
	   		}
	   		reg = 3;
	   		retval = tsc_i2c_write(device, reg, data);	// Set state
	   	}
		break;

	// U121 PORT#0 & PORT#1
	case 2:
	case 3:
	   	addr = 0x77;
		device = (bus & 7) << 29; device |= addr & 0x7f; device |= ((rs - 1) & 3) << 16; device |= ((ds - 1) & 3) << 18;
	   	if (id == 2){
	   		reg = 0; // PORT#0
	   		retval = tsc_i2c_read(device, reg, &data); // Get current state
	   		// Set enable
	   		data ^= (-sfp_enable ^ data) & (1 << 0); // Bit n will be set if x is 1, and cleared if x is 0. // number ^= (-x ^ number) & (1 << n);
	   		switch(sfp_rate) {
				// Set rate[00]
				case 0 :
					data ^= (-(0) ^ data) & (1 << 1);
					data ^= (-(0) ^ data) & (1 << 2);
					break;
				// Set rate[01]
				case 1  :
					data ^= (-(1) ^ data) & (1 << 1);
					data ^= (-(0) ^ data) & (1 << 2);
					break;
				// Set rate[10]
				case 2  :
					data ^= (-(0) ^ data) & (1 << 1);
					data ^= (-(1) ^ data) & (1 << 2);
					break;
				// Set rate[11]
				case 3  :
					data ^= (-(1) ^ data) & (1 << 1);
					data ^= (-(1) ^ data) & (1 << 2);
					break;
				default :
					NULL;
	   		}
	   		reg = 2;
	   		retval = tsc_i2c_write(device, reg, data);	// Set state
	   	}
	   	else if (id == 3){
	   		reg = 1; // PORT#1
	   		retval = tsc_i2c_read(device, reg, &data); // Get current state
	   		// Set enable
	   		data ^= (-sfp_enable ^ data) & (1 << 0); // Bit n will be set if x is 1, and cleared if x is 0. // number ^= (-x ^ number) & (1 << n);
	   		switch(sfp_rate) {
				// Set rate[00]
				case 0 :
					data ^= (-(0) ^ data) & (1 << 1);
					data ^= (-(0) ^ data) & (1 << 2);
					break;
				// Set rate[01]
				case 1  :
					data ^= (-(1) ^ data) & (1 << 1);
					data ^= (-(0) ^ data) & (1 << 2);
					break;
				// Set rate[10]
				case 2  :
					data ^= (-(0) ^ data) & (1 << 1);
					data ^= (-(1) ^ data) & (1 << 2);
					break;
				// Set rate[11]
				case 3  :
					data ^= (-(1) ^ data) & (1 << 1);
					data ^= (-(1) ^ data) & (1 << 2);
					break;
				default :
					NULL;
	   		}
	   		reg = 3;
	   		retval = tsc_i2c_write(device, reg, data);	// Set state
	   	}
		break;

	// U107 PORT#1
	case 4:
	   	addr = 0x74;
	   	device = (bus & 7) << 29; device |= addr & 0x7f; device |= ((rs - 1) & 3) << 16; device |= ((ds - 1) & 3) << 18;
	   	reg = 1; // PORT#1
	   	retval = tsc_i2c_read(device, reg, &data); // Get current state
   		// Set enable
   		data ^= (-sfp_enable ^ data) & (1 << 0); // Bit n will be set if x is 1, and cleared if x is 0. // number ^= (-x ^ number) & (1 << n);
   		switch(sfp_rate) {
			// Set rate[00]
			case 0 :
				data ^= (-(0) ^ data) & (1 << 1);
				data ^= (-(0) ^ data) & (1 << 2);
				break;
			// Set rate[01]
			case 1  :
				data ^= (-(1) ^ data) & (1 << 1);
				data ^= (-(0) ^ data) & (1 << 2);
				break;
			// Set rate[10]
			case 2  :
				data ^= (-(0) ^ data) & (1 << 1);
				data ^= (-(1) ^ data) & (1 << 2);
				break;
			// Set rate[11]
			case 3  :
				data ^= (-(1) ^ data) & (1 << 1);
				data ^= (-(1) ^ data) & (1 << 2);
				break;
			default :
				NULL;
   		}
   		reg = 3;
   		retval = tsc_i2c_write(device, reg, data);	// Set state
		break;

	// U108 PORT#1 & PORT#0
	case 5:
	case 6:
	   	addr = 0x75;
	   	device = (bus & 7) << 29; device |= addr & 0x7f; device |= ((rs - 1) & 3) << 16; device |= ((ds - 1) & 3) << 18;
	   	if (id == 5){
	   		reg = 1; // PORT#1
	   		retval = tsc_i2c_read(device, reg, &data); // Get current state
	   		// Set enable
	   		data ^= (-sfp_enable ^ data) & (1 << 0); // Bit n will be set if x is 1, and cleared if x is 0. // number ^= (-x ^ number) & (1 << n);
	   		switch(sfp_rate) {
				// Set rate[00]
				case 0 :
					data ^= (-(0) ^ data) & (1 << 1);
					data ^= (-(0) ^ data) & (1 << 2);
					break;
				// Set rate[01]
				case 1  :
					data ^= (-(1) ^ data) & (1 << 1);
					data ^= (-(0) ^ data) & (1 << 2);
					break;
				// Set rate[10]
				case 2  :
					data ^= (-(0) ^ data) & (1 << 1);
					data ^= (-(1) ^ data) & (1 << 2);
					break;
				// Set rate[11]
				case 3  :
					data ^= (-(1) ^ data) & (1 << 1);
					data ^= (-(1) ^ data) & (1 << 2);
					break;
				default :
					NULL;
	   		}
	   		reg = 3;
	   		retval = tsc_i2c_write(device, reg, data);	// Set state
	   	}
	   	else if (id == 6){
	   		reg = 0; // PORT#0
	   		retval = tsc_i2c_read(device, reg, &data); // Get current state
	   		// Set enable
	   		data ^= (-sfp_enable ^ data) & (1 << 0); // Bit n will be set if x is 1, and cleared if x is 0. // number ^= (-x ^ number) & (1 << n);
	   		switch(sfp_rate) {
				// Set rate[00]
				case 0 :
					data ^= (-(0) ^ data) & (1 << 1);
					data ^= (-(0) ^ data) & (1 << 2);
					break;
				// Set rate[01]
				case 1  :
					data ^= (-(1) ^ data) & (1 << 1);
					data ^= (-(0) ^ data) & (1 << 2);
					break;
				// Set rate[10]
				case 2  :
					data ^= (-(0) ^ data) & (1 << 1);
					data ^= (-(1) ^ data) & (1 << 2);
					break;
				// Set rate[11]
				case 3  :
					data ^= (-(1) ^ data) & (1 << 1);
					data ^= (-(1) ^ data) & (1 << 2);
					break;
				default :
					NULL;
	   		}
	   		reg = 2;
	   		retval = tsc_i2c_write(device, reg, data);	// Set state
	   	}
		break;

	default :
		printf("Bad index ! \n");
		printf("Available id is 0 to 6 \n");
		return(-1);
	}
	return retval;
}
