/*=========================< begin file & file header >=======================
 *  References
 *  
 *    filename : zdclib.c
 *    author   : JFG, XP
 *    company  : IOxOS
 *    creation : october 18,2018
 *
 *----------------------------------------------------------------------------
 *  Description
 *
 *    That library contains a set of function to access the TOSCA II XUSER
 *    FASTSCOPE ZDC data acquisition logic.
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
#include <errno.h>
#include <tsculib.h>
#include <tscextlib.h>
#include <zdclib.h>
#include <gscopelib.h>

int zdc_verbose_flag = 0;

int 
zdc_set_verbose( int vf)
{
  zdc_verbose_flag = vf;
  return(zdc_verbose_flag);
}

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * Function name : zdc_XXX
 * Prototype     : int
 * Parameters    : void
 * Return        : 
 *----------------------------------------------------------------------------
 * Description   : 
 *
 *++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

int 
zdc_XXX( void)
{

  return( 0);
}

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * Function name : zdc_identify
 * Prototype     : int
 * Parameters    : none
 * Return        : zdc signature (expect ZDC_SIGN_ID)
 *----------------------------------------------------------------------------
 * Description   : returns the content of zdc signature register 
 * (ZDC_CSR_SIGN1 at index 0x60)
 *
 *++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

int zdc_identify(void) {
	int id;

	id = ZDC_SIGN_ID;

	return (id);
}

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * Function name : zdc_smem_init
 * Prototype     : int
 * Parameters    : none
 * Return        : 0
 *----------------------------------------------------------------------------
 * Description   : initialize GSCOPE registers for SMEM data acquisition 
 *
 *++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

int zdc_smem_init(int fd, int chan, int size) 
{
  int csr;
  /* Initialize Multiplexer FMC1 */
  gscope_csr_wr(fd, GSCOPE_CSR_MAP1, 0x00000000);

  /* SMEM1 init ring buffers group 1 (A,B,C,D) */
  gscope_csr_wr(fd, GSCOPE_CSR_SWR1_RGB_CFG, 0x10020000); /* select RGBUF0 MEM1          */
  gscope_csr_wr(fd, GSCOPE_CSR_SWR1_RGB_BAS, 0x10000000); /* base 0x10000000/0x11000000  */
  gscope_csr_wr(fd, GSCOPE_CSR_SWR1_RGB_CFG, 0x10020001); /* select RGBUF1 MEM1          */
  gscope_csr_wr(fd, GSCOPE_CSR_SWR1_RGB_BAS, 0x12000000); /* base 0x12000000/0x13000000  */
  gscope_csr_wr(fd, GSCOPE_CSR_SWR1_RGB_CFG, 0x10020002); /* select RGBUF2 MEM1          */
  gscope_csr_wr(fd, GSCOPE_CSR_SWR1_RGB_BAS, 0x14000000); /* base 0x14000000/0x15000000  */
  gscope_csr_wr(fd, GSCOPE_CSR_SWR1_RGB_CFG, 0x10020003); /* select RGBUF3 MEM1          */
  gscope_csr_wr(fd, GSCOPE_CSR_SWR1_RGB_BAS, 0x16000000); /* base 0x16000000/0x17000000  */
  gscope_csr_wr(fd, GSCOPE_CSR_SWR1_RGB_CFG, 0x10020004); /* select RGBUF4 MEM1          */
  gscope_csr_wr(fd, GSCOPE_CSR_SWR1_RGB_BAS, 0x00000000); /* base 0x00000000             */
  gscope_csr_wr(fd, GSCOPE_CSR_SWR1_RGB_CFG, 0x10020005); /* select RGBUF5 MEM1          */
  gscope_csr_wr(fd, GSCOPE_CSR_SWR1_RGB_BAS, 0x00000000); /* base 0x00000000             */
  gscope_csr_wr(fd, GSCOPE_CSR_SWR1_RGB_CFG, 0x10020006); /* select RGBUF6 MEM1          */
  gscope_csr_wr(fd, GSCOPE_CSR_SWR1_RGB_BAS, 0x00000000); /* base 0x00000000             */
  gscope_csr_wr(fd, GSCOPE_CSR_SWR1_RGB_CFG, 0x10020007); /* select RGBUF7 MEM1          */
  gscope_csr_wr(fd, GSCOPE_CSR_SWR1_RGB_BAS, 0x00000000); /* base 0x00000000             */

  gscope_csr_wr(fd, GSCOPE_CSR_SWR1_GEN_CTL, 0x00001166); /* SWR1_FMC_GEN_CTK : ADS0123 CLR='0' ADS0123 RUN='1' MODE ="01"  */

  /* Initialize Multiplexer FMC2 */
  gscope_csr_wr(fd, GSCOPE_CSR_MAP2, 0x00000000);

  /* SMEM2 init ring buffers group 1 (A,B,C,D) */
  gscope_csr_wr(fd, GSCOPE_CSR_SWR2_RGB_CFG, 0x10020000); /* select RGBUF0 MEM2          */
  gscope_csr_wr(fd, GSCOPE_CSR_SWR2_RGB_BAS, 0x10000000); /* base 0x10000000/0x11000000  */
  gscope_csr_wr(fd, GSCOPE_CSR_SWR2_RGB_CFG, 0x10020001); /* select RGBUF1 MEM2          */
  gscope_csr_wr(fd, GSCOPE_CSR_SWR2_RGB_BAS, 0x12000000); /* base 0x12000000/0x13000000  */
  gscope_csr_wr(fd, GSCOPE_CSR_SWR2_RGB_CFG, 0x10020002); /* select RGBUF2 MEM2          */
  gscope_csr_wr(fd, GSCOPE_CSR_SWR2_RGB_BAS, 0x14000000); /* base 0x14000000/0x15000000  */
  gscope_csr_wr(fd, GSCOPE_CSR_SWR2_RGB_CFG, 0x10020003); /* select RGBUF3 MEM2          */
  gscope_csr_wr(fd, GSCOPE_CSR_SWR2_RGB_BAS, 0x16000000); /* base 0x16000000/0x17000000  */
  gscope_csr_wr(fd, GSCOPE_CSR_SWR2_RGB_CFG, 0x10020004); /* select RGBUF4 MEM2          */
  gscope_csr_wr(fd, GSCOPE_CSR_SWR2_RGB_BAS, 0x00000000); /* base 0x00000000             */
  gscope_csr_wr(fd, GSCOPE_CSR_SWR2_RGB_CFG, 0x10020005); /* select RGBUF5 MEM2          */
  gscope_csr_wr(fd, GSCOPE_CSR_SWR2_RGB_BAS, 0x00000000); /* base 0x00000000             */
  gscope_csr_wr(fd, GSCOPE_CSR_SWR2_RGB_CFG, 0x10020006); /* select RGBUF6 MEM2          */
  gscope_csr_wr(fd, GSCOPE_CSR_SWR2_RGB_BAS, 0x00000000); /* base 0x00000000             */
  gscope_csr_wr(fd, GSCOPE_CSR_SWR2_RGB_CFG, 0x10020007); /* select RGBUF7 MEM2          */
  gscope_csr_wr(fd, GSCOPE_CSR_SWR2_RGB_BAS, 0x00000000); /* base 0x00000000             */


  /* enable FMUX1 & FMUX2 */

  switch( chan)
  {
    case 1:
    {
      gscope_csr_wr(fd, GSCOPE_CSR_FE1_CSR2, 0x98000003);     /* enable FMUX1 chan 0:1            */
      gscope_csr_wr(fd, GSCOPE_CSR_FE2_CSR2, 0xa8000001);     /* enable FMUX2 chan 1              */
      break;
    }
    case 2:
    {
      gscope_csr_wr(fd, GSCOPE_CSR_FE1_CSR2, 0xa8000003);     /* enable FMUX1 chan 0:2            */
      gscope_csr_wr(fd, GSCOPE_CSR_FE2_CSR2, 0xb8000001);     /* enable FMUX2 chan 2              */
      break;
    }
    case 3:
    {
      gscope_csr_wr(fd, GSCOPE_CSR_FE1_CSR2, 0xb8000003);     /* enable FMUX1 chan 0:3            */
      gscope_csr_wr(fd, GSCOPE_CSR_FE2_CSR2, 0xc8000001);     /* enable FMUX2 chan 3              */
      break;
    }
    default:
    {
      gscope_csr_wr(fd, GSCOPE_CSR_FE1_CSR2, 0x98000003);     /* enable FMUX1 chan 0:1            */
      gscope_csr_wr(fd, GSCOPE_CSR_FE2_CSR2, 0x98000001);     /* enable FMUX2 chan 0              */
      break;
    }
  }
  csr = size & GSCOPE_RGBUF_SIZE_MASK;
  /* enable SMEM1 */
  gscope_csr_wr(fd, GSCOPE_CSR_SWR1_RGB_CFG, csr | 0x80000000); /* select RGBUF0 MEM1          */
  gscope_csr_wr(fd, GSCOPE_CSR_SWR1_RGB_CFG, csr | 0xc0000010); /* select RGBUF0 MEM1          */

  /* enable SMEM2 */
  gscope_csr_wr(fd, GSCOPE_CSR_SWR2_RGB_CFG, csr | 0x80000000); /* select RGBUF0 MEM2          */
  gscope_csr_wr(fd, GSCOPE_CSR_SWR2_RGB_CFG, csr | 0xc0000010); /* select RGBUF0 MEM2          */

    return (0);
}

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * Function name : zdc_smem_load
 * Prototype     : int
 * Parameters    : src       data source
 *                 buf       pointer to destination buffer
 * 		   new_size  size to be transferred
 * Return        : fail/success
 *----------------------------------------------------------------------------
 * Description   : Save the datas from SMEM in a buffer of a
 *                 given size
 *
 *++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

