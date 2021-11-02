/*=========================< begin file & file header >=======================
 *  References
 *
 *    filename : dac38j84.c
 *    author   : CG, JFG, XP
 *    company  : IOxOS
 *    creation : september 23,2020
 *
 *----------------------------------------------------------------------------
 *  Description
 *
 *    That library contains a set of function to access the DA1430 data
 *    acquisition logic.
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
#include <math.h>
#include <tsculib.h>
#include <tscextlib.h>
#include <fmclib.h>
#include <adclib.h>
#include <xilinxlib.h>
#include <dac38j84lib.h>

/* ------------------------------------------------------------------------------------------------------------------ */
/* local macros used to display messages
 */

#define dac38j84_info(...)          ({ if (dac38j84_verbose_mode) fprintf(stdout, __VA_ARGS__); })
#define dac38j84_error(...)         ({ if (dac38j84_verbose_mode) fprintf(stderr, __VA_ARGS__); })

/* ------------------------------------------------------------------------------------------------------------------ */
/* Module Private's Variables
 */

static int          dac38j84_verbose_mode = 0;      /* verbose mode                       */

static int          dac38j84_spi_cmd = 0;           /* SPI command to select DAC38j84     */
static int          dac38j84_jesd204b_base = 0;     /* JESD204B interface base address    */

static xilinx_t     dac38j84_xil;                   /* Xilinx descriptor                  */

/* ------------------------------------------------------------------------------------------------------------------ */

static double dac38j84_serdes_pll_mpy[] = {
  4.0, 5.0, 6.0, 8.0, 8.25, 10.0, 12.0, 12.5, 15.0, 16.0, 16.5, 20.0, 22.0, 25.0
};

static int dac38j84_serdes_pll_mpy_enc[] = {
  0x10, 0x14, 0x18, 0x20, 0x21, 0x28, 0x30, 0x32, 0x3c, 0x40, 0x42, 0x50, 0x58, 0x64
};

/* ------------------------------------------------------------------------------------------------------------------ */

/*
 * Pre-calculated VCO frequencies for H-Band (depending on pll_vco = 0 .. 63)
 *
 * H-Band (4.44 - 5.60 GHz) -> f_VCO_H = 0.109980 * pll_vco^2 + 10.5740 * pll_vco + 4446.3
 */

static double dac38j84_pll_h_band[] =
{
  4446.30000, 4456.98398, 4467.88792, 4479.01182, 4490.35568, 4501.91950, 4513.70328, 4525.70702,
  4537.93072, 4550.37438, 4563.03800, 4575.92158, 4589.02512, 4602.34862, 4615.89208, 4629.65550,
  4643.63888, 4657.84222, 4672.26552, 4686.90878, 4701.77200, 4716.85518, 4732.15832, 4747.68142,
  4763.42448, 4779.38750, 4795.57048, 4811.97342, 4828.59632, 4845.43918, 4862.50200, 4879.78478,
  4897.28752, 4915.01022, 4932.95288, 4951.11550, 4969.49808, 4988.10062, 5006.92312, 5025.96558,
  5045.22800, 5064.71038, 5084.41272, 5104.33502, 5124.47728, 5144.83950, 5165.42168, 5186.22382,
  5207.24592, 5228.48798, 5249.95000, 5271.63198, 5293.53392, 5315.65582, 5337.99768, 5360.55950,
  5383.34128, 5406.34302, 5429.56472, 5453.00638, 5476.66800, 5500.54958, 5524.65112, 5548.97262
};

/* ------------------------------------------------------------------------------------------------------------------ */

/*
 * Pre-calculated VCO frequencies for L-Band (depending on pll_vco = 0 .. 63)
 *
 * L-Band (3.70 - 4.66 GHz) -> f_VCO_L = 0.089703 * pll_vco^2 +  8.8312 * pll_vco + 3752.5
 */

static double dac38j84_pll_l_band[] =
{
  3752.50000, 3761.42090, 3770.52121, 3779.80093, 3789.26005, 3798.89858, 3808.71651, 3818.71385,
  3828.89059, 3839.24674, 3849.78230, 3860.49726, 3871.39163, 3882.46541, 3893.71859, 3905.15118,
  3916.76317, 3928.55457, 3940.52537, 3952.67558, 3965.00520, 3977.51422, 3990.20265, 4003.07049,
  4016.11773, 4029.34438, 4042.75043, 4056.33589, 4070.10075, 4084.04502, 4098.16870, 4112.47178,
  4126.95427, 4141.61617, 4156.45747, 4171.47818, 4186.67829, 4202.05781, 4217.61673, 4233.35506,
  4249.27280, 4265.36994, 4281.64649, 4298.10245, 4314.73781, 4331.55258, 4348.54675, 4365.72033,
  4383.07331, 4400.60570, 4418.31750, 4436.20870, 4454.27931, 4472.52933, 4490.95875, 4509.56758,
  4528.35581, 4547.32345, 4566.47049, 4585.79694, 4605.30280, 4624.98806, 4644.85273, 4664.89681
};

/* ------------------------------------------------------------------------------------------------------------------ */

const char *dac38j84_atest_str[] = {
  "off",
  "DAC PLL VSSA (0V)",
  "DAC PLL VDD at DACCLK receiver and ndivider (0.9V)",
  "DAC PLL 100uA bias current measurement into 0V",
  "DAC PLL 100uA vbias at VCO (~0.8V nmos diode)",
  "DAC PLL VDD at prescaler and mdivider (0.9V)",
  "DAC PLL VSSA (0V)",
  "DAC PLL VDDA1.8 (1.8V)",
  "DAC PLL loop filter voltage (0 to 1V, ~0.5V when locked)",
  "DACA VDDA18 (1.8V)",
  "DACA VDDCLK (0.9)",
  "DACA VDDDAC (0.9)",
  "DACA VSSA (0V)",
  "DACA VSSESD (0V)",
  "DACA VSSA (0V)",
  "DACA main current source PMOS cascode bias (1.65V)",
  "DACA output switch cascode bias (0.4V)",
  "DACB VDDA18 (1.8V)",
  "DACB VDDCLK (0.9)",
  "DACB VDDDAC (0.9)",
  "DACB VSSA (0V)",
  "DACB VSSESD (0V)",
  "DACB VSSA (0V)",
  "DACB main current source PMOS cascode bias (1.65V)",
  "DACB output switch cascode bias (0.4V)",
  "DACC VDDA18 (1.8V)",
  "DACC VDDCLK (0.9)",
  "DACC VDDDAC (0.9)",
  "DACC VSSA (0V)",
  "DACC VSSESD (0V)",
  "DACC VSSA (0V)",
  "DACC main current source PMOS cascode bias (1.65V)",
  "DACC output switch cascode bias (0.4V)",
  "DACD VDDA18 (1.8V)",
  "DACD VDDCLK (0.9)",
  "DACD VDDDAC (0.9)",
  "DACD VSSA (0V)",
  "DACD VSSESD (0V)",
  "DACD VSSA (0V)",
  "DACD main current source PMOS cascode bias (1.65V)",
  "DACD output switch cascode bias (0.4V)",
  "Temp Sensor VSSA (0V)",
  "Temp Sensor amplifier output (0 to 1.8V)",
  "Temp Sensor reference output (~0.6V, can be trimmed)",
  "Temp Sensor comparator output (0 to 1.8V)",
  "Temp Sensor 64uA bias voltage (~0.8V nmos diode)",
  "BIASGEN 100uA bias measured to 0V (to be trimmed)",
  "Temp Sensor VDD (0.9V)",
  "Temp Sensor VDDA18 (1.8V)",
  "DAC bias current measured into 1.8V. scales with coarse DAC setting (7.3uA to 117uA)",
  "Bangap PTAT current measured into 0V (~20uA)",
  "CoarseDAC PMOS current source gate (~1V)",
  "RBIAS (0.9V)",
  "EXTIO (0.9V)",
  "Bandgap PMOS cascode gate (0.7V)",
  "Bandgap startup circuit output (~0V when BG started)",
  "Bandgap output (0.9V, can be trimmed)",
  "SYNCB LVDS buffer reference voltage (1.2V) must set syncb_lvds_efuse_sel to measure.",
  "VSS in digital core MET1 (0V)",
  "VSS in digital core MET1 (0V)",
  "VSS near bump (0V)",
  "VDDDIG in digital core MET1 (0.9V)",
  "VDDDIG in digital core MET1 (0.9V)",
  ""
};

