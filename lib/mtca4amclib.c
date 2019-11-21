/*=========================< begin file & file header >=======================
 *  References
 *
 *    filename : mtca4amclib.c
 *    author   : RH
 *    company  : IOxOS
 *    creation : September 13,2019
 *
 *----------------------------------------------------------------------------
 *  Description
 *
 *    This file contains a set of function to act on a MTCA.4 AMC.
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

#include "mtca4amclib.h"
#include "ponmboxlib.h"

/*
 * Service requests commands
 */
#define CMD_SET_RTM_CLK_IN_ENABLE                 0x06
#define CMD_SET_RTM_TCLK_IN_ENABLE                0x07
#define CMD_SET_RTM_CLK_OUT_ENABLE                0x08

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * Function name : set_mtca4_rtm_clk_in_enable
 * Prototype     : mtca4_rtm_clk_in_enable_t
 * Parameters    : state
 * Return        : status of operation
 *----------------------------------------------------------------------------
 * Description   : enable/disable CLK_IN output to RTM
 *
 *++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

unsigned char set_mtca4_rtm_clk_in_enable(
  int fd,
  mtca4_rtm_clk_in_enable_t state)
{
  mbox_info_t *mbox = get_mbox_info(fd);
  unsigned char status = send_mbox_service_request(
                           fd,
                           mbox,
                           CMD_SET_RTM_CLK_IN_ENABLE,
                           1,
                           state);
  free_mbox_info(mbox);
  return status;
}

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * Function name : set_mtca4_rtm_tclk_in_enable
 * Prototype     : mtca4_rtm_tclk_in_enable_t
 * Parameters    : state
 * Return        : status of operation
 *----------------------------------------------------------------------------
 * Description   : enable/disable TCLK_IN output to RTM
 *
 *++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

unsigned char set_mtca4_rtm_tclk_in_enable(
  int fd,
  mtca4_rtm_tclk_in_enable_t state)
{
  mbox_info_t *mbox = get_mbox_info(fd);
  unsigned char status = send_mbox_service_request(
                           fd,
                           mbox,
                           CMD_SET_RTM_TCLK_IN_ENABLE,
                           1,
                           state);
  free_mbox_info(mbox);
  return status;
}

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * Function name : set_mtca4_rtm_clk_out_enable
 * Prototype     : mtca4_rtm_clk_out_enable_t
 * Parameters    : state
 * Return        : status of operation
 *----------------------------------------------------------------------------
 * Description   : enable/disable CLK_OUT input from RTM
 *
 *++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

unsigned char set_mtca4_rtm_clk_out_enable(
  int fd,
  mtca4_rtm_clk_out_enable_t state)
{
  mbox_info_t *mbox = get_mbox_info(fd);
  unsigned char status = send_mbox_service_request(
                           fd,
                           mbox,
                           CMD_SET_RTM_CLK_OUT_ENABLE,
                           1,
                           state);
  free_mbox_info(mbox);
  return status;
}

