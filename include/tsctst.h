/*=========================< begin file & file header >=======================
 *  References
 *
 *    filename : tsctst.h
 *    author   : XP
 *    company  : IOxOS
 *    creation : june 30,2016
 *    version  : 0.0.1
 *
 *----------------------------------------------------------------------------
 *  Description
 *
 *    This file contains the definition of all structures and define used
 *    in the TSC environment test.
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

struct tsc_para{
	int crate;
	int serial;
};

// Structure with all configuration parameters
struct tscTst{
	/* Local board 0 */
	int shm_base_0;
	int shm_size_0;
	int shm_offset_0;
	int sram_offset_0;
	int usr_base_0;
	int usr_size_0;
	int usr_offset_0;

};

struct tst_ctl{
	int    status;
	int    test_idx;
	int    loop_mode;
	int    loop_cnt;
	int    log_mode;
	int    err_mode;
	char   *log_filename;
	FILE   *log_file;
	struct tscTst *at;
	int    para_cnt;
	char   **para_p;
	int    exec_mode;
	int    fd;
};

#define TST_DMA_WAIT_NO     0x00000000
#define TST_DMA_WAIT_POLL   0x01000000
#define TST_DMA_WAIT_INTR   0x02000000
#define TST_DMA_WAIT_MASK   0x0f000000
#define TST_DMA_CHAN_0      0x00000000
#define TST_DMA_CHAN_1      0x10000000
#define TST_DMA_CHAN_MASK   0x10000000

#define TST_LOG_OFF    		0x00000000
#define TST_LOG_NEW    		0x01000000
#define TST_LOG_ADD    		0x02000000

#define TST_STS_IDLE      	0x00000000
#define TST_STS_STARTED   	0x00000001
#define TST_STS_DONE      	0x00000002
#define TST_STS_STOPPED   	0x00000004
#define TST_STS_ERR       	0x00000008

#define TST_ERR_CONT        0x00000000
#define TST_ERR_HALT        0x00000001

#define TST_EXEC_FAST       0x00000001
#define TST_EXEC_VAL        0x00000000

char logline[0x200];
#define TST_LOG( x, y) \
sprintf y;\
printf( "%s", logline);fflush( stdout);\
if( x->log_mode){fprintf( x->log_file, "%s", logline);fflush( x->log_file);}
