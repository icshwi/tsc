/*=========================< begin file & file header >=======================
 *  References
 *  
 *    filename : fmclib.c
 *    author   : CG
 *    company  : IOxOS
 *    creation : october 20,2020
 *
 *----------------------------------------------------------------------------
 *  Description
 *
 *    That library contains a set of function to contorl FMC slots
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

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <string.h>
#include <tsculib.h>
#include <tscextlib.h>
#include <fmclib.h>

/*--------------------------------------------------------------------------------------------------------------------*/

static uint           fmc_ifc_pid        = 0;     /* Local IFC product ID                           */
static int            fmc_ifc_loc        = 0;     /* Local IFC when 0 or Remote when <> 0           */
static unsigned char  fmc_fru_buf[FMC_FRU_SIZE];  /* Buffer used to read & write FMC FRU to EEPROM  */

/*--------------------------------------------------------------------------------------------------------------------*/

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * Function name : fmc_init
 * Prototype     : -
 * Parameters    : -
 * Return        : int
 *----------------------------------------------------------------------------
 * Description   : Initialize FMC slots
 *                 returns: 1 = initialization done successfully
 *                          0 = can not do initialization (remote device)
 *                          0 < an error has occurred
 *++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

int fmc_init(int fd)
{
  int tmp, mask, ret=0;
  
  fmc_ifc_loc = 0;
  
  /* local device ? */
  if (tsc_get_device_id() == PCI_DEVICE_ID_IOXOS_TSC_CENTRAL_1)
  {
    ret = tsc_pon_read(fd, 0x0, &tmp);
    if (ret  < 0)
      return(ret);

    fmc_ifc_pid = (tmp & 0xffff);
    fmc_ifc_loc = 0;

    /* turn FMC slot ON if necessary */
    mask = ((1<<31)|(1<<30));
    ret = tsc_pon_read(fd, 0xC, &tmp);

    if (ret < 0)
      return(ret);

    if ((tmp & mask) != mask)
    {
      tmp |= mask;
      ret = tsc_pon_write(fd, 0xC, &tmp);
      if (ret < 0)
        return(ret);
      usleep(20000);
    }
    ret = 1;
  }
  
  return(ret);
}

/*--------------------------------------------------------------------------------------------------------------------*/

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * Function name : fmc_csr_rd
 * Prototype     : int
 * Parameters    : fmc      fmc index (1 or 2)
 *                 csr      register index (0 to 63)
 *                 data     pointer to a variable for data
 * Return        : int
 *----------------------------------------------------------------------------
 * Description   : Read a data from a FMC register
 *++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

int fmc_csr_read(int fd, int fmc, int csr, int *data)
{
  int addr, tmp, ret;
 
  addr = FMC_CSR_ADDR(fmc, csr);

  ret = tsc_csr_read(fd, addr, &tmp);

  if (data != NULL)
  {
    (*data) = tmp;
  }

  return (ret);
}

/*--------------------------------------------------------------------------------------------------------------------*/

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * Function name : fmc_csr_wr
 * Prototype     : void
 * Parameters    : fmc  fmc index (1 or 2)
 *                 csr  register index
 *                 data  data to be written in register
 * Return        : void
 *----------------------------------------------------------------------------
 * Description   : Write FMC CSR register referred by csr with data
 *
 *++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

int fmc_csr_write(int fd, int fmc, int csr, int data)
{
  int addr, ret;

  addr = FMC_CSR_ADDR(fmc, csr);

  ret = tsc_csr_write(fd, addr, &data);

  return ret;
}

/*--------------------------------------------------------------------------------------------------------------------*/

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * Function name : fmc_identify
 * Prototype     : int
 * Parameters    : fmc FMC identifier (1 or 2)
 * Return        : FMC signature (expect FMC_SIGN_ID)
 *----------------------------------------------------------------------------
 * Description   : returns the content of FMC signature register (index 0x0)
 *
 *++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

int fmc_identify(int fd, int fmc, int * id, int * ver, int *rev)
{
  int tmp, ret = 0;

  ret = fmc_csr_read(fd, fmc, FMC_CSR_SIGN, &tmp);
  if (ret < 0)
    return(ret);

  if (id != NULL)
  {
    /* turn on FMC if expected one */ 
    if ((tmp & 0xFFFF0000) == (*id))
    {
      ret = fmc_csr_write(fd, fmc, FMC_CSR_SIGN, tmp);
      
      if (ret < 0)
        return(ret);
      
      usleep(20000);
    }

    (*id) = tmp;
  }
  else
  {
    ret = tmp;
  }
  
  if (ver != NULL)
  {
    (*ver) = ((tmp >> 8) & 0xff);
  }

  if (rev != NULL)
  {
    (*rev) = (tmp & 0xff);
  }

  return(ret);
}

