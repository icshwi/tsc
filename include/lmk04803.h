/*=========================< begin file & file header >=======================
 *  References
 *  
 *    filename : lmk04803.h
 *    author   : JFG, XP
 *    company  : IOxOS
 *    creation : october 18,2018
 *
 *----------------------------------------------------------------------------
 *  Description
 *
 *    
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

int adc3112_lmk_intref_500M[32] =
{
  0x001c1000,  /* LMK04803B_R00 Enable  ClkOut_01   + ClkOUT01_DIV = 128 ADS5409 23 SYNC  / TRIG_Calibration    15.625 MHz  */
  0x00140080,  /* LMK04803B_R01 Enable  ClkOut_23   + ClkOUT23_DIV = 4   FMC_CLK0_M2C     / ADS5409 01 CLKIN_DEL 500 MHz    */
  0x00140080,  /* LMK04803B_R02 Enable  ClkOut_45   + ClkOUT23_DIV = 4   FMC_CLK0_M2C     / ADS5409 01 CLKIN_DEL 500 MHz    */
  0x00141000,  /* LMK04803B_R03 Enable  ClkOut_67   + ClkOUT67_DIV = 128 ADS5409 01 SYNC  / Not used             15.625 MHz */
  0x00180080,  /* LMK04803B_R04 Enable  ClkOut_89   + ClkOUT89_DIV = 4   ADS5409 01 CLKIN / Not used             500 MHz    */
  0x00200080,  /* LMK04803B_R05 Enable  ClkOut_1011 + ClkOUT1011_DIV = 4 Not used         / ADS5409 23 CLKIN     500 MHz    */
  0x01110000,  /* LMK04803B_R06 ClkOUT3_Type=0 /ClkOUT2_Type=1 / ClkOUT1_Type=1/ ClkOUT0_Type=1 (0=Power down; 1=LVDS) for direct ADC CLK  */
  0x01010000,  /* LMK04803B_R07 ClkOUT7_Type=0 /ClkOUT6_Type=1 / ClkOUT5_Type=0/ ClkOUT4_Type=1 (0=Power down; 1=LVDS) for direct ADC CLK */
  0x10010000,  /* LMK04803B_R08 ClkOUT11_Type=1/ClkOUT10_Type=0/ ClkOUT9_Type=0/ ClkOUT8_Type=1 (0=Power down; 1=LVDS) for direct ADC CLK */
  0x55555540,  /* LMK04803B_R09 LMK04803B_R09 TI/NS write MUST  */
  0x11004200,  /* LMK04803B_R10 LMK04803B_R10 OscOUT_Type = 1 (LVDS)  Power down + OscIn enabled */
  0x37610000,  /* LMK04803B_R11 Device MODE=0x6 + SYNC Enable  + NO_SYNC_CLKoutX_Y = 110110  SYNC_EN_AUTO = 0   clk_OUT0/clk_OUT6 forced '0'*/
  0x13000000,  /* LMK04803B_R12 LMK04803B_R12 LD pin programmable = PLL2 DLD  */
  0x3B7002c8,  /* LMK04803B_R13 HOLDOVER pin uWIRE SDATOUT  Enable CLKin1 */
  0x00000000,  /* LMK04803B_R14 Bipolar Mode CLKin1 INPUT  */
  0x00000000,  /* LMK04803B_R15 DAC unused */
  0x01550400,  /* LMK04803B_R16  OSC IN level */
  0x00000000,  /* LMK04803B_R17  */
  0x00000000,  /* LMK04803B_R18  */
  0x00000000,  /* LMK04803B_R19  */
  0x00000000,  /* LMK04803B_R20  */
  0x00000000,  /* LMK04803B_R21  */
  0x00000000,  /* LMK04803B_R22  */
  0x00000000,  /* LMK04803B_R23  */
  0x00000000,  /* LMK04803B_R24 PLL1 not used  / PPL2 */
  0x00000000,  /* LMK04803B_R25 DAC config not used */
  0x8fa00000,  /* LMK04803B_R26 PLL2 used  / ICP = 3200 uA */
  0x00000000,  /* LMK04803B_R27 PLL1 not used  */
  0x00100000,  /* LMK04803B_R28 PLL2_R = 1 (freq PFD=100 MHz) / PLL1 N divider = 00 */
  0x01000080,  /* LMK04803B_R29 OSCIN_FREQ <= 100 MHz / PLL2_NCAL = 4) */
  0x05000080,  /* LMK04803B_R30 /PLL2_P = 5 PLL2_N = 4 + activate SYNC */
  0x00000000   /* LMK04803B_R31  LMK04803B_R31 uWIRE Not LOCK */
};
    
