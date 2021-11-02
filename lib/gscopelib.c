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

/*----------------------------------------------------------------------------------------------------------------------*/

static int gscope_adc_map_init = 0;
static int gscope_adc_map[10];

/*----------------------------------------------------------------------------------------------------------------------*/

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
 * Function name : gscope_acq_release_buf
 * Prototype     : int
 * Parameters    : fmc  FMC identifier (1 or 2)
 *                 buf    0 = primary, 1 = secondary
 * Return        : current value of GSCOPE_CSR_SWRx_ACQ_MGT register
 *----------------------------------------------------------------------------
 * Description   : set buffer ready again
 *
 *++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

int gscope_acq_release_buf(int fd, int fmc, int buf)
{
  int csr, tmp, ret;
  
  ret = -1;
  
  csr = ((fmc == GSCOPE_FMC2) ? GSCOPE_CSR_SWR2_ACQ_MGT : GSCOPE_CSR_SWR1_ACQ_MGT);

  tmp = gscope_csr_rd(fd, csr);
  
  if (tmp & GSCOPE_ACQ_DUALBUF_ENA)
  {
    tmp &= 0x0fffffff;
    if (buf == -1)
    {
      /* buffer filled -> set ready again */
      if (tmp & GSCOPE_ACQ_DUALBUF0_DONE) 
      {
        tmp |= GSCOPE_ACQ_DUALBUF0_RDY;
      }

      /* buffer filled -> set ready again */
      if (tmp & GSCOPE_ACQ_DUALBUF1_DONE) 
      {
        tmp |= GSCOPE_ACQ_DUALBUF1_RDY;
      }
    }
    else
    {
      tmp |= ((buf&1) ? GSCOPE_ACQ_DUALBUF1_RDY : GSCOPE_ACQ_DUALBUF0_RDY);
    }    
    gscope_csr_wr(fd, csr, tmp);
    ret = tmp;
  }

  return(ret);
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
	int csr, acq_mode, acq_trig, sync, mask, tmp;

  mask = 0;
  
  if (fmc == GSCOPE_FMC2)
  {
    csr = GSCOPE_CSR_SWR2_ACQ_MGT;
  } 
  else
  {
    csr = GSCOPE_CSR_SWR1_ACQ_MGT;
  }

  tmp = gscope_csr_rd(fd, csr);

  /* Dual Buffer Mode Enabled */
  if (tmp & GSCOPE_ACQ_DUALBUF_ENA)
  {
    /* Set Primary & Secondary Buffer READY */
    mask |= (GSCOPE_ACQ_DUALBUF0_RDY | GSCOPE_ACQ_DUALBUF1_RDY | GSCOPE_ACQ_DUALBUF_ENA);
  }

  mask |= (rearm ? GSCOPE_REARM_MASK : GSCOPE_ARM_MASK);

  acq_mode = (trig_mode & GSCOPE_ARM_CODE_MODE_MASK);
  acq_trig = (trig_mode & GSCOPE_ARM_CODE_TRIG_MASK);
  sync     = (trig_mode & GSCOPE_ARM_CODE_SYNC_MASK);

  if (acq_mode == GSCOPE_ARM_CODE_CONT)
  {
    mask |= (GSCOPE_ARM_CONT << GSCOPE_ARM_MODE_START);
  }
  else
  {
    mask |= (GSCOPE_ARM_SINGLE << GSCOPE_ARM_MODE_START);
  }
  if (acq_trig == GSCOPE_ARM_CODE_NORMAL)
  {
    mask |= (GSCOPE_ARM_NORMAL << GSCOPE_ARM_TRIG_START);
  }
  else
  {
    mask |= (GSCOPE_ARM_AUTO << GSCOPE_ARM_TRIG_START);
  }
  if (sync == GSCOPE_ARM_CODE_MASTER)
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
 * Function name : gscope_acq_abort
 * Prototype     : int
 * Parameters    : fmc  FMC identifier (1 or 2)
 * Return        : current value of GSCOPE_CSR_SWRx_ACQ_MGT register
 *----------------------------------------------------------------------------
 * Description   : abort the data acquisition, put the state machine in idle
 * and returns its current status
 *
 *++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

int gscope_acq_abort(int fd, int fmc)
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

void gscope_trig_dis(int fd, int fmc){
	if(fmc == 1){
		gscope_csr_wr(fd, GSCOPE_CSR_FE1_TRIG, 0);
		gscope_csr_rd(fd, GSCOPE_CSR_FE1_TRIG);
	}
	else{
		gscope_csr_wr(fd, GSCOPE_CSR_FE2_TRIG, 0);
		gscope_csr_rd(fd, GSCOPE_CSR_FE2_TRIG);
	}
}

/*----------------------------------------------------------------------------------------------------------------------*/

void gscope_dump(int fd, int fmc)
{
  int i, data, tmp;
  int fmc_type, scope_type, rg_buf_size, rg_buf_sta;
  char *scope_type_str, chan_active_str[10];
  char *rg_buf_sta_str[] = {
    "working",
    "ended successfully",
    "incoherent active channel or no clock from ADC",
    "smem access time-out"
  };

  if (fmc < 1 || fmc > 2)
  {
    return;
  }

  tmp = gscope_csr_rd(fd, GSCOPE_CSR_SIGN1+4*(fmc&1));

  fmc_type   =  (tmp      & 0xffff);
  scope_type = ((tmp>>16) & 0xffff);

  switch(scope_type)
  {
    case 0x1235:
      scope_type_str = "SMEM GENERIC_SCOPE application";
      break;

    case 0x1236:
      scope_type_str = "DPRAM GENERIC_SCOPE application";
      break;

    default:
      scope_type_str = "unknown application";
      break;
  }

  printf("SCOPE%d : %04X - %s\n", fmc, scope_type, scope_type_str);

  tmp = gscope_csr_rd(fd, GSCOPE_CSR_MAP1+4*(fmc-1));

  if (!(fmc_type == 0x1430 && fmc == 2))
  {
    printf("FMC#%d  : %04X\n",      fmc, fmc_type);
    printf("FMC#%d is %s\n", fmc, ((tmp&11) ? "PRESENT" : "ABSENT"));
  }

  if (scope_type == 0x1236)
  {
    printf("DPRAM buffer size : %d kbytes\n", 32*((tmp>>8)&3));
  }

  for (i=0; i<7; i++)
  {
    tmp = ((tmp & 0xFF0FFFFF) | (i<<20));

    gscope_csr_wr(fd, GSCOPE_CSR_MAP1+4*(fmc-1), tmp);
    data = gscope_csr_rd(fd, GSCOPE_CSR_MAP1+4*(fmc-1));
    printf("SCOPE%d channel %d -> ADC channel %d\n", fmc, i, ((data>>12)&0xff));
  }

  if (fmc_type == 0x1430)
  {
    tmp = gscope_csr_rd(fd, GSCOPE_CSR_FE1_CSR2);
  }
  else
  {
    tmp = gscope_csr_rd(fd, GSCOPE_CSR_FE1_CSR2+4*(fmc-1));
  }

  for (i = 0; i < 8; i++)
  {
    chan_active_str[i] = ((tmp&(1<<i)) ? ('0'+i):'-');
  }
  if (fmc_type == 0x1430)
  {
    for (;i<10; i++)
    {
      chan_active_str[i] = ((tmp&(1<<i)) ? ('0'+i):'-');
    }
  }
  chan_active_str[i] = 0;
  printf("SCOPE%d channel active : %s\n", fmc, chan_active_str);

  printf("SCOPE%d function=%d %s\n", fmc, ((tmp>>28)&7), ((tmp&(1<<31)) ? "enabled" : "disabled"));

  /* write in order to get a valid frequency */
  gscope_csr_wr(fd, GSCOPE_CSR_FE1_CSR3+4*(fmc-1), 0);
  usleep(1000);
  tmp = gscope_csr_rd(fd, GSCOPE_CSR_FE1_CSR3+4*(fmc-1));

  if (!(fmc_type == 0x1430 && fmc == 2))
  {
    printf("SCOPE%d ADCDIR frequency = %d MHz\n", fmc, (tmp&0xFFF));
  }

  /* TODO: add trigger */
  /*tmp = gscope_csr_rd(fd, GSCOPE_CSR_FE1_TRIG+0x10*(fmc&1));*/

  if (scope_type == 0x1236)
  {
    /* TODO: add DPRAM */
  }

  if (scope_type == 0x1235)
  {
    tmp = gscope_csr_rd(fd, GSCOPE_CSR_SWR1_RGB_CFG+8*(fmc-1));
    rg_buf_size = ((tmp>>16)&0x1fff);
    rg_buf_size = (rg_buf_size == 0 ? 8192 : rg_buf_size) * 64;
    rg_buf_sta = ((tmp>>5)&3);


    printf("SCOPE%d burst size %d kbytes\n",        fmc, ((tmp & (1<<29))? 4 : 2));
    printf("SCOPE%d ring buffer size %d kbytes\n",  fmc, rg_buf_size);
    printf("SCOPE%d buffer status '%s' (%d) \n",    fmc, rg_buf_sta_str[rg_buf_sta], rg_buf_sta);

    for (i=0; i<8; i++)
    {
      tmp = ((tmp & 0xFFFFFFF8) | i);
      gscope_csr_wr(fd, GSCOPE_CSR_SWR1_RGB_CFG+8*(fmc-1), tmp);

      data = gscope_csr_rd(fd, GSCOPE_CSR_SWR1_RGB_BAS+8*(fmc-1));

      printf("SCOPE%d buffer[%d] primary base address = 0x%04X0000, secondary base address = 0x%04X0000\n",
        fmc, i, ((data>>16)&0xffff), (data & 0xffff));
    }
  }

}

/*----------------------------------------------------------------------------------------------------------------------*/

/*!+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * \brief   Build a mapping of scope channel for each ADC
 *++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

void gscope_mux_build_map(int fd)
{
  int i, tmp, adc;

  memset(gscope_adc_map, 0, sizeof(gscope_adc_map));

  /* read mapping for SCOPE_1 */
  tmp = gscope_csr_rd(fd, GSCOPE_CSR_MAP1);

  for (i=0; i<8; i++)
  {
    tmp &= 0xFF0FFFFF;
    tmp |= ((i&0xf)<<20);
    gscope_csr_wr(fd, GSCOPE_CSR_MAP1, tmp);
    tmp = gscope_csr_rd(fd, GSCOPE_CSR_MAP1);
    adc = ((tmp>>12)&0xff);
    if (adc >= 0 && adc <= 9)
    {
      gscope_adc_map[adc] |= (1<<i);
    }
  }

  /* read mapping for SCOPE_2 */
  tmp = gscope_csr_rd(fd, GSCOPE_CSR_MAP2);

  for (i=0; i<8; i++)
  {
    tmp &= 0xFF0FFFFF;
    tmp |= ((i&0xf)<<20);
    gscope_csr_wr(fd, GSCOPE_CSR_MAP2, tmp);
    tmp = gscope_csr_rd(fd, GSCOPE_CSR_MAP2);
    adc = ((tmp>>12)&0xff);
    if (adc >= 0 && adc <= 9)
    {
      gscope_adc_map[adc] |= (1<<(i+8));
    }
  }
  gscope_adc_map_init = 1;
}

