/*=========================< begin file & file header >=======================
 *  References
 *  
 *    filename : tsculib.h
 *    author   : JFG, XP
 *    company  : IOxOS
 *    creation : june 30,2008
 *
 *----------------------------------------------------------------------------
 *  Description
 *
 *    This file contain the declarations of all exported functions define in
 *    tsculib.c
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

#ifndef _H_TSCULIB
#define _H_TSCULIB

#include <tscioctl.h>

int CheckByteOrder(void);
int tsc_read_loop(int fd, uint64_t rem_addr, char *buf, int len, uint mode);
int tsc_write_loop(int fd, uint64_t rem_addr, char *buf, int len, uint mode);
int tsc_init(uint32_t card);
int tsc_exit(int fd);
char *tsc_get_lib_version( void);
char *tsc_get_drv_version( void);
char *tsc_get_drv_name( void);
short tsc_get_vendor_id( void);
short tsc_get_device_id( void);
int tsc_csr_write(int fd, int idx, int *data_p);   /* write to TSC CSR register      */
int tsc_csr_read(int fd, int idx, int *data_p);    /* read from TSC CSR register     */
int tsc_smon_write(int fd, int idx, int *data_p);  /* write to ARTIX7 SMON register      */
int tsc_smon_read(int fd, int idx, int *data_p);   /* read from ARTIX7 SMON register     */
int tsc_pon_write(int fd, int idx, int *data_p);   /* write to PMON register             */
int tsc_pon_read(int fd, int idx, int *data_p);    /* read from PON register             */
int tsc_axi_get_cap(int *cap);
int tsc_axil_write( int idx, int wstrb, int prot, int * data_p);
int tsc_axil_read( int idx, int prot, int * data_p);
int tsc_pciep_write(int fd, int idx, int *data_p); /* write to ARTIX7 PCIEP register     */
int tsc_pciep_read(int fd, int idx, int *data_p);  /* read from ARTIX7 PCIEP register    */
uint64_t tsc_swap_64(uint64_t);
int tsc_swap_32( int);
short tsc_swap_16( short);
int tsc_write_blk(int fd, uint64_t addr, char *buf, int len, uint mode);
int tsc_read_blk(int fd, uint64_t addr, char *buf, int len, uint mode);
int tsc_write_sgl(int fd, uint64_t addr, char *buf, uint mode);
int tsc_read_sgl(int fd, uint64_t addr, char *buf, uint mode);
int tsc_shm_write(int fd, uint shm_addr, char *buf, int len, int ds, int swap, int mem);
int tsc_shm_read(int fd, uint shm_addr, char *buf, int len, int ds, int swap, int mem);
int tsc_usr_write(int fd, uint shm_addr, char *buf, int len, int ds, int swap, int mem);
int tsc_usr_read(int fd, uint shm_addr, char *buf, int len, int ds, int swap, int mem);
int tsc_dma_move(int fd, struct tsc_ioctl_dma_req *dr_p);
int tsc_map_alloc(int fd, struct tsc_ioctl_map_win *w);
int tsc_map_get(int fd, struct tsc_ioctl_map_win *w);
int tsc_map_free(int fd, struct tsc_ioctl_map_win *w);
int tsc_map_modify(int fd, struct tsc_ioctl_map_win *w);
int tsc_map_read(int fd, struct tsc_ioctl_map_ctl *m);
int tsc_map_clear(int fd, struct tsc_ioctl_map_ctl *m);
void *tsc_pci_mmap(int fd, off_t pci_addr, size_t size);
int tsc_pci_munmap(void *pci_addr, size_t size);
int tsc_kbuf_alloc(int fd, struct tsc_ioctl_kbuf_req *kr_p);
int tsc_kbuf_free(int fd, struct tsc_ioctl_kbuf_req *kr_p);
void *tsc_kbuf_mmap(int fd, struct tsc_ioctl_kbuf_req *kr_p);
int tsc_kbuf_munmap(struct tsc_ioctl_kbuf_req *kr_p);
int tsc_kbuf_read(int fd, void *k_addr, char *buf, uint size);
int tsc_kbuf_write(int fd, void *k_addr, char *buf, uint size);
int tsc_dma_move(int fd, struct tsc_ioctl_dma_req *dr_p);
int tsc_dma_wait(int fd, struct tsc_ioctl_dma_req *dr_p);
int tsc_dma_status(int fd, struct tsc_ioctl_dma_sts *ds_p);
int tsc_dma_mode(int fd, struct tsc_ioctl_dma_mode *dm_p);
int tsc_dma_alloc(int fd, int chan);
int tsc_dma_free(int fd, int chan);
int tsc_dma_clear(int fd, int chan);
int tsc_dma_transfer(int fd, struct tsc_ioctl_dma_req *dr_p);
int tsc_sflash_rdid(int fd, char *id);
int tsc_sflash_rdsr(int fd, char *sr);
int tsc_sflash_wrsr(int fd, char *sr);
int tsc_sflash_read(int fd, int offset, char *buf, int len);
int tsc_sflash_write(int fd, int offset, char *buf, int len);
int tsc_timer_start(int fd, int mode, int msec);
int tsc_timer_restart(int fd);
int tsc_timer_stop(int fd);
int tsc_timer_read(int fd, struct tsc_time *tm);
int tsc_fifo_init(int fd, int idx, int mode);
int tsc_fifo_status(int fd, uint idx, uint *sts);
int tsc_fifo_clear(int fd, uint idx, uint *sts);
int tsc_fifo_wait_ef(int fd, uint idx,  uint *sts, uint tmo);
int tsc_fifo_wait_ff(int fd, uint idx,  uint *sts, uint tmo);
int tsc_fifo_read(int fd, uint idx,  uint *data, uint wcnt, uint *sts);
int tsc_fifo_write(int fd, uint idx,  uint *data, uint wcnt, uint *sts);
int tsc_i2c_read(int fd, uint dev,  uint reg, uint *data);
int tsc_i2c_write(int fd, uint dev,  uint reg, uint data);
int tsc_i2c_cmd(int fd, uint dev,  uint cmd);
int tsc_i2c_reset(uint dev);
int tsc_semaphore_status(int fd, uint *sts);
int tsc_semaphore_release(int fd, uint idx, uint tag);
int tsc_semaphore_get(int fd, uint idx, uint tag);
int tsc_user_irq_wait(int fd, struct tsc_ioctl_user_irq *user_irq_p);
int tsc_user_irq_subscribe(int fd, struct tsc_ioctl_user_irq *user_irq_p);
int tsc_user_irq_unsubscribe(int fd, struct tsc_ioctl_user_irq *user_irq_p);
#endif /*  _H_TSCULIB */
