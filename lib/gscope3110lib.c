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
#include <gscope3110lib.h>
#include <adc3110lib.h>
#include <errno.h>
#include <time.h>

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
 * Function name : gscope3110_csr_rd
 * Prototype     : int
 * Parameters    : csr  register index
 * Return        : register content
 *----------------------------------------------------------------------------
 * Description   : returns the content of gscope3110 register whose index is csr
 *
 *++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

int gscope3110_csr_rd(int fd, int csr) {
	int data;
	tsc_csr_read(fd, GSCOPE3110_CSR_ADDR(csr), &data);
	return (data);
}

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * Function name : gscope3110_csr_wr
 * Prototype     : void
 * Parameters    : csr  register index
 * 				   data  data to be written in register
 * Return        : void
 *----------------------------------------------------------------------------
 * Description   : write data in the gscope3110 register whose index is csr
 *
 *++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

void gscope3110_csr_wr(int fd, int csr, int data) {
	tsc_csr_write(fd, GSCOPE3110_CSR_ADDR(csr), &data);
	return;
}

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * Function name : gscope3110_identify
 * Prototype     : int
 * Parameters    : none
 * Return        : gscope3110 signature (expect GSCOPE3110_SIGN_ID)
 *----------------------------------------------------------------------------
 * Description   : returns the content of gscope3110 signature register 
 * (GSCOPE3110_CSR_SIGN1 at index 0x60)
 *
 *++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

int gscope3110_identify(int fd) {
	int id;

	id = gscope3110_csr_rd(fd, GSCOPE3110_CSR_SIGN1);

	return (id);
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
	int ret;

	printf("in gscope3110_map_usr()\n");
	map = malloc(sizeof(struct tsc_ioctl_map_win));
	bzero((void*) map, sizeof(struct tsc_ioctl_map_win));
	map->req.rem_addr = GSCOPE3110_MAP_USR_OFF + offset;
	if (fmc == ADC3110_FMC2) {
		map->req.mode.space = MAP_SPACE_USR2;
	} else {
		map->req.mode.space = MAP_SPACE_USR1;
	}
	map->req.mode.sg_id = MAP_ID_MAS_PCIE_MEM;
	map->req.size = size;
	ret = tsc_map_alloc(fd, map);
	if (ret < 0) {
		printf("cannot allocate user map %lx, %x [err: %d-%d]\n",
				map->sts.loc_base, map->sts.size, ret, errno);
		*buf = NULL;
		return (NULL);
	}
	*buf = (char*) tsc_pci_mmap(fd, map->sts.loc_base, map->sts.size);
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
	tsc_pci_munmap(u_addr, map->sts.size);
	tsc_map_free(fd, map);
	free(map);
	return;
}

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * Function name : gscope3110_acq_smem_init
 * Prototype     : int
 * Parameters    : fmc
 * Return        : fail/success
 *----------------------------------------------------------------------------
 * Description   : 
 *
 *++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

//#ifdef JFG
//  int csr_idx, csr;
//  int rgbuf_idx;
//  int base, size;
//
//  if( mode == GSCOPE3110_RGBUF_MODE_DUAL)
//  {
//    for( rgbuf_idx = 0;  rgbuf_idx < GSCOPE3110_RGBUF_NUM_MAX; rgbuf_idx++)
//    {
//      if( chan_set & (1 << rgbuf_idx))
//      {
//	csr = (base+size) | (base >> 16);
//	base += 2*size;
//      }
//    }
//  }
//  else 
//  {
//    for( rgbuf_idx = 0;  rgbuf_idx < GSCOPE3110_RGBUF_NUM_MAX; rgbuf_idx++)
//    {
//      if( chan_set & (1 << rgbuf_idx))
//      {
//	csr = base | (base >> 16);
//	base += size;
//      }
//    }
//  }
//#endif

