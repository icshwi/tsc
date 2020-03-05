/*=========================< begin file & file header >=======================
 *  References
 *  
 *    filename : fscopelib.h
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

#define IFC1211

#define ADC_FMC1        1
#define ADC_FMC2        2
#define ADC_FMC_NUM     2

#define ADC_CSR_BASE   0x00001000
#define ADC_CSR_OFF_FMC2    0x100
#define ADC_CSR_SIGN         0x80
#define ADC_CSR_CTL          0x81
#define ADC_CSR_LED          0x82
#define ADC_CSR_SERIAL       0x83
#define ADC_CSR_GPIO         0x85
#define ADC_CSR_DISC         0x86
#define ADC_CSR_FMC          0x87
#define ADC_CSR_ADDR( csr)   (ADC_CSR_BASE | (csr<<2))

#define ADC_SIGN_MASK  0xffff0000

int adc_csr_rd(int fd, int fmc, int csr);
void adc_csr_wr(int fd, int fmc, int csr, int data);
int adc_identify(int fd, int fmc);
int adc_spi_read(int fd, int fmc, int cmd, int reg);
int adc_spi_write(int fd, int fmc, int cmd, int reg, int data);
uint adc_i2c_read(int fd, int fmc, uint cmd, uint reg);
int adc_i2c_write(int fd, int fmc, uint cmd, uint reg, uint data);
    
