/*=========================< begin file & file header >=======================
 *  References
 *
 *    filename : tdma.h
 *    author   : XP
 *    company  : IOxOS
 *    creation : Mars 15,2018
 *
 *----------------------------------------------------------------------------
 *  Description
 *
 *    This file contain the declarations of all exported functions defined in
 *    tdma.c
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

#ifndef _H_TDMA
#define _H_TDMA

int tdma_init(int quiet);
int tsc_tdma_move(int chan, uint64_t des_addr, uint64_t src_addr, int  size, int mode);
int tsc_tdma_status(int chan);
int tsc_tdma_clear(int chan);
int tsc_tdma(struct cli_cmd_para *c);

#endif /* _H_TDMA */
