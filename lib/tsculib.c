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
 *  Copyright (C) IOxOS Technologies SA <ioxos@ioxos.ch>
 *  Copyright (C) 2019  European Spallation Source ERIC
 *
 *    THIS SOFTWARE IS PROVIDED BY THE AUTHOR AND CONTRIBUTORS ``AS IS'' AND
 *    ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 *    IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 *    ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR OR CONTRIBUTORS BE LIABLE
 *    FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 *    DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 *    OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 *    HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 *    LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 *    OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 *    SUCH DAMAGE.
 *
 *    GPL license :
 *    This program is free software; you can redistribute it and/or
 *    modify it under the terms of the GNU General Public License
 *    as published by the Free Software Foundation; either version 2
 *    of the License, or (at your option) any later version.
 *
 *    This program is distributed in the hope that it will be useful,
 *    but WITHOUT ANY WARRANTY; without even the implied warranty of
 *    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *    GNU General Public License for more details.
 *
 *    You should have received a copy of the GNU General Public License
 *    along with this program; if not, write to the Free Software
 *    Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
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

char tsc_lib_version[] = "2.00";
static char tsc_drv_name[16] = {0,};
static char tsc_drv_version[16] = {0,};
static unsigned short tsc_vendor_id;
static unsigned short tsc_device_id;
static int  tsc_has_axi = -1;
static int  tsc_axi_cap = 0;

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
 * Function name : tsc_device_init
 * Prototype     : int
 * Parameters    : none
 * Return        : 1 = success,  0 < error
 *----------------------------------------------------------------------------
 * Description   : Initialize device specific internal variables
 *
 *++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

/* prototype definition, as function declared further down in file */
int tsc_csr_read(int, int, int *); 

