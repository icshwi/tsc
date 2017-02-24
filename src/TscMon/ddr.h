/*=========================< begin file & file header >=======================
 *  References
 *
 *    filename : ddr.h
 *    author   : Xavier Paillard
 *    company  : IOxOS
 *    creation : February 2017
 *    version  : 0.0.1
 *
 *----------------------------------------------------------------------------
 *  Description
 *
 *    This file contain the declarations of all exported functions define in
 *    ddr.c
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

#ifndef _H_DDR
#define _H_DDR

int tsc_ddr(struct cli_cmd_para *);
void bin(unsigned n, uint bit_size);
int checksum_sum1(void *buf, int size);
int ddr_idel_conf(void);
int ddr_idel_reset(void);
int ddr_idel_status(void);
int ddr_idel_set(uint dq, uint dqs, char *pm);

#endif /* _H_DDR */
