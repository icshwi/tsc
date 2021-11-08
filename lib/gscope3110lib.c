/*=========================< begin file & file header >=======================
 *  References
 *
 *    filename : gscope3110lib.c
 *    author   : JFG, XP
 *    company  : IOxOS
 *    creation : october 18,2018
 *
 *----------------------------------------------------------------------------
 *  Description
 *
 *    That library contains a set of function to access the TOSCA II XUSER
 *    Generic SCOPE ADC3110 data acquisition logic.
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

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <tsculib.h>
#include <tscioctl.h>
#include <tscextlib.h>
#include <gscopelib.h>
#include <gscope3110lib.h>
#include <fmclib.h>
#include <adclib.h>
#include <adc3110lib.h>
#include <adc3210lib.h>
#include <adc3117lib.h>
#include <daq1430lib.h>
#include <errno.h>
#include <time.h>

#include <debug.h>

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * Function name : gscope3110_XXX
 * Prototype     : int
 * Parameters    : void
 * Return        :
 *----------------------------------------------------------------------------
 * Description   :
 *
 *++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

int gscope3110_XXX(void) {

	return (0);
}

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * Function name : gscope3110_map_usr
 * Prototype     : char *
 * Parameters    : map pointer
 *                 address offset
 *                 size
 *                 space
 * Return        : fail/success
 *----------------------------------------------------------------------------
 * Description   :
 *
 *++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

struct tsc_ioctl_map_win*
gscope3110_map_usr(int fd, int fmc, char **buf, int offset, int size) {
	struct tsc_ioctl_map_win *map;
	int space;

	printf("in gscope3110_map_usr()\n");
	space = MAP_SPACE_USR1;
	if (fmc == GSCOPE_FMC2) space = MAP_SPACE_USR2;
	map = gscope_map(fd,  space, buf,  GSCOPE_MAP_USR_OFF + offset, size);
	printf("buf_ptr = %p\n", *buf);
	return (map);
}

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * Function name : gscope3110_unmap_usr
 * Prototype     : int
 * Parameters    : pointer to map
 * Return        : fail/success
 *----------------------------------------------------------------------------
 * Description   :
 *
 *++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

void gscope3110_unmap_usr(int fd, struct tsc_ioctl_map_win *map, char *u_addr) {
        gscope_unmap(fd, map, u_addr);
 	return;
}

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * Function name : gscope3110_mux_init
 * Prototype     : int
 * Parameters    : mux_map: mapping table for multiplexer
 * Return        : 0
 *----------------------------------------------------------------------------
 * Description   : Initialize input multiplexer
 *
 *++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

int
gscope3110_mux_init( int *mux_map)
{
  return(0);

}

/*--------------------------------------------------------------------------------------------------------------------*/

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * Function name : gscope1430_acq_smem_init
 * Prototype     : int
 * Parameters    :
 *                 scope_unit
 *                 chan_set:    list of the channels you want to init
 *                 rgbuf_base:  the base adress of the first ring buffer
 *                 rgbuf_size:  the size of the buffers
 *                 mode:        single or dual (currently unsupported)
 * Return        : current value of GSCOPE_CSR_SWRx_ACQ_MGT register
 *----------------------------------------------------------------------------
 * Description   : Initialize the acquisition: assign to each selected buffer
 *                 a base adress and a size
 *
 *++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

int gscope1430_acq_smem_init(int fd, int chan_set, int rgbuf_base, int rgbuf_size, int mode)
{
  int cnt_1, cnt_2, sel_1, sel_2, final_addr_1, final_addr_2, busy_1, busy_2, sec_off_1, sec_off_2;
  int i, j, base, size, chan_set_copy, mask, new_base, adc_map, fct;
  uint32_t tmp;

  gscope_mux_build_map(fd);

  /* set base address for each channel */
  base = (rgbuf_base & GSCOPE_RGBUF_BASE_MASK);
  size = (rgbuf_size & GSCOPE_RGBUF_SIZE_MASK);

  cnt_1 = 0;  /* for scope unit #1 */
  cnt_2 = 0;  /* for scope unit #2 */
  sel_1 = 0;
  sel_2 = 0;
  chan_set_copy = chan_set;

  for (i = 0; i < 10; i++)
  {
    adc_map = gscope_mux_get_map(fd, i);

    /* ADC channel active & mapped ? */
    if ((chan_set_copy & 1) && adc_map != 0)
    {
      for (j=0; j<16; j++)
      {
        if (adc_map & (1<<j))
        {
          if (j<=7)
          {
            cnt_1++;
            sel_1 |= (1<<j);
          }
          else
          {
            cnt_2++;
            sel_2 |= (1<<(j-8));
          }
        }
      }
    }
    chan_set_copy >>= 1;
  }

  if (mode == GSCOPE_RGBUF_MODE_DUAL)
  {
    sec_off_1 = cnt_1 * size;
    sec_off_2 = cnt_2 * size;
    cnt_1 *= 2;
    cnt_2 *= 2;
  }

  final_addr_1 = base + cnt_1 * size;
  final_addr_2 = base + cnt_2 * size;

  if (final_addr_1 > GSCOPE_RGBUF_ADDR_MAX || final_addr_2 > GSCOPE_RGBUF_ADDR_MAX)
  {
    return -1;
  }

  gscope_acq_abort(fd, 1);
  gscope_acq_abort(fd, 2);

  chan_set_copy = chan_set;
  new_base = base;

  /* initialize scope unit #1 */
  for (i = 0; i < 8; i++)
  {
    mask = (size | i);
    gscope_csr_wr(fd, GSCOPE_CSR_SWR1_RGB_CFG, mask);

    /* scope channel active ? -> allocate */
    if (sel_1 & (1<<i))
    {
      tmp = (new_base & 0xffff0000);
      if (mode == GSCOPE_RGBUF_MODE_DUAL)
      {
        tmp |= (((new_base + sec_off_1)>>16)&0xffff);
      }
      gscope_csr_wr(fd, GSCOPE_CSR_SWR1_RGB_BAS, tmp);
      new_base += size;
    }
    else
    {
      tmp = ((GSCOPE_RGBUF_ADDR_START & 0xffff0000) | ((GSCOPE_RGBUF_ADDR_START>>16)&0xffff));
      gscope_csr_wr(fd, GSCOPE_CSR_SWR1_RGB_BAS, tmp);
    }
  }

  /* reload initial base address */
  new_base = base;

  /* initialize scope unit #2 */
  for (i = 0; i < 8; i++)
  {
    mask = (size | i);
    gscope_csr_wr(fd, GSCOPE_CSR_SWR2_RGB_CFG, mask);

    /* scope channel active ? -> allocate */
    if (sel_2 & (1<<i))
    {
      tmp = (new_base & 0xffff0000);

      if (mode == GSCOPE_RGBUF_MODE_DUAL)
      {
        tmp |= (((new_base + sec_off_2)>>16)&0xffff);
      }

      gscope_csr_wr(fd, GSCOPE_CSR_SWR2_RGB_BAS, tmp);
      new_base += size;
    }
    else
    {
      tmp = ((GSCOPE_RGBUF_ADDR_START & 0xffff0000) | ((GSCOPE_RGBUF_ADDR_START>>16)&0xffff));
      gscope_csr_wr(fd, GSCOPE_CSR_SWR2_RGB_BAS, tmp);
    }
  }

  /* Enable Dual Buffer Mode */
  if (mode == GSCOPE_RGBUF_MODE_DUAL)
  {
    tmp = gscope_csr_rd(fd, GSCOPE_CSR_SWR1_ACQ_MGT);
    tmp &= 0x0fffffff;
    tmp |= GSCOPE_ACQ_DUALBUF_ENA;
    gscope_csr_wr(fd, GSCOPE_CSR_SWR1_ACQ_MGT, tmp);

    tmp = gscope_csr_rd(fd, GSCOPE_CSR_SWR2_ACQ_MGT);
    tmp &= 0x0fffffff;
    tmp |= GSCOPE_ACQ_DUALBUF_ENA;
    gscope_csr_wr(fd, GSCOPE_CSR_SWR2_ACQ_MGT, tmp);
  }

  fmc_csr_read(fd, 1, ADC_CSR_CTL, &fct);
  /* 1x sample or 2x samples mode (16-bit / 32-bit) */
  fct = ((fct & (1<<24)) ? 0 : 1);

  /* enable FMUX & DPRAM */
  mask = ((1<<31) | ((fct&7)<<28) | (chan_set & 0x3FF));

  gscope_csr_wr(fd, GSCOPE_CSR_FE1_CSR2, mask);

  /* init buffer with 0,1,2,3,.. */
  gscope_csr_wr(fd, GSCOPE_CSR_SWR1_RGB_CFG, (0xC0000010 | size));
  gscope_csr_wr(fd, GSCOPE_CSR_SWR2_RGB_CFG, (0xC0000010 | size));

  do
  {
    usleep(1000);

    busy_1 = (gscope_csr_rd(fd, GSCOPE_CSR_SWR1_RGB_CFG) & GSCOPE_SMEM_INIT_BUSY_MASK);
    busy_2 = (gscope_csr_rd(fd, GSCOPE_CSR_SWR2_RGB_CFG) & GSCOPE_SMEM_INIT_BUSY_MASK);

  } while (busy_1 == GSCOPE_SMEM_INIT_BUSY_MASK || busy_2 == GSCOPE_SMEM_INIT_BUSY_MASK);

  return gscope_csr_rd(fd, GSCOPE_CSR_SWR1_ACQ_MGT);
}

