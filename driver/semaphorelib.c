/*=========================< begin file & file header >=======================
 *  References
 *
 *    filename : semaphorelib.c
 *    author   : XP
 *    company  : IOxOS
 *    creation : Sept 4,2016
 *
 *----------------------------------------------------------------------------
 *  Description
 *
 *    This file contains the low level functions to drive the semaphore
 *    implemented on the TSC.
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

#include "tscos.h"
#include "tscdrvr.h"

#define DBG
#include "debug.h"

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * Function name : semaphore_status
 * Prototype     : int
 * Parameters    : pointer to tsc device control structure
 *                 pointer to semaphore structure
 * Return        : 0
 *----------------------------------------------------------------------------
 * Description   : get semaphore status
 *
 *++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

int semaphore_status(struct tsc_device *ifc, struct tsc_ioctl_semaphore  *semaphore){
	semaphore->sts = ioread32(ifc->csr_ptr + TSC_CSR_SEMAPHORE);
	return 0;
}

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * Function name : semaphore_release
 * Prototype     : int
 * Parameters    : pointer to tsc device control structure
 *                 pointer to semaphore structure
 * Return        : 0
 *----------------------------------------------------------------------------
 * Description   : release semaphore function
 *
 *++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

int semaphore_release(uint idx, void *base_shm_ptr, uint tag){
	uint temp_rd = 0;
	base_shm_ptr = (int*)base_shm_ptr + 32 + (idx * 2); // Go to SHM offset 0x80 for SEMAPHORE region
												  	    // + offset of specific semaphore (64b step)
	if (tag){
		temp_rd = ioread32(base_shm_ptr); 				// check current semaphore state
		if((temp_rd & 0x7fffffff) != tag){
			return -1;									// tag is not matching content of semaphore
		}
	}
	*(int*)base_shm_ptr = 0;					        // Release the semaphore and reinit the tag
	return 0;
}

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * Function name : semaphore_get
 * Prototype     : int
 * Parameters    : pointer to tsc device control structure
 *                 pointer to semaphore structure
 * Return        : error/success
 *----------------------------------------------------------------------------
 * Description   : get semaphore function
 *
 *++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

int semaphore_get(uint idx, void *base_shm_ptr, uint tag){
	uint temp_wr = 0;
	uint temp_rd = 0;
	base_shm_ptr = (int*)base_shm_ptr + 32 + (idx * 2); // Go to SHM offset 0x80 for SEMAPHORE region
												  	    // + offset of specific semaphore (64 step)

	temp_rd = ioread32( base_shm_ptr);	    			// check current semaphore state
	temp_wr = 0x80000000 | tag;

	if(!(temp_rd & 0x80000000))          // if free ->
	{
	  iowrite32( temp_wr, base_shm_ptr); //try to acquire it
	}
	temp_rd = ioread32( base_shm_ptr);	// check again current semaphore state
	if( temp_rd == temp_wr){            // Semaphore is owned by requester
		return 0;
	}
	else{                              // semaphore busy
		return 3;
	}
}
