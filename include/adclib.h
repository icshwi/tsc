/*=========================< begin file & file header >=======================
 *  References
 *  
 *    filename : adclib.h
 *    author   : JFG, XP
 *    company  : IOxOS
 *    creation : october 18,2018
 *
 *----------------------------------------------------------------------------
 *  Description
 *
 *    That library contains a set of function to access the ADC FMC
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

#ifndef _H_ADCLIB
#define _H_ADCLIB

#define ADC_CSR_SIGN          0x0     /* Signature                        */
#define ADC_CSR_CTL           0x1     /* Main control & status            */
#define ADC_CSR_LED           0x2     /* Front-Panel LED management       */
#define ADC_CSR_SERIAL        0x3     /* Serial Interface                 */
#define ADC_CSR_GPIO          0x5     /* Front-Panel GPIO management      */
#define ADC_CSR_DISC          0x6     /* Discriminator Function           */
#define ADC_CSR_FMC           0x7     /* IFC carrier support              */

/* --------------------------------------------------------------------------------------------------- */
/*  Function Prototypes                                                                                */
/* --------------------------------------------------------------------------------------------------- */
extern int adc_spi_read(int fd, int fmc, int cmd, int reg, int *data);
extern int adc_spi_write(int fd, int fmc, int cmd, int reg, int data);
extern int adc_read_tmp102(int fd, int fmc, uint dev, uint *temp, uint *temp_lo, uint *temp_hi);
extern int adc_set_tmp102(int fd, int fmc, uint dev, uint *temp_lo, uint *temp_hi);

#endif /* _H_ADCLIB */
