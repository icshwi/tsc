/*=========================< begin file & file header >=======================
 *  References
 *  
 *    filename : scope1430lib.c
 *    author   : JFG, XP
 *    company  : IOxOS
 *    creation : october 18,2018
 *
 *----------------------------------------------------------------------------
 *  Description
 *
 *    That library contains a set of function to access the TOSCA II XUSER
 *    SCOPE DAQ1430 data acquisition logic.
 *
 *----------------------------------------------------------------------------
 *
 *  Copyright (C) IOxOS Technologies SA <ioxos@ioxos.ch>
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

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <tsculib.h>
#include <tscioctl.h>
#include <tscextlib.h>
#include <scope1430lib.h>
#include <errno.h>

/* ------------------------------------------------------------------------------------------------------------------ */

#define SCOPE1430_WORKBUF_SIZE    (64*1024)

/* ------------------------------------------------------------------------------------------------------------------ */

/* scope base register depending on scope unit */
static int scope1430_regs_base[] =
{
  SCOPE1430_CSR_SRAM1_CS, SCOPE1430_CSR_SRAM2_CS, SCOPE1430_CSR_SMEM1_CS, SCOPE1430_CSR_SMEM2_CS 
};

/* hardware buffer mapped */
static int                        scope1430_hwbuf_mapped[4] = {0};            /* hardware buffer mapped   */
static void *                     scope1430_hwbuf_ptr[4] = {NULL};            /* hardware buffer pointer  */

static struct tsc_ioctl_map_win   scope1430_hwbuf_map_win[4];                 /* hardware buffer map      */

static void *                     scope1430_workbuf = NULL;                   /* work buffer              */
static int                        scope1430_workbuf_size = 0;


/* ------------------------------------------------------------------------------------------------------------------ */

/*!
 * \brief   Release all allocated ressources
 */

void scope1430_cleanup(int fd)
{
  int i;
  
  for (i=0; i<4; i++)
  {
    scope1430_unmap_hwbuf(fd, i);
  }
  
  if (scope1430_workbuf != NULL)
  {
    free(scope1430_workbuf);
    scope1430_workbuf = NULL;
    scope1430_workbuf_size = 0;
  }
}

/* ------------------------------------------------------------------------------------------------------------------ */

/*!
 * \brief   Read a XUSER_SCOPE register data value given it register index
 *
 * \param   csr       register index (between 0x60 - 0x7F)
 * \param   data      pointer on variable that will received the data read
 * 
 * \return  >=0 on succes and <0 on error
 */

int scope1430_csr_rd(int fd, int csr, int *data)
{  
  return tsc_csr_read(fd, SCOPE1430_CSR_ADDR(csr), data);  
}

/* ------------------------------------------------------------------------------------------------------------------ */

/*!
 * \brief   Write a XUSER_SCOPE register data value given it register index
 *
 * \param   csr       register index (between 0x60 - 0x7F)
 * \param   data      variable to write into register
 * 
 * \return  >=0 on succes and <0 on error
 */

int scope1430_csr_wr(int fd, int csr, int data)
{
  return tsc_csr_write(fd, SCOPE1430_CSR_ADDR(csr), &data);
}

/* ------------------------------------------------------------------------------------------------------------------ */

/*!
 * \brief   Identify XUSER_SCOPE_1430
 *
 * \return  <0 on error, =1 expected signature, =0 invalid signature
 */

int scope1430_identify(int fd)
{
  int id, ret;

  ret = scope1430_csr_rd(fd, SCOPE1430_CSR_SIGN, &id);
  if (ret < 0)
  {
    return(ret);
  }

  return ((id == SCOPE1430_SIGN) ? 1 : 0);
}

/* ------------------------------------------------------------------------------------------------------------------ */

/*!
 * \brief   Initialize SMEM1 or SMEM2 scope unit
 *
 * \param   unit          scope unit SMEM1 or SMEM2
 * \param   base_addr     Base address (only bit 29 to 19 are used)
 * \param   buf_size      size in MByte (0 = 256 MBytes)
 * \param   burst_size    burst size (512, 1k, 2k or 4k)
 *
 * \return  <0 on error or >=0 on success
 */