/*--------------------------------------------------------------------------------------------------------------------*/

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * Function name : fmc_i2c_get_device_address
 * Prototype     : int
 * Parameters    : fmc, device, ifc_pid
 * Return        : int
 * Scope         : local
 *----------------------------------------------------------------------------
 * Description   : Calculate I2C device address depending in IFC & FMC
 *
 *++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

static int fmc_i2c_calc_add(int fd, int fmc, uint *dev)
{
  int fmc_ga = -1, ret;
  uint i2c_dev = 0;

  ret = 0;

  switch(fmc_ifc_pid & 0xff00)
  {
    /* FMC GA[0:1] on IFC_1410 & IFC_1411 */
    case 0x1400:
      fmc_ga = ((fmc == 1) ? 0x0 : 0x2);
      break;

    /* FMC GA[0:1] on IFC-1211 */
    case 0x1200:
      fmc_ga = ((fmc == 1) ? 0x2 : 0x1);
      break;

    default:
      return(-1);
  }

  if (dev != NULL)
  {
    switch((*dev) & 0xff)
    {
      /* TMP102 with ADD connected to GA[1] */
      case 0x48:
        i2c_dev = ((*dev) | ((fmc_ga>>1)&1));
        break;

      /* default following standard, EEPROM */
      default:
        i2c_dev = ((*dev) | (fmc_ga&3));
        break;
    }
    (*dev) = i2c_dev;
    ret = 1;
  }

#ifdef DEBUG
/*  printf("i2c_dev = 0x%02X\n", i2c_dev);*/
#endif /* DEBUG */
  return(ret);
}

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * Function name : fmc_i2c_read
 * Prototype     : int
 * Parameters    : fmc  FMC identifier (1 or 2)
 *                 dev  I2C device address
 *                 reg  register index
 * Return        : content of register
 *----------------------------------------------------------------------------
 * Description   : returns the content of register reg located in the resource
 * identified by dev.
 *
 *++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

int fmc_i2c_read(int fd, int fmc, uint dev, uint reg, uint * data)
{
  int ret;
  uint tmp;

  /* FMC I2C is not available on remote CPU */
  if (fmc_ifc_loc != 0)
    return(-1);

  ret = fmc_i2c_calc_add(fd, fmc, &dev);
  if (ret < 0)
    return (ret);

  ret = tsc_i2c_read(fd, dev, reg, &tmp);
  
  if((ret & TSC_I2C_CTL_EXEC_MASK) == TSC_I2C_CTL_EXEC_ERR)
    return (-1);

  if (data != NULL)
  {
    (*data) = tmp;
  }
  return(1);
}  

/*--------------------------------------------------------------------------------------------------------------------*/

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * Function name : fmc_i2c_write
 * Prototype     : int
 * Parameters    : fmc  fmc number (1 or 2)
 *                 dev  I2C device address
 *                 reg  register index
 *                 data  data to be written in register
 * Return        :  1   on success
 *                  0<  on error
 *----------------------------------------------------------------------------
 * Description   : write data in the register reg located in the resource 
 *                 identified by dev
 *++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

int fmc_i2c_write(int fd, int fmc, uint dev, uint reg, uint data)
{
  int ret;

  /* FMC I2C is not available on remote CPU */
  if (fmc_ifc_loc != 0)
    return(-1);

  ret = fmc_i2c_calc_add(fd, fmc, &dev);
  if (ret < 0)
    return(ret);

  ret = tsc_i2c_write(fd, dev, reg, data);

  if ((ret & TSC_I2C_CTL_EXEC_MASK) == TSC_I2C_CTL_EXEC_ERR)
    return (-1);

  return(1);
} 

/*--------------------------------------------------------------------------------------------------------------------*/

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * Function name : fmc_i2c_eeprom_rd
 * Prototype     : void
 * Parameters    : device, eeprom_addr, buf, len
 * Return        : void
 * Scope         : local
 *----------------------------------------------------------------------------
 * Description   : Read from EEPROM
 *
 *++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

