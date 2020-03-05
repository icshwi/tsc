/*=========================< begin file & file header >=======================
 *  References
 *  
 *    filename : rcf1450lib.c
 *    author   : RH
 *    company  : IOxOS
 *    creation : December 11,2019
 *
 *----------------------------------------------------------------------------
 *  Description
 *
 *    That library contains a set of functions to handle the RCF_1450 bundle
 *    composed of the RCC_1466 mTCA.4.1 RTM and a FBI_1482 mezzanine card.
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
#include "../include/ponmboxlib.h"
#include "../include/tsculib.h"
#include "../include/rcc1466lib.h"
#include "../include/fbi1482lib.h"
#include "../include/rcf1450lib.h"
#include "../include/pca9539lib.h"
#include "../include/i2ceepromlib.h"

#define CF_I2C_BUS 4


/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * Function name : rcf1450_presence
 * Prototype     : int
 * Parameters    : int *present
 * Return        : status of operation
 *----------------------------------------------------------------------------
 * Description   : RCF1450 board support function, check presence
 *
 *++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

int rcf1450_presence(int fd)
{
  int retval = 0;

  retval = rcc1466_presence(fd);
  if (retval) return retval;

  return fbi1482_presence(CF_I2C_BUS);
}

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * Function name : rcf1450_init
 * Prototype     : int
 * Parameters    : void
 * Return        : status of operation
 *----------------------------------------------------------------------------
 * Description   : Initialization function
 *
 *++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

int rcf1450_init(int fd){
  int retval = 0;

  retval = rcc1466_init(fd);
  if (retval)
  {
    puts("Failed to initialize carrier board");
    return retval;
  }

  retval = rcc1466_select_i2c_channel(fd, RCC1466_I2C_MEZZANINE);
  if (retval)
  {
    puts("Failed to select mezzanine I2C channel");
    return retval;
  }

  return fbi1482_init(fd, CF_I2C_BUS);
}

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * Function name : rcf1450_uid
 * Prototype     : int
 * Parameters    : void
 * Return        : status of operation
 *----------------------------------------------------------------------------
 * Description   : read unique id from UID EEPROM
 *
 *++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

int rcf1450_get_uid(int fd, unsigned char *uid)
{
  int retval  = 0;

  retval = rcc1466_select_i2c_channel(fd, RCC1466_I2C_MEZZANINE);
  if (retval)
  {
    puts("Failed to select mezzanine I2C channel");
    return retval;
  }

  return fbi1482_get_uid(fd, CF_I2C_BUS, uid);
}

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * Function name : rcf1450_temp
 * Prototype     : int
 * Parameters    : void
 * Return        : status of operation
 *----------------------------------------------------------------------------
 * Description   : read pcb temperature
 *
 *++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

int rcf1450_get_temp(int fd, int *temp)
{
  return fbi1482_get_temp(fd, CF_I2C_BUS, temp);
}

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * Function name : rcf1450_rs485_set_indicator
 * Prototype     : int
 * Parameters    : void
 * Return        : status of operation
 *----------------------------------------------------------------------------
 * Description   : Set state of RS485 indicator
 *
 *++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

int rcf1450_rs485_set_indicator(int fd, fbi1482_rs485_indicator_t indicator, int state)
{
  int retval = 0;

  retval = rcc1466_select_i2c_channel(fd, RCC1466_I2C_MEZZANINE);
  if (retval)
  {
    puts("Failed to select mezzanine I2C channel");
    return retval;
  }

  return fbi1482_rs485_set_indicator(fd, CF_I2C_BUS, indicator, state);
}

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * Function name : rcf1450_rs485_txen
 * Prototype     : int
 * Parameters    : void
 * Return        : status of operation
 *----------------------------------------------------------------------------
 * Description   : Enable/disable RS485 transceiver
 *
 *++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

int rcf1450_rs485_txen(int fd, fbi1482_rs485_txen_t channel, int state)
{
  int retval = 0;

  retval = rcc1466_select_i2c_channel(fd, RCC1466_I2C_MEZZANINE);
  if (retval)
  {
    puts("Failed to select mezzanine I2C channel");
    return retval;
  }

  return fbi1482_rs485_txen(fd, CF_I2C_BUS, channel, state);
}
