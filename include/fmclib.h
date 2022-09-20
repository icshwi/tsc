/*=========================< begin file & file header >=======================
 *  References
 *
 *    filename : fmclib.h
 *    author   : CG
 *    company  : IOxOS
 *    creation : october 20,2020
 *
 *----------------------------------------------------------------------------
 *  Description
 *
 *    That library contains a set of function to control the FMC slots
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

#ifndef _H_FMCLIB
#define _H_FMCLIB

/* ------------------------------------------------------------------------------------------------------------------ */
/*  Type Definition                                                                                                   */
/* ------------------------------------------------------------------------------------------------------------------ */

typedef struct
{
  char board_name[8];      /*   8 board name     i.e. ADC_3110       */
  char serial[4];          /*   4 serial number, 4 last digits only  */
  char version[8];         /*   8 PCB version 1600.0                 */
  char revision[2];        /*   2 PCB revision A0 for .0             */
  char rsv[6];             /*   6 reserved                           */
  char test_date[8];       /*   8 date of test DDMMYYYY              */
  char pad[219];           /* 219 padding used for information       */
                           /*     depending of board_name            */
  char cks;                /*   1 checksum                           */
} __attribute__((packed)) fmc_sign_t;

/* ------------------------------------------------------------------------------------------------------------------ */
/*  Register Definition                                                                                               */
/* ------------------------------------------------------------------------------------------------------------------ */

#define FMC1_CSR_BASE           0x1200
#define FMC2_CSR_BASE           0x1300

#define FMC_CSR_SIGN            0x00        /* FMC signature                                                          */

/* Build an address from a register index */
#define FMC_CSR_ADDR(fmc, csr)  (((fmc==2) ? FMC2_CSR_BASE : FMC1_CSR_BASE) | ((csr&0x3f)<<2))

#define FMC_SIGN_MASK           0xffff0000

/* I2C EEPROM offsets */
#define FMC_FRU_SIZE            0x0800  /* FMC IPMI FRU size                */
#define FMC_FRU_OFF             0x0000  /* FMC IPMI FRU   EEPROM offset     */
#define FMC_PARAM_OFF           0x6000  /* FMC PARAMETER  EEPROM offset     */
#define FMC_SIGN_OFF            0x7000  /* signature      EEPROM offset     */

/* ------------------------------------------------------------------------------------------------------------------ */
/*  Function Prototypes                                                                                               */
/* ------------------------------------------------------------------------------------------------------------------ */

extern int            fmc_init              (int fd);
extern int            fmc_csr_read          (int fd, int fmc, int csr, int *data);
extern int            fmc_csr_write         (int fd, int fmc, int csr, int data);
extern int            fmc_identify          (int fd, int fmc, int * id, int * ver, int *rev);

extern int            fmc_i2c_read          (int fd, int fmc, uint dev, uint reg, uint *data);
extern int            fmc_i2c_write         (int fd, int fmc, uint dev, uint reg, uint data);

extern int            fmc_i2c_eeprom_read   (int fd, int fmc, uint dev, int eeprom_addr, unsigned char *buf, int len);
extern int            fmc_i2c_eeprom_write  (int fd, int fmc, uint dev, int eeprom_addr, unsigned char *buf, int len);
extern int            fmc_i2c_eeprom_dump   (int fd, int fmc, uint dev, int eeprom_addr, int len);

extern int            fmc_fru_calc_size     ();
extern int            fmc_fru_load          (int fd, int fmc, uint dev);
extern int            fmc_fru_store         (int fd, int fmc, uint dev);

extern int            fmc_fru_read          (const char *filename, int quiet);
extern int            fmc_fru_write         (const char *filename, int quiet);

extern unsigned char  fmc_calc_chksum       (unsigned char *buf, int len);
extern int            fmc_sign_write        (int fd, int fmc, uint dev, fmc_sign_t *sign);
extern int            fmc_sign_read         (int fd, int fmc, uint dev, fmc_sign_t *sign);
extern int            fmc_sign_print        (FILE *fout, fmc_sign_t *sign);
extern int            fmc_sign_set          (fmc_sign_t *sign, 
                                             char *board_name, 
                                             char *serial, 
                                             char *version, 
                                             char *revision, 
                                             int set_test_date, 
                                             int quiet);

#endif /* _H_FMCLIB */

