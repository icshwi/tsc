/*=========================< begin file & file header >=======================
 *  References
 *  
 *    filename : ioctllib.c
 *    author   : JFG
 *    company  : IOxOS
 *    creation : Sept 30,2015
 *    version  : 0.0.1
 *
 *----------------------------------------------------------------------------
 *  Description
 *
 *   This file...
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


/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * Function name : ioctl_csr
 * Prototype     : unsigned int
 * Parameters    : pointer to IFC1211 device control structure
 *                 cmd   -> command code
 *                 arg   -> command argument
 * Return        : error/success
 *----------------------------------------------------------------------------
 * Description   : perform access on IFC1211 registers
 *
 *++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

int 
ioctl_csr( struct ifc1211_device *ifc,
	   unsigned int cmd,
	   unsigned long arg)
{
  int retval;
  struct tsc_ioctl_csr_op c;

  retval = 0;

  if( copy_from_user( (void *)&c, (void *)arg, sizeof( c)))
  {
    return(-EFAULT);
  }
  c.operation = cmd;
  retval = tsc_csr_op( ifc, &c);
  if( retval < 0)
  {
    return( retval);
  }
  if( copy_to_user( (void *)arg, (void *)&c, sizeof( c)))
  {
    return(-EFAULT);
  }
  return( retval);
}

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * Function name : ioctl_map
 * Prototype     : unsigned int
 * Parameters    : pointer to IFC1211 device control structure
 *                 cmd   -> command code
 *                 arg   -> command argument
 * Return        : error/success
 *----------------------------------------------------------------------------
 * Description   : perform IFC1211 address mapping operations
 *
 *++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

int 
ioctl_map( struct ifc1211_device *ifc,
	   unsigned int cmd,
	   unsigned long arg)
{
  int retval;

  retval = 0;
  debugk(("in ioctl_map( %p, %x, %lx)\n", ifc, cmd, arg));
  switch ( cmd & TSC_IOCTL_MAP_OP_MSK)
  {
    case TSC_IOCTL_MAP_READ:
    {
      struct tsc_ioctl_map_ctl m;
      if( copy_from_user( (void *)&m, (void *)arg, sizeof( m)))
      {
        retval = -EFAULT;
        break;
      }
      retval = tsc_map_read( ifc, &m);
      if( retval < 0)
      {
        break;
      }
      if( copy_to_user( (void *)arg, (void *)&m, sizeof( m)))
      {
        retval = -EFAULT;
        break;
      }
      break;
    }
    case TSC_IOCTL_MAP_CLEAR:
    {
      struct tsc_ioctl_map_ctl m;
      if( copy_from_user( (void *)&m, (void *)arg, sizeof( m)))
      {
        retval = -EFAULT;
        break;
      }
      retval = tsc_map_clear( ifc, &m);
      if( retval < 0)
      {
        break;
      }
      break;
    }
    case TSC_IOCTL_MAP_MAS:
    {
      struct tsc_ioctl_map_win w;
      short mode;

      if( copy_from_user( (void *)&w, (void *)arg, sizeof( w)))
      {
        retval = -EFAULT;
        break;
      }
      if( cmd == TSC_IOCTL_MAP_MAS_ALLOC)
      {
        mode  = tsc_map_mas_set_mode( ifc, &w.req.mode);
	if( tsc_map_mas_alloc( ifc, &w) >= 0)
	{
          if( copy_to_user( (void *)arg, (void *)&w, sizeof( w)))
          {
            retval = -EFAULT;
            break;
          }
	}
	else 
	{
          retval = -EINVAL;
          break;
	}
      }
      if( cmd == TSC_IOCTL_MAP_MAS_FREE)
      {
	retval = tsc_map_mas_free( ifc, w.req.mode.sg_id, w.pg_idx);
      }
      break;
    }
    case TSC_IOCTL_MAP_SLV:
    {
      struct tsc_ioctl_map_win w;

      if( copy_from_user( (void *)&w, (void *)arg, sizeof( w)))
      {
        retval = -EFAULT;
        break;
      }
      if( cmd == TSC_IOCTL_MAP_SLV_ALLOC)
      {
	if( tsc_map_slv_alloc( ifc, &w) >= 0)
	{
          if( copy_to_user( (void *)arg, (void *)&w, sizeof( w)))
          {
            retval = -EFAULT;
            break;
          }
	}
	else 
	{
          retval = -EINVAL;
          break;
	}
      }
      if( cmd == TSC_IOCTL_MAP_SLV_FREE)
      {
	retval = tsc_map_slv_free( ifc, w.req.mode.sg_id, w.pg_idx);
      }
      break;
    }
    default:
    {
      retval = -EINVAL;
    }
  }
  return( retval);
}

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * Function name : ioctl_rdwr
 * Prototype     : unsigned int
 * Parameters    : pointer to IFC1211 device control structure
 *                 cmd   -> command code
 *                 arg   -> command argument
 * Return        : error/success
 *----------------------------------------------------------------------------
 * Description   : perform IFC1211 read/write operations operations
 *
 *++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

int 
ioctl_rdwr( struct ifc1211_device *ifc,
	    unsigned int cmd,
	    unsigned long arg)
{
  int retval;
  struct tsc_ioctl_rdwr rw;

  retval = 0;
  if( copy_from_user( (void *)&rw, (void *)arg, sizeof( rw)))
  {
    return( -EFAULT);
  }

  switch ( cmd ) 
  {
    case TSC_IOCTL_RDWR_READ:
    {
      retval = tsc_rem_read( ifc, &rw);
      if( retval < 0)
      {
	return( retval);
      }
      break;
    }
    case TSC_IOCTL_RDWR_WRITE:
    {
      retval = tsc_rem_write( ifc, &rw);
      if( retval < 0)
      {
	return( retval);
      }
      break;
    }
    default:
    {
      return( -EINVAL);
    }
  }
  if( copy_to_user( (void *)arg, (void *)&rw, sizeof( rw)))
  {
    return( -EFAULT);
  }
  return( retval);
}

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * Function name : ioctl_dma
 * Prototype     : unsigned int
 * Parameters    : pointer to IFC1211 device control structure
 *                 cmd   -> command code
 *                 arg   -> command argument
 * Return        : error/success
 *----------------------------------------------------------------------------
 * Description   : perform IFC1211 DMA operations
 *
 *++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

int 
ioctl_dma( struct ifc1211_device *ifc,
	   unsigned int cmd,
	   unsigned long arg)
{
  int retval;

  retval = 0;
  switch ( cmd)
  {
    case TSC_IOCTL_DMA_MOVE:
    {
      struct tsc_ioctl_dma_req dma_req;

      if( copy_from_user(&dma_req, (void *)arg, sizeof(dma_req)))
      {
	return( -EFAULT);
      }
      retval = tsc_dma_move( ifc, &dma_req);
      if( retval < 0)
      {
	return( retval);
      }
      if( copy_to_user( (void *)arg, &dma_req, sizeof( dma_req)))
      {
	return -EFAULT;
      }
      break;
    }
    case TSC_IOCTL_DMA_WAIT:
    {
      struct tsc_ioctl_dma_req dma_req;

      if( copy_from_user(&dma_req, (void *)arg, sizeof(dma_req)))
      {
	return( -EFAULT);
      }
      retval = tsc_dma_wait( ifc, &dma_req);
      if( retval < 0)
      {
	return( retval);
      }
      if( copy_to_user( (void *)arg, &dma_req, sizeof( dma_req)))
      {
	return -EFAULT;
      }
      break;
    }
    case TSC_IOCTL_DMA_STATUS:
    {
      struct tsc_ioctl_dma_sts dma_sts;

      if( copy_from_user(&dma_sts, (void *)arg, sizeof(dma_sts)))
      {
	return( -EFAULT);
      }
      retval = tsc_dma_status( ifc, &dma_sts);
      if( retval < 0)
      {
	return( retval);
      }
      if( copy_to_user( (void *)arg, &dma_sts, sizeof( dma_sts)))
      {
	return -EFAULT;
      }
      break;
    }
    case TSC_IOCTL_DMA_MODE:
    {
      struct tsc_ioctl_dma_mode dma_mode;

      if( copy_from_user( &dma_mode, (void *)arg, sizeof(dma_mode)))
      {
	return( -EFAULT);
      }
      retval = tsc_dma_mode( ifc, &dma_mode);
      if( retval < 0)
      {
	return( retval);
      }
      if( copy_to_user( (void *)arg, &dma_mode, sizeof( dma_mode)))
      {
	return -EFAULT;
      }
      break;
    }
    case TSC_IOCTL_DMA_ALLOC:
    case TSC_IOCTL_DMA_FREE:
    case TSC_IOCTL_DMA_CLEAR:
    {
      struct tsc_ioctl_dma dma;

      if( copy_from_user(&dma, (void *)arg, sizeof(dma)))
      {
	return( -EFAULT);
      }
      if( cmd == TSC_IOCTL_DMA_ALLOC) retval = tsc_dma_alloc( ifc, &dma);
      else if( cmd == TSC_IOCTL_DMA_FREE) retval = tsc_dma_free( ifc, &dma);
      else if( cmd == TSC_IOCTL_DMA_CLEAR) retval = tsc_dma_clear( ifc, &dma);
      else retval = -EINVAL;
      if( retval < 0)
      {
	return( retval);
      }
      if( copy_to_user( (void *)arg, &dma, sizeof( dma)))
      {
	return -EFAULT;
      }
      break;
    }
    default:
    {
      return( -EINVAL);
    }
  }
  return( retval);
}

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * Function name : ioctl_kbuf
 * Prototype     : unsigned int
 * Parameters    : pointer to IFC1211 device control structure
 *                 cmd   -> command code
 *                 arg   -> command argument
 * Return        : error/success
 *----------------------------------------------------------------------------
 * Description   : perform IFC1211 DMA operations
 *
 *++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

int 
ioctl_kbuf( struct ifc1211_device *ifc,
	   unsigned int cmd,
	   unsigned long arg)
{
  int retval;

  retval = 0;
  switch ( cmd)
  {
    case TSC_IOCTL_KBUF_ALLOC:
    {
      struct tsc_ioctl_kbuf_req kbr;

      if( copy_from_user(&kbr, (void *)arg, sizeof(kbr)))
      {
	return( -EFAULT);
      }
      retval = tsc_kbuf_alloc( ifc, &kbr);
      if( retval < 0)
      {
	return( retval);
      }
      if( copy_to_user( (void *)arg, &kbr, sizeof( kbr)))
      {
	return -EFAULT;
      }
      break;
    }
    case TSC_IOCTL_KBUF_FREE:
    {
      struct tsc_ioctl_kbuf_req kbr;

      if( copy_from_user(&kbr, (void *)arg, sizeof(kbr)))
      {
        return( -EFAULT);
      }
      retval = tsc_kbuf_free( ifc, &kbr);
      break;
    }
    case TSC_IOCTL_KBUF_READ:
    {
      struct tsc_ioctl_kbuf_rw krw;

      /* get read request */
      if( copy_from_user(&krw, (void *)arg, sizeof(krw)))
      {
        return( -EFAULT);
      }
      /* transfer data */
      if( copy_to_user( krw.buf, krw.k_addr, krw.size))
      {
	return -EFAULT;
      }
      break;
    }
    case TSC_IOCTL_KBUF_WRITE:
    {
      struct tsc_ioctl_kbuf_rw krw;

      /* get read request */
      if( copy_from_user(&krw, (void *)arg, sizeof(krw)))
      {
        return( -EFAULT);
      }
      /* transfer data */
      if( copy_from_user( krw.k_addr, krw.buf, krw.size))
      {
	return -EFAULT;
      }
      break;
    }
    default:
    {
      return( -EINVAL);
    }
  }
  return( retval);
}

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * Function name : ioctl_sflash
 * Prototype     : unsigned int
 * Parameters    : pointer to IFC1211 device control structure
 *                 cmd   -> command code
 *                 arg   -> command argument
 * Return        : error/success
 *----------------------------------------------------------------------------
 * Description   : perform IFC1211 DMA operations
 *
 *++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

