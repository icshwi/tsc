/*=========================< begin file & file header >=======================
 *  References
 *  
 *    filename : sflashlib.c
 *    author   : JFG
 *    company  : IOxOS
 *    creation : Sept 27,2015
 *    version  : 0.0.1
 *
 *----------------------------------------------------------------------------
 *  Description
 *
 *    This file contains the low level functions to drive the address mappers
 *    implemented on the TSC.
 *
 *----------------------------------------------------------------------------
 *
 * Copyright (c) 2015 IOxOS Technologies SA <ioxos@ioxos.ch>
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 * GPL license :
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 *
 *----------------------------------------------------------------------------
 *  Change History
 *  
 *
 *=============================< end file header >============================*/
#include "tscos.h"
#include "tscdrvr.h"

#define DBGno
#include "debug.h"

struct sflash_para flash_para_S25FL128P =
{
  { 0x01,0x20,0x18,0x00},
  0x1000000,
  0x0010000,
  0x0000100,
    1000000
};

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * Function name : sflash_load_cmd
 * Prototype     : int
 * Parameters    : pointer to TSC device control structure
 *                 pointer to SFLASH slave configuraton parameters
 * Return        : error/success
 *----------------------------------------------------------------------------
 * Description   : read TSC SFLASH
 *
 *++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

void
sflash_load_cmd( struct tsc_device *ifc,
                 uint cmd,
		 uint para)
{
  int ds;
  uint data;

  switch( cmd)
  {
    case SFLASH_CMD_READ:
    case SFLASH_CMD_FREAD:
    case SFLASH_CMD_PP:
    case SFLASH_CMD_SE:
    {
      cmd = ( cmd << 24) | (para & 0xffffff);
      ds = 32;
      break;
    }
    case SFLASH_CMD_WREN:
    case SFLASH_CMD_WRDI:
    case SFLASH_CMD_RDID:
    case SFLASH_CMD_RDSR:
    case SFLASH_CMD_RDSR2:
    case SFLASH_CMD_BE:
    case SFLASH_CMD_DUMMY:
    {
      ds = 8;
      break;
    }
    case SFLASH_CMD_WRSR:
    {
      cmd = ( cmd << 16) | ((para & 0xff)<<8) | ((para & 0xff00)>>8);
      ds = 24;
      break;
    }
    default:
    {
      return;
    }
  }

  while( ds--)
  {
    data = 0;
    if( cmd & ( 1 <<  ds)) data = TSC_ILOC_SPI_DO;
    data |= TSC_ILOC_SPI_CLK | TSC_ILOC_SPI_CS;
    iowrite32( data, ifc->csr_ptr + TSC_CSR_ILOC_SPI);
  }

  return;
}

void
sflash_start_cmd( struct tsc_device *ifc)
{
  iowrite32( TSC_ILOC_SPI_CS, ifc->csr_ptr + TSC_CSR_ILOC_SPI);
}

void
sflash_end_cmd( struct tsc_device *ifc)
{
  iowrite32( TSC_ILOC_SPI_END, ifc->csr_ptr + TSC_CSR_ILOC_SPI);
}

void
sflash_write_byte( struct tsc_device *ifc,
                   unsigned char b)
{
  uint i, data;

  i = 8;
  while( i--)
  {
    data = 0;
    if( b & (1<<i)) data = TSC_ILOC_SPI_DO;
    data |= TSC_ILOC_SPI_CLK | TSC_ILOC_SPI_CS;
    iowrite32( data, ifc->csr_ptr + TSC_CSR_ILOC_SPI);
  }

  return;
}

unsigned char
sflash_read_byte( struct tsc_device *ifc)
{
  unsigned char b;
  uint i, data;

  i = 8;
  b = 0;
  while( i--)
  {
    data = ioread32( ifc->csr_ptr + TSC_CSR_ILOC_SPI);
    if( data & TSC_ILOC_SPI_DI)
    {
      b |= 1 <<  i;
    }
    iowrite32( TSC_ILOC_SPI_CLK | TSC_ILOC_SPI_CS, ifc->csr_ptr + TSC_CSR_ILOC_SPI);
  }

  return( b);
}

unsigned char
sflash_wait_busy( struct tsc_device *ifc,
                  uint tmo)
{
  unsigned char status;

  status = 0;
  sflash_start_cmd( ifc);
  sflash_load_cmd( ifc, SFLASH_CMD_RDSR, 0);
  while(tmo--)
  {
    status = sflash_read_byte( ifc);
    if( !(status & SFLASH_SR_BUSY))
    {
      break;
    }
    cpu_relax();
  };
  sflash_end_cmd( ifc);
  return( status);
}

unsigned char
sflash_write_enable( struct tsc_device *ifc,
                     uint tmo)
{
  unsigned char status;

  status = 0;
  sflash_start_cmd( ifc);
  sflash_load_cmd( ifc, SFLASH_CMD_WREN, 0);
  sflash_end_cmd( ifc);
  sflash_start_cmd( ifc);
  sflash_load_cmd( ifc, SFLASH_CMD_RDSR, 0);
  while(tmo--)
  {
    status = sflash_read_byte( ifc);
    if( status & SFLASH_SR_WEL)
    {
      break;
    }
    cpu_relax();
  };
  sflash_end_cmd( ifc);
  return( status);
}