int zdc_copy_buffer(int start, 
                    int end, 
                    int pos,
                    char *acq_buf,
		    char* output_buffer) 
{
  uint *s, *d;
  int i;

  d = (uint *)&output_buffer[pos];
  s = (uint *)&acq_buf[start];
  for (i = start; i < end; i += 4)
  {
    //*d = tsc_swap_64(*s);
    *d++ = *s++;
    pos += sizeof(uint);
  }

  return pos;
}

int 
zdc_smem_load(int fd, int src, char *buf, int new_size)
{
  int csr;
  int base, size, pos, trig, prop, pre_trig;
  int size_mask, pre_trig_mask;

  int extr, start, end, gap;
  struct tsc_ioctl_map_win shm_mas_map_win;
  char *acq_buf;

  if( src == ZDC_SRC_FIFO) 
  {
    csr = gscope_csr_rd(fd, GSCOPE_CSR_SWR2_RGB_CFG);
    csr &= GSCOPE_SAVE_CHAN_MASK;
    gscope_csr_wr(fd, GSCOPE_CSR_SWR2_RGB_CFG, csr);
    base = gscope_csr_rd(fd, GSCOPE_CSR_SWR2_RGB_BAS);
    trig = gscope_csr_rd(fd, GSCOPE_CSR_SWR2_TRIG_MKT);
    trig &= 0xfffff000; /* JFG */
    pre_trig = gscope_csr_rd(fd, GSCOPE_CSR_SWR2_ACQ_MGT);
  }
  else 
  {
    csr = gscope_csr_rd(fd, GSCOPE_CSR_SWR1_RGB_CFG);
    csr &= GSCOPE_SAVE_CHAN_MASK;
    if( src != ZDC_SRC_ADC0) 
    {
      csr |= 1;
    }
    gscope_csr_wr(fd, GSCOPE_CSR_SWR1_RGB_CFG, csr);
    base = gscope_csr_rd(fd, GSCOPE_CSR_SWR1_RGB_BAS);
    trig = gscope_csr_rd(fd, GSCOPE_CSR_SWR1_TRIG_MKT);
    trig &= 0xfffff000; /* JFG */
    pre_trig = gscope_csr_rd(fd, GSCOPE_CSR_SWR1_ACQ_MGT);
  }
  size_mask = GSCOPE_RGBUF_SIZE_MASK;
  size = csr & size_mask;
  if( !size) size = 0x10000000;
  printf("base = %x size = %x\n", base, size);

  pre_trig = pre_trig >> GSCOPE_ARM_TRIG_POS_START;

	//111...111
  pre_trig_mask = (1 << GSCOPE_ARM_TRIG_POS_SIZE) - 1;
  pre_trig = pre_trig & pre_trig_mask;
  prop = pre_trig;
  pre_trig *= size;
  pre_trig = pre_trig / GSCOPE_ARM_TRIG_POS_MAX;

  bzero(&shm_mas_map_win, sizeof(shm_mas_map_win));
  shm_mas_map_win.req.mode.sg_id = MAP_ID_MAS_PCIE_PMEM;
  shm_mas_map_win.req.mode.space = MAP_SPACE_SHM;
  if( src == ZDC_SRC_FIFO) 
  {
    shm_mas_map_win.req.mode.space = MAP_SPACE_SHM2;
  }
  shm_mas_map_win.req.rem_addr = base;
  shm_mas_map_win.req.size = size;
  tsc_map_alloc(fd, &shm_mas_map_win);
  acq_buf = (char*) tsc_pci_mmap(fd, shm_mas_map_win.sts.loc_base,
	                          shm_mas_map_win.sts.size); 
  if(new_size > size || new_size == 0)
  {
    new_size = size;
  }

  extr = (trig + size - pre_trig) % size;
  gap = size - new_size;

  start = (extr + ((gap * prop) / GSCOPE_ARM_TRIG_POS_MAX)) % size;
  end = (extr - ((gap * (GSCOPE_ARM_TRIG_POS_MAX - prop)) / GSCOPE_ARM_TRIG_POS_MAX) + size) % size;

  printf("trig = %x start = %x end = %x size = %x\n", trig, start, end, size);
  if(end <= trig || start >= trig)
  {
    //start = (start + 0xfff) & 0xff000;
    pos = zdc_copy_buffer(start, size, 0, acq_buf, buf);
    //end = (end + 0xfff) & 0xff000;
    pos = zdc_copy_buffer(0, end, pos, acq_buf, buf);
  }
  else
  {
    //start = (start + 0xfff) & 0xff000;
    //end = (end + 0xfff) & 0xff000;
    pos = zdc_copy_buffer(start, end, 0, acq_buf, buf);
  }
#ifdef JFG
  pos = zdc_copy_buffer(0, size, 0, acq_buf, buf);
  //pos = zdc_copy_buffer(trig, size, 0, acq_buf, buf);
#endif
    
  tsc_pci_munmap(acq_buf, shm_mas_map_win.sts.size);
  tsc_map_free(fd, &shm_mas_map_win);

  return(0);

}