/*--------------------------------------------------------------------------------------------------------------------*/

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * Function name : gscope3110_acq_smem_init
 * Prototype     : int
 * Parameters    : chan_set:   list of the channels you want to init
					   rgbuf_base: the base adress of the first ring buffer
					   rgbuf_size: the size of the buffers
					   mode: single or dual (currently unsupported)
 * Return        : current value of GSCOPE_CSR_SWRx_ACQ_MGT register
 *----------------------------------------------------------------------------
 * Description   : Initialize the acquisition: assign to each selected buffer
 * a base adress and a size
 *
 *++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

int gscope3110_acq_smem_init(int fd, int chan_set, int rgbuf_base,
		int rgbuf_size, int mode)
{

	int cnt, i, base, size, chan_set_copy, mask, mask2, busy, busy2, final_addr, new_base;

	base = rgbuf_base & GSCOPE_RGBUF_BASE_MASK;
	size = rgbuf_size & GSCOPE_RGBUF_SIZE_MASK;

	cnt = 0;
	chan_set_copy = chan_set;
	for(i = 0; i < ADC3110_CHAN_NUM; i++)
	{
		cnt += chan_set_copy & 1;
		chan_set_copy = chan_set_copy >> 1;
	}

	final_addr = base + cnt * size;

	if(final_addr > GSCOPE_RGBUF_ADDR_MAX)
	{
		return -1;
	}

	gscope3110_acq_abort(fd, 1);
	gscope3110_acq_abort(fd, 2);

	chan_set_copy = chan_set;
	new_base = base;

	for(i = 0; i < (ADC3110_CHAN_NUM / 2); i++)
	{
		mask = size | i;
		gscope_csr_wr(fd, GSCOPE_CSR_SWR1_RGB_CFG, mask);
		//flush write cycle
		gscope_csr_rd(fd, GSCOPE_CSR_SWR1_RGB_CFG);
		if((chan_set_copy & 1) == 1)
		{
			gscope_csr_wr(fd, GSCOPE_CSR_SWR1_RGB_BAS, new_base);
			new_base = new_base + size;
		}
		else
		{
			gscope_csr_wr(fd, GSCOPE_CSR_SWR1_RGB_BAS, GSCOPE_RGBUF_ADDR_START);
		}
		chan_set_copy = chan_set_copy >> 1;
	}

	new_base = base;

	for(i = 0; i < (ADC3110_CHAN_NUM / 2); i++)
	{
		mask = size | i;
		gscope_csr_wr(fd, GSCOPE_CSR_SWR2_RGB_CFG, mask);
		//flush write cycle
		gscope_csr_rd(fd, GSCOPE_CSR_SWR2_RGB_CFG);
		if((chan_set_copy & 1) == 1)
		{
			gscope_csr_wr(fd, GSCOPE_CSR_SWR2_RGB_BAS, new_base);
			new_base = new_base + size;
		}
		else
		{
			gscope_csr_wr(fd, GSCOPE_CSR_SWR2_RGB_BAS, GSCOPE_RGBUF_ADDR_START);
		}
		chan_set_copy = chan_set_copy >> 1;
	}

	mask = 0x90000000 |(chan_set & 0xF);
	mask2 = 0x90000000 |(chan_set >> 4);


	/* enable FMUX & DPRAM */
	gscope_csr_wr(fd, GSCOPE_CSR_FE1_CSR2, mask);
	gscope_csr_wr(fd, GSCOPE_CSR_FE2_CSR2, mask2);
	gscope_csr_rd(fd, GSCOPE_CSR_FE1_CSR2);
	gscope_csr_rd(fd, GSCOPE_CSR_FE2_CSR2);

	/* init DPRAM withfd 0,1,2,3,.. */
	gscope_csr_wr(fd, GSCOPE_CSR_SWR1_RGB_CFG, (0xc0000010 | size));
	gscope_csr_wr(fd, GSCOPE_CSR_SWR2_RGB_CFG, (0xc0000010 | size));


	do
	{
		usleep(1000);
		busy = gscope_csr_rd(fd, GSCOPE_CSR_SWR1_RGB_CFG) & GSCOPE_SMEM_INIT_BUSY_MASK;
		busy2 = gscope_csr_rd(fd, GSCOPE_CSR_SWR2_RGB_CFG) & GSCOPE_SMEM_INIT_BUSY_MASK;
	} while(busy == GSCOPE_SMEM_INIT_BUSY_MASK || busy2 == GSCOPE_SMEM_INIT_BUSY_MASK);

	return (gscope_csr_rd(fd, GSCOPE_CSR_SWR1_ACQ_MGT));

}
/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * Function name : gscope3210_acq_smem_init
 * Prototype     : int
 * Parameters    : chan_set:   list of the channels you want to init
					   rgbuf_base: the base adress of the first ring buffer
					   rgbuf_size: the size of the buffers
					   mode: single or dual (currently unsupported)
 * Return        : current value of GSCOPE_CSR_SWRx_ACQ_MGT register
 *----------------------------------------------------------------------------
 * Description   : Initialize the acquisition: assign to each selected buffer
 * a base adress and a size
 *
 *++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

int gscope3210_acq_smem_init(int fd, int chan_set, int rgbuf_base,
		int rgbuf_size, int mode)
{

	int cnt, i, base, size, chan_set_copy, mask, mask2, busy, busy2, final_addr, new_base;

	base = rgbuf_base & GSCOPE_RGBUF_BASE_MASK;
	size = rgbuf_size & GSCOPE_RGBUF_SIZE_MASK;

	cnt = 0;
	chan_set_copy = chan_set;
	for(i = 0; i < ADC3210_CHAN_NUM; i++)
	{
		cnt += chan_set_copy & 1;
		chan_set_copy = chan_set_copy >> 1;
	}

	final_addr = base + cnt * size;

	if(final_addr > GSCOPE_RGBUF_ADDR_MAX)
	{
		return -1;
	}

	gscope3110_acq_abort(fd, 1);
	gscope3110_acq_abort(fd, 2);

	chan_set_copy = chan_set;
	new_base = base;

	for(i = 0; i < (ADC3210_CHAN_NUM / 2); i++)
	{
		mask = size | i;
		gscope_csr_wr(fd, GSCOPE_CSR_SWR1_RGB_CFG, mask);
		//flush write cycle
		gscope_csr_rd(fd, GSCOPE_CSR_SWR1_RGB_CFG);
		if((chan_set_copy & 1) == 1)
		{
			gscope_csr_wr(fd, GSCOPE_CSR_SWR1_RGB_BAS, new_base);
			new_base = new_base + size;
		}
		else
		{
			gscope_csr_wr(fd, GSCOPE_CSR_SWR1_RGB_BAS, GSCOPE_RGBUF_ADDR_START);
		}
		chan_set_copy = chan_set_copy >> 1;
	}

	new_base = base;

	for(i = 0; i < (ADC3210_CHAN_NUM / 2); i++)
	{
		mask = size | i;
		gscope_csr_wr(fd, GSCOPE_CSR_SWR2_RGB_CFG, mask);
		//flush write cycle
		gscope_csr_rd(fd, GSCOPE_CSR_SWR2_RGB_CFG);
		if((chan_set_copy & 1) == 1)
		{
			gscope_csr_wr(fd, GSCOPE_CSR_SWR2_RGB_BAS, new_base);
			new_base = new_base + size;
		}
		else
		{
			gscope_csr_wr(fd, GSCOPE_CSR_SWR2_RGB_BAS, GSCOPE_RGBUF_ADDR_START);
		}
		chan_set_copy = chan_set_copy >> 1;
	}

	mask = 0x90000000 |(chan_set & 0x3);
	mask2 = 0x90000000 |(chan_set >> 2);


	/* enable FMUX & DPRAM */
	gscope_csr_wr(fd, GSCOPE_CSR_FE1_CSR2, mask);
	gscope_csr_wr(fd, GSCOPE_CSR_FE2_CSR2, mask2);
	gscope_csr_rd(fd, GSCOPE_CSR_FE1_CSR2);
	gscope_csr_rd(fd, GSCOPE_CSR_FE2_CSR2);

	/* init DPRAM with 0,1,2,3,.. */
	gscope_csr_wr(fd, GSCOPE_CSR_SWR1_RGB_CFG, (0xc0000010 | size));
	gscope_csr_wr(fd, GSCOPE_CSR_SWR2_RGB_CFG, (0xc0000010 | size));


	do
	{
		usleep(1000);
		busy = gscope_csr_rd(fd, GSCOPE_CSR_SWR1_RGB_CFG) & GSCOPE_SMEM_INIT_BUSY_MASK;
		busy2 = gscope_csr_rd(fd, GSCOPE_CSR_SWR2_RGB_CFG) & GSCOPE_SMEM_INIT_BUSY_MASK;
	} while(busy == GSCOPE_SMEM_INIT_BUSY_MASK || busy2 == GSCOPE_SMEM_INIT_BUSY_MASK);

	return (gscope_csr_rd(fd, GSCOPE_CSR_SWR1_ACQ_MGT));

}
/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * Function name : gscope3117_acq_smem_init
 * Prototype     : int
 * Parameters    : chan_set:   list of the channels you want to init
					   rgbuf_base: the base adress of the first ring buffer
					   rgbuf_size: the size of the buffers
					   mode: single or dual (currently unsupported)
 * Return        : current value of GSCOPE_CSR_SWRx_ACQ_MGT register
 *----------------------------------------------------------------------------
 * Description   : Initialize the acquisition: assign to each selected buffer
 * a base adress and a size
 *
 *++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