int scope1430_smem_init(int fd, int unit, int base_addr, int buf_size, int burst_size)
{
  int csr, data;
  
  /* unmap hardware buffer */
  scope1430_unmap_hwbuf(fd, unit);
  
  csr = (((unit&1) == 0) ? SCOPE1430_CSR_SMEM1_BASE : SCOPE1430_CSR_SMEM2_BASE);

  data = ((base_addr & 0x3FF80000) | (burst_size & (3<<8)) | (buf_size & 0xff));

  return scope1430_csr_wr(fd, csr, data);
}

/* ------------------------------------------------------------------------------------------------------------------ */

/*!
 * \brief   Configure legacy mode for SMEM1 or SMEM2
 *
 * \param   unit          scope unit SMEM1 or SMEM2
 * \param   ch_a          channel A selection (0 to 7)
 * \param   ch_b          channel B selection (0 to 7)
 * \param   down_sampling down-sampling factor
 * \returns <0 on error or >=0 on success
 */

int scope1430_smem_set_legacy_mode(int fd, int unit, int ch_a, int ch_b, int down_sampling)
{
  int csr, data, tmp, ret;
  
  csr = (((unit&1) == 0) ? SCOPE1430_CSR_SMEM1_CS : SCOPE1430_CSR_SMEM2_CS);
  
  ret = scope1430_csr_rd(fd, csr, &tmp);
  if (ret < 0)
  {
    return(ret);
  }

  data = (tmp & ~((7<<2) | (7<<12) | (7<<16)));

  data |= ((down_sampling&7)<<4);   /* down_sampling factor */
  data |= ((ch_a&7)<<12);           /* channel a selection  */
  data |= ((ch_b&7)<<16);           /* channel b selection  */
  
  ret = scope1430_csr_wr(fd, csr, data);

  return(ret);
}

/* ------------------------------------------------------------------------------------------------------------------ */

/*!
 * \brief   Configure new mode for SMEM1 or SMEM2
 *
 * \param   unit          scope unit SMEM1 or SMEM2
 * \param   down_sampling down-sampling factor (1:4 to 1:256)
 *
 * \return  <0 on error or >=0 on success
 */

int scope1430_smem_set_new_mode(int fd, int unit, int down_sampling)
{
  int csr, data, tmp, ret;

  csr = (((unit&1) == 0) ? SCOPE1430_CSR_SMEM1_CS : SCOPE1430_CSR_SMEM2_CS);
  
  /* 1:1 is not available for new mode */
  if (down_sampling == 0)
  {
    return(-1);
  }
  
  ret = scope1430_csr_rd(fd, csr, &tmp);
  if (ret < 0)
  {
    return(ret);
  }

  data = (tmp & ~((7<<2) | (7<<12) | (7<<16)));
  data |= ((down_sampling&7)<<4);     /* down-sampling factor   */
  data |= (1<<15);                    /* new mode               */

  ret = scope1430_csr_wr(fd, csr, data);

  return(ret);
}

/* ------------------------------------------------------------------------------------------------------------------ */

/*!
 * \brief   Initialize SRAM1 or SRAM2 scope unit
 *
 * \param   unit          scope unit SRAM1 or SRAM2
 * \param   buf_size      buf_size 
 *
 * \return  <0 on error or >=0 on success
 */

int scope1430_dpram_init(int fd, int unit, int buf_size)
{
  int csr, data, tmp, ret;
  
  csr = (((unit&1) == 0) ? SCOPE1430_CSR_SRAM1_CS : SCOPE1430_CSR_SRAM2_CS);

  ret = scope1430_csr_rd(fd, csr, &tmp);
  if (ret<0)
  {
    return(ret);
  }
  data = ((tmp & ~(7<<12)) | ((buf_size & 7)<<12));

  ret = scope1430_csr_wr(fd, csr, data);

  return(ret);
}

/* ------------------------------------------------------------------------------------------------------------------ */

/*!
 * \brief   Configure down-sampling mode for SRAM1 or SRAM2
 *
 * \param   unit          scope unit SRAM1 or SRAM2
 * \param   down_sampling down-sampling factor
 *
 * \return  <0 on error or >=0 on success
 */

int scope1430_dpram_set_mode(int fd, int unit, int down_sampling)
{
  int csr, data, tmp, ret;
  
  csr = (((unit&1) == 0) ? SCOPE1430_CSR_SRAM1_CS : SCOPE1430_CSR_SRAM2_CS);

  ret = scope1430_csr_rd(fd, csr, &tmp);
  if (ret < 0)
  {
    return(ret);
  }

  data = (tmp & ~((7<<2) | (1<<15)));

  data |= ((down_sampling&7)<<4);       /* down-sampling factor   */
  data |= (((down_sampling>>3)&1)<<15); /* down-sampling msb bit  */

  ret = scope1430_csr_wr(fd, csr, data);

  return(ret);
}

