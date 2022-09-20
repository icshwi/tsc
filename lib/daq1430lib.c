/*=========================< begin file & file header >=======================
 *  References
 *
 *    filename : daq1430lib.c
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
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <tsculib.h>
#include <tscextlib.h>
#include <fmclib.h>
#include <adclib.h>
#include <lmklib.h>
#include <xilinxlib.h>
#include <ads42lb69lib.h>
#include <ltc2489lib.h>
#include <dac38j84lib.h>
#include <daq1430lib.h>

/* ------------------------------------------------------------------------------------------------------------------ */
/* local macros used to display messages
 */

#define daq1430_info(...)          ({ if (daq1430_verbose_mode) fprintf(stdout, __VA_ARGS__); })
#define daq1430_error(...)         ({ if (daq1430_verbose_mode) fprintf(stderr, __VA_ARGS__); })

/* ------------------------------------------------------------------------------------------------------------------ */
/* Module Private's Variables
 */

static int daq1430_verbose_mode = 0;      /* verbose mode       */

static int daq1430_spi_ads[DAQ1430_CHAN_NUM] =
{
  DAQ1430_SPI_ADS01,
  DAQ1430_SPI_ADS01,
  DAQ1430_SPI_ADS23,
  DAQ1430_SPI_ADS23,
  DAQ1430_SPI_ADS45,
  DAQ1430_SPI_ADS45,
  DAQ1430_SPI_ADS67,
  DAQ1430_SPI_ADS67,
  DAQ1430_SPI_ADS89,
  DAQ1430_SPI_ADS89
};

/* initalization register values for ADS42LB69 */
static int daq1430_ads42lb69_init_regs[] = {
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x80, 0x00,   /* 0x00 - 0x07  */
  0x04, 0x00, 0x00, 0x04, 0x04, 0x6C, 0x00, 0x00,   /* 0x08 - 0x0F  */
  0x00, 0x00, 0x00, 0x00, 0x08, 0x00, 0x00, 0x00,   /* 0x10 - 0x17  */
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,   /* 0x18 - 0x1F  */
  0x00                                              /* 0x20         */
};

/* initialization register values for LMK04806 */

/* clock distribution mode */
static int daq1430_lmk04806_init_clkdist[] =
{
  /*
   * CLKin_0   -> FMC_CLK2                           -> OFF
   * CLKin_1   -> RTM_CLK0                           -> 250.00 MHz
   *
   * CLKOUT_0  -> -                                  -> OFF
   * CLKOUT_1  -> ADC_CLK_IN_67                      -> 250 MHz / LVDS
   * CLKOUT_2  -> FMC_CLK_M2C                        -> 250 MHz / LVDS
   * CLKOUT_3  -> ADC_CLK_IN_45                      -> 250 MHz / LVDS
   * CLKOUT_4  -> DAC_CLK_MIRROR                     -> 250 MHz / LVDS
   * CLKOUT_5  -> DAC_CLK                            -> 250 MHZ / LVPECL
   * CLKOUT_6  -> -                                  -> OFF
   * CLKOUT_7  -> ADC_CLK_IN_01                      -> 250 MHz / LVDS
   * CLKOUT_8  -> ADC_CLK_IN_23                      -> 250 MHz / LVDS
   * CLKOUT_9  -> -                                  -> OFF
   * CLKOUT_10 -> ADC_CLK_IN_89                      -> 250 MHz / LVDS
   * CLKOUT_11 -> -                                  -> OFF
   * OSCout_0  ->                                    -> OFF
   */
  0x00000020,   /* LMK04806_R00 Enable + ClkOUT0_DIV = 10  <(Test point R184))              */
  0x00000020,   /* LMK04806_R01 Enable  ClkOut_1 + ClkOUT0_DIV = 1  -> 250/1 = 250 MHz      */
  0x00000020,   /* LMK04806_R02 Enable  ClkOut_2 + ClkOUT0_DIV = 1                          */
  0x00000020,   /* LMK04806_R03 Enable  ClkOut_3 + ClkOUT0_DIV = 1                          */
  0x00000020,   /* LMK04806_R04 Enable  ClkOut_4 + ClkOUT0_DIV = 1                          */
  0x00000020,   /* LMK04806_R05 Enable  ClkOut_5 + ClkOUT0_DIV = 1  -> 250/1 = 250 MHz      */

  0x11100000,   /* LMK04806_R06 CLKOUT_3,  CLKOUT_2, CLKOUT_1 -> LVDS                       */
  0x10410000,   /* LMK04806_R07 CLKOUT_7,  CLKOUT_5, CLKOUT_4 -> LVDS / LVPECL              */
  0x01010000,   /* LMK04806_R08 CLKOUT_10, CLKOUT_8           -> LVDS                       */

  0x55555540,   /* LMK04806_R09 TI/NS write MUST                                            */
  0x11484000,   /* LMK04806_R10 OscOUT_Type = 1 (LVDS)  Powerdown                           */
/*  0x84010000,*/   /* LMK04806_R11 Device MODE=0x6 + No SYNC output                            */
  0x87F08000,   /* LMK04806_R11 Device MODE=0x6 + No SYNC output                            */
  0x130C0000,   /* LMK04806_R12 LD pin programmable                                         */
  0x3B23A260,   /* LMK04806_R13 HOLDOVER pin uWIRE SDATOUT  Enable CLKin1                   */
  0x00000000,   /* LMK04806_R14 Bipolar Mode CLKin1 INPUT                                   */
  0x00000000,   /* LMK04806_R15 DAC unused                                                  */
  0x01550400,   /* LMK04806_R16 OSC IN level                                                */
  0x00000000,   /* LMK04806_R17                                                             */
  0x00000000,   /* LMK04806_R18                                                             */
  0x00000000,   /* LMK04806_R19                                                             */
  0x00000000,   /* LMK04806_R20                                                             */
  0x00000000,   /* LMK04806_R21                                                             */
  0x00000000,   /* LMK04806_R22                                                             */
  0x00000000,   /* LMK04806_R23                                                             */
  0x00000000,   /* LMK04806_R24 PLL1 not used  / PPL2                                       */
  0x00000000,   /* LMK04806_R25 DAC config not used                                         */
  0x03a00000,   /* LMK04806_R26 PLL2 used  / ICP = 3200 uA                                  */
  0x00000000,   /* LMK04806_R27 PLL1 not used                                               */
  0x00000000,   /* LMK04806_R28 PLL2_R = 2 /PPL1 N divider = 00                             */
  0x00000000,   /* LMK04806_R29 OSCIN_FREQ /PLL2_NCAL = 25)                                 */
  0x00000000,   /* LMK04806_R30 /PLL2_P = 2 PLL2_N = 25    VCO_2= 2500MHz                   */
  0x00000000    /* LMK04806_R31 uWIRE Not LOCK                                              */
};