int gscope3117_acq_smem_init(int fd, int fmc, int rgbuf_base,
			     int rgbuf_size, int mode)
{

	int cnt, i, base, size, chan_set_copy, mask, busy, final_addr, new_base;
	int chan_set;

	base = rgbuf_base & GSCOPE_RGBUF_BASE_MASK;
	size = (rgbuf_size & GSCOPE_RGBUF_SIZE_MASK)*ADC3117_CHAN_GROUP;
	chan_set = 0xfffff;
	cnt = 0;
	chan_set_copy = chan_set;
	for(i = 0; i < ADC3110_CHAN_NUM; i++)
	{
		cnt += chan_set_copy & 1;
		chan_set_copy = chan_set_copy >> 1;
	}

	final_addr = base + (cnt * size);

	if(final_addr > GSCOPE_RGBUF_ADDR_MAX)
	{
		return -1;
	}

	gscope3110_acq_abort(fd, fmc);

	chan_set_copy = chan_set;
	new_base = base;

	if( fmc == ADC3117_FMC1)
	{
          for(i = 0; i < ADC3117_CHAN_NUM/ADC3117_CHAN_GROUP; i++)
	  {
	        mask = size | i;
		gscope_csr_wr(fd, GSCOPE_CSR_SWR1_RGB_CFG, mask);
		//flush write cycle
		gscope_csr_rd(fd, GSCOPE_CSR_SWR1_RGB_CFG);
		gscope_csr_wr(fd, GSCOPE_CSR_SWR1_RGB_BAS, new_base);
		new_base = new_base + size;
	  }
	  gscope_csr_wr(fd, GSCOPE_CSR_FE1_CSR2, 0x9000001f);
	  gscope_csr_rd(fd, GSCOPE_CSR_FE1_CSR2);
  	  gscope_csr_wr(fd, GSCOPE_CSR_SWR1_RGB_CFG, (0xc0000010 | size));
	  do
	  {
		usleep(1000);
		busy = gscope_csr_rd(fd, GSCOPE_CSR_SWR1_RGB_CFG) & GSCOPE_SMEM_INIT_BUSY_MASK;
	  } while(busy == GSCOPE_SMEM_INIT_BUSY_MASK);
 	  return (gscope_csr_rd(fd, GSCOPE_CSR_SWR1_ACQ_MGT));
	}
	else
	{
          for(i = 0; i < ADC3117_CHAN_NUM/ADC3117_CHAN_GROUP; i++)
	  {
	        mask = size | i;
		gscope_csr_wr(fd, GSCOPE_CSR_SWR2_RGB_CFG, mask);
		//flush write cycle
		gscope_csr_rd(fd, GSCOPE_CSR_SWR2_RGB_CFG);
		gscope_csr_wr(fd, GSCOPE_CSR_SWR2_RGB_BAS, new_base);
		new_base = new_base + size;
	  }
	  gscope_csr_wr(fd, GSCOPE_CSR_FE2_CSR1, 0x9000001f);
	  gscope_csr_rd(fd, GSCOPE_CSR_FE2_CSR1);
	  gscope_csr_wr(fd, GSCOPE_CSR_SWR2_RGB_CFG, (0xc0000010 | size));
	  do
	  {
		usleep(1000);
		busy = gscope_csr_rd(fd, GSCOPE_CSR_SWR2_RGB_CFG) & GSCOPE_SMEM_INIT_BUSY_MASK;
	  } while(busy == GSCOPE_SMEM_INIT_BUSY_MASK);
	  return (gscope_csr_rd(fd, GSCOPE_CSR_SWR2_ACQ_MGT));
	}
}

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * Function name : gscope3110_acq_dpram_init
 * Prototype     : int
 * Parameters    : fmc  FMC identifier (1 or 2)
 * 				   chan_set  set of channel to be enabled for acquisition
 * 				   			 (bitfield)
 * 				   size  buffer size
 * Return        : current value of GSCOPE_CSR_SWRx_ACQ_MGT register
 *----------------------------------------------------------------------------
 * Description   : initialize the data acquisition state machine and enable the
 * channels set in chan_set.The parameter size defines the DPRAM buffer size
 *
 *++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

int gscope3110_acq_dpram_init(int fd, int fmc, int chan_set, int mirror) {
	chan_set &= GSCOPE_FEM_CHAN_ALL;
	chan_set |= GSCOPE_FEM_ENA | GSCOPE_FEM_FCT_1;
	if (fmc == GSCOPE_FMC2) {
		/* abort any pending acquisition */
		gscope_csr_wr(fd, GSCOPE_CSR_SWR2_ACQ_MGT, 0x20000000);
		usleep(2000);
		/* return to idle state */
		gscope_csr_wr(fd, GSCOPE_CSR_SWR2_ACQ_MGT, 0x40000000);
		usleep(2000);
		/* enable FMUX & DPRAM */
		gscope_csr_wr(fd, GSCOPE_CSR_FE2_CSR2, chan_set);
		gscope_csr_wr(fd, GSCOPE_CSR_SWR2_RGB_CFG, 0x80000000);
		/* init DPRAM with 0,1,2,3,.. */
		gscope_csr_wr(fd, GSCOPE_CSR_SWR2_RGB_CFG, 0xc0000010);
		return (gscope_csr_rd(fd, GSCOPE_CSR_SWR2_ACQ_MGT));
	} else {
		/* abort any pending acquisition */
		gscope_csr_wr(fd, GSCOPE_CSR_SWR1_ACQ_MGT, 0x20000000);
		usleep(2000);
		/* return to idle state */
		gscope_csr_wr(fd, GSCOPE_CSR_SWR1_ACQ_MGT, 0x40000000);
		usleep(2000);
		/* enable FMUX & DPRAM */
		gscope_csr_wr(fd, GSCOPE_CSR_FE1_CSR2, chan_set);
		gscope_csr_wr(fd, GSCOPE_CSR_SWR1_RGB_CFG, 0x80000000);
		/* init DPRAM with 0,1,2,3,.. */
		gscope_csr_wr(fd, GSCOPE_CSR_SWR1_RGB_CFG, 0xc0000010);
		return (gscope_csr_rd(fd, GSCOPE_CSR_SWR1_ACQ_MGT));
	}
}

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * Function name : gscope3110_acq_status
 * Prototype     : int
 * Parameters    : fmc  FMC identifier (1 or 2)
 * Return        : current value of GSCOPE_CSR_SWRx_ACQ_MGT register
 *----------------------------------------------------------------------------
 * Description   : return the current status of the acquisition state machine
 *
 *++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

