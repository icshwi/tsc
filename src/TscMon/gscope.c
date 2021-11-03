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
 *  Copyright (C) IOxOS Technologies SA <ioxos@ioxos.ch>
 *  Copyright (C) 2019  European Spallation Source ERIC
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

#define DEBUGno
#include <debug.h>

#include <sys/types.h>
#include <sys/time.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <time.h>
#include <errno.h>
#include <cli.h>
#include <unistd.h>
#include "../../include/tscextlib.h"
#include <tscioctl.h>
#include <tsculib.h>
#include "gscope.h"
#include <gscopelib.h>
#include <gscope3110lib.h>
#include <adc3110lib.h>
#include <fmclib.h>

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
    printf("Generic Scope signature: %08x\n", gscope_identify(tsc_fd));
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
		if( c->cnt < 5)
		{
			printf("gscope init smem needs more arguments\n");
			goto tsc_gscope_init_smem_usage;

		}

		sscanf(c->para[2], "%x", &chan_set);
		sscanf(c->para[3], "%x", &base);
		sscanf(c->para[4], "%x", &size);
		if((gscope_identify(tsc_fd) & 0xffff) == 0x3110)
		{
		  gscope3110_acq_smem_init(tsc_fd, chan_set, base, size, 0);
		}
		if((gscope_identify(tsc_fd) & 0xffff) == 0x3210)
		{
		  gscope3210_acq_smem_init(tsc_fd, chan_set, base, size, 0);
		}
		if((gscope_identify(tsc_fd) & 0xffff) == 0x3117)
		{
		  gscope3117_acq_smem_init(tsc_fd, fmc, base, size, 0);
		}
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
        int prop, acq_mode, acq_trig, sync, mode; //, mode2;

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
			// mode2 = mode | 4;
			if(sync == GSCOPE3110_ARM_CODE_MASTER){
			  printf("arm FMC2 slave : %x\n", mode | 4);
			  gscope3110_acq_arm(tsc_fd, ADC3110_FMC2, mode| 4, prop, 0, 0);
			  printf("arm FMC1 master : %x\n", mode & ~4);
			  gscope3110_acq_arm(tsc_fd, ADC3110_FMC1, mode & ~4, prop, 0, 0);
			}
			else{
			  printf("arm FMC1 slave : %x\n", mode | 4);
			  gscope3110_acq_arm(tsc_fd, ADC3110_FMC1, mode | 4, prop, 0, 0);
			  printf("arm FMC2 master : %x\n", mode & ~4);
			  gscope3110_acq_arm(tsc_fd, ADC3110_FMC2, mode & ~4, prop, 0, 0);
			}


        }else{
            gscope3110_acq_arm(tsc_fd, fmc, mode, prop, 0, 0);

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
            gscope_trig_dis(tsc_fd, fmc);
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

	        if((gscope_identify(tsc_fd) & 0xffff) == 0x3110)
	        {
                if(chan < 0 || chan > 7){
                    printf("that channel does not exist\n");
                    return(-1);
                }
	        }
            if((gscope_identify(tsc_fd) & 0xffff) == 0x3210)
            {
 	            if(chan < 0 || chan > 3){
	                printf("that channel does not exist\n");
	                return(-1);
	            }
	        }
            if((gscope_identify(tsc_fd) & 0xffff) == 0x3117)
                {
 	            if(chan < 0 || chan > 19){
	                printf("that channel does not exist\n");
	                return(-1);
	            }
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
        gscope3110_trig_set(tsc_fd, fmc, chan, mode, offset, hyst);

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
 * Function name : tsc_gscope_trig
 * Prototype     : int
 * Parameters    : cli command parameter structure
 * Return        : success/error
 *----------------------------------------------------------------------------
 * Description   : adc3110 command
 *
 *++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

int 
tsc_gscope_trig( struct cli_cmd_para *c)
{
	int fmc;
	char *p;
	int mode, offset, main_mode, lvl_edge, direct, sign, hyst, chan, base;

	if( c->cnt < 2)
	{
		printf("gscope trig command needs more arguments\n");
		goto tsc_gscope_trig_usage;
	}


		offset = 0;
		hyst = 0;
		base = 0;
		fmc = ADC3110_FMC1;
		main_mode = GSCOPE3110_TRIG_CODE_MAN;
		lvl_edge = GSCOPE3110_TRIG_CODE_LVL;
		direct = GSCOPE3110_TRIG_CODE_UP;
		sign = GSCOPE3110_TRIG_CODE_SIGN;

		if(c->cnt < 2){
			printf("gscope acq trig command needs more arguments\n");
			goto tsc_gscope_trig_usage;
		}

		if( c->ext){
			fmc = strtoul( c->ext, &p, 16);
		}
		else{
			goto tsc_gscope_trig_usage;
		}

		if(fmc < ADC3110_FMC1 || fmc > ADC3110_FMC2){
			printf("Bad FMC index\n");
			return(-1);
		}

		if(!strncmp( "dis", c->para[1], 3)){
			gscope_trig_dis(tsc_fd, fmc);
			return 0;
		}

		if( !strncmp( "man", c->para[1], 3))
		{
			main_mode = GSCOPE3110_TRIG_CODE_MAN;
		}
		else if(!strncmp( "gpio", c->para[1], 3))
		{
			main_mode = GSCOPE3110_TRIG_CODE_GPIO;
		}
		else if(!strncmp( "adc", c->para[1], 3))
		{

			if(c->cnt < 5){
				printf("gscope acq trig adc command needs more arguments\n");
				goto tsc_gscope_trig_usage;
			}


			if(sscanf(&c->para[1][4], "%d.%d", &offset, &hyst) < 2){
			  printf("Bad parameter : %s\n", c->para[1]);
				goto tsc_gscope_trig_usage;
			}

			chan = 0;
			if(sscanf(c->para[2], "%d", &chan) < 1){
			  printf("Bad parameter : %s\n", c->para[2]);
				goto tsc_gscope_trig_usage;
			}

		        if((gscope_identify(tsc_fd) & 0xffff) == 0x3110)
		        {
		 	  if(chan < 0 || chan > 7){
				printf("that channel does not exist\n");
				return(-1);
			  }
			}
		        if((gscope_identify(tsc_fd) & 0xffff) == 0x3210)
		        {
		 	  if(chan < 0 || chan > 3){
				printf("that channel does not exist\n");
				return(-1);
			  }
			  base = chan << 14;
			  if((offset > 0x3fff) || (offset < 0)) 
			  {
				printf("Bad offset parameter\n");
				return(-1);
			  }
			  if((hyst > 0x3fff) || (hyst < 0)) 
			  {
				printf("Bad hysteresis parameter\n");
				return(-1);
			  }
			}
		        if((gscope_identify(tsc_fd) & 0xffff) == 0x3117)
		        {
		 	  if(chan < 0 || chan > 19){
				printf("that channel does not exist\n");
				return(-1);
			  }
			}

			main_mode = GSCOPE3110_TRIG_CODE_ADC;
			if(!strncmp( "lvl", c->para[3], 3))
			{
				lvl_edge = GSCOPE3110_TRIG_CODE_LVL;
			} 
			else if(!strncmp( "edge", c->para[3], 3))
			{
				lvl_edge = GSCOPE3110_TRIG_CODE_EDGE;
			}
			else
			{
			  printf("Bad parameter : %s\n", c->para[3]);
				goto tsc_gscope_trig_usage;
			}


			if(!strncmp( "up", c->para[4], 2))
			{
				direct = GSCOPE3110_TRIG_CODE_UP;
			} 

			else if(!strncmp( "down", c->para[4], 3))
			{
				direct = GSCOPE3110_TRIG_CODE_DOWN;
			}
			else{
			  printf("Bad parameter : %s\n", c->para[4]);
				goto tsc_gscope_trig_usage;
			}

			if(c->cnt > 5){

				if(!strncmp( "unsigned", c->para[5], 3))
				{
					sign = GSCOPE3110_TRIG_CODE_UNSIGN;
				}
				else if(!strncmp( "signed", c->para[5], 3))
				{
					sign = GSCOPE3110_TRIG_CODE_SIGN;
				}
				else
				{
			  printf("Bad parameter : %s\n", c->para[5]);
					goto tsc_gscope_trig_usage;
				}
			}

		} 

		else
		{
			goto tsc_gscope_trig_usage;
		}

		mode = main_mode | lvl_edge | direct | sign;
		gscope3110_trig_set(tsc_fd, fmc, chan, mode, base+offset, base+hyst);

		return(0);
tsc_gscope_trig_usage:
	printf("usage: gscope.<x> trig dis\n");
	printf("       gscope.<x> trig man\n");
	printf("       gscope.<x> trig auto\n");
	printf("       gscope.<x> trig gpio\n");
	printf("       gscope.<x> trig adc.<offset>.<hyst> <chan> <type> <dir> [<sign>]\n");
	printf("       where offset   = the level the signal needs to exceed to trigger the acquisition\n");
	printf("             chan     = the channel which is going to trigger the acquisition\n");
	printf("             hyst     = hysteresis value\n");
	printf("             type     = lvl or edge\n");
	printf("             dir      = above/below the level or rising/falling edge (up/down)\n");
	printf("             sign     = signed or unsigned offset (signed per default)\n");
  return (-1);
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
  
    int new_size, chanset, i, buf_mode, fmc, mask;
    char *new_filename, *p;

    fmc = ADC3110_FMC1;
	if(c->cnt < 4){
		goto tsc_gscope_save_usage;
	}

	if( c->ext)
	{
	  fmc = strtoul( c->ext, &p, 16);
	}
	else 
	{
	    goto tsc_gscope_save_usage;
	}
	if( !strncmp( "smem", c->para[1], 2))
	{
	  buf_mode = GSCOPE3110_ACQ_MODE_SMEM;
	}
	else if( !strncmp( "dpram", c->para[1], 2))
	{
	  buf_mode = GSCOPE3110_ACQ_MODE_DPRAM;
	}
	else
	{
		goto tsc_gscope_save_usage;
	}
	if(sscanf( c->para[2],"%x", &chanset) < 1){
		goto tsc_gscope_save_usage;
	}

	if((gscope_identify(tsc_fd) & 0xffff) == 0x3110)
	{
  	  if(chanset < 0 || chanset > 0xFF){
		printf("that chanset does not exist\n");
		return (-1);
	  }
	}
	if((gscope_identify(tsc_fd) & 0xffff) == 0x3210)
	{
  	  if(chanset < 0 || chanset > 0xF){
		printf("that chanset does not exist\n");
		return (-1);
	  }
	}
	if((gscope_identify(tsc_fd) & 0xffff) == 0x3117)
	{
  	  if(chanset < 0 || chanset > 0xFFFFF){
		printf("that chanset does not exist\n");
		return (-1);
	  }
	}

	new_size = 0;
	if(c->cnt >= 5){
		sscanf( c->para[4],"%x", &new_size);
	}

	mask = 0xffff;
	if(c->cnt >= 6){
		sscanf( c->para[5],"%x", &mask);
	}

	i = 0;
	while(chanset != 0){
		if((chanset & 1) == 1){
			new_filename = gscope3110_filename_generator(c->para[3], i);
			if( buf_mode == GSCOPE3110_ACQ_MODE_SMEM)
			{
                if((gscope_identify(tsc_fd) & 0xffff) == 0x3110)
		        {
			        gscope3110_smem_save(tsc_fd, i, new_filename, 1, new_size, mask);
			    }
		        if((gscope_identify(tsc_fd) & 0xffff) == 0x3210)
		        {
			        gscope3210_smem_save(tsc_fd, i, new_filename, 1, new_size, mask);
			    }
		        if((gscope_identify(tsc_fd) & 0xffff) == 0x3117)
		        {
			        gscope3117_smem_save(tsc_fd, fmc, i, new_filename, 1, new_size, mask);
			    }
			}
			if( buf_mode == GSCOPE3110_ACQ_MODE_DPRAM)
			{
			    gscope3110_dpram_save(tsc_fd, fmc, i, new_filename, 1, new_size);
			}
		}
		chanset = chanset >> 1;
		i++;
	}
	return 0;


tsc_gscope_save_usage:
	printf("usage: gscope.<fmc> save <buf> <chan> <filename> [<size>] [<mask>]\n");
	printf("       where buf      = smem or dpram\n");
	printf("             chan     = channel\n");
	printf("             filename = histogram filename\n");
	printf("             size     = size of the file\n");
	printf("             mask     = data mask\n");
	return(-1);
}

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * Function name : tsc_gscope_fwinfo_read
 * Prototype     : int
 * Parameters    : char containing address to the hash register
 *               : pointer the the data buffer
 * Return        : success/error
 *----------------------------------------------------------------------------
 * Description   : Helper function for tsc_gscope_fwinfo.
 *                 Reads the 8-character short-hash from the USER register
 *                 by performing 4 individual byte reads, and formatting the
 *                 result as a 32-bit hex word.
 *
 *++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

int tsc_gscope_fwinfo_read(char addr, int *hash) {

    char val = 0;
    int retval;
    uint i = 0;
    *hash = 0;
    /* Compose hash via 4x byte reads */
    for (i=0; i<4; i++) {
        retval = tsc_usr_read(tsc_fd, addr+i, &val, 0, 1, 0, 1);
        if (retval < 0) {
            printf("Read from USER register 0x%2x failed!\n", addr);
            return retval;
        }
        *hash += ((unsigned char)val << (i*8));
    }

    return retval;
}

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * Function name : tsc_gscope_fwinfo_tosca
 * Prototype     : int
 * Parameters    : char containing address to tosca hash register
 * Return        : success/error
 *----------------------------------------------------------------------------
 * Description   : Helper function for tsc_gscope_fwinfo.
 *                 Gets the short-hash value for the tosca repository, and
 *                 prints to the console.
 *
 *++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

int tsc_gscope_fwinfo_tosca(char addr) {

    int hash = 0, retval;

    retval = tsc_gscope_fwinfo_read(addr, &hash);
    if (retval < 0 ) { return retval; }

    printf("tosca2b\t\t\trepository hash (0x%2x): %8x\n", addr, hash);

    return 0;
}

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * Function name : tsc_gscope_fwinfo_xapp
 * Prototype     : int
 * Parameters    : char containing address to xuser_app  hash register
 * Return        : success/error
 *----------------------------------------------------------------------------
 * Description   : Helper function for tsc_gscope_fwinfo.
 *                 Gets the short-hash value for the xuser_app repository, and
 *                 prints to the console.
 *
 *++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

int tsc_gscope_fwinfo_xapp(char addr) {

    int hash = 0, retval;

    retval = tsc_gscope_fwinfo_read(addr, &hash);
    if (retval < 0 ) { return retval; }

    printf("xuser_scope_generic\trepository hash (0x%2x): %8x\n", addr, hash);

    return 0;
}

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * Function name : tsc_gscope_fwinfo_fmc
 * Prototype     : int
 * Parameters    : string containing fmc number {"1", "2"}
 : char containing address to fmc hash register
 * Return        : success/error
 *----------------------------------------------------------------------------
 * Description   : Helper function for tsc_gscope_fwinfo.
 *                 Gets the short-hash value for the relevant FMC number
 *                 repository, and prints to the console.
 *
 *++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

int tsc_gscope_fwinfo_fmc(char *fmc, char addr) {

    const int fmcSign = (int)FMC1_CSR_BASE;
    int hash = 0, id, retval, fmcType = 0;

    id = tscext_csr_rd(tsc_fd, fmcSign);
    if((id & 0xffff0000) == 0x31170000) {
        fmcType = 7;
    }
    retval = tsc_gscope_fwinfo_read(addr, &hash);
    if (retval < 0 ) { return retval; }

    printf("adc_311%d_fdk (fmc%s)\trepository hash (0x%2x): %8x\n", fmcType, fmc, addr, hash);

    return 0;
}

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * Function name : tsc_gscope_fwinfo
 * Prototype     : int
 * Parameters    : cli command parameter structure
 * Return        : success/error
 *----------------------------------------------------------------------------
 * Description   : Get Firmware info about the bitstream that is loaded
 *                 to the FPGA in the form of 8-character short-hashes from
 *                 corresponding GIT repositories.
 *
 *++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

int tsc_gscope_fwinfo(struct cli_cmd_para *c)
{
    /* Define register addresses */
    const char toscaAddr = 0x20;
    const char xappAddr = 0x24;
    char fmcAddr = 0x34;         // default to address of FMC 1
    const char fmc2Addr = 0x44;

    int retval;
    char *fmc;

    /* Function needs atleast 2 input arguments */
    /* tscmon> gscope fwinfo tosca */
    /*           0       1     2   */
    if(c->cnt < 2) {
        goto tsc_gscope_fwinfo_usage;
    }

    if(!strncmp("tosca", c->para[1], 5)) {
        retval = tsc_gscope_fwinfo_tosca(toscaAddr);
        return retval;
    }
    if(!strncmp("xapp", c->para[1], 4)) {
        retval = tsc_gscope_fwinfo_xapp(xappAddr);
        return retval;
    }
    if(!strncmp("fmc", c->para[1], 3)) {
        /* Function needs atleast 3 input arguments */
        /* tscmon> gscope fwinfo fmc 1 */
        /*           0       1    2  3 */
        if(c->cnt < 3) {
            printf("gscope fwinfo fmc command needs more arguments\n");
            printf("usage: gscope fwinfo fmc <#>, where # is {1, 2}\n");
            return(-1);
        }
        fmc = c->para[2];
        if(!strncmp("2", fmc, 1))
            fmcAddr = fmc2Addr;

        retval = tsc_gscope_fwinfo_fmc(fmc, fmcAddr);
        return retval;
    }
    if(!strncmp("all", c->para[1], 3)) {
        retval = tsc_gscope_fwinfo_tosca(toscaAddr);
        if (retval < 0 ) { return retval; }
        retval = tsc_gscope_fwinfo_xapp(xappAddr);
        if (retval < 0 ) { return retval; }
        retval = tsc_gscope_fwinfo_fmc("1", fmcAddr);
        if (retval < 0 ) { return retval; }
        retval = tsc_gscope_fwinfo_fmc("2", fmc2Addr);
        return retval;
    }

tsc_gscope_fwinfo_usage:
    printf("usage: gscope fwinfo <reg> [<n>]\n");
    printf("       where reg = \n");
    printf("           tosca   =  short-hash from toasca2b git repository\n");
    printf("           xapp    =  short-hash from xuser_scope_generic git repository\n");
    printf("           fmc <n> =  short-hash from the adc_311<x>_fdk git repository\n");
    printf("                      for fmc identifier <n-{1,2}>\n");
    printf("           all     =  all of the above\n");
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
	if( !strncmp( "trig", c->para[0], 2))
	{
		return(tsc_gscope_trig(c));
	}
	if( !strncmp( "save", c->para[0], 2))
	{
		return(tsc_gscope_save(c));
	}
    if( !strncmp( "fwinfo", c->para[0], 2)) {
        return(tsc_gscope_fwinfo(c));
    }
	printf("gscope operation not supported\n");
	return(-1);
}