/* lmk using on-board reference */
static int daq1430_lmk04806_init_intref[] =
{
  /*
   * CLKin_0   -> FMC_CLK2                           -> OFF
   * CLKin_1   -> RTM_CLK0                           -> OFF
   *
   * CLKOUT_0  -> -                                  -> OFF
   * CLKOUT_1  -> ADC_CLK_IN_67                      -> 250 MHz / LVDS
   * CLKOUT_2  -> FMC_CLK_M2C                        -> 250 MHz / LVDS
   * CLKOUT_3  -> ADC_CLK_IN_45                      -> 250 MHz / LVDS
   * CLKOUT_4  -> DAC_CLK_MIRROR                     -> 250 MHz / LVDS
   * CLKOUT_5  -> DAC_CLK                            -> 250 MHZ / LVPECL
   * CLKOUT_6  -> -                                  -> OFF
   * CLKOUT_7  -> ADC_CLK_IN_01                      -> 250 MHz / LVDS
   * CLKOUT_8  -> ADC_CLK_IN_23                      -> 250 MHz / LVDS
   * CLKOUT_9  -> -                                  -> OFF
   * CLKOUT_10 -> ADC_CLK_IN_89                      -> 250 MHz / LVDS
   * CLKOUT_11 -> -                                  -> OFF
   * OSCout_0  ->                                    -> OFF
   */
  0x00000140,   /* LMK04806_R00 Enable + ClkOUT0_DIV = 10  <(Test point R184))            */
  0x00000140,   /* LMK04806_R01 Enable  ClkOut_1 + ClkOUT0_DIV = 10  -> 2500/10 = 250 MHz */
  0x00000140,   /* LMK04806_R02 Enable  ClkOut_2 + ClkOUT0_DIV = 10                       */
  0x00000140,   /* LMK04806_R03 Enable  ClkOut_3 + ClkOUT0_DIV = 10                       */
  0x00000140,   /* LMK04806_R04 Enable  ClkOut_4 + ClkOUT0_DIV = 10                       */
  0x00000140,   /* LMK04806_R05 Enable  ClkOut_5 + ClkOUT0_DIV = 10  -> 2500/10 = 250 MHz */

  0x11100000,   /* LMK04806_R06 CLKOUT_3,  CLKOUT_2, CLKOUT_1 -> LVDS                     */
  0x10410000,   /* LMK04806_R07 CLKOUT_7,  CLKOUT_5, CLKOUT_4 -> LVDS / LVPECL            */
  0x01010000,   /* LMK04806_R08 CLKOUT_10, CLKOUT_8           -> LVDS                     */

  0x55555540,   /* LMK04806_R09 TI/NS write MUST                                          */
  0x0000400C,   /* LMK04806_R10 OscOUT_Type = 1 (LVDS)  Powerdown                         */
  0x34010000,   /* LMK04806_R11 Device MODE=0x6 + SYNC output                             */
/*  0x33F08000,*/   /* LMK04806_R11 Device MODE=0x6 + No SYNC output                          */
  0x13000040,   /* LMK04806_R12 LD pin programmable                                       */
  0x3B23A260,   /* LMK04806_R13 HOLDOVER pin uWIRE SDATOUT  Enable CLKin1                 */
  0x00000000,   /* LMK04806_R14 Bipolar Mode CLKin1 INPUT                                 */
  0x00000000,   /* LMK04806_R15 DAC unused                                                */
  0x01550400,   /* LMK04806_R16 OSC IN level                                              */
  0x00000000,   /* LMK04806_R17                                                           */
  0x00000000,   /* LMK04806_R18                                                           */
  0x00000000,   /* LMK04806_R19                                                           */
  0x00000000,   /* LMK04806_R20                                                           */
  0x00000000,   /* LMK04806_R21                                                           */
  0x00000000,   /* LMK04806_R22                                                           */
  0x00000000,   /* LMK04806_R23                                                           */
  0x00000000,   /* LMK04806_R24 PLL1 not used  / PPL2                                     */
  0x00000000,   /* LMK04806_R25 DAC config not used                                       */
  0x8fa00000,   /* LMK04806_R26 PLL2 used  / ICP = 3200 uA                                */
  0x00000000,   /* LMK04806_R27 PLL1 not used                                             */
  0x00200000,   /* LMK04806_R28 PLL2_R = 2 /PPL1 N divider = 00                           */
  0x01800320,   /* LMK04806_R29 OSCIN_FREQ /PLL2_NCAL = 25)                               */
  0x02000320,   /* LMK04806_R30 /PLL2_P = 2 PLL2_N = 25    VCO_2= 2500MHz                 */
  0x00000000    /* LMK04806_R31 uWIRE Not LOCK                                            */
};