int gscope3110_acq_smem_init(int fd, int chan_set, int rgbuf_base,
		int rgbuf_size, int mode) 
{

	int cnt, i, base, size, chan_set_copy, mask, mask2, busy, busy2, final_addr, new_base;

	base = rgbuf_base & GSCOPE3110_RGBUF_BASE_MASK;
	size = rgbuf_size & GSCOPE3110_RGBUF_SIZE_MASK;

	cnt = 0;
	chan_set_copy = chan_set;
	for(i = 0; i < ADC3110_CHAN_NUM; i++)
	{
		cnt += chan_set_copy & 1;
		chan_set_copy = chan_set_copy >> 1;
	}

	final_addr = base + cnt * size;

	if(final_addr > GSCOPE3110_RGBUF_ADDR_MAX)
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
		gscope3110_csr_wr(fd, GSCOPE3110_CSR_SWR1_RGB_CFG, mask);
		//flush write cycle
		gscope3110_csr_rd(fd, GSCOPE3110_CSR_SWR1_RGB_CFG);
		if((chan_set_copy & 1) == 1)
		{
			gscope3110_csr_wr(fd, GSCOPE3110_CSR_SWR1_RGB_BAS, new_base);
			new_base = new_base + size;
		}
		else
		{
			gscope3110_csr_wr(fd, GSCOPE3110_CSR_SWR1_RGB_BAS, GSCOPE3110_RGBUF_ADDR_START);
		}
		chan_set_copy = chan_set_copy >> 1;
	}

	new_base = base;

	for(i = 0; i < (ADC3110_CHAN_NUM / 2); i++)
	{
		mask = size | i;
		gscope3110_csr_wr(fd, GSCOPE3110_CSR_SWR2_RGB_CFG, mask);
		//flush write cycle
		gscope3110_csr_rd(fd, GSCOPE3110_CSR_SWR2_RGB_CFG);
		if((chan_set_copy & 1) == 1)
		{
			gscope3110_csr_wr(fd, GSCOPE3110_CSR_SWR2_RGB_BAS, new_base);
			new_base = new_base + size;
		}
		else
		{
			gscope3110_csr_wr(fd, GSCOPE3110_CSR_SWR2_RGB_BAS, GSCOPE3110_RGBUF_ADDR_START);
		}
		chan_set_copy = chan_set_copy >> 1;
	}

	mask = 0x90000000 |(chan_set & 0xF);
	mask2 = 0x90000000 |(chan_set >> 4);


	/* enable FMUX & DPRAM */
	gscope3110_csr_wr(fd, GSCOPE3110_CSR_FE2_CSR1, mask);
	gscope3110_csr_wr(fd, GSCOPE3110_CSR_FE2_CSR2, mask2);
	gscope3110_csr_rd(fd, GSCOPE3110_CSR_FE2_CSR1);
	gscope3110_csr_rd(fd, GSCOPE3110_CSR_FE2_CSR2);

	/* init DPRAM withfd 0,1,2,3,.. */
	gscope3110_csr_wr(fd, GSCOPE3110_CSR_SWR1_RGB_CFG, (0xc0000010 | size));
	gscope3110_csr_wr(fd, GSCOPE3110_CSR_SWR2_RGB_CFG, (0xc0000010 | size));


	do
	{
		usleep(1000);
		busy = gscope3110_csr_rd(fd, GSCOPE3110_CSR_SWR1_RGB_CFG) & GSCOPE3110_SMEM_INIT_BUSY_MASK;
		busy2 = gscope3110_csr_rd(fd, GSCOPE3110_CSR_SWR2_RGB_CFG) & GSCOPE3110_SMEM_INIT_BUSY_MASK;
	} while(busy == GSCOPE3110_SMEM_INIT_BUSY_MASK || busy2 == GSCOPE3110_SMEM_INIT_BUSY_MASK);

	return (gscope3110_csr_rd(fd, GSCOPE3110_CSR_SWR1_ACQ_MGT));

}

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * Function name : gscope3110_acq_dpram_init
 * Prototype     : int
 * Parameters    : fmc  FMC identifier (1 or 2)
 * 				   chan_set  set of channel to be enabled for acquisition 
 * 				   			 (bitfield)
 * 				   size  buffer size
 * Return        : current value of GSCOPE3110_CSR_SWRx_ACQ_MGT register
 *----------------------------------------------------------------------------
 * Description   : initialize the data acquisition state machine and enable the 
 * channels set in chan_set.The parameter size defines the DPRAM buffer size
 *
 *++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

