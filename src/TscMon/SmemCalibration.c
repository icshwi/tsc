/*=========================< begin file & file header >=======================
 *  References
 *
 *    filename : SmemCalibration.c
 *    author   : XP
 *    company  : IOxOS
 *    creation : Mars 2018
 *
 *----------------------------------------------------------------------------
 *  Description
 *
 *    This file contains the core of the SmemCalibration function
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

#include <sys/types.h>
#include <sys/time.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <ctype.h>
#include <pty.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/mman.h>

#include <cli.h>
#include <tscioctl.h>
#include <tsculib.h>
#include "TscMon.h"

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * Function name : tsc_ddr_is_calibrated
 * Prototype     : int
 * Parameters    : -
 * Return        : 0  = not calibrated,                 need a calibration
 *                 -1 = timeout waiting on calibration, need a calibration
 *                 1  = already calibrated
 *----------------------------------------------------------------------------
 * Description   : verify if ddr memory is already calibrated
 *
 *++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

static int tsc_ddr_is_calibrated(int tsc_fd)
{
  int data, to = 1000;

  while (to>0)
  {
    /* verify if SMEM is calibrated and take the semaphore is if not calibrated */
    tsc_csr_read_dbg(tsc_fd, TSC_CSR_SMEM_DDR3_CALSEM, &data);

    /* calibration semaphore register not present ? -> calibration needed */
    if ((data & 0xe0000000) == TSC_SMEM_DDR3_CALSEM_ABSENT)
    {
      return(0);
    }

    /* semaphore already taken -> we should wait */
    if ((data & 0xe0000000) == TSC_SMEM_DDR3_CALSEM_OWNED)
    {
      to--;
      continue;
    }

    /* calibration done ! -> ok already calibrated */
    if ((data & 0xe0000000) == TSC_SMEM_DDR3_CALSEM_DONE)
    {
      return(1);
    }

    /* Calibration need to be done (or semaphore not available) ? -> do it */
    if ((data & 0xe0000000) != TSC_SMEM_DDR3_CALSEM_DONE)
    {
      return(0);
    }
  }
  /* reset semaphore and redo the calibration */
  data = 0;
  tsc_csr_write_dbg(tsc_fd, TSC_CSR_SMEM_DDR3_CALSEM, &data);

  /* timeout */
  return (-1);
}

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * Function name : tsc_ddr_set_calibration_done
 * Prototype     : int
 * Parameters    : -
 * Return        : success/error
 *----------------------------------------------------------------------------
 * Description   : set calibration done flag for the ddr memory
 *
 *++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

static void tsc_ddr_set_calibration_done(int tsc_fd, int done)
{
  int data;
  data = ((done!=0) ? TSC_SMEM_DDR3_CALSEM_DONE : 0);
  tsc_csr_write_dbg(tsc_fd, TSC_CSR_SMEM_DDR3_CALSEM, &data);
}

void usage()
{
	printf("SmemCalibration usage:\n");
	printf("Run the DDR calibration routine on card in pcie slot <slotnr>:\ntscmon s<slotnr>\n");
	printf("Calibrate DDR and power on FMC's and exit:\ntscmon e\n");
	printf("Run SmemCalibration in quiet mode:\ntscmon q\n\n");
	printf("q\t\t- Quiet mode, reduces prints\n");
	printf("s<slotnr>\t- PCIe slot number\n");
	printf("h\t\t- Help, prints this (also -h and --help)\n\n");
}

