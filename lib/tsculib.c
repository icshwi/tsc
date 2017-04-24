/*=========================< begin file & file header >=======================
 *  References
 *  
 *    filename : tsculib.c
 *    author   : JFG
 *    company  : IOxOS
 *    creation : 
 *    version  : 0.0.1
 *
 *----------------------------------------------------------------------------
 *  Description
 *
 *     That library contains a set of function to access the IDT PCI Express
 *     switch
 *
 *----------------------------------------------------------------------------
 *  Copyright Notice
 *  
 *    Copyright and all other rights in this document are reserved by 
 *    IOxOS Technologies SA. This documents contains proprietary information    
 *    and is supplied on express condition that it may not be disclosed, 
 *    reproduced in whole or in part, or used for any other purpose other
 *    than that for which it is supplies, without the written consent of  
 *    IOxOS Technologies SA                                                        
 *
 *----------------------------------------------------------------------------
 *  Change History
 *
 *
 *=============================< end file header >============================*/

#ifndef lint
static char rcsid[] = "$Id: tsculib.c,v 1.15 2016/03/02 09:44:14 ioxos Exp $";
#endif
typedef long dma_addr_t;

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

#include "../include/tscioctl.h"
#include "../include/i2c-dev.h"

char tsc_lib_version[] = "1.00";
int tsc_fd         = -1;
int tsc_fd_io      = -1;
int tsc_fd_central = -1;
static char tsc_drv_name[16] = {0,};
static char tsc_drv_version[16] = {0,};
static unsigned short tsc_vendor_id;
static unsigned short tsc_device_id;

char *
tsc_rcsid()
{
  return( rcsid);
}

long long
tsc_swap_64( long long data)
{
  char ci[8];
  char co[8];

  *(long long *)ci = data;
  co[0] = ci[7];
  co[1] = ci[6];
  co[2] = ci[5];
  co[3] = ci[4];
  co[4] = ci[3];
  co[5] = ci[2];
  co[6] = ci[1];
  co[7] = ci[0];

  return( *(long long *)co);
}

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
 * Prototype     : void
 * Parameters    : none
 * Return        : device ID : central = 0, io = 1
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
 * Prototype     : void
 * Parameters    : none
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
 * Description   : return a pointer to the driver versionidentifier string
 *++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
char *
tsc_get_drv_version()
{
  return( tsc_drv_version);
}

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * Function name : tsc_get_vendor_id
 * Prototype     : char *
 * Parameters    : none
 * Return        : IFC1211 vendor id
 *----------------------------------------------------------------------------
 * Description   : return a pointer to the driver versionidentifier string
 *++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
unsigned short
tsc_get_vendor_id()
{
  return( tsc_vendor_id);
}

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * Function name : tsc_get_device_id
 * Prototype     : char *
 * Parameters    : none
 * Return        : IFC1211 device id
 *----------------------------------------------------------------------------
 * Description   : return a pointer to the driver versionidentifier string
 *++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
unsigned short
tsc_get_device_id()
{
  return( tsc_device_id);
}

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * Function name : tsc_init
 * Prototype     : void
 * Parameters    : device name
 * Return        : file descritor for tsc control device
 *----------------------------------------------------------------------------
 * Description   : return allocated resources to OS
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
 * Description   : return allocated resources to OS
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
 * Description   : write the IFC1211 CSR pointed by idx with the content of
 *                 data_p. The status of the write operation is returned.
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
 * Description   : read the content of the IFC1211 CSR pointed by idx and return
 *                 its current value in data_p. The status of the read is
 *                 returned
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
 * Description   : set bit in the IFC1211 CSR pointed by idx with the content of
 *                 data_p. The status of the write operation is returned.
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
 * Description   : write the IFC1211 SMON register pointed by idx with the content
 *                 of data_p. The status of the write operation is returned.
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
 * Description   : read the content of the IFC1211 SMON register pointed by idx
 *                 and return its current value in data_p. The status of the
 *                 operation is returned
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
 * Description   : write the IFC1211 PON register pointed by idx with the content
 *                 of data_p. The status of the write operation is returned.
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
 * Description   : read the content of the IFC1211 PON register pointed by idx
 *                 and return its current value in data_p. The status of the
 *                 operation is returned
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
 * Description   : write the IFC1211 PCIEP register pointed by idx with the content
 *                 of data_p. The status of the write operation is returned.
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
 * Description   : read the content of the IFC1211 PCIEP register pointed by idx
 *                 and return its current value in data_p. The status of the
 *                 operation is returned
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
 * Parameters    : remote adrress
 *                 data buffer pointer
 *                 transfer size (in bytes)
 *                 transfer mode (am,ds,..)
 * Return        : status of read operation
 *----------------------------------------------------------------------------
 * Description   : copy <len> bytes from buffer <buf> to remote space 
 *                 the adressing mode and data size are specified in <mode>
 *++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