/* dual pll mode */
static int daq1430_lmk04806_init_dualpll[] =
{
  /*
   * CLKin_0   -> FMC_CLK2                           -> OFF
   * CLKin_1   -> RTM_CLK0                           -> 10.00 MHz / 100.00 MHZ / 250 MHz / LVDS
   *
   * CLKOUT_0  -> -                                  -> OFF
   * CLKOUT_1  -> ADC_CLK_IN_67                      -> 250 MHz / LVDS
   * CLKOUT_2  -> FMC_CLK_M2C                        -> 250 MHz / LVDS
   * CLKOUT_3  -> ADC_CLK_IN_45                      -> 250 MHz / LVDS
   * CLKOUT_4  -> DAC_CLK_MIRROR                     -> 250 MHz / LVDS      <-- FBMUX
   * CLKOUT_5  -> DAC_CLK                            -> 250 MHZ / LVPECL
   * CLKOUT_6  -> -                                  -> OFF
   * CLKOUT_7  -> ADC_CLK_IN_01                      -> 250 MHz / LVDS
   * CLKOUT_8  -> ADC_CLK_IN_23                      -> 250 MHz / LVDS
   * CLKOUT_9  -> -                                  -> OFF
   * CLKOUT_10 -> ADC_CLK_IN_89                      -> 250 MHz / LVDS
   * CLKOUT_11 -> -                                  -> OFF
   * OSCout_0  ->                                    -> OFF
   */
  0x00000140,   /* LMK04806_R00 Enable + ClkOUT0_DIV = 10  <(Test point R184))              */
  0x00000140,   /* LMK04806_R01 Enable  ClkOut_1 + ClkOUT0_DIV = 10  -> 2500/10 = 250 MHz   */
  0x00000140,   /* LMK04806_R02 Enable  ClkOut_2 + ClkOUT0_DIV = 10                         */
  0x00000140,   /* LMK04806_R03 Enable  ClkOut_3 + ClkOUT0_DIV = 10                         */
  0x00000140,   /* LMK04806_R04 Enable  ClkOut_4 + ClkOUT0_DIV = 10                         */
  0x00000140,   /* LMK04806_R05 Enable  ClkOut_5 + ClkOUT0_DIV = 10  -> 2500/10 = 250 MHz   */

  0x11100000,   /* LMK04806_R06 CLKOUT_3,  CLKOUT_2, CLKOUT_1 -> LVDS                       */
  0x10410000,   /* LMK04806_R07 CLKOUT_7,  CLKOUT_5, CLKOUT_4 -> LVDS / LVPECL              */
  0x01010000,   /* LMK04806_R08 CLKOUT_10, CLKOUT_8           -> LVDS                       */

  0x55555540,   /* LMK04806_R09 TI/NS write MUST                                            */
  0x10004840,   /* LMK04806_R10 OscOUT_Type = 1 (LVDS)  Powerdown                           */
  0x14010000,   /* LMK04806_R11 Device MODE=0x6 + No SYNC output                            */
/*  0x13F08000,*/   /* LMK04806_R11 Device MODE=0x6 + No SYNC output                            */
  0x1B000040,   /* LMK04806_R12 LD pin programmable                                         */
  0x3B23A260,   /* LMK04806_R13 HOLDOVER pin uWIRE SDATOUT  Enable CLKin1                   */
  0x13000000,   /* LMK04806_R14 Bipolar Mode CLKin1 INPUT                                   */
  0x00000000,   /* LMK04806_R15 DAC unused                                                  */
  0x01550400,   /* LMK04806_R16 OSC IN level                                                */
  0x00000000,   /* LMK04806_R17                                                             */
  0x00000000,   /* LMK04806_R18                                                             */
  0x00000000,   /* LMK04806_R19                                                             */
  0x00000000,   /* LMK04806_R20                                                             */
  0x00000000,   /* LMK04806_R21                                                             */
  0x00000000,   /* LMK04806_R22                                                             */
  0x00000000,   /* LMK04806_R23                                                             */
  0x00000000,   /* LMK04806_R24 PLL1 not used  / PPL2                                       */
  0x0009C400,   /* LMK04806_R25 DAC config not used                                         */
  0xcfa00800,   /* LMK04806_R26 PLL2 used  / ICP = 3200 uA                                  */
  /* 10 MHz */
/*  0x10000040, */  /* LMK04806_R27 PLL1 not used                                              */
/*  0x00200640, */  /* LMK04806_R28 PLL2_R = 2 /PPL1 N divider = 00                            */
  /* 100 MHz */
/*  0x10000280, */  /* LMK04806_R27 PLL1 not used                                              */
/*  0x00200640, */  /* LMK04806_R28 PLL2_R = 2 /PPL1 N divider = 00                            */
  /* 250 MHz */
  0x10000280,
  0x00200280,
  0x01000320,   /* LMK04806_R29 OSCIN_FREQ /PLL2_NCAL = 25)                                 */
  0x02000320,   /* LMK04806_R30 /PLL2_P = 2 PLL2_N = 25    VCO_2= 2500MHz                   */
  0x00000000    /* LMK04806_R31 uWIRE Not LOCK                                              */
};

static lmk_t daq1430_lmk;

/* ------------------------------------------------------------------------------------------------------------------ */

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * Function name : daq1430_lmk_func
 * Prototype     : int
 * Parameters    : cmd      LMK_REG_READ or LMK_REG_WRITE
 *                 reg      LMK register address
 *                 data     pointer to data word
 *                 priv     private value (here FMC number)
 * Return        : int
 *----------------------------------------------------------------------------
 * Description   : do an LMK read or write using ADC serial interface
 *
 */

