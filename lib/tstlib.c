/*=========================< begin file & file header >=======================
 *  References
 *  
 *    filename : tstlib.c
 *    author   : JFG, XP
 *    company  : IOxOS
 *    creation : october 10,2008
 *
 *----------------------------------------------------------------------------
 *  Description
 *
 *    That library allows to access test functions.
 *
 *----------------------------------------------------------------------------
 *
 *  Copyright Notice
 *  
 *    Copyright and all other rights in this document are reserved by 
 *    IOxOS Technologies SA. This documents contains proprietary information    
 *    and is supplied on express condition that it may not be disclosed, 
 *    reproduced in whole or in part, or used for any other purpose other
 *    than that for which it is supplies, without the written consent of  
 *    IOxOS Technologies SA                                                        
 *  
 *=============================< end file header >============================*/

#include <stdlib.h>
#include <stdio.h>
#include <pty.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/time.h>

typedef unsigned int u32;

#include <tscioctl.h>
#include <tsculib.h>

// -------------------------------------------------------------------------------------

int tst_get_range( char *para, int *first, int *last){
  	  char *p;

  	  *first = strtoul( para, &p, 16);
  	  *last = *first;
  	  p = strpbrk( para,".");
  	  if( p){
    	para = p + strspn(p,".");
    	*last =  strtoul( para, &p, 16);
  	  }
  	  return( *first);
}


int tst_cpu_fill( void *a, int size, int mode, int p1, int p2){
	int i;

	if(mode == 0){
		for( i = 0; i < size; i += 4){
			*(int *)(a+i) = p1;
		}
	}
	if( mode == 1){
		for( i = 0; i < size; i += 4){
			*(int *)(a+i) = p1;
			p1 += p2;
		}
	}
	return( 0);
}

// -------------------------------------------------------------------------------------

void * tst_cpu_check( void *a, int size, int mode, int p1, int p2){
	int i;

	if( mode == 0){
		for( i = 0; i < size; i += 4){
			if( *(int *)(a+i) != p1){
				return( a+i);
			}
		}
	}
	if( mode == 1){
		for( i = 0; i < size; i += 4){
			if( *(int *)(a+i) != p1){
				return( a+i);
			}
			p1 += p2;
		}
	}
	return(NULL);
}

// -------------------------------------------------------------------------------------

int tst_cpu_copy( void *a1, void *a2, int size, int ds){
	int i;

	if( ds == 1){
		for( i = 0; i < size; i += ds){
			*(char *)(a1+i) = *(char *)(a2+i);
		}
		return( i);
	}
	if( ds == 2){
		for( i = 0; i < size; i += ds){
			*(short *)(a1+i) = *(short *)(a2+i);
		}
		return( i);
	}
	if( ds == 4){
		for( i = 0; i < size; i += ds){
			*(int *)(a1+i) = *(int *)(a2+i);
		}
		return( i);
	}
	if( ds == 8){
		for( i = 0; i < size; i += ds){
			*(long *)(a1+i) = *(long *)(a2+i);
		}
		return( i);
	}
	return( -1);
}

static char cmp_err_db1;
static char cmp_err_db2;
static short cmp_err_ds1;
static short cmp_err_ds2;
static int cmp_err_di1;
static int cmp_err_di2;
static long cmp_err_dl1;
static long cmp_err_dl2;

// -------------------------------------------------------------------------------------

int tst_cpu_cmp( void *a1, void *a2, int size, int ds){
	int i;

	if( ds == 1){
		for( i = 0; i < size; i += ds){
			cmp_err_db1 = *(char *)(a1+i);
			cmp_err_db2 = *(char *)(a2+i);
			if( cmp_err_db1 != cmp_err_db2){
				break;
			}
		}
		return( i);
	}
	if( ds == 2){
		for( i = 0; i < size; i += ds){
			cmp_err_ds1 = *(short *)(a1+i);
			cmp_err_ds2 = *(short *)(a2+i);
			if( cmp_err_ds1 != cmp_err_ds2){
				break;
			}
		}
		return( i);
	}
	if( ds == 4){
		for( i = 0; i < size; i += ds){
			cmp_err_di1 = *(int *)(a1+i);
			cmp_err_di2 = *(int *)(a2+i);
			if( cmp_err_di1 != cmp_err_di2){
				break;
			}
		}
		return( i);
	}
	if( ds == 8){
		for( i = 0; i < size; i += ds){
			cmp_err_dl1 = *(long *)(a1+i);
			cmp_err_dl2 = *(long *)(a2+i);
			if( cmp_err_dl1 != cmp_err_dl2){
				break;
			}
		}
		return( i);
	}
	return( -1);
}

// -------------------------------------------------------------------------------------

int tst_get_cmp_err( void *a1, void *a2, int ds){
	if( ds == 1){
		*(char *)a1 = cmp_err_db1;
		*(char *)a2 = cmp_err_db2;
		return(0);
	}
	if( ds == 2){
		*(short *)a1 = cmp_err_ds1;
		*(short *)a2 = cmp_err_ds2;
		return(0);
	}
	if( ds == 4){
		*(int *)a1 = cmp_err_di1;
		*(int *)a2 = cmp_err_di2;
		return(0);
	}
	if( ds == 8){
		*(long *)a1 = cmp_err_dl1;
		*(long *)a2 = cmp_err_dl2;
		return(0);
	}
	return(-1);
}
