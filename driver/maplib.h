/*=========================< begin file & file header >=======================
 *  References
 *  
 *    filename : maplib.h
 *    author   : JFG
 *    company  : IOxOS
 *    creation : Sept 30,2015
 *    version  : 0.0.1
 *
 *----------------------------------------------------------------------------
 *  Description
 *
 *    This file contain the declarations of all exported functions define in
 *    maplib.c
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


#ifndef _H_MAPLIB
#define _H_MAPLIB

#define MAP_INVALID             0
#define MAP_MAS_PCIE_MEM        1
#define MAP_MAS_PCIE_PMEM       2
#define MAP_MAS_PCIE_CSR        3

#define MAP_FLAG_FREE           0
#define MAP_FLAG_BUSY           1
#define MAP_FLAG_PRIVATE        2
#define MAP_FLAG_FORCE          4
#define MAP_FLAG_LOCKED         8
#define MAP_MODE_MASK           0xffff

#define MAP_ERR_BAD_SGID         -1    /* Bad mapper identoifier */
#define MAP_ERR_NO_SPACE         -2    /* not enough space left  */
#define MAP_ERR_BAD_ADDR         -3    /* Bad requested address  */
#define MAP_ERR_BAD_SIZE         -4    /* Bad requested size     */
#define MAP_ERR_BAD_IDX          -5    /* Invalid page index     */
 
struct map_ctl
{
  struct map_blk
  {
    char flag; char usr; short npg;
    uint mode;
    ulong rem_addr;
    pid_t tgid;           /* task id to which the block is allocated     */
  } *map_p;
  char rsv; char sg_id; short pg_num;
  int pg_size;
  ulong win_base;                   /* base address of mapping window */
  uint win_size;                    /* size of mapping window */
  struct mutex map_lock;            /* mutex to lock MAP access                     */
};

struct map_req
{
  uint size;                            /* mapping size required by user            */
  char flag; char sg_id; ushort mode;   /* mapping mode                             */
  ulong rem_addr;                       /* remote address to be mapped              */
  ulong loc_addr;                       /* local address returned by mapper         */
  pid_t tgid;                           /* task id to which the block is allocated     */
  uint offset;                          /* offset of page containing local address  */
  uint win_size;                        /* size actually mapped                     */
  ulong rem_base;                       /* remote address of window actually mapped */
  ulong loc_base;                       /* local address of window actually mapped  */
  void *usr_addr;                       /* user address pointing to local address   */
  ulong bus_base;                       /* bus base address of SG window            */
};

struct map_ctl *map_init( int sg_id, long base, int pg_num, int pg_size);
int map_blk_force( struct map_ctl *, struct map_req *);
int map_blk_alloc( struct map_ctl *, struct map_req *);
int map_blk_find( struct map_ctl *, struct map_req *);
int map_blk_modify( struct map_ctl *, struct map_req *, ulong *);
int map_blk_free( struct map_ctl *, int);
int map_release( struct map_ctl *);
void map_clear( struct map_ctl *);
void map_exit( struct map_ctl *);

#endif /*  _H_MAPLIB */

/*================================< end file >================================*/
