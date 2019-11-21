/*=========================< begin file & file header >=======================
 *  References
 *  
 *    filename : TscMon.h
 *    author   : JFG, XP
 *    company  : IOxOS
 *    creation : Sept 14,2015
 *
 *----------------------------------------------------------------------------
 *  Description
 *
 *    This file contain the declarations of all exported functions define in
 *    TscMon.c
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

#ifndef _H_TSCMON
#define _H_TSCMON

#define TSC_OK    0
#define TSC_ERR  -1
#define TSC_QUIT -2

#include "acq1430.h"
#include "adc3110.h"
#include "adc3112.h"
#include "adc3117.h"
#include "alias.h"
#include "amc.h"
#include "buf.h"
#include "conf.h"   
#include "ddr.h"
#include "dma.h"
#include "fifo.h"
#include "i2c.h"
#include "lmk.h"
#include "map.h"
#include "mbox.h"
#include "rdwr.h"
#include "rsp1461.h"
#include "rdt1465.h"
#include "rcc1466.h"
#include "rtm.h"
#include "script.h"   
#include "semaphore.h"
#include "sflash.h"
#include "timer.h"
#include "tst.h"
#include "gscope.h"

int tsc_print_usage(struct cli_cmd_para *c);
int tsc_ddr_idel_calib_start(int quiet);

#endif /* _H_TSCMON */
