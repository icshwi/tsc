/*=========================< begin file & file header >=======================
 *  References
 *
 *    filename : dac38j84lib.h
 *    author   : CG, JFG, XP
 *    company  : IOxOS
 *    creation : november 6,2020
 *
 *----------------------------------------------------------------------------
 *  Description
 *
 *    That library contains a set of function to access the DAC38j84 DAC
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

#ifndef _H_DAC38J84LIB
#define _H_DAC38J84LIB

/* ------------------------------------------------------------------------------------------------------------------ */
/*  Definition                                                                                                        */
/* ------------------------------------------------------------------------------------------------------------------ */

/* ------------------------------------------------------------------------------------------------------------------ */
/* JESD Control & Status Register Bits Definition                                                                     */

#define DAC38J84_JESD_GT_TX_RESET_DONE    (1<<16)
#define DAC38J84_JESD_GT228_PLL_LOCK      (1<<18)
#define DAC38J84_JESD_GT226_PLL_LOCK      (1<<19)
#define DAC38J84_JESD_GT228_REFCLK_LOST   (1<<20)
#define DAC38J84_JESD_GT226_REFCLK_LOST   (1<<21)
#define DAC38J84_JESD_DAC_ALARM           (1<<24)

/* JESD Configuration Register #4 */
#define DAC38J84_JESD_GT_RESET            (1<<0)
#define DAC38J84_JESD_GT_ENABLE           (1<<1)
#define DAC38J84_JESD_GT_REFCLK_SEL       (1<<2)
#define DAC38J84_JESD_SYNC_POL            (1<<3)
#define DAC38J84_JESD_SYNC_SEL(s)         ((s&3)<<4)
#define DAC38J84_JESD_SYSREF_ENABLE       (1<<6)
#define DAC38J84_JESD_SYSREF_MODE         (1<<7)
#define DAC38J84_JESD_SYSREF_VAL(v)       ((v&0xff)<<8)
#define DAC38J84_JESD_DAC_SLEEP           (1<<16)
#define DAC38J84_JESD_DAC_TXENA           (1<<17)

/* ------------------------------------------------------------------------------------------------------------------ */

#define DAC38J84_JESD_DAC_SYNCB           0 /* Use SYNC_B pin     */
#define DAC38J84_JESD_DAC_SYNC_N_AB       1 /* Use SYNC_N_AB pin  */
#define DAC38J84_JESD_DAC_SYNC_N_CD       2 /* Use SYNC_N_CD pin  */

#define DAC38J84_JESD_SYNC_LOW            0 /* Active-low         */
#define DAC38J84_JESD_SYNC_HIGH           1 /* Active-high        */

/* ------------------------------------------------------------------------------------------------------------------ */

#define DAC38J84_DRP_GO                   (1<<31)     /* Start a transfer on DRP interface    */
#define DAC38J84_DRP_RnW                  (1<<30)     /* 1= Read transfer, 0= Write transfer  */
#define DAC38J84_DRP_BUSY                 (1<<29)     /* DRP interface is busy                */
#define DAC38J84_DRP_TIMEOUT              (1<<28)     /* DRP interface timeout                */

#define DAC38J84_DRP_SEL_GT_226_LANE_0    0
#define DAC38J84_DRP_SEL_GT_226_LANE_1    1
#define DAC38J84_DRP_SEL_GT_226_LANE_2    2
#define DAC38J84_DRP_SEL_GT_226_LANE_3    3
                     
#define DAC38J84_DRP_SEL_GT_228_LANE_0    4
#define DAC38J84_DRP_SEL_GT_228_LANE_1    5
#define DAC38J84_DRP_SEL_GT_228_LANE_2    6
#define DAC38J84_DRP_SEL_GT_228_LANE_3    7
                     
#define DAC38J84_DRP_SEL_GT_226_COMMON    8
#define DAC38J84_DRP_SEL_GT_228_COMMON    9

/* ------------------------------------------------------------------------------------------------------------------ */

#define DAC38J84_DAC_PLL                  (1<<0)
#define DAC38J84_SERDES_BLK0_PLL          (1<<2)
#define DAC38J84_SERDES_BLK1_PLL          (1<<3)

/* ------------------------------------------------------------------------------------------------------------------ */

#define DAC38J84_SYSREF_DISABLED          0 /* No SYSREF generated            */
#define DAC38J84_SYSREF_RESYNC            1 /* SYSREF generated on (RE-)SYNC  */
#define DAC38J84_SYSREF_PERIODIC          2 /* Periodic SYSREF generated      */

/* ------------------------------------------------------------------------------------------------------------------ */

extern const char   *dac38j84_atest_str[];

/* ------------------------------------------------------------------------------------------------------------------ */
/*  Function Prototypes                                                                                               */
/* ------------------------------------------------------------------------------------------------------------------ */

extern void dac38j84_configure            (int spi_cmd, int jesd204b_base);
extern int  dac38j84_spi_write            (int fd, int fmc, int reg, int data);
extern int  dac38j84_spi_read             (int fd, int fmc, int reg, int *data);
extern int  dac38j84_spi_init             (int fd, int fmc);
extern int  dac38j84_spi_dump             (int fd, int fmc);

extern  int dac38j84_drp_write            (int fd, int fmc, int sel, int reg, int data);
extern  int dac38j84_drp_read             (int fd, int fmc, int sel, int reg, int *data);

extern int  dac38j84_jesd_write           (int fd, int fmc, int reg, int data);
extern int  dac38j84_jesd_read            (int fd, int fmc, int reg, int *data);
extern int  dac38j84_jesd_dump            (int fd, int fmc);
extern int  dac38j84_jesd_qpll_reset      (int fd, int fmc);
extern int  dac38j84_jesd_qpll_configure  (int fd, int fmc, double freq_refclk, double linerate);

extern int  dac38j84_jesd_set_sync        (int fd, int fmc, int sync_sel, int sync_pol);
extern int  dac38j84_jesd_set_sysref_mode (int fd, int fmc, int mode, int val);
extern int  dac38j84_jesd_dac_txenable    (int fd, int fmc, int dac_txena, int dac_sleep);
extern int  dac38j84_jesd_configure       (int fd, int fmc, int rdb, int f, int k, int m, int l, int s, int scr, int hd);
extern int  dac38j84_jesd_start           (int fd, int fmc);

extern int  dac38j84_set_verbose          (int mode);
extern int  dac38j84_set_atest            (int fd, int fmc, int atest_sel);
extern int  dac38j84_set_interpolation    (int fd, int fmc, int interp);
extern int  dac38j84_set_polarity         (int fd, int fmc, int polarity);
extern int  dac38j84_set_jesd_clock       (int fd, int fmc, int interp, int l, int m);
extern int  dac38j84_pll_calc_param       (double freq_in, double freq_out, int *config49, int *config50, int *config51);
extern int  dac38j84_pll_configure        (int fd, int fmc, double freq_in, double freq_out);
extern int  dac38j84_serdes_pll_calc_param(double freq_in, double linerate, int *config59, int *config60, int *config62);
extern int  dac38j84_serdes_pll_configure (int fd, int fmc, double freq_in, double linerate);
extern int  dac38j84_check_status         (int fd, int fmc, int timeout, int alarm_mask);
extern int  dac38j84_check_lane_status    (int fd, int fmc, int lane_mask, int timeout, int alarm_mask);
extern int  dac38j84_read_status          (int fd, int fmc, int *status_main, int *status_lane);
extern int  dac38j84_clear_status         (int fd, int fmc);

#endif /*  _H_DAC38J84LIB */