int gscope3110_acq_status(int fd, int fmc) {
	if (fmc == GSCOPE_FMC2) {
		return (gscope_csr_rd(fd, GSCOPE_CSR_SWR2_ACQ_MGT));
	} else {
		return (gscope_csr_rd(fd, GSCOPE_CSR_SWR1_ACQ_MGT));
	}
}

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * Function name : gscope3110_acq_arm
 * Prototype     : int
 * Parameters    : fmc  FMC identifier (1 or 2)
 *                 trig_mode  the arm mode
 *                            1st bit (MSB) = master(0)/slave(1)
 *                            2nd = normal(0)/auto(1)
 *                            3rd (LSB) = continuous(0)/single(1)
 *                 trig_pos  the pre_trigger proportion of datas
 *                           (0 to 7 out of 8)
 *                 buf_mode  single or dual (currently unsupported)
 * Return        : current value of GSCOPE_CSR_SWRx_ACQ_MGT register
 *----------------------------------------------------------------------------
 * Description   : arms the data acquisition state machine and returns its
 * current status
 *
 *++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

int gscope3110_acq_arm(int fd, int fmc, int trig_mode, int trig_pos, int buf_mode, int rearm) {
	int csr, acq_mode, acq_trig, sync, mask;

  if (fmc == GSCOPE_FMC2)
  {
    csr = GSCOPE_CSR_SWR2_ACQ_MGT;
  }
  else
  {
    csr = GSCOPE_CSR_SWR1_ACQ_MGT;
  }

  mask = (rearm ? GSCOPE_REARM_MASK : GSCOPE_ARM_MASK);

  acq_mode = (trig_mode & GSCOPE_ARM_CODE_MODE_MASK);
  acq_trig = (trig_mode & GSCOPE_ARM_CODE_TRIG_MASK);
  sync     = (trig_mode & GSCOPE_ARM_CODE_SYNC_MASK);

  if(acq_mode == GSCOPE_ARM_CODE_CONT)
  {
    mask |= (GSCOPE_ARM_CONT << GSCOPE_ARM_MODE_START);
  }
  else
  {
    mask |= (GSCOPE_ARM_SINGLE << GSCOPE_ARM_MODE_START);
  }
  if(acq_trig == GSCOPE_ARM_CODE_NORMAL)
  {
    mask |= (GSCOPE_ARM_NORMAL << GSCOPE_ARM_TRIG_START);
  }
  else
  {
    mask |= (GSCOPE_ARM_AUTO << GSCOPE_ARM_TRIG_START);
  }
  if(sync == GSCOPE_ARM_CODE_MASTER)
  {
    mask |= (GSCOPE_ARM_MASTER << GSCOPE_ARM_SYNC_START);
  }
  else
  {
    mask |= (GSCOPE_ARM_SLAVE << GSCOPE_ARM_SYNC_START);
  }

  mask |= (trig_pos << GSCOPE_ARM_TRIG_POS_START);

  gscope_csr_wr(fd, csr, mask);
  usleep(2000);

  return (gscope_csr_rd(fd, csr));
}
/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * Function name : gscope3110_acq_abort
 * Prototype     : int
 * Parameters    : fmc  FMC identifier (1 or 2)
 * Return        : current value of GSCOPE_CSR_SWRx_ACQ_MGT register
 *----------------------------------------------------------------------------
 * Description   : abort the data acquisition, put the state machine in idle
 * and returns its current status
 *
 *++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

int gscope3110_acq_abort(int fd, int fmc)
{
  int tmp;

  if (fmc == GSCOPE_FMC2)
  {
    tmp = gscope_csr_rd(fd, GSCOPE_CSR_SWR2_ACQ_MGT);
    tmp &= ~(0xF0000000);

    gscope_csr_wr(fd, GSCOPE_CSR_SWR2_ACQ_MGT, (0x20000000 | tmp));
    usleep(2000);
    /* return to idle state */
    gscope_csr_wr(fd, GSCOPE_CSR_SWR2_ACQ_MGT, (0x40000000 | tmp));
    usleep(2000);
    return (gscope_csr_rd(fd, GSCOPE_CSR_SWR2_ACQ_MGT));
  }
  else
  {
    tmp = gscope_csr_rd(fd, GSCOPE_CSR_SWR1_ACQ_MGT);
    tmp &= ~(0xF0000000);

    gscope_csr_wr(fd, GSCOPE_CSR_SWR1_ACQ_MGT, (0x20000000 | tmp));
    usleep(2000);
    /* return to idle state */
    gscope_csr_wr(fd, GSCOPE_CSR_SWR1_ACQ_MGT, (0x40000000 | tmp));
    usleep(2000);
    return (gscope_csr_rd(fd, GSCOPE_CSR_SWR1_ACQ_MGT));
  }
}
/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * Function name : gscope3110_trig_set
 * Prototype     : void
 * Parameters    : fmc  FMC identifier (1 or 2)
 *                 chan  the channel that will trigger the acquisition
 *                 mode  the acquisition mode
 *                       1st bit (MSB) = manual(0)/engine(1)
 *                       2nd = gpio or manual(0)/adc(1)
 *                       3rd = 0 not used
 *                       4th = level(0)/edge(1)
 *                       5th = up(0)/down(1)
 *                       6th (LSB) = unsigned(0)/signed(0)
 *                 offset  reference value for trigger action
 *                 hyst  hysteresis value for trigger action
 * Return        : void
 *----------------------------------------------------------------------------
 * Description   : set up the way the acqusition will be triggered
 *
 *++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

void gscope3110_trig_set(int fd, int fmc, int chan, int mode, int offset, int hyst) {

	int csr_index, main_mode, mask, lvl_edge, sign, dir;

	if (fmc == GSCOPE_FMC2) {
		csr_index = GSCOPE_CSR_FE2_TRIG;
	} else {
		csr_index = GSCOPE_CSR_FE1_TRIG;
	}



	main_mode = mode & GSCOPE_TRIG_CODE_MAIN_MASK ;

	if(main_mode == GSCOPE_TRIG_CODE_MAN)
	{
		gscope_csr_wr(fd, csr_index, GSCOPE_TRIG_MAN);
		return;
	}



	if(main_mode == GSCOPE_TRIG_CODE_GPIO)
	{
		gscope_csr_wr(fd, csr_index, GSCOPE_TRIG_GPIO_MASK);
		return;
	}

	gscope_csr_wr(fd, csr_index, GSCOPE_TRIG_HYST_MASK | (hyst & GSCOPE_TRIG_HYST_OVF));

	mask = GSCOPE_TRIG_ENG_MASK;
	mask = mask | (offset & GSCOPE_TRIG_OFFSET_OVF) | (GSCOPE_TRIG_HYST << GSCOPE_TRIG_HYST_START);
	lvl_edge = mode & GSCOPE_TRIG_CODE_LVL_MASK;
	dir = mode & GSCOPE_TRIG_CODE_DIR_MASK;
	sign = mode & GSCOPE_TRIG_CODE_SIGN_MASK;

	if(dir == GSCOPE_TRIG_CODE_UP){
		mask = mask | (GSCOPE_TRIG_UP << GSCOPE313_TRIG_DIR_START);
	} else {
		mask = mask | (GSCOPE_TRIG_DOWN << GSCOPE313_TRIG_DIR_START);
	}

	if(lvl_edge == GSCOPE_TRIG_CODE_LVL){
		mask = mask | (GSCOPE_TRIG_LVL << GSCOPE313_TRIG_LVL_START);
	} else{
		mask = mask | (GSCOPE_TRIG_EDGE << GSCOPE313_TRIG_LVL_START);
	}

	if(sign == GSCOPE_TRIG_CODE_SIGN){
		mask = mask | (GSCOPE_TRIG_SIGN << GSCOPE313_TRIG_SIGN_START);
	}
	else{
		mask = mask | (GSCOPE_TRIG_UNSIGN << GSCOPE313_TRIG_SIGN_START);
	}

	mask = mask | chan << GSCOPE_TRIG_CHAN_START;

	gscope_csr_wr(fd, csr_index, mask);
}

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * Function name : gscope3110_acq_load
 * Prototype     : int
 * Parameters    : fmc
 * Return        : fail/success
 *----------------------------------------------------------------------------
 * Description   : clear acquisition buffers
 *
 *++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