/* ------------------------------------------------------------------------------------------------------------------ */

/*!+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * \brief   do an DRP read or write on Xilinx DRP interface 
 *
 * \param   cmd         XILINX_DRP_READ or XILINX_DRP_WRITE command 
 * \param   sel         selected DRP interface
 * \param   reg         DRP register address
 * \param   data        pointer to data word
 * \param   priv        pointer to private data
 * 
 *
 * \return  >=0 on success, <0 on error
 *++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

static int dac38j84_xil_func(int fd, int cmd, int sel, int reg, int *data, int priv)
{
  int ret = -1;
  
  switch(cmd)
  {
    case XILINX_DRP_READ:
      ret = dac38j84_drp_read(fd, priv, sel, reg, data);
      break;
  
    case XILINX_DRP_WRITE:
      if (data != NULL)
      {
        ret = dac38j84_drp_write(fd, priv, sel, reg, (*data));
      }
      break;

    default:
      break;
  }
  return(ret);
}

/* ------------------------------------------------------------------------------------------------------------------ */

/*!+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * \brief   Set verbose mode
 *
 * \param   mode        enable (=1) or disable (0) verbose mode
 *
 * \return  current verbose mode
 *++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

int dac38j84_set_verbose(int mode)
{
  dac38j84_verbose_mode = mode;
  return(dac38j84_verbose_mode);
}
/* ------------------------------------------------------------------------------------------------------------------ */

/*!+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * \brief   Configure DAC38j84, set SPI command and JESD204B base address
 *
 * \param   spi_cmd         SPI command to access DAC38j84 command interface
 * \param   jesd204b_base   JES204B interface base address
 *
 *++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

void dac38j84_configure(int spi_cmd, int jesd204b_base)
{
  dac38j84_spi_cmd = spi_cmd;
  dac38j84_jesd204b_base = jesd204b_base;
  
  xilinx_configure(&dac38j84_xil, &dac38j84_xil_func);
}

/* ------------------------------------------------------------------------------------------------------------------ */

/*!+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * \brief   Read a data value from register of DAC38j84 on SPI bus
 *
 * \param   fmc             fmc slot number (1 or 2)
 * \param   reg             register address
 * \param   data            integer pointer to store data read
 *
 * \return  >= on success, <0 on error
 *++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

int dac38j84_spi_read(int fd, int fmc, int reg, int *data)
{
  return adc_spi_read(fd, fmc, dac38j84_spi_cmd, (reg&0xffff), data);
}

/* ------------------------------------------------------------------------------------------------------------------ */

/*!+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * \brief   Write a data value into register of DAC38j84 on SPI bus
 *
 * \param   fmc             fmc slot number (1 or 2)
 * \param   reg             register address
 * \param   data            data value to write
 *
 * \return  >= on success, <0 on error
 *++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

int dac38j84_spi_write(int fd, int fmc, int reg, int data)
{
  return adc_spi_write(fd, fmc, dac38j84_spi_cmd, (reg&0xffff), data);
}

/* ------------------------------------------------------------------------------------------------------------------ */

/*!+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * \brief   Initialize DAC38j84 SPI interface
 *
 * \param   fmc         fmc slot number (1 or 2)
 *
 * \return  >=0 on success, <0 on error
 *++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

int dac38j84_spi_init(int fd, int fmc)
{
  int ret;

  if (fmc < 0 || fmc > 2)
  {
    return(-1);
  }

  /* reset SIF registers */
  ret = dac38j84_spi_write(fd, fmc, 0x0002, 0x0001);
  if (ret < 0) {
    return(ret);
  }

  /* Enable SIF 4 terminal mode + twos complement */
  ret = dac38j84_spi_write(fd, fmc, 0x0002, 0x2082);
  if (ret < 0) {
    return(ret);
  }

  return(1);
}

/* ------------------------------------------------------------------------------------------------------------------ */

/*!+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * \brief   Display all SPI registers of DAC38j84
 *
 * \param   fmc         fmc slot number (1 or 2)
 *
 * \return  >=0 on success, <0 on error
 *++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

int dac38j84_spi_dump(int fd, int fmc)
{
  int i, reg, data, ret;

  if (fmc < 0 || fmc > 2)
  {
    return(-1);
  }

  printf("Displaying all SPI registers of DAC38j84:\n");

  for (reg=0; reg<128; reg += 16)
  {
    printf("\n%02X : ", reg);
    for (i=0; i<16; i++)
    {
      ret = dac38j84_spi_read(fd, fmc, (reg+i), &data);
      if (ret < 0) return(ret);
      printf("%04X ", (data & 0xffff));
    }
  }
  printf("\n\n");

  return(1);
}

/* ------------------------------------------------------------------------------------------------------------------ */

/*!+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * \brief   Display all SPI registers of DAC38j84
 *
 * \param   fmc           fmc slot number (1 or 2)
 * \param   status_main   main error flags
 * \param   status_lane   error flags per lane 
 *
 * \return  >=0 on success, <0 on error
 *++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

int dac38j84_read_status(int fd, int fmc, int *status_main, int *status_lane)
{
  int rc = 0, tmp, i;

  if (status_main != NULL)
  {
    rc = dac38j84_spi_read(fd, fmc, 0x006C, &tmp);
    if (rc >= 0)
    {
      (*status_main) = tmp;
    }
  }

  if (rc >= 0 && status_lane != NULL)
  {
    for (i=0; i<8; i++)
    {
      rc = dac38j84_spi_read(fd, fmc, (0x0064+i), &tmp);
      if (rc < 0)
      {
        break;
      }
      status_lane[i] = tmp;
    }
  }
  return(rc);
}

/* ------------------------------------------------------------------------------------------------------------------ */

/*!+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * \brief   Clear all alarms
 *
 * \param   fmc         fmc slot number (1 or 2)
 *
 * \return  >=0 on success, <0 on error
 *++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

int dac38j84_clear_status(int fd, int fmc)
{
  int r, rc;

  for (r=0x64; r<=0x6C; r++)
  {
    rc = dac38j84_spi_write(fd, fmc, r, 0);
    if (rc < 0)
    {
      break;
    }
  }
  return(rc);
}

/* ------------------------------------------------------------------------------------------------------------------ */