int fmc_i2c_eeprom_read(int fd, int fmc, uint dev, int eeprom_addr, unsigned char *buf, int len)
{
  int i, ret;
  uint data;

  for (i=0; i<len; i++) 
  {
    data = 0;
    
    ret = fmc_i2c_read(fd, fmc, dev, (eeprom_addr+i), &data);
    if (ret < 0)
      return(ret);
    
    buf[i] = (unsigned char)data;
  }
  return(0);
}

/*--------------------------------------------------------------------------------------------------------------------*/

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * Function name : fmc_i2c_eeprom_wr
 * Prototype     : void
 * Parameters    : device, eeprom_addr, buf, len
 * Return        : void
 * Scope         : local
 *----------------------------------------------------------------------------
 * Description   : Write into EEPROM
 *
 *++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

int fmc_i2c_eeprom_write(int fd, int fmc, uint dev, int eeprom_addr, unsigned char *buf, int len)
{
  int i, ret;
  uint data;

  for(i=0; i<len; i++)
  {
    data = (uint)buf[i];
    ret = fmc_i2c_write(fd, fmc, dev, (eeprom_addr+i), data);
    if (ret < 0)
      return(ret);
      
    usleep(5000);
  }
  return(0);
}

/*--------------------------------------------------------------------------------------------------------------------*/

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * Function name : fmc_i2c_eeprom_dump
 * Prototype     : void
 * Parameters    : device, eeprom_addr, len
 * Return        : void
 * Scope         : local
 *----------------------------------------------------------------------------
 * Description   : Write into EEPROM
 *
 *++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

int fmc_i2c_eeprom_dump(int fd, int fmc, uint dev, int eeprom_addr, int len)
{
  int i, j, ret;
  unsigned char buf[16];

  printf("Displaying EEPROM from %x to %x\n", eeprom_addr, (eeprom_addr+len));

  for (i=0; i<len; i+=16)
  {
    ret = fmc_i2c_eeprom_read(fd, fmc, dev, (eeprom_addr+i), buf, 16);
    if (ret < 0)
    {
      fprintf(stderr, "error reading eeprom (%d)\n", ret);
      return(ret);
    }
    printf("%04x: ", (eeprom_addr+i));
    for (j=0; j<16; j++)
    {
      printf("%02x ", buf[j]);
    }
    printf("\n");
  }

  return(0);
}

/*--------------------------------------------------------------------------------------------------------------------*/

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * Function name : fmc_fru_calc_size()
 * Prototype     : int
 * Parameters    : -
 * Return        : success/error
 * Scope         : global
 *----------------------------------------------------------------------------
 * Description   : Calculate the size of FRU loaded into buffer
 *
 *++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

int fmc_fru_calc_size()
{
  int i, n;
  unsigned char *p;

  /* try to determine the size of FRU if possible */
  p = fmc_fru_buf;
  n = 0;
  /* no internal_use record */
  if (p[1] == 0) {
    n = 8;                /* common header length */
    /* has a chassis_info record */
    if (p[2] != 0) {
      i = (8*p[2]);       /* chassis info area offset */
      n += (8 * p[i+1]);  /* chassis info area length in multiples of 8 bytes */
    }
    /* has a board_area */
    if (p[3] != 0) {
      i = (8*p[3]);       /* board info area offset */
      n += (8 * p[i+1]);  /* board info area length in multiples of 8 bytes */
    }
    /* has a product info area */
    if (p[4] != 0) {
      i = (8*p[4]);       /* product info area offset */
      n += (8 * p[i+1]);  /* product info area length in multiples of 8 bytes */
    }
    /* has multirecord */
    if (p[5] != 0) {
      i = (8*p[5]);       /* multirecord offset */

      for (;;) {
        n += (5+p[i+2]);      /* multirecord + record length */

        /* End of list ? */
        if (p[i+1] & 0x80)
          break;

        i += (5+p[i+2]);      /* go to next multirecord */
      }
    }
  }
  else 
  {
    n = sizeof(fmc_fru_buf);
  }
  return n;
}

/*--------------------------------------------------------------------------------------------------------------------*/

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * Function name : fmc_fru_load
 * Prototype     : int
 * Parameters    : fmc, device, filename, quiet
 * Return        : return the size of FRU or <0 on error
 * Scope         : global
 *----------------------------------------------------------------------------
 * Description   : read FRU from EEPROM and load it to fmc_fru_buf
 *
 *++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