void gscope3110_acq_load(int fd, int fmc, char *buf, int size) {
	struct tsc_ioctl_map_win *usr_map;
	char *adc_buf;
	int i, start;

	start = 0x0;
	if (size > 0x8000) {
		size = 0x8000;
	}
	usr_map = gscope3110_map_usr(fd, fmc, &adc_buf, start, size);
	printf("move  data from %p to %p\n", adc_buf, buf);
	for (i = 0; i < size; i += 4) {
		*(int*) &buf[i] = *(int*) &adc_buf[i];
	}
	gscope3110_unmap_usr(fd, usr_map, adc_buf);

	return;
}

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * Function name : gscope3110_acq_dpram_move
 * Prototype     : int
 * Parameters    : fmc  FMC identifier (1 or 2)
 * 				   chan  ADC channel (0 -> ADC3110_CHAN_NUM - 1)
 * 				   pci_addr  PCI detination address
 * 				   dpram_off  DPRAM offset (source)
 * 				   size  transfer size (in bytes)
 * Return        : 0   if DMA transfer OK
 * 				   < 0 if error (see TSC API for DMA transfer status)
 *----------------------------------------------------------------------------
 * Description   : start a DMA transfer of size bytes from DPRAM offset
 * dpram_off associated to ADC channel chan to PCI address pci_addr.
 * The status of the DMA transfer is returned.
 *
 *++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

int gscope3110_acq_dpram_move(int fd, int fmc, int chan, uint64_t pci_addr,
		uint64_t dpram_off, int size) {
	struct tsc_ioctl_dma_req dma_req;
	int dma_chan;
	int retval;

	if (chan >= ADC3110_CHAN_NUM) {
		return (-1);
	}
	if (size > 0x40000) {
		size = 0x40000;
	}
	dma_chan = 0;
	if (fmc == GSCOPE_FMC2) {
		dma_chan = 2;
	}
	dma_req.size = size;
	dma_req.src_addr = dpram_off + (0x100000 * chan);
	dma_req.src_space = DMA_SPACE_DIRECT;
	dma_req.src_mode = 0;
	dma_req.des_addr = pci_addr;
	dma_req.des_space = DMA_SPACE_PCIE;
	dma_req.des_mode = 0;
	dma_req.start_mode = (char) DMA_START_CHAN(dma_chan);
	dma_req.end_mode = 0;
	dma_req.wait_mode = DMA_WAIT_INTR | DMA_WAIT_1S | (5 << 4); /* 5 sec timeout */
	if (tsc_dma_alloc(fd, dma_chan)) {
		printf("Cannot perform DMA transfer on channel #%d -> %s\n", chan,
				strerror(errno));
		return (-1);
	}
/*===> JFG <===*/
	/* 64 word dummy transfer to make sure first 64bit word is transferred by DMA */
	dma_req.size = 64;
	retval = tsc_dma_move(fd, &dma_req);
	dma_req.size = size;
	retval = tsc_dma_move(fd, &dma_req);
	if (retval < 0) {
		printf("Cannot perform DMA transfer on channel #%d -> %s\n", chan,
				strerror(errno));
	} else {
		if (dma_req.dma_status & DMA_STATUS_TMO) {
			printf("NOK -> timeout - status = %08x\n", dma_req.dma_status);
			tsc_dma_clear(fd, chan);
			retval = -1;
		} else if (dma_req.dma_status & DMA_STATUS_ERR) {
			printf("NOK -> transfer error - status = %08x\n",
					dma_req.dma_status);
			retval = -1;
		}
	}
	tsc_dma_free(fd, dma_chan);

	return (retval);
}

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * Function name : gscope3110_acq_smem_move
 * Prototype     : int
 * Parameters    : fmc
 * Return        : fail/success
 *----------------------------------------------------------------------------
 * Description   : clear acquisition buffers
 *
 *++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

int gscope3110_acq_smem_move(int fd, int fmc, int chan, uint64_t pci_addr,
		uint64_t shm_off, int size) {
	struct tsc_ioctl_dma_req dma_req;
	int dma_chan;
	int chan_off;
	int retval;

	if (chan >= ADC3110_CHAN_NUM) {
		return (-1);
	}
	if (size > 0x8000) {
		size = 0x8000;
	}
	dma_chan = 0;
	chan_off = 0x10000;
	if (fmc == GSCOPE_FMC2) {
		dma_chan = 2;
	}
	if (chan < 4) {
		dma_req.src_addr = 0x10000000 + (chan_off * chan) + shm_off;
		dma_req.src_space = DMA_SPACE_SHM;
	} else {
		dma_req.src_addr = 0x10000000 + (chan_off * (chan - 4)) + shm_off;
		dma_req.src_space = DMA_SPACE_SHM2;
	}
	dma_req.src_mode = 0;
	dma_req.des_addr = pci_addr;
	dma_req.des_space = DMA_SPACE_PCIE;
	dma_req.des_mode = 0;
	dma_req.start_mode = (char) DMA_START_CHAN(dma_chan);
	dma_req.end_mode = 0;
	dma_req.wait_mode = DMA_WAIT_INTR | DMA_WAIT_1S | (5 << 4); /* 5 sec timeout */
	if (tsc_dma_alloc(fd, dma_chan)) {
		printf("Cannot perform DMA transfer on channel #%d -> %s\n", chan,
				strerror(errno));
		return (-1);
	}
	retval = tsc_dma_move(fd, &dma_req);
	if (retval < 0) {
		printf("Cannot perform DMA transfer on channel #%d -> %s\n", chan,
				strerror(errno));
	} else {
		if (dma_req.dma_status & DMA_STATUS_TMO) {
			printf("NOK -> timeout - status = %08x\n", dma_req.dma_status);
			tsc_dma_clear(fd, chan);
			retval = -1;
		} else if (dma_req.dma_status & DMA_STATUS_ERR) {
			printf("NOK -> transfer error - status = %08x\n",
					dma_req.dma_status);
			retval = -1;
		}
	}
	tsc_dma_free(fd, dma_chan);

	return (retval);
}


/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * Function name : gscope3110_part_save_file
 * Prototype     : int
 * Parameters    : start      start in byte
 *                 end        end   in byte
 *                 trig       trigger position
 *                 acq_buf    pointer on acquisition buffer
 *                 acq_file   pointer to file
 *                 inc        increment
 *                 mask       AND-mask sample
 *                 fmt        sample format representation
 *                              0 = unsigned 16-bit
 *                              1 = signed 16-bit
 * Return        :
 *----------------------------------------------------------------------------
 * Description   : save 16 bit adc samples from acq_buf in text file acq_file
 *
 *++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

int gscope3110_part_save_file(int start, int end, int trig, char *acq_buf, FILE *acq_file, int inc, int mask, int fmt)
{
  int data, i, pos;

  pos = 0;
  for (i = start; i < end; i += 2*inc)
  {
    data = *(unsigned char*) &acq_buf[i] | (*(unsigned char*) &acq_buf[i + 1] << 8);
    data &= mask;
    if (fmt == 0)
    {
      fprintf(acq_file, ((trig==i) ? "%-6d    # <-- TRIGGER\n" : "%d\n"), (unsigned short)data);
    }
    else
    {
      fprintf(acq_file, ((trig==i) ? "%-6d    # <-- TRIGGER\n" : "%d\n"), (signed short)data);
    }
    pos++;
  }

  return pos;
}

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * Function name : gscope3110_part_save_buffer
 * Prototype     : int
 * Parameters    :
 * Return        :
 *----------------------------------------------------------------------------
 * Description   : save 16 bit adc samples from acq_buf to output_buffer
 *
 *++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

int gscope3110_part_save_buffer(int start, int end, int pos, char *acq_buf, char* output_buffer, int inc, int mask)
{
  int i, cnt;

  if (inc == 1 && mask == 0xffff)
  {
    cnt = (end - start - 1);
    memcpy((void *)&output_buffer[pos], (void *)&acq_buf[start], cnt);
    pos += cnt;
  }
  else
  {
    for (i = start; i < end; i += 2*inc)
    {
      output_buffer[pos] = *(unsigned char*) &acq_buf[i] & (mask >> 8);
      output_buffer[pos+1] = *(unsigned char*) &acq_buf[i+1] & mask;
      pos += 2;
    }
  }

  return pos;
}

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * Function name : gscope3110_smem_save
 * Prototype     : int
 * Parameters    : chan      the channel from which the datas are saved
 *                 str       the name of the file or the buffer
 *                 mode
 *                      [0] buffer = 0 or file = 1
 *                      [1] 0 = auto buffer, 1 = force buffer (only in dual buffer mode)
 *                      [2] primary = 0 or secondary = 1
 *                 new_size  the size of the file
 * Return        : fail/success
 *----------------------------------------------------------------------------
 * Description   : Save the datas from a channel in a file or a buffer of a
 *                 given size
 *
 *++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