/*!+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * \brief   Write into FPGA DRP interface
 *
 * \param   fmc         fmc slot number (1 or 2)
 * \param   sel         select DRP port (0-9)
 * \param   reg         register address
 * \param   data        data to write
 *
 * \return  =1 on success, =0 on timeout
 *++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

int dac38j84_drp_write(int fd, int fmc, int sel, int reg, int data)
{
  int tmp, to, rc;
  
  
  rc = dac38j84_jesd_write(fd, fmc, 9, (data & 0xffff));
  if (rc < 0)
  {
    return(rc);
  }

  tmp = (DAC38J84_DRP_GO | ((sel&0xf)<<16) | (reg & 0xFFFF));
    
  rc = dac38j84_jesd_write(fd, fmc, 8, tmp);
  if (rc < 0)
  {
    return(rc);
  }

  to = 100;
  
  while (to > 0)
  {
    rc = dac38j84_jesd_read(fd, fmc, 8, &tmp);
    if (rc < 0)
    {
      return(rc);
    }
    if ((tmp & DAC38J84_DRP_BUSY) == 0)
      break;

    usleep(1000);
    to --;
  }
  return (((tmp & DAC38J84_DRP_TIMEOUT) != 0) ? 0 : 1);
}

/* ------------------------------------------------------------------------------------------------------------------ */

/*!+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * \brief   Read from FPGA DRP interface 
 *
 * \param   fmc         fmc slot number (1 or 2)
 * \param   sel         select DRP unit
 * \param   reg         register address
 * \param   data        pointer to a variable to store data read
 *
 * \return  =1 on success, =0 on timeout, <0 on error
 *++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

int dac38j84_drp_read(int fd, int fmc, int sel, int reg, int *data)
{
  int tmp, to, rc;

  if (data == NULL)
  {
    return(-1);
  }
    
  tmp = (DAC38J84_DRP_GO | DAC38J84_DRP_RnW | ((sel&0xf)<<16) | (reg & 0xFFFF));
    
  rc = dac38j84_jesd_write(fd, fmc, 8, tmp);
  if (rc < 0)
  {
    return(rc);
  }

  to = 100;
  
  while (to > 0)
  {
    rc = dac38j84_jesd_read(fd, fmc, 8, &tmp);
    if (rc < 0)
    {
      return(rc);
    }
    if ((tmp & DAC38J84_DRP_BUSY) == 0)
      break;

    usleep(1000);
    to --;
  }

  if ((tmp & DAC38J84_DRP_TIMEOUT) == 0)
  {
    rc = dac38j84_jesd_read(fd, fmc, 9, data);
    if (rc < 0)
    {
      return(rc);
    }
  }

  return (((tmp & DAC38J84_DRP_TIMEOUT) != 0) ? 0 : 1);
}

/* ------------------------------------------------------------------------------------------------------------------ */

/*!+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * \brief   Write into JESD204B interface register
 *
 * \param   fmc         fmc slot number (1 or 2)
 * \param   reg         register address
 * \param   data        data to write
 *
 * \return  >=0 on success, <0 on error
 *++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

int dac38j84_jesd_write(int fd, int fmc, int reg, int data)
{
  int ret;

  ret = fmc_csr_write(fd, fmc, dac38j84_jesd204b_base, reg);
  if (ret<0) {
    return(ret);
  }

  ret = fmc_csr_write(fd, fmc, (dac38j84_jesd204b_base+1), data);

  return(ret);
}

/* ------------------------------------------------------------------------------------------------------------------ */

/*!+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * \brief   Read from JESD204B interface register
 *
 * \param   fmc         fmc slot number (1 or 2)
 * \param   reg         register address
 * \param   data        pointer to a variable to store data read
 *
 * \return  >=0 on success, <0 on error
 *++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

int dac38j84_jesd_read(int fd, int fmc, int reg, int *data)
{
  int ret;

  ret = fmc_csr_write(fd, fmc, dac38j84_jesd204b_base, reg);
  if (ret<0) {
    return(ret);
  }

  ret = fmc_csr_read(fd, fmc, (dac38j84_jesd204b_base+1), data);

  return(ret);
}

/* ------------------------------------------------------------------------------------------------------------------ */