int
tsc_write_blk( ulong rem_addr,
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
 * Parameters    : remote adrress
 *                 data buffer pointer
 *                 transfer size (in bytes)
 *                 transfer mode (am,ds,..)
 * Return        : status of read operation
 *----------------------------------------------------------------------------
 * Description   : 
 *++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
int
tsc_read_blk( ulong rem_addr,
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
 * Function name : tsc_write_blk
 * Prototype     : int
 * Parameters    : remote adrress
 *                 data buffer pointer
 *                 transfer size (in bytes)
 *                 transfer mode (am,ds,..)
 * Return        : status of read operation
 *----------------------------------------------------------------------------
 * Description   : copy <len> bytes from buffer <buf> to remote space 
 *                 the adressing mode and data size are specified in <mode>
 *++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
int
tsc_write_loop( ulong rem_addr,
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
 * Function name : tsc_read_blk
 * Prototype     : int
 * Parameters    : remote adrress
 *                 data buffer pointer
 *                 transfer size (in bytes)
 *                 transfer mode (am,ds,..)
 * Return        : status of read operation
 *----------------------------------------------------------------------------
 * Description   : 
 *++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
int
tsc_read_loop( ulong rem_addr,
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
 * Parameters    : remote adrress
 *                 data  pointer
 *                 transfer mode (am,ds,..)
 * Return        : status of read operation
 *----------------------------------------------------------------------------
 * Description   : perform a single write in a remote space (SHM)
 *++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
int
tsc_write_sgl( ulong rem_addr,
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
 * Parameters    : remote adrress
 *                 data pointer
 *                 transfer mode (am,ds,..)
 * Parameters    : register index
 *                 data pointer
 * Return        : status of read operation
 *----------------------------------------------------------------------------
 * Description   : perform a single read in a remote space (SHM)
 *++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
int
tsc_read_sgl( ulong rem_addr,
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
 * Return        : status of read operation
 *----------------------------------------------------------------------------
 * Description   : copy <len> bytes from buffer <buf> to SHM address space
 *++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
int tsc_shm_write(uint shm_addr, char *buf, int len, int ds, int swap){
	struct tsc_ioctl_rdwr rdwr;
	int retval;

	if(len < 0) return(-EINVAL);
	if(tsc_fd < 0) return(-EBADF);
	rdwr.rem_addr = (ulong)shm_addr;
	rdwr.buf = buf;
	rdwr.len = len;
	rdwr.m.space = RDWR_SPACE_SHM;
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
 * Return        : status of read operation
 *----------------------------------------------------------------------------
 * Description   : Read a block of data from SHM address space
 *++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

int tsc_shm_read(uint shm_addr, char *buf, int len, int ds, int swap){
	struct tsc_ioctl_rdwr rdwr;
	int retval;

	if(len < 0) return(-EINVAL);
	if(tsc_fd < 0) return(-EBADF);
	rdwr.rem_addr = (ulong)shm_addr;
	rdwr.buf = buf;
	rdwr.len = len;
	rdwr.m.space = RDWR_SPACE_SHM;
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
 * Return        : status of read operation
 *----------------------------------------------------------------------------
 * Description   : read the content 
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
 * Description   : read the content 
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
 * Return        : status of read operation
 *----------------------------------------------------------------------------
 * Description   : read the content 
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
 * Return        : status of read operation
 *----------------------------------------------------------------------------
 * Description   : read the content 
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
 * Return        : status of read operation
 *----------------------------------------------------------------------------
 * Description   : read the content 
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
 * Description   : read the content 
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
 * Return        : status of set operation
 *----------------------------------------------------------------------------
 * Description   : map kernel buffer in user's space
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
 * Return        : status of set operation
 *----------------------------------------------------------------------------
 * Description   : unmap kernel buffer
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
 * Return        : status of set operation
 *----------------------------------------------------------------------------
 * Description   : allocate a kernel buffer suitable for DMA operation
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
 * Return        : status of set operation
 *----------------------------------------------------------------------------
 * Description   : allocate a kernel buffer suitable for DMA operation
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
 * Return        : status of set operation
 *----------------------------------------------------------------------------
 * Description   : map kernel buffer in user's space
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
 * Return        : status of set operation
 *----------------------------------------------------------------------------
 * Description   : unmap kernel buffer
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
 * Return        : status of the read operation
 *----------------------------------------------------------------------------
 * Description   : copy data to a kernel buffer
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
 * Parameters    : 
 * Return        : status of DMA operation
 *----------------------------------------------------------------------------
 * Description   : wait for DMA transfer to complete
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
 * Return        : status of set operation
 *----------------------------------------------------------------------------
 * Description   : request DMA chhannel allocation
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
 * Parameters    : pointer to mapping control structure
 * Return        : status of set operation
 *----------------------------------------------------------------------------
 * Description   : free allocated DMA channel
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
 * Parameters    : pointer to mapping control structure
 * Return        : status of set operation
 *----------------------------------------------------------------------------
 * Description   : reset the IRQ mechanism
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
 * Parameters    : pointer to 4 byte string to hold SFLASH ID
 * Return        : status of set operation
 *----------------------------------------------------------------------------
 * Description   : read SFLASH identifiers (3 bytes)
 *                 id[0]   -> manufacturer ID
 *                 id[1,2] -> devices ID (15 bits)] 
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
 * Parameters    : pointer to cha to hold current value of SR register
 * Return        : status of set operation
 *----------------------------------------------------------------------------
 * Description   : read SFLASH identifiers (3 bytes)
 *                 id[0]   -> manufacturer ID
 *                 id[1,2] -> devices ID (15 bits)] 
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
 * Return        : status of set operation
 *----------------------------------------------------------------------------
 * Description   : write SFLASH status register
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
 * Return        : status of set operation
 *----------------------------------------------------------------------------
 * Description   : read SFLASH identifiers (3 bytes)
 *                 id[0]   -> manufacturer ID
 *                 id[1,2] -> devices ID (15 bits)] 
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
 * Parameters    : offset in SFLASH from where to read
 *                 pointer to destination buffer
 *                 number of byte to read
 * Return        : status of set operation
 *----------------------------------------------------------------------------
 * Description   : read SFLASH identifiers (3 bytes)
 *                 id[0]   -> manufacturer ID
 *                 id[1,2] -> devices ID (15 bits)] 
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
 * Return        : status of set operation
 *----------------------------------------------------------------------------
 * Description   : start the IFC1211 global timer
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
 * Return        : status of set operation
 *----------------------------------------------------------------------------
 * Description   : restart the IFC1211 global timer
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
 * Return        : status of set operation
 *----------------------------------------------------------------------------
 * Description   : stop the IFC1211 global timer
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
 * Return        : status of set operation
 *----------------------------------------------------------------------------
 * Description   : return the current value og the IFC1211 global timer
 *                 in the tsc_time data structure
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
 * Parameters    : FIFO index ( 0 -> 7)
 *                 operating mode
 * Return        : status of set operation
 *----------------------------------------------------------------------------
 * Description   : initialize FIFO according to mode parameter
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
 * Function name : tsc_fifo_
 * Prototype     : int
 * Parameters    : FIFO index ( 0 -> 7)
 *                 pointer to hold current status (uint *)
 * Return        : status of set operation
 *----------------------------------------------------------------------------
 * Description   : returns the current status of FIFO idx
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
  if(! retval)
  {
    retval = fifo.sts;
  }
  return(retval);
}

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * Function name : tsc_fifo_clear
 * Prototype     : int
 * Parameters    : FIFO index ( 0 -> 7)
 *                 pointer to hold current status (uint *)
 * Return        : status of set operation
 *----------------------------------------------------------------------------
 * Description   : clear FIFO idx
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
 * Parameters    : FIFO index ( 0 -> 7)
 *                 pointer to hold status (uint *)
 *                 timeout
 * Return        : status of set operation
 *----------------------------------------------------------------------------
 * Description   : wait for FIFO emplty -> not empty
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
 * Parameters    : FIFO index ( 0 -> 7)
 *                 pointer to hold status (uint *)
 *                 timeout
 * Return        : status of set operation
 *----------------------------------------------------------------------------
 * Description   : wait for FIFO full -> not full
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
 * Parameters    : FIFO index ( 0 -> 7)
 *                 point to buffer to hold data (uint *)
 *                 word count
 *                 pointer to hold status (uint *)
 *                 timeout
 * Return        : status of set operation
 *----------------------------------------------------------------------------
 * Description   : read up to <wcnt> data from FIFO <idx> and store in buffer
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
 * Parameters    : FIFO index ( 0 -> 7)
 *                 point to buffer to hold data (uint *)
 *                 word count
 *                 pointer to hold status (uint *)
 *                 timeout
 * Return        : status of set operation
 *----------------------------------------------------------------------------
 * Description   : write up to <wcnt> data to FIFO <idx> from  buffer pointed
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
 * Function name : tsc_i2c
 * Prototype     : int
 * Parameters    : I2C device
 *                 register index
 *                 data
 * Return        : status of read operation
 *----------------------------------------------------------------------------
 * Description   : read register <reg> of I2C device <dev> and returns the
 *                 result in <data>
 *                 by <data>.
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

int
tsc_i2c_read( uint dev,
	      uint reg,
	      uint *data)
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
  return(0);
}

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * Function name : tsc_i2c_cmd
 * Prototype     : int
 * Parameters    : I2C device
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

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * Function name : tsc_i2c_write
 * Prototype     : int
 * Parameters    : I2C device
 *                 register index
 *                 data
 * Return        : status of read operation
 *----------------------------------------------------------------------------
 * Description   : write  <data> to register <reg> of I2C device <dev>
 *                 return current status in <sts>
 *                 
 *++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
int
tsc_i2c_write( uint dev,
	       uint reg,
	       uint data)
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
  return(0);
}

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * Function name : tsc_i2c_reset
 * Prototype     : int
 * Parameters    : I2C device
 * Return        : status of read operation
 *----------------------------------------------------------------------------
 * Description   : reset I2C controller
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
 *
 *++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
int tsc_semaphore_release(uint idx){
	struct tsc_ioctl_semaphore semaphore;
	int retval = 0;

	semaphore.idx = idx;
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
 *
 *++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
int tsc_semaphore_get(uint idx, uint *tag){
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
 * Description   : change between CENTRAL and IO device if available
 *
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
