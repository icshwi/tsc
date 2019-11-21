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

int adc3110_lmk_intref_250M[32] =
{
  0x80000140,   /* LMK04906_R00 Enable + ClkOUT0_DIV = 10  <(Test point R184))             */
  0x00000140,   /* LMK04906_R01 Enable  ClkOut_1 + ClkOUT0_DIV = 10  -> 2500/10 = 250 MHz  */
  0x00000140,   /* LMK04906_R02 Enable  ClkOut_2 + ClkOUT0_DIV = 10                        */
  0x00000140,   /* LMK04906_R03 Enable  ClkOut_3 + ClkOUT0_DIV = 10                        */
  0x00000140,   /* LMK04906_R04 Enable  ClkOut_4 + ClkOUT0_DIV = 10                        */
  0x00000140,   /* LMK04906_R05 Enable  ClkOut_5 + ClkOUT0_DIV = 10  -> 2500/10 = 250 MHz  */
  0x01100000,   /* LMK04906_R06 ClkOUT0_Type/ClkOUT1_Type = 1 (LVDS)                       */
  0x01100000,   /* LMK04906_R07 ClkOUT2_Type/ClkOUT3_Type = 1 (LVDS)                       */
  0x01010000,   /* LMK04906_R08 ClkOUT4_Type/ClkOUT5_Type = 1 (LVDS)                       */
  0x55555540,   /* LMK04906_R09 TI/NS write MUST                                           */
  0x11404200,   /* LMK04906_R10 OscOUT_Type = 1 (LVDS)  Powerdown                          */
  //0x34028000,   /* LMK04906_R11 Device MODE=0x6 + No SYNC output + AUTO SYNC + QUAL      */
  0x3402b000,   /* LMK04906_R11 Device MODE=0x6 + No SYNC output                           */
  0x03000000,   /* LMK04906_R12 LD pin programmable                                        */
  0x3B7002c8,   /* LMK04906_R13 HOLDOVER pin uWIRE SDATOUT  Enable CLKin1                  */
  0x00000000,   /* LMK04906_R14 Bipolar Mode CLKin1 INPUT                                  */
  0x00000000,   /* LMK04906_R15 DAC unused                                                 */
  0x01550400,   /* LMK04906_R16 OSC IN level                                               */
  0x00000000,   /* LMK04906_R17                                                            */
  0x00000000,   /* LMK04906_R18                                                            */
  0x00000000,   /* LMK04906_R19                                                            */
  0x00000000,   /* LMK04906_R20                                                            */
  0x00000000,   /* LMK04906_R21                                                            */
  0x00000000,   /* LMK04906_R22                                                            */
  0x00000000,   /* LMK04906_R23                                                            */
  0x00000000,   /* LMK04906_R24 PLL1 not used  / PPL2                                      */
  0x00000000,   /* LMK04906_R25 DAC config not used                                        */
  0x8fa00000,   /* LMK04906_R26 PLL2 used  / ICP = 3200 uA                                 */
  0x00000000,   /* LMK04906_R27 PLL1 not used                                              */
  0x00200000,   /* LMK04906_R28 PLL2_R = 2 /PPL1 N divider = 00                            */
  0x01800320,   /* LMK04906_R29 OSCIN_FREQ /PLL2_NCAL = 25)                                */
  0x02000320,   /* LMK04906_R30 /PLL2_P = 2 PLL2_N = 25    VCO_2= 2500MHz                  */
  0x00000000    /* LMK04906_R31 uWIRE Not LOCK                                             */
};

int adc3110_lmk_extref[32] =
{
  0x80000020,    /* LMK04906_R00 Disable ClkOut_0 + ClkOUT0_DIV = 10  <(Test point R184))  */
  0x00000020,    /* LMK04906_R01 Enable  ClkOut_1 + ClkOUT0_DIV = 10  -> 2500/10 = 250 MHz  */
  0x00000020,    /* LMK04906_R02 Enable  ClkOut_2 + ClkOUT0_DIV = 10   */
  0x00000020,    /* LMK04906_R03 Enable  ClkOut_3 + ClkOUT0_DIV = 10  */
  0x00000020,    /* LMK04906_R04 Enable  ClkOut_4 + ClkOUT0_DIV = 10  */
  0x00000020,    /* LMK04906_R05 Enable  ClkOut_5 + ClkOUT0_DIV = 10  */
  0x01100000,    /* LMK04906_R06 ClkOUT0_Type/ClkOUT1_Type = 1 (LVDS)  */
  0x01100000,    /* LMK04906_R07 ClkOUT2_Type/ClkOUT3_Type = 1 (LVDS)  */
  0x01010000,    /* LMK04906_R08 ClkOUT4_Type/ClkOUT5_Type = 1 (LVDS)  */
  0x55555540,    /* LMK04906_R09 TI/NS write MUST  */
  0x11484000,    /* LMK04906_R10 OscOUT_Type = 1 (LVDS)  Powerdown  */
  0x8402B000,    /* LMK04906_R11 Device MODE=0x10 + SYNC manual mode  */
  0x030c0000,    /* LMK04906_R12 LD pin programmable  */
  0x3B7002c0,    /* LMK04906_R13 HOLDOVER pin uWIRE SDATOUT  Enable CLKin1  */
  0x00000000,    /* LMK04906_R14 Bipolar Mode CLKin1 INPUT  */
  0x00000000,    /* LMK04906_R15 DAC unused  */
  0x01550400,    /* LMK04906_R16 OSC IN level  */
  0x00000000,    /* LMK04906_R17                                                         */
  0x00000000,    /* LMK04906_R18                                                            */
  0x00000000,    /* LMK04906_R19                                                            */
  0x00000000,    /* LMK04906_R20                                                            */
  0x00000000,    /* LMK04906_R21                                                            */
  0x00000000,    /* LMK04906_R22                                                            */
  0x00000000,    /* LMK04906_R23                                                            */
  0x00000000,    /* LMK04906_R24 PLL1/PPL2 not used  */
  0x00000000,    /* LMK04906_R25 R18 DAC config not used  */
  0x03a00000,    /* LMK04906_R26 PLL2 not used  */
  0x00000000,    /* LMK04906_R27 PLL1 not used  */
  0x00000000,    /* LMK04906_R28 PLL1/PPL2 not used  */
  0x00000000,    /* LMK04906_R29 OSCIN_FREQ /PLL2_NCAL)  */
  0x00000000,    /* LMK04906_R30 PLL2 P/N  */
  0x00000000     /* LMK04906_R31 uWIRE Not LOCK  */
};