int 
ioctl_sflash( struct ifc1211_device *ifc,
	   unsigned int cmd,
	   unsigned long arg)
{
  int retval;

  retval = 0;
  switch ( cmd)
  {
    case TSC_IOCTL_SFLASH_RDID:
    {
      char id[4];

      retval = tsc_sflash_read_ID( ifc, id);
      if( copy_to_user( (void *)arg, id, 4))
      {
        return( -EFAULT);
      }
      break;
    }
    case TSC_IOCTL_SFLASH_RDSR:
    {
      unsigned short sr;

      sr = tsc_sflash_read_sr( ifc);
      retval = 0;
      if( copy_to_user( (void *)arg, &sr, 2))
      {
        return( -EFAULT);
      }
      break;
    }
    case TSC_IOCTL_SFLASH_WRSR:
    {
      unsigned short sr;

      if( copy_from_user( &sr, (void *)arg, 2))
      {
        return( -EFAULT);
      }
      tsc_sflash_write_sr( ifc, sr);
      break;
    }
    case TSC_IOCTL_SFLASH_READ:
    case TSC_IOCTL_SFLASH_WRITE:
    {
      struct tsc_ioctl_rdwr srw;
      char *kbuf;

      /* get read request */
      if( copy_from_user( &srw, (void *)arg, sizeof(srw)))
      {
        return( -EFAULT);
      }
      kbuf = (char *)kzalloc( srw.len, GFP_KERNEL);
      if( !kbuf)
      {
	return(-ENOMEM);
      }
      if( cmd == TSC_IOCTL_SFLASH_READ)
      {
	/* get data from SFLASH device */
	retval = tsc_sflash_read( ifc, srw.rem_addr, kbuf, srw.len);
	if( retval > 0)
	{
          if( copy_to_user( (void *)srw.buf, kbuf, retval))
          {
            return( -EFAULT);
          }
	}
      }
      if( cmd == TSC_IOCTL_SFLASH_WRITE)
      {
	/* write data to SFLASH device */
	if( copy_from_user( kbuf, (void *)srw.buf, srw.len))
        {
          return( -EFAULT);
        }
	retval = tsc_sflash_write( ifc, srw.rem_addr, kbuf, srw.len);
      }
      if( kbuf)
      {
	kfree( kbuf);
      }
      break;
    }
    default:
    {
      return( -EINVAL);
    }
  }

  return( retval);
}

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * Function name : ioctl_timer
 * Prototype     : unsigned int
 * Parameters    : pointer to IFC1211 device control structure
 *                 cmd   -> command code
 *                 arg   -> command argument
 * Return        : error/success
 *----------------------------------------------------------------------------
 * Description   : perform IFC1211 DMA operations
 *
 *++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