/* ------------------------------------------------------------------------------------------------------------------ */

/*!
 * \brief   Map hardware buffer of unit
 *
 * \param   unit          scope unit SRAM1, SRAM2, SMEM1 or SMEM2
 *
 * \return  <0 on error or >=0 on success
 */

int scope1430_map_hwbuf(int fd, int unit)
{
  int ret=0;
  if (scope1430_hwbuf_mapped[(unit&3)] == 0)
  {
    ret = scope1430_map(fd, unit, &scope1430_hwbuf_map_win[(unit&3)], &scope1430_hwbuf_ptr[(unit&3)], 0, 0);
    if (ret >= 0)
    {
      scope1430_hwbuf_mapped[(unit&3)] = 1;
    }
  }
  return(ret);
}

/* ------------------------------------------------------------------------------------------------------------------ */

/*!
 * \brief   Returns a pointer on tsc_ioctl_map_win strcture if allocated
 *
 * \param   unit          scope unit SRAM1, SRAM2, SMEM1 or SMEM2
 *
 * \return  NULL on error or valid pointer on success
 */

struct tsc_ioctl_map_win *scope1430_hwbuf_get_map_win(int unit)
{
  if (scope1430_hwbuf_mapped[(unit&3)] != 0)
  {
    return &scope1430_hwbuf_map_win[(unit&3)];
  }
  return(NULL);
} 

/* ------------------------------------------------------------------------------------------------------------------ */

/*!
 * \brief   Unmap hardware buffer of unit
 *
 * \param   unit          scope unit SRAM1, SRAM2, SMEM1 or SMEM2
 *
 * \return  <0 on error or >=0 on success
 */

void scope1430_unmap_hwbuf(int fd, int unit)
{
  if (scope1430_hwbuf_mapped[(unit&3)] != 0)
  {
    scope1430_unmap(fd, &scope1430_hwbuf_map_win[(unit&3)], &scope1430_hwbuf_ptr[(unit&3)]);
    scope1430_hwbuf_mapped[(unit&3)] = 0;
  }
}

/* ------------------------------------------------------------------------------------------------------------------ */

/*!
 * \brief   Map scope buffer into user space
 *
 * \param   unit          scope unit SRAM1, SRAM2, SMEM1 or SMEM2
 * \param   map           pointer to a memory map structure
 * \param   buf           pointer to a variable pointer to mapped buffer
 * \param   offset        offset in number of bytes
 * \param   size          size in number of bytes (0 = whole buffer)
 *
 * \return  <0 on error or >=0 on success
 */

int scope1430_map(int fd, int unit, struct tsc_ioctl_map_win *map, void **buf, int offset, int size)
{
  int ret, data, buf_start, buf_size;

  memset((void *)map, 0, sizeof(struct tsc_ioctl_map_win));
  
  if (offset < 0 || size < 0)
  {
    return(-1);
  }
  
  switch(unit)
  {
    case SCOPE1430_UNIT_SRAM1:
      buf_start = SCOPE1430_MAP_USR_OFF;
      buf_size  = SCOPE1430_MAP_USR_SIZE;
      map->req.mode.space = MAP_SPACE_USR1;
      break;
    
    case SCOPE1430_UNIT_SRAM2:
      buf_start = SCOPE1430_MAP_USR_OFF;
      buf_size  = SCOPE1430_MAP_USR_SIZE;
      map->req.mode.space = MAP_SPACE_USR2;
      break;
            
    case SCOPE1430_UNIT_SMEM1:
      
      ret = scope1430_csr_rd(fd, SCOPE1430_CSR_SMEM1_BASE, &data);
      if (ret < 0) return(ret);

      buf_start = (data & 0x3FF00000);
      buf_size  = (((data & 0xff) == 0) ? 0x10000000 : ((data & 0xff) * 0x100000));
      map->req.mode.space = MAP_SPACE_SHM1;
      break;

    case SCOPE1430_UNIT_SMEM2:
      ret = scope1430_csr_rd(fd,SCOPE1430_CSR_SMEM1_BASE, &data);
      if (ret < 0) return(ret);

      buf_start = (data & 0x3FF00000);
      buf_size  = (((data & 0xff) == 0) ? 0x10000000 : ((data & 0xff) * 0x100000));
      map->req.mode.space = MAP_SPACE_SHM2;
      break;
      
    default:
      return(-1);
  }
  
  if ((offset+size) > buf_size)
  {
    return(-1);
  }

  map->req.rem_addr   = (buf_start + offset);
  map->req.size       = ((size == 0) ? buf_size : size);
  map->req.mode.sg_id = MAP_ID_MAS_PCIE_MEM;

  ret = tsc_map_alloc(fd, map);
  if (ret < 0)
  {
    fprintf(stderr, "cannot allocate user map %lx, %x [err: %d-%d]\n",  map->sts.loc_base, map->sts.size, ret, errno);
    (*buf) = NULL;
    return(-1);
  }
  
  (*buf) = tsc_pci_mmap(fd, map->sts.loc_base, map->sts.size);
  
  return(1);
}

