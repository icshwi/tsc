/*=========================< begin file & file header >=======================
 *  References
 *  
 *    filename : rfc1450.c
 *    author   : RH
 *    company  : IOxOS
 *    creation : Dec 11,2019
 *
 *----------------------------------------------------------------------------
 *  Description
 *
 *    That file contains the functions to drive RTM.
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
#include "fbi1482lib.h"
#include "ponmboxlib.h"
#include "mtca4rtmlib.h"
#include "TscMon.h"

#define EXT_I2C_BUS 3

extern int tsc_fd;

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * Function name : tsc_fbi1482
 * Prototype     : int
 * Parameters    : pointer to command parameter list
 * Return        : int
 *
 *----------------------------------------------------------------------------
 * Description   : fbi1482 operations
 *
 *++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

int tsc_fbi1482_init(struct cli_cmd_para *c, uint args_left)
{
  int retval = -1;

  if (args_left == 0)
  {
    retval = fbi1482_init(tsc_fd, EXT_I2C_BUS);
    if (!retval)
    {
      printf("Initialization done... \n");
    }
    return retval;
  }

  tsc_print_usage(c);
  return(-1);
}


int tsc_fbi1482_uid(struct cli_cmd_para *c, uint args_left)
{
  int retval = -1;
  unsigned char uid[4] = {0, 0, 0, 0};

  if (args_left == 0)
  {
    retval = fbi1482_get_uid(tsc_fd, EXT_I2C_BUS, uid);
    if (!retval)
    {
      printf("0x%02x%02x%02x%02x\n", uid[0], uid[1], uid[2], uid[3]);
    }
    return retval;
  }

  tsc_print_usage(c);
  return(-1);
}


int tsc_fbi1482_temp(struct cli_cmd_para *c, uint args_left)
{
  int retval = -1;
  int temp = 0;

  if (args_left == 0)
  {
    retval = fbi1482_get_temp(tsc_fd, EXT_I2C_BUS, &temp);
    if (!retval)
    {
      printf("%d °C\n", temp);
    }
    return retval;
  }

  tsc_print_usage(c);
  return(-1);
}


int tsc_fbi1482_rs485_led(struct cli_cmd_para *c, uint args_left)
{
  int state = 0;
  fbi1482_rs485_indicator_t indicator;

  if (args_left == 2)
  {
      if(!strcmp("flt", c->para[c->cnt - args_left]))
      {
        indicator = FBI1482_RS485_FAULT;
      }
      else if(!strcmp("act", c->para[c->cnt - args_left]))
      {
        indicator = FBI1482_RS485_ACTIVITY;
      }
      else
      {
        tsc_print_usage(c);
        return(-1);
      }

      if(!strcmp("on", c->para[c->cnt - args_left + 1]))
      {
        state = 1;
      }
      else if(!strcmp("off", c->para[c->cnt - args_left + 1]))
      {
        state = 0;
      }
      else
      {
        tsc_print_usage(c);
        return(-1);
      }

      return fbi1482_rs485_set_indicator(tsc_fd, EXT_I2C_BUS, indicator, state);
  }

  tsc_print_usage(c);
  return(-1);
}


int tsc_fbi1482_rs485_txen(struct cli_cmd_para *c, uint args_left)
{
  fbi1482_rs485_txen_t channel;
  int state;

  if (args_left == 2)
  {
      if (!strcmp("0", c->para[c->cnt - args_left]))
      {
        channel = FBI1482_RS485_TXEN_CHANNEL_0;
      }
      else if (!strcmp("1", c->para[c->cnt - args_left]))
      {
        channel = FBI1482_RS485_TXEN_CHANNEL_1;
      }
      else if (!strcmp("2", c->para[c->cnt - args_left]))
      {
        channel = FBI1482_RS485_TXEN_CHANNEL_2;
      }
      else if (!strcmp("3", c->para[c->cnt - args_left]))
      {
        channel = FBI1482_RS485_TXEN_CHANNEL_3;
      }
      else
      {
        tsc_print_usage(c);
        return(-1);
      }

      if(!strcmp("on", c->para[c->cnt - args_left + 1]))
      {
        state = 1;
      }
      else if(!strcmp("off", c->para[c->cnt - args_left + 1]))
      {
        state = 0;
      }
      else
      {
        tsc_print_usage(c);
        return(-1);
      }

      return fbi1482_rs485_txen(tsc_fd, EXT_I2C_BUS, channel, state);
  }

  tsc_print_usage(c);
  return(-1);
}


int tsc_fbi1482_rs485(struct cli_cmd_para *c, uint args_left)
{
  if (args_left > 0)
  {
    if (!strcmp("led", c->para[c->cnt - args_left]))
    {
      return tsc_fbi1482_rs485_led(c, args_left-1);
    }
    if (!strcmp("tx", c->para[c->cnt - args_left]))
    {
      return tsc_fbi1482_rs485_txen(c, args_left-1);
    }
  }

  tsc_print_usage(c);
  return(-1);
}


int tsc_fbi1482(struct cli_cmd_para *c){
  int cnt           = c->cnt;
  int data          = 0;

  // Check if the board is a IFC14XX
  tsc_pon_read(tsc_fd, 0x0, &data);
  data &= 0xFFFFFF00;
  if (data != 0x73571400)
  {
    printf("Command available only on IFC14xx board\n");
    return (CLI_ERR);
  }

  // Check if card is present
  if (fbi1482_presence(EXT_I2C_BUS))
  {
    return (CLI_ERR);
  }

  if (cnt--)
  {
    if (!strcmp("init", c->para[0]))
    {
      return tsc_fbi1482_init(c, cnt);
    }
    if (!strcmp("uid", c->para[0]))
    {
      return tsc_fbi1482_uid(c, cnt);
    }
    if (!strcmp("temp", c->para[0]))
    {
      return tsc_fbi1482_temp(c, cnt);
    }
    if (!strcmp("rs485", c->para[0]))
    {
      return tsc_fbi1482_rs485(c, cnt);
    }
  }

  tsc_print_usage(c);
  return(CLI_ERR);
}