int 
ioctl_timer( struct ifc1211_device *ifc,
	   unsigned int cmd,
	   unsigned long arg)
{
  int retval;

  retval = 0;
  switch ( cmd)
  {
    case TSC_IOCTL_TIMER_START:
    {
      struct tsc_ioctl_timer tmr;

      if( copy_from_user( &tmr, (void *)arg, sizeof( tmr)))
      {
        return( -EFAULT);
      }
      retval = tsc_timer_start( ifc, &tmr);

      break;
    }
    case TSC_IOCTL_TIMER_RESTART:
    {
      struct tsc_time tm;
      if( (void *)arg == NULL)
      {
        retval = tsc_timer_restart( ifc, NULL);
      }
      else 
      {
        if( copy_from_user( &tm, (void *)arg, sizeof( tm)))
        {
          return( -EFAULT);
        }
        retval = tsc_timer_restart( ifc, &tm);
      }
      break;
    }
    case TSC_IOCTL_TIMER_STOP:
    {
      retval = tsc_timer_stop( ifc);
      break;
    }
    case TSC_IOCTL_TIMER_READ:
    {
      struct tsc_time tm;

      retval = tsc_timer_read( ifc, &tm);
      if( copy_to_user( (void *)arg, &tm, sizeof( tm)))
      {
        return( -EFAULT);
      }
      break;
    }
    default:
    {
      return( -EINVAL);
    }
  }

  return( retval);
}

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * Function name : ioctl_fifo
 * Prototype     : unsigned int
 * Parameters    : pointer to IFC1211 device control structure
 *                 cmd   -> command code
 *                 arg   -> command argument
 * Return        : error/success
 *----------------------------------------------------------------------------
 * Description   : perform IFC1211 DMA operations
 *
 *++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