int adc3112_lmk_intref_1G[32] =
{
  0x30000800,  /* LMK04803B_R00 Enable  ClkOut_01   + ClkOUT01_DIV = 64  ADS5409 23 SYNC  / TRIG_Calibration     31.250 MHz */
  0x00000040,  /* LMK04803B_R01 Enable  ClkOut_23   + ClkOUT23_DIV = 2   FMC_CLK0_M2C     / ADS5409 01 CLKIN_DEL 1000 MHz   */
  0x00000040,  /* LMK04803B_R02 Enable  ClkOut_45   + ClkOUT45_DIV = 4   FMC_LA_32        / ADS5409 23 CLKIN_DEL 500 MHz   */
  0x30000800,  /* LMK04803B_R03 Enable  ClkOut_67   + ClkOUT67_DIV = 64  ADS5409 01 SYNC  / Not used             31.250 MHz */
  0x00000040,  /* LMK04803B_R04 Enable  ClkOut_89   + ClkOUT89_DIV = 2   ADS5409 01 CLKIN / Not used             1000 MHz   */
  0x00000040,  /* LMK04803B_R05 Enable  ClkOut_1011 + ClkOUT1011_DIV = 2 Not used         / ADS5409 23 CLKIN     1000 MHz   */
  0x01110000,  /* LMK04803B_R06 ClkOUT3_Type=0 /ClkOUT2_Type=1 / ClkOUT1_Type=1/ ClkOUT0_Type=1 (0=Power down; 1=LVDS) for direct ADC CLK  */
  0x01010000,  /* LMK04803B_R07 ClkOUT7_Type=0 /ClkOUT6_Type=1 / ClkOUT5_Type=0/ ClkOUT4_Type=1 (0=Power down; 1=LVDS) for direct ADC CLK */
  0x10010000,  /* LMK04803B_R08 ClkOUT11_Type=1/ClkOUT10_Type=0/ ClkOUT9_Type=0/ ClkOUT8_Type=1 (0=Power down; 1=LVDS) for direct ADC CLK */
  0x55555540,  /* LMK04803B_R09 LMK04803B_R09 TI/NS write MUST  */
  0x11004200,  /* LMK04803B_R10 LMK04803B_R10 OscOUT_Type = 1 (LVDS)  Power down + OscIn enabled */
  0x37610000,  /* LMK04803B_R11 Device MODE=0x6 + SYNC Enable  + NO_SYNC_CLKoutX_Y = 110110  SYNC_EN_AUTO = 0   clk_OUT0/clk_OUT6 forced '0'*/
  0x13000000,  /* LMK04803B_R12 LMK04803B_R12 LD pin programmable = PLL2 DLD  */
  0x3B7002c8,  /* LMK04803B_R13 HOLDOVER pin uWIRE SDATOUT  Enable CLKin1 */
  0x00000000,  /* LMK04803B_R14 Bipolar Mode CLKin1 INPUT  */
  0x00000000,  /* LMK04803B_R15 DAC unused */
  0x01550400,  /* LMK04803B_R16  OSC IN level */
  0x00000000,  /* LMK04803B_R17  */
  0x00000000,  /* LMK04803B_R18  */
  0x00000000,  /* LMK04803B_R19  */
  0x00000000,  /* LMK04803B_R20  */
  0x00000000,  /* LMK04803B_R21  */
  0x00000000,  /* LMK04803B_R22  */
  0x00000000,  /* LMK04803B_R23  */
  0x00000000,  /* LMK04803B_R24 PLL1 not used  / PPL2 */
  0x00000000,  /* LMK04803B_R25 DAC config not used */
  0x8fa00000,  /* LMK04803B_R26 PLL2 used  / ICP = 3200 uA */
  0x00000000,  /* LMK04803B_R27 PLL1 not used  */
  0x00100000,  /* LMK04803B_R28 PLL2_R = 1 (freq PFD=100 MHz) / PLL1 N divider = 00 */
  0x01000080,  /* LMK04803B_R29 OSCIN_FREQ <= 100 MHz / PLL2_NCAL = 4) */
  0x05000080,  /* LMK04803B_R30 /PLL2_P = 5 PLL2_N = 4 + activate SYNC */
  0x00000000   /* LMK04803B_R31  LMK04803B_R31 uWIRE Not LOCK */
};
    