int fmc_fru_load(int fd, int fmc, uint dev)
{
  int rc, n;

  if (fmc < 1 || fmc > 2)
    return(-1);

  rc = fmc_i2c_eeprom_read(fd, fmc, dev, FMC_FRU_OFF, fmc_fru_buf, sizeof(fmc_fru_buf));
  if (rc < 0) 
    return(rc);
  
  n = fmc_fru_calc_size();
  
  return(n);
}

/*--------------------------------------------------------------------------------------------------------------------*/

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * Function name : fmc_fru_store
 * Prototype     : int
 * Parameters    : fmc, device, filename, quiet
 * Return        : return the size of FRU or <0 on error
 * Scope         : global
 *----------------------------------------------------------------------------
 * Description   : write fmc_fru_buf into EEPROM
 *
 *++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

int fmc_fru_store(int fd, int fmc, uint dev)
{
  int n, rc;

  if (fmc < 1 || fmc > 2)
    return(-1);
  
  n = fmc_fru_calc_size();

  rc = fmc_i2c_eeprom_write(fd, fmc, dev, FMC_FRU_OFF, fmc_fru_buf, n);
  if (rc < 0)
    return(rc);
  
  return(n);
}

/*--------------------------------------------------------------------------------------------------------------------*/

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * Function name : fmc_fru_read
 * Prototype     : int
 * Parameters    : filename, quiet
 * Return        : return the size of FRU or <0 on error
 * Scope         : global
 *----------------------------------------------------------------------------
 * Description   : read FRU from a file into fmc_fru_buf
 *
 *++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

int fmc_fru_read(const char *filename, int quiet)
{
  int n;
  FILE *fin;
  
  if (filename == NULL)
    return(-1);
  
  fin = fopen(filename, "rb");
  if (fin == NULL)
  {
    if (!quiet)
      fprintf(stderr, "Cannot open file '%s'\n", filename);
    return(-1);
  }
  
  n = fread(fmc_fru_buf, 1, sizeof(fmc_fru_buf), fin);
  
  if (ferror(fin))
    n = -1;
  
  fclose(fin);
  
  return(n);
}

/*--------------------------------------------------------------------------------------------------------------------*/

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * Function name : fmc_fru_write
 * Prototype     : int
 * Parameters    : filename, quiet
 * Return        : return the size of FRU or <0 on error
 * Scope         : global
 *----------------------------------------------------------------------------
 * Description   : write FRU ifrom fmc_fru_buf to a file 
 *
 *++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

int fmc_fru_write(const char *filename, int quiet)
{
  int n;
  FILE *fout;
  
  if (filename == NULL)
    return(-1);
  
  fout = fopen(filename, "wb");
  if (fout == NULL)
  {
    if (!quiet)
      fprintf(stderr, "Cannot create file '%s'\n", filename);
    return(-1);
  }
  
  n = fmc_fru_calc_size();
  
  if (fwrite(fmc_fru_buf, n, 1, fout) != 1)
  {
    if (!quiet)
      fprintf(stderr, "Error writing file '%s'\n", filename);
    n = -1;
  }
    
  fclose(fout);
  
  return(n);
}

/*--------------------------------------------------------------------------------------------------------------------*/

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * Function name : fmc_calc_chksum
 * Prototype     : void
 * Parameters    : buf, len
 * Return        : checksum
 * Scope         : local
 *----------------------------------------------------------------------------
 * Description   : Calculate a check sum
 *
 *++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

unsigned char fmc_calc_chksum(unsigned char *buf, int len)
{
  int i;
  unsigned char cks = 0;
  unsigned char ch;

  for (i = 0; i < len; i++)
  {
    ch = buf[i];
    cks += ch;
  }
  return cks;
}

/*--------------------------------------------------------------------------------------------------------------------*/

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * Function name : fmc_sign_write
 * Prototype     : void
 * Parameters    : buf, len
 * Return        : checksum
 * Scope         : global
 *----------------------------------------------------------------------------
 * Description   : Write signature into EEPROM
 *
 *++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

int fmc_sign_write(int fd, int fmc, uint dev, fmc_sign_t *sign)
{
  /* update check sum */
  sign->cks = fmc_calc_chksum((unsigned char *)sign, sizeof(sign));
  
  return fmc_i2c_eeprom_write(fd, fmc, dev, FMC_SIGN_OFF, (unsigned char *)sign, sizeof(fmc_sign_t));
}

