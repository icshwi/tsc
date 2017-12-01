/*=========================< begin file & file header >=======================
 *  References
 *
 *    filename : tstulib.h
 *    author   : JFG, XP
 *    company  : IOxOS
 *    creation : june 30,2008
 *
 *----------------------------------------------------------------------------
 *  Description
 *
 *    This file contain the declarations of all functions used by the tsct
 *    test libraty.
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

int tst_cpu_fill( void *a, int size, int mode, int p1, int p2);
void * tst_cpu_check( void *a, int size, int mode, int p1, int p2);
int tst_cpu_copy( void *a1, void *a2, int size, int ds);
int tst_cpu_cmp( void *a1, void *a2, int size, int ds);
int tst_get_cmp_err( void *a1, void *a2, int ds);