/*----------------------------------------------------------------------------------------------------------------------*/

/*!+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * \param   adc         ADC number (0 to 9)
 *
 * \return  bitmap, bit 0 to  7 are for scope_1 channel #0 to #7
 *                  bit 8 to 15 are for scope_2 channel #0 to #7
 *----------------------------------------------------------------------------
 * \brief   this function returns the scope mapping per ADC
 *
 * \note    gscope_mux_build_map() must be called at least once and after
 *          each modification of the mapping.
 *++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

int gscope_mux_get_map(int fd, int adc)
{
  int map = 0;

  if (gscope_adc_map_init == 0)
  {
    gscope_mux_build_map(fd);
  }

  if (adc >= 0 && adc <= 9)
  {
    map = gscope_adc_map[adc];
  }
  return map;
}

/*----------------------------------------------------------------------------------------------------------------------*/

/*!+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * \param   adc         ADC number (0 to 9)
 *
 * \return  >=0: 0 -  7 -> scope_1 channel number
 *               8 - 15 -> scope_2 channel number + 8
 *          <0: ADC is not conncted to a scope channel
 *----------------------------------------------------------------------------
 * \brief   this function returns the scope channel for an ADC
 *
 * \note    gscope_mux_build_map() must be called at least once and after
 *          each modification of the mapping.
 *++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

int gscope_mux_get_scope_chan(int fd, int adc)
{
  int i, map;

  if (gscope_adc_map_init == 0)
  {
    gscope_mux_build_map(fd);
  }

  if (adc >= 0 && adc <= 9)
  {
    map = gscope_adc_map[adc];

    for (i=0; i<16; i++)
    {
      if (map & (1<<i))
      {
        return(i);
      }
    }
  }
  return(-1);
}

/*----------------------------------------------------------------------------------------------------------------------*/
