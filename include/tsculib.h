/*=========================< begin file & file header >=======================
 *  References
 *  
 *    filename : tsculib.h
 *    author   : JFG
 *    company  : IOxOS
 *    creation : june 30,2008
 *    version  : 0.0.1
 *
 *----------------------------------------------------------------------------
 *  Description
 *
 *    This file contain the declarations of all exported functions define in
 *    tsculib.c
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

#ifndef _H_TSCULIB
#define _H_TSCULIB

#include <tscioctl.h>

int tsc_init( void);
int tsc_exit( void);
char *tsc_get_lib_version( void);
char *tsc_get_drv_version( void);
char *tsc_get_drv_name( void);
short tsc_get_vendor_id( void);
short tsc_get_device_id( void);
int tsc_csr_write( int idx, int *data_p);   /* write to IFC1211 CSR register      */
int tsc_csr_read( int idx, int *data_p);    /* read from IFC1211 CSR register     */
int tsc_smon_write( int idx, int *data_p);  /* write to ARTIX7 SMON register      */
int tsc_smon_read( int idx, int *data_p);   /* read from ARTIX7 SMON register     */
int tsc_pon_write( int idx, int *data_p);   /* write to PMON register             */
int tsc_pon_read( int idx, int *data_p);    /* read from PON register             */
int tsc_pciep_write( int idx, int *data_p); /* write to ARTIX7 PCIEP register     */
int tsc_pciep_read( int idx, int *data_p);  /* read from ARTIX7 PCIEP register    */
long long tsc_swap_64( long long);
int tsc_swap_32( int);
short tsc_swap_16( short);
int tsc_write_blk( ulong addr, char *buf, int len, uint mode);
int tsc_read_blk( ulong addr, char *buf, int len, uint mode);
int tsc_write_sgl( ulong addr, char *buf, uint mode);
int tsc_read_sgl( ulong addr, char *buf, uint mode);
int tsc_shm_write( uint shm_addr, char *buf, int len, int ds, int swap);
int tsc_shm_read( uint shm_addr, char *buf, int len, int ds, int swap);
int tsc_dma_move(  struct tsc_ioctl_dma_req *dr_p);
int tsc_map_alloc( struct tsc_ioctl_map_win *w);
int tsc_map_get( struct tsc_ioctl_map_win *w);
int tsc_map_free( struct tsc_ioctl_map_win *w);
int tsc_map_modify( struct tsc_ioctl_map_win *w);
int tsc_map_read( struct tsc_ioctl_map_ctl *m);
int tsc_map_clear( struct tsc_ioctl_map_ctl *m);
void *tsc_pci_mmap( off_t pci_addr, size_t size);
int tsc_pci_munmap( void *pci_addr, size_t size);
int tsc_kbuf_alloc( struct tsc_ioctl_kbuf_req *kr_p);
int tsc_kbuf_free( struct tsc_ioctl_kbuf_req *kr_p);
void *tsc_kbuf_mmap( struct tsc_ioctl_kbuf_req *kr_p);
int tsc_kbuf_munmap( struct tsc_ioctl_kbuf_req *kr_p);
int tsc_kbuf_read( void *k_addr, char *buf, uint size);
int tsc_kbuf_write( void *k_addr, char *buf, uint size);
int tsc_dma_move( struct tsc_ioctl_dma_req *dr_p);
int tsc_dma_wait( struct tsc_ioctl_dma_req *dr_p);
int tsc_dma_status( struct tsc_ioctl_dma_sts *ds_p);
int tsc_dma_mode( struct tsc_ioctl_dma_mode *dm_p);
int tsc_dma_alloc( int chan);
int tsc_dma_free( int chan);
int tsc_dma_clear( int chan);
int tsc_sflash_rdid( char *id);
int tsc_sflash_rdsr( char *sr);
int tsc_sflash_wrsr( char *sr);
int tsc_sflash_read( int offset, char *buf, int len);
int tsc_sflash_write( int offset, char *buf, int len);
int tsc_timer_start( int mode, int msec);
int tsc_timer_restart( void);
int tsc_timer_stop( void);
int tsc_timer_read( struct tsc_time *tm);
int tsc_fifo_init(int idx, int mode);
int tsc_fifo_status( uint idx, uint *sts);
int tsc_fifo_clear( uint idx, uint *sts);
int tsc_fifo_wait_ef( uint idx,  uint *sts, uint tmo);
int tsc_fifo_wait_ff( uint idx,  uint *sts, uint tmo);
int tsc_fifo_read( uint idx,  uint *data, uint wcnt, uint *sts);
int tsc_fifo_write( uint idx,  uint *data, uint wcnt, uint *sts);
int tsc_i2c_read( uint dev,  uint reg, uint *data);
int tsc_i2c_write( uint dev,  uint reg, uint data);
int tsc_i2c_cmd( uint dev,  uint cmd);
int tsc_i2c_reset( uint dev);
int tsc_semaphore_status(uint *sts);
int tsc_semaphore_release(uint idx);
int tsc_semaphore_get(uint idx, uint *tag);

#endif /*  _H_TSCULIB */