int
sflash_sector_erase( struct tsc_device *ifc,
                     uint offset)
{

  sflash_start_cmd( ifc);
  sflash_load_cmd(  ifc, SFLASH_CMD_SE, offset);
  sflash_end_cmd( ifc);
  if( sflash_wait_busy(  ifc, 1000000) & SFLASH_SR_BUSY) return( -1);
  return( 0);
}

int
sflash_page_program( struct tsc_device *ifc,
                     uint offset,
		     unsigned char *p,
		     uint size)
{
  int i;
 
  sflash_start_cmd( ifc);
  sflash_load_cmd(  ifc, SFLASH_CMD_PP, offset);
  for( i = 0; i < size; i++)
  {
    sflash_write_byte(  ifc, *p++);
  }
  sflash_end_cmd( ifc);
  if( sflash_wait_busy( ifc, 1000000) & SFLASH_SR_BUSY)
  {
    return( -1);
  }
  return( 0);
}

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * Function name : tsc_sflash_read_ID
 * Prototype     : void
 * Parameters    : pointer to TSC device control structure
 *                 pointer to 
 * Return        : none
 *----------------------------------------------------------------------------
 * Description   : load the 3 byte SFLASH identifier in the string pointed
 *                 by data_p 
 *
 *++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

int
tsc_sflash_read_ID( struct tsc_device *ifc,
                    char *data_p)
{
  sflash_start_cmd( ifc);
  sflash_load_cmd( ifc, SFLASH_CMD_RDID, 0);
  data_p[0] = sflash_read_byte(ifc);
  data_p[1] = sflash_read_byte(ifc);
  data_p[2] = sflash_read_byte(ifc);
  sflash_end_cmd(ifc);
  return( 0);
}
EXPORT_SYMBOL( tsc_sflash_read_ID);

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * Function name : tsc_sflash_read_status
 * Prototype     : int
 * Parameters    : pointer to TSC device control structure
 * Return        : 2 bytes status
 *----------------------------------------------------------------------------
 * Description   : read TSC SFLASH status (2 bytes)
 *
 *++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

unsigned short
tsc_sflash_read_sr( struct tsc_device *ifc)
{
  unsigned short sr;

  sflash_start_cmd( ifc);
  sflash_load_cmd( ifc, SFLASH_CMD_RDSR2, 0);
  sr = (sflash_read_byte( ifc) << 8) & 0xff00;
  sflash_end_cmd( ifc);
  sflash_start_cmd( ifc);
  sflash_load_cmd( ifc, SFLASH_CMD_RDSR, 0);
  sr |= sflash_read_byte( ifc) & 0xff;
  sflash_end_cmd( ifc);
  return( sr);
}
EXPORT_SYMBOL( tsc_sflash_read_sr);

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * Function name : tsc_sflash_write_status
 * Prototype     : void
 * Parameters    : pointer to TSC device control structure
 * Return        : none
 *----------------------------------------------------------------------------
 * Description   : write TSC SFLASH status (2 bytes)
 *
 *++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

void
tsc_sflash_write_sr( struct tsc_device *ifc,
		     unsigned short sr)
{
  int tmo;
 
  tmo = 1000;
  sflash_write_enable( ifc, tmo);
  sflash_start_cmd( ifc);
  sflash_load_cmd( ifc, SFLASH_CMD_WRSR, (uint)sr);
  sflash_end_cmd( ifc);
  return;
}
EXPORT_SYMBOL( tsc_sflash_write_sr);



/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * Function name : tsc_sflash_conf
 * Prototype     : void
 * Parameters    : pointer to TSC device control structure
 *                 pointer to 
 * Return        : none
 *----------------------------------------------------------------------------
 * Description   : load the 3 byte SFLASH identifier in the string pointed
 *                 by data_p 
 *
 *++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

int
tsc_sflash_conf( struct tsc_device *ifc)
                 
{
  struct sflash_para *fp;
  if( ifc->sflash_ctl)
  {
    fp = &flash_para_S25FL128P;
    if( !strncmp( ifc->sflash_ctl->id, fp->id, 3))
    { 
      ifc->sflash_ctl->para = &flash_para_S25FL128P;
      return( 0);
    }
  }
  return( -ENODEV);
}
EXPORT_SYMBOL( tsc_sflash_conf);

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * Function name : tsc_sflash_read
 * Prototype     : int
 * Parameters    : pointer to TSC device control structure
 *                 pointer to SFLASH slave configuraton parameters
 * Return        : error/success
 *----------------------------------------------------------------------------
 * Description   : read TSC SFLASH
 *
 *++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

