/*=========================< begin file & file header >=======================
 *  References
 *  
 *    filename : pev791xlib.h
 *    author   : JFG
 *    company  : IOxOS
 *    creation : june 30,2008
 *    version  : 0.0.1
 *
 *----------------------------------------------------------------------------
 *  Description
 *
 *    This file contain the declarations of all exported functions define in
 *    pev791xlib.c
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
 *
 *=============================< end file header >============================*/
#ifndef _H_PEV791XLIB
#define _H_PEV791XLIB

float pev791x_bmr_conv_11bit_u( unsigned short);
float pev791x_bmr_conv_11bit_s( unsigned short);
float pev791x_bmr_conv_16bit_u( unsigned short);
int pev791x_pex_write( uint, uint);
int pev791x_bmr_read( uint, uint, uint *, uint);
int pev791x_bmr_write( uint, uint, uint, uint);

#endif /*  _H_PEV791XLIB */
