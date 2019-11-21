/*=========================< begin file & file header >=======================
 *  References
 *  
 *    filename : cli.h
 *    author   : JFG, XP
 *    company  : IOxOS
 *    creation : june 30,2008
 *
 *----------------------------------------------------------------------------
 *  Description
 *
 *    This file contain sthe declarations and definitions used by TscMon
 *    to interpret user's commands.
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

#ifndef _H_CLI
#define _H_CLI

#define CLI_HISTORY_SIZE 80
#define CLI_COMMAND_SIZE 0x100

struct cli_cmd_history *cli_history_init( struct cli_cmd_history *);
char *cli_get_cmd( struct cli_cmd_history *, char*);
struct cli_cmd_para *cli_cmd_parse( char *, struct cli_cmd_para *);
int cli_get_para_str( struct cli_cmd_para *c, int idx, char *str, int len);
int cli_get_para_hex( struct cli_cmd_para *c, int idx, int *val_p);
char *cli_history_find_idx( struct cli_cmd_history *h, int idx);
char *cli_history_find_str( struct cli_cmd_history *h, char *cmd);
void cli_history_print( struct cli_cmd_history *h);

struct cli_cmd_list
{
  char *cmd;
  int (* func)();
  char **msg;
  long idx;
};

struct cli_cmd_history
{
  long status;
  short wr_idx;short rd_idx;
  unsigned short size;short cnt;
  short end_idx;short insert_idx;
  char bufline[CLI_HISTORY_SIZE][CLI_COMMAND_SIZE];
};

struct cli_cmd_para
{
  long idx;
  long cnt;
  char *cmd;
  char *ext;
  char *para[12];
  char cmdline[256];
};

// General structure for TscMon environment configuration
// Global structure to all program
struct cli_xconf
{
  int safe; // Safe mode : 0 [disable], 1 [enable]
  // complete structure if needed
};

struct cli_xconf xconf_structure;

#define CLI_ERR   -1
#define CLI_OK     0


#define CLI_ERR_ADDR   0x1
#define CLI_ERR_DATA   0x2
#define CLI_ERR_LEN    0x4
#define CLI_ERR_AM     0x8
#define CLI_ERR_CRATE  0x10

#endif /*  _H_CLI */
