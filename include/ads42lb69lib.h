/*=========================< begin file & file header >=======================
 *  References
 *
 *    filename : daq1430lib.h
 *    author   : CG, JFG, XP
 *    company  : IOxOS
 *    creation : september 23,2020
 *
 *----------------------------------------------------------------------------
 *  Description
 *
 *    That library contains a set of function to access the DAQ1430 FMC
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

#ifndef _H_ADS42LB69LIB
#define _H_ADS42LB69LIB

/* ------------------------------------------------------------------------------------------------------------------ */
/*  Register Definition                                                                                               */
/* ------------------------------------------------------------------------------------------------------------------ */

#define ADS42LB69_CSR_GPIO                0x5     /* ADCDIR data selector               */

#define ADS42LB69_CSR_OFF                 0x8     /* first OFFSET A/B registers         */

#define ADS42LB69_CSR_IDELAY              0xE     /* IDELAY/ISERDES Calibration Support */
#define ADS42LB69_CSR_IDELAY              0xE     /* IDELAY/ISERDES Calibration Support */
#define ADS42LB69_CSR_ADCDIR              0xF     /* ADCDIR data                        */


#define ADS42LB69_CSR_IDELAY              0xE     /* IDELAY/ISERDES Calibration Support */

/* IDELAY */
#define ADS42LB69_IDLY_IO_QA_0            (1<< 0) /* ADS42LB69 QA[0]                */
#define ADS42LB69_IDLY_IO_QA_1            (1<< 1) /* ADS42LB69 QA[1]                */
#define ADS42LB69_IDLY_IO_QA_2            (1<< 2) /* ADS42LB69 QA[2]                */
#define ADS42LB69_IDLY_IO_QA_3            (1<< 3) /* ADS42LB69 QA[3]                */
#define ADS42LB69_IDLY_IO_FRAM_A          (1<< 4) /* ADS42LB69 FRAM_A               */
#define ADS42LB69_IDLY_IO_OVR_A           (1<< 5) /* ADS42LB69 OVR_A                */
#define ADS42LB69_IDLY_IO_QB_0            (1<< 6) /* ADS42LB69 QB[0]                */
#define ADS42LB69_IDLY_IO_QB_1            (1<< 7) /* ADS42LB69 QB[1]                */
#define ADS42LB69_IDLY_IO_QB_2            (1<< 8) /* ADS42LB69 QB[2]                */
#define ADS42LB69_IDLY_IO_QB_3            (1<< 9) /* ADS42LB69 QB[3]                */
#define ADS42LB69_IDLY_IO_FRAM_B          (1<<10) /* ADS42LB69 FRAM_B               */
#define ADS42LB69_IDLY_IO_OVR_B           (1<<11) /* ADS42LB69 OVR_B                */
#define ADS42LB69_IDLY_IO_ALL_A           (0x03F)
#define ADS42LB69_IDLY_IO_ALL_B           (0xFC0)
#define ADS42LB69_IDLY_IO_ALL             (0xFFF)

#define ADS42LB69_IDLY_SEL_ADS01          (0<<12) /* ADS42LB69 device #01           */
#define ADS42LB69_IDLY_SEL_ADS23          (1<<12) /* ADS42LB69 device #23           */
#define ADS42LB69_IDLY_SEL_ADS45          (2<<12) /* ADS42LB69 device #45           */
#define ADS42LB69_IDLY_SEL_ADS67          (3<<12) /* ADS42LB69 device #67           */
#define ADS42LB69_IDLY_SEL_ADS89          (4<<12) /* ADS42LB69 device #89           */
#define ADS42LB69_IDLY_SEL_ALL            (7<<12) /* All ADS42LB69                  */

#define ADS42LB69_IDLY_TAP10              (1<<27) /* 1/10 incr/decr                 */
#define ADS42LB69_IDLY_CMD_NOP            (0<<28) /* No operation                   */
#define ADS42LB69_IDLY_CMD_LD             (1<<28) /* Load                           */
#define ADS42LB69_IDLY_CMD_INC            (2<<28) /* Increment                      */
#define ADS42LB69_IDLY_CMD_DEC            (3<<28) /* Decrement                      */

#define ADS42LB69_IDLY_ISERDESE3_RST      (1<<30) /* Reset ISERDESE3                */
#define ADS42LB69_IDLY_IDELAYE3_RST       (1<<31) /* Reset IDELAYE3                 */

/* ------------------------------------------------------------------------------------------------------------------ */

#define ADS42LB69_MODE_NORM     0x00              /* Normal Mode                              */
#define ADS42LB69_MODE_ZERO     0x01              /* Output all 0s                            */
#define ADS42LB69_MODE_ONE      0x02              /* Output all 1s                            */
#define ADS42LB69_MODE_TOGGLE   0x03              /* Output toggle pattern (0xAAAA / 0x5555)  */
#define ADS42LB69_MODE_RAMP     0x04              /* Output digital ramp                      */
#define ADS42LB69_MODE_TEST1    0x06              /* Single pattern                           */
#define ADS42LB69_MODE_TEST2    0x07              /* Double pattern                           */
#define ADS42LB69_MODE_DESKEW   0x08              /* Deskew pattern (0xAAAA)                  */
#define ADS42LB69_MODE_RAND     0x0a              /* PRBS pattern                             */
#define ADS42LB69_MODE_SINE     0x0b              /* 8-pt sine wave                           */

/* ------------------------------------------------------------------------------------------------------------------ */
/*  Function Prototypes                                                                                               */
/* ------------------------------------------------------------------------------------------------------------------ */

extern void ads42lb69_configure           (int ads_num, int *ads_map);
extern int  ads42lb69_init                (int fd, int fmc, int chan_set, int regs[], int quiet);
extern int  ads42lb69_dump                (int fd, int fmc, int chan_set);
extern int  ads42lb69_set_mode            (int fd, int fmc, int chan, int mode);
extern int  ads42lb69_get_mode            (int fd, int fmc, int chan, int *mode);
extern int  ads42lb69_set_pattern         (int fd, int fmc, int chan, int pattern);
extern int  ads42lb69_get_pattern         (int fd, int fmc, int chan, int *pattern);
extern int  ads42lb69_set_offset          (int fd, int fmc, int off[]);
extern int  ads42lb69_get_offset          (int fd, int fmc, int off[]);
extern int  ads42lb69_calib_reset_idelay  (int fd, int fmc);
extern int  ads42lb69_calib_set_idelay    (int fd, int fmc, int chan, int index, int idelay);
extern int  ads42lb69_calib_get_idelay    (int fd, int fmc, int chan, int index, int *idelay);
extern int  ads42lb69_calib_dump_idelay   (int fd, int fmc, int chan, char *cell_str[]);
extern int  ads42lb69_direct_read         (int fd, int fmc, int chan, int *value);
extern int  ads42lb69_direct_dump         (int fd, int fmc, int chan);
extern int  ads42lb69_calibrate           (int fd, int fmc, int chan, int step, int result);

#endif /*  _H_ADS42LB69LIB */