int
tsc_sflash_read( struct tsc_device *ifc,
                 uint offset,
	         char *kbuf,
	         uint size)
{
  int cnt;
  if( ifc->sflash_ctl)
  {
    if( offset > ifc->sflash_ctl->para->device_size)
    {
      return( -EINVAL);
    }
    if( (offset+size) > ifc->sflash_ctl->para->device_size)
    {
      size = ifc->sflash_ctl->para->device_size - offset;
    }
    sflash_start_cmd( ifc);
    sflash_load_cmd( ifc, SFLASH_CMD_READ, offset);
    cnt = 0;
    while( size--)
    {
      *kbuf++ = sflash_read_byte( ifc);
      cnt++;
    }
    sflash_end_cmd( ifc);
    return( cnt);
  }
  return( -ENODEV);
}
EXPORT_SYMBOL( tsc_sflash_read);

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * Function name : tsc_sflash_write
 * Prototype     : int
 * Parameters    : pointer to TSC device control structure
 *                 pointer to SFLASH slave configuraton parameters
 * Return        : error/success
 *----------------------------------------------------------------------------
 * Description   : write TSC SFLASH
 *
 *++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

int
tsc_sflash_wrprot( struct tsc_device *ifc)
{
  uint data;

  if( ifc->sflash_ctl)
  {
    data = ioread32( ifc->csr_ptr + TSC_CSR_ILOC_SPI);
    return( TSC_ILOC_SPI_WRPROT( data));
  }
  return( -ENODEV);
}
EXPORT_SYMBOL( tsc_sflash_wrprot);

int
sflash_sector_write( struct tsc_device *ifc,
                     uint offset,
	             unsigned char *buf)
{
  int i, retval, tmo;
  unsigned char status;
  struct sflash_para *p;
 
  retval = 0;
  tmo = 1000;
  p = ifc->sflash_ctl->para;
  /* check sector alignment */
  if( offset & (p->sector_size-1))
  {
    return(-1);
  }
  status = sflash_write_enable( ifc, 1000);
  if( !(status & SFLASH_SR_WEL))
  {
  	return( -1);
  }
  if( sflash_sector_erase( ifc, offset) < 0)
  {
    retval = -1;
    goto sflash_sector_write_exit;
  }
  for( i = 0; i < (p->sector_size >> 8); i++)
  {
    sflash_write_enable( ifc, tmo);
    if( sflash_page_program( ifc, offset, buf, p->page_size) < 0)
    {	
      retval = -1;
      goto sflash_sector_write_exit;
    }
    buf += p->page_size;
    offset += p->page_size;
  }

sflash_sector_write_exit:
  /* write disable */
  sflash_start_cmd( ifc);
  sflash_load_cmd(  ifc, SFLASH_CMD_WRDI, 0);
  sflash_end_cmd( ifc);

  return( retval);

}

int
tsc_sflash_write( struct tsc_device *ifc,
                  uint offset,
	          char *k_buf,
	          uint size)
{
  uint s_start;         /* start of first sector                    */
  uint s_end;           /* end of last sector                       */
  uint first;           /* data offset in first sector              */
  uint last ;           /* data offset in last sector               */
  int sect_size, sect_mask, sect_num;
  char *t_buf;

  /* check for write protection */
  if( tsc_sflash_wrprot(ifc))
  {
    //return( -EACCES);
  }
  if( ifc->sflash_ctl)
  {
    sect_size = ifc->sflash_ctl->para->sector_size;
    sect_mask = sect_size - 1;

    s_start = offset & ~sect_mask;
    first = offset & sect_mask;
    s_end = (offset + size) & ~sect_mask;
    last =  (offset + size) & sect_mask;

    if( last)
    {
      s_end += sect_size;
    }
    sect_num = (s_end - s_start)/sect_size;
    if( s_end > ifc->sflash_ctl->para->device_size)
    {
      return( -EINVAL);
    }
    t_buf = (char *)kzalloc( sect_size, GFP_KERNEL);
    if( !t_buf)
    {
      return(-ENOMEM);
    }
    mutex_lock( &ifc->sflash_ctl->sflash_lock);
    if( first)
    {
      int len;

      tsc_sflash_read( ifc, s_start, t_buf, sect_size);
      if( (sect_num == 1) && last)
      {
	len = last - first;
      }
      else
      {
	len = sect_size - first;
      }
      memcpy( t_buf + first, k_buf, len);
      sflash_sector_write( ifc, s_start, t_buf);
      sect_num -= 1;
      s_start +=  sect_size;
      k_buf += len;
    }
    while( sect_num > 1)
    {
      sflash_sector_write( ifc, s_start, k_buf);
      sect_num -= 1;
      s_start +=  sect_size;
      k_buf += sect_size;
    }
    if( sect_num)
    {
      int len;

      tsc_sflash_read( ifc, s_start, t_buf, sect_size);
      if( last)
      {
	len = last;
      }
      else
      {
	len = sect_size;
      }
      memcpy( t_buf, k_buf, len);
      sflash_sector_write( ifc, s_start, t_buf);
    }
    mutex_unlock( &ifc->sflash_ctl->sflash_lock);
    kfree( t_buf);
    return(size);
  }
  return( -ENODEV);
}
EXPORT_SYMBOL( tsc_sflash_write);