/*!+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * \brief   Display all JESD registers of DAC38j84
 *
 * \param   fmc         fmc slot number (1 or 2)
 *
 * \return  >=0 on success, <0 on error
 *++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

int dac38j84_jesd_dump(int fd, int fmc)
{
  int rc, tmp;

  const char *ila_fsm_str[] = {
    "IDLE", "SYNC", "", "", "INIT_0", "INIT_CFG", "INIT_1", "INIT_2", "RUN"
  };

  if (fmc < 0 || fmc > 2)
  {
    return(-1);
  }

  printf("\nDisplaying all JESD registers of DAC38j84:\n");

  rc = fmc_csr_read(fd, fmc, dac38j84_jesd204b_base, &tmp);
  if (rc < 0)
  {
    return(rc);
  }
  /* Display Main Status bits */
  printf("  GT_TX_RESET_DONE      = %d\n", (tmp>>16)&1);
  printf("  GT228_PLL_LOCK        = %d\n", (tmp>>18)&1);
  printf("  GT226_PLL_LOCK        = %d\n", (tmp>>19)&1);
  printf("  GT228_REFCLK_LOST     = %d\n", (tmp>>20)&1);
  printf("  GT226_REFCLK_LOST     = %d\n", (tmp>>21)&1);
  printf("  DAC_ALARM             = %d\n", (tmp>>24)&1);


  if ((tmp&(1<<16)) && (tmp&(1<<18)) && (tmp&(1<<19)))
  {
    rc = dac38j84_jesd_read(fd, fmc, 0, &tmp);
    if (rc < 0)
    {
      return(rc);
    }

    printf("  ENABLE                = %d\n",  (tmp>>0)&1);
    printf("  SCRAMBLE              = %d\n",  (tmp>>1)&1);
    printf("  TESTMODE              = %d\n",  (tmp>>4)&7);
    printf("  NUM_ILA               = %d\n", ((tmp>>8)&0xFF)+1);
    printf("  TX_CFG_F              = %d\n", ((tmp>>16)&0xFF)+1);
    printf("  TX_CFG_K              = %d\n", ((tmp>>24)&0x1F)+1);
    printf("  TX_CFG_SUBCLASS       = %d\n",  (tmp>>29)&7);

    rc = dac38j84_jesd_read(fd, fmc, 1, &tmp);
    if (rc < 0)
    {
      return(rc);
    }

    printf("  TX_CFG_DID            = %d\n",  (tmp>>0)&0xFF);
    printf("  TX_CFG_BID            = %d\n",  (tmp>>8)&0xF);
    printf("  TX_CFG_L              = %d\n", ((tmp>>16)&0x1F)+1);
    printf("  TX_CFG_M              = %d\n", ((tmp>>24)&0xFF)+1);

    rc = dac38j84_jesd_read(fd, fmc, 2, &tmp);
    if (rc < 0)
    {
      return(rc);
    }

    printf("  TX_CFG_N              = %d\n", ((tmp>>0)&0x1F)+1);
    printf("  TX_CFG_CS             = %d\n",  (tmp>>5)&0x3);
    printf("  TX_CFG_HD             = %d\n",  (tmp>>7)&1);
    printf("  TX_CFG_NP             = %d\n", ((tmp>>8)&0x1F)+1);
    printf("  TX_CFG_S              = %d\n", ((tmp>>16)&0x1F)+1);
    printf("  TX_CFG_CF             = %d\n",  (tmp>>24)&1);

    rc = dac38j84_jesd_read(fd, fmc, 3, &tmp);
    if (rc < 0)
    {
      return(rc);
    }

    printf("  TX_CFG_ADJCNT         = %d\n", (tmp>>00)&0xF);
    printf("  TX_CFG_ADJDIR         = %d\n", (tmp>>04)&1);
    printf("  TX_CFG_PHADJ          = %d\n", (tmp>>05)&1);

    rc = dac38j84_jesd_read(fd, fmc, 4, &tmp);
    if (rc < 0)
    {
      return(rc);
    }

    printf("  GT_RESET              = %d\n", (tmp>>0)&1);
    printf("  GT_ENABLE             = %d\n", (tmp>>1)&1);
    printf("  GT_REFCLK_SEL         = %d\n", (tmp>>2)&1);
    printf("  SYNC_POL              = %d\n", (tmp>>3)&1);
    printf("  SYNC_SEL              = %d\n", (tmp>>4)&3);
    printf("  SYSREF_ENABLE         = %d\n", (tmp>>6)&1);
    printf("  SYSREF_MODE           = %d\n", (tmp>>7)&1);
    printf("  SYSREF_VAL            = %d\n", (tmp>>8)&0xFF);
    printf("  DAC_SLEEP             = %d\n", (tmp>>16)&1);
    printf("  DAC_TXENA             = %d\n", (tmp>>17)&1);

    rc = dac38j84_jesd_read(fd, fmc, 5, &tmp);
    if (rc < 0)
    {
      return(rc);
    }

    printf("  ILA_FSM_LANE_0        = %s (%d)\n", ila_fsm_str[(tmp>>0)&15],  (tmp>>0)&15);
    printf("  ILA_FSM_LANE_1        = %s (%d)\n", ila_fsm_str[(tmp>>4)&15],  (tmp>>4)&15);
    printf("  ILA_FSM_LANE_2        = %s (%d)\n", ila_fsm_str[(tmp>>8)&15],  (tmp>>8)&15);
    printf("  ILA_FSM_LANE_3        = %s (%d)\n", ila_fsm_str[(tmp>>12)&15], (tmp>>12)&15);
    printf("  ILA_FSM_LANE_4        = %s (%d)\n", ila_fsm_str[(tmp>>16)&15], (tmp>>16)&15);
    printf("  ILA_FSM_LANE_5        = %s (%d)\n", ila_fsm_str[(tmp>>20)&15], (tmp>>20)&15);
    printf("  ILA_FSM_LANE_6        = %s (%d)\n", ila_fsm_str[(tmp>>24)&15], (tmp>>24)&15);
    printf("  ILA_FSM_LANE_7        = %s (%d)\n\n", ila_fsm_str[(tmp>>28)&15], (tmp>>28)&15);
  }
  else
  {
    fprintf(stderr, "GT_228 QPLL & GT_226 QPLL are NOT running!\n");
  }

  return(1);
}

/* ------------------------------------------------------------------------------------------------------------------ */

/*!+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * \brief   RESET JESD204 QPLL
 *
 * \param   fmc         fmc slot number (1 or 2)
 *
 * \return  1= QPLL locked 0= QPLL unlocked, <0 on error
 *++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

int dac38j84_jesd_qpll_reset(int fd, int fmc)
{
  int rc, tmp, to, mask;

  /* GT_RESET = 1 */
  rc = dac38j84_jesd_write(fd, fmc, 0x4, DAC38J84_JESD_GT_RESET);
  if (rc < 0)
  {
    return(rc);
  }

  /* GT_ENABLE = 1 */
  rc = dac38j84_jesd_write(fd, fmc, 0x4, DAC38J84_JESD_GT_ENABLE);
  if (rc < 0)
  {
    return(rc);
  }

  to = 100;
  mask = (DAC38J84_JESD_GT_TX_RESET_DONE | DAC38J84_JESD_GT228_PLL_LOCK | DAC38J84_JESD_GT226_PLL_LOCK);

  while (to>0)
  {
    rc = fmc_csr_read(fd, fmc, dac38j84_jesd204b_base, &tmp);
    if (rc < 0)
    {
      return(rc);
    }

    tmp &= mask;
    if (tmp == mask)
    {
      break;
    }

    usleep(100);
    to--;
  }

  if (tmp == mask)
  {
    dac38j84_info("dac38j84: JESD GT_228 & GT_226 QPLL LOCKED !\n");
  }
  else
  {
    dac38j84_error("dac38j84: JESD GT_228 & GT_226 QPLL UNLOCKED !\n");
  }

  return((tmp == mask) ? 1 : 0);
}

/* ------------------------------------------------------------------------------------------------------------------ */

/*!+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * \brief   (Re-)configure JESD204B QPLL
 *
 * \param   fmc           fmc slot number (1 or 2)
 * \param   freq_refclk   REFCLK frequency in MHz
 * \param   linerate      linerate in Gbit/s
 *
 * \return  1= QPLL locked 0= QPLL unlocked, <0 on error
 *++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

int dac38j84_jesd_qpll_configure(int fd, int fmc, double freq_refclk, double linerate)
{
  int rc, tmp;
  
  /* assert GT_RESET (i.e. place QPLL0 of GT_226 & GT_228 in RESET */
  rc = dac38j84_jesd_read(fd, fmc, 0x4, &tmp);
  if (rc < 0)
  {
    return(rc);
  }

  tmp |= (DAC38J84_JESD_GT_RESET | DAC38J84_JESD_GT_ENABLE);

  rc = dac38j84_jesd_write(fd, fmc, 0x4, tmp);
  if (rc < 0)
  {
    return(rc);
  }

  /* reconfigure GT_226 & GT_228 QPLLs */
  rc = xilinx_qpll_configure(&dac38j84_xil, fd,  DAC38J84_DRP_SEL_GT_226_COMMON, 0, freq_refclk, linerate, fmc);
  if (rc < 0)
  {
    return(rc);
  }

  rc = xilinx_qpll_configure(&dac38j84_xil, fd, DAC38J84_DRP_SEL_GT_228_COMMON, 0, freq_refclk, linerate, fmc);
  if (rc < 0)
  {
    return(rc);
  }

  return dac38j84_jesd_qpll_reset(fd, fmc);
}

/* ------------------------------------------------------------------------------------------------------------------ */