int gscope3110_smem_save(int fd, int chan, char *str, int mode, int new_size, int mask, int fmt)
{
  int base, size, pos, trig, size_mask, pre_trig, start, end, csr, burst_size, extr, gap, prop, ctl;
  struct tsc_ioctl_map_win shm_mas_map_win;
  char *acq_buf;
  FILE* acq_file;
  int inc;
  int scope_chan;

  inc = 1;

  /* if ADC3117 samples are grouped by 4 */
  /*if ((gscope_csr_rd(GSCOPE_CSR_SIGN1) & 0xffff) == 0x3117)
  {
    inc = 4;
  }*/

  /*gscope_mux_build_map();*/

  scope_chan = gscope_mux_get_scope_chan(fd, chan);

  if (scope_chan == -1)
  {
    fprintf(stderr, "adc channel not mapped to a scope channel !\n");
    return(-1);
  }

  if (scope_chan < 8)
  {
    ctl = gscope_csr_rd(fd, GSCOPE_CSR_SWR1_ACQ_MGT);
    csr = gscope_csr_rd(fd, GSCOPE_CSR_SWR1_RGB_CFG);
    csr = (csr & GSCOPE_SAVE_CHAN_MASK) | scope_chan;

    gscope_csr_wr(fd, GSCOPE_CSR_SWR1_RGB_CFG, csr);
    base = gscope_csr_rd(fd, GSCOPE_CSR_SWR1_RGB_BAS);

    /* Dual Buffer Mode Enabled ? AND
     * Secondary Buffer Filled ? OR
     * User Requested Secondary Buffer ?
     */
    if ((ctl & GSCOPE_ACQ_DUALBUF_ENA) &&
        ((ctl & GSCOPE_ACQ_DUALBUF1_DONE) || ((mode & GSCOPE_SAVE_FORCE_BUF) && (mode & GSCOPE_SAVE_SEC_BUF))))
    {
      base = ((base << 16) & 0xffff0000);
    }
    else
    {
      base = (base  & 0xffff0000);
    }

    trig = gscope_csr_rd(fd, GSCOPE_CSR_SWR1_TRIG_MKT);
    pre_trig = gscope_csr_rd(fd, GSCOPE_CSR_SWR1_ACQ_MGT);
  }
  else
  {
    ctl = gscope_csr_rd(fd, GSCOPE_CSR_SWR2_ACQ_MGT);
    csr = gscope_csr_rd(fd, GSCOPE_CSR_SWR2_RGB_CFG);
    csr = (csr & GSCOPE_SAVE_CHAN_MASK) | (scope_chan - 8);
    gscope_csr_wr(fd, GSCOPE_CSR_SWR2_RGB_CFG, csr);
    base = gscope_csr_rd(fd, GSCOPE_CSR_SWR2_RGB_BAS);

    /* Dual Buffer Mode Enabled ? AND
     * Secondary Buffer Filled ? OR
     * User Requested Secondary Buffer ?
     */
    if ((ctl & GSCOPE_ACQ_DUALBUF_ENA) &&
        ((ctl & GSCOPE_ACQ_DUALBUF1_DONE) || ((mode & GSCOPE_SAVE_FORCE_BUF) && (mode & GSCOPE_SAVE_SEC_BUF))))
    {
      base = ((base << 16) & 0xffff0000);
    }
    else
    {
      base = (base  & 0xffff0000);
    }

    trig = gscope_csr_rd(fd, GSCOPE_CSR_SWR2_TRIG_MKT);
    pre_trig = gscope_csr_rd(fd, GSCOPE_CSR_SWR2_ACQ_MGT);
  }

  trig &= 0x1FFFFFF8;

  burst_size = ((csr & (1<<29)) ? 4096 : 2048);

  size_mask = GSCOPE_RGBUF_SIZE_MASK;
  size = (csr & size_mask);
  if (!size)
  {
    size = 8192*64*1024;
  }

  prop = ((pre_trig >> GSCOPE_ARM_TRIG_POS_START) & 0x7);

  pre_trig  = (   prop  * size / GSCOPE_ARM_TRIG_POS_MAX);

  bzero(&shm_mas_map_win, sizeof(shm_mas_map_win));
  shm_mas_map_win.req.mode.sg_id = MAP_ID_MAS_PCIE_PMEM;
  shm_mas_map_win.req.mode.space = ((scope_chan < 8) ? MAP_SPACE_SHM : MAP_SPACE_SHM2);

  shm_mas_map_win.req.rem_addr = base;
  shm_mas_map_win.req.size = size;
  tsc_map_alloc(fd, &shm_mas_map_win);
  acq_buf = (char*) tsc_pci_mmap(fd, shm_mas_map_win.sts.loc_base,
    shm_mas_map_win.sts.size);

  if (new_size > size || new_size == 0)
  {
    new_size = size;
  }

  gap = (size - new_size);
  extr = ((trig + size - pre_trig) % size);

  if (gap <= (trig % burst_size))
  {
    if (pre_trig > 0)
    {
      extr = (((extr / burst_size) + 1) * burst_size) % size;
    }
    else
    {
      extr = ((extr / burst_size) * burst_size) % size;
    }
  }

  start = (extr + ((gap *   prop)  / 8)        ) % size;
  end   = (extr - ((gap *(8-prop)) / 8) + size ) % size;

  debug("trig = 0x%08X, start = 0x%08X, end = 0x%08X, base = 0x%08X", trig, start, end, base);

  if (mode&1)
  {

    acq_file = fopen(str, "w");

    if(!acq_file)
    {
      fprintf(stderr, "cannot create acquisition file %s\n", str);
      return (-1);
    }
    if(end < trig || start >= trig)
    {
      pos = gscope3110_part_save_file(start, size, trig, acq_buf, acq_file, inc, mask, fmt);
      pos = gscope3110_part_save_file(0,     end,  trig, acq_buf, acq_file, inc, mask, fmt);
    }
    else
    {
      pos = gscope3110_part_save_file(start, end, trig, acq_buf, acq_file, inc, mask, fmt);
    }
    fclose(acq_file);
  }
  else
  {
    if(end < trig || start >= trig)
    {
      pos = gscope3110_part_save_buffer(start, size,   0, acq_buf, str, inc, mask);
      pos = gscope3110_part_save_buffer(0,      end, pos, acq_buf, str, inc, mask);
    }
    else
    {
      pos = gscope3110_part_save_buffer(start, end, 0, acq_buf, str, inc, mask);
    }
  }

  tsc_pci_munmap(acq_buf, shm_mas_map_win.sts.size);
  tsc_map_free(fd, &shm_mas_map_win);

  return(new_size);
}

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * Function name : gscope3210_smem_save
 * Prototype     : int
 * Parameters    : chan      the channel from which the datas are saved
 *                 str       the name of the file or the buffer
 * 				   mode      buffer (0) or file (1)
 * 				   new_size  the size of the file
 * Return        : fail/success
 *----------------------------------------------------------------------------
 * Description   : Save the datas from a channel in a file or a buffer of a
 *                 given size
 *
 *++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

