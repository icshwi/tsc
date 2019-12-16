/*=========================< begin file & file header >=======================
 *  References
 *  
 *    filename : cmdlist.h
 *    author   : JFG, XP
 *    company  : IOxOS
 *    creation : Sept 14,2015
 *
 *----------------------------------------------------------------------------
 *  Description
 *
 *    This file contains the list of commands implemented in TscMon.
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

#include "TscMon.h"

int tsc_func_help(struct cli_cmd_para *);
int tsc_func_history(struct cli_cmd_para *);
int tsc_wait(struct cli_cmd_para *);
int tsc_tdma(struct cli_cmd_para *);

char *alias_msg[]   = 
{
  "  Handle aliases for TscMon commands. Command is executed by entering +<alias>",
  "  alias show",
  "  alias clear",
  "  alias set <alias> \"<cmdline>\"",
  "  alias save <filename>",
  "  alias load <filename>",
  "     where <alias> = alias name to be created by the set operation",
  "           <cmdline> = command line to be executed when $<alias> is entered",
  "           <flename> = name of the file to be used by save and load operations",
0};

char *acq1430_msg[] = 
{
  "ACQ1430 operations",
  "acq1430.<x> <dev> read <reg>",
  "acq1430.<x> <dev> write <reg> <data>",
  "acq1430.<x> <dev> acqfif <offset> <size>",
  "acq1430.<x> <dev> check <offset> <size>",
  "acq1430.<x> <dev> acq<size> h:<file_his> d:<file_dat> t:<trig> s:<smem> c:<csr> a:<last_addr> ",
  "acq1430.<x> <dev> calib",
  "acq1430.<x> <file> save <offset> <size>",
  "acq1430.<x> tmp102 show",
  "acq1430.<x> eeprom sign set",
  "acq1430.<x> eeprom sign def b:<board> s:<serial> v:<ver> r:<rev>",
  "acq1430.<x> eeprom dump",
  "   where <x>        = index",
  "         <dev>      = lmk, ads01, ads23, ads45, ads67, ads89",
  "         <reg>      = register",
  "         <data>     = data",
  "         <offset>   = offset of data buffer",
  "         <size>     = size of data buffer",

  "         <file_his> = histogram filename",
  "         <file_dat> = raw data filename",
0};

char *adc3110_msg[] = 
{
  "ADC3110 operations",
  "adc3110.<x> <dev> read <reg>",
  "adc3110.<x> <dev> write <reg> <data>",
  "adc3110.<x> <dev> init [<mode>]",
  "adc3110.<x> <dev> calib",
  "adc3110.<x> <dev> acqfif <offset> <size>",
  "adc3110.<x> <dev> check <offset> <size>",
  "adc3110.<x> <dev> acq<size> h:<file_his> d:<file_dat> t:<trig> s:<smem> c:<csr> a:<last_addr> ",
  "adc3110.<x> <file> save <offset> <size>",
  "adc3110.<x> tmp102 show",
  "adc3110.<x> eeprom sign set",
  "adc3110.<x> eeprom sign def b:<board> s:<serial> v:<ver> r:<rev>",
  "adc3110.<x> eeprom dump",
  "   where <x>        = index",
  "         <dev>      = lmk, ads01, ads23, ads45, ads67",
  "         <reg>      = register",
  "         <data>     = data",
  "         <mode>     = initialization mode",
  "         <offset>   = offset of data buffer",
  "         <size>     = size of data buffer",
  "         <file_his> = histogram filename",
  "         <file_dat> = raw data filename",
0};

char *adc3112_msg[] = 
{
  "ADC3112 read/write operations",
  "adc3112.<x> <dev> read <reg>",
  "adc3112.<x> <dev> write <reg> <data>",
  "adc3112.<x> <dev> acq h:<file_his> d:<file_dat> t:<trig> s:<sram>",
  "adc3112.<x> <dev> save<chan> h:<file_his> d:<file_dat> t:<trig> s:<sram>",
  "adc3112.<x> <dev> calib [reset] [verbose] [d:<data>]",
  "adc3112.<x> <dev> itl calib",
  "adc3112.<x> <dev> itl buf h:<file_his> d:<file_dat>",
  "adc3112.<x> <dev> itl save/restore",
  "adc3112.<x> <dev> itl enable/disable",
  "adc3112.<x> <dev> temp",
  "adc3112.<x> eeprom sign set",
  "adc3112.<x> eeprom sign def b:<board> s:<serial> v:<ver> r:<rev>",
  "adc3112.<x> eeprom dump",
  "adc3112.<x> fpfbuf save <filename>",
  "   where <x>    = index",
  "         <dev>  = ads01, ads23, ads1, ads2, dac, xra01, xra23, xratrig, lmk, sy",
  "         <reg>  = register",
  "         <data> = data",
0};

char *adc3117_msg[] =
{
  "ADC3117 operations",
  "adc3117.<x> <dev>  read <reg>",
  "adc3117.<x> <dev>  write <reg> <data>",
  "adc3117.<x> adc<y> acq",
  "adc3117.<x> adc<y> set <sw_n> <sw_p> <gain> <dig_offset> <dac_offset>",
  "adc3117.<x> dac<y> set <dac_val> {<pot_val>}",
  "adc3117.<x> rate   set <val>",
  "adc3117.<x> vcal   set <val>",
  "adc3117.<x> adc<y> save o:<offset> s:<size> h:<file_his> d:<file_dat>",
  "adc3117.<x> tmp102 show",
  "adc3117.<x> eeprom sign set",
  "adc3117.<x> eeprom sign def b:<board> s:<serial> v:<ver> r:<rev>",
  "adc3117.<x> eeprom dump",
  "adc3117.<x> eeprom vref show",
  "adc3117.<x> eeprom vref store",
  "adc3117.<x> eeprom vref load",
  "adc3117.<x> eeprom vref write <filename>",
  "adc3117.<x> eeprom vref read  <filename>",
  "adc3117.<x> eeprom corr show",
  "adc3117.<x> eeprom corr store",
  "adc3117.<x> eeprom corr load",
  "adc3117.<x> eeprom corr write <filename>",
  "adc3117.<x> eeprom corr read  <filename>",
  "adc3117.<x> eeprom fru  write <filename>",
  "adc3117.<x> eeprom fru  read  <filename>",
  "   where <x>    = fmc number (1 or 2)",
  "         <y>    = device number (0-19 for adc, 0-1 for dac)",
  "         <dev>  = adc0 - adc19, dac0, dac1, vref_var, mmcm, tmp102, eeprom, rate, vcal",
  "         <reg>  = register",
  "         <data> = data",
0};

char *amc_msg[] = 
{
  "MTCA.4 AMC operations",
  "amc rtm_clk_in <state>",
  "   where <state> = on, off",
  "   (enable/disable CLK_IN output to RTM)",
  "",
  "amc rtm_tclk_in <state>",
  "   where <state> = on, off",
  "   (enable/disable TCLK_IN output to RTM)",
  "",
  "amc rtm_clk_out <state>",
  "   where <state> = on, off",
  "   (enable/disable CLK_OUT input from RTM)",
0};

char *ci_msg[] =
{
  "Compare with mask the content of IFC PON register with data",
  "ci <offset> <data> <mask>",
  "   where <offset> = address offset in hexadecimal",
  "         <data>   = data to be compared in hexadecimal",
  "         <mask>   = mask to apply in hexadecimal",
0};

char *cl_msg[] =
{
  "Compare with mask the content of AXI-4 Lite register with data",
  "cl <offset> <data> <mask>",
  "   where <offset> = address offset in hexadecimal",
  "         <data>   = data to be compared in hexadecimal",
  "         <mask>   = mask to apply in hexadecimal",
0};

char *cmp_msg[] =
{
  "Compare two data buffers",
  "cmp <off1>.<sp1><idx1> <off2>.<sp2><idx2> <len>",
  "   where <off>     = address offset in hexadecimal",
  "         <sp><idx> = buffer space [s1,s2,u1,u2,k0->k7]",
  "         <len>     = buffer size in 4 bytes multiple",
0};

char *conf_msg[] = 
{
  "Show TSC current configuration",
  "conf [show <device>]",
  "   where <device> = all",
  "                    static",
  "                    device",
  "                    ddr",
  "                    identifiers",
  "                    pcie",
  "                    msi",
  "                    smon",
0};

char *ca_msg[] =
{
  "Compare with mask the content of a location in AXI-4 space with data",
  "ca.<ds> <offset> <data> <mask>",
  "     where <ds>    = b,s,w -> data size: 1,2,4",
  "          <offset> = address offset in hexadecimal",
  "          <data>   = data to be compared in hexadecimal",
  "          <mask>   = mask to apply in hexadecimal",
  " ",
  "AXI-4 space is not always available !",
0};

char *cp_msg[] = 
{ 
  "Compare with mask the content of a location in PCI tree with data",
  "cp.<ds> <offset> <data> <mask>",
  "cp1.<ds> <offset> <data> <mask>",
  "cp2.<ds> <offset> <data> <mask>",
  "     where <ds>    = b,s,w -> data size: 1,2,4",
  "          <offset> = address offset in hexadecimal",
  "          <data>   = data to be compared in hexadecimal",
  "          <mask>   = mask to apply in hexadecimal",
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

char *da_msg[] =
{ "Display content of AXI-4 space",
  "da.<ds><sw> <start>[..<end>]",
  "   where <ds>    = b,s,w,l -> data size: 1,2,4,8",
  "         <sw>    = s       -> display swapped data",
  "         <start> = start offset in hexadecimal",
  "         <end>   = end offset in hexadecimal (default = <start> + 0x40)",
  " ",
  "AXI-4 space is not always available !",
0};

char *dc_msg[] = 
{ "  display content of a set of TSC PCI CFG registers",
  "  dc <start>[..<end>]",
  "     where <start> = offset in hexadecimal of first register",
  "            <end>  = offset in hexadecimal of last register",
0};

char *ddr_msg[]=
{
  "Perform specific command on SMEM DDR memory",
  "smem ddr calib <MEM>",
//  "smem ddr reset <MEM>",
//  "smem ddr status <MEM>",
//  "smem ddr set <MEM> <DQ> <STEP> <PM>",
  "   where smem ddr <calib | scalib> <MEM>     = SMEM DDR memory IDELAY alignment",
// "         smem ddr reset <MEM>                = Reset to default SMEM DDR memory IDELAY",
// "         smem ddr status <MEM>               = Show the SMEM DDR3 memory DQ[15:0] line selection",
//  "         smem ddr set <MEM> <DQ> <STEP> <PM> = Set the IDELAY for specifics lane",
  "                 where <calib | scalib>      = calib normal execution, scalib silent execution",
  "                 where <MEM>                 = Select the specific SMEM DDR3 1 or 2 or both 12",
//  "                 where <DQ>                  = Select the specific lane DQ[15:0]",
//  "                 where <STEP>                = Select the specific step for increment / decrement",
//  "                                               STEP = 1 to 16",
//  "                 where <PM>                  = Increment + STEP[ps] or decrement - STEP[ps]the IDELAY",
//  "                                               (STEP = 1 to 16)",
0};

char *di_msg[] = 
{ "Display content of a set of IFC PON registers",
  "di <start>[..<end>]",
  "   where <start> = offset in hexadecimal of first register",
  "          <end>  = offset in hexadecimal of last register",
0};

char *dk_msg[] =
{ "Display content of kernel buffer in System Memory",
  "dk<idx>.<ds><sw> <start>[..<end>]",
  "   where <idx>   = buffer index [0 -> 7]",
  "         <ds>    = b,s,w,l -> data size: 1,2,4,8",
  "         <sw>    = s       -> display swapped data",
  "         <start> = start offset in hexadecimal",
  "         <end>   = end offset in hexadecimal (default = <start> + 0x40)",
0};

char *dl_msg[] = 
{ "display content of AXI-4 Lite registers",
  "dl <start>[..<end>]",
  "   where <start> = offset in hexadecimal of first register",
  "          <end>  = offset in hexadecimal of last register",
0};

char *dm_msg[] = 
{ "Display content of kernel buffer in System Memory",
  "dm.<ds><sw> <start>[..<end>]",
  "   where <ds>    = b,s,w,l -> data size: 1,2,4,8",
  "         <sw>    = s       -> display swapped data",
  "         <start> = start offset in hexadecimal",
  "         <end>   = end offset in hexadecimal (default = <start> + 0x40)",
0};

char *dma_msg[] = 
{
  "Perform DMA operation using channel <x>",
  "dma.<x> start <des_start>:<des_space>[.s] <src_start>:<src_space>[.s] <size>",
  "dma.<x> status",
  "dma.<x> alloc",
  "dma.<x> free",
  "dma.<x> clear",
  "     where <x>     =  DMA channel (0 to 3)",
  "           <space> =  0 -> PCIe bus address EP#0",
  "                      1 -> PCIe bus address EP#1",
  "                      2 -> Shared Memory #1",
  "                      3 -> Shared Memory #2",
  "                      4 -> FPGA user area #1",
  "                      5 -> FPGA user area #2",
  "                      8 -> Test buffer allocated in system memory over PCIe EP#0",
  "                      9 -> Test buffer allocated in system memory over PCIe EP#1",
  "           If .s is appended to <space>, byte swapping if performed",
0};

char *dp_msg[] = 
{ "Display content in PCI tree",
  "dp1.<ds><sw> <start>[..<end>]",
  "dp2.<ds><sw> <start>[..<end>]",
  "   where <ds>    = b,s,w,l -> data size: 1,2,4,8",
  "         <sw>    = s       -> display swapped data",
  "         <start> = start address in hexadecimal",
  "         <end>   = end address in hexadecimal (default = <start> + 0x40)",
0};

char *dr_msg[] = 
{ "Display content of a set of TSC CSR registers",
  "dr <start>[..<end>]",
  "   where <start> = ofset in hexadecimal of first register",
  "         <end>   = ofset in hexadecimal of last register",
0};

char *ds_msg[] = 
{ "Display content of Shared Memory",
  "ds.<ds><sw> <start>[..<end>]",
  "ds1.<ds><sw> <start>[..<end>]",
  "ds2.<ds><sw> <start>[..<end>]",
  "   where <ds>    = b,s,w,l -> data size: 1,2,4,8",
  "         <sw>    = s       -> display swapped data",
  "         <start> = start address in hexadecimal",
  "         <end>   = end address in hexadecimal (default = <start> + 0x40)",
0};

char *du_msg[] =
{ "Display content of USR space",
  "du.<ds><sw> <start>[..<end>]",
  "du1.<ds><sw> <start>[..<end>]",
  "du2.<ds><sw> <start>[..<end>]",
  "   where <ds>    = b,s,w,l -> data size: 1,2,4,8",
  "         <sw>    = s       -> display swapped data",
  "         <start> = start address in hexadecimal",
  "         <end>   = end address in hexadecimal (default = <start> + 0x40)",
0};

char *fa_msg[] =
{ "Fill AXI-4 space with data",
  "fa.<ds> <start>..<end> <data>",
  "   where <ds>    = b,s,w,l -> data size: 1,2,4,8",
  "         <start> = start address in hexadecimal",
  "         <end>   = end address in hexadecimal (default = <start> + 0x40)",
  "         <data>  = data in hexadecimal",
  " ",
  "AXI-4 space is not always available !",
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
{ "Fill kernel buffer in System Memory",
  "fk<idx>.<ds> <start>..<end> <data>",
  "   where <idx>   = buffer index [0 -> 7]",
  "         <ds>    = b,s,w,l -> data size: 1,2,4,8",
  "         <start> = start address in hexadecimal",
  "         <end>   = end address in hexadecimal (default = <start> + 0x40)",
  "         <data>  = data in hexadecimal",
0};

char *fm_msg[] = 
{ "Fill kernel buffer in System Memory",
  "fm.<ds> <start>..<end> <data>",
  "   where <ds>    = b,s,w,l -> data size: 1,2,4,8",
  "         <start> = start address in hexadecimal",
  "         <end>   = end address in hexadecimal (default = <start> + 0x40)",
  "         <data>  = data in hexadecimal",
0};

char *fs_msg[] = 
{ "Fill Shared Memory with data",
  "fs.<ds> <start>..<end> <data>",
  "fs1.<ds> <start>..<end> <data>",
  "fs2.<ds> <start>..<end> <data>",
  "   where <ds>    = b,s,w,l -> data size: 1,2,4,8",
  "         <start> = start address in hexadecimal",
  "         <end>   = end address in hexadecimal (default = <start> + 0x40)",
  "         <data>  = data in hexadecimal",
0};

char *fp_msg[] = 
{ "Fill PCI tree addresses with data",
  "fp.<ds> <start>..<end> <data>",
  "fp1.<ds> <start>..<end> <data>",
  "fp2.<ds> <start>..<end> <data>",
  "   where <ds>    = b,s,w,l -> data size: 1,2,4,8",
  "         <start> = start address in hexadecimal",
  "         <end>   = end address in hexadecimal (default = <start> + 0x40)",
  "         <data>  = data in hexadecimal",
0};

char *fu_msg[] =
{ "Fill USR space with data",
  "fu.<ds> <start>..<end> <data>",
  "fu1.<ds> <start>..<end> <data>",
  "fu2.<ds> <start>..<end> <data>",
  "   where <ds>    = b,s,w,l -> data size: 1,2,4,8",
  "         <start> = start address in hexadecimal",
  "         <end>   = end address in hexadecimal (default = <start> + 0x40)",
  "         <data>  = data in hexadecimal",
0};

char *gscope_msg[]   = 
{
  "Perform Generic Scope operation",
  "gscope identify",
  "gscope.<x> acq trig <mode>",
  "         <x>        = fmc identifier (1 or 2)",
  "         <mode>     = man, gpio, adc",
  "if <mode> is adc the synthax is",
  "gscope.<x> acq trig adc.<offset>.<hyst> <chan> <type> <dir> [<sign>]",
  "         <offset>   = the level the signal needs to exceed to trigger the acquisition",
  "         <hyst>     = hysteresis value",
  "         <chan>     = the channel which will trigger the acquisition",
  "         <type>     = lvl or edge",
  "         <dir>      = above/below the level or rising/falling edge (up/down)",
  "         <sign>     = signed or unsigned offset (signed per default)",
  "gscope.<x> acq arm [<prop> <sync> <mode> <trig>]",
  "         <x>        = fmc identifier (1 or 2) or 3 for a double arm",
  "         <prop>     = pre trigger proportion of data ([0;7] out of 8, 4 per default)",
  "         <sync>     = master or slave (master per default)",
  "                      if <x> is 3, fmc 1 will be the <sync> and fmc 2 the opposite",
  "         <mode>     = continuous (cont) or single (sgl) acquisition (sgl per default)",
  "         <trig>     = trigger mode: normal or auto (normal per default)",
  "gscope acq status",
  "gscope.<x> acq abo",
  "         <x>        = fmc identifier (1 or 2) or 3 for the two at the same time",
  "gscope init smem <chanset> <base> <size>",
  "         <chanset>  = list of enabled channels",
  "         <base>     = first buffer base adress",
  "         <size>     = size of the buffers",
  "gscope save <chanset> <filename> [<size>]",
  "         <chanset>  = list of the channels",
  "         <filename> = generic name of the file",
  "         parameters: %c = channel",
  "                     %d = date",
  "                     %t = time",
  "         <size>     = size of the file",
  "gscope fwinfo.<reg> <slot>",
  "         <reg> = tosca,xapp,fmc,all",
  "             tosca      -> Short-hash from toasca2b git repository",
  "             xapp       -> Short-hash from xuser_scope_generic git repository",
  "             fmc <slot> -> Short-hash from the adc_311<x>_fdk git repository",
  "             all        -> all of the above",
0};

char *help_msg[]   = 
{
  "Display list of commands or syntax of command <cmd>",
  "help",
  "help <cmd>",
0};

char *history_msg[]   = 
{
  "Display history of commands",
  "Commands in history list can re-executed by entering '!' followed by history line number",
  "history",
0};

char *i2c_msg[]     = 
{
  "Perform i2c command ",
  "   i2c[.<bus]> <addr> read[.<ds>] <reg>[.<rs>]",
  "   i2c[.<bus]> <addr> write[.<ds>] <reg>[.<rs>] <data>",
  "   where <bus>  = bus idx (0 -> 7, default 0)",
  "         <addr> = device address (0 -> ff)",
  "         <ds>   = data size (1,2,3,4 bytes, default 1)",
  "         <rs>   = register size (1,2,3,4 bytes, default 1)",
  "         <reg>  = register idx",
  "         <data> = data",
0};

char *kbuf_msg[] = 
{
  "Kernel buffer operations",
  "kbuf.<id> alloc <size>",
  "kbuf.<id> show",
  "kbuf.<id> free",
  "kbuf.<id> map <map> [<offset>]",
  "kbuf.<id> unmap",
  "   where <id>     = buffer identifier [0->7]",
  "         <size>   = buffer size",
  "         <offset> = requested address offset (optional)",
0};

char *la_msg[]     = 
{ 
  "Read/write loop from/to AXI-4 space",
  "la.<ds> <offset> <data> [l:<loop>]",
  "   where <ds>     = b, s, w, l [data size 1, 2, 4, 8]",
  "         <offset> = PCI address offset in hexadecimal",
  "         <data>   = data in hexadecimal [write cycle]",
  "         <loop>   = loop count (0 -> infinite)",
  "         If s is appended to <ds>, byte swapping if performed",
  " ",
  "AXI-4 space is not always available !",
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

char *lp_msg[]     = 
{ 
  "Read/write loop from/to PCI addresses",
  "lp<i>.<ds> <offset> <data> [l:<loop>]",
  "   where <i>      = 1, 2 [PCI tree index]",
  "         <ds>     = b, s, w, l [data size 1, 2, 4, 8]",
  "         <offset> = PCI address offset in hexadecimal",
  "         <data>   = data in hexadecimal [write cycle]",
  "         <loop>   = loop count (0 -> infinite)",
  "         If s is appended to <ds>, byte swapping if performed",
0};

char *ls_msg[]     = 
{ 
  "Read/write loop from/to shared memory",
  "ls<i>.<ds> <offset> <data> [l:<loop>]",
  "   where <i>      = 1, 2 [shared memory index]",
  "         <ds>     = b, s, w, l [data size 1, 2, 4, 8]",
  "         <offset> = shared memory address offset in hexadecimal",
  "         <data>   = data in hexadecimal [write cycle]",
  "         <loop>   = loop count (0 -> infinite)",
  "         If s is appended to <ds>, byte swapping if performed",
0};

char *lu_msg[]     = 
{ 
  "Read/write loop from/to user area",
  "lu<i>.<ds> <offset> <data> [l:<loop>]",
  "   where <i>      = 1, 2 [user area index]",
  "         <ds>     = b, s, w, l [data size 1, 2, 4, 8]",
  "         <offset> = USR address offset in hexadecimal",
  "         <data>   = data in hexadecimal [write cycle]",
  "         <loop>   = loop count (0 -> infinite)",
  "         If s is appended to <ds>, byte swapping if performed",
0};

char *map_msg[] = 
{
  "Address mapping operations",
  "map show [<map>]",
  "map clear <map>",
  "map.<space> alloc <map> <addr> <size>[<offset>]",
  "map free <map> offset",
  "   where <map>    = mas_mem, mas_pmem",
  "         <space>  = p,s or u -> remote space ( PCI, Shared Memory or USER)",
  "         <addr>   = remote address in hexadecimal",
  "         <size>   = requested mapping size",
  "         <offset> = requested address offset (optional)",
0};

char *mbox_msg[] = 
{
  "MTCA.4 PON mailbox operations",
  "  mbox read <address> [<count>]",
  "  mbox write <address> <value>",
  "  mbox info",
  "  mbox payload_sensor show [<name]>",
  "  mbox payload_sensor create <name> <size>",
  "   where <size>    = 1..4",
  "  mbox payload_sensor set <name> <value>",
0};

char *pa_msg[] =
{ "Read/write data from/to AXI-4 space",
  "  pa.<ds> <offset> [<data>]",
  "     where <ds>     = b,s,w,l -> data size: 1,2,4,8",
  "           <offset> = address offset in hexadecimal",
  "           <data>   = data in hexadecimal [write cycle]",
  " ",
  "AXI-4 space is not always available !",
0};

char *pc_msg[] = 
{ "Read/write data from/to PCI CFG register",
  "  pc <offset> [<data>]",
  "     where <offset> = register offset in hexadecimal",
  "           <data>   = data in hexadecimal [write cycle]",
0};

char *pi_msg[] = 
{ "Read/write data from/to IFC PON register",
  "  pi <offset> [<data>]",
  "     where <offset> = register offset in hexadecimal",
  "           <data>   = data in hexadecimal [write cycle]",
0};

char *pk_msg[] =
{ "Read/write data from/to kernel buffer in System Memory",
  "  pk<idx>.<ds> <offset> [<data>]",
  "     where <idx>    = buffer index [0 -> 7]",
  "           <ds>     = b,s,w,l -> data size: 1,2,4,8",
  "           <offset> = address offset in hexadecimal",
  "           <data>   = data in hexadecimal [write cycle]",
0};

char *pl_msg[] =
{ "Read/write data from/to AXI-4 Lite register",
  "  pl <offset> [<data>]",
  "     where <offset> = register address in hexadecimal",
  "           <data>   = data in hexadecimal [write cycle]",
0};

char *pm_msg[] = 
{ "Read/write data from/to kernel buffer in System Memory",
  "  pm.<ds> <offset> [<data>]",
  "     where <ds>     = b,s,w,l -> data size: 1,2,4,8",
  "           <offset> = address offset in hexadecimal",
  "           <data>   = data in hexadecimal [write cycle]",
0};

char *pp_msg[] = 
{ "Read/write data from/to PCI tree address",
  "  pp1.<ds> <offset> [<data>]",
  "  pp2.<ds> <offset> [<data>]",
  "     where <ds>     = b,s,w,l -> data size: 1,2,4,8",
  "           <offset> = address offset in hexadecimal",
  "           <data>   = data in hexadecimal [write cycle]",
0};

char *pr_msg[] = 
{ "Read/write data from/to CSR register",
  "  pr <offset> [<data>]",
  "     where <offset> = register offset in hexadecimal",
  "           <data>   = data in hexadecimal [write cycle]",
0};

char *ps_msg[] = 
{ "Read/write data from/to Shared Memory",
  "  ps.<ds> <offset> [<data>]",
  "  ps1.<ds> <offset> [<data>]",
  "  ps2.<ds> <offset> [<data>]",
  "     where <ds>     = b,s,w,l -> data size: 1,2,4,8",
  "           <offset> = address offset in hexadecimal",
  "           <data>   = data in hexadecimal [write cycle]",
0};

char *pu_msg[] =
{ "Read/write data from/to FPGA USR space",
  "  pu.<ds> <offset> [<data>]",
  "  pu1.<ds> <offset> [<data>]",
  "  pu2.<ds> <offset> [<data>]",
  "     where <ds>     = b,s,w,l -> data size: 1,2,4,8",
  "           <offset> = address offset in hexadecimal",
  "           <data>   = data in hexadecimal [write cycle]",
0};

char *rsp1461_msg[] =
{ "rsp1461 control command",
  "   rsp1461 init",
  "",
  "   rsp1461 extension present",
  "",
  "   rsp1461 extension status",
  "",
  "   rsp1461 extension <control> <pin>",
  "      where control = 0 -> low, 1 -> high, 2 -> z",
  "            pin     = 0, 1, 2, 3, 4, 5, 6",
  "",
  "   rsp1461 extension get <pin>",
  "      where pin = 0, 1, 2, 3, 4, 5, 6",
  "",
  "   rsp1461 led on <id>",
  "      where id = 0 -> LED123_GREEN, 1 -> LED123_RED, 2 -> LED124_GREEN, 3 -> LED124_RED",
  "                 4 -> LED125_GREEN, 5 -> LED125_RED, 6 -> LED126_GREEN, 7 -> LED126_RED",
  "",
  "   rsp1461 led off <id>",
  "      where id = 0 -> LED123_GREEN, 1 -> LED123_RED, 2 -> LED124_GREEN, 3 -> LED124_RED",
  "                 4 -> LED125_GREEN, 5 -> LED125_RED, 6 -> LED126_GREEN, 7 -> LED126_RED",
  "",
  "   rsp1461 sfp status <id>",
  "      where id = fpga0, fpga1, fpga2, fpga3, eth0, eth1, eth2, all",
  "",
  "   rsp1461 sfp control <enable> <rx_rate> <tx_rate> <id>",
  "      where enable  = enable, disable",
  "            rx_rate = low, high",
  "            tx_rate = low, high",
  "            id      = fpga0, fpga1, fpga2, fpga3, eth0, eth1, eth2",
0};

char *rdt1465_msg[] =
{ "rdt1465 control command",
  "   rdt1465 init",
  "",
  "   rdt1465 analog <control>",
  "     where control = on, off",
  "",
  "   rdt1465 gpio_out <control>",
  "     where control = on, off",
  "",
  "   rdt1465 red_led <control>",
  "     where control = on, off",
  "",
  "   rdt1465 extension present",
  "",
  "   rdt1465 extension status",
  "",
  "   rdt1465 extension <control> <pin>",
  "      where control = 0 -> low, 1 -> high, 2 -> z",
  "            pin     = 0 to 11",
  "",
  "   rdt1465 extension get <pin>",
  "      where pin = 0 to 11",
  "",
0};

char *rcc1466_msg[] =
{ "rcc1466 control command",
  "   rcc1466 init",
  "",
  "   rcc1466 extension present",
  "",
  "   rcc1466 led on <id>",
  "      where id = 0 -> LED123_GREEN, 1 -> LED123_RED, 2 -> LED124_GREEN, 3 -> LED124_RED",
  "                 4 -> LED125_GREEN, 5 -> LED125_RED, 6 -> LED126_GREEN, 7 -> LED126_RED",
  "",
  "   rcc1466 led off <id>",
  "      where id = 0 -> LED123_GREEN, 1 -> LED123_RED, 2 -> LED124_GREEN, 3 -> LED124_RED",
  "                 4 -> LED125_GREEN, 5 -> LED125_RED, 6 -> LED126_GREEN, 7 -> LED126_RED",
  "",
  "   rcc1466 sfp status <id>",
  "      where id = fpga0, fpga1, fpga2, fpga3, eth0, all",
  "",
  "   rcc1466 sfp control <enable> <rx_rate> <tx_rate> <id>",
  "      where enable  = enable, disable",
  "            rx_rate = low, high",
  "            tx_rate = low, high",
  "            id      = fpga0, fpga1, fpga2, fpga3, eth0",
0};

char *rtm_msg[] = 
{
  "MTCA.4 RTM operations",
  "rtm led <led_name> <led_state>",
  "   where <led_name> = green, red",
  "         <led_state> = on, off, slow_blink, fast_blink",
  "",
  "rtm eeprom <state>",
  "   where <state> = we (write enabled), wp (write protected)",
  "",
  "rtm reset <state>",
  "   where <state> = on, off",
  "",
  "rtm zone3 <state>",
  "   where <state> = on, off",
0};

char *semaphore_msg[]=
{
  "Perform semaphore operations",
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
  "Perform sflash operations",
  "sflash rdid",
  "sflash rdsr",
  "sflash wrsr <sr>",
  "sflash dump <off> <len>",
  "sflash load <off> <file>",
  "sflash dynopt",
0};

char *ta_msg[]  = 
{
  "Perform read/write test in AXI-4 space",
  "ta.<ds> <start>..<end> <data> [<loop>]",
  "   where <ds>    = b, s, w, l [data size 1, 2, 4, 8]",
  "         <start> = address offset in hexadecimal of first  location",
  "         <end>   = address offset in hexadecimal of last location",
  "         <data>  = data in hexadecimal",
  "         <loop>  = number of loop",
  " ",
  "AXI-4 space is not always available !",
0};

char *tdma_msg[] = 
{
  "Perform CPU DMA operations using channel <x>",
  "dma.<x> start <des_start>:<des_space>[.s] <src_start>:<src_space>[.s] <size>",
  "dma.<x> status",
0};

char *timer_msg[] = 
{
  "Perform operation on TSC global timer ",
  "timer start   -> Start timer",
  "timer restart -> Restart the timer",
  "timer stop    -> Stop the timer",
  "timer read    -> Acquire the timer value",
  "timer date    -> Acquire the timer value in a date format",
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

char *tp_msg[]  = 
{
  "Perform read/write test in PCI tree address space",
  "tp.<ds> <start>..<end> <data> [<loop>]",
  "tp1.<ds> <start>..<end> <data> [<loop>]",
  "tp2.<ds> <start>..<end> <data> [<loop>]",
  "   where <ds>    = b, s, w, l [data size 1, 2, 4, 8]",
  "         <start> = address offset in hexadecimal of first  location",
  "         <end>   = address offset in hexadecimal of last location",
  "         <data>  = data in hexadecimal",
  "         <loop>  = number of loop",
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

char *wait_msg[] =
{
  "Wait until a character is entered",
  "wait [<tmo>]",
  "   where <tmo> = timeout in sec",
0};

struct cli_cmd_list cmd_list[] =
{
  { "acq1430"   , tsc_acq1430,      acq1430_msg   , 0},
  { "adc3110"   , tsc_adc3110,      adc3110_msg   , 0},
  { "adc3112"   , tsc_adc3112,      adc3112_msg   , 0},
  { "adc3117"   , tsc_adc3117,      adc3117_msg   , 0},
  { "alias"     , tsc_alias,        alias_msg     , 0},
  { "amc"       , tsc_amc,          amc_msg       , 0},
  { "ci"        , tsc_rdwr_cr,      ci_msg        , 0},
  { "cl"        , tsc_rdwr_cr,      cl_msg        , 0},
  { "cmp"       , tsc_rdwr_cmp,     cmp_msg       , 0},
  { "conf"      , tsc_conf_show,    conf_msg      , 0},
  { "ca"        , tsc_rdwr_cx,      ca_msg        , 0},
  { "cp1"       , tsc_rdwr_cx,      cp_msg        , 0},
  { "cp2"       , tsc_rdwr_cx,      cp_msg        , 0},
  { "cr"        , tsc_rdwr_cr,      cr_msg        , 0},
  { "cs1"       , tsc_rdwr_cx,      cs_msg        , 0},
  { "cs2"       , tsc_rdwr_cx,      cs_msg        , 0},
  { "cs"        , tsc_rdwr_cx,      cs_msg        , 0},
  { "cu1"       , tsc_rdwr_cx,      cu_msg        , 0},
  { "cu2"       , tsc_rdwr_cx,      cu_msg        , 0},
  { "cu"        , tsc_rdwr_cx,      cu_msg        , 0},
  { "da"        , tsc_rdwr_dx,      da_msg        , 0},
  { "dc"        , tsc_rdwr_dr,      dc_msg        , 0},
  { "di"        , tsc_rdwr_dr,      di_msg        , 0},
  { "dk"        , tsc_rdwr_dx,      dk_msg        , 0},
  { "dl"        , tsc_rdwr_dr,      dl_msg        , 0},
  { "dma"       , tsc_dma    ,      dma_msg       , 0},
  { "dm"        , tsc_rdwr_dx,      dm_msg        , 0},
  { "dp1"       , tsc_rdwr_dx,      dp_msg        , 0},
  { "dp2"       , tsc_rdwr_dx,      dp_msg        , 0},
  { "dr"        , tsc_rdwr_dr,      dr_msg        , 0},
  { "ds1"       , tsc_rdwr_dx,      ds_msg        , 0},
  { "ds2"       , tsc_rdwr_dx,      ds_msg        , 0},
  { "ds"        , tsc_rdwr_dx,      ds_msg        , 0},
  { "du1"       , tsc_rdwr_dx,      du_msg        , 0},
  { "du2"       , tsc_rdwr_dx,      du_msg        , 0},
  { "du"        , tsc_rdwr_dx,      du_msg        , 0},
  { "fa"        , tsc_rdwr_fx,      fa_msg        , 0},
  { "fifo"      , tsc_fifo,         fifo_msg      , 0},
  { "fk"        , tsc_rdwr_fx,      fk_msg        , 0},
  { "fp1"       , tsc_rdwr_fx,      fp_msg        , 0},
  { "fp2"       , tsc_rdwr_fx,      fp_msg        , 0},
  { "fm"        , tsc_rdwr_fx,      fm_msg        , 0},
  { "fs1"       , tsc_rdwr_fx,      fs_msg        , 0},
  { "fs2"       , tsc_rdwr_fx,      fs_msg        , 0},
  { "fs"        , tsc_rdwr_fx,      fs_msg        , 0},
  { "fu"        , tsc_rdwr_fx,      fu_msg        , 0},
  { "fu1"       , tsc_rdwr_fx,      fu_msg        , 0},
  { "fu2"       , tsc_rdwr_fx,      fu_msg        , 0},
  { "gscope"    , tsc_gscope,       gscope_msg    , 0},
  { "help"      , tsc_func_help,    help_msg      , 0},
  { "his"       , tsc_func_history, history_msg   , 0},
  { "i2c"       , tsc_i2c,          i2c_msg       , 0},
  { "kbuf"      , tsc_kbuf,         kbuf_msg      , 0},
  { "la"        , tsc_rdwr_lx,      la_msg        , 0},
  { "lmk"       , tsc_lmk,          lmk_msg       , 0},
  { "lp1"       , tsc_rdwr_lx,      lp_msg        , 0},
  { "lp2"       , tsc_rdwr_lx,      lp_msg        , 0},
  { "ls1"       , tsc_rdwr_lx,      ls_msg        , 0},
  { "ls2"       , tsc_rdwr_lx,      ls_msg        , 0},
  { "ls"        , tsc_rdwr_lx,      ls_msg        , 0},
  { "lu2"       , tsc_rdwr_lx,      lu_msg        , 0},
  { "lu1"       , tsc_rdwr_lx,      lu_msg        , 0},
  { "lu"        , tsc_rdwr_lx,      lu_msg        , 0},
  { "map"       , tsc_map,          map_msg       , 0},
  { "mbox"      , tsc_mbox,         mbox_msg      , 0},
  { "pa"        , tsc_rdwr_px,      pa_msg        , 0},
  { "pc"        , tsc_rdwr_pr,      pc_msg        , 0},
  { "pi"        , tsc_rdwr_pr,      pi_msg        , 0},
  { "pk"        , tsc_rdwr_px,      pk_msg        , 0},
  { "pl"        , tsc_rdwr_pr,      pl_msg        , 0},
  { "pm"        , tsc_rdwr_px,      pm_msg        , 0},
  { "pp1"       , tsc_rdwr_px,      pp_msg        , 0},
  { "pp2"       , tsc_rdwr_px,      pp_msg        , 0},
  { "pr"        , tsc_rdwr_pr,      pr_msg        , 0},
  { "ps1"       , tsc_rdwr_px,      ps_msg        , 0},
  { "ps2"       , tsc_rdwr_px,      ps_msg        , 0},
  { "ps"        , tsc_rdwr_px,      ps_msg        , 0},
  { "pu1"       , tsc_rdwr_px,      pu_msg        , 0},
  { "pu2"       , tsc_rdwr_px,      pu_msg        , 0},
  { "pu"        , tsc_rdwr_px,      pu_msg        , 0},
  { "rsp1461"   , tsc_rsp1461,      rsp1461_msg   , 0},
  { "rdt1465"   , tsc_rdt1465,      rdt1465_msg   , 0},
  { "rcc1466"   , tsc_rcc1466,      rcc1466_msg   , 0},
  { "rtm"       , tsc_rtm,          rtm_msg       , 0},
  { "semaphore" , tsc_semaphore,    semaphore_msg , 0},
  //{ "sflash"  , tsc_sflash,       sflash_msg    , 0},
  { "set"       , tsc_set_device,   set_device_msg, 0},
  { "smem"      , tsc_ddr,          ddr_msg       , 0},
  { "ta"        , tsc_rdwr_tx,      ta_msg        , 0},
  { "tdma"      , tsc_tdma,         tdma_msg      , 0},
  { "timer"     , tsc_timer,        timer_msg     , 0},
  { "tinit"     , tsc_tinit,        tinit_msg     , 0},
  { "tkill"     , tsc_tkill,        tkill_msg     , 0},
  { "tlist"     , tsc_tlist,        tlist_msg     , 0},
  { "tstatus"   , tsc_tstatus,      tstatus_msg   , 0},
  { "tstart"    , tsc_tstart,       tstart_msg    , 0},
  { "tstop"     , tsc_tstop,        tstop_msg     , 0},
  { "tset"      , tsc_tset,         tset_msg      , 0},
  { "tp1"       , tsc_rdwr_tx,      tp_msg        , 0},
  { "tp2"       , tsc_rdwr_tx,      tp_msg        , 0},
  { "ts1"       , tsc_rdwr_tx,      ts_msg        , 0},
  { "ts2"       , tsc_rdwr_tx,      ts_msg        , 0},
  { "ts"        , tsc_rdwr_tx,      ts_msg        , 0},
  { "tu1"       , tsc_rdwr_tx,      tu_msg        , 0},
  { "tu2"       , tsc_rdwr_tx,      tu_msg        , 0},
  { "tu"        , tsc_rdwr_tx,      tu_msg        , 0},
  { "twait"     , tsc_twait,        twait_msg     , 0},
  { "wait"      , tsc_wait,         wait_msg      , 0},
  { (char *)0,} 
};
