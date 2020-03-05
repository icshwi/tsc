/*=========================< begin file & file header >=======================
 *  References
 *  
 *    filename : fscope3117lib.h
 *    author   : JFG, XP
 *    company  : IOxOS
 *    creation : october 18,2018
 *
 *----------------------------------------------------------------------------
 *  Description
 *
 *    That library contains a set of function to access the ADC3117 FMC
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

#define ADC3117_FMC1               1
#define ADC3117_FMC2               2
#define ADC3117_FMC_NUM            2
#define ADC3117_CHAN_NUM          20
#define ADC3117_CHAN_GROUP         4

#define ADC3117_CSR_BASE   0x00001000
#define ADC3117_CSR_OFF_FMC2    0x100
#define ADC3117_CSR_SIGN         0x80
#define ADC3117_CSR_CTL          0x81
#define ADC3117_CSR_LED          0x82
#define ADC3117_CSR_SERIAL       0x83
#define ADC3117_CSR_GPIO         0x85
#define ADC3117_CSR_DISC         0x86
#define ADC3117_CSR_FMC          0x87
#define ADC3117_CSR_ADDR( csr)   (ADC3117_CSR_BASE | (csr<<2))

#define ADC3117_SIGN_ID    0x31170000
#define ADC3117_SIGN_MASK  0xffff0000


#define adc3117_csr_rd(fd, fmc, csr)                 adc_csr_rd(fd, fmc, csr)
#define adc3117_csr_wr(fd, fmc, csr, data)           adc_csr_rd(fd, fmc, csr, data)
#define adc3117_identify(fd, fmc)                     adc_identify(fd, fmc)
#define adc3117_spi_read(fd, fmc, cmd, reg)          adc_spi_read(fd, fmc, cmd, reg)
#define adc3117_spi_write(fd, fmc, cmd, reg, data)   adc_spi_write(fd, fmc, cmd, reg, data)
#define adc3117_i2c_read(fd, fmc, dev, reg)          adc_i2c_read(fd, fmc, cmd, reg)
#define adc3117_i2c_write(fd, fmc, dev, reg, data)   adc_i2c_write(fd, fmc, cmd, reg, data)

int adc3117_set_verbose( int vf);
    
