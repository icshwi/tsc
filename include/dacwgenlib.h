/*=========================< begin file & file header >=======================
 *  References
 *
 *    filename : dacwgenlib.h
 *    author   : CG
 *    company  : IOxOS
 *    creation : August 24,2020
 *
 *----------------------------------------------------------------------------
 *  Description
 *
 *    This file contain the declarations of all exported functions define in
 *    dacwgenlib.c
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

#ifndef _H_DACWGENLIB
#define _H_DACWGENLIB

#include <sys/types.h>

#define XUSER_WGEN_CS0_CS       0x00000000    /* Control & Status Register        */
#define XUSER_WGEN_CS0_SIGN     0x00000004    /* Signature                        */
#define XUSER_WGEN_CS0_DEF_AB   0x00000008    /* Default value for channel A & B  */
#define XUSER_WGEN_CS0_DEF_CD   0x0000000C    /* Default value for channel C & D  */
#define XUSER_WGEN_CS0_INI_A    0x00000020    /* Init register for channel A      */
#define XUSER_WGEN_CS0_STEP_A   0x00000024    /* Step register for channel A      */
#define XUSER_WGEN_CS0_INI_B    0x00000028    /* Init register for channel B      */
#define XUSER_WGEN_CS0_STEP_B   0x0000002C    /* Step register for channel B      */
#define XUSER_WGEN_CS0_INI_C    0x00000030    /* Init register for channel C      */
#define XUSER_WGEN_CS0_STEP_C   0x00000034    /* Step register for channel C      */
#define XUSER_WGEN_CS0_INI_D    0x00000038    /* Init register for channel D      */
#define XUSER_WGEN_CS0_STEP_D   0x0000003C    /* Step register for channel D      */

#define XUSER_WGEN_SIGN         0x57415600    /* Signature 'WAV' + revision       */

#define XUSER_WGEN_CS1_BUF_A    0x00000000    /* Buffer for channel A             */
#define XUSER_WGEN_CS1_BUF_B    0x00020000    /* Buffer for channel B             */
#define XUSER_WGEN_CS1_BUF_C    0x00040000    /* Buffer for channel C             */
#define XUSER_WGEN_CS1_BUF_D    0x00060000    /* Buffer for channel D             */

/*
 * Waveform Generator Mode
 */
#define XUSER_WGEN_MODE_LUT_SINGLE                0   /* LUT immediate single                                         */
#define XUSER_WGEN_MODE_LUT_CYCLIC                1   /* LUT immediate cyclic                                         */
#define XUSER_WGEN_MODE_PERIODIC_Y_SYMETRICAL     2   /* Periodic y-symetrical  i.e. f(x) = f(p-x)                    */
#define XUSER_WGEN_MODE_PERIODIC_XY_SYMETRICAL    3   /* Periodic xy-symetrical i.e. f(x) = f(p-x) and f(-x) = -f(x)  */
#define XUSER_WGEN_MODE_SQUARE_WAVE               4   /* Squae Wave Generator                                         */

#define XUSER_WGEN_CS_RUN(ch)         (    1<<((3-(ch&3))*8+7)) /* Generator RUN      */
#define XUSER_WGEN_CS_ENABLE(ch)      (    1<<((3-(ch&3))*8+6)) /* Channel Enable     */
#define XUSER_WGEN_CS_BUSY(ch)        (    1<<((3-(ch&3))*8+5)) /* Generator BUSY     */
#define XUSER_WGEN_CS_PRESENT(ch)     (    1<<((3-(ch&3))*8+4)) /* Channel Present    */
#define XUSER_WGEN_CS_MODE(ch, m)     ((m&7)<<((3-(ch&3))*8+0)) /* Mode               */

int dacwgen_fill_file    (int fd, int ch, char *filename, int quiet); 
int dacwgen_start_wave   (int fd, int ch, int mode);
int dacwgen_stop_wave    (int fd, int ch);
int dacwgen_init         (int fd);
int dacwgen_set_wave     (int fd, int ch, int ref, int init, int step, int quiet);

#endif /*  _H_DACWGENLIB */
