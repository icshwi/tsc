/*=========================< begin file & file header >=======================
 *  References
 *  
 *    filename : amc.c
 *    author   : RH
 *    company  : IOxOS
 *    creation : September 13,2019
 *
 *----------------------------------------------------------------------------
 *  Description
 *
 *    That file contains the functions to act on a AMC.
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
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <cli.h>
#include <tscioctl.h>
#include <tsculib.h>
#include <aio.h>
#include <errno.h>
#include <ctype.h>
#include <time.h>
#include "rtm.h"
#include "ponmboxlib.h"
#include "mtca4amclib.h"
#include "TscMon.h"

extern int tsc_fd;

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * Function name : amc_rtm_clk_in
 * Prototype     : int
 * Parameters    : pointer to command parameter list
 * Return        : int
 *
 *----------------------------------------------------------------------------
 * Description   : rtm clk_in control
 *
 *++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

int amc_rtm_clk_in(struct cli_cmd_para *c){
	mtca4_rtm_clk_in_enable_t state;

	if ((c->cnt != 2)){
		tsc_print_usage(c);
		return(CLI_ERR);
	}

	if (!strcasecmp(c->para[1], "on")){
		state = MTCA4_RTM_CLK_IN_ENABLED;
	}
	else if (!strcasecmp(c->para[1], "off")){
		state = MTCA4_RTM_CLK_IN_DISABLED;
	}
	else{
		tsc_print_usage(c);
		return(CLI_ERR);
	}

	set_mtca4_rtm_clk_in_enable(tsc_fd, state);

	return CLI_OK;
}

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * Function name : amc_rtm_tclk_in
 * Prototype     : int
 * Parameters    : pointer to command parameter list
 * Return        : int
 *
 *----------------------------------------------------------------------------
 * Description   : rtm tclk_in control
 *
 *++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

int amc_rtm_tclk_in(struct cli_cmd_para *c){
	mtca4_rtm_tclk_in_enable_t state;

	if ((c->cnt != 2)){
		tsc_print_usage(c);
		return(CLI_ERR);
	}

	if (!strcasecmp(c->para[1], "on")){
		state = MTCA4_RTM_TCLK_IN_ENABLED;
	}
	else if (!strcasecmp(c->para[1], "off")){
		state = MTCA4_RTM_TCLK_IN_DISABLED;
	}
	else{
		tsc_print_usage(c);
		return(CLI_ERR);
	}

	set_mtca4_rtm_tclk_in_enable(tsc_fd, state);

	return CLI_OK;
}

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * Function name : amc_rtm_clk_out
 * Prototype     : int
 * Parameters    : pointer to command parameter list
 * Return        : int
 *
 *----------------------------------------------------------------------------
 * Description   : rtm clk_out control
 *
 *++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

int amc_rtm_clk_out(struct cli_cmd_para *c){
	mtca4_rtm_clk_out_enable_t state;

	if ((c->cnt != 2)){
		tsc_print_usage(c);
		return(CLI_ERR);
	}

	if (!strcasecmp(c->para[1], "on")){
		state = MTCA4_RTM_CLK_OUT_ENABLED;
	}
	else if (!strcasecmp(c->para[1], "off")){
		state = MTCA4_RTM_CLK_OUT_DISABLED;
	}
	else{
		tsc_print_usage(c);
		return(CLI_ERR);
	}

	set_mtca4_rtm_clk_out_enable(tsc_fd, state);

	return CLI_OK;
}

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * Function name : tsc_amc
 * Prototype     : int
 * Parameters    : pointer to command parameter list
 * Return        : int
 *
 *----------------------------------------------------------------------------
 * Description   : amc operations
 *
 *++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

int tsc_amc(int fd, struct cli_cmd_para *c){
	int cnt  = 0;
	int i    = 0;
	int data = 0;

	// Check if the board is a IFC14XX
	tsc_pon_read(fd, 0x0, &data);
	data &= 0xFFFFFF00;
	if (data != 0x73571400) {
		printf("Command available only on IFC14xx board\n");
		return (CLI_ERR);
	}

	cnt = c->cnt;
	i = 0;
	if(cnt--){
		if(!strcmp("rtm_clk_in", c->para[i])){
			return(amc_rtm_clk_in(c));
		}
		else if(!strcmp("rtm_tclk_in", c->para[i])){
			return(amc_rtm_tclk_in(c));
		}
		else if(!strcmp("rtm_clk_out", c->para[i])){
			return(amc_rtm_clk_out(c));
		}
	}

	tsc_print_usage(c);
	return(CLI_ERR);
}