/* ------------------------------------------------------------------------------------------------------------------ */

/*!
 * \brief   Unmap scope buffer from user space
 *
 * \param   map           pointer to a memory map structure
 * \param   u_addr        pointer to mapped buffer
 *
 * \return  <0 on error or >=0 on success
 */

void scope1430_unmap(int fd, struct tsc_ioctl_map_win *map, void *u_addr)
{
  tsc_pci_munmap(u_addr, map->sts.size);
  tsc_map_free(fd, map);
}

/* ------------------------------------------------------------------------------------------------------------------ */

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * Function name : fscope3112_acq
 * Prototype     : int
 * Parameters    : fmc
 * Return        : fail/success
 *----------------------------------------------------------------------------
 * Description   : 
 *
 */

void scope1430_acq_init(int fd)
{
  scope1430_csr_wr(fd, SCOPE1430_CSR_INTR_CTL, 0);
}

/* ------------------------------------------------------------------------------------------------------------------ */

/*!
 * \brief   return the current status of the acquisition
 *
 * \param   status        pointer to a variable to receive status 
 *
 * \return  <0 on error or >=0 on success
 */

int scope1430_intr_status(int fd, int *status)
{
  int data, ret;
  
  ret = scope1430_csr_rd(fd, SCOPE1430_CSR_INTR_STA, &data);
  
  if (status != NULL)
  {
    (*status) = data;
  }
  
  return (ret);
}

/* ------------------------------------------------------------------------------------------------------------------ */

/*!
 * \brief   return the current status of the acquisition
 *
 * \param   unit          scope unit SRAM1, SRAM2, SMEM1 or SMEM2
 * \param   status        pointer to a variable to receive status 
 *
 * \return  <0 on error or >=0 on success
 */

int scope1430_acq_status(int fd, int unit, int * status)
{
  int csr;
  
  csr = scope1430_regs_base[(unit&3)];
  
  return scope1430_csr_rd(fd, csr+0, status);
}

/* ------------------------------------------------------------------------------------------------------------------ */

/*!
 * \brief   Set ADC trigger mode
 *
 * \param   unit          scope unit SRAM1, SRAM2, SMEM1 or SMEM2
 * \param   channel       Channel number (0-7 for SRAM1 or SMEM1 & 8-9 for SRAM2 or SMEM2)
 * \param   level         level
 * \param   pol           polarity (0 = negative, 1 = positive)
 *
 * \return  <0 on error or >=0 on success
 */

int scope1430_trigger_set_adc_mode(int fd, int unit, int channel, int level, int pol)
{
  int csr, data;
  
  csr = scope1430_regs_base[(unit&3)]+1;
  
  data = (SCOPE1430_TRIG_ENABLE         |
          SCOPE1430_TRIG_ADC(channel&7) |
          SCOPE1430_TRIG_ADC_SEL        |
          SCOPE1430_TRIG_LEVEL(level)   |
          (pol ? SCOPE1430_TRIG_POL_POS : SCOPE1430_TRIG_POL_NEG));
  
  return scope1430_csr_wr(fd, csr, data);
}

/* ------------------------------------------------------------------------------------------------------------------ */

/*!
 * \brief   Set GPIO trigger mode
 *
 * \param   unit          scope unit SRAM1, SRAM2, SMEM1 or SMEM2
 * \param   gpio          GPIO number
 * \param   pol           polarity (0 = negative edge, 1 = positive edge)
 *
 * \return  <0 on error or >=0 on success
 */