int gscope3110_acq_dpram_init(int fd, int fmc, int chan_set, int mirror) {
	chan_set &= GSCOPE3110_FEM_CHAN_ALL;
	chan_set |= GSCOPE3110_FEM_ENA | GSCOPE3110_FEM_FCT_1;
	if (fmc == ADC3110_FMC2) {
		/* abort any pending acquisition */
		gscope3110_csr_wr(fd, GSCOPE3110_CSR_SWR2_ACQ_MGT, 0x20000000);
		usleep(2000);
		/* return to idle state */
		gscope3110_csr_wr(fd, GSCOPE3110_CSR_SWR2_ACQ_MGT, 0x40000000);
		usleep(2000);
		/* enable FMUX & DPRAM */
		gscope3110_csr_wr(fd, GSCOPE3110_CSR_FE2_CSR2, chan_set);
		gscope3110_csr_wr(fd, GSCOPE3110_CSR_SWR2_RGB_CFG, 0x80000000);
		/* init DPRAM with 0,1,2,3,.. */
		gscope3110_csr_wr(fd, GSCOPE3110_CSR_SWR2_RGB_CFG, 0xc0000010);
		return (gscope3110_csr_rd(fd, GSCOPE3110_CSR_SWR2_ACQ_MGT));
	} else {
		/* abort any pending acquisition */
		gscope3110_csr_wr(fd, GSCOPE3110_CSR_SWR1_ACQ_MGT, 0x20000000);
		usleep(2000);
		/* return to idle state */
		gscope3110_csr_wr(fd, GSCOPE3110_CSR_SWR1_ACQ_MGT, 0x40000000);
		usleep(2000);
		/* enable FMUX & DPRAM */
		gscope3110_csr_wr(fd, GSCOPE3110_CSR_FE1_CSR2, chan_set);
		gscope3110_csr_wr(fd, GSCOPE3110_CSR_SWR1_RGB_CFG, 0x80000000);
		/* init DPRAM with 0,1,2,3,.. */
		gscope3110_csr_wr(fd, GSCOPE3110_CSR_SWR1_RGB_CFG, 0xc0000010);
		return (gscope3110_csr_rd(fd, GSCOPE3110_CSR_SWR1_ACQ_MGT));
	}
}

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * Function name : gscope3110_acq_status
 * Prototype     : int
 * Parameters    : fmc  FMC identifier (1 or 2)
 * Return        : current value of GSCOPE3110_CSR_SWRx_ACQ_MGT register
 *----------------------------------------------------------------------------
 * Description   : return the current status of the acquisition state machine
 *
 *++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

int gscope3110_acq_status(int fd, int fmc) {
	if (fmc == ADC3110_FMC2) {
		return (gscope3110_csr_rd(fd, GSCOPE3110_CSR_SWR2_ACQ_MGT));
	} else {
		return (gscope3110_csr_rd(fd, GSCOPE3110_CSR_SWR1_ACQ_MGT));
	}
}

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * Function name : gscope3110_acq_arm
 * Prototype     : int
 * Parameters    : fmc  FMC identifier (1 or 2)
 * Return        : current value of GSCOPE3110_CSR_SWRx_ACQ_MGT register
 *----------------------------------------------------------------------------
 * Description   : arms the data acquisition state machine and returns its 
 * current status
 *
 *++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

int gscope3110_acq_arm(int fd, int fmc, int trig_mode, int trig_pos, int buf_mode) {
	int csr, acq_mode, acq_trig, sync, mask;

	if (fmc == ADC3110_FMC2) 
	{
		csr = GSCOPE3110_CSR_SWR2_ACQ_MGT;
	} else 
	{
		csr = GSCOPE3110_CSR_SWR1_ACQ_MGT;
	}

	mask = GSCOPE3110_ARM_MASK;

	acq_mode = trig_mode & GSCOPE3110_ARM_CODE_MODE_MASK;
	acq_trig = trig_mode & GSCOPE3110_ARM_CODE_TRIG_MASK;
	sync = trig_mode & GSCOPE3110_ARM_CODE_SYNC_MASK;

	if(acq_mode == GSCOPE3110_ARM_CODE_CONT){
		mask = mask | (GSCOPE3110_ARM_CONT << GSCOPE3110_ARM_MODE_START);
	}
	else{
		mask = mask | (GSCOPE3110_ARM_SINGLE << GSCOPE3110_ARM_MODE_START);
	}
	if(acq_trig == GSCOPE3110_ARM_CODE_NORMAL){
		mask = mask | (GSCOPE3110_ARM_NORMAL << GSCOPE3110_ARM_TRIG_START);
	}
	else{
		mask = mask | (GSCOPE3110_ARM_AUTO << GSCOPE3110_ARM_TRIG_START);
	}
	if(sync == GSCOPE3110_ARM_CODE_MASTER){
		mask = mask | (GSCOPE3110_ARM_MASTER << GSCOPE3110_ARM_SYNC_START);
	}
	else{
		mask = mask | (GSCOPE3110_ARM_SLAVE << GSCOPE3110_ARM_SYNC_START);
	}

	mask = mask | (trig_pos << GSCOPE3110_ARM_TRIG_POS_START);

	gscope3110_csr_wr(fd, csr, mask);
	usleep(2000);

	return (gscope3110_csr_rd(fd, csr));
}
/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * Function name : gscope3110_acq_abort
 * Prototype     : int
 * Parameters    : fmc  FMC identifier (1 or 2)
 * Return        : current value of GSCOPE3110_CSR_SWRx_ACQ_MGT register
 *----------------------------------------------------------------------------
 * Description   : abort the data acquisition, put the state machine in idle 
 * and returns its current status
 *
 *++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