int adc3112_lmk_extref[32] =
{
  0x30000800, /* LMK04803B_R00 Enable  ClkOut_01   + ClkOUT01_DIV = 64  ADS5409 23 SYNC   TRIG_Calibration     31.250 MHz   */
  0x00000020, /* LMK04803B_R01 Enable  ClkOut_23   + ClkOUT23_DIV = 1   FMC_CLK0_M2C      ADS5409 01 CLKIN_DEL 1000 MHz      */
  0x00000020, /* LMK04803B_R02 Enable  ClkOut_45   + ClkOUT45_DIV = 1   FMC_LA_32         ADS5409 23 CLKIN_DEL 1000 MHz      */
  0x30000800, /* LMK04803B_R03 Enable  ClkOut_67   + ClkOUT67_DIV = 64  ADS5409 01 SYNC   Not used             31.250 MHz   */
  0x00000020, /* LMK04803B_R04 Enable  ClkOut_89   + ClkOUT89_DIV = 1   ADS5409 01 CLKIN  Not used             1000 MHz      */
  0x00000020, /* LMK04803B_R05 Enable  ClkOut_1011 + ClkOUT1011_DIV = 1 Not used          ADS5409 23 CLKIN     1000 MHz      */
  0x11110000, /* LMK04803B_R06 ClkOUT3_Type=0 /ClkOUT2_Type=1 / ClkOUT1_Type=1/ ClkOUT0_Type=1 (0=Power down; 1=LVDS) for direct ADC CLK   */
  0x01110000, /* LMK04803B_R07 ClkOUT7_Type=0 /ClkOUT6_Type=1 / ClkOUT5_Type=0/ ClkOUT4_Type=1 (0=Power down; 1=LVDS) for direct ADC CLK   */
  0x10010000, /* LMK04803B_R08 ClkOUT11_Type=1/ClkOUT10_Type=0/ ClkOUT9_Type=0/ ClkOUT8_Type=1 (0=Power down; 1=LVDS) for direct ADC CLK   */
  0x55555540, /* LMK04803B_R09 TI/NS write MUST */
  0x11004200, /* LMK04803B_R10 OscOUT_Type = 1 (LVDS)  Power down + OscIn enabled   */
  0x87610000, /* LMK04803B_R11 Device MODE=0x10 (external) + No SYNC output                     */
  0x1b000000, /* LMK04803B_R12 LD pin programmable = PLL2 DLD                       */
  /* 0x0B600240, LMK04803B_R13 HOLDOVER pin uWIRE SDATOUT  Enable CLKin1            */
  0x3B7002c8, /* LMK04803B_R13 HOLDOVER pin uWIRE SDATOUT  Enable CLKin1 */
  0x00000000, /* LMK04803B_R14 Bipolar Mode CLKin1 INPUT                            */
  0x00000000, /* LMK04803B_R15 DAC=512 / MAN_DAC unused                             */
  0x01550400, /* LMK04803B_R16 OSC IN level          */
  0x00000000, /* LMK04803B_R17 */
  0x00000000, /* LMK04803B_R18 */
  0x00000000, /* LMK04803B_R19 */
  0x00000000, /* LMK04803B_R20 */
  0x00000000, /* LMK04803B_R21 */
  0x00000000, /* LMK04803B_R22 */
  0x00000000, /* LMK04803B_R23 */
  0x00000040, /* LMK04803B_R24 PLL1 not used   PPL2  */
  0x00000040, /* LMK04803B_R25 DAC config not used   */
  0x8fa00040, /* LMK04803B_R26 PLL2 used   ICP = 3200 uA                                     */
  0x10000040, /* LMK04803B_R27 PLL1 not used                                                 */
  0x00100100, /* LMK04803B_R28 PLL2_R = 1 (freq PFD=100 MHz) / PLL1 N divider = 00           */
  0x01000080, /* LMK04803B_R29 OSCIN_FREQ <= 100 MHz  PLL2_NCAL = 4)                         */
  0x05000080, /* LMK04803B_R30 PLL2_P = 5 PLL2_N = 4                                         */
  0x00000000  /* LMK04803B_R31 uWIRE Not LOCK                                                */
};
    