int scope1430_trigger_set_gpio_mode(int fd, int unit, int gpio, int pol)
{
  int csr, data;
  
  csr = scope1430_regs_base[(unit&3)]+1;
  
  data = (SCOPE1430_TRIG_ENABLE           |
          SCOPE1430_TRIG_GPIO(gpio&0x7f)  |
          SCOPE1430_TRIG_GPIO_SEL         |
          (pol ? SCOPE1430_TRIG_POL_POS : SCOPE1430_TRIG_POL_NEG));
  
  return scope1430_csr_wr(fd, csr, data);
}

/* ------------------------------------------------------------------------------------------------------------------ */

/*!
 * \brief   Disable trigger
 *
 * \param   unit          scope unit SRAM1, SRAM2, SMEM1 or SMEM2
 *
 * \return  <0 on error or >=0 on success
 */

int scope1430_trigger_disable(int fd, int unit)
{
  int csr;
  
  csr = scope1430_regs_base[(unit&3)]+1;
  
  return scope1430_csr_wr(fd, csr, 0);
}

/* ------------------------------------------------------------------------------------------------------------------ */

/*!
 * \brief   Set trigger position (pre-/post- trigger)
 *
 * \param   unit          scope unit SRAM1, SRAM2, SMEM1 or SMEM2
 * \param   pos           trigger position (0/8, 1/8, 1/4, 3/8, 1/2, 5/8, 3/4, 7/8)
 *
 * \return  <0 on error or >=0 on success
 */

int scope1430_trigger_set_pos(int fd, int unit, int pos)
{
  int csr, tmp, data, ret;

  csr = scope1430_regs_base[(unit&3)]+0;
  
  ret = scope1430_csr_rd(fd, csr, &tmp);
  if (ret < 0)
  {
    return (ret);
  }

  data = ((tmp & ~(SCOPE1430_TRIG_POS_MASK | SCOPE1430_CMD_MASK)) | ((pos&7)<<5));

  ret = scope1430_csr_wr(fd, csr, data);

  return(ret);
}

/* ------------------------------------------------------------------------------------------------------------------ */

/*!
 * \brief   Return pre-trigger position
 *
 * \param   unit          scope unit SRAM1, SRAM2, SMEM1 or SMEM2
 *
 * \return  <0 on error or >=0 on success
 */

int scope1430_pretrigger_get_pos(int fd, int unit)
{
  int csr, tmp, data, ret;

  csr = scope1430_regs_base[(unit&3)];

  /* LAST_ADDR */
  ret = scope1430_csr_rd(fd, csr+2, &data);
  if (ret < 0)
  {
    return (ret);
  }

  if (unit == SCOPE1430_UNIT_SMEM1 || unit == SCOPE1430_UNIT_SMEM2)
  {
    ret = scope1430_csr_rd(fd, csr+3, &tmp);
    if (ret < 0)
    {
      return(ret);
    }
    tmp &= 0x3FF00000;
    data -= tmp;
  }

  return(data);
}

/* ------------------------------------------------------------------------------------------------------------------ */

/*!
 * \brief   Generate a manual trigger
 *
 * \param   unit          scope unit SRAM1, SRAM2, SMEM1 or SMEM2
 *
 * \return  <0 on error or >=0 on success
 */

int scope1430_manual_trigger(int fd, int unit)
{
  int csr;
  
  csr = scope1430_regs_base[(unit&3)]+2;

  return scope1430_csr_wr(fd, csr, SCOPE1430_TRIG_MAN);
}

/* ------------------------------------------------------------------------------------------------------------------ */

/*!
 * \brief   Generate a manual trigger
 *
 * \param   unit          scope unit SRAM1, SRAM2, SMEM1 or SMEM2
 * \param   mode          acquisition mode (SINGLE / CONTINOUS, NORMAL / AUTO)
 * 
 * \return  <0 on error or >=0 on success
 */

int scope1430_acq_set_mode(int fd, int unit, int mode)
{
  int csr, tmp, data, ret;

  csr = scope1430_regs_base[(unit&3)]+0;
  
  ret = scope1430_csr_rd(fd, csr, &tmp);
  if (ret < 0)
  {
    return (ret);
  }

  data = ((tmp & ~(SCOPE1430_TRIG_POS_MASK | SCOPE1430_CMD_MASK | SCOPE1430_MODE_MASK)) | (mode & SCOPE1430_MODE_MASK));

  return scope1430_csr_wr(fd, csr, data);
}

