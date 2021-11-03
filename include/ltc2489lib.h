/*=========================< begin file & file header >=======================
 *  References
 *
 *    filename : ltc2489lib.h
 *    author   : CG
 *    company  : IOxOS
 *    creation : June 23,2020
 *
 *----------------------------------------------------------------------------
 *  Description
 *
 *    This file contain the declarations of all exported functions define in
 *    ltc2489lib.c
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

#ifndef _H_LTC2489LIB
#define _H_LTC2489LIB

#include <sys/types.h>

/* Command bits */
#define LTC2489_CMD_MUX(a)      ((a&1)<<0)
#define LTC2489_CMD_INV         (1<<3)
#define LTC2489_CMD_SGL         (1<<4)
#define LTC2489_CMD_EN          (1<<5)
#define LTC2489_CMD             (2<<6)

/* Command for differential-mode channel acquisition */
#define LTC2489_CMD_CH01_DIFF   (LTC2489_CMD_MUX(0) |                   LTC2489_CMD_EN | LTC2489_CMD)
#define LTC2489_CMD_CH10_DIFF   (LTC2489_CMD_MUX(0) | LTC2489_CMD_INV | LTC2489_CMD_EN | LTC2489_CMD)
#define LTC2489_CMD_CH23_DIFF   (LTC2489_CMD_MUX(1) |                   LTC2489_CMD_EN | LTC2489_CMD)
#define LTC2489_CMD_CH32_DIFF   (LTC2489_CMD_MUX(1) | LTC2489_CMD_INV | LTC2489_CMD_EN | LTC2489_CMD)

/* Command for single-mode channel acquisition */
#define LTC2489_CMD_CH0_SGL     (LTC2489_CMD_MUX(0) | LTC2489_CMD_SGL |                   LTC2489_CMD_EN | LTC2489_CMD)
#define LTC2489_CMD_CH1_SGL     (LTC2489_CMD_MUX(0) | LTC2489_CMD_SGL | LTC2489_CMD_INV | LTC2489_CMD_EN | LTC2489_CMD)
#define LTC2489_CMD_CH2_SGL     (LTC2489_CMD_MUX(1) | LTC2489_CMD_SGL |                   LTC2489_CMD_EN | LTC2489_CMD)
#define LTC2489_CMD_CH3_SGL     (LTC2489_CMD_MUX(1) | LTC2489_CMD_SGL | LTC2489_CMD_INV | LTC2489_CMD_EN | LTC2489_CMD)

int ltc2489_read(int fd, int i2c_bus, int device_address, int cmd, int *data);

#endif /*  _H_LTC2489LIB */
