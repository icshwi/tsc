/*=========================< begin file & file header >=======================
 *  References
 *  
 *    filename : cmdlist.h
 *    author   : JFG
 *    company  : IOxOS
 *    creation : Sept 14,2015
 *    version  : 0.0.1
 *
 *----------------------------------------------------------------------------
 *  Description
 *
 *    This file contains the list of commands implemented in XprsMon
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

#include "TscMon.h"

int tsc_func_help(struct cli_cmd_para *);
int tsc_wait(struct cli_cmd_para *);

char *ci_msg[] =
{
  "Compare with mask the content of IFC PON register with data",
  "ci <offset> <data> <mask>",
  "   where <offset> = address offset in hexadecimal",
  "         <data>   = data to be compared in hexadecimal",
  "         <mask>   = mask to apply in hexadecimal",
0};

char *cmp_msg[] =
{
  "Compare two data buffer",
  "cmp <off1>.<sp1><idx1> <off2>.<sp2><idx2> <len>",
  "   where <off>     = address offset in hexadecimal",
  "         <sp><idx> = buffer space [s1,s2,u1,u2,k1->k8]",
  "         <len>     = buffer size in bytes",
0};

char *conf_msg[] = 
{
  "  Show IFC1211 current configuration",
  "  conf [show <device>]",
  "     where <device> = all",
  "                      static",
  "                      device",
  "                      dynamic",
  "                      identifiers",
  "                      pcie",
  "                      msi",
  "                      smon",
0};

char *cr_msg[] = 
{ 
  "Compare with mask the content of CSR register with data",
  "cr <offset> <data> <mask>",
  "   where <offset> = address offset in hexadecimal",
  "         <data>   = data to be compared in hexadecimal",
  "         <mask>   = mask to apply in hexadecimal",
0};

char *cs_msg[] = 
{ 
  "Compare with mask the content of a location in SHM with data",
  "cs.<ds> <offset> <data> <mask>",
  "cs1.<ds> <offset> <data> <mask>",
  "cs2.<ds> <offset> <data> <mask>",
  "     where <ds>    = b,s,w -> data size: 1,2,4",
  "          <offset> = address offset in hexadecimal",
  "          <data>   = data to be compared in hexadecimal",
  "          <mask>   = mask to apply in hexadecimal",
0};

char *cu_msg[] =
{
  "Compare with mask the content of a location in USR space with data",
  "cu.<ds> <offset> <data> <mask>",
  "cu1.<ds> <offset> <data> <mask>",
  "cu2.<ds> <offset> <data> <mask>",
  "     where <ds>    = b,s,w -> data size: 1,2,4",
  "          <offset> = address offset in hexadecimal",
  "          <data>   = data to be compared in hexadecimal",
  "          <mask>   = mask to apply in hexadecimal",
0};

char *dc_msg[] = 
{ "  display content of a set of IFC1211 PCI CFG registers",
  "  dc <start>[..<end>]",
  "     where <start> = offset in hexadecimal of first register",
  "            <end>  = offset in hexadecimal of last register",
0};

char *ddr_msg[]=
{
  "Perform specific command on DDR memory",
  "ddr calib <MEM>",
  "ddr reset <MEM>",
  "ddr status <MEM>",
  "ddr set <MEM> <DQ> <STEP> <PM>",
  "   where ddr calib <MEM>                = DDR memory IDELAY alignment",
  "         ddr reset <MEM>                = Reset to default DDR memory IDELAY",
  "         ddr status <MEM>               = Show the DDR3 memory DQ[15:0] line selection",
  "         ddr set <MEM> <DQ> <STEP> <PM> = Set the IDELAY for specifics lane",
  "            where <MEM>                 = Select the specific DDR3 1 or 2",
  "            where <DQ>                  = Select the specific lane DQ[15:0]",
  "            where <STEP>                = Select the specific step for increment / decrement",
  "                                          STEP = 1 to 16",
  "            where <PM>                  = Increment + STEP[ps] or decrement - STEP[ps ]the IDELAY",
  "                                          (STEP = 1 to 16)",
0};

char *di_msg[] = 
{ "  display content of a set of IFC PON registers",
  "  di <start>[..<end>]",
  "     where <start> = offset in hexadecimal of first register",
  "            <end>  = offset in hexadecimal of last register",
0};

char *dk_msg[] =
{ "  display content of kernel buffer in System Memory",
  "  dk<idx>.<ds><sw> <start>[..<end>]",
  "     where <idx>   = buffer index [0 -> 7]",
  "           <ds>    = b,s,w,l -> data size: 1,2,4,8",
  "           <sw>    = s       -> display swapped data",
  "           <start> = start offset in hexadecimal",
  "           <end>   = end offset in hexadecimal (default = <start> + 0x40)",
0};

char *dm_msg[] = 
{ "  display content of kernel buffer in System Memory",
  "  dm.<ds><sw> <start>[..<end>]",
  "     where <ds>    = b,s,w,l -> data size: 1,2,4,8",
  "           <sw>    = s       -> display swapped data", 
  "           <start> = start offset in hexadecimal",
  "           <end>   = end offset in hexadecimal (default = <start> + 0x40)",
0};

char *dma_msg[] = 
{
  "Perform DMA operation using channel <x>",
  "dma.<x> start <des_start>:<des_space>[.s] <src_start>:<src_space>[.s] <size>",
  "dma.<x> status",
  "dma.<x> alloc",
  "dma.<x> free",
  "dma.<x> clear",
  "     where <x>     =  DMA channel (0 or 1)",
  "           <space> =  0 -> PCIe bus address",
  "                      2 -> Shared Memory (on PEV1100, IPV1102, VCC1104, IFC1210)",
  "                      2 -> Shared Memory #1 (on MPC1200)",
  "                      3 -> Shared Memory #2 (on MPC1200)",
  "                      3 -> FPGA user area (on PEV1100, IPV1102, VCC1104)",
  "                      4 -> FPGA user area #1 (on IFC1210, MPC1210)",
  "                      5 -> FPGA user area #2 (on IFC1210, MPC1210)",
  "                      8 -> Test buffer allocated in system memory",
  "           If .s is appended to <space>, byte swapping if performed",
0};

char *dr_msg[] = 
{ "  display content of a set of IFC1211 CSR registers",
  "  dr <start>[..<end>]",
  "     where <start> = ofset in hexadecimal of first register",
  "           <end>   = ofset in hexadecimal of last register",
0};

char *ds_msg[] = 
{ "  display content of Shared Memory",
  "  ds.<ds><sw> <start>[..<end>]",
  "  ds1.<ds><sw> <start>[..<end>]",
  "  ds2.<ds><sw> <start>[..<end>]",
  "     where <ds>    = b,s,w,l -> data size: 1,2,4,8",
  "           <sw>    = s       -> display swapped data", 
  "           <start> = start address in hexadecimal",
  "           <end>   = end address in hexadecimal (default = <start> + 0x40)",
0};

char *du_msg[] =
{ "  display content of USR space",
  "  du.<ds><sw> <start>[..<end>]",
  "  du1.<ds><sw> <start>[..<end>]",
  "  du2.<ds><sw> <start>[..<end>]",
  "     where <ds>    = b,s,w,l -> data size: 1,2,4,8",
  "           <sw>    = s       -> display swapped data",
  "           <start> = start address in hexadecimal",
  "           <end>   = end address in hexadecimal (default = <start> + 0x40)",
0};

char *fifo_msg[] =
{
  "Perform operation on communication FIFOs",
  "fifo init",
  "fifo.<idx> init <mode>",
  "fifo.<idx> status",
  "fifo.<idx> clear",
  "fifo.<idx> read",
  "fifo.<idx> write <data>",
  "fifo.<idx> wait <delay>",
  "   where <idx>   = fifo index (from 0 to 7)",
  "         <mode>  = 0 for fifo, 1 for mailbox",
  "         <data>  = data to write",
  "         <delay> = delay to wait Not_Empty flag [0 = no timeout wait IRQ]",
0};

char *fk_msg[] =
{ "  fill kernel buffer in System Memory",
  "  fk<idx>.<ds> <start>..<end> <data>",
  "     where <idx>   = buffer index [0 -> 7]",
  "           <ds>    = b,s,w,l -> data size: 1,2,4,8",
  "           <start> = start address in hexadecimal",
  "           <end>   = end address in hexadecimal (default = <start> + 0x40)",
  "           <data>  = data in hexadecimal",
0};

char *fm_msg[] = 
{ "  fill kernel buffer in System Memory",
  "  fm.<ds> <start>..<end> <data>",
  "     where <ds>    = b,s,w,l -> data size: 1,2,4,8",
  "           <start> = start address in hexadecimal",
  "           <end>   = end address in hexadecimal (default = <start> + 0x40)",
  "           <data>  = data in hexadecimal",
0};

char *fs_msg[] = 
{ "  fill Shared Memory with data",
  "  fs.<ds> <start>..<end> <data>",
  "  fs1.<ds> <start>..<end> <data>",
  "  fs2.<ds> <start>..<end> <data>",
  "     where <ds>    = b,s,w,l -> data size: 1,2,4,8",
  "           <start> = start address in hexadecimal",
  "           <end>   = end address in hexadecimal (default = <start> + 0x40)",
  "           <data>  = data in hexadecimal",
0};

char *fu_msg[] =
{ "  fill USR space with data",
  "  fu.<ds> <start>..<end> <data>",
  "  fu1.<ds> <start>..<end> <data>",
  "  fu2.<ds> <start>..<end> <data>",
  "     where <ds>    = b,s,w,l -> data size: 1,2,4,8",
  "           <start> = start address in hexadecimal",
  "           <end>   = end address in hexadecimal (default = <start> + 0x40)",
  "           <data>  = data in hexadecimal",
0};

char *help_msg[]   = 
{
  "  Display list of commands or syntax of command <cmd>",
  "  help",
  "  help <cmd>",
0};

char *kbuf_msg[] = 
{
  "IFC1211 kernel buffer operations",
  "kbuf.<id> alloc <size>",
  "kbuf.<id> show",
  "kbuf.<id> free",
  "kbuf.<id> map <map> [<offset>]",
  "kbuf.<id> unmap",
  "   where <id>     = buffer identifier [0->7]",
  "         <size>   = buffer size",
  "         <offset> = requested address offset (optional)",
0};

char *lmk_msg[]   = 
{
  "Read/write LMK04803 registers",
  "lmk.<x> read <reg>",
  "lmk.<x> write <reg> <data>",
  "   where <x>     = LMK index ",
  "         <reg>   = register address",
  "         <data>  = data to be written",
0};

char *map_msg[] = 
{
  "IFC1211 address mapping operations",
  "map show [<map>]",
  "map clear <map>",
  "map.<space> alloc <map> <addr> <size>[<offset>]",
  "map free <map> offset",
  "   where <map>    = mas_mem, mas_pmem",
  "         <space>  = p,v,s or u -> remote space ( PCI, Shared Memory or USER)",
  "         <addr>   = remote address in hexadecimal",
  "         <size>   = requested mapping size",
  "         <offset> = requested address offset (optional)",
0};

char *pc_msg[] = 
{ "  read/write data from/to IFC1211 PCI CFG register",
  "  pc <offset> [<data>]",
  "     where <offset> = register offset in hexadecimal",
  "           <data>   = data in hexadecimal [write cycle]",
0};

char *pi_msg[] = 
{ "  read/write data from/to IFC PON register",
  "  pi <offset> [<data>]",
  "     where <offset> = register offset in hexadecimal",
  "           <data>   = data in hexadecimal [write cycle]",
0};

char *pk_msg[] =
{ "  read/write data from/to kernel buffer in System Memory",
  "  pk<idx>.<ds> <offset> [<data>]",
  "     where <idx>    = buffer index [0 -> 7]",
  "           <ds>     = b,s,w,l -> data size: 1,2,4,8",
  "           <offset> = address offset in hexadecimal",
  "           <data>   = data in hexadecimal [write cycle]",
0};

char *pm_msg[] = 
{ "  read/write data from/to kernel buffer in System Memory",
  "  pm.<ds> <offset> [<data>]",
  "     where <ds>     = b,s,w,l -> data size: 1,2,4,8",
  "           <offset> = address offset in hexadecimal",
  "           <data>   = data in hexadecimal [write cycle]",
0};

char *pr_msg[] = 
{ "  read/write data from/to IFC1211 CSR register",
  "  pr <offset> [<data>]",
  "     where <offset> = register offset in hexadecimal",
  "           <data>   = data in hexadecimal [write cycle]",
0};

char *ps_msg[] = 
{ "  read/write data from/to FPGA Shared Memory",
  "  ps.<ds> <offset> [<data>]",
  "  ps1.<ds> <offset> [<data>]",
  "  ps2.<ds> <offset> [<data>]",
  "     where <ds>     = b,s,w,l -> data size: 1,2,4,8",
  "           <offset> = address offset in hexadecimal",
  "           <data>   = data in hexadecimal [write cycle]",
0};

char *pu_msg[] =
{ "  read/write data from/to FPGA USR space",
  "  pu.<ds> <offset> [<data>]",
  "  pu1.<ds> <offset> [<data>]",
  "  pu2.<ds> <offset> [<data>]",
  "     where <ds>     = b,s,w,l -> data size: 1,2,4,8",
  "           <offset> = address offset in hexadecimal",
  "           <data>   = data in hexadecimal [write cycle]",
0};

char *semaphore_msg[]=
{
  "Perform semaphore operation",
  "semaphore status",
  "semaphore get <idx> <tag>",
  "semaphore release <idx>",
  "   where <idx>                   = Index of the semaphore from 0 to F",
  "         <tag>                   = Unique tag [7 bits]",
0};

char *set_device_msg[]=
{
  "Select central or io device",
  "set device <device>",
  "   where <device>               = central or io",
0};

char *sflash_msg[]= 
{ 
  "sflash rdid",
  "sflash rdsr",
  "sflash wrsr <sr>",
  "sflash dump <off> <len>",
  "sflash load <off> <file>",
  "sflash dynopt",
0};

char *timer_msg[] = 
{
  "Perform operation on IFC1211 global timer ",
  "timer start",
  "timer restart",
  "timer stop",
  "timer read",
0};

char *tinit_msg[] = 
{
  "Launch test suite",
  "tinit.<x> [<testfile>] [<console>]",
  "   where <x> = test program index",
  "         <testfile> = path of test program to be launched",
  "         <console>  = device to be used for output messages",
0};

char *tkill_msg[] = 
{
  "Kill test suite",
0};

char *tlist_msg[] = 
{
  "Display a list of existing test",
0};

char *tset_msg[] = 
{
  "Set test control parameter",
  "tset exec=fast -> execute test in fast mode [default]",
  "tset exec=val  -> execute test in validation mode",
  "tset err=halt  -> stop test execution if error",
  "tset err=cont  -> go for next test if error",
  "tset loop=<n>",
  "     where <n>   = number of time a test is execute (0->infinite)",
  "tset logfile=<filename>",
  "tset log=<op>",
  "     where <op>  = off -> close logfile if currently open",
  "                   new -> create new logfile",
  "                   add -> append to existing logfile or create logfile",
0};

char *tstart_msg[] = 
{
  "Start execution of a test or a chain of tests",
  "tstart <test>",
  "tstart <start>..<end>",
0};

char *tstop_msg[] = 
{
  "Stop current test execution",
0};

char *tstatus_msg[] = 
{
  "Show status of test suite",
0};

char *twait_msg[] = 
{
  "Wait for test to complete",
  "wait [<tmo>]",
  "   where <tmo> = timeout in sec",
0};

char *ts_msg[]  = 
{
  "Perform read/write test on shared memory",
  "ts.<ds> <start>..<end> <data> [<loop>]",
  "ts1.<ds> <start>..<end> <data> [<loop>]",
  "ts2.<ds> <start>..<end> <data> [<loop>]",
  "   where <ds>    = b, s, w, l [data size 1, 2, 4, 8]",
  "         <start> = address offset in hexadecimal of first  location",
  "         <end>   = address offset in hexadecimal of last location",
  "         <data>  = data in hexadecimal",
  "         <loop>  = number of loop",
0};

char *tu_msg[]  =
{
  "Perform read/write test on FPGA USR memory",
  "tu.<ds> <start>..<end> <data> [<loop>]",
  "tu1.<ds> <start>..<end> <data> [<loop>]",
  "tu2.<ds> <start>..<end> <data> [<loop>]",
  "   where <ds>    = b, s, w, l [data size 1, 2, 4, 8]",
  "         <start> = address offset in hexadecimal of first  location",
  "         <end>   = address offset in hexadecimal of last location",
  "         <data>  = data in hexadecimal",
  "         <loop>  = number of loop",
0};

struct cli_cmd_list cmd_list[] =
{
  { "ci"     	, tsc_rdwr_cr,      ci_msg     	  , 0},
  { "cmp"     	, tsc_rdwr_cmp,     cmp_msg       , 0},
  { "conf"   	, tsc_conf_show,    conf_msg      , 0},
  { "cr"     	, tsc_rdwr_cr,      cr_msg     	  , 0},
  { "cs1"     	, tsc_rdwr_cx,      cs_msg     	  , 0},
  { "cs2"     	, tsc_rdwr_cx,      cs_msg     	  , 0},
  { "cs"     	, tsc_rdwr_cx,      cs_msg     	  , 0},
  { "cu1"     	, tsc_rdwr_cx,      cu_msg     	  , 0},
  { "cu2"     	, tsc_rdwr_cx,      cu_msg     	  , 0},
  { "cu"     	, tsc_rdwr_cx,      cu_msg     	  , 0},
  { "dc"     	, tsc_rdwr_dr,      dc_msg     	  , 0},
  { "ddr"    	, tsc_ddr,          ddr_msg 	  , 0},
  { "di"     	, tsc_rdwr_dr,      di_msg     	  , 0},
  { "dk"     	, tsc_rdwr_dx,      dm_msg     	  , 0},
  { "dma"     	, tsc_dma    ,      dma_msg       , 0},
  { "dm"     	, tsc_rdwr_dx,      dm_msg     	  , 0},
  { "dr"     	, tsc_rdwr_dr,      dr_msg     	  , 0},
  { "ds1"     	, tsc_rdwr_dx,      ds_msg     	  , 0},
  { "ds2"     	, tsc_rdwr_dx,      ds_msg     	  , 0},
  { "ds"     	, tsc_rdwr_dx,      ds_msg     	  , 0},
  { "du1"     	, tsc_rdwr_dx,      du_msg     	  , 0},
  { "du2"     	, tsc_rdwr_dx,      du_msg     	  , 0},
  { "du"     	, tsc_rdwr_dx,      du_msg     	  , 0},
  //{ "fifo"   	, tsc_fifo,         fifo_msg      , 0},
  { "fk"     	, tsc_rdwr_fx,      fk_msg     	  , 0},
  { "fm"     	, tsc_rdwr_fx,      fm_msg     	  , 0},
  { "fs"     	, tsc_rdwr_fx,      fs_msg     	  , 0},
  { "help"   	, tsc_func_help,    help_msg   	  , 0},
  { "kbuf"    	, tsc_kbuf,         kbuf_msg  	  , 0},
  { "lmk"    	, tsc_lmk,          lmk_msg    	  , 0},
  { "map"    	, tsc_map,          map_msg    	  , 0},
  { "pc"     	, tsc_rdwr_pr,      pc_msg     	  , 0},
  { "pi"     	, tsc_rdwr_pr,      pi_msg     	  , 0},
  { "pk"     	, tsc_rdwr_px,      pk_msg     	  , 0},
  { "pm"     	, tsc_rdwr_px,      pm_msg     	  , 0},
  { "pr"     	, tsc_rdwr_pr,      pr_msg     	  , 0},
  { "ps1"     	, tsc_rdwr_px,      ps_msg     	  , 0},
  { "ps2"     	, tsc_rdwr_px,      ps_msg     	  , 0},
  { "ps"     	, tsc_rdwr_px,      ps_msg     	  , 0},
  { "pu1"     	, tsc_rdwr_px,      pu_msg     	  , 0},
  { "pu2"     	, tsc_rdwr_px,      pu_msg     	  , 0},
  { "pu"     	, tsc_rdwr_px,      pu_msg     	  , 0},
  //{ "semaphore" , tsc_semaphore,  semaphore_msg , 0},
  //{ "sflash" 	, tsc_sflash,       sflash_msg 	  , 0},
  { "set" 	    , tsc_set_device,   set_device_msg, 0},
  //{ "timer"  	, tsc_timer,        timer_msg  	  , 0},
  { "tinit"  	, tsc_tinit,        tinit_msg  	  , 0},
  { "tkill"  	, tsc_tkill,        tkill_msg  	  , 0},
  { "tlist"  	, tsc_tlist,        tlist_msg  	  , 0},
  { "tstatus"	, tsc_tstatus,      tstatus_msg	  , 0},
  { "tstart" 	, tsc_tstart,       tstart_msg 	  , 0},
  { "tstop"  	, tsc_tstop,        tstop_msg  	  , 0},
  { "tset"   	, tsc_tset,         tset_msg   	  , 0},
  { "ts1"     	, tsc_rdwr_tx,      ts_msg     	  , 0},
  { "ts2"     	, tsc_rdwr_tx,      ts_msg     	  , 0},
  { "ts"     	, tsc_rdwr_tx,      ts_msg     	  , 0},
  { "tu1"     	, tsc_rdwr_tx,      tu_msg     	  , 0},
  { "tu2"     	, tsc_rdwr_tx,      tu_msg     	  , 0},
  { "tu"     	, tsc_rdwr_tx,      tu_msg     	  , 0},
  { "twait"  	, tsc_twait,        twait_msg  	  , 0},
  { (char *)0,} 
};