/* ------------------------------------------------------------------------------------------------------------------ */

/*!
 * \brief   ARM or REARM acquisition
 *
 * \param   unit          scope unit SRAM1, SRAM2, SMEM1 or SMEM2
 *
 * \return  <0 on error or >=0 on success
 */

int scope1430_acq_arm(int fd, int unit)
{
  int csr, tmp, data, ret;

  csr = scope1430_regs_base[(unit&3)]+0;
  
  ret = scope1430_csr_rd(fd, csr, &tmp);
  if (ret < 0)
  {
    return(ret);
  }

  data = ((tmp & ~(SCOPE1430_CMD_MASK)) | SCOPE1430_CMD_REARM);

  ret = scope1430_csr_wr(fd, csr, data);

  return(ret);
}

/* ------------------------------------------------------------------------------------------------------------------ */

/*!
 * \brief   STOP acquisition
 *
 * \param   unit          scope unit SRAM1, SRAM2, SMEM1 or SMEM2
 *
 * \return  <0 on error or >=0 on success
 */

int scope1430_acq_stop(int fd, int unit)
{
  int csr, tmp, data, ret;

  csr = scope1430_regs_base[(unit&3)]+0;
  
  ret = scope1430_csr_rd(fd, csr, &tmp);
  if (ret < 0)
  {
    return(ret);
  }

  data = ((tmp & ~(SCOPE1430_CMD_MASK)) | SCOPE1430_CMD_STOP);

  ret = scope1430_csr_wr(fd, csr, data);

  return(ret);
}

/* ------------------------------------------------------------------------------------------------------------------ */

/*!
 * \brief   clear acquisition buffers
 *
 * \param   unit          scope unit SRAM1, SRAM2, SMEM1 or SMEM2
 *
 * \return  <0 on error or >=0 on success
 */

int scope1430_acq_clear(int fd, int unit)
{
  int ret;
  
  ret = scope1430_map_hwbuf(fd, unit);

  if (ret >= 0)
  {
    memset(scope1430_hwbuf_ptr[(unit&3)], 0, scope1430_hwbuf_map_win[(unit&3)].req.size);
  }
  
  return(ret);
}

/* ------------------------------------------------------------------------------------------------------------------ */

/*!
 * \brief   return the size of hardware buffer in bytes
 *
 * \param   unit          scope unit SRAM1, SRAM2, SMEM1 or SMEM2
 *
 * \return  <0 on error or >=0 on success
 */

int scope1430_hwbuf_get_size(int fd, int unit)
{ 
  int size, ret, data;
  
  switch(unit)
  {
    case SCOPE1430_UNIT_SRAM1:
    case SCOPE1430_UNIT_SRAM2:
      size  = SCOPE1430_MAP_USR_SIZE;
      break;
                
    case SCOPE1430_UNIT_SMEM1:
    case SCOPE1430_UNIT_SMEM2:
      ret = scope1430_csr_rd(fd, scope1430_regs_base[(unit&3)]+3, &data);
      if (ret < 0) return(ret);

      size  = (((data & 0xff) == 0) ? 0x10000000 : ((data & 0xff) * 0x100000));
      break;
      
    default:
      return(-1);
  }
  return(size);
}

/* ------------------------------------------------------------------------------------------------------------------ */

/*!
 * \brief   return the size of sample buffer in bytes for one channel
 *
 * \param   unit          scope unit SRAM1, SRAM2, SMEM1 or SMEM2
 *
 * \return  <0 on error or >=0 on success
 */