int main(int argc, char * argv[]){

    // SMEM control & status register
    unsigned int SMEM_DDR3_CSR[2] = {0x800, 0xc00};

    // IDEL adjustment register for both DDR3 memory
    unsigned int SMEM_DDR3_IFSTA[2] = {0x808, 0xc08};

    // IDEL control register for both DDR3 memory
    unsigned int SMEM_DDR3_IDEL[2] = {0x80c, 0xc0c};

    ////////////////////////////////////////////////////////////////////////////////////
    // ADJUST DEFAULT VALUE FOR CURRENT_DLY ACCORDING TO HARDWARE IMPLEMENTATION      //
    unsigned int    CURRENT_DLY     = 256;                                            //
    // ADJUST DEFAULT STEP VALUE FOR INC / DEC VALUE                                  //
    unsigned int    CURRENT_STEP    = 4;                                              //
    // ADJUSTE DEFAULT INIT DELAY                                                     //
    unsigned int    DEFAULT_DELAY   = 0x40;                                           //
    // MAX DELAY VALUE                                                                //
    unsigned int    MAX             = 0x1ff;                                          //
    ////////////////////////////////////////////////////////////////////////////////////

    struct tsc_ioctl_map_win map_win;
    int             quiet           = 0;
    int             tsc_fd          = -1;
    int             DQ_NOK[16];
    int             DQ_OK[16];
    int             ppc             = 0;
    float           f0, f1, f2      = 0.0;
    int             retval          = 0;
    unsigned int    *buf_ddr        = NULL;     // Buffer mapped directly in DDR3 area
    unsigned int    *buf_tx         = NULL;     // Locally buffer to send data to DDR3
    unsigned int    *buf_rx         = NULL;     // Locally buffer to receive data from DDR3
    unsigned int    *buf_tx_start   = NULL;
    unsigned int    size            = 0x40;
    unsigned int    offset          = 0x100000; // DDR3 offset memory
    int             d0, d1, d2      = 0;
    int             data            = 0;
    int             cnt_value       = 0;
    unsigned int    memOrg          = 0;
    unsigned int    mem             = 0x01;
    unsigned int    init_delay_1[16]          = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
    unsigned int    init_delay_2[16]          = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
    unsigned int    temp_cnt_value_store[16]  = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
    unsigned int    final_cnt_value_store[16] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
    unsigned int    final_cnt_value_store1[16] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
    int             dq_path         = 0;
    unsigned int    r, rr           = 0;
    unsigned int    best            = 0;
    unsigned int    worst           = 0;
    unsigned int    location        = 0;
    int             vtc_read        = 0;
    int             vtc_set         = 0;
    unsigned int    j, k, m, n      = 0;       // Loop increment
    unsigned int    start           = 0;       // Save the start index
    unsigned int    end             = 0;       // Save the end index
    unsigned int    ok              = 0;       // Count the number of passed test "1"
    unsigned int    avg_x           = 0;       // Horizontal DQ average delay for 16 lines of DQ
    unsigned int    marker          = 0;       // Final delay marker value per line
    unsigned int    NOK             = 1;       // Calibration is done or not
    unsigned int    pattern[32]     = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
    unsigned int    ref_pattern[32] = {0, 1, 0, 1, 1, 0, 0, 1, 0, 0, 1, 1, 0, 1, 0, 0, 1, 0, 1, 1, 0, 1, 0, 1, 1, 0, 0, 1, 0, 0, 1, 1};
    int slot = 0;
    int i = 0;
    unsigned int expected_val = 0;
    // 32 bits reference pattern for each DQ : 0101 1001 0011 0100 1011 0101 1001 0011

    unsigned int word0  = 0xffff0000; // 1111 1111 1111 1111 0000 0000 0000 0000
    unsigned int word1  = 0xffff0000; // 1111 1111 1111 1111 0000 0000 0000 0000
    unsigned int word2  = 0x0000ffff; // 0000 0000 0000 0000 1111 1111 1111 1111
    unsigned int word3  = 0xffff0000; // 1111 1111 1111 1111 0000 0000 0000 0000
    unsigned int word4  = 0x00000000; // 0000 0000 0000 0000 0000 0000 0000 0000
    unsigned int word5  = 0xffffffff; // 1111 1111 1111 1111 1111 1111 1111 1111
    unsigned int word6  = 0xffff0000; // 1111 1111 1111 1111 0000 0000 0000 0000
    unsigned int word7  = 0x00000000; // 0000 0000 0000 0000 0000 0000 0000 0000
    unsigned int word8  = 0x0000ffff; // 0000 0000 0000 0000 1111 1111 1111 1111
    unsigned int word9  = 0xffffffff; // 1111 1111 1111 1111 1111 1111 1111 1111
    unsigned int word10 = 0xffff0000; // 1111 1111 1111 1111 0000 0000 0000 0000
    unsigned int word11 = 0xffff0000; // 1111 1111 1111 1111 0000 0000 0000 0000
    unsigned int word12 = 0x0000ffff; // 0000 0000 0000 0000 1111 1111 1111 1111
    unsigned int word13 = 0xffff0000; // 1111 1111 1111 1111 0000 0000 0000 0000
    unsigned int word14 = 0x00000000; // 0000 0000 0000 0000 0000 0000 0000 0000
    unsigned int word15 = 0xffffffff; // 1111 1111 1111 1111 1111 1111 1111 1111

    ppc = CheckByteOrder(); // Check endianness: 1-> ppc, 0-> x86

	while (i < argc) {
		if (!strncmp(argv[i], "q", 1)) {
			quiet = 1;
		}
		else if (!strncmp(argv[i], "s", 1)) {
			if (!ppc)
				slot = (int)strtol(&argv[i][1], NULL, 10);
		}
		else if (!strncmp(argv[i], "h", 1)  ||
			 !strncmp(argv[i], "-h", 2) ||
			 !strncmp(argv[i], "--help", 6)) {
			usage();
			return 0;
		}
		i++;
	}

	tsc_fd = tsc_init(slot);
	if(tsc_fd < 0){
		printf("Cannot find interface\n");
		usage();
		return -1;
	}

    if(argc == 2){
        if (!strcmp(argv[1], "-q")){
            //Quiet mode
            quiet = 1;
        }
    }

    // Check if calibration is needed for mem1, mem2 or mem1 & mem2
    if(mem == 0x1){
        mem = 1;
        memOrg = mem;
        rr = 1;
    }
    else if(mem == 0x2){
        mem = 2;
        memOrg = mem;
        rr = 1;
    }
    else if(mem == 0x12){
        mem = 1;
        memOrg = mem;
        rr = 2;
    }

      //  if (tsc_ddr_is_calibrated(tsc_fd) == 1)
      // {
      //   if (!quiet)
      //   {
      //       printf("Calibration already done !\n");
      //   }
      // return (0);
 //  }

    /* Reset memory controller */
    /***************************************************************************/
    // Only on the first memory due to the fact that reset impact both memory
    // Calibration need to be done in the order : 1 -> 2
     // Don't modify the bit 7
    if(mem == 1){
        printf ("Reset memory controller \n");
        tsc_csr_read_dbg(tsc_fd, SMEM_DDR3_CSR[mem - 1], &data);
        data = 0x8000 | (data & (1 << 7));
        tsc_csr_write_dbg(tsc_fd, SMEM_DDR3_CSR[mem - 1], &data);

        usleep(20000);

        tsc_csr_read_dbg(tsc_fd, SMEM_DDR3_CSR[mem - 1], &data);

        data = 0x2000 | (data & (1 << 7));
        tsc_csr_write_dbg(tsc_fd, SMEM_DDR3_CSR[mem - 1], &data);

 
        usleep(20000);
        printf ("Readback DDR3 controller status:");
        tsc_csr_read_dbg(tsc_fd, SMEM_DDR3_CSR[mem - 1], &data);
    }

    /* INITIAL READ */
    
    printf("Initial read \n");
    
 
    /***************************************************************************/

    // Loop on 2 SMEM
    for (r = 0; r < rr; r++){
        // Reset calibration register
        printf ("Reset calibration register \n");
        data = 0;
        tsc_csr_write_dbg(tsc_fd, SMEM_DDR3_IFSTA[mem - 1], &data);
        tsc_csr_write_dbg(tsc_fd, SMEM_DDR3_IDEL[mem - 1], &data);
if (!quiet) {
        printf("Initial value for MEM%x : \n", mem);
}
        // Loop on 16 DQ
        for(j = 0; j < 1; j++){
            printf("\n>>>Loop to store initial value of count of the IFSTA reg - Interaction [%d]<<< \n", j);
            // Store initial value of count of the IFSTA register
            dq_path = (j << 12);
            tsc_csr_write_dbg(tsc_fd, SMEM_DDR3_IFSTA[mem - 1], &dq_path);
            tsc_csr_read_dbg(tsc_fd, SMEM_DDR3_IDEL[mem - 1], &vtc_read);           // Acquire current value of the register
            vtc_set = (vtc_read | (1 << 28));                           // Set value to disable VTC
            tsc_csr_write_dbg(tsc_fd, SMEM_DDR3_IDEL[mem - 1], &vtc_set);           // Disable VTC
            tsc_csr_read_dbg(tsc_fd, SMEM_DDR3_IFSTA[mem - 1], &cnt_value);         // Read initial value of IFSTA register
            tsc_csr_write_dbg(tsc_fd, SMEM_DDR3_IDEL[mem - 1], &vtc_read);          // Re-active active VTC

            // MEM1
            if(r == 0) {
                init_delay_1[j]         = cnt_value & 0x1ff;
if (!quiet) {
                printf("DQ[%02i] IFSTA register 0x%08x -> Initial delay 0x%03x \n", j, cnt_value, init_delay_1[j]);
}
            }
            // MEM2
            else if (r == 1){
                init_delay_2[j]         = cnt_value & 0x1ff;
if (!quiet) {
                printf("DQ[%02i] IFSTA register 0x%08x -> Initial delay 0x%03x \n", j, cnt_value, init_delay_2[j]);
}
            }
        }
        mem++;
    }
if (!quiet) {
    printf("\n");
}
    /* CALIBRATION */
    /***************************************************************************/

    mem = memOrg;
    printf ("CALIBRATION... \n");
    // Global loop for DDR calibration
    for (r = 0; r < rr; r++){

        // Buffer definition
        buf_tx       = malloc(size);
        buf_rx       = malloc(size);
        buf_tx_start = buf_tx;

        // Map DDR3 memory region
        buf_ddr = NULL;
        memset(&map_win, 0, sizeof(map_win));
        map_win.req.rem_addr   = offset;
        map_win.req.loc_addr   = 0;
        map_win.req.size       = size;
        map_win.req.mode.sg_id = MAP_ID_MAS_PCIE_MEM;

        if ((mem - 1) == 0){
            map_win.req.mode.space = MAP_SPACE_SHM1; // SHM #1
if (!quiet) {
            printf("Calibration of the SMEM1\n");
}
        }
        else if((mem - 1) == 1){
            map_win.req.mode.space = MAP_SPACE_SHM2; // SHM #2
if (!quiet) {
            printf("Calibration of the SMEM2\n");
}
        }

        map_win.req.mode.flags = 0;
        retval = tsc_map_alloc(tsc_fd, &map_win);
        if(retval < 0){
            printf("Error in mapping SHM");
            tsc_ddr_set_calibration_done(tsc_fd, 0);
                        return (-1);
        }

        buf_ddr = mmap(NULL, map_win.req.size, PROT_READ | PROT_WRITE, MAP_SHARED, tsc_fd, map_win.req.loc_addr);
        if(buf_ddr == MAP_FAILED){
            printf("Error MAP FAILED \n");
            tsc_ddr_set_calibration_done(tsc_fd, 0);
            return (-1);
        }

        // Init local tx buffer with specific pattern
        *buf_tx = word0;
        buf_tx++;
        *buf_tx = word1;
        buf_tx++;
        *buf_tx = word2;
        buf_tx++;
        *buf_tx = word3;
        buf_tx++;
        *buf_tx = word4;
        buf_tx++;
        *buf_tx = word5;
        buf_tx++;
        *buf_tx = word6;
        buf_tx++;
        *buf_tx = word7;
        buf_tx++;
        *buf_tx = word8;
        buf_tx++;
        *buf_tx = word9;
        buf_tx++;
        *buf_tx = word10;
        buf_tx++;
        *buf_tx = word11;
        buf_tx++;
        *buf_tx = word12;
        buf_tx++;
        *buf_tx = word13;
        buf_tx++;
        *buf_tx = word14;
        buf_tx++;
        *buf_tx = word15;

        buf_tx = buf_tx_start;
		if (!quiet) {
				// Acquire temperature and voltage of current system
				d0 = 0x3000;
				tsc_smon_write(tsc_fd, 0x41, &d0);
				printf("   FPGA System Monitoring\n");
				tsc_smon_read(tsc_fd, 0x00, &d0);
				f0 = (((double)(d0 >> 6) * 503.975) / 1024.) - (double)273.15;
				tsc_smon_read(tsc_fd, 0x20, &d1);
				f1 = (((double)(d1 >> 6) * 503.975) / 1024.) - 273.15;
				tsc_smon_read(tsc_fd, 0x24, &d2);
				f2 = (((double)(d2 >> 6) * 503.975) / 1024.) - 273.15;
				printf("      Temperature          : %.2f [%.2f - %.2f]\n", f0, f1, f2);
				tsc_smon_read(tsc_fd, 0x01, &d0);
				f0 = (((double)(d0 >> 6) * 3.0) / 1024.);
				tsc_smon_read(tsc_fd, 0x21, &d1);
				f1 = (((double)(d1 >> 6) * 3.0) / 1024.);
				tsc_smon_read(tsc_fd, 0x25, &d2);
				f2 = (((double)(d2 >> 6) * 3.0) / 1024.);
				printf("      VCCint               : %.2f [%.2f - %.2f]\n", f0, f1, f2);
				tsc_smon_read(tsc_fd, 0x02, &d0);
				f0 = (((double)(d0 >> 6) * 3.0) / 1024.);
				tsc_smon_read(tsc_fd, 0x22, &d1);
				f1 = (((double)(d1 >> 6) * 3.0) / 1024.);
				tsc_smon_read(tsc_fd, 0x26, &d2);
				f2 = (((double)(d2 >> 6) * 3.0) / 1024.);
				printf("      VCCaux               : %.2f [%.2f - %.2f]\n", f0, f1, f2);
				d0 = 3;
				tsc_smon_write(tsc_fd, 0x40, &d0);
		
				printf("\n");
				printf("Calibration pattern   : 0101 1001 0011 0100 1011 0101 1001 0011 \n");
				printf("Default INC           : %d \n", CURRENT_STEP);
				printf("Default CNT           : %02x \n", DEFAULT_DELAY);
				printf("\n");
		}
        // Reset calibration register
        data = 0;
         printf("Reset calibration register ");
        tsc_csr_write_dbg(tsc_fd, SMEM_DDR3_IFSTA[mem - 1], &data);
        tsc_csr_write_dbg(tsc_fd, SMEM_DDR3_IDEL[mem - 1], &data);

         printf("Pass the entire possible delay taps ");
if (!quiet) {
        printf("+--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------+ \n");
        printf(" Default delay     :  ");
        for(j = DEFAULT_DELAY; j < MAX; j++){
            if(j == (CURRENT_DLY / CURRENT_STEP)){
                printf("*");
                break;
            }
            else{
                printf("   ");
            }
        }
        printf("\n");
        printf(" Delay value [MSB] : 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 01 01 01 01 01 01 01 01 01 01 01 01 01 01 01 01 01 01 01 01 01 01 01 01 01 01 01 01 01 01 01 01 01 01 01 01 01 01 01 01 01 01 01 01 01 01 01 01 01 01 01 01 01 01 01 01 01 01 01 01 01 01 01 01 \n");
        printf(" Delay value [LSB] : 40 44 48 4C 50 54 58 5C 60 64 68 6C 70 74 78 7C 80 84 88 8C 90 94 98 9C A0 A4 A8 AC B0 B4 B8 BC C0 C4 C8 CC D0 D4 D8 DC E0 E4 E8 EC F0 F4 F8 FC 00 04 08 0C 10 14 18 1C 20 24 28 2C 30 34 38 3F 40 44 48 4C 50 54 58 5C 60 64 68 6C 70 74 78 7C 80 84 88 8C 90 94 98 9C A0 A4 A8 AC B0 B4 B8 BC C0 C4 C8 CC D0 D4 D8 DC E0 E4 E8 EC F0 F4 F8 FC \n");
        printf("+--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------+ \n");
}
        
        data = 0;
        printf("Set IDEL to 0 \n");
        tsc_csr_write_dbg(tsc_fd, SMEM_DDR3_IDEL[mem - 1], &data);

       
        printf("Loop on 16 DQ \n");

        for(j = 0; j < 16; j++){             
            printf("\n >>>Store initial value of count of the IFSTA register [%d]<<< \n", j);           
            dq_path = (j << 12);
            tsc_csr_write_dbg(tsc_fd, SMEM_DDR3_IFSTA[mem - 1], &dq_path);
            tsc_csr_read_dbg(tsc_fd, SMEM_DDR3_IDEL[mem - 1], &vtc_read);               // Acquire current value of the register
            vtc_set = (vtc_read | (1 << 28)); 
            printf("Disable VTC - Set: 0x%08x \n", vtc_set);                            // Set value to disable VTC
            tsc_csr_write_dbg(tsc_fd, SMEM_DDR3_IDEL[mem - 1], &vtc_set);               // Disable VTC
            tsc_csr_read_dbg(tsc_fd, SMEM_DDR3_IFSTA[mem - 1], &cnt_value);             // Read initial value of IFSTA register
            tsc_csr_write_dbg(tsc_fd, SMEM_DDR3_IDEL[mem - 1], &vtc_read);              // Re-active active VTC
            temp_cnt_value_store[j] = DEFAULT_DELAY;

if (!quiet) {
            if(j < 8){
                printf(" DQ[%02d] test >>>>> :", j + 8);
            }
            else{
                printf(" DQ[%02d] test >>>>> :", j - 8);
            }
}
            // Reset avg_x, start index, number of test passed "ok" and end value for each DQ
            avg_x = 0;
            start = 0;
            end   = 0;
            ok    = 0;

            // Add steps by steps for current DQ from initial count value to max
            printf("\n Add steps by steps for current DQ from initial count value to max. \n");
            for(k = DEFAULT_DELAY; k < MAX ; k = k + CURRENT_STEP){
                printf("\nK: [%d] : [%d] \n", k);
                printf("Fill DDR3 with test pattern \n");
                memcpy(buf_ddr, buf_tx, size);
                // Get data from DDR3
                memcpy(buf_rx, buf_ddr, size);
                // Acquire test results in result array corresponding to the current DQ
                for(m = 0; m < 16; m++){
                    pattern[m * 2]       =  (buf_rx[m] & (0x1 << j)) >>  j;

                    pattern[(m * 2) + 1] =  (buf_rx[m] & (0x1 << (16 + j))) >> (16 + j);
                }

                // Check data received with reference pattern
                if (!memcmp(pattern, ref_pattern, 32 * sizeof(int))){
if (!quiet) {
                    printf("  Y");
}
                    end   = k;
                    NOK   = 0;
                    ok++;
                }
                else{
if (!quiet) {
                    printf("  -");
}
                }

                // Increment only the tap delay when we are < MAX tap
                if(k < (MAX - CURRENT_STEP)) {

if (ppc == 1) {
                    if(j < 8){
                        // Compute new count value and write IFSTA
                        data = (temp_cnt_value_store[j] + CURRENT_STEP) << 16;
                        tsc_csr_write_dbg(tsc_fd, SMEM_DDR3_IFSTA[mem - 1], &data);

                        // Load new count value
                        data = (1 << 31) | (0x1 << (j + 8));
                        tsc_csr_write_dbg(tsc_fd, SMEM_DDR3_IDEL[mem - 1], &data);

                        // Update new value of count
                        temp_cnt_value_store[j] = temp_cnt_value_store[j] + CURRENT_STEP;
                    }
                    else {
                        // Compute new count value and write IFSTA
                       
                        data = (temp_cnt_value_store[j] + CURRENT_STEP) << 16;
                        tsc_csr_write_dbg(tsc_fd, SMEM_DDR3_IFSTA[mem - 1], &data);

                        // Load new count value
                        
                        data = (1 << 31) | (0x1 << (j - 8));
                        tsc_csr_write_dbg(tsc_fd, SMEM_DDR3_IDEL[mem - 1 ], &data);


                        // Update new value of count
                        temp_cnt_value_store[j] = temp_cnt_value_store[j] + CURRENT_STEP;
                    
                    }
}
else {
                    // Compute new count value and write IFSTA
                    printf("\nCompute new count value and write IFSTA [%d] \n", j);
                    data = (temp_cnt_value_store[j] + CURRENT_STEP) << 16;
                    tsc_csr_write_dbg(tsc_fd, SMEM_DDR3_IFSTA[mem - 1], &data);

                    // Load new count value
                    data = (1 << 31) | (0x1 << (j));
                    printf("Load new count value \n");
                    tsc_csr_write_dbg(tsc_fd, SMEM_DDR3_IDEL[mem - 1], &data);

                    // Update new value of count
                    temp_cnt_value_store[j] = temp_cnt_value_store[j] + CURRENT_STEP;
                    printf("temp_cnt_value_store: [%02d] \n", temp_cnt_value_store[j]);
}
                }
            }

            // If calibration failed set the default count value
            if (ok == 0){
                marker = DEFAULT_DELAY;
            }
            // Update the new count value with the median value
            else {
                // Compute the start window
                start = (end - (ok * CURRENT_STEP)) + CURRENT_STEP;

                // Compute the average of the window
                avg_x = (ok * CURRENT_STEP) / 2;

                // Compute the new delay marker to apply
                marker = start + avg_x;
            }

            // Update the array with the new count value
            if (r == 0)
                final_cnt_value_store1[j] = marker;

            final_cnt_value_store[j] = marker;
if (!quiet) {
            if(j < 8){
                // Trace new delay
                printf("\n");
                // MEM1
                if(r == 0) {
                    printf(" Init  delay 0x%03x :\n", init_delay_1[j + 8]);
                }
                // MEM 2
                else if (r == 1){
                    printf(" Init  delay 0x%03x :\n", init_delay_2[j + 8]);
                }
                printf(" Final delay 0x%03x :", marker);

                for(n = DEFAULT_DELAY ; n < marker; n = n + CURRENT_STEP){
                    printf("   ");
                }
                printf("  *");
                printf("\n");
            }
            else{
                // Trace new delay
                printf("\n J >8 \n");
                // MEM1
                if(r == 0) {
                    printf(" Init  delay 0x%03x :\n", init_delay_1[j - 8]);
                }
                // MEM 2
                else if (r == 1){
                    printf(" Init  delay 0x%03x :\n", init_delay_2[j - 8]);
                }
                printf(" Final delay 0x%03x :", marker);
                for(n = DEFAULT_DELAY ; n < marker; n = n + CURRENT_STEP){
                    printf("   ");
                }
                printf("  *");
                printf("\n");
            }
}
printf("****||End DQ[%d]||****\n",j);
if (ppc == 1) {
            if(j < 8){
                // Compute new count value and write IFSTA
                data = final_cnt_value_store[j] << 16;                
                tsc_csr_write_dbg(tsc_fd, SMEM_DDR3_IFSTA[mem - 1], &data);

                // Load new count value
                data = (1 << 31) | (0x1 << (j + 8));
                tsc_csr_write_dbg(tsc_fd, SMEM_DDR3_IDEL[mem - 1], &data);
            }
            else {
                // Compute new count value and write IFSTA
                data = final_cnt_value_store[j] << 16;
                tsc_csr_write_dbg(tsc_fd, SMEM_DDR3_IFSTA[mem - 1], &data);

                // Load new count value
                data = (1 << 31) | (0x1 << (j - 8));
                tsc_csr_write_dbg(tsc_fd, SMEM_DDR3_IDEL[mem - 1 ], &data);
            }
}
else {
if (!quiet) {
            printf("+--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------+ \n");
}
            // Compute new count value and write IFSTA
            data = final_cnt_value_store[j] << 16;
            printf ("Final Count Value Store [%02d]: \n", final_cnt_value_store[j]);
            printf ("Writing Final value for DQ[%02d]: \n",j);
            tsc_csr_write_dbg(tsc_fd, SMEM_DDR3_IFSTA[mem - 1], &data);

            // Load new count value
            data = (1 << 31) | (0x1 << (j));
            printf ("New Count value >> [%02d] [0x%08x]: \n", j, data);
            printf ("Load new count value for DQ[%02d]: \n",j);
            tsc_csr_write_dbg(tsc_fd, SMEM_DDR3_IDEL[mem - 1], &data);
}

            // Check status
            if (ok == 0){
                NOK = 1;
                DQ_NOK[j] = 1;
            }
            else {
                DQ_NOK[j] = 0;
            }

            DQ_OK[j] = ok;
if (!quiet) {
            printf("+--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------+ \n");
}
            // Set IDEL and IFSTA to 0
            data = 0;
            tsc_csr_write_dbg(tsc_fd, SMEM_DDR3_IDEL[mem - 1], &data);
            tsc_csr_write_dbg(tsc_fd, SMEM_DDR3_IFSTA[mem - 1], &data);

        }

        // Execution is finished OK or NOK
if (!quiet) {
        printf("\n");
}
        if (NOK == 1){
            printf("Calibration is not possible, error on line(s) : \n");
            for (m = 0; m < 16; m++){
                if (DQ_NOK[m] == 1){
                    printf("DQ[%i] \n", m);
                }
            }
        }
        else {
            // Search best case
            best = DQ_OK[0];

            for (m = 1 ;m < 16 ;m++) {
                if ( DQ_OK[m] > best ) {
                    //best = DQ_OK[m];
                    if (m < 8){
                        best = DQ_OK[m + 8];
                        location = m + 8;
                    }
                    else {
                        best = DQ_OK[m - 8];
                        location = m - 8;
                    }
                }
            }
if (!quiet) {
            printf("Best calibration window size is %i for DQ[%02i] \n", best, location);
}
            // Search worst case
            worst = DQ_OK[0];

            for (m = 1 ;m < 16 ;m++) {
                if ( DQ_OK[m] < worst ) {
                    worst = DQ_OK[m];
                    if (m < 8){
                        best = DQ_OK[m + 8];
                        location = m + 8;
                    }
                    else {
                        best = DQ_OK[m - 8];
                        location = m - 8;
                    }
                }
            }
if (!quiet) {
            printf("Worst calibration windows size is %i for DQ[%02i] \n", worst, location);
}
        }

        // Print initial and final
if (!quiet) {
        printf("\n");
        if (r == 0){ // MEM1
            for (m = 0 ;m < 16 ;m++) {
                if(m < 8){
                    printf("DQ[%02d] - Initial delay 0x%03x - Final delay 0x%03x \n", m, init_delay_1[m], final_cnt_value_store[m + 8]);
                }
                else{
                    printf("DQ[%02d] - Initial delay 0x%03x - Final delay 0x%03x \n", m, init_delay_1[m], final_cnt_value_store[m - 8]);
                }
            }
        }
        else if (r == 1){ // MEM2
            for (m = 0 ;m < 16 ;m++) {
                if(m < 8){
                    printf("DQ[%02d] - Initial delay 0x%03x - Final delay 0x%03x \n", m, init_delay_2[m], final_cnt_value_store[m + 8]);
                }
                else{
                    printf("DQ[%02d] - Initial delay 0x%03x - Final delay 0x%03x \n", m, init_delay_2[m], final_cnt_value_store[m - 8]);
                }
            }
        }

        printf("\n");
        printf("Calibration finished ! \n");
        printf("\n");
}
        // Set IDEL and IFSTA to 0
        data = 0;
        tsc_csr_write_dbg(tsc_fd, SMEM_DDR3_IDEL[mem - 1], &data);
        tsc_csr_write_dbg(tsc_fd, SMEM_DDR3_IFSTA[mem - 1], &data);

        // Unmap DDR3 memory
        munmap(buf_ddr, map_win.req.size);
        tsc_map_free(tsc_fd, &map_win);

        // Free buffer
        free(buf_tx);
        free(buf_rx);

        mem++;
    }

    /* FINAL READ */
    /***************************************************************************/
    printf("FINAL READ \n");
    // Check if calibration is needed for mem1, mem2 or mem1 & mem2
    mem = memOrg;

    // Loop on SMEM
    for (r = 0; r < rr; r++){
        // Reset calibration register
        data = 0;
        tsc_csr_write_dbg(tsc_fd, SMEM_DDR3_IFSTA[mem - 1], &data);
        tsc_csr_write_dbg(tsc_fd, SMEM_DDR3_IDEL[mem - 1], &data);
//if (!quiet) {
        printf("Final value for MEM%x : \n", mem);
//}      
        // Loop on 16 DQ
        for(j = 0; j < 16; j++){
            // Read final value of the IFSTA register
            printf("\n >>>LOOP[%d]<<< \n", j);
            dq_path = (j << 12);
            tsc_csr_write_dbg(tsc_fd, SMEM_DDR3_IFSTA[mem - 1], &dq_path);
            tsc_csr_read_dbg(tsc_fd, SMEM_DDR3_IDEL[mem - 1], &vtc_read);           // Acquire current value of the register
            vtc_set = (vtc_read | (1 << 28));                                   // Set value to disable VTC
            tsc_csr_write_dbg(tsc_fd, SMEM_DDR3_IDEL[mem - 1], &vtc_set);           // Disable VTC
            tsc_csr_read_dbg(tsc_fd, SMEM_DDR3_IFSTA[mem - 1], &cnt_value);         // Read final value of IFSTA register
            tsc_csr_write_dbg(tsc_fd, SMEM_DDR3_IDEL[mem - 1], &vtc_read);          // Re-active active VTC
//if (!quiet) {
            /* IDELAY value is stored in the 9 LSBs of SMEM_DDR_IFSTA register (SMEM_DDR3_DELQ register) */
            // MEM1
            if(r == 0) {
                if (j < 8) {
                    expected_val = final_cnt_value_store1[j + 8] & 0x1ff;
                } else {
                    expected_val = final_cnt_value_store1[j - 8] & 0x1ff;
                }
                printf("DQ[%02i] Initial delay 0x%03x - IFSTA register 0x%08x -> Final delay 0x%03x  -> Expected delay 0x%03x\n", j, init_delay_1[j], cnt_value, cnt_value & 0x1ff, expected_val);
            }
            // MEM2
            else if (r == 1){
                if (j < 8) {
                    expected_val = final_cnt_value_store[j + 8] & 0x1ff;
                } else {
                    expected_val = final_cnt_value_store[j - 8] & 0x1ff;
                }
                printf("DQ[%02i] Initial delay 0x%03x - IFSTA register 0x%08x -> Final delay 0x%03x -> Expected delay 0x%03x\n", j, init_delay_2[j], cnt_value, cnt_value & 0x1ff, expected_val);
            }
//}
        }

        // Set IDEL and IFSTA to 0
        data = 0;
        printf("\n Set IDEL and IFSTA to 0 \n");
        tsc_csr_write_dbg(tsc_fd, SMEM_DDR3_IDEL[mem - 1], &data);
        tsc_csr_write_dbg(tsc_fd, SMEM_DDR3_IFSTA[mem - 1], &data);

        mem++;

    }
    tsc_ddr_set_calibration_done(tsc_fd, 1);

//if (!quiet) {
    printf("\n");
//}
    // TSC exit
    tsc_exit(tsc_fd);

    return 0;
}