int gscope3110_acq_abort(int fd, int fmc) {
	if (fmc == ADC3110_FMC2) {
		gscope3110_csr_wr(fd, GSCOPE3110_CSR_SWR2_ACQ_MGT, 0x20000000);
		usleep(2000);
		/* return to idle state */
		gscope3110_csr_wr(fd, GSCOPE3110_CSR_SWR2_ACQ_MGT, 0x40000000);
		usleep(2000);
		return (gscope3110_csr_rd(fd, GSCOPE3110_CSR_SWR2_ACQ_MGT));
	} else {
		gscope3110_csr_wr(fd, GSCOPE3110_CSR_SWR1_ACQ_MGT, 0x20000000);
		usleep(2000);
		/* return to idle state */
		gscope3110_csr_wr(fd, GSCOPE3110_CSR_SWR1_ACQ_MGT, 0x40000000);
		usleep(2000);
		return (gscope3110_csr_rd(fd, GSCOPE3110_CSR_SWR1_ACQ_MGT));
	}
}
/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * Function name : gscope3110_acq_trig
 * Prototype     : void
 * Parameters    : fmc  FMC identifier (1 or 2)
 * Return        : void
 *----------------------------------------------------------------------------
 * Description   : manually trig a data acquisition
 *
 *++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

void gscope3110_acq_trig(int fd, int fmc, int chan, int mode, int offset, int hyst) {

	int csr_index, main_mode, mask, lvl_edge, sign, dir;

	if (fmc == ADC3110_FMC2) {
		csr_index = GSCOPE3110_CSR_FE2_TRIG;
	} else {
		csr_index = GSCOPE3110_CSR_FE1_TRIG;
	}



	main_mode = mode & GSCOPE3110_TRIG_CODE_MAIN_MASK ;

	if(main_mode == GSCOPE3110_TRIG_CODE_MAN)
	{
		gscope3110_csr_wr(fd, csr_index, GSCOPE3110_TRIG_MAN);
		return;
	}



	if(main_mode == GSCOPE3110_TRIG_CODE_GPIO)
	{
		gscope3110_csr_wr(fd, csr_index, GSCOPE3110_TRIG_GPIO_MASK);
		return;
	}

	gscope3110_csr_wr(fd, csr_index, GSCOPE3110_TRIG_HYST_MASK | (hyst & GSCOPE3110_TRIG_HYST_OVF));

	mask = GSCOPE3110_TRIG_ENG_MASK;
	mask = mask | (offset & GSCOPE3110_TRIG_OFFSET_OVF) | (GSCOPE3110_TRIG_HYST << GSCOPE3110_TRIG_HYST_START);
	lvl_edge = mode & GSCOPE3110_TRIG_CODE_LVL_MASK;
	dir = mode & GSCOPE3110_TRIG_CODE_DIR_MASK;
	sign = mode & GSCOPE3110_TRIG_CODE_SIGN_MASK;

	if(dir == GSCOPE3110_TRIG_CODE_UP){
		mask = mask | (GSCOPE3110_TRIG_UP << GSCOPE313_TRIG_DIR_START);
	} else {
		mask = mask | (GSCOPE3110_TRIG_DOWN << GSCOPE313_TRIG_DIR_START);
	}

	if(lvl_edge == GSCOPE3110_TRIG_CODE_LVL){
		mask = mask | (GSCOPE3110_TRIG_LVL << GSCOPE313_TRIG_LVL_START);
	} else{
		mask = mask | (GSCOPE3110_TRIG_EDGE << GSCOPE313_TRIG_LVL_START);
	}

	if(sign == GSCOPE3110_TRIG_CODE_SIGN){
		mask = mask | (GSCOPE3110_TRIG_SIGN << GSCOPE313_TRIG_SIGN_START);
	}
	else{
		mask = mask | (GSCOPE3110_TRIG_UNSIGN << GSCOPE313_TRIG_SIGN_START);
	}

	mask = mask | chan << GSCOPE3110_TRIG_CHAN_START;

	gscope3110_csr_wr(fd, csr_index, mask);
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
	if (size > 0x8000) {
		size = 0x8000;
	}
	dma_chan = 0;
	if (fmc == ADC3110_FMC2) {
		dma_chan = 2;
	}
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
	int retval;

	if (chan >= ADC3110_CHAN_NUM) {
		return (-1);
	}
	if (size > 0x8000) {
		size = 0x8000;
	}
	dma_chan = 0;
	if (fmc == ADC3110_FMC2) {
		dma_chan = 2;
	}
	if (chan < 4) {
		dma_req.src_addr = 0x10000000 + (0x2000000 * chan) + shm_off;
		dma_req.src_space = DMA_SPACE_SHM;
	} else {
		dma_req.src_addr = 0x10000000 + (0x2000000 * (chan - 4)) + shm_off;
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

int gscope3110_part_save_file(int start, int end, int pos, char *acq_buf,
		FILE *acq_file, int trig) {

	int data, i;

	for (i = start; i < end; i += 2) {
		data = *(unsigned char*) &acq_buf[i] | (*(unsigned char*) &acq_buf[i + 1] << 8);
		fprintf(acq_file, "%d %d\n", pos, (short) data);
		pos++;
	}

	return pos;
}

int gscope3110_part_save_buffer(int start, int end, int pos, char *acq_buf,
		char* output_buffer) {

	int i;

	for (i = start; i < end; i++) {
		output_buffer[pos] = acq_buf[i];
		pos++;
	}

	return pos;
}

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * Function name : gscope3110_save
 * Prototype     : int
 * Parameters    : chan       the channel from which the datas are saved
 * 				   *acq_file  the file where the datas are saved
 * 				   new_size   the size of the file
 * Return        : fail/success
 *----------------------------------------------------------------------------
 * Description   : Save the datas from a channel in a file of a given size
 *
 *++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

char* gscope3110_save(int fd, int chan, char *str, int mode, int new_size)
{

	int base, size, pos, trig, size_mask, prop, pre_trig, pre_trig_mask, extr, start, end, gap, csr;
	struct tsc_ioctl_map_win shm_mas_map_win;
	char *acq_buf;
	FILE* acq_file;

	if (chan < ADC3110_CHAN_NUM / 2) 
	{

		csr = gscope3110_csr_rd(fd, GSCOPE3110_CSR_SWR1_RGB_CFG);
		csr = (csr & GSCOPE3110_SAVE_CHAN_MASK) | chan;

		gscope3110_csr_wr(fd, GSCOPE3110_CSR_SWR1_RGB_CFG, csr);
		base = gscope3110_csr_rd(fd, GSCOPE3110_CSR_SWR1_RGB_BAS);
		trig = gscope3110_csr_rd(fd, GSCOPE3110_CSR_SWR1_TRIG_MKT);
		pre_trig = gscope3110_csr_rd(fd, GSCOPE3110_CSR_SWR1_ACQ_MGT);

	}else 
	{

		csr = gscope3110_csr_rd(fd, GSCOPE3110_CSR_SWR2_RGB_CFG);
		csr = (csr & GSCOPE3110_SAVE_CHAN_MASK) | (chan % ADC3110_CHAN_NUM / 2);
		gscope3110_csr_wr(fd, GSCOPE3110_CSR_SWR2_RGB_CFG, csr);
		base = gscope3110_csr_rd(fd, GSCOPE3110_CSR_SWR2_RGB_BAS);
		trig = gscope3110_csr_rd(fd, GSCOPE3110_CSR_SWR2_TRIG_MKT);
		pre_trig = gscope3110_csr_rd(fd, GSCOPE3110_CSR_SWR2_ACQ_MGT);

	}

	size_mask = GSCOPE3110_RGBUF_SIZE_MASK;
	size = csr & size_mask;


	pre_trig = pre_trig >> GSCOPE3110_ARM_TRIG_POS_START;

	//111...111
	pre_trig_mask = (1 << GSCOPE3110_ARM_TRIG_POS_SIZE) - 1;
	pre_trig = 	pre_trig & 	pre_trig_mask;
	prop = pre_trig;
	pre_trig *= size;
	pre_trig = pre_trig / GSCOPE3110_ARM_TRIG_POS_MAX;



	bzero(&shm_mas_map_win, sizeof(shm_mas_map_win));
	shm_mas_map_win.req.mode.sg_id = MAP_ID_MAS_PCIE_PMEM;
	shm_mas_map_win.req.mode.space = MAP_SPACE_SHM;
	if (chan >= ADC3110_CHAN_NUM / 2)
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
	else
	{
		new_size = new_size & GSCOPE3110_RGBUF_SIZE_MASK;
	}

	extr = (trig + size - pre_trig) % size;
	gap = size - new_size;

	start = (extr + ((gap * prop) / GSCOPE3110_ARM_TRIG_POS_MAX)) % size;
	end = (extr - ((gap * (GSCOPE3110_ARM_TRIG_POS_MAX - prop)) / GSCOPE3110_ARM_TRIG_POS_MAX) + size) % size;

	if(mode){
		acq_file = fopen(str, "w");

		if(!acq_file)
		{
			printf("cannot create acquisition file %s\n", str);
			return "-1";
		}
		if(end < trig || start > trig)
		{
			pos = gscope3110_part_save_file(start, size, 0, acq_buf, acq_file, trig);
			pos = gscope3110_part_save_file(0, end, pos, acq_buf, acq_file, trig);
		}
		else
		{
			pos = gscope3110_part_save_file(start, end, 0, acq_buf, acq_file, trig);
		}
		fclose(acq_file);
	}
	else{
		if(end < trig || start > trig)
		{
			pos = gscope3110_part_save_buffer(start, size, 0, acq_buf, str);
			pos = gscope3110_part_save_buffer(0, end, pos, acq_buf, str);
		}
		else
		{
			pos = gscope3110_part_save_buffer(start, end, 0, acq_buf, str);
		}
	}


	tsc_pci_munmap(acq_buf, shm_mas_map_win.sts.size);
	tsc_map_free(fd, &shm_mas_map_win);

	return str;
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
 * Function name : tsc_gscope_save
 * Prototype     : int
 * Parameters    : cli command parameter structure
 * Return        : success/error
 *----------------------------------------------------------------------------
 * Description   : adc3110 command
 *
 *++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

char* gscope3110_filename_generator(char* filename, int chan){



	int i, j, length, ch, total_l, l, first;
	char *new_filename;
	struct map m;
	time_t t = time(NULL);
	struct tm tm = *localtime(&t);



	m.idx = (int*) malloc(16);

	for(i = 0; i < 16; i++){
		m.string[i] = (char*) malloc(16);
	}

	sprintf(m.string[0], "%02d%02d%02d", tm.tm_year - 100,tm.tm_mon + 1, tm.tm_mday);
	sprintf(m.string[1], "%02d%02d%02d", tm.tm_hour,tm.tm_min, tm.tm_sec);
	sprintf(m.string[2], "%d", chan);
	m.chan_idx = 2;

	length = strlen(filename);

	for(i = 0; i < 3; i++){
		m.idx[i] = -1;
	}

	for(ch = 0; ch < length; ch++){
		if(filename[ch] == '%'){
			if(filename[ch + 1] == 'd'){
				m.idx[0] = ch;
			}
			else if(filename[ch + 1] == 'c'){
				m.idx[2] = ch;
			}
			else{
				m.idx[1] = ch;
			}
		}
	}

	sort(&m);

	i = 0;
	first = 3;
	do{
		if(m.idx[i] != -1){
			first = i;
		}
		i++;
	}while(first > 2);

	new_filename = (char*) malloc(length + 20);


	total_l = 0;
	ch = 0;
	for(i = first; i < 3; i++){
		for(; ch < m.idx[i]; ch++){
			new_filename[ch] = filename[ch - total_l];
		}
		l = strlen(m.string[i]);
		for(; ch < m.idx[i] + l; ch++){
			new_filename[ch] = m.string[i][ch - m.idx[i]];
		}
		total_l += l - 2;
		for(j = i + 1; j < 3; j++){
			m.idx[j] += l - 2;
		}
	}

	for(;ch < length + total_l; ch++){
		new_filename[ch] = filename[ch - total_l];
	}

	new_filename[ch] = 0;

	return new_filename;
}

void gscope_acq_dis(int fd, int fmc){
	if(fmc == ADC3110_FMC1){
		gscope3110_csr_wr(fd, GSCOPE3110_CSR_FE1_TRIG, 0);
		gscope3110_csr_rd(fd, GSCOPE3110_CSR_FE1_TRIG);
	}
	else{
		gscope3110_csr_wr(fd, GSCOPE3110_CSR_FE2_TRIG, 0);
		gscope3110_csr_rd(fd, GSCOPE3110_CSR_FE2_TRIG);
	}
}