/*!+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * \brief   Configure SYNC pin
 *
 * \param   fmc         fmc slot number (1 or 2)
 * \param   sync_sel    sync selection (DAC_SYNCB, DAC_SYNC_N_AB or
 *                      DAC_SYNC_N_CD)
 * \param   sync_pol    sync polarity (positive or negative)
 *
 * \return  0>= on success or <0 on error
 *++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

int dac38j84_jesd_set_sync(int fd, int fmc, int sync_sel, int sync_pol)
{
  int rc, tmp;

  rc = dac38j84_jesd_read(fd, fmc, 0x4, &tmp);
  if (rc >= 0)
  {
    tmp &= ~(DAC38J84_JESD_SYNC_POL | DAC38J84_JESD_SYNC_SEL(3));
    tmp |= (((sync_sel&3)<<4) | ((sync_pol&1)<<3));

    rc = dac38j84_jesd_write(fd, fmc, 0x4, tmp);
  }

  if (rc >= 0)
  {
    switch(sync_sel)
    {
      default:
      case DAC38J84_JESD_DAC_SYNCB:
        tmp = (0xf);
        break;

      case DAC38J84_JESD_DAC_SYNC_N_AB:
        tmp = (0xf0 | ((sync_pol&1)<<15));
        break;

      case DAC38J84_JESD_DAC_SYNC_N_CD:
        tmp = (0xf00 | ((sync_pol&1)<<15));
        break;
    }

    rc = dac38j84_spi_write(fd, fmc, 0x0061, tmp);
  }

  if (rc >= 0)
  {
    dac38j84_info("dac38j84: SYNC%s pin used for JESD synchronization.\n",
      ((sync_sel == DAC38J84_JESD_DAC_SYNC_N_AB) ? "_N_AB" : ((sync_sel == DAC38J84_JESD_DAC_SYNC_N_CD) ? "_N_CD" : "B")));
  }

  return(rc);
}

/* ------------------------------------------------------------------------------------------------------------------ */

/*!+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * \brief   Configure SYNC pin
 *
 * \param   fmc         fmc slot number (1 or 2)
 * \param   mode        SYSREF on SYNC or periodic
 * \param   val         REFCLK frequency / (val - 1)
 *
 * \return  0>= on success or <0 on error
 *++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

int dac38j84_jesd_set_sysref_mode(int fd, int fmc, int mode, int val)
{
  int rc, tmp, link, clk_div, sysref_ena, sysref_mode;

  if (mode == DAC38J84_SYSREF_DISABLED)
  {
    link = 0;
    clk_div = 0;
    sysref_ena = 0;
    sysref_mode = 0;
  }
  else
  {
    link = 1;
    clk_div = 1;
    sysref_ena = 1;
    sysref_mode = ((mode == DAC38J84_SYSREF_PERIODIC) ? 1 : 0);
  }

  /* Set SYSREF mode for links */
  tmp = (((link&7)<<12) | ((link&7)<<8) | ((link&7)<<4) | (link&7));
  rc = dac38j84_spi_write(fd, fmc, 0x005C, tmp);
  if (rc < 0)
  {
    return(rc);
  }

  /* Set SYSREF mode for clock dividers */
  tmp = ((clk_div&7)<<4);
  rc = dac38j84_spi_write(fd, fmc, 0x0024, tmp);
  if (rc < 0)
  {
    return(rc);
  }

  /* Enable SYSREF generation */
  rc = dac38j84_jesd_read(fd, fmc, 0x4, &tmp);
  if (rc >= 0)
  {
    tmp &= 0xFFFF03F;
    tmp |= ((((val-1) & 0xff) << 8) | (sysref_mode << 7) | (sysref_ena << 6));
    rc = dac38j84_jesd_write(fd, fmc, 0x4, tmp);
  }

  if (rc >= 0)
  {
    dac38j84_info("dac38j84: %s SYSREF generation\n", (sysref_ena ? "enable" : "disable"));
  }

  return(rc);
}

/* ------------------------------------------------------------------------------------------------------------------ */

/*!+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * \brief   Configure DAC_TXENA & DAC_SLEEP pins
 *
 * \param   fmc         fmc slot number (1 or 2)
 * \param   dac_txena   Enable TX on DAC (direct control of DAC_TXENA pin)
 * \param   dac_sleep   Put DAC in sleep mode (direct control of DAC_SLEEP pin)
 *
 * \return  0>= on success or <0 on error
 *++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

int dac38j84_jesd_dac_txenable(int fd, int fmc, int dac_txena, int dac_sleep)
{
  int rc, tmp;

  rc = dac38j84_jesd_read(fd, fmc, 0x4, &tmp);
  if (rc >= 0)
  {
    tmp &= ~(DAC38J84_JESD_DAC_TXENA | DAC38J84_JESD_DAC_SLEEP);
    tmp |= (((dac_txena&1)<<17) | ((dac_sleep&1)<<16));

    rc = dac38j84_jesd_write(fd, fmc, 0x4, tmp);
  }
  if (rc >= 0)
  {
    dac38j84_info("dac38j84: %s DAC outputs ! (DAC_TXENA pin)\n", (dac_txena ? "enable" : " disable"));
  }

  return(rc);
}

/* ------------------------------------------------------------------------------------------------------------------ */

/*!+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * \brief   Configure  LANE 0 to 7
 *
 * \param   fmc         fmc slot number (1 or 2)
 * \param   rdb         RDB
 * \param   f           number of octet(s) per frame
 * \param   k           number of frame(s) per mult-frame
 * \param   m           number of converter (always 4)
 * \param   l           number of lanes (always 8)
 * \param   scr         scramble enable
 * \param   hd          high-density enable
 *
 * \return  >=0 on success, <0 on error
 *++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

int dac38j84_jesd_configure(int fd, int fmc, int rdb, int f, int k, int m, int l, int s, int scr, int hd)
{
  int rc, tmp/*, s*/;

  /* number of converter samples per frame */
  /*s = 1;*/

  /* Configure DAC38j84 */

  /* place DAC38j84 JESD interface into INIT_STATE */
  tmp = 0xff1e;
  rc = dac38j84_spi_write(fd, fmc, 0x004A, tmp);
  if (rc < 0)
  {
    return(rc);
  }

  /* RBD, F, K, L, K, M, S, SCR, HD  */
  tmp = ((((rdb-1) & 0x1f) << 8) | ((f-1) & 0xff));
  rc = dac38j84_spi_write(fd, fmc, 0x004B, tmp);
  if (rc < 0)
  {
    return(rc);
  }

  tmp = ((((k-1) & 0x1f) << 8) | ((l-1) & 0x1f));
  rc = dac38j84_spi_write(fd, fmc, 0x004C, tmp);
  if (rc < 0)
  {
    return(rc);
  }

  tmp = ((((m-1) & 0x1f) << 8) | ((s-1) & 0x1f));
  rc = dac38j84_spi_write(fd, fmc, 0x004D, tmp);
  if (rc < 0)
  {
    return(rc);
  }

  tmp = ((15<<8) | ((hd&1)<<6) | ((scr&1)<<5) | 15);
  rc = dac38j84_spi_write(fd, fmc, 0x004E, tmp);
  if (rc < 0)
  {
    return(rc);
  }

  rc = dac38j84_spi_write(fd, fmc, 0x004F, 0x1C21);
  if (rc < 0)
  {
    return(rc);
  }

  rc = dac38j84_spi_write(fd, fmc, 0x0050, 0x0000);
  if (rc < 0)
  {
    return(rc);
  }

  for (l=0; l<3; l++)
  {
    rc = dac38j84_spi_write(fd, fmc, 0x0051+(3*l), 0x00FF);
    if (rc < 0)
    {
      return(rc);
    }

    rc = dac38j84_spi_write(fd, fmc, 0x0052+(3*l), 0x00FF);
    if (rc < 0)
    {
      return(rc);
    }
  }

  /* Configure JESD Interface */
  tmp = ((((m-1)&0xff)<<24) | (((l-1)&0x1f)<<16));
  rc = dac38j84_jesd_write(fd, fmc, 0x1, tmp);
  if (rc < 0)
  {
    return(rc);
  }

  tmp = ((hd&1)<<7);
  rc = dac38j84_jesd_write(fd, fmc, 0x2, tmp);
  if (rc < 0)
  {
    return(rc);
  }

  tmp = ((((k-1)&0x1f)<<24) | (((f-1)&0xff)<<16) | ((scr&1)<<1));
  rc = dac38j84_jesd_write(fd, fmc, 0x0, tmp);
  if (rc < 0)
  {
    return(rc);
  }

  dac38j84_info("dac38j84: JESD configuration (rdb = %d, f = %d, k = %d, m = %d, l = %d, scr = %d, hd = %d)\n",
    rdb, f, k, m, l, scr, hd);

  return (0);
}

