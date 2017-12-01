/*=========================< begin file & file header >=======================
 *  References
 *  
 *    filename : tscklib.h
 *    author   : JFG, XP
 *    company  : IOxOS
 *    creation : june 30,2008
 *
 *----------------------------------------------------------------------------
 *  Description
 *
 *    This file contain the declarations of all exported functions define in
 *    tscklib.c
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


#ifndef _H_TSCKLIB
#define _H_TSCKLIB

unsigned int tsc_msb32( unsigned int x);
int tsc_dev_init( struct tsc_device *ifc);
void tsc_dev_exit( struct tsc_device *ifc);
int tsc_irq_init( struct tsc_device *ifc);
void tsc_irq_exit( struct tsc_device *ifc);
int tsc_map_mas_init( struct tsc_device *ifc);
void tsc_map_mas_exit( struct tsc_device *ifc);
int tsc_map_slv_init( struct tsc_device *ifc);
void tsc_map_slv_exit( struct tsc_device *ifc);
int tsc_map_read( struct tsc_device *ifc, struct tsc_ioctl_map_ctl *m);
int tsc_map_clear( struct tsc_device *ifct, struct tsc_ioctl_map_ctl *m);
int tsc_csr_op( struct tsc_device *ifc, struct tsc_ioctl_csr_op *csr_op);
int tsc_shm_init( struct tsc_device *ifc, int idx);
void tsc_shm_exit( struct tsc_device *ifc, int idx);
int tsc_kbuf_alloc( struct tsc_device *ifc, struct tsc_ioctl_kbuf_req *r);
int tsc_kbuf_free( struct tsc_device *ifc, struct tsc_ioctl_kbuf_req *r);
int tsc_sflash_init( struct tsc_device *ifc);
int tsc_dma_init( struct tsc_device *ifc);
void tsc_dma_exit( struct tsc_device *ifc);
int tsc_timer_init( struct tsc_device *ifc);
int tsc_i2c_init( struct tsc_device *ifc);
void tsc_i2c_exit( struct tsc_device *ifc);
int tsc_semaphore_release(struct tsc_device *ifc, struct tsc_ioctl_semaphore *semaphore);
int tsc_semaphore_get(struct tsc_device *ifc, struct tsc_ioctl_semaphore *semaphore);

#include "irqlib.h"
#include "maplib.h"
#include "mapmaslib.h"
#include "mapslvlib.h"
#include "rdwrlib.h"
#include "sflashlib.h"
#include "dmalib.h"
#include "timerlib.h"
#include "fifolib.h"
#include "i2clib.h"
#include "semaphorelib.h"

#endif /*  _H_TSCKLIB */