int gscope3210_smem_save(int fd, int chan, char *str, int mode, int new_size, int mask, int fmt)
{

  int base, size, pos, trig, size_mask, prop, pre_trig, extr, start, end, gap, csr, burst_size;
  struct tsc_ioctl_map_win shm_mas_map_win;
  char *acq_buf;
  FILE* acq_file;
  int inc;

  inc = 1;
  /* if ADC3117 samples are grouped by 4 */
  if( (gscope_csr_rd(fd, GSCOPE_CSR_SIGN1) & 0xffff) == 0x3117) inc = 4;

  if (chan < ADC3210_CHAN_NUM / 2)
  {
    csr = gscope_csr_rd(fd, GSCOPE_CSR_SWR1_RGB_CFG);
    csr = (csr & GSCOPE_SAVE_CHAN_MASK) | chan;

    gscope_csr_wr(fd, GSCOPE_CSR_SWR1_RGB_CFG, csr);
    base = gscope_csr_rd(fd, GSCOPE_CSR_SWR1_RGB_BAS);
    trig = gscope_csr_rd(fd, GSCOPE_CSR_SWR1_TRIG_MKT);
    pre_trig = gscope_csr_rd(fd, GSCOPE_CSR_SWR1_ACQ_MGT);
  }
  else
  {
    csr = gscope_csr_rd(fd, GSCOPE_CSR_SWR2_RGB_CFG);
    csr = (csr & GSCOPE_SAVE_CHAN_MASK) | (chan - (ADC3210_CHAN_NUM / 2));
    gscope_csr_wr(fd, GSCOPE_CSR_SWR2_RGB_CFG, csr);
    base = gscope_csr_rd(fd, GSCOPE_CSR_SWR2_RGB_BAS);
    trig = gscope_csr_rd(fd, GSCOPE_CSR_SWR2_TRIG_MKT);
    pre_trig = gscope_csr_rd(fd, GSCOPE_CSR_SWR2_ACQ_MGT);
  }

  trig &= 0x1FFFFFF8;

  burst_size = ((csr & (1<<29)) ? 4096 : 2048);

  size_mask = GSCOPE_RGBUF_SIZE_MASK;
  size = csr & size_mask;
  if (!size)
  {
    size = 8192*64*1024;
  }

  prop = ((pre_trig >> GSCOPE_ARM_TRIG_POS_START) & 0x7);

  pre_trig = ( prop * size / GSCOPE_ARM_TRIG_POS_MAX);

  bzero(&shm_mas_map_win, sizeof(shm_mas_map_win));
  shm_mas_map_win.req.mode.sg_id = MAP_ID_MAS_PCIE_PMEM;
  shm_mas_map_win.req.mode.space = MAP_SPACE_SHM;
  if (chan >= ADC3210_CHAN_NUM / 2)
  {
    shm_mas_map_win.req.mode.space = MAP_SPACE_SHM2;
  }
  shm_mas_map_win.req.rem_addr = base;
  shm_mas_map_win.req.size = size;
  tsc_map_alloc(fd, &shm_mas_map_win);
  acq_buf = (char*) tsc_pci_mmap(fd, shm_mas_map_win.sts.loc_base,
      shm_mas_map_win.sts.size);

  if (new_size > size || new_size == 0)
  {
    new_size = size;
  }

  gap = (size - new_size);
  extr = ((trig + size - pre_trig) % size);

  if (gap <= (trig % burst_size))
  {
    if (pre_trig > 0)
    {
      extr = (((extr / burst_size) + 1) * burst_size) % size;
    }
    else
    {
      extr = ((extr / burst_size) * burst_size) % size;
    }
  }

  start = (extr + ((gap *  prop     ) / 8)       ) % size;
  end   = (extr - ((gap * (8 - prop)) / 8) + size) % size;

#ifdef DEBUG
  printf("trig = 0x%08X, start = 0x%08X, end = 0x%08X\n", trig, start, end);
#endif /* DEBUG */

  if (mode)
  {

    acq_file = fopen(str, "w");

    if(!acq_file)
    {
      fprintf(stderr, "cannot create acquisition file %s\n", str);
      return (-1);
    }
    if(end < trig || start >= trig)
    {
      pos = gscope3110_part_save_file(start, size, trig, acq_buf, acq_file, inc, mask, fmt);
      pos = gscope3110_part_save_file(0,     end,  trig, acq_buf, acq_file, inc, mask, fmt);
    }
    else
    {
      pos = gscope3110_part_save_file(start, end, trig, acq_buf, acq_file, inc, mask, fmt);
    }
    fclose(acq_file);
  }
  else
  {
    if(end < trig || start >= trig)
    {
      pos = gscope3110_part_save_buffer(start, size, 0, acq_buf, str, inc, mask);
      pos = gscope3110_part_save_buffer(0, end, pos, acq_buf, str, inc, mask);
    }
    else
    {
      pos = gscope3110_part_save_buffer(start, end, 0, acq_buf, str, inc, mask);
    }
  }

  tsc_pci_munmap(acq_buf, shm_mas_map_win.sts.size);
  tsc_map_free(fd, &shm_mas_map_win);

  return(new_size);
}

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * Function name : gscope3117_smem_save
 * Prototype     : int
 * Parameters    : chan      the channel from which the datas are saved
 *                 str       the name of the file or the buffer
 * 				   mode      buffer (0) or file (1)
 * 				   new_size  the size of the file
 * Return        : fail/success
 *----------------------------------------------------------------------------
 * Description   : Save the datas from a channel in a file or a buffer of a
 *                 given size
 *
 *++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

int gscope3117_smem_save(int fd, int fmc, int chan, char *str, int mode, int new_size, int mask, int fmt)
{

  int base, size, pos, trig, size_mask, prop, pre_trig, extr, start, end, gap, csr, burst_size;
  struct tsc_ioctl_map_win shm_mas_map_win;
  char *acq_buf;
  FILE* acq_file;
  int inc, off;

  inc = ADC3117_CHAN_GROUP;
  off = (chan%ADC3117_CHAN_GROUP)*2;

  if( fmc == ADC3117_FMC1)
  {
    csr = gscope_csr_rd(fd, GSCOPE_CSR_SWR1_RGB_CFG);
    csr = (csr & GSCOPE_SAVE_CHAN_MASK) | (chan/ADC3117_CHAN_GROUP);

    gscope_csr_wr(fd, GSCOPE_CSR_SWR1_RGB_CFG, csr);
    base = gscope_csr_rd(fd, GSCOPE_CSR_SWR1_RGB_BAS);
    trig = gscope_csr_rd(fd, GSCOPE_CSR_SWR1_TRIG_MKT);
    pre_trig = gscope_csr_rd(fd, GSCOPE_CSR_SWR1_ACQ_MGT);
  }
  else
  {
    csr = gscope_csr_rd(fd, GSCOPE_CSR_SWR2_RGB_CFG);
    csr = (csr & GSCOPE_SAVE_CHAN_MASK) | (chan/ADC3117_CHAN_GROUP);
    gscope_csr_wr(fd, GSCOPE_CSR_SWR2_RGB_CFG, csr);
    base = gscope_csr_rd(fd, GSCOPE_CSR_SWR2_RGB_BAS);
    trig = gscope_csr_rd(fd, GSCOPE_CSR_SWR2_TRIG_MKT);
    pre_trig = gscope_csr_rd(fd, GSCOPE_CSR_SWR2_ACQ_MGT);
  }

  trig &= 0x1FFFFFF8;

  burst_size = ((csr & (1<<29)) ? 4096 : 2048);

  size_mask = GSCOPE_RGBUF_SIZE_MASK;
  size = csr & size_mask;
  if (!size)
  {
    size = (8192*64*1024);     /* 64K * 8192 when 0 */
  }

  prop  = ((pre_trig >> GSCOPE_ARM_TRIG_POS_START) & 0x7);

  pre_trig = (prop * size / GSCOPE_ARM_TRIG_POS_MAX);

  bzero(&shm_mas_map_win, sizeof(shm_mas_map_win));
  shm_mas_map_win.req.mode.sg_id = MAP_ID_MAS_PCIE_PMEM;
  shm_mas_map_win.req.mode.space = MAP_SPACE_SHM;
  if( fmc == ADC3117_FMC2)
  {
    shm_mas_map_win.req.mode.space = MAP_SPACE_SHM2;
  }
  shm_mas_map_win.req.rem_addr = base;
  shm_mas_map_win.req.size = size;
  tsc_map_alloc(fd, &shm_mas_map_win);
  acq_buf = (char*) tsc_pci_mmap(fd, shm_mas_map_win.sts.loc_base, shm_mas_map_win.sts.size);

  if (new_size > size || new_size == 0)
  {
    new_size = size;
  }

  gap = (size - new_size);
  extr = ((trig + size - pre_trig) % size);

  if (gap <= (trig % burst_size))
  {
    if (pre_trig > 0)
    {
      extr = (((extr / burst_size) + 1) * burst_size ) % size;
    }
    else
    {
      extr = ((extr / burst_size) * burst_size) % size;
    }
  }

  start = (extr + ((gap *  prop)      / 8)        ) % size;
  end   = (extr - ((gap * (8 - prop)) / 8) + size ) % size;

#ifdef DEBUG
  printf("trig = 0x%08X, start = 0x%08X, end = 0x%08X\n", trig, start, end);
#endif /* DEBUG */

  if(mode)
  {

    acq_file = fopen(str, "w");

    if(!acq_file)
    {
      fprintf(stderr, "cannot create acquisition file %s\n", str);
      return (-1);
    }
    if (end < trig || start >= trig)
    {
      pos = gscope3110_part_save_file(start+off, size, trig, acq_buf, acq_file, inc, mask, fmt);
      pos = gscope3110_part_save_file(off,       end,  trig, acq_buf, acq_file, inc, mask, fmt);
    }
    else
    {
      pos = gscope3110_part_save_file(start+off, end, trig, acq_buf, acq_file, inc, mask, fmt);
    }
    fclose(acq_file);
  }
  else
  {
    if (end < trig || start >= trig)
    {
      pos = gscope3110_part_save_buffer(start+off, size, 0, acq_buf, str, inc, mask);
      pos = gscope3110_part_save_buffer(off, end, pos, acq_buf, str, inc, mask);
    }
    else
    {
      pos = gscope3110_part_save_buffer(start+off, end, 0, acq_buf, str, inc, mask);
    }
  }
  tsc_pci_munmap(acq_buf, shm_mas_map_win.sts.size);
  tsc_map_free(fd, &shm_mas_map_win);

  return(new_size);
}

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * Function name : gscope3110_dpram_save
 * Prototype     : int
 * Parameters    : chan      the channel from which the datas are saved
 *                 str       the name of the file or the buffer
 * 				   mode      buffer (0) or file (1)
 * 				   new_size  the size of the file
 * Return        : fail/success
 *----------------------------------------------------------------------------
 * Description   : Save the datas from a channel in a file or a buffer of a
 *                 given size
 *
 *++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

