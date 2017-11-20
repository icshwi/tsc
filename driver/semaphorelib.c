/*=========================< begin file & file header >=======================
 *  References
 *
 *    filename : semaphorelib.c
 *    author   : XP
 *    company  : IOxOS
 *    creation : Sept 4,2016
 *    version  : 0.0.1
 *
 *----------------------------------------------------------------------------
 *  Description
 *
 *    This file contains the low level functions to drive the semaphore
 *    implemented on the TSC.
 *
 *----------------------------------------------------------------------------
 *  Copyright Notice
 *
 *    Copyright and all other rights in this document are reserved by
 *    IOxOS Technologies SA. This documents contains proprietary information
 *    and is supplied on express condition that it may not be disclosed,
 *    reproduced in whole or in part, or used for any other purpose other
 *    than that for which it is supplies, without the written consent of
 *    IOxOS Technologies SA
 *
 *----------------------------------------------------------------------------
 *  Change History
 *
 *
 *=============================< end file header >============================*/

#include "tscos.h"
#include "tscdrvr.h"

#define DBG
#include "debug.h"

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * Function name : semaphore_status
 * Prototype     : int
 * Parameters    : pointer to TSC device control structure
 *                 pointer to semaphore structure
 * Return        : error/success
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
 * Parameters    : pointer to TSC device control structure
 *                 pointer to semaphore structure
 * Return        : error/success
 *----------------------------------------------------------------------------
 * Description   : release semaphore function
 *
 *++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
int semaphore_release(uint idx, void *base_shm_ptr){
	base_shm_ptr = (int*)base_shm_ptr + 32 + (idx * 2); // Go to SHM offset 0x80 for SEMAPHORE region
												  	    // + offset of specific semaphore (64b step)
	*(int*)base_shm_ptr = 0;					        // Release the semaphore and reinit the tag
	return 0;
}

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * Function name : semaphore_get
 * Prototype     : int
 * Parameters    : pointer to TSC device control structure
 *                 pointer to semaphore structure
 * Return        : error/success
 *----------------------------------------------------------------------------
 * Description   : get semaphore function
 *
 *++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
int semaphore_get(uint idx, void *base_shm_ptr, uint *tag){
	uint temp_wr = 0;
	uint temp_rd = 0;
	base_shm_ptr = (int*)base_shm_ptr + 32 + (idx * 2); // Go to SHM offset 0x80 for SEMAPHORE region
												  	    // + offset of specific semaphore (64 step)
#ifdef PPC
	temp_wr = 0x80 + (*tag & 0x7f); 			  	    // Get semaphore and add specific tag
#else
	temp_wr = 0x80000000 + ((*tag & 0x7f) << 24); 		// Get semaphore and add specific tag
#endif

	temp_rd = *(int*)base_shm_ptr;	 // Acquire current state of the semaphore to check availability

	if((temp_rd == temp_wr) || (temp_rd == 0)){ // Semaphore is already owned or free -> get it
		*(int*)base_shm_ptr = (temp_wr);
		return 0;
	}
	else{ // semaphore busy
		return 3;
	}
}
