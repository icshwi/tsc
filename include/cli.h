/*=========================< begin file & file header >=======================
 *  References
 *  
 *    filename : cli.h
 *    author   : JFG
 *    company  : IOxOS
 *    creation : june 30,2008
 *    version  : 0.0.1
 *
 *----------------------------------------------------------------------------
 *  Description
 *
 *    This file contain the declarations and definitions used by XprsMon
 *    to interpret user's commands.
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

// General structure for XprsMon environment configuration
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