/* ------------------------------------------------------------------------------------------------------------------ */

/*!+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * \brief   Start JESD204B interface
 *
 * \param   fmc         fmc slot number (1 or 2)
 *
 * \return  >=0 on success, <0 on error
 *++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

int dac38j84_jesd_start(int fd, int fmc)
{
  int rc, tmp;

  /* Enable JESD204B TX interface */
  rc = dac38j84_jesd_read(fd, fmc, 0, &tmp);
  if (rc < 0)
  {
    return(rc);
  }

  tmp |= 1;

  rc = dac38j84_jesd_write(fd, fmc, 0, tmp);
  if (rc < 0)
  {
    return(rc);
  }

  /* Start JESD204B link initialization */
  tmp = 0xff01;
  rc = dac38j84_spi_write(fd, fmc, 0x004a, tmp);


  if (rc >= 0)
  {
    dac38j84_info("dac38j84: enable JESD links\n");
  }
  return(rc);
}

/* ------------------------------------------------------------------------------------------------------------------ */

/*!+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * \brief   Change ATEST selection
 *
 * \param   fmc         fmc slot number (1 or 2)
 * \param   atest_sel   Internal signals selections on ATEST pin.
 *
 * \return  >=0 on success, <0 on error
 *++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

int dac38j84_set_atest(int fd, int fmc, int atest_sel)
{
  int ret, tmp, data;

  ret = dac38j84_spi_read(fd, fmc, 0x001B, &data);
  if (ret < 0) {
    return(ret);
  }

  tmp = ((data & 0xFFC0) | (atest_sel & 0x3F));

  ret = dac38j84_spi_write(fd, fmc, 0x001B, tmp);

  return(ret);
}

/* ------------------------------------------------------------------------------------------------------------------ */

/*!+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * \brief   Set the interpolation amount
 *
 * \param   fmc             fmc slot number (1 or 2)
 * \param   interp          interpolation factor (1, 2, 4, 8 or 16)
 *
 * \return  >=0 on success, <0 on error
 *++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

int dac38j84_set_interpolation(int fd, int fmc, int interp)
{
  int rc, tmp, v;

  if (interp != 1 &&
      interp != 2 &&
      interp != 4 &&
      interp != 8 &&
      interp != 16)
  {
    return(-1);
  }

  rc = dac38j84_spi_read(fd, fmc, 0x0000, &tmp);
  if (rc >= 0)
  {
    tmp &= ~(0xf<<8);
    v = (interp / 2);
    tmp |= ((v&0xf) << 8);

    rc = dac38j84_spi_write(fd, fmc, 0x0000, tmp);
  }

  return(rc);
}

/* ------------------------------------------------------------------------------------------------------------------ */

/*!+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * \brief   Set polarity of LANE 0 to 7
 *
 * \param   fmc         fmc slot number (1 or 2)
 * \param   polarity    invert polarity when set (=1)
 *
 * \return  >=0 on success, <0 on error
 *++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

int dac38j84_set_polarity(int fd, int fmc, int polarity)
{
  return dac38j84_spi_write(fd, fmc, 0x003F, (polarity&0xff));
}

/* ------------------------------------------------------------------------------------------------------------------ */

/*!+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * \brief   Calculate and set the JESD clock divider
 *
 * \param   fmc         fmc slot number (1 or 2)
 * \param   interp      interpolation (1, 2, 4, 8 or 16)
 * \param   l           number of lane
 * \param   m           number of converter
 *
 * \return  0>= on success or <0 on error
 *++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

int dac38j84_set_jesd_clock(int fd, int fmc, int interp, int l, int m)
{
  int rc, tmp, div, exp;

  div = interp * (l / m);

  if (div != 1 &&
      div != 2 &&
      div != 4 &&
      div != 8 &&
      div != 16 &&
      div != 32)
  {
    return(-1);
  }

  exp = 0;
  while (div > 1)
  {
    div /= 2;
    exp ++;
  }

  tmp = ((exp&7)<<13);

  rc = dac38j84_spi_write(fd, fmc, 0x0025, tmp);

  return(rc);
}

/* ------------------------------------------------------------------------------------------------------------------ */