int ioctl_fifo( struct ifc1211_device *ifc, unsigned int cmd, unsigned long arg){
	int retval;
	struct tsc_ioctl_fifo fifo;

	retval = 0;
	if( copy_from_user( &fifo, (void *)arg, sizeof( fifo))){
		return( -EFAULT);
	}
	switch (cmd){
    	case TSC_IOCTL_FIFO_INIT:
    	{
    		tsc_fifo_init(ifc, fifo.idx, fifo.mode);
    		break;
    	}
    	case TSC_IOCTL_FIFO_READ:
    	case TSC_IOCTL_FIFO_WRITE:
    	{
    		int *data;
    		data = (int *)kzalloc( sizeof( int) * (IFC1211_FIFO_CTL_WCNT_MAX+1), GFP_KERNEL);
    		if(!data){
    			return(-ENOMEM);
    		}
    		if(cmd == TSC_IOCTL_FIFO_READ){
    			if( copy_from_user( data, fifo.data, sizeof( int) * fifo.cnt)){
    				retval = -EFAULT;
    			}
    			else {
    				retval = tsc_fifo_read( ifc, fifo.idx, data, fifo.cnt, &fifo.sts);
    			}
    		}
    		if(cmd == TSC_IOCTL_FIFO_WRITE){
    			retval = tsc_fifo_write( ifc, fifo.idx, data, fifo.cnt, &fifo.sts);
    			if(retval > 0){
    				/* fifo_write OK -> retval holds actual word count */
    				if(copy_to_user( fifo.data, data, sizeof( int) * retval)){
    					retval = -EFAULT;
    				}
    			}
    		}
    		if(data){
    			kfree( data);
    		}
    		break;
    	}
    	case TSC_IOCTL_FIFO_CLEAR:
    	{
    		tsc_fifo_clear(ifc, fifo.idx);
    		break;
    	}
    	case TSC_IOCTL_FIFO_WAIT_EF:
    	{
    		retval = tsc_fifo_wait_ef(ifc, &fifo);
    		break;
    	}
    	case TSC_IOCTL_FIFO_WAIT_FF:
    	{
    		retval = tsc_fifo_wait_ff(ifc, &fifo);
    		break;
    	}
    	case TSC_IOCTL_FIFO_STATUS:
    	{
    		tsc_fifo_status(ifc, fifo.idx, &fifo.sts);
    		break;
    	}
    	default:
    	{
    		return( -EINVAL);
    	}
	}
	return(retval);
}
/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * Function name : ioctl_i2c
 * Prototype     : unsigned int
 * Parameters    : pointer to IFC1211 device control structure
 *                 cmd   -> command code
 *                 arg   -> command argument
 * Return        : error/success
 *----------------------------------------------------------------------------
 * Description   : perform IFC1211 I2C operations
 *
 *++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

