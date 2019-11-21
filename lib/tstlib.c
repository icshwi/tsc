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

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * Function name : tst_get_range
 * Prototype     : int
 * Parameters    : parameter, start of range, stop of range
 * Return        : first
 *----------------------------------------------------------------------------
 * Description   : get range of test
 *
 *++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

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

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * Function name : tst_cpu_fill
 * Prototype     : int
 * Parameters    : a, size, mode, base, increment
 * Return        : 0
 *----------------------------------------------------------------------------
 * Description   : fill data with a specific increment
 *
 *++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

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

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * Function name : tst_cpu_check
 * Prototype     : void
 * Parameters    : data, size, mode, base, increment
 * Return        : null or offset error
 *----------------------------------------------------------------------------
 * Description   : compare data with specific increment value
 *
 *++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

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

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * Function name : tst_cpu_copy
 * Prototype     : int
 * Parameters    : source, destination, size, data size
 * Return        : error/success
 *----------------------------------------------------------------------------
 * Description   : copy data
 *
 *++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

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

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * Function name : tst_cpu_cmp
 * Prototype     : int
 * Parameters    : a1, a2, size, ds
 * Return        : error/success
 *----------------------------------------------------------------------------
 * Description   : compare 2 values
 *
 *++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

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

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * Function name : tst_get_cmp_err
 * Prototype     : int
 * Parameters    : a1, a2, ds
 * Return        : error/success
 *----------------------------------------------------------------------------
 * Description   : compare 2 values
 *
 *++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

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