static int tsc_device_init(int fd)
{
  int tmp, ret;

  /* retrieve Vendor_ID & Device_ID of current selected device */
  ioctl(fd, TSC_IOCTL_ID_VENDOR, &tsc_vendor_id);
  ioctl(fd, TSC_IOCTL_ID_DEVICE, &tsc_device_id);
  
  /* verify present of AXI-4 Bridge */
  tsc_has_axi = 0;
 
  /* read AXI-4 Bridge Signature */
  ret = tsc_csr_read(fd, TSC_CSR_AXI4_SIGN, &tmp);
  if (ret < 0) return ret;
   
  /* check AXI-4 Bridge Signature */
  if (((tmp >> 16) & 0xFFFF) == TSC_AXI4_SIGN)
  {
    /* read AXI-4 Bridge configuration */
    ret = tsc_csr_read(fd, TSC_CSR_AXI4_CFG, &tsc_axi_cap);
    printf("AXI-4 Bridge Configuration ret: %d, %8x\n", ret, tsc_axi_cap);
    if (ret < 0) return ret;
  
    tsc_has_axi = 1;
    printf("AXI-4 bridge detected...\n");
  }
  return 1;
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
tsc_init(uint32_t card)
{
  char device_name[64];
  int fd;

  sprintf(device_name, "%s%d", "/dev/bus/bridge/tsc_ctl_central", card);
  fd = open(device_name, O_RDWR);

  if(fd >= 0)
  {
    ioctl(fd, TSC_IOCTL_ID_NAME, tsc_drv_name);
    ioctl(fd, TSC_IOCTL_ID_VERSION, tsc_drv_version);
    ioctl(fd, TSC_IOCTL_ID_VENDOR, &tsc_vendor_id);
    ioctl(fd, TSC_IOCTL_ID_DEVICE, &tsc_device_id);
  }

 printf("Initializing the device...\n");   
 tsc_device_init(fd);

  return(fd);
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
tsc_exit(int fd)
{
  int retval;

  retval = -EBADF;
  if(fd >= 0)
  {
    retval = close(fd);
  }
  fd = -1;

  return(retval);
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
tsc_csr_write(int fd, int idx,
	       int *data_p)
{
  struct tsc_ioctl_csr_op csr_op;

  if(fd < 0) return(-EBADF);
  csr_op.offset = idx;
  csr_op.data = *data_p;
  return(ioctl(fd, TSC_IOCTL_CSR_WR, &csr_op));
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
tsc_csr_read(int fd, int idx,
	      int *data_p)
{
  struct tsc_ioctl_csr_op csr_op;
  int retval;

  if(fd < 0) return(-EBADF);
  csr_op.offset = idx;
  csr_op.data = -1;
  retval = ioctl(fd, TSC_IOCTL_CSR_RD, &csr_op);
  *data_p = csr_op.data;

  return(retval);
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
tsc_csr_set(int fd, int idx,
	     int *data_p)
{
  struct tsc_ioctl_csr_op csr_op;

  if(fd < 0) return(-EBADF);
  csr_op.offset = idx;
  csr_op.data = *data_p;
  csr_op.mask = *data_p;
  return(ioctl(fd, TSC_IOCTL_CSR_WRm, &csr_op));
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
tsc_smon_write(int fd, int idx,
	       int *data_p)
{
  struct tsc_ioctl_csr_op smon_op;

  if(fd < 0) return(-EBADF);
  smon_op.offset = idx;
  smon_op.data = *data_p;
  return(ioctl(fd, TSC_IOCTL_CSR_SMON_WR, &smon_op));
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
tsc_smon_read(int fd, int idx,
	      int *data_p)
{
  struct tsc_ioctl_csr_op smon_op;
  int retval;

  if(fd < 0) return(-EBADF);
  smon_op.offset = idx;
  smon_op.data = -1;
  retval = ioctl(fd, TSC_IOCTL_CSR_SMON_RD, &smon_op);
  *data_p = smon_op.data;

  return(retval);
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
tsc_pon_write(int fd, int idx,
	       int *data_p)
{
  struct tsc_ioctl_csr_op pon_op;

  if(fd < 0) return(-EBADF);
  pon_op.offset = idx;
  pon_op.data = *data_p;
  return(ioctl(fd, TSC_IOCTL_CSR_PON_WR, &pon_op));
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
tsc_pon_read(int fd, int idx,
	      int *data_p)
{
  struct tsc_ioctl_csr_op pon_op;
  int retval;

  if(fd < 0) return(-EBADF);
  pon_op.offset = idx;
  pon_op.data = -1;
  retval = ioctl(fd, TSC_IOCTL_CSR_PON_RD, &pon_op);
  *data_p = pon_op.data;

  return(retval);
}

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * Function name : tsc_axi_get_cap
 * Prototype     : int
 * Parameters    : variable for holding 'AXI-4 capabilities'
 * Return        : 0 = no AXI-4 Bridge available, 
 *                 1 = AXI-4 Bridge capabilities available
 *----------------------------------------------------------------------------
 * Description   : gives information about AXI-4 capabilities and availablity
 *++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

int tsc_axi_get_cap(int *cap)
{
  if (tsc_has_axi && cap != NULL)
  {
    (*cap) = tsc_axi_cap;
  }
  return tsc_has_axi;
}


/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * Function name : tsc_axil_write
 * Prototype     : int
 * Parameters    : AXI-4 Lite 32-bit address
 *                 AXI-4 Lite Write STRobe
 *                 AXI-4 Lite PROTection bits
 *                 data pointer
 * Return        : status of write operation
 *----------------------------------------------------------------------------
 * Description   : do a write at address addr with data_p on AXI-4 Lite Master.
 *                 The status of the write operation is returned.
 *++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

int tsc_axil_write(int fd, int addr, int wstrb, int prot, int *data_p)
{
  int tmp;
  int timeout = 10000;
  int ret;

  /* no AXI-4 Bridge -> exit */
  if (tsc_has_axi != 1)                               return -1;
  
  /* AXI-4 Lite Master disabled -> exit */
  if ((tsc_axi_cap & TSC_AXI4_CFG_AXIL_MASTER) == 0)  return -1;
 
  tmp = (TSC_AXIL_ADD_ADDR(addr) | TSC_AXIL_ADD_PROT(prot));
  ret = tsc_csr_write(fd, TSC_CSR_AXIL_ADD, &tmp);
  if (ret < 0) return ret;
  
  if (data_p != NULL)
  {
    ret = tsc_csr_write(fd, TSC_CSR_AXIL_DAT, data_p);
    if (ret < 0) return ret;
  }
  tmp = (TSC_AXIL_CTL_GO | TSC_AXIL_CTL_WSTRB(wstrb) | TSC_AXIL_CTL_ADDR(addr));
  ret = tsc_csr_write(fd, TSC_CSR_AXIL_CTL, &tmp);
  if (ret < 0) return ret;
  
  while (timeout > 0)
  {
    ret = tsc_csr_read(fd, TSC_CSR_AXIL_CTL, &tmp);
    if (ret < 0) return ret;
    
    if ((tmp & TSC_AXIL_CTL_BUSY)!=TSC_AXIL_CTL_BUSY)
      break;
    timeout --;
  }
  return (((timeout==0) || (tmp & (TSC_AXIL_CTL_TIMEOUT|TSC_AXIL_CTL_ERROR)))?-1:0);
}


/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * Function name : tsc_axil_read
 * Prototype     : int
 * Parameters    : AXI-4 Lite 32-bit address
 *                 AXI-4 Lite PROTection bits
 *                 data pointer
 * Return        : status of write operation
 *----------------------------------------------------------------------------
 * Description   : do a read at address addr and store data into data_p on 
 *                 AXI-4 Lite Master.
 *                 The status of the read operation is returned.
 *++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

int tsc_axil_read(int fd, int addr, int prot, int *data_p)
{
  int tmp;
  int timeout = 10000;
  int ret;
  
  /* no AXI-4 Bridge -> exit */
  if (tsc_has_axi != 1)                               return -1;
  
  /* AXI-4 Lite Master disabled -> exit */
  if ((tsc_axi_cap & TSC_AXI4_CFG_AXIL_MASTER) == 0)  return -1;

  tmp = (TSC_AXIL_ADD_ADDR(addr) | TSC_AXIL_ADD_PROT(prot));
  ret = tsc_csr_write(fd, TSC_CSR_AXIL_ADD, &tmp);
  if (ret < 0) return ret;

  tmp = (TSC_AXIL_CTL_GO | TSC_AXIL_CTL_RnW | TSC_AXIL_CTL_ADDR(addr));
  ret = tsc_csr_write(fd, TSC_CSR_AXIL_CTL, &tmp);
  if (ret < 0) return ret;

  while (timeout > 0)
  {
    ret = tsc_csr_read(fd, TSC_CSR_AXIL_CTL, &tmp);
    if (ret < 0) return ret;

    if ((tmp & TSC_AXIL_CTL_BUSY)!=TSC_AXIL_CTL_BUSY)
    {
      if (data_p != NULL)
      {
        ret = tsc_csr_read(fd, TSC_CSR_AXIL_DAT, data_p);
        if (ret < 0) return ret;
      }
      break;
    }
    timeout --;
  }
  return (((timeout==0) || (tmp & (TSC_AXIL_CTL_TIMEOUT|TSC_AXIL_CTL_ERROR)))?-1:0);
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
tsc_pciep_write(int fd, int idx,
	         int *data_p)
{
  struct tsc_ioctl_csr_op pciep_op;

  if(fd < 0) return(-EBADF);
  pciep_op.offset = idx;
  pciep_op.data = *data_p;
  return(ioctl(fd, TSC_IOCTL_CSR_PCIEP_WR, &pciep_op));
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
tsc_pciep_read(int fd, int idx,
	      int *data_p)
{
  struct tsc_ioctl_csr_op pciep_op;
  int retval;

  if(fd < 0) return(-EBADF);
  pciep_op.offset = idx;
  pciep_op.data = -1;
  retval = ioctl(fd, TSC_IOCTL_CSR_PCIEP_RD, &pciep_op);
  *data_p = pciep_op.data;

  return(retval);
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
tsc_write_blk(int fd, uint64_t rem_addr,
	       char *buf,
	        int len,
	        uint mode)
{
  struct tsc_ioctl_rdwr rdwr;
  int retval;

  if( len <= 0) return(-EINVAL);
  if(fd < 0) return(-EBADF);
  rdwr.rem_addr = rem_addr;
  rdwr.buf = buf;
  rdwr.len = len;
  rdwr.mode = mode;
  retval = ioctl(fd, TSC_IOCTL_RDWR_WRITE, &rdwr);

  return(retval);
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
tsc_read_blk(int fd, uint64_t rem_addr,
	      char *buf,
	      int len,
	      uint mode)
{
  struct tsc_ioctl_rdwr rdwr;
  int retval;

  if( len <= 0) return(-EINVAL);
  if(fd < 0) return(-EBADF);
  rdwr.rem_addr = rem_addr;
  rdwr.buf = buf;
  rdwr.len = len;
  rdwr.mode = mode;
  retval = ioctl(fd, TSC_IOCTL_RDWR_READ, &rdwr);

  return(retval);
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
tsc_write_loop(int fd, uint64_t rem_addr,
	        char *buf,
	        int len,
	        uint mode)
{
  struct tsc_ioctl_rdwr rdwr;
  int retval;

  if( len <= 0) return(-EINVAL);
  if(fd < 0) return(-EBADF);
  rdwr.rem_addr = rem_addr;
  rdwr.buf = buf;
  rdwr.len = len | RDWR_LOOP;
  rdwr.mode = mode;
  retval = ioctl(fd, TSC_IOCTL_RDWR_WRITE, &rdwr);

  return(retval);
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
tsc_read_loop(int fd, uint64_t rem_addr,
	      char *buf,
	      int len,
	      uint mode)
{
  struct tsc_ioctl_rdwr rdwr;
  int retval;

  if( len <= 0) return(-EINVAL);
  if(fd < 0) return(-EBADF);
  rdwr.rem_addr = rem_addr;
  rdwr.buf = buf;
  rdwr.len = len | RDWR_LOOP;
  rdwr.mode = mode;
  retval = ioctl(fd, TSC_IOCTL_RDWR_READ, &rdwr);

  return(retval);
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
tsc_write_sgl(int fd, uint64_t rem_addr,
	       char *data_p,
	       uint mode)
{
  struct tsc_ioctl_rdwr rdwr;
  int retval;

  if(fd < 0) return(-EBADF);
  rdwr.rem_addr = rem_addr;
  rdwr.buf = data_p;
  rdwr.len = 0;
  rdwr.mode = mode;
  retval = ioctl(fd, TSC_IOCTL_RDWR_WRITE, &rdwr);

  return(retval);
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
tsc_read_sgl(int fd, uint64_t rem_addr,
	      char *data_p,
	      uint mode)
{
  struct tsc_ioctl_rdwr rdwr;
  int retval;

  if(fd < 0) return(-EBADF);
  rdwr.rem_addr = rem_addr;
  rdwr.buf = data_p;
  rdwr.len = 0;
  rdwr.mode = mode;
  retval = ioctl(fd, TSC_IOCTL_RDWR_READ, &rdwr);

  return(retval);
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

int tsc_shm_write(int fd, uint shm_addr, char *buf, int len, int ds, int swap, int mem){
        struct tsc_ioctl_rdwr rdwr = { .rem_addr=0, .buf="", .len=0, .m={.ads='\0', .space='\0', .swap='\0', .am='\0'}};
	int retval;

	if(len < 0) return(-EINVAL);
	if(fd < 0) return(-EBADF);
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
	rdwr.m.ads = (char)RDWR_MODE_SET_DS(rdwr.m.ads,(char)ds);
	rdwr.m.swap = (char)swap;
	retval = ioctl(fd, TSC_IOCTL_RDWR_WRITE, &rdwr);

	return(retval);
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

int tsc_shm_read(int fd, uint shm_addr, char *buf, int len, int ds, int swap, int mem){
        struct tsc_ioctl_rdwr rdwr = { .rem_addr=0, .buf="", .len=0, .m={.ads='\0', .space='\0', .swap='\0', .am='\0'}};
	int retval;

	if(len < 0) return(-EINVAL);
	if(fd < 0) return(-EBADF);
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
	rdwr.m.ads = (char)RDWR_MODE_SET_DS(rdwr.m.ads,(char)ds);
	rdwr.m.swap = (char)swap;
	retval = ioctl(fd, TSC_IOCTL_RDWR_READ, &rdwr);

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

int tsc_usr_write(int fd, uint usr_addr, char *buf, int len, int ds, int swap, int mem){
        struct tsc_ioctl_rdwr rdwr = { .rem_addr=0, .buf="", .len=0, .m={.ads='\0', .space='\0', .swap='\0', .am='\0'}};
	int retval;

	if(len < 0) return(-EINVAL);
	if(fd < 0) return(-EBADF);
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
	rdwr.m.ads = (char)RDWR_MODE_SET_DS(rdwr.m.ads,(char)ds);
	rdwr.m.swap = (char)swap;
	retval = ioctl(fd, TSC_IOCTL_RDWR_WRITE, &rdwr);

	return(retval);
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

int tsc_usr_read(int fd, uint usr_addr, char *buf, int len, int ds, int swap, int mem){
        struct tsc_ioctl_rdwr rdwr = { .rem_addr=0, .buf="", .len=0, .m={.ads='\0', .space='\0', .swap='\0', .am='\0'}};
	int retval;

	if(len < 0) return(-EINVAL);
	if(fd < 0) return(-EBADF);
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
	rdwr.m.ads = (char)RDWR_MODE_SET_DS(rdwr.m.ads,(char)ds);
	rdwr.m.swap = (char)swap;
	retval = ioctl(fd, TSC_IOCTL_RDWR_READ, &rdwr);

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
tsc_map_alloc(int fd, struct tsc_ioctl_map_win *w)
{
  if(fd < 0) return(-EBADF);
  switch( w->req.mode.sg_id)
  {
    case MAP_ID_MAS_PCIE_MEM:
    case MAP_ID_MAS_PCIE_PMEM:
    {
      return(ioctl(fd, TSC_IOCTL_MAP_MAS_ALLOC, w));
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
tsc_map_get(int fd, struct tsc_ioctl_map_win *w)
{
  if(fd < 0) return(-EBADF);
  switch( w->sts.mode.sg_id)
  {
    case MAP_ID_MAS_PCIE_MEM:
    case MAP_ID_MAS_PCIE_PMEM:
    {
      return(ioctl(fd, TSC_IOCTL_MAP_MAS_GET, w));
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
tsc_map_free(int fd, struct tsc_ioctl_map_win *w)
{
  if(fd < 0) return(-EBADF);
  switch( w->req.mode.sg_id)
  {
    case MAP_ID_MAS_PCIE_MEM:
    case MAP_ID_MAS_PCIE_PMEM:
    {
      return(ioctl(fd, TSC_IOCTL_MAP_MAS_FREE, w));
    }
    default:
    {
      return(-1);
    }
  }
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
tsc_map_modify(int fd, struct tsc_ioctl_map_win *w)
{
  if(fd < 0) return(-EBADF);
  switch( w->req.mode.sg_id)
  {
    case MAP_ID_MAS_PCIE_MEM:
    case MAP_ID_MAS_PCIE_PMEM:
    {
      return(ioctl(fd, TSC_IOCTL_MAP_MAS_MODIFY, w));
    }
    default:
    {
      return(-1);
    }
  }
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
tsc_map_read(int fd, struct tsc_ioctl_map_ctl *m)
{
  if(fd < 0) return(-EBADF);
  return(ioctl(fd, TSC_IOCTL_MAP_READ, m));
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
tsc_map_clear(int fd, struct tsc_ioctl_map_ctl *m)
{
  if(fd < 0) return(-EBADF);
  return(ioctl(fd, TSC_IOCTL_MAP_CLEAR, m));
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
tsc_pci_mmap(int fd, off_t pci_addr,
	      size_t size)
{
  if(fd < 0) return(NULL);
  return(mmap(NULL, size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, pci_addr));
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
tsc_pci_munmap(void *addr,
	        size_t size)
{
  return(munmap(addr, size));
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
tsc_kbuf_alloc(int fd, struct tsc_ioctl_kbuf_req *kr_p)
{
  if(fd < 0) return(-EBADF);
  return(ioctl(fd, TSC_IOCTL_KBUF_ALLOC, kr_p));
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
tsc_kbuf_free(int fd, struct tsc_ioctl_kbuf_req *kr_p)
{
  if(fd < 0) return(-EBADF);
  return(ioctl(fd, TSC_IOCTL_KBUF_FREE, kr_p));
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
tsc_kbuf_mmap(int fd, struct tsc_ioctl_kbuf_req *kr_p)
{
  if(fd < 0) return(NULL);
  kr_p->u_base =  mmap(NULL, kr_p->size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, (off_t)kr_p->b_base);
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
tsc_kbuf_munmap(struct tsc_ioctl_kbuf_req *kr_p)
{
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
tsc_kbuf_read(int fd, void *k_addr,
	       char *buf,
	       uint size)
{
  struct tsc_ioctl_kbuf_rw rw;

  if(fd < 0) return(-EBADF);
  rw.buf = (void *)buf;
  rw.k_addr = k_addr;
  rw.size = size;
  return(ioctl(fd, TSC_IOCTL_KBUF_READ, &rw));
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
tsc_kbuf_write(int fd, void *k_addr,
	        char *buf,
	        uint size)
{
  struct tsc_ioctl_kbuf_rw rw;

  if(fd < 0) return(-EBADF);
  rw.buf = (void *)buf;
  rw.k_addr = k_addr;
  rw.size = size;
  return(ioctl(fd, TSC_IOCTL_KBUF_WRITE, &rw));
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
tsc_dma_move(int fd, struct tsc_ioctl_dma_req *dr_p)
{
  if(fd < 0) return(-EBADF);
  return(ioctl(fd, TSC_IOCTL_DMA_MOVE, dr_p));
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
tsc_dma_wait(int fd, struct tsc_ioctl_dma_req *dr_p)
{
  if(fd < 0) return(-EBADF);
  return(ioctl(fd, TSC_IOCTL_DMA_WAIT, dr_p));
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
tsc_dma_status(int fd, struct tsc_ioctl_dma_sts *ds_p)
{
  if(fd < 0) return(-EBADF);
  return(ioctl(fd, TSC_IOCTL_DMA_STATUS, ds_p));
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
tsc_dma_mode(int fd, struct tsc_ioctl_dma_sts *dm_p)
{
  if(fd < 0) return(-EBADF);
  return(ioctl(fd, TSC_IOCTL_DMA_MODE, dm_p));
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
tsc_dma_alloc(int fd, int chan)
{
  struct tsc_ioctl_dma dma;

  dma.chan = (char)chan;
  if(fd < 0) return(-EBADF);
  return(ioctl(fd, TSC_IOCTL_DMA_ALLOC, &dma));
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
tsc_dma_free(int fd, int chan)
{
  struct tsc_ioctl_dma dma;

  dma.chan = (char)chan;
  if(fd < 0) return(-EBADF);
  return(ioctl(fd, TSC_IOCTL_DMA_FREE, &dma));
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
tsc_dma_clear(int fd, int chan)
{
  struct tsc_ioctl_dma dma;

  dma.chan = (char)chan;
  if(fd < 0) return(-EBADF);
  return(ioctl(fd, TSC_IOCTL_DMA_CLEAR, &dma));
} 

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * Function name : tsc_dma_transfer
 * Prototype     : int
 * Parameters    : pointer to dma  request data structure
 * Return        : status of DMA operation
 *----------------------------------------------------------------------------
 * Description   : perform a DMA transfer
 *
 *++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

int
tsc_dma_transfer(int fd, struct tsc_ioctl_dma_req *dr_p)
{
  if(fd < 0) return(-EBADF);
  return(ioctl(fd, TSC_IOCTL_DMA_TRANSFER, dr_p));
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
tsc_sflash_rdid(int fd, char *id)
{
  if(fd < 0) return(-EBADF);
  return(ioctl(fd, TSC_IOCTL_SFLASH_RDID, id));
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
tsc_sflash_rdsr(int fd, char *sr)
{
  if(fd < 0) return(-EBADF);
  return(ioctl(fd, TSC_IOCTL_SFLASH_RDSR, sr));
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
tsc_sflash_wrsr(int fd, char *sr)
{
  if(fd < 0) return(-EBADF);
  return(ioctl(fd, TSC_IOCTL_SFLASH_WRSR, sr));
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
tsc_sflash_read(int fd, int offset, char *buf, int len)
{
  struct tsc_ioctl_rdwr rw;

  if(fd < 0) return(-EBADF);
  rw.rem_addr = offset;
  rw.buf = buf;
  rw.len = len;
  return(ioctl(fd, TSC_IOCTL_SFLASH_READ, &rw));
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
tsc_sflash_write(int fd, int offset, char *buf, int len)
{
  struct tsc_ioctl_rdwr rw;

  if(fd < 0) return(-EBADF);
  rw.rem_addr = offset;
  rw.buf = buf;
  rw.len = len;
  return(ioctl(fd, TSC_IOCTL_SFLASH_WRITE, &rw));
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
tsc_timer_start(int fd, int mode,
		 int msec)
{
  struct tsc_ioctl_timer tmr;

  tmr.mode = mode;
  tmr.time.msec = msec;
  tmr.time.usec = 0;

  if(fd < 0) return(-EBADF);
  return(ioctl(fd, TSC_IOCTL_TIMER_START, &tmr));
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
tsc_timer_restart(int fd)
{
  if(fd < 0) return(-EBADF);
  return(ioctl(fd, TSC_IOCTL_TIMER_RESTART, NULL));
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
tsc_timer_stop(int fd)
{
  if(fd < 0) return(-EBADF);
  return(ioctl(fd, TSC_IOCTL_TIMER_STOP, NULL));
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
tsc_timer_read(int fd, struct tsc_time *tm)
{
  if(fd < 0) return(-EBADF);
  return(ioctl(fd, TSC_IOCTL_TIMER_READ, tm));
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
tsc_fifo_init(int fd, int idx,
               int mode)
{
  struct tsc_ioctl_fifo fifo;
  int retval;
  
  if(fd < 0) return(-EBADF);
  fifo.idx  = idx;
  fifo.mode = mode;
  retval = ioctl(fd, TSC_IOCTL_FIFO_INIT, &fifo);
  return(retval);
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
tsc_fifo_status(int fd, uint idx,
		 uint *sts)
{
  struct tsc_ioctl_fifo fifo;
  int retval;
  
  fifo.idx = idx;
  fifo.sts = 0;

  if(fd < 0) return(-EBADF);
  retval = ioctl(fd, TSC_IOCTL_FIFO_STATUS, &fifo);
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
tsc_fifo_clear(int fd, uint idx,
		uint *sts)
{
  struct tsc_ioctl_fifo fifo;
  int retval;
  
  fifo.idx = idx;
  fifo.sts = 0;
  if(fd < 0) return(-EBADF);
  retval = ioctl(fd, TSC_IOCTL_FIFO_CLEAR, &fifo);
  if(sts)
  {
    *sts = fifo.sts;
  }
  return(retval);
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
tsc_fifo_wait_ef(int fd, uint idx,
		  uint *sts,
		  uint tmo)
{
  struct tsc_ioctl_fifo fifo;
  int retval;
  
  fifo.idx = idx;
  fifo.tmo = tmo;
  fifo.sts = 0;
  if(fd < 0) return(-EBADF);
  retval = ioctl(fd, TSC_IOCTL_FIFO_WAIT_EF, &fifo);
  if(sts)
  {
    *sts = fifo.sts;
  }
  return(retval);
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
tsc_fifo_wait_ff(int fd, uint idx,
		  uint *sts,
		  uint tmo)
{
  struct tsc_ioctl_fifo fifo;
  int retval;
  
  fifo.idx = idx;
  fifo.tmo = tmo;
  fifo.sts = 0;
  if(fd < 0) return(-EBADF);
  retval = ioctl(fd, TSC_IOCTL_FIFO_WAIT_FF, &fifo);
  if(sts)
  {
    *sts = fifo.sts;
  }
  return(retval);
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
tsc_fifo_read(int fd, uint idx,
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
  if(fd < 0) return(-EBADF);
  retval = ioctl(fd, TSC_IOCTL_FIFO_READ, &fifo);
  if(sts)
  {
    *sts = fifo.sts;
  }
  return(retval);
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
tsc_fifo_write(int fd, uint idx,
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
  if(fd < 0) return(-EBADF);
  retval = ioctl(fd, TSC_IOCTL_FIFO_WRITE, &fifo);
  if(sts)
  {
    *sts = fifo.sts;
  }
  return(retval);
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
tsc_i2c_read(int fd, uint dev,
	      uint reg,
	      uint *data)
{
  if(dev & 0x80000000)
  {
    struct tsc_ioctl_i2c i2c;

    i2c.device = dev & ~0x80000000;
    i2c.cmd = reg;
    i2c.data = 0;
    if(fd < 0) return(-EBADF);
    ioctl(fd, TSC_IOCTL_I2C_READ, &i2c);
    *data = i2c.data;

    return(i2c.status);
  }
  else 
  {
    int fd_i2c;
    int cmd_size;
    int data_size;

    fd_i2c = i2c_set_dev(dev);
    if(fd_i2c < 0)
    {
      return(-1);
    }
    cmd_size  = ((dev>>16)&3)+1;
    data_size = ((dev>>18)&3)+1;
    if(cmd_size == 2)
    {
      i2c_smbus_write_byte_data(fd_i2c, (reg>>8)&0xff, reg&0xff);
      *data = i2c_smbus_read_byte(fd_i2c);
    }
    if(cmd_size == 1)
    {
      if(data_size == 1) *data = i2c_smbus_read_byte_data(fd_i2c, reg);
      if(data_size == 2) *data = i2c_smbus_read_word_data(fd_i2c, reg);
    }
    close(fd_i2c);
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
tsc_i2c_cmd(int fd, uint dev,
	     uint cmd)
{
  int fd_i2c;

  if(dev & 0x80000000)
  {
    struct tsc_ioctl_i2c i2c;

    i2c.device = dev & ~0x80000000;
    i2c.cmd = cmd;
    i2c.data = 0;
    if(fd < 0) return(-EBADF);
    ioctl(fd, TSC_IOCTL_I2C_CMD, &i2c);

    return(i2c.status);
  }
  else 
  {
    fd_i2c = i2c_set_dev(dev);
    if(fd_i2c < 0)
    {
      return(-1);
    }
    if(i2c_smbus_write_byte(fd_i2c, cmd) < 0)
    {
      return(-1);
    }
    close(fd_i2c);
    return(0);
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
tsc_i2c_write(int fd, uint dev,
	       uint reg,
	       uint data)
{
  if(dev & 0x80000000)
  {
    struct tsc_ioctl_i2c i2c;

    i2c.device = dev & ~0x80000000;
    i2c.cmd = reg;
    i2c.data = data;
    if(fd < 0) return(-EBADF);
    ioctl(fd, TSC_IOCTL_I2C_WRITE, &i2c);

    return(i2c.status);
  }
  else 
  {
    int fd_i2c;
    int cmd_size;
    int data_size;

    fd_i2c = i2c_set_dev(dev);
    if(fd_i2c < 0)
    {
      return(-1);
    }
    cmd_size  = ((dev>>16)&3)+1;
    data_size = ((dev>>18)&3)+1;
    if(cmd_size == 2)
    {
      i2c_smbus_write_word_data(fd_i2c, (reg>>8)&0xff, (reg&0xff) | ((data&0xff)<<8));
    }
    if(cmd_size == 1)
    {
      if(data_size == 1) i2c_smbus_write_byte_data(fd_i2c, reg, data);
      if(data_size == 2) i2c_smbus_write_word_data(fd_i2c, reg, data);
    }
    close(fd_i2c);
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
tsc_i2c_reset(uint dev)
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

int tsc_semaphore_status(int fd, uint *sts){
	struct tsc_ioctl_semaphore semaphore;
	int retval = 0;

	semaphore.sts = 0;

	if(fd < 0) return(-EBADF);
	retval = ioctl(fd, TSC_IOCTL_SEMAPHORE_STATUS, &semaphore);
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

int tsc_semaphore_release(int fd, uint idx, uint tag){
	struct tsc_ioctl_semaphore semaphore;
	int retval = 0;

	semaphore.idx = idx;
	semaphore.tag = tag;
	semaphore.sts = 0;

	if(fd < 0) return(-EBADF);
	retval = ioctl(fd, TSC_IOCTL_SEMAPHORE_RELEASE, &semaphore);
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

int tsc_semaphore_get(int fd, uint idx, uint tag){
	struct tsc_ioctl_semaphore semaphore;
	int retval = 0;

	semaphore.idx = idx;
	semaphore.tag = tag;

	if(fd < 0) return(-EBADF);
	retval = ioctl(fd, TSC_IOCTL_SEMAPHORE_GET, &semaphore);
	return(retval);
}

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * Function name : tsc_user_irq_wait
 * Prototype     : int
 * Parameters    : USER irq ioctl structure
 * Return        : error/success
 *----------------------------------------------------------------------------
 * Description   : user irq wait
 *
 *++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
int tsc_user_irq_wait(int fd, struct tsc_ioctl_user_irq *user_irq_p)
{
	int retval = 0;

	if(fd < 0) return -EBADF;
	retval = ioctl(fd, TSC_IOCTL_USER_WAIT, user_irq_p);
	return retval;
}

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * Function name : tsc_user_irq_subscribe
 * Prototype     : int
 * Parameters    : USER irq ioctl structure
 * Return        : error/success
 *----------------------------------------------------------------------------
 * Description   : user irq subscribe
 *
 *++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
int tsc_user_irq_subscribe(int fd, struct tsc_ioctl_user_irq *user_irq_p)
{
	int retval = 0;

	if(fd < 0) return -EBADF;
	retval = ioctl(fd, TSC_IOCTL_USER_SUBSCRIBE, user_irq_p);
	return retval;
}

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * Function name : tsc_user_irq_unsubscribe
 * Prototype     : int
 * Parameters    : USER irq ioctl structure
 * Return        : error/success
 *----------------------------------------------------------------------------
 * Description   : user irq unsubscribe
 *
 *++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
int tsc_user_irq_unsubscribe(int fd, struct tsc_ioctl_user_irq *user_irq_p)
{
	int retval = 0;

	if(fd < 0) return -EBADF;
	retval = ioctl(fd, TSC_IOCTL_USER_UNSUBSCRIBE, user_irq_p);
	return retval;
}
