/*=========================< begin file & file header >=======================
 *  References
 *  
 *    filename : fifolib.c
 *    author   : JFG
 *    company  : IOxOS
 *    creation : Sept 27,2015
 *    version  : 0.0.1
 *
 *----------------------------------------------------------------------------
 *  Description
 *
 *    This file contains the low level functions to contro FIFOs implemented
 *    in the ifc1211.
 *
 *----------------------------------------------------------------------------
 *
 * Copyright (c) 2015 IOxOS Technologies SA <ioxos@ioxos.ch>
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 * GPL license :
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 *
 *----------------------------------------------------------------------------
 *  Change History
 *  
 *=============================< end file header >============================*/

#include "tscos.h"
#include "tscdrvr.h"
#define DBG
#include "debug.h"

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * Function name : tsc_fifo_init
 * Prototype     : int
 * Parameters    : pointer to IFC1211 device control structure
 *                 FIFO index
 * Return        : 0
 *----------------------------------------------------------------------------
 * Description   : Init fifo
 *
 *++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

int tsc_fifo_init(struct ifc1211_device *ifc, int idx, int mode){
	uint ctl = 0;

	if(idx > IFC1211_FIFO_NUM){
		return ( -EINVAL);
	}
	if(mode == 1){ // Mailbox mode
		ctl = IFC1211_FIFO_CTL_ENA
			| IFC1211_FIFO_CTL_WEA
			| IFC1211_FIFO_CTL_REA
			| IFC1211_FIFO_CTL_MBX;
	}
	else{ // Fifo mode
		ctl = IFC1211_FIFO_CTL_ENA
			| IFC1211_FIFO_CTL_WEA
			| IFC1211_FIFO_CTL_REA;
	}
	iowrite32(ctl, ifc->csr_ptr + IFC1211_CSR_FIFO_CTL[idx]);
	return 0;
}

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * Function name : tsc_fifo_clear
 * Prototype     : int
 * Parameters    : pointer to IFC1211 device control structure
 *                 FIFO index
 * Return        : 0
 *----------------------------------------------------------------------------
 * Description   : Clear fifo
 *
 *++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

int tsc_fifo_clear( struct ifc1211_device *ifc, int idx){
	uint ctl;

	if(idx > IFC1211_FIFO_NUM){
		return ( -EINVAL);
	}
	// Clear
	ctl = IFC1211_FIFO_CTL_RESET
		| IFC1211_FIFO_CTL_ERRF;
	iowrite32(ctl, ifc->csr_ptr + IFC1211_CSR_FIFO_CTL[idx]);
	// Reset all to 0
	ctl = 0;
	iowrite32(ctl, ifc->csr_ptr + IFC1211_CSR_FIFO_CTL[idx]);
	return(0);
}

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * Function name : tsc_fifo_status
 * Prototype     : int
 * Parameters    : pointer to IFC1211 device control structure
 *                 FIFO index
 * Return        : 0
 *----------------------------------------------------------------------------
 * Description   : Get fifo status
 *
 *++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

int tsc_fifo_status(struct ifc1211_device *ifc, int idx, int *sts){
	*sts = ioread32( ifc->csr_ptr + IFC1211_CSR_FIFO_CTL[idx]);
	  debugk(("status%x\n", *sts));
	return 0;
}

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * Function name : tsc_fifo_wait_ef
 * Prototype     : int
 * Parameters    : pointer to IFC1211 device control structure
 *                 FIFO index
 * Return        : 0
 *----------------------------------------------------------------------------
 * Description   : Wait empty fifo
 *
 *++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

int tsc_fifo_wait_ef(struct ifc1211_device *ifc, struct tsc_ioctl_fifo *fifo){
	return 0;
}

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * Function name : tsc_fifo_wait_ff
 * Prototype     : int
 * Parameters    : pointer to IFC1211 device control structure
 *                 FIFO index
 * Return        : 0
 *----------------------------------------------------------------------------
 * Description   : Wait fifo full
 *
 *++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

int tsc_fifo_wait_ff(struct ifc1211_device *ifc, struct tsc_ioctl_fifo *fifo){
	return 0;
}

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * Function name : tsc_fifo_read
 * Prototype     : int
 * Parameters    : pointer to IFC1211 device control structure
 *                 FIFO index
 *                 data pointer
 *                 data count
 *                 pointer to hold FIFO status after last readout
 * Return        : number of word actually written in FIFO
 *----------------------------------------------------------------------------
 * Description   : read <cnt> data from FIFO
 *
 *++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

int tsc_fifo_read(struct ifc1211_device *ifc, int idx, int *data, int cnt, int *sts){
	int wcnt, tot;

	if(idx >IFC1211_FIFO_NUM){
		return ( -EINVAL);
	}
	tot = 0;
	*sts = ioread32( ifc->csr_ptr + IFC1211_CSR_FIFO_CTL[idx]);
	while(1){
		wcnt = IFC1211_FIFO_CTL_WCNT( *sts);
		if (!wcnt){
			return(tot);
		}
		while( wcnt--){
			*data++ = ioread32( ifc->csr_ptr + IFC1211_CSR_FIFO_PORT[idx]);
			tot++;
			if(tot >= cnt){
				*sts = ioread32( ifc->csr_ptr + IFC1211_CSR_FIFO_CTL[idx]);
				return(tot);
			}
		}
		*sts = ioread32( ifc->csr_ptr + IFC1211_CSR_FIFO_CTL[idx]);
	}
}

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * Function name : tsc_fifo_write
 * Prototype     : int
 * Parameters    : pointer to IFC1211 device control structure
 *                 FIFO index
 *                 data pointer
 *                 data count
 *                 pointer to FIFO status after last write
 * Return        : number of word actually written in FIFO
 *----------------------------------------------------------------------------
 * Description   : write <cnt> data in FIFO
 *
 *++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

int tsc_fifo_write(struct ifc1211_device *ifc, int idx, int *data, int cnt, int *sts){
	int wcnt, tot;

	if(idx >IFC1211_FIFO_NUM){
		return ( -EINVAL);
	}
	tot = 0;
	*sts = ioread32( ifc->csr_ptr + IFC1211_CSR_FIFO_CTL[idx]);
	while(1){
		wcnt = IFC1211_FIFO_CTL_WCNT_MAX - IFC1211_FIFO_CTL_WCNT( *sts);
		if (!wcnt){
			return(tot);
		}
		while(wcnt--){
			iowrite32( *data++ , ifc->csr_ptr + IFC1211_CSR_FIFO_PORT[idx]);
			tot++;
			if(tot >= cnt){
				*sts = ioread32( ifc->csr_ptr + IFC1211_CSR_FIFO_CTL[idx]);
				return(tot);
			}
		}
		*sts = ioread32( ifc->csr_ptr + IFC1211_CSR_FIFO_CTL[idx]);
	}
}