/*!+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * \brief   calculate the best VCO frequency and best DAC frequency
 *
 * \param   freq_in     input frequency at DAC (DACCLK_N/DACCLK_P pin)
 * \param   freq_out    output frequency used for DAC outputs (DACCLK)
 * \param   config49    pointer to configuration register 49 value
 * \param   config50    pointer to configuration register 50 value
 * \param   config51    pointer to configuration register 51 value
 *
 * \return  >=1 on success, <0 on error
 *++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

int dac38j84_pll_calc_param(double freq_in, double freq_out, int *config49, int *config50, int *config51)
{
  double div_h[64], err_h[64], fout_h[64], fout_h_err[64], div_l[64], err_l[64], fout_l[64], fout_l_err[64], tol;
  double best_fout_l, best_fout_l_err, best_fout_h, best_fout_h_err, p_div, m_div, pm_div_l, pm_div_h;
  int best_fout_l_idx, best_fout_h_idx;
  int i, found, tmp;
  int pll_n, pll_p, pll_m, pll_vcosel, pll_vco, pll_vcoitune, pll_cp_adj;
  double f_vco, f_dacout;

  if (config49 == NULL || config50 == NULL || config51 == NULL)
  {
    return(-1);
  }

  /* tolerance */
  tol = 0.05;
  found = 0;

  best_fout_l_err = 1.0;
  best_fout_h_err = 1.0;

  best_fout_l_idx = -1;
  best_fout_h_idx = -1;

  /* find best VCO frequencies */
  for (i=0; i<64; i++)
  {
    div_l[i] = (dac38j84_pll_l_band[i] / freq_in);
    div_h[i] = (dac38j84_pll_h_band[i] / freq_in);
    pm_div_l = round(div_l[i]);
    pm_div_h = round(div_h[i]);

    err_l[i] = fabs(div_l[i] - pm_div_l);
    err_h[i] = fabs(div_h[i] - pm_div_h);

    if (err_l[i] <= tol)
    {
      fout_l[i] = (dac38j84_pll_l_band[i] / freq_out);
      p_div = round(fout_l[i]);
      fout_l_err[i] = fabs(fout_l[i] - p_div);
      m_div = (pm_div_l / p_div);

      if ((m_div - floor(m_div)) == 0.0 )
      {
        /* P divider should be 2  .. 9, 10 or 12 */
        if ((p_div >= 2.0 && p_div <= 9.0) || p_div == 10.0 || p_div == 12.0)
        {
          if (best_fout_l_idx == -1 || (fout_l_err[i] < best_fout_l_err))
          {
            best_fout_l     = fout_l[i];
            best_fout_l_err = fout_l_err[i];
            best_fout_l_idx = i;
            found++;
          }
        }
      }
    }
    else
    {
      fout_l[i] = 0.0;
      fout_l_err[i] = 1.0;
    }

    if (err_h[i] <= tol)
    {

      fout_h[i] = (dac38j84_pll_h_band[i] / freq_out);
      p_div = round(fout_h[i]);
      fout_h_err[i] = fabs(fout_h[i] - p_div);
      m_div = (pm_div_h / p_div);

      if ((m_div - floor(m_div)) == 0.0 )
      {
        /* P divider should be 2  .. 9, 10 or 12 */
        if ((p_div >= 2.0 && p_div <= 9.0) || p_div == 10.0 || p_div == 12.0)
        {
          if (best_fout_h_idx == -1 || (fout_h_err[i] < best_fout_h_err))
          {
            best_fout_h     = fout_h[i];
            best_fout_h_err = fout_h_err[i];
            best_fout_h_idx = i;
            found++;
          }
        }
      }
    }
    else
    {
      fout_h[i] = 0.0;
      fout_h_err[i] = 1.0;
    }
  }

  /* no valid frequency found */
  if (found == 0)
  {
    dac38j84_error("dac38j84: no valid PLL frequency found !\n");
    return(-1);
  }

  if (best_fout_l_err < best_fout_h_err)
  {
    /* use L-BAND */
    pll_n = 1;
    pll_p = (int)round(best_fout_l);
    pll_m = (int)round(div_l[best_fout_l_idx]) / pll_p;
    pll_vcosel = 1;
    pll_vco = best_fout_l_idx;
    f_vco = dac38j84_pll_l_band[best_fout_l_idx];
    f_dacout = dac38j84_pll_l_band[best_fout_l_idx] / best_fout_l;
  }
  else
  {
    /* use H-BAND */
    pll_n = 1;
    pll_p = (int)round(best_fout_h);
    pll_m = (int)round(div_h[best_fout_h_idx]) / pll_p;
    pll_vcosel = 0;
    pll_vco = best_fout_h_idx;
    f_vco = dac38j84_pll_h_band[best_fout_h_idx];
    f_dacout = dac38j84_pll_h_band[best_fout_h_idx] / best_fout_h;
  }
  pll_vcoitune = 3;
  pll_cp_adj = 12;

  tmp = ((*config49) & 0x1800);
  tmp |= (3<<13) | (1<<10) | ((pll_n-1) << 3);
  (*config49) = tmp;

  tmp = ((pll_m-1) << 8) | (((pll_p == 12) ? 12 : ((pll_p == 10) ? 11 : (pll_p - 2))) << 4);
  (*config50) = tmp;

  tmp = (pll_vcosel << 15) | (pll_vco << 9) | (pll_vcoitune << 7) | (pll_cp_adj << 2);
  (*config51) = tmp;

  dac38j84_info("dac38j84: PLL configuration: f_vco = %g MHz, f_dacout = %g MHZ, pll_n = %d, pll_m = %d, pll_p = %d, pll_vcosel = %d, pll_vco = %d\n",
    f_vco, f_dacout, pll_n, pll_m, pll_p, pll_vcosel, pll_vco);

  return (1);
}

/* ------------------------------------------------------------------------------------------------------------------ */

/*!+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * \brief   configure PLL with the best VCO frequency and best DAC frequency
 *
 * \param   fmc         fmc slot number (1 or 2)
 * \param   freq_in     input frequency at DAC (DACCLK_N/DACCLK_P pin)
 * \param   freq_out    output frequency used for DAC outputs (DACCLK)
 *
 * \return  =1 PLL locked, =0 PLL unlocked, <0 on error
 *++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

int dac38j84_pll_configure(int fd, int fmc, double freq_in, double freq_out)
{
  int config49, config50, config51, rc;

  rc = dac38j84_pll_calc_param(freq_in, freq_out, &config49, &config50, &config51);
  if (rc < 0)
  {
    return(rc);
  }

  dac38j84_info("dac38j84: PLL configuration: config49 = 0x%04X, config50 = 0x%04X, config51 = 0x%04X\n",
    config49, config50, config51);

  rc = dac38j84_spi_write(fd, fmc, 0x001A, 0x0000);
  if (rc < 0)
  {
    return(rc);
  }

  /* Assert PLL reset */
  config49 |= (1<<12);

  rc = dac38j84_spi_write(fd, fmc, 0x0031, config49);
  if (rc < 0)
  {
    return(rc);
  }

  rc = dac38j84_spi_write(fd, fmc, 0x0032, config50);
  if (rc < 0)
  {
    return(rc);
  }


  rc = dac38j84_spi_write(fd, fmc, 0x0033, config51);
  if (rc < 0)
  {
    return(rc);
  }

  /* Release PLL reset */
  config49 &= ~(1<<12);

  rc = dac38j84_spi_write(fd, fmc, 0x0031, config49);
  if (rc < 0)
  {
    return(rc);
  }

  /* Unmask DAC PLL lock alarm */
  rc = dac38j84_spi_write(fd, fmc, 0x0005, 0xFFFE);
  if (rc < 0)
  {
    return(rc);
  }

  rc = dac38j84_check_status(fd, fmc, 100, DAC38J84_DAC_PLL);
  if (rc < 0)
  {
    return(rc);
  }

  if (rc == 0)
  {
    dac38j84_info("dac38j84: PLL locked successfully !\n");
  }
  else
  {
    dac38j84_info("dac38j84: PLL unlocked ! (0x%04X)\n", rc);
  }

  /* PLL is locked */
  return((rc == 0) ? 1 : 0);
}


/* ------------------------------------------------------------------------------------------------------------------ */

