/*=========================< begin file & file header >=======================
 *  References
 *  
 *    filename : gscope.c
 *    author   : JFG, XP
 *    company  : IOxOS
 *    creation : July 1,2019
 *
 *----------------------------------------------------------------------------
 *  Description
 *
 *    That library contains a set of function to access the TOSCA II XUSER
 *    GENERIC SCOPE data acquisition logic.
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

#define DEBUG
#include <debug.h>

#include <sys/types.h>
#include <sys/time.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <time.h>
#include <cli.h>
#include <unistd.h>
#include "../../include/tscextlib.h"
#include <tscioctl.h>
#include <tsculib.h>
#include "gscope.h"
#include <gscope3110lib.h>
#include <adc3110lib.h>

extern int tsc_fd;

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * Function name : tsc_gscope_identify
 * Prototype     : int
 * Parameters    : cli command parameter structure
 * Return        : success/error
 *----------------------------------------------------------------------------
 * Description   : adc3110 command
 *
 *++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

int 
tsc_gscope_identify(struct cli_cmd_para *c)
{
	//printf("In tsc_gscope_identify()\n");
	printf("Generic Scope signature: %08x\n", gscope3110_identify(tsc_fd));
	return(0);
}

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * Function name : tsc_gscope_init
 * Prototype     : int
 * Parameters    : cli command parameter structure
 * Return        : success/error
 *----------------------------------------------------------------------------
 * Description   : adc3110 command
 *
 *++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

int 
tsc_gscope_init(struct cli_cmd_para *c)
{
	char *p;
	int chan_set, base, size;

	if( c->cnt < 2)
	{
		printf("gscope init command needs more arguments\n");
		goto tsc_gscope_init_usage;
	}
	if( !strncmp( "dpram", c->para[1], 3))
	{
		int fmc;

		fmc = ADC3110_FMC1;
		if( c->ext) 
		{
			fmc = strtoul( c->ext, &p, 16);
			if(( fmc < ADC3110_FMC1) || ( fmc > ADC3110_FMC2))
			{
				printf("bad FMC index : %d\n", fmc);
				return( -1);
			}
		}
		gscope3110_acq_dpram_init(tsc_fd, fmc, 0xff, 0x8000);
		return(0);
	}
	if( !strncmp( "smem", c->para[1], 3))
	{
		if( c->cnt < 5)
		{
			printf("gscope init smem needs more arguments\n");
			goto tsc_gscope_init_smem_usage;

		}

		sscanf(c->para[2], "%x", &chan_set);
		sscanf(c->para[3], "%x", &base);
		sscanf(c->para[4], "%x", &size);

		gscope3110_acq_smem_init(tsc_fd, chan_set, base, size, 0);
		return(0);
	}

	tsc_gscope_init_smem_usage:
	printf("usage: gscope init smem <chanset> <base> <size>\n");
	printf("       where chanset  = list of enabled channel\n");
	printf("             base     = first buffer base adress\n");
	printf("             size     = size of the buffers\n");
	return (-1);

	tsc_gscope_init_usage:
	printf("usage: gscope init <mode>\n");
	printf("       where mode = dpram or smem\n");
	return(-1);
}

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * Function name : tsc_gscope_acq
 * Prototype     : int
 * Parameters    : cli command parameter structure
 * Return        : success/error
 *----------------------------------------------------------------------------
 * Description   : adc3110 command
 *
 *++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

int 
tsc_gscope_acq(struct cli_cmd_para *c)
{
	int fmc;
	char *p;

	if( c->cnt < 2)
	{
		printf("gscope acq command needs more arguments\n");
		goto tsc_gscope_acq_usage;
	}
	if( !strncmp( "arm", c->para[1], 3))
	{
		int prop, acq_mode, acq_trig, sync, mode, mode2;

		fmc = ADC3110_FMC1;
		prop = 4;
		acq_mode = GSCOPE3110_ARM_CODE_SINGLE;
		acq_trig = GSCOPE3110_ARM_CODE_NORMAL;
		sync = GSCOPE3110_ARM_CODE_MASTER;

		if( c->ext){
			fmc = strtoul( c->ext, &p, 16);
		}
		else{
			goto tsc_gscope_acq_arm_usage;
		}

		if(fmc < ADC3110_FMC1 || fmc > GSCOPE3110_MIXED_FMC){
			printf("Bad FMC index\n");
			return(-1);
		}

		if(c->cnt > 2){
			if(sscanf(c->para[2], "%x", &prop) < 1){
				goto tsc_gscope_acq_arm_usage; 
			}
			if(prop >= GSCOPE3110_ARM_TRIG_POS_MAX || prop < 0){
				printf("Bad pre-trigger position\n");
				return(-1);
			}
		}

		if(c->cnt > 3){
			if( !strncmp( "master", c->para[3], 4)){
				sync = GSCOPE3110_ARM_CODE_MASTER;
			}
			else if( !strncmp( "slave", c->para[3], 4)){
				sync = GSCOPE3110_ARM_CODE_SLAVE;
			}
			else{
				goto tsc_gscope_acq_arm_usage;
			}
		}

		if(c->cnt > 4){
			if( !strncmp( "cont", c->para[4], 4)){
				acq_mode = GSCOPE3110_ARM_CODE_CONT;
			}
			else if( !strncmp( "sgl", c->para[4], 4)){
				acq_mode = GSCOPE3110_ARM_CODE_SINGLE;
			}
			else{
				goto tsc_gscope_acq_arm_usage;
			}
		}

		if(c->cnt > 5){
			if( !strncmp( "normal", c->para[5], 4)){
				acq_trig = GSCOPE3110_ARM_CODE_NORMAL;
			}
			else if( !strncmp( "auto", c->para[5], 4)){
				acq_trig = GSCOPE3110_ARM_CODE_AUTO;
			}
			else{
				goto tsc_gscope_acq_arm_usage;
			}
		}

		mode = acq_mode | acq_trig | sync;

		if(fmc == GSCOPE3110_MIXED_FMC){
			mode2 = mode ^ 4;
			if(sync == GSCOPE3110_ARM_CODE_MASTER){
				gscope3110_acq_arm(tsc_fd, ADC3110_FMC2, mode2, prop, 0);
				gscope3110_acq_arm(tsc_fd, ADC3110_FMC1, mode, prop, 0);
			}
			else{
				gscope3110_acq_arm(tsc_fd, ADC3110_FMC1, mode, prop, 0);
				gscope3110_acq_arm(tsc_fd, ADC3110_FMC2, mode2, prop, 0);
			}


		}else{
			gscope3110_acq_arm(tsc_fd, fmc, mode, prop, 0);

		}

		return(0);

		tsc_gscope_acq_arm_usage:
		printf("usage: gscope.<x> acq arm [<prop> <sync> <mode> <trig>]\n");
		printf("       x        = fmc identifier (1 or 2) or 3 for a double arm\n");
		printf("       prop     = pre trigger proportion of data (out of 8, 4 per default)\n");
		printf("       sync     = master or slave (master per default)\n");
		printf("                  if <x> is 3, fmc 1 will be the <sync> and fmc 2 the opposite\n");
		printf("       mode     = continuous (cont) or single (sgl) acquisition (sgl per default)\n");
		printf("       trig     = trigger mode: normal or auto (normal per default)\n");
		return (-1);

	}

	if( !strncmp( "abo", c->para[1], 3))
	{
		if( c->ext){
					fmc = strtoul( c->ext, &p, 16);
				}
				else{
					goto tsc_gscope_acq_abo_usage;
				}

		if(fmc < ADC3110_FMC1 || fmc > GSCOPE3110_MIXED_FMC){
			printf("Bad FMC index\n");
			return(-1);
		}
		if(fmc >= ADC3110_FMC2){
			gscope3110_acq_abort(tsc_fd, ADC3110_FMC2);
		}
		if((fmc % 2) == ADC3110_FMC1){
			gscope3110_acq_abort(tsc_fd, ADC3110_FMC1);
		}
		return(0);

		tsc_gscope_acq_abo_usage:
		printf("usage: gscope.<x> acq abo\n");
		printf("      <x> = fmc identifier (1 or 2) or 3 for the two at the same time\n");
	}
	if( !strncmp( "trig", c->para[1], 3))
	{

		int mode, offset, main_mode, lvl_edge, direct, sign, hyst, chan;

		offset = 0;
		hyst = 0;
		fmc = ADC3110_FMC1;
		main_mode = GSCOPE3110_TRIG_CODE_MAN;
		lvl_edge = GSCOPE3110_TRIG_CODE_LVL;
		direct = GSCOPE3110_TRIG_CODE_UP;
		sign = GSCOPE3110_TRIG_CODE_SIGN;

		if(c->cnt < 3){
			printf("gscope acq trig command needs more arguments\n");
			goto tsc_gscope_acq_trig_usage;
		}

		if( c->ext){
			fmc = strtoul( c->ext, &p, 16);
		}
		else{
			goto tsc_gscope_acq_trig_usage;
		}

		if(fmc < ADC3110_FMC1 || fmc > ADC3110_FMC2){
			printf("Bad FMC index\n");
			return(-1);
		}

		if(!strncmp( "dis", c->para[2], 3)){
			if( c->ext){
				fmc = strtoul( c->ext, &p, 16);
			}
			else{
				goto tsc_gscope_acq_trig_usage;
			}
			gscope_acq_dis(tsc_fd, fmc);
			return 0;
		}

		if( !strncmp( "man", c->para[2], 3))
		{
			main_mode = GSCOPE3110_TRIG_CODE_MAN;
		}
		else if(!strncmp( "gpio", c->para[2], 3))
		{
			main_mode = GSCOPE3110_TRIG_CODE_GPIO;
		}
		else if(!strncmp( "adc", c->para[2], 3))
		{

			if(c->cnt < 6){
				printf("gscope acq trig adc command needs more arguments\n");
				goto tsc_gscope_acq_trig_adc_usage;
			}


			if(sscanf(&c->para[2][4], "%d.%d", &offset, &hyst) < 2){
				goto tsc_gscope_acq_trig_adc_usage;
			}

			chan = 0;
			if(sscanf(c->para[3], "%d", &chan) < 1){
				goto tsc_gscope_acq_trig_adc_usage;
			}

			if(chan < 0 || chan > 7){
				printf("that channel does not exist\n");
				return(-1);
			}

			main_mode = GSCOPE3110_TRIG_CODE_ADC;
			if(!strncmp( "lvl", c->para[4], 3))
			{
				lvl_edge = GSCOPE3110_TRIG_CODE_LVL;
			} 
			else if(!strncmp( "edge", c->para[4], 3))
			{
				lvl_edge = GSCOPE3110_TRIG_CODE_EDGE;
			}
			else
			{
				goto tsc_gscope_acq_trig_adc_usage;
			}


			if(!strncmp( "up", c->para[5], 2))
			{
				direct = GSCOPE3110_TRIG_CODE_UP;
			} 

			else if(!strncmp( "down", c->para[5], 3))
			{
				direct = GSCOPE3110_TRIG_CODE_DOWN;
			}
			else{
				goto tsc_gscope_acq_trig_adc_usage;
			}

			if(c->cnt > 6){

				if(!strncmp( "unsigned", c->para[6], 3))
				{
					sign = GSCOPE3110_TRIG_CODE_UNSIGN;
				}
				else if(!strncmp( "signed", c->para[6], 3))
				{
					sign = GSCOPE3110_TRIG_CODE_SIGN;
				}
				else
				{
					goto tsc_gscope_acq_trig_adc_usage;
				}
			}

		} 

		else
		{
			goto tsc_gscope_acq_trig_usage;
		}

		mode = main_mode | lvl_edge | direct | sign;
		gscope3110_acq_trig(tsc_fd, fmc, chan, mode, offset, hyst);

		return(0);
	}

	if( !strncmp( "status", c->para[1], 3))
	{
		int sts1, sts2;
		sts1 = gscope3110_acq_status(tsc_fd, 1);
		sts2 = gscope3110_acq_status(tsc_fd,  2);
		printf("acquisition status: %08x - %08x\n", sts1, sts2);
		return( 0);
	}

	tsc_gscope_acq_trig_usage:
	printf("usage: gscope.<x> acq trig <mode>\n");
	printf("       where mode = man, gpio, adc, dis\n");
	printf("             x    = fmc identifier (1 or 2)\n");
	return (-1);

	tsc_gscope_acq_trig_adc_usage:
	printf("usage: gscope.<x> acq trig adc.<offset>.<hyst> <chan> <type> <dir> [<sign>]\n");
	printf("       where offset   = the level the signal needs to exceed to trigger the acquisition\n");
	printf("             chan     = the channel which is going to trigger the acquisition\n");
	printf("             hyst     = hysteresis value\n");
	printf("             type     = lvl or edge\n");
	printf("             dir      = above/below the level or rising/falling edge (up/down)\n");
	printf("             sign     = signed or unsigned offset (signed per default)\n");
	return (-1);

	tsc_gscope_acq_usage:
	printf("usage: gscope acq <op>\n");
	printf("       where op = arm, trig, abort or status\n");
	return(-1);
}


/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * Function name : tsc_gscope_save
 * Prototype     : int
 * Parameters    : cli command parameter structure
 * Return        : success/error
 *----------------------------------------------------------------------------
 * Description   : adc3110 command
 *
 *++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

int tsc_gscope_save(struct cli_cmd_para *c){

	int new_size, chanset, i;
	char *new_filename;

	if(c->cnt < 3){
		goto tsc_gscope_save_usage;
	}

	if(sscanf( c->para[1],"%x", &chanset) < 1){
		goto tsc_gscope_save_usage;
	}

	if(chanset < 0 || chanset > 0xFF){
		printf("that chanset does not exist\n");
		return (-1);
	}

	new_size = 0;
	if(c->cnt == 4){
		sscanf( c->para[3],"%x", &new_size);
	}

	i = 0;
	while(chanset != 0){
		if((chanset & 1) == 1){
			new_filename = gscope3110_filename_generator(c->para[2], i);
			gscope3110_save(tsc_fd, i, new_filename, 1, new_size);
		}
		chanset = chanset >> 1;
		i++;
	}
	return 0;


	tsc_gscope_save_usage:
	printf("usage: gscope save <chan> <filename> [<size>]\n");
	printf("       where chan     = channel\n");
	printf("             filename = histogram filename\n");
	printf("             size     = size of the file\n");
	return(-1);
}


/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * Function name : tsc_gscope
 * Prototype     : int
 * Parameters    : cli command parameter structure
 * Return        : success/error
 *----------------------------------------------------------------------------
 * Description   : adc3110 command
 *
 *++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

int 
tsc_gscope(struct cli_cmd_para *c)
{
	if( c->cnt < 1)
	{
		printf("gscope command needs more arguments\n");
		printf("usage: gscope.<fmc> <op> [<para>] ..\n");
		return(-1);
	}
	if( !strncmp( "identify", c->para[0], 2))
	{
		return(tsc_gscope_identify(c));
	}
	if( !strncmp( "init", c->para[0], 2))
	{
		return(tsc_gscope_init(c));
	}
	if( !strncmp( "acq", c->para[0], 2))
	{
		return(tsc_gscope_acq(c));
	}
	if( !strncmp( "save", c->para[0], 2))
	{
		return(tsc_gscope_save(c));
	}
	printf("gscope operation not supported\n");
	return(-1);
}