int adc3112_lmk_extref_120M[32] =
{
  0x00000100, /* R00 1920 MHz /  8 = 240 MHz ( 0, 1)                                                              */
  0x00000040, /* R01 1920 MHz /  2 = 960 MHz ( 2, 3)                                                              */
  0x00000040, /* R02 1920 MHz /  2 = 960 MHz ( 4, 5)                                                              */
  0x00000100, /* R03 1920 MHz /  8 = 240 MHz ( 6, 7)                                                              */
  0x00000040, /* R04 1920 MHz /  2 = 960 MHz ( 8, 9)                                                              */
  0x00000040, /* R05 1920 MHz /  2 = 960 MHz (10,11)}                                                             */
  0x10000000, /* R06 CLKOUT_3 -> LVDS, CLKOUT_2 -> OFF,  CLKOUT_1 -> OFF,   CLKOUT_0 -> OFF                       */
  0x00100000, /* R07 CLKOUT_7 -> OFF,  CLKOUT_6 -> OFF , CLKOUT_5 -> LVDS , CLKOUT_4 -> OFF                       */
  0x10010000, /* R08 CLKOUT_11-> LVDS, CLKOUT_10-> OFF,  CLKOUT_9 -> OFF ,  CLKOUT_8 -> LVDS                      */
  0x55555540, /* R09 TI/NS MagicWord                                                                              */
  0x10004820, /* R10 VCO_MUX=VCO, EN_FBMux=ON, FBMux=CLKout2                                                      */
//0x14018000, /* R11 MODE=2 (dual PLL, int. VCO, zero-delay) + No SYNC output                                     */
  0x17618000, /* R11 MODE=2 (dual PLL, int. VCO, zero-delay) + No SYNC output                                     */
//0x17610000, /* LMK04803B_R11 Device MODE=0x2  + No SYNC output                                                  */
  0x1B000040, /* R12 LD_MUX=PLL1 & PLL2 DLD                                                                       */
//0x13000040, /* R12 LD_MUX=PLL1 & PLL2 DLD                                                                       */
  0x3B139260, /* R13 HOLDOVER_MUX=uWire Readback, EN_CLKin1, Status_CLKin1_MUX=LOS, DISABLE_DLD1_DET              */
  0x13000000, /* R14 EN_LOS, LOS_TIMEOUT = 1200 ns                                                                */
  0x00000000, /* R15 EN_LOS, LOS_TIMEOUT = 1200 ns                                                                */
  0x01550400, /* R16 XTAL_LVL                                                                                     */
  0x00000000, /* R17                                                                                              */
  0x00000000, /* R18                                                                                              */
  0x00000000, /* R19                                                                                              */
  0x00000000, /* R20                                                                                              */
  0x00000000, /* R21                                                                                              */
  0x00000000, /* R22                                                                                              */
  0x00000000, /* R23                                                                                              */
//0x00000000, /* R24                                                                                              */
  0x000000c0, /* R24                                                                                              */
  0x0009C400, /* R25 DAC not used, PLL1_DLD_CNT = 10000                                                           */
  0xEFA00800, /* R26 EN_PLL2_REF_2X=1, PLL2_CP_POL=NEG, PLL2_WND_SIZE=3.7ns, PLL2_CP_GAIN=3200uA, PLL2_DLD_CNT=64 */
//0x10000060, /* R27  PLL1_CP_POL=POS, PLL1_CP_GAIN=100uA, CLKin1_PreR_DIV=1, CLKin0_PreR_DIV=1, PLL1_R=3         */
  0x100000c0, /* R27  PLL1_CP_POL=POS, PLL1_CP_GAIN=100uA, CLKin1_PreR_DIV=1, CLKin0_PreR_DIV=1, PLL1_R=3         */
  0x00500600, /* R28 PLL2_R=5, PLL1_N=24                                                                          */
  0x01000300, /* R29 OSCIN_FREQ (>63 MHz to 127 MHz), PLL2_N_CAL = 24                                             */
  0x02000300, /* R30 PLL2_P = 2 PLL2_N = 24                                                                       */
  0x00000000, /* R31 uWIRE Not LOCKED                                                                             */
};