int 
ioctl_i2c( struct ifc1211_device *ifc,
	    unsigned int cmd,
	    unsigned long arg)
{
  int retval;
  struct tsc_ioctl_i2c i2c;

  retval = 0;
  if( copy_from_user( &i2c, (void *)arg, sizeof( i2c)))
  {
    return( -EFAULT);
  }
  switch ( cmd)
  {
    case TSC_IOCTL_I2C_RESET:
    {
      break;
    }
    case TSC_IOCTL_I2C_READ:
    {
      retval = tsc_i2c_read( ifc, &i2c);
      break;
    }
    case TSC_IOCTL_I2C_WRITE:
    { 
      retval = tsc_i2c_write( ifc, &i2c);
      break;
    }
    case TSC_IOCTL_I2C_CMD:
    {
      retval = tsc_i2c_cmd( ifc, &i2c);
      break;
    }
    default:
    {
      return( -EINVAL);
    }
  }
  if( copy_to_user( (void *)arg, &i2c, sizeof( i2c)))
  {
    return( -EFAULT);
  }

  return( retval);
}

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * Function name : ioctl_semaphore
 * Prototype     :  unsigned int
 * Parameters    : pointer to IFC1211 device control structure
 *                 cmd   -> command code
 *                 arg   -> command argument
 * Return        : error/success
 *----------------------------------------------------------------------------
 * Description   : perform IFC1211 SEMAPHORE operations
 *
 *++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

int ioctl_semaphore(struct ifc1211_device *ifc, unsigned int cmd, unsigned long arg){
	int retval = 0;
	struct tsc_ioctl_semaphore  semaphore;
	retval = 0;

	if(copy_from_user(&semaphore, (void *)arg, sizeof(semaphore))){
		return(-EFAULT);
	}
	switch (cmd){
    	case TSC_IOCTL_SEMAPHORE_STATUS:
    	{
    		semaphore_status(ifc, &semaphore);
    		break;
    	}
    	case TSC_IOCTL_SEMAPHORE_GET:
    	{
    		retval = tsc_semaphore_get(ifc, &semaphore);
    		break;
    	}
    	case TSC_IOCTL_SEMAPHORE_RELEASE:
    	{
    		tsc_semaphore_release(ifc, &semaphore);
    		break;
    	}
    	default:
    	{
    		return(-EINVAL);
    	}
	}
	if(copy_to_user((void *)arg, &semaphore, sizeof(semaphore))){
		return(-EFAULT);
	}

	return(retval);
}
