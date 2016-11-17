/*=========================< begin file & file header >=======================
 *  References
 *  
 *    filename : script.c
 *    author   : JFG
 *    company  : IOxOS
 *    creation : Sept 17,2015
 *    version  : 0.0.1
 *
 *----------------------------------------------------------------------------
 *  Description
 *
 *     That file contains a set of function called by TscMon to handle
 *     script files.
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

#ifndef lint
static char *rcsid = "$Id: script.c,v 1.2 2015/12/03 15:15:21 ioxos Exp $";
#endif

#define DEBUGno
#include <debug.h>
#include <sys/types.h>
#include <sys/time.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <cli.h>
#include <tscioctl.h>
#include <tsculib.h>

char line[256];
extern struct cli_cmd_list cmd_list[];
extern char cli_prompt[];
extern struct cli_cmd_para cmd_para;
extern int script_exit;

int tsc_cmd_exec( struct cli_cmd_list *, struct cli_cmd_para *);

char *
script_rcsid()
{
  return( rcsid);
}

int tsc_script( char *filename,
	     struct cli_cmd_para *s)

{
  FILE *file;
  int retval;

  filename = strtok( filename, " \t");
  retval = 0;
  file = fopen( filename, "r");
  
  if( !file)
  {
    printf("Cannot open script file %s\n", filename);
    return( -1);
  }
  printf("Start execution of script file %s\n", s->cmd);
  
  while( fgets( line, 256, file))
  {
    strtok( line, "\n\r#");
    if( line[0] == '#') continue;
    if( line[0] == '\n') continue;
    if( line[0] == '\r') continue;
    if( line[0] == '$')
    {
      uint para;

      if( !strncmp( "exit", &line[1], 4))
      {
	retval = 2;
	break;
      }
      if( !strncmp( "sleep", &line[1], 5))
      {
	sscanf(line, "$sleep %d", &para);
	sleep( para);
      }
      if( !strncmp( "usleep", &line[1], 5))
      {
	sscanf(line, "$usleep %d", &para);
	usleep( para);
      }
      if( !strncmp( "echo", &line[1], 4))
      {
	printf("%s\n", &line[6]);
      }
      continue;
    }
    
    if( line[0] == 'q') break;
    
    if( line[0] == '@') {
      struct cli_cmd_para script_para;

      printf("%s%s\n", cli_prompt, line);
      cli_cmd_parse(  &line[1], &script_para);
      if( (retval = tsc_script( &line[1], &script_para)))
      {
	break;
      }
      continue;
    }
    
    if( line[0]) {
      int i, j;
      char arg[3];

      printf("%s%s\n", cli_prompt, line);
      cli_cmd_parse( line, &cmd_para);
      arg[0] = '$';
      arg[2] = 0;
      for( i = 0; i < cmd_para.cnt; i++)
      {
        for( j = 0; j < s->cnt; j++)
        {
	  arg[1] = (char)((int)'0'+ j);
          if( !strncmp( arg, cmd_para.para[i],2))
          {
	    cmd_para.para[i] = s->para[j];
	  }
        }
      }
      script_exit = 0;
      tsc_cmd_exec( &cmd_list[0], &cmd_para);
      if( script_exit)
      {
	retval = 1;
	break;
      }
    }
  }
  fclose( file);
  return( retval);
}