int scope1430_sbuf_get_size(int fd, int unit)
{ 
  int size=0, ret, data;

  ret = scope1430_csr_rd(fd, scope1430_regs_base[(unit&3)]+0, &data);
  if (ret < 0)
  {
    return(ret);
  }
  
  switch(unit)
  {
    case SCOPE1430_UNIT_SRAM1:
    case SCOPE1430_UNIT_SRAM2:
      switch((data>>14) & 7)
      {
        case 0:
          /* SRAM Buffer Size 64KB or 32KB ? */
          size = ((data & (1<<11)) ? 0x10000 : 0x8000);  /* 64KB or 32KB */
          break;
          
        case 1:
          size = 0x8000;  /* 32KB */
          break;
          
        case 2:
          size = 0x4000;  /* 16KB */
          break;

        case 3:
          size = 0x2000;  /* 8KB */
          break;

        case 4:
          size = 0x1000;  /* 4KB */
          break;

        case 5:
          size = 0x800;  /* 2KB */
          break;
        
        default:
          break;
      }
      break;
                
    case SCOPE1430_UNIT_SMEM1:
    case SCOPE1430_UNIT_SMEM2:
      size = scope1430_hwbuf_get_size(fd, unit);
      if (size < 0)
      {
        return(size);
      }
      /* new mode (0, 1, 2, 3, 4, 5, 6, 7) */
      if (((data>>4)&7) != 0 && (data & (1<<15)))
      {
        size /= 8;
      }
      /* legacy mode (a, b) */
      else
      {
        size /= 2;
      }
      break;
      
    default:
      return(-1);
  }
  return(size);
}

/* ------------------------------------------------------------------------------------------------------------------ */

/*!
 * \brief   extract sample of a channel from sample buffer
 *
 * \param   dst           destination buffer
 * \param   src           source buffer
 * \param   dst_size      destination buffer size in number of bytes
 * \param   pre_start     pre-trigger start offset (in byte)
 * \param   pre_size      size of pre-trigger area in number of bytes
 * \param   off           channel offset in number of samples
 * \param   incr          increment in number of samples to the next channel's samples
 * \param   count         number of samples per channel
 * \param   swap          byte swap flag
 *
 */


static void scope1430_acq_extract (
  uint16_t *dst, 
  uint16_t *src, 
  int       dst_size, 
  int       pre_start, 
  int       pre_size, 
  int       off, 
  int       incr, 
  int       count, 
  int       swap
)
{
  uint16_t *p, *q, v;
  int n, s, pos;
  
  p = dst;
  q = src;

  pos = (pre_start / 2) + off;
  for (n=0; n<(dst_size / 2); n+=count)
  {
    for (s=0; s<count; s++)
    {
      v = q[pos+s];
    
      if (swap)
      {
        v = (((v>>8)&0xff) | ((v<<8)&0xff00));
      }
      p[n+s] = v;
    }
    if (n < (pre_size/2))
    {
      pos = (pos + incr) % (pre_size / 2);
    }
    else
    {
      pos += incr;
    }
  }
}

/* ------------------------------------------------------------------------------------------------------------------ */

/*!
 * \brief   returns a sample buffer from hardware buffer
 *
 * \param   unit          scope unit SRAM1, SRAM2, SMEM1 or SMEM2
 * \param   chan          channel (0,1 for legacy smem mode or 0-7 other)
 * \param   sbuf_ptr      sample buffer pointer (allocate by this function)
 * \param   sbuf_size     sample buffer size 
 * \param   mode          mode (0 = raw without using chan, 1 = extract samples
 *                              for only one channel)
 *
 * \return  <0 on error or >=0 on success
 */