/*!+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * \brief   configure SerDes PLL
 *
 * \param   freq_in     DAC output frequency
 * \param   linerate    SerDes Line Rate
 * \param   config59    pointer to configuration register 59
 * \param   config60    pointer to configuration register 60
 * \param   config62    pointer to configuration register 62
 *
 * \return  >=1 on success, <0 on error
 *++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

int dac38j84_serdes_pll_calc_param(double freq_in, double linerate, int *config59, int *config60, int *config62)
{
  int refclk_div, rate, mpy, vrange, lb, tmp, i;
  double r_div, f_vco, f_refclk, m_div;

  if (config59 == NULL || config60 == NULL || config62 == NULL)
  {
    return(-1);
  }

  /* base frequency outside the valid range ? */
  if ((freq_in < 100.0) || (freq_in / 16.0) > 800.0)
  {
    dac38j84_error("dac38j84: input frequency outside valid range !\n");
    return(-1);
  }

  /* linerate too slow or too fast */
  if (linerate < 0.78125 || linerate > 12.5)
  {
    dac38j84_error("dac38j84: SerDes linerate outside valid range !\n");
    return(-1);
  }

  /* find best refclk frequency (i.e. in 100.0 .. 800.0 range) */
  for (refclk_div=0; refclk_div<16; refclk_div++)
  {
    f_refclk = (freq_in / (double)(refclk_div+1));
    if (f_refclk >= 100.0 && f_refclk <= 800.0)
      break;
  }

  /* find best VCO frequency */
  for (rate=0, r_div=0.25; rate < 4; rate ++, r_div *= 2.0)
  {
    f_vco = linerate * (double) r_div;
    if (f_vco >= 1.5625 && f_vco <= 3.125)
      break;
  }

  //m_div = (f_vco / f_refclk);

  /* find a mpy */

  mpy = -1;

  for (i=0; i<sizeof(dac38j84_serdes_pll_mpy)/sizeof(double); i++)
  {
    m_div = dac38j84_serdes_pll_mpy[i];
    if ((f_vco * 1000.0) == (m_div * f_refclk))
    {
      mpy = dac38j84_serdes_pll_mpy_enc[i];
      break;
    }
  }

  if (mpy == -1)
  {
    dac38j84_error("dac38j84: unable to find a valid SerDes PLL multiplier !\n");
    return (-1);
  }

  tmp = ((1<<15) | (refclk_div<<11));
  (*config59) = tmp;

  vrange = ((f_vco < 2.17) ? 1 : 0);

  /* Ultra high loop bandwidth for mpy >= 8 and Meduium loop bandwidth */
  lb = ((m_div < 8.0) ? 0 : 1);

  tmp = ( (lb<<11) | (vrange<<9) | (mpy<<1));
  (*config60) = tmp;

  /* TERM = AC-coupling, BUSWIDTH = 20-bit */
  tmp =  ((1<<8) | (rate<<5) | (2<<2));
  (*config62) = tmp;

  dac38j84_info("dac38j84: SerDes PLL configuration: f_vco = %g GHz, f_refclk = %g MHz, m_div = %g, mpy = %X, r_div = %g, rate = %d, linerate = %g Gbit/s\n",
    f_vco, f_refclk, m_div, mpy, r_div, rate, linerate);

  return (1);
}

/* ------------------------------------------------------------------------------------------------------------------ */

/*!+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * \brief   configure SerDes PLL
 *
 * \param   fmc         FMC slot number (1 or 2)
 * \param   freq_in     DAC output frequency
 * \param   linerate    SerDes Line Rate
 *
 * \return  =1 on success, =0 on PLL unlocked, <0 on error
 *++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

int dac38j84_serdes_pll_configure(int fd, int fmc, double freq_in, double linerate)
{
  int config59, config60, config62, rc;

  rc = dac38j84_serdes_pll_calc_param(freq_in, linerate, &config59, &config60, &config62);
  if (rc < 0)
  {
    return(rc);
  }

  dac38j84_info("dac38j84: SerDes PLL configuration: config59 = 0x%04X, config60 = 0x%04X, config62 = 0x%04X\n",
    config59, config60, config62);

  /* Set Interpolation x1 */
  rc = dac38j84_spi_write(fd, fmc, 0x0000, 0x0018);
  if (rc < 0)
  {
    return(rc);
  }

  /* EQ_MODE = Fully adaptive, Enable Offset Compensation */
  rc = dac38j84_spi_write(fd, fmc, 0x003D, 0x0088);
  if (rc < 0)
  {
    return(rc);
  }

  /* Initialize SerDes PLL */
  rc = dac38j84_spi_write(fd, fmc, 0x003b, config59);
  if (rc < 0)
  {
    return(rc);
  }

  rc = dac38j84_spi_write(fd, fmc, 0x003c, config60);
  if (rc < 0)
  {
    return(rc);
  }

  rc = dac38j84_spi_write(fd, fmc, 0x003e, config62);
  if (rc < 0)
  {
    return(rc);
  }

  /* Keep JESD204B in RESET and Enable all LANEs!*/
  rc = dac38j84_spi_write(fd, fmc, 0x004A, 0xff1e);
  if (rc < 0)
  {
    return(rc);
  }

  /* Unmask SerDes block #0 & #1 PLL lock & SYSREF alarms (error on links & setup/hold) */
  rc = dac38j84_spi_write(fd, fmc, 0x0005, 0x0FF0);
  if (rc < 0)
  {
    return(rc);
  }

  rc = dac38j84_check_status(fd, fmc, 100, (DAC38J84_DAC_PLL|DAC38J84_SERDES_BLK0_PLL|DAC38J84_SERDES_BLK1_PLL));
  if (rc < 0)
  {
    return(rc);
  }

  if (rc == 0)
  {
    dac38j84_info("dac38j84: SerDes PLL locked successfully !\n");
  }
  else
  {
    dac38j84_info("dac38j84: SerDes PLL unlocked ! (0x%04X)\n", rc);
  }

  return ((rc == 0) ? 1 : 0);
}

/* ------------------------------------------------------------------------------------------------------------------ */

/*!+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * \brief   Verify that no alarm is coming after a while
 *
 * \param   fmc         FMC slot number (1 or 2)
 * \param   timeout     timeout value in milliseconds
 * \param   alarm_mask  Mask of alarm, we are interesting in
 *
 * \return  =0: all PLL locked, <0: on error,
 *          >0: some PLL are not locked
 *              bit #0 = DAC PLL unlocked,
 *              bit #2 = SerDes Block #0 PLL unlocked
 *              bit #3 = SerDes Block #0 PLL unlocked
 *              bit #12-15 = SYSREF errors for lane 0 to 3
 *++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

int dac38j84_check_status(int fd, int fmc, int timeout, int alarm_mask)
{
  int to, tmp, rc;

  to = timeout;

  while(to>0)
  {
    rc = dac38j84_spi_read(fd, fmc, 0x006C, &tmp);
    if (rc < 0)
    {
      return(rc);
    }

    tmp &= alarm_mask;

    /* no alarm ? */
    if (tmp == 0)
    {
      return(0);
    }

    /* clear all alarms */
    rc = dac38j84_spi_write(fd, fmc, 0x006C, 0x0);
    if (rc  < 0)
    {
      return (rc);
    }
    usleep(1000);
    to--;
  }

  return(tmp);
}

/* ------------------------------------------------------------------------------------------------------------------ */

/*!+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * \brief   Verify that no alarm is coming after a while
 *
 * \param   fmc         FMC slot number (1 or 2)
 * \param   lane_mask   lane mask (0xFF = all)
 * \param   timeout     timeout value in milliseconds
 * \param   alarm_mask  Mask of alarm, we are interesting in
 *
 * \return  =1: error detected, =0: no error detect, <0: on fatal error
 *++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

int dac38j84_check_lane_status(int fd, int fmc, int lane_mask, int timeout, int alarm_mask)
{
  int to, tmp, rc, l, status_ok, status[8];

  to = timeout;

  while (to>0)
  {
    status_ok = 0;

    rc = dac38j84_read_status(fd, fmc, NULL, (int *)&status);
    if (rc < 0)
    {
      return(rc);
    }

    for (l=0; l<8; l++)
    {
      if (lane_mask & (1<<l))
      {
        tmp = (status[l] & alarm_mask);
      }
      else
      {
        tmp = 0;
      }

      if (tmp == 0)
      {
        status_ok++;
        continue;
      }
    }

    if (status_ok == 8)
    {
      return(0);
    }

    /* clear all alarms */
    rc = dac38j84_clear_status(fd, fmc);
    if (rc < 0)
    {
      return(rc);
    }

    usleep(1000);
    to--;
  }
  return(1);
}

/* ------------------------------------------------------------------------------------------------------------------ */