int
gscope3110_dpram_save(int fd, int fmc, int chan, char *str, int mode, int new_size)
{
  int retval, i;

  struct tsc_ioctl_kbuf_req adc_kbuf;
  unsigned short *buf;
  FILE *acq_file;
  int csr, trig, size, start, prop;

  retval = -1;
  adc_kbuf.size = 0x10000;
  adc_kbuf.k_base = NULL;
  if( tsc_kbuf_alloc(fd, &adc_kbuf))
  {
    printf("Cannot allocate kernel buffer\n");
    return( retval);
  }
  if( !tsc_kbuf_mmap(fd, &adc_kbuf))
  {
    printf("Cannot map kernel buffer in user space\n");
    tsc_kbuf_free(fd, &adc_kbuf);
    return(retval);
  }
  if (fmc == GSCOPE_FMC2)
  {
    csr = gscope_csr_rd(fd, GSCOPE_CSR_DWR2_RGB_CFG);
    trig = gscope_csr_rd(fd, GSCOPE_CSR_DWR2_TRIG_MKT);
    prop = gscope_csr_rd(fd, GSCOPE_CSR_DWR1_ACQ_MGT);
  }
  else
  {
    csr = gscope_csr_rd(fd, GSCOPE_CSR_DWR2_RGB_CFG);
    trig = gscope_csr_rd(fd, GSCOPE_CSR_DWR1_TRIG_MKT);
    prop = gscope_csr_rd(fd, GSCOPE_CSR_DWR1_ACQ_MGT);
  }
  size = 0x8000 << ((csr&GSCOPE_DPRAM_SIZE_MASK )>> 10);
  printf("adc chan %d -> buf addr = %p - %p\n", chan, adc_kbuf.k_base, adc_kbuf.u_base);
  printf("Move data from DPRAM to SYSMEM\n");

  //buf = (unsigned short *)adc_kbuf.u_base;
  //printf("%04x %04x %04x %04x \n", tsc_swap_16(buf[0]), tsc_swap_16(buf[1]), tsc_swap_16(buf[2]), tsc_swap_16(buf[3]));
  //printf("%04x %04x %04x %04x \n", tsc_swap_16(buf[4]), tsc_swap_16(buf[5]), tsc_swap_16(buf[6]), tsc_swap_16(buf[7]));
  if( gscope3110_acq_dpram_move(fd, fmc, chan, adc_kbuf.b_base, 0, size) < 0)
  {
    printf("cannot move data\n");
    goto gscope3110_dpram_save_exit;
  }
/*===> JFG check beginning of data buffer<===*/
  //buf = (unsigned short *)adc_kbuf.u_base;
  //printf("%04x %04x %04x %04x \n", tsc_swap_16(buf[0]), tsc_swap_16(buf[1]), tsc_swap_16(buf[2]), tsc_swap_16(buf[3]));
  //printf("%04x %04x %04x %04x \n", tsc_swap_16(buf[4]), tsc_swap_16(buf[5]), tsc_swap_16(buf[6]), tsc_swap_16(buf[7]));

  prop = (prop >> 5) & 0x7;
  //start = ((trig) & 0x3fffc) - ((size * prop)/8);
  start = ((trig) & (size-1)) - ((size * prop)/8);
  if( start < 0)
  {
    start = size + start;
  }
  printf("start = %x [%d] - end = %x [%d] prop = %d\n", start, start, size, size, prop);
  if(mode)
  {
    int n;

    printf("saving in file %s - Trigger mark = %x\n", str, trig);
    buf = (unsigned short *)adc_kbuf.u_base;
    acq_file = fopen(str, "w");
    if( acq_file)
    {
      n = 0;
      for( i = (start/2); i < (size/2); i++)
      {
        fprintf( acq_file, "%d, %d\n", n, (int)(tsc_swap_16(buf[i])));
	n++;
      }
      for( i = 0; i < (start/2); i++)
      {
        fprintf( acq_file, "%d, %d\n", n, (int)(tsc_swap_16(buf[i])));
	n++;
      }
    }
    else
    {
      printf("cannot create acquisition file %s\n", str);
      goto gscope3110_dpram_save_exit;
    }
    fclose(acq_file);
    retval = 0;
  }
  else
  {
    unsigned short *des;


    printf("saving in buffer %p - Trigger mark = %x\n", str, trig);
    buf = (unsigned short *)adc_kbuf.u_base;
    des = (unsigned short *)str;
    for( i = (start/2); i < (size/2); i++)
    {
      *des++ = tsc_swap_16(buf[i]);
    }
    for( i = 0; i < (start/2); i++)
    {
      *des++ = tsc_swap_16(buf[i]);
    }

  }
gscope3110_dpram_save_exit:
  tsc_kbuf_munmap(&adc_kbuf);
  tsc_kbuf_free(fd, &adc_kbuf);


  return(retval);
}

struct map{
	int *idx;
	char *string[16];
	int chan_idx;
};

int sort(struct map *mp){
	int i, j, best, tmp, length;
	char *tmp2;
	length = 3;
	for(i = 0; i < length - 1; i++){
		best = i;
		for(j = i; j < length; j++){
			if(mp->idx[j] < mp->idx[best]){
				best = j;
			}
		}
		tmp = mp->idx[i];
		mp->idx[i] = mp->idx[best];
		mp->idx[best] = tmp;

		tmp2 = mp->string[i];
		mp->string[i] = mp->string[best];
		mp->string[best] = tmp2;

		if(i == mp->chan_idx){
			mp->chan_idx = best;
		}else if(best == mp->chan_idx){
			mp->chan_idx = i;
		}
	}
	return 0;
}


/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * Function name : gscope3110_filename_generator(char* filename, int chan)
 * Prototype     : int
 * Parameters    : filename the generic name of the file
 *                 chan     the channel number
 * Return        : the new filename
 *----------------------------------------------------------------------------
 * Description   : Take a generic name and replace the parameters by their
 * values, the parameters can be the time %t, the date %d or the channel %c
 *
 *++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

char * gscope3110_filename_generator(char * filename, int chan)
{
  char *buf, *p, *q, time_str[7], date_str[7], chan_str[3];
  int l;
  time_t t;
  struct tm tm;

  if (chan < 0 && chan > 99)
  {
    return NULL;
  }

  l = 0;
  p = filename;
  while (p[0] != '\0')
  {
    if (p[0] == '%')
    {
      if (p[1] == 't' || p[1] == 'd' || p[1] == 'c')
      {
        l+= ((p[1] == 'c') ? 1 : 5);
        p++;
      }
    }
    l++;
    p++;
  }

  buf = malloc(l+1);
  if (buf != NULL)
  {
    t = time(NULL);
    tm = *localtime(&t);
    sprintf(date_str, "%02d%02d%02d", tm.tm_year - 100,tm.tm_mon + 1, tm.tm_mday);
    sprintf(time_str, "%02d%02d%02d", tm.tm_hour,tm.tm_min, tm.tm_sec);
    sprintf(chan_str, "%d", chan);

    p = filename;
    q = buf;
    while (*p != '\0')
    {
      if (p[0] == '%')
      {
        switch(p[1])
        {
          case 't':
            strcpy(q, time_str);
            q+= 6;
            p+= 2;
            continue;

          case 'd':
            strcpy(q, date_str);
            q+= 6;
            p+= 2;
            continue;

          case 'c':
            strcpy(q, chan_str);
            while (*q != '\0') q++;
            p+= 2;
            continue;

          default:
            *q++ = *p++;
            break;
        }
      }
      *q++ = *p++;
    }
    *q = '\0';
  }
  return buf;
}