int scope1430_acq_read(int fd, int unit, int chan, uint16_t ** sbuf_ptr, int *sbuf_size, int mode)
{
  int ret, hwbuf_size, buf_size, tmp, count, off, incr, pre_size, pre_end, pre_start;
  uint16_t *buf_ptr;

  if (sbuf_ptr == NULL)
  {
    return (-1);
  }
  
  buf_ptr = (*sbuf_ptr);

  /* map hardware buffer if needed */
  ret = scope1430_map_hwbuf(fd, unit);
  if (ret < 0)
  {
    return(ret);
  }
  
  /* get size of hardware buffer */
  hwbuf_size = scope1430_hwbuf_get_size(fd, unit);
  if (hwbuf_size < 0)
  {
    return(hwbuf_size);
  }

  /* raw mode */
  if (mode == 0)
  {
    /* allocate if memory is not already allocated */
    buf_ptr = (uint16_t *)realloc((void *)buf_ptr, hwbuf_size);
    if (buf_ptr != NULL)
    {
      memcpy((void *)buf_ptr, scope1430_hwbuf_ptr[(unit&3)], hwbuf_size);
    }
    if (sbuf_size != NULL)
    {
      (*sbuf_size) = ((buf_ptr != NULL) ? hwbuf_size : 0);
    }
    (*sbuf_ptr) = buf_ptr;
  }
  /* extract samples mode */
  else
  {
    buf_size = scope1430_sbuf_get_size(fd, unit);
    
    ret = scope1430_csr_rd(fd, scope1430_regs_base[(unit&3)]+0, &tmp);
    if (ret < 0)
    {
      return(ret);
    }
    pre_size = ((tmp >> 5) & 7) * (buf_size/8);
    if (pre_size !=0)
    {
      pre_end = scope1430_pretrigger_get_pos(fd, unit);
      if (pre_end < 0)
      {
        return(pre_end);
      }
      pre_start = ((pre_end+1) % pre_size);
    }
    else
    {
      pre_start = pre_end = 0;
    }

    /* allocate work buffer */
    scope1430_workbuf = realloc(scope1430_workbuf, hwbuf_size);
    if (scope1430_workbuf != NULL)
    {
      scope1430_workbuf_size = hwbuf_size;
    }
    
    /* copy hardware buffer to work buffer */
    memcpy(scope1430_workbuf, scope1430_hwbuf_ptr[(unit&3)], hwbuf_size); 


    if (unit == SCOPE1430_UNIT_SRAM1 || unit == SCOPE1430_UNIT_SRAM2)
    {
      off = (((SCOPE1430_MAP_USR_SIZE/8) * (chan&7)) / 2);
      incr = 1;
      count = 1;
    }
    /* SCOPE1430_UNIT_SMEM1 || SCOPE1430_UNIT_SMEM2 */
    else
    {
      /* legacy mode */
      if (hwbuf_size == (2*buf_size))
      {
        off = 2*(chan&1);
        incr = 4;
        count = 2;
      }
      /* new mode */
      else
      {
        off = 8 * (chan&7);
        incr = 16;
        count = 2;
      }
    }
    /* allocate if memory is not already allocated */
    if (buf_ptr == NULL)
    {
      buf_ptr = (uint16_t *)malloc(buf_size);
    }
    if (buf_ptr != NULL)
    {
      scope1430_acq_extract(buf_ptr, (uint16_t *)scope1430_workbuf, buf_size, pre_start, pre_size, off, incr, count, 1);
    }
    if (sbuf_size != NULL)
    {
      (*sbuf_size) = ((buf_ptr != NULL) ? buf_size : 0);
    }
    (*sbuf_ptr) = buf_ptr;
  }
  return(1);
}

/* ------------------------------------------------------------------------------------------------------------------ */

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * Function name : fscope3112_acq_save
 * Prototype     : int
 * Parameters    : fmc
 *                 channel
 *                 filename
 * Return        : fail/success
 *----------------------------------------------------------------------------
 * Description   : 
 *
 */
/*
int scope1430_acq_save(int unit, int chan, char *filename)
{
  unsigned short *buf;
  int i;
  FILE *acq_file;
  int buf_size;

  acq_file = fopen(filename, "w");
  if( !acq_file)
  {
    return( -1);
  }
  buf = (unsigned short *)malloc( FSCOPE3112_ACQ_SIZE);
  buf_size = fscope3112_acq_read( fmc, chan, buf, FSCOPE3112_ACQ_SIZE);
  for( i = 0; i < buf_size/2; i++)
  {
    fprintf( acq_file, "%d, %d\n", i, (int)buf[i] - 0x800);
  }
  fclose( acq_file);
  free( buf);
  return( 0);
}
*/
/* ------------------------------------------------------------------------------------------------------------------ */

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * Function name : fscope3112_acq_load
 * Prototype     : int
 * Parameters    : fmc
 * Return        : fail/success
 *----------------------------------------------------------------------------
 * Description   : clear acquisition buffers
 *
 */
/*
void scope1430_acq_load(int fmc, int chan,  char *buf, int size)
{
  struct tsc_ioctl_map_win *usr_map;
  char *adc_buf;
  int i, start;

  start =  FSCOPE3112_ACQ_SIZE*chan;
  if( size > FSCOPE3112_ACQ_SIZE)
  {
    size = FSCOPE3112_ACQ_SIZE;
  }
  usr_map = fscope3112_map_usr( fmc, &adc_buf, start, size);
  for( i = 0; i < size; i += 4)
  {
    *(int *)&buf[i] =*(int *)&adc_buf[i];
  }
  fscope3112_unmap_usr( usr_map, adc_buf);

  return;
}
*/