/*--------------------------------------------------------------------------------------------------------------------*/

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * Function name : fmc_sign_read
 * Prototype     : void
 * Parameters    : buf, len
 * Return        : 0 = signature read checksum incorrect
 *                 1 = signature read checksum correct  
 * Scope         : global
 *----------------------------------------------------------------------------
 * Description   : Read signature from EEPROM and calculate checksum
 *
 *++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

int fmc_sign_read(int fd, int fmc, uint dev, fmc_sign_t *sign)
{
  int rc, calc_cks;
  
  rc = fmc_i2c_eeprom_read(fd, fmc, dev, FMC_SIGN_OFF, (unsigned char *)sign, sizeof(fmc_sign_t));
  if (rc >= 0)
  {
    calc_cks = fmc_calc_chksum((unsigned char *)sign, sizeof(sign));
    rc = ((calc_cks != 0xFF)?1:0);
  }
  return (rc);
}

/*--------------------------------------------------------------------------------------------------------------------*/

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * Function name : fmc_sign_print
 * Prototype     : void
 * Parameters    : buf, len
 * Return        : 0 = signature read checksum incorrect
 *                 1 = signature read checksum correct  
 * Scope         : global
 *----------------------------------------------------------------------------
 * Description   : Display FMC signature
 *
 *++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

int fmc_sign_print(FILE *fout, fmc_sign_t *sign)
{
  unsigned char *p;
  
  if (sign == NULL)
    return(-1);
  
  if (fout == NULL)
    fout = stdout;

  p = (unsigned char *)sign->board_name;
  fprintf(fout, "%-30s : %c%c%c%c%c%c%c%c\n",  "Board Name", p[0],p[1],p[2],p[3],p[4],p[5],p[6],p[7]);
  p = (unsigned char *)sign->serial;
  fprintf(fout, "%-30s : %c%c%c%c\n",          "Serial Number", p[0],p[1],p[2],p[3]);
  p = (unsigned char *)sign->version;
  fprintf(fout, "%-30s : %c%c%c%c%c%c%c%c\n",  "PCB Version", p[0],p[1],p[2],p[3],p[4],p[5],p[6],p[7]);
  p = (unsigned char *)sign->revision;
  fprintf(fout, "%-30s : %c%c\n",              "Hardware Revision", p[0],p[1]);
  p = (unsigned char *)sign->test_date;
  fprintf(fout, "%-30s : %c%c%c%c%c%c%c%c\n",  "Test Date", p[0],p[1],p[2],p[3],p[4],p[5],p[6],p[7]);

  return (0);
}

/*--------------------------------------------------------------------------------------------------------------------*/

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * Function name : fmc_sign_set
 * Prototype     : void
 * Parameters    : buf, len
 * Return        : >=0    on sucess number of field updated
 *                 -1     on error
 * Scope         : global
 *----------------------------------------------------------------------------
 * Description   : Set field in signature
 *
 *++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

int fmc_sign_set(fmc_sign_t *sign, 
                 char *board_name, char *serial, char *version, char *revision, 
                 int set_test_date, int quiet)
{
  int n = 0;
  char buf[10];
  time_t now;
  
  if (sign == NULL) 
    return(-1);

  if (set_test_date)
  {
    now = time(0);
    strftime(buf, sizeof(buf), "%d%m%Y", gmtime(&now));

    if (!quiet)
      printf("current date: %s\n", buf);
  }
    
  if (!quiet && board_name != NULL)
    printf("%s signature\n", board_name);
  
  if (board_name != NULL)
  {
    memcpy(sign->board_name,  board_name, sizeof(sign->board_name));
    n++;
  }
  
  if (serial != NULL)
  {
    memcpy(sign->serial,      serial,     sizeof(sign->serial));
    n++;
  }
  
  if (version != NULL)
  {
    memcpy(sign->version,     version,    sizeof(sign->version));
    n++;
  }
  
  if (revision != NULL)
  {
    memcpy(sign->revision,    revision,   sizeof(sign->revision));
    n++;
  }
  
  if (set_test_date)
  {
    strncpy((char *)sign->test_date,  buf,        sizeof(sign->test_date));
    n++;
  }
  
  return (n);
}

/*--------------------------------------------------------------------------------------------------------------------*/