static int daq1430_lmk_func(int fd, int cmd, int reg, int *data, int priv)
{
  int ret = -1;

  if (cmd == LMK_REG_READ)
  {
    ret = adc_spi_read(fd, priv, DAQ1430_SPI_LMK, reg, data);
  }
  if (cmd == LMK_REG_WRITE)
  {
    if (data != NULL)
    {
      ret = adc_spi_write(fd, priv, DAQ1430_SPI_LMK, reg, (*data));
    }
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
 */

int daq1430_set_verbose(int mode)
{
  xilinx_set_verbose(mode);
  dac38j84_set_verbose(mode);
  daq1430_verbose_mode = mode;
  return(daq1430_verbose_mode);
}

/* ------------------------------------------------------------------------------------------------------------------ */

/*!+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * \brief   Perform a reset of the DAQ1430.
 *
 * \param   fmc         fmc slot number (1 or 2)
 *
 * \return  >=0 on success, <0 on error
 */

int daq1430_reset(int fd, int fmc)
{
  int tmp, ret;

  if (fmc != 1)
  {
    daq1430_error("daq_1430: can only be installed on FMC slot 1 !\n");
    return(-1);
  }

  ret = fmc_init(fd);
  if (ret < 0)
  {
    return(ret);
  }

  /* enabling DAQ_1430 if signature is ok */
  tmp = DAQ1430_SIGN_ID;

  ret = fmc_identify(fd, fmc, &tmp, NULL, NULL);
  if (ret < 0)
  {
    return(ret);
  }

  /* verify signature */
  if ((tmp & FMC_SIGN_MASK) != DAQ1430_SIGN_ID)
  {
    daq1430_error("daq_1430: Invalid signature, expected 0x%08X but found 0x%08X !\n", DAQ1430_SIGN_ID, tmp);
    return(-1);
  }

  /* configure interface SPI to acces ADS42LB69, LMK & DAC38j84 */
  ads42lb69_configure(DAQ1430_CHAN_NUM, daq1430_spi_ads);

  lmk_configure(&daq1430_lmk, &daq1430_lmk_func);

  dac38j84_configure(DAQ1430_SPI_DAC38J84, DAQ1430_CSR_DAC_CTL);

  /* Resetting MMCM, ADCs and DAC38J84 */
  ret = fmc_csr_write(fd, fmc, DAQ1430_CSR_CTL, (DAQ1430_CTL_MMCM_RESET    |
                                             DAQ1430_CTL_ADC_89_PWRD   |
                                             DAQ1430_CTL_ADC_4567_PWRD |
                                             DAQ1430_CTL_ADC_23_PWRD   |
                                             DAQ1430_CTL_ADC_01_PWRD   |
                                             DAQ1430_CTL_ADC_RESET));
  if (ret < 0)
  {
    return(ret);
  }

  usleep(50000);

  /* Remove all RESETs with exception of MMCM */
  ret = fmc_csr_write(fd, fmc, DAQ1430_CSR_CTL, DAQ1430_CTL_MMCM_RESET);
  if (ret < 0)
  {
    return(ret);
  }

  usleep(50000);

  /* Remove all RESETs */
  ret = fmc_csr_write(fd, fmc, DAQ1430_CSR_CTL, 0);
  if (ret < 0)
  {
    return(ret);
  }

  /* initialize SPI interface of DAC38j84 */
  ret = dac38j84_spi_init(fd, fmc);
  if (ret  < 0)
  {
    daq1430_error("daq_1430: DAC38j84 initialization error ! (%d)\n", ret);
    return(ret);
  }

  return (1);
}

/* ------------------------------------------------------------------------------------------------------------------ */

/*!+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * \brief   Initialize DAC38j84 on DAQ_1430.
 *
 * \param   fmc         fmc slot number (1 or 2)
 * \param   freq_refclk DAC reference clock pnput frequency (DACCLK_p/_n pin)
 *                      in Mhz
 * \param   freq_dac    DAC output frequency in MHz
 * \param   linerate    SerDes Line Rate in Gbit/s (0.0 = use default)
 * \param   interp      Interpolation (-1 = default)
 * \param   k           number of frames per mult-frame (k = 1 to 32)
 * \param   rdb
 * \param   scr         scrambling enable (0 = disable or 1 = enable)
 * \param   sysref_mode SYSREF mode (0 = SYSREF on SYNC, 1 = periodic)
 *
 * \return  >=0 on success, <0 on error
 */

int daq1430_dac_init (int     fd,
                      int     fmc,
                      double  freq_refclk,
                      double  freq_dac,
                      double  linerate,
                      int     interp,
                      int     k,
                      int     rdb,
                      int     scr,
                      int     sysref_mode)
{
  int rc, l, m, f, hd, s, n_min;
  double lmfc, sysref;

  if (fmc != 1)
  {
    return(-1);
  }

  /* Use default interpolation depending on selected DAC output frequency */
  if (interp == -1)
  {
    if (freq_dac > 2500.0)
    {
      interp = 4;
    }
    if (freq_dac > 2460.0)
    {
      interp = 2;
    }
    if (freq_dac > 1230.0)
    {
      interp = 2;
    }
    else
    {
      interp = 1;
    }
  }

  /* Calculate linerate (should 10 * freq_dac / interp) */
  if (linerate == 0.0)
  {
    linerate = (10.0 * freq_dac) / (1000.0 * (double)interp);
  }

  /* static parameters */
  s = 1;  /* number of converters samples per frame     */
  l = 8;  /* number of lanes                            */
  m = 4;  /* number of converter                        */
  f = 1;  /* number of octet(s) per frame               */
  hd = 1; /* high density i.e. frame shared among lanes */

  /* Use default k */
  if (k == -1)
  {
    k = 4;
  }

  /* Use default RDB */
  if (rdb == -1)
  {
    rdb = k;
  }

  /* Use default SCR */
  if (scr == -1)
  {
    scr = 1;
  }

  /* Use default SYSREF */
  if (sysref_mode == -1)
  {
    sysref_mode = 1;
  }

  rc = dac38j84_jesd_qpll_configure(fd, fmc, freq_refclk, linerate);

  /* non-default values -> reconfigure QPLLs */
/*  if (freq_refclk != 250.0 || linerate != 10.0)
  {
    rc = dac38j84_jesd_qpll_configure(fmc, freq_refclk, linerate);
  }
  else
  {
    rc = dac38j84_jesd_qpll_reset(fmc);
  }
*/
  if (rc == 0)
  {
    daq1430_error("daq_1430: JESD QPLL not locked !\n");
  }

  /* error or QPLL not locked */
  if (rc <= 0)
  {
    return(rc);
  }

  daq1430_info("daq_1430: JESD QPLL locked successfully.\n");

  /* Disable DAC outputs */
  rc = dac38j84_jesd_dac_txenable(fd, fmc, 0, 0);
  if (rc < 0)
  {
    return(rc);
  }

  /* Use SYNC_B pin for SYNC */
  rc = dac38j84_jesd_set_sync(fd, fmc, DAC38J84_JESD_DAC_SYNCB, DAC38J84_JESD_SYNC_LOW);
  if (rc < 0)
  {
    return(rc);
  }

  /* Set Interpolation */
  rc = dac38j84_set_interpolation(fd, fmc, interp);
  if (rc  < 0)
  {
    return(rc);
  }

  /* Configure PLL */
  rc = dac38j84_pll_configure(fd, fmc, freq_refclk, freq_dac);

  if (rc <= 0)
  {
    if (rc == 0)
    {
      daq1430_error("daq_1430: PLL not locked !\n");
    }
    return(rc);
  }

  daq1430_info("daq_1430: PLL locked successfully.\n");

  /* Set DAC lane polarity */
  rc = dac38j84_set_polarity(fd, fmc, 0xFD);
  if (rc < 0)
  {
    return(rc);
  }

  /* PLL */
  rc = dac38j84_serdes_pll_configure(fd, fmc, freq_dac, linerate);
  if (rc <= 0)
  {
    if (rc == 0)
    {
      daq1430_error("daq_1430: SerDes PLL not locked !\n");
    }
    return(rc);
  }

  daq1430_info("daq_1430: SerDes PLL locked successfully.\n");

  /* JESDCLK = DACCLK / x */
  rc = dac38j84_set_jesd_clock(fd, fmc, interp, l, m);
  if (rc < 0)
  {
    return(rc);
  }

  rc = dac38j84_jesd_configure(fd, fmc, rdb, f, k, m, l, s, scr, hd);
  if (rc < 0)
  {
    return(rc);
  }

  /* JESD */

  /* caulcaute LMFC = linerate / 10 * F * K */
  lmfc = (linerate * 1000.0) / (10.0 * (double)f * (double)k);

  n_min = (int)(freq_dac / freq_refclk);

  sysref = lmfc / (double)(4*n_min);
  daq1430_info("daq_1430: lmfc = %g, sysref = %g, n = %d\n", lmfc, sysref, 4*n_min);

  /* n_min = freq_dac / freq_refclk  */

  /* Enable SYSREF generation */
  rc = dac38j84_jesd_set_sysref_mode(fd, fmc, sysref_mode, 4*n_min);
  if (rc < 0)
  {
    return(rc);
  }

  rc = dac38j84_jesd_start(fd, fmc);
  if (rc < 0)
  {
    return(rc);
  }

  /* clear all status flags */
  rc = dac38j84_clear_status(fd, fmc);
  if (rc < 0)
  {
    return(rc);
  }

  /* check status */
  rc = dac38j84_check_lane_status(fd, fmc, 0xFF, 10, 0xFF0F);
  if (rc < 0)
  {
    return(rc);
  }

  /* no alarm -> enable DAC outputs */
  if (rc == 0)
  {
    daq1430_info("daq_1430: all lanes successfully initialized. Enabling DAC outputs.\n");
    rc = dac38j84_jesd_dac_txenable(fd, fmc, 1, 0);
  }
  else
  {
    daq1430_error("daq_1430: errors during initialization.\n");
  }

  return (rc);
}

/* ------------------------------------------------------------------------------------------------------------------ */

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * Function name : daq1430_lmk_init
 * Prototype     : int
 * Parameters    : fmc      FMC identifier (1)
 *                 mode
 *                      DAQ1430_LMK_MODE_CLKDIST  -> Clock Distribution Mode
 *                      DAQ1430_LMK_MODE_INTREF   -> on-board single pll mode
 *                      DAQ1430_LMK_MODE_DUALPLL  -> dual pll mode
 *                 ref
 *                      DAQ1430_LMK_REF_FMC       -> using FMC_CLK2 as reference
 *                      DAQ1430_LMK_REF_RTM       -> using RTM_CLK0 as reference
 *                 freq
 *
 * Return        : current value of IDELAY register
 *----------------------------------------------------------------------------
 * Description   : adjust the data interface calibration delay to delay for
 *                 channel chan.
 *                 If chan equal -1, ajustment is applied to all channels.
 *                 If delay is set to -1, default delay value is restored.
 *
 */

int daq1430_lmk_init(int fd, int fmc, int mode, int ref, int freq)
{
  int ret, need_vcxo = 0;
  int lmk_reg[32];

  if (fmc != 1)
  {
    daq1430_error("daq_1430: can only be installed on FMC slot 1 !\n");
    return(-1);
  }

  switch(mode)
  {
    case DAQ1430_LMK_MODE_CLKDIST:
      memcpy(lmk_reg, daq1430_lmk04806_init_clkdist, sizeof(lmk_reg));
      need_vcxo = 0;
      break;

    case DAQ1430_LMK_MODE_INTREF:
      memcpy(lmk_reg, daq1430_lmk04806_init_intref, sizeof(lmk_reg));
      need_vcxo = 1;
      break;

    case DAQ1430_LMK_MODE_DUALPLL:
      memcpy(lmk_reg, daq1430_lmk04806_init_dualpll, sizeof(lmk_reg));
      need_vcxo = 1;
      break;

    default:
      daq1430_error("daq1430_lmk_init: invalid mode %d\n", mode);
      return(-1);
  }

  if (mode != DAQ1430_LMK_MODE_INTREF)
  {
    switch(ref)
    {
      case DAQ1430_LMK_REF_FMC:
        lmk_reg[13] = 0x3B23A060;
        break;

      /* default */
      case DAQ1430_LMK_REF_RTM:
      default:
        break;
    }
  }

  if (mode == DAQ1430_LMK_MODE_DUALPLL)
  {
    switch(freq)
    {
      case DAQ1430_LMK_FREQ_10M:
        lmk_reg[27] = 0x10000040;
        lmk_reg[28] = 0x00200640;
        break;

      case DAQ1430_LMK_FREQ_100M:
        lmk_reg[27] = 0x10000280;
        lmk_reg[28] = 0x00200640;
        break;

      /* default frequency */
      case DAQ1430_LMK_FREQ_250M:
        break;

      default:
        daq1430_error("daq1430_lmk_init: invalid frequency %d\n", freq);
        return(-1);
    }
  }

  ret = lmk04806_init(&daq1430_lmk, fd, lmk_reg, fmc, (daq1430_verbose_mode==0));
  if (ret < 0) return (ret);

  /* --------------------------------------------*/
  /* Enable On-board 100 MHz clock from +OSC575  */
  /* --------------------------------------------*/
  ret = fmc_csr_write(fd, fmc, ADC_CSR_LED, (need_vcxo ? 0x80000003 : 0x3));   /* FP Led flashing + CCHD575-100MHz  Power-on */
  if (ret < 0) return (ret);

  usleep(20000);

  /* LMK04806_R30 PLL2 P/N Recallibration */
  ret = lmk_write(&daq1430_lmk, fd, 0x1E, lmk_reg[0x1E], fmc);
  if (ret < 0) return (ret);

  /* LMK04806_R12 LD pin programmable  */
  ret = lmk_write(&daq1430_lmk, fd, 0x0C, lmk_reg[0x0C] | 0x800000, fmc);
  if (ret < 0) return (ret);

  usleep( 20000);

  /* --------------------------------------------*/
  /* Check if manual SYNC to be performed        */
  /* --------------------------------------------*/

  if (!(lmk_reg[0x0B]&0x04000000))
  {
    printf("Perform manual synchronisation of clock outputs\n");

    /* force manual SYNC */
    ret = lmk_write(&daq1430_lmk, fd, 0x1e, lmk_reg[0x0B] | 0x10000, fmc);
    if (ret < 0) return (ret);

    usleep(20000);

    ret = lmk_write(&daq1430_lmk, fd, 0x1e, lmk_reg[0x0B], fmc);
    if (ret < 0) return (ret);
  }

  return(1);
}

/* ------------------------------------------------------------------------------------------------------------------ */

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * Function name : daq1430_lmk_set_adc_divider
 * Prototype     : int
 * Parameters    : fmc      FMC identifier (1)
 *                 divider
 *                          divider value of all ADC clock inputs
 * Return        :
 *----------------------------------------------------------------------------
 * Description   :
 *
 */

int daq1430_lmk_set_adc_divider(int fd, int fmc, int divider)
{
  int ret, tmp;
  if (fmc != 1)
  {
    daq1430_error("daq_1430: can only be installed on FMC slot 1 !\n");
    return(-1);
  }
  if (divider < 1 && divider > 1045)
  {
    daq1430_error("daq_1430: invalid LMK divider value !\n");
    return(-1);
  }
  /*
   * R0: CLKOUT_0  / CLKOUT_1  ->              - / ADC_CLK_IN_67
   * R1: CLKOUT_2  / CLKOUT_3  ->    FMC_CLK_M2C / ADC_CLK_IN_45
   * R2: CLKOUT_4  / CLKOUT_5  -> DAC_CLK_MIRROR / DAC_CLK
   * R3: CLKOUT_6  / CLKOUT_7  ->              - / ADC_CLK_IN_01
   * R4: CLKOUT_8  / CLKOUT_9  ->  ADC_CLK_IN_23 / -
   * R5: CLKOUT_10 / CLKOUT_11 ->  ADC_CLK_IN_89 / -
   */
  tmp = ((divider&0x7FF)<<5);
  ret = lmk_write(&daq1430_lmk, fd, 0x0, tmp, fmc);
  if (ret < 0) return(ret);

  ret = lmk_write(&daq1430_lmk, fd, 0x1, tmp, fmc);
  if (ret < 0) return(ret);

  ret = lmk_write(&daq1430_lmk, fd, 0x3, tmp, fmc);
  if (ret < 0) return(ret);

  ret = lmk_write(&daq1430_lmk, fd, 0x4, tmp, fmc);
  if (ret < 0) return(ret);

  ret = lmk_write(&daq1430_lmk, fd, 0x5, tmp, fmc);
  if (ret < 0) return(ret);

  return(1);
}

/* ------------------------------------------------------------------------------------------------------------------ */

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * Function name : daq1430_lmk_dump
 * Prototype     : void
 * Parameters    : fmc  FMC identifier (1 or 2)
 * Return        : void
 *----------------------------------------------------------------------------
 * Description   : displays the content of the 32 registers
 *
 */

int daq1430_lmk_dump(int fd, int fmc)
{
  if (fmc != 1) {
    daq1430_error("daq_1430: can only be installed on FMC slot 1 !\n");
    return(-1);
  }
  return lmk04806_dump(&daq1430_lmk, fd, fmc);
}

/* ------------------------------------------------------------------------------------------------------------------ */

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * Function name : daq1430_lmk_get_status
 * Prototype     : void
 * Parameters    : fmc  FMC identifier (1 or 2)
 * Return        : void
 *----------------------------------------------------------------------------
 * Description   : displays the content of the 32 registers
 *
 */

int daq1430_lmk_get_status(int fd, int fmc, int *data)
{
  int rc;

  if (fmc != 1 || data == NULL)
    return(-1);

  rc = fmc_csr_read(fd, fmc, ADC_CSR_SERIAL, data);

  return(rc);
}


/* ------------------------------------------------------------------------------------------------------------------ */

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * Function name : daq1430_ads42lb69_init
 * Prototype     : void
 * Parameters    : fmc  FMC identifier (1)
 * Return        : void
 *----------------------------------------------------------------------------
 * Description   : initializes the four ads42lb69 dual analog-to-digital
 *                 converters present on the DAQ_1430 FMC
 *
 */

int daq1430_ads42lb69_init(int fd, int fmc, int chan_set, int fmt)
{
  int tmp, ret, timeout = 1000;

  if (fmc != 1) {
    daq1430_error("daq_1430: can only be installed on FMC_1 !\n");
    return(-1);
  }

  /* Configure ADS42LB69 */
  ret = ads42lb69_init(fd, fmc, chan_set, daq1430_ads42lb69_init_regs, (daq1430_verbose_mode==0), fmt);

  /* RESET MMCM/PLL */
  tmp = DAQ1430_CTL_MMCM_RESET;
  ret = fmc_csr_write(fd, fmc, ADC_CSR_CTL, tmp);
  if (ret < 0)
  {
    return(ret);
  }
  /* Release reset of MMCM/PLL */
  tmp = 0;
  ret = fmc_csr_write(fd, fmc, ADC_CSR_CTL, tmp);
  if (ret < 0)
  {
    return(ret);
  }

  while (timeout > 0)
  {
    ret = fmc_csr_read(fd, fmc, ADC_CSR_CTL, &tmp);
    if (ret < 0)
    {
      return(ret);
    }
    if (tmp & (DAQ1430_CTL_MMCM_LOCKED | DAQ1430_CTL_PLLE3_LOCKED))
      break;

    usleep(100);
    timeout --;
  }

  if (timeout == 0)
  {
    daq1430_error("daq_1430: unable to lock MMCM or PLL !\n");
    return(-2);
  }

  /* Do a RESET of all IDELAY/ISERDES */
  ret = ads42lb69_calib_reset_idelay(fd, fmc);
  if (ret < 0)
  {
    return (ret);
  }

  /* bit #8 in ADC_CSR_CTL RESET the DAC */
  dac38j84_configure(DAQ1430_SPI_DAC38J84, DAQ1430_CSR_DAC_CTL);

  return(ret);
}

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * Function name : daq1430_ads42lb69_dump
 * Prototype     : void
 * Parameters    : fmc    FMC identifier (1)
 * Return        : void
 *----------------------------------------------------------------------------
 * Description   : display the content of the ads42lb69 registers.
 *
 */

int daq1430_ads42lb69_dump(int fd, int fmc, int chan_set)
{
  int ret;

  if (fmc != 1) {
    daq1430_error("daq_1430: can only be installed on FMC_1 !\n");
  }

  ret = ads42lb69_dump(fd, fmc, chan_set);

  return(ret);
}

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * Function name : daq1430_ads42lb69_set_mode
 * Prototype     : int
 * Parameters    : fmc    FMC identifier (always 1)
 *                 chan   channel number (0 -> DAQ1430_CHAN_NUM - 1)
 *                 mode   operation mode
 * Return        : mode   current value of ADC42LB69 register 0xF
 *----------------------------------------------------------------------------
 * Description   : allow to set the operation mode of for ADC channel referred
 * by chan. The mode (loaded in register 0xF) shall be:
 * - DAQ1430_ADS_MODE_NORM    → normal operation
 * - DAQ1430_ADS_MODE_ZERO    → output all 0s
 * - DAQ1430_ADS_MODE_ONE     → output all 1s
 * - DAQ1430_ADS_MODE_TOGGLE  → output toggles 0s and 1s
 * - DAQ1430_ADS_MODE_RAMP    → output digital ramp from 0 to 65535
 * - DAQ1430_ADS_MODE_TEST1   → output test pattern 1
 * - DAQ1430_ADS_MODE_TEST2   → output alternate test pattern 1 & 2
 * - DAQ1430_ADS_MODE_SINE    → output sine wave
 *
 */

int daq1430_ads42lb69_set_mode(int fd, int fmc, int chan, int mode)
{
  if (fmc !=1 || chan < 0 || chan >= DAQ1430_CHAN_NUM) return(-1);

  return ads42lb69_set_mode(fd, fmc, chan, mode);
}

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * Function name : daq1430_ads42lb69_set_pattern
 * Prototype     : int
 * Parameters    : fmc      FMC identifier (1)
 *                 chan     channel number (0 -> DAQ1430_CHAN_NUM - 1)
 *                 pattern  32 bit pattern loaded in registers 0x10 → 0x13
 * Return        : pattern  current value of ADC42LB67 register 0x10 → 0x13
 *----------------------------------------------------------------------------
 * Description   : allow to set for any channel the data pattern to be used in
 *                 test mode
 *
 */

int daq1430_ads42lb69_set_pattern(int fd, int fmc, int chan, int pattern)
{
  if (fmc != 1 || chan < 0 || chan >= DAQ1430_CHAN_NUM) return(-1);

  return ads42lb69_set_pattern(fd, fmc, chan, pattern);
}

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * Function name : daq1430_calib_set_idelay
 * Prototype     : int
 * Parameters    : fmc      FMC identifier (1)
 *                 chan     channel number (0 -> DAQ1430_CHAN_NUM - 1)
 *                            if -1 adjust for all channels
 *                 idelay   calibration delay ( 0 -> 0x1ff)
 *                            if -1 reset IDELAY
 * Return        : current value of IDELAY register
 *----------------------------------------------------------------------------
 * Description   : adjust the data interface calibration delay to delay for
 *                 channel chan.
 *                 If chan equal -1, ajustment is applied to all channels.
 *                 If delay is set to -1, default delay value is restored.
 *
 */

int daq1430_calib_set_idelay(int fd, int fmc, int chan, int idelay)
{
  /* sanity check */
  if(fmc != 1 || chan < -1 || chan >= DAQ1430_CHAN_NUM) return( -1);

  return ads42lb69_calib_set_idelay(fd, fmc, chan, -1, idelay);
}

/* ------------------------------------------------------------------------------------------------------------------ */

/*!
 * \brief   return the current value of DAQ1430_CSR_IDELAY register for
 *          channel chan
 *
 * \param   fmc       fmc slot number (1)
 * \param   chan      channel number (0 -> DAQ1430_CHAN_NUM - 1)
 * \param   idelay    pointer to idelay value
 *
 * \return  current value of IDELAY register
 */

int daq1430_calib_get_idelay(int fd, int fmc, int chan, int *idelay)
{
  /* sanity check */
  if(fmc != 1 || chan < -1 || chan >= DAQ1430_CHAN_NUM)
  {
    return(-1);
  }

  return ads42lb69_calib_get_idelay(fd, fmc, chan, -1, idelay);
}

/* ------------------------------------------------------------------------------------------------------------------ */

/*!
 * \brief   Calibrate ADCs
 *
 * \param   fmc       FMC identifier (1)
 * \param   chan      ADC channel (-1 = all channels)
 * \param   step      step (1 to 32)
 *
 * \return  >=0 on success, <0 on error
 */

int daq1430_calibrate(int fd, int fmc, int chan, int step)
{
  int rc;

  rc = ads42lb69_calibrate(fd, fmc, chan, step, 1);

  if (rc > 0)
  {
    daq1430_error("daq_1430: ADCs calibration failure !\n");
  }

  return(rc);
}

/* ------------------------------------------------------------------------------------------------------------------ */

/*!
 * \brief   Display current IDELAY values
 *
 * \param   fmc      FMC identifier (1)
 *
 * \return  >=0 on success, <0 on error
 */

int daq1430_calib_dump_idelay(int fd, int fmc)
{
  int ret=0;

  char *__cell_str[] = {
    "QA[0]", "QA[1]", "QA[2]", "QA[3]", "FRAM_A", "OVR_A",
    "QB[0]", "QB[1]", "QB[2]", "QB[3]", "FRAM_B", "OVR_B"
  };

  if (fmc != 1)
  {
    return(-1);
  }

  ret = ads42lb69_calib_dump_idelay(fd, fmc, -1, __cell_str);

  return (ret);
}

/* ------------------------------------------------------------------------------------------------------------------ */

/*!
 * \brief   Display ADC value using direct register
 *
 * \param   fmc       FMC identifier (1)
 * \param   chan      ADC channel (-1 = all channels)
 *
 * \return  >=0 on success, <0 on error
 */

int daq1430_direct_dump(int fd, int fmc, int chan)
{
  if (fmc != 1)
  {
    return(-1);
  }
  return ads42lb69_direct_dump(fd, fmc, chan);
}

/* ------------------------------------------------------------------------------------------------------------------ */

/*!
 * \brief   Measure a signal present on ATEST pin
 *
 * \param   fmc         FMC identifier (1)
 * \param   atest_sel   internal signals selections on ATEST pin
 * \param   measured    integer pointer on ATEST pin measured value in 100 uV
 * \param   adc_code    integer pointer to ADC code
 * \param   descr       string pointer to description of the ATEST signals
 *
 * \return  >=0 on success, <0 on error
 */

int daq1430_read_atest(int fd, int fmc, int atest_sel, int *measured, int *adc_code, const char ** descr)
{
  int rc, ret = 1, data, adc_cmd;

  if (descr != NULL)
  {
    (*descr) = dac38j84_atest_str[atest_sel&0x3f];
  }

  ret = dac38j84_set_atest(fd, fmc, atest_sel);
  if (ret < 0)
  {
    return(ret);
  }

  /* only channel #0 is used */
  adc_cmd = LTC2489_CMD_CH0_SGL;

  rc = ltc2489_read(fd, DAQ1430_I2C_BUS, DAQ1430_I2C_ADC, adc_cmd, &data);

  if (adc_code != NULL)
  {
    (*adc_code) = data;
  }

  if (measured != NULL)
  {
    if (rc < 0)
    {
      (*measured) = 0;
    }
    else
    {
      (*measured) = ((data * 18150) / 65536);
    }
  }

  ret = dac38j84_set_atest(fd, fmc, 0);
  if (ret < 0)
  {
    return(ret);
  }

  return(rc);
}

/* ------------------------------------------------------------------------------------------------------------------ */

int daq1430_gpio_trig(int fmc)
{
  return -1;
}

/* ------------------------------------------------------------------------------------------------------------------ */
