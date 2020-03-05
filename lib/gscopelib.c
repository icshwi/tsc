/*=========================< begin file & file header >=======================
 *  References
 *  
 *    filename : gscopelib.c
 *    author   : JFG, XP
 *    company  : IOxOS
 *    creation : october 18,2018
 *
 *----------------------------------------------------------------------------
 *  Description
 *
 *    That library contains a set of function to access the TOSCA II XUSER
 *    Generic SCOPE ADC data acquisition logic.
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

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <tsculib.h>
#include <tscioctl.h>
#include <tscextlib.h>
#include <gscopelib.h>
#include <adclib.h>
#include <errno.h>
#include <time.h>


/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * Function name : gscope_XXX
 * Prototype     : int
 * Parameters    : void
 * Return        : 
 *----------------------------------------------------------------------------
 * Description   : 
 *
 *++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

int gscope_XXX(void) {

	return (0);
}

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * Function name : gscope_csr_rd
 * Prototype     : int
 * Parameters    : csr  register index
 * Return        : register content
 *----------------------------------------------------------------------------
 * Description   : returns the content of gscope register whose index is csr
 *
 *++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

int gscope_csr_rd(int fd, int csr) {
	int data;
	tsc_csr_read(fd, GSCOPE_CSR_ADDR(csr), &data);
	return (data);
}

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * Function name : gscope_csr_wr
 * Prototype     : void
 * Parameters    : csr  register index
 * 				   data  data to be written in register
 * Return        : void
 *----------------------------------------------------------------------------
 * Description   : write data in the gscope register whose index is csr
 *
 *++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

void gscope_csr_wr(int fd, int csr, int data) {
	tsc_csr_write(fd, GSCOPE_CSR_ADDR(csr), &data);
	return;
}

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * Function name : gscope_identify
 * Prototype     : int
 * Parameters    : none
 * Return        : gscope signature (expect GSCOPE_SIGN_ID)
 *----------------------------------------------------------------------------
 * Description   : returns the content of gscope signature register 
 * (GSCOPE_CSR_SIGN1 at index 0x60)
 *
 *++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

int gscope_identify(int fd) {
	int id;

	id = gscope_csr_rd(fd, GSCOPE_CSR_SIGN1);

	return (id);
}


/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * Function name : gscope_map_usr
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

struct tsc_ioctl_map_win* gscope_map(int fd, int space, char **buf, int offset, int size) 
{
  struct tsc_ioctl_map_win *map;
  int ret;

  printf("in gscope_map()\n");
  map = malloc(sizeof(struct tsc_ioctl_map_win));
  bzero((void*) map, sizeof(struct tsc_ioctl_map_win));
  map->req.rem_addr = offset;
  map->req.mode.space = space;
  map->req.mode.sg_id = MAP_ID_MAS_PCIE_MEM;
  map->req.size = size;
  ret = tsc_map_alloc(fd, map);
  if (ret < 0)
  {
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
 * Function name : gscope_unmap
 * Prototype     : int
 * Parameters    : pointer to map 
 * Return        : fail/success
 *----------------------------------------------------------------------------
 * Description   : 
 *
 *++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

void gscope_unmap(int fd, struct tsc_ioctl_map_win *map, char *u_addr)
{
  tsc_pci_munmap(u_addr, map->sts.size);
  tsc_map_free(fd, map);
  free(map);
  return;
}

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * Function name : gscope_acq_status
 * Prototype     : int
 * Parameters    : fmc  FMC identifier (1 or 2)
 * Return        : current value of GSCOPE_CSR_SWRx_ACQ_MGT register
 *----------------------------------------------------------------------------
 * Description   : return the current status of the acquisition state machine
 *
 *++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

int gscope_acq_status(int fd, int fmc) {
	if (fmc == GSCOPE_FMC2) {
		return (gscope_csr_rd(fd, GSCOPE_CSR_SWR2_ACQ_MGT));
	} else {
		return (gscope_csr_rd(fd, GSCOPE_CSR_SWR1_ACQ_MGT));
	}
}

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * Function name : gscope_acq_arm
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

int gscope_acq_arm(int fd, int fmc, int trig_mode, int trig_pos, int buf_mode, int rearm) {
	int csr, acq_mode, acq_trig, sync, mask;

	if (fmc == GSCOPE_FMC2) 
	{
		csr = GSCOPE_CSR_SWR2_ACQ_MGT;
	} else 
	{
		csr = GSCOPE_CSR_SWR1_ACQ_MGT;
	}

	mask = rearm ? GSCOPE_REARM_MASK : GSCOPE_ARM_MASK;

	acq_mode = trig_mode & GSCOPE_ARM_CODE_MODE_MASK;
	acq_trig = trig_mode & GSCOPE_ARM_CODE_TRIG_MASK;
	sync = trig_mode & GSCOPE_ARM_CODE_SYNC_MASK;

	if(acq_mode == GSCOPE_ARM_CODE_CONT){
		mask = mask | (GSCOPE_ARM_CONT << GSCOPE_ARM_MODE_START);
	}
	else{
		mask = mask | (GSCOPE_ARM_SINGLE << GSCOPE_ARM_MODE_START);
	}
	if(acq_trig == GSCOPE_ARM_CODE_NORMAL){
		mask = mask | (GSCOPE_ARM_NORMAL << GSCOPE_ARM_TRIG_START);
	}
	else{
		mask = mask | (GSCOPE_ARM_AUTO << GSCOPE_ARM_TRIG_START);
	}
	if(sync == GSCOPE_ARM_CODE_MASTER){
		mask = mask | (GSCOPE_ARM_MASTER << GSCOPE_ARM_SYNC_START);
	}
	else{
		mask = mask | (GSCOPE_ARM_SLAVE << GSCOPE_ARM_SYNC_START);
	}

	mask = mask | (trig_pos << GSCOPE_ARM_TRIG_POS_START);

	gscope_csr_wr(fd, csr, mask);
	usleep(2000);

	return (gscope_csr_rd(fd, csr));
}
/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * Function name : gscope_acq_abort
 * Prototype     : int
 * Parameters    : fmc  FMC identifier (1 or 2)
 * Return        : current value of GSCOPE_CSR_SWRx_ACQ_MGT register
 *----------------------------------------------------------------------------
 * Description   : abort the data acquisition, put the state machine in idle 
 * and returns its current status
 *
 *++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

int gscope_acq_abort(int fd, int fmc) {
	if (fmc == GSCOPE_FMC2) {
		gscope_csr_wr(fd, GSCOPE_CSR_SWR2_ACQ_MGT, 0x20000000);
		usleep(2000);
		/* return to idle state */
		gscope_csr_wr(fd, GSCOPE_CSR_SWR2_ACQ_MGT, 0x40000000);
		usleep(2000);
		return (gscope_csr_rd(fd, GSCOPE_CSR_SWR2_ACQ_MGT));
	} else {
		gscope_csr_wr(fd, GSCOPE_CSR_SWR1_ACQ_MGT, 0x20000000);
		usleep(2000);
		/* return to idle state */
		gscope_csr_wr(fd, GSCOPE_CSR_SWR1_ACQ_MGT, 0x40000000);
		usleep(2000);
		return (gscope_csr_rd(fd, GSCOPE_CSR_SWR1_ACQ_MGT));
	}
}

void gscope_trig_dis(int fd, int fmc){
	if(fmc == ADC_FMC1){
		gscope_csr_wr(fd, GSCOPE_CSR_FE1_TRIG, 0);
		gscope_csr_rd(fd, GSCOPE_CSR_FE1_TRIG);
	}
	else{
		gscope_csr_wr(fd, GSCOPE_CSR_FE2_TRIG, 0);
		gscope_csr_rd(fd, GSCOPE_CSR_FE2_TRIG);
	}
}
