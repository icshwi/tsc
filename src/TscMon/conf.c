/*=========================< begin file & file header >=======================
 *  References
 *  
 *    filename : conf.c
 *    author   : JFG, XP
 *    company  : IOxOS
 *    creation : Sept 14,2015
 *
 *----------------------------------------------------------------------------
 *  Description
 *
 *     That file contains a set of function called by TscMon to configure
 *     the TSC interface.
 *
 *----------------------------------------------------------------------------
 *
 *  Copyright Notice
 *
 *    Copyright and all other rights in this document are reserved by
 *    IOxOS Technologies SA. This documents contains proprietary information
 *    and is supplied on express condition that it may not be disclosed,
 *    reproduced in whole or in part, or used for any other purpose other
 *    than that for which it is supplies, without the written consent of
 *    IOxOS Technologies SA
 *
 *=============================< end file header >============================*/

#ifndef lint
static char *rcsid = "$Id: conf.c,v 1.6 2015/12/03 15:14:50 ioxos Exp $";
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
#include <pev791xlib.h>

char *
conf_rcsid()
{
  return( rcsid);
}

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * Function name : conf_show_static
 * Prototype     : void
 * Parameters    : none
 * Return        : none
 *----------------------------------------------------------------------------
 * Description   : display static switch configuration
 *
 *++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

void
conf_show_static( void)
{
  int d0;
  int a24_base;

  tsc_csr_read( 0x00, &d0);
  printf("   Static Options [0x%08x]\n", d0);

  if( d0 & (1<<31))
  {
    printf("      PCI IO size          : 256 Bytes\n");
  }
  else 
  {
    printf("      PCI IO size          : 4 KBytes\n");
  }
  if( d0 & 0x7000)
  {
    printf("      PCI MEM size         : ");
    printf("%d MBytes\n", 2 << ((d0>>12)&7));
  }
  else
  {
    printf("      PCI MEM size         : disabled\n");
  }
  if( d0 & 0xe00)
  {
    int tmp;

    tmp = (d0>>9)&7;
    printf("      PCI PMEM size        : ");
    if( d0 & (1<<8))
    {
      printf("%d MBytes\n", 32 << tmp);
    }
    else
    {
      printf("A32 -> ");
      if( tmp > 5)
      {
        printf("size invalid\n");
      }
      else 
      {
        printf("%d MBytes\n", 32 << tmp);
      }
    }
  }
  else
  {
    printf("      PCI PMEM size        : disabled\n");
  }

  return;
}

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * Function name : conf_show_ddr
 * Prototype     : void
 * Parameters    : none
 * Return        : none
 *----------------------------------------------------------------------------
 * Description   : display ddr information
 *
 *++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

void
conf_show_ddr( void)
{
  int d0;

  printf("   DDR3 configuration\n");

  tsc_csr_read( TSC_CSR_SMEM_DDR3_CSR, &d0);
  printf("      DDR3_SIZE            : ");
    switch( (d0 >> 2) & 3)
    {
      case 0:
      {
        printf("Not Present\n");
        break;
      }
      case 1:
      {
        printf("2 Gbit [0x10000000]\n");
        break;
      }
      case 2:
      {
        printf("4 Gbit [0x40000000]\n");
        break;
      }
      case 3:
      {
        printf("8 Gbit [0x80000000]\n");
        break;
      }
    }
  return;
}

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * Function name : conf_show_ipcie
 * Prototype     : void
 * Parameters    : none
 * Return        : none
 *----------------------------------------------------------------------------
 * Description   : display ipcie information
 *
 *++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

void
conf_show_ipcie( void)
{
  int d0;

  printf("    PCIE identifier \n");

  tsc_csr_read( TSC_CSR_IPCIE, &d0);
  printf("      Board ID             : ");
    switch( d0 & 0xf)
    {
      case 0:
      {
        printf("Reserved\n");
        break;
      }
      case 1:
      {
        printf("IFC 1410\n");
        break;
      }
      case 2:
      {
        printf("IFC 1411\n");
        break;
      }
      default:
      {
        printf("Others\n");
        break;
      }
    }
  printf("      Revision ID          : %x\n", (d0 >> 3) & 0xf);
  printf("      PCIe EP              : ");
    switch( (d0 >> 8) & 0xf)
      {
        case 0:
        {
          printf("PCIe EP 0\n");
          break;
        }
        case 1:
        {
          printf("PCIe EP 1\n");
          break;
        }
        case 2:
        {
          printf("PCIe EP 2\n");
          break;
        }
        default:
        {
          printf("Undefined\n");
          break;
        }
      }
    if (((d0 & 0xf) == 1) | ((d0 & 0xf) == 2)){
      printf("      MTCA Slot ID         : %x\n", (d0 >> 12) & 0xf);
    }
  return;
}

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * Function name : conf_show_identifiers
 * Prototype     : void
 * Parameters    : none
 * Return        : none
 *----------------------------------------------------------------------------
 * Description   : display tsc identifiers
 *
 *++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

void
conf_show_identifiers( void)
{
  int d0;

  printf("   Identifiers\n");

  tsc_csr_read( TSC_CSR_ILOC_EFUSE_USR, &d0);
  printf("      FPGA eFUSE           : 0x%08x\n", d0);

  tsc_csr_read( TSC_CSR_ILOC_SIGN, &d0);
  printf("      FPGA Signature       : 0x%08x\n", d0);

  tsc_csr_read( TSC_CSR_ILOC_GENCTL, &d0);
  printf("      FPGA Version         : 0x%08x\n", d0);

  tsc_csr_read( TSC_CSR_ILOC_TOSCA2_SIGN, &d0);
  printf("      TOSCA Signature      : 0x%08x\n", d0);


  return;
}

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * Function name : conf_show_msi
 * Prototype     : void
 * Parameters    : none
 * Return        : none
 *----------------------------------------------------------------------------
 * Description   : display pcie msi counters
 *
 *++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

void
conf_show_msi( void)
{
  int d0;

  printf("   PCIe MSI counters\n");

  tsc_csr_read( TSC_CSR_A7_PCIE_REQCNT, &d0);
  printf("      MSI Requests         : 0x%08x\n", d0);

  tsc_csr_read( TSC_CSR_A7_PCIE_ARMCNT, &d0);
  printf("      MSI Armed            : 0x%08x\n", d0);

  tsc_csr_read( TSC_CSR_A7_PCIE_ACKCNT, &d0);
  printf("      MSI Acknowledge      : 0x%08x\n", d0);

  tsc_csr_read( TSC_CSR_A7_PCIE_CTL, &d0);
  printf("      MSI Status           : 0x%08x\n", d0);

  return;
}

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * Function name : conf_show_smon
 * Prototype     : void
 * Parameters    : none
 * Return        : none
 *----------------------------------------------------------------------------
 * Description   : display artix7 system monitoring parameters
 *
 *++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

void
conf_show_smon( void)
{
  int d0, d1, d2;
  double f0, f1, f2;

  d0 = 0x3000;
  tsc_smon_write( 0x41, &d0);
  printf("   Artix7 System Monitoring\n");
  tsc_smon_read( 0x00, &d0);
  f0 = (((double)(d0>>6)*503.975)/1024.) - (double)273.15;
  tsc_smon_read( 0x20, &d1);
  f1 = (((double)(d1>>6)*503.975)/1024.) - 273.15;
  tsc_smon_read( 0x24, &d2);
  f2 = (((double)(d2>>6)*503.975)/1024.) - 273.15;
  printf("      Temperature          : %.2f [%.2f - %.2f]\n", f0, f1, f2);
  tsc_smon_read( 0x01, &d0);
  f0 = (((double)(d0>>6)*3.0)/1024.);
  tsc_smon_read( 0x21, &d1);
  f1 = (((double)(d1>>6)*3.0)/1024.);
  tsc_smon_read( 0x25, &d2);
  f2 = (((double)(d2>>6)*3.0)/1024.);
  printf("      VCCint               : %.2f [%.2f - %.2f]\n", f0, f1, f2);
  tsc_smon_read( 0x02, &d0);
  f0 = (((double)(d0>>6)*3.0)/1024.);
  tsc_smon_read( 0x22, &d1);
  f1 = (((double)(d1>>6)*3.0)/1024.);
  tsc_smon_read( 0x26, &d2);
  f2 = (((double)(d2>>6)*3.0)/1024.);
  printf("      VCCaux               : %.2f [%.2f - %.2f]\n", f0, f1, f2);
  d0 = 3;
  tsc_smon_write( 0x40, &d0);

  return;
}

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * Function name : conf_show_lm95255
 * Prototype     : void
 * Parameters    : none
 * Return        : none
 *----------------------------------------------------------------------------
 * Description   : display lm95235 information
 *
 *++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

void
conf_show_lm95235( void)
{
  uint dev, reg, data;
  unsigned char mid, did;
  char temp;
  uint sts;

  if( (tsc_get_device_id() != TSC_BOARD_TSC_IO) || (tsc_get_device_id() != TSC_BOARD_TSC_CENTRAL))
  {
    return;
  }
  dev = I2C_DEV( 0x4c, 0, 0);
  reg = 0xfe;
  sts = tsc_i2c_read( dev, reg, &data);
  if( (sts & TSC_I2C_CTL_EXEC_MASK) == TSC_I2C_CTL_EXEC_ERR)
  {
    printf("   Cannot access LM95235 [%08x]\n", sts);
    return;
  }
  mid = (unsigned char)data;
  reg = 0xff;
  tsc_i2c_read( dev, reg, &data);
  did = (char)data;
  if( (mid == 0x01) && (did == 0xb1))
  {
    printf("   LM95235 Temperature sensor\n");
    reg = 0;
    tsc_i2c_read( dev, reg, &data);
    temp = (unsigned char)data;
    printf("      Local Temp           : %d\n", temp);
    reg = 1;
    tsc_i2c_read( dev, reg, &data);
    temp = (char)data;
    printf("      Remote Temp          : %d\n", temp);
  }
  else
  {
    printf("   unrecognized device %02x:%02x\n", mid, did);
  }

  return;

}

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * Function name : conf_show_bmr463
 * Prototype     : void
 * Parameters    : none
 * Return        : none
 *----------------------------------------------------------------------------
 * Description   : display bmr463 system monitoring parameters
 *
 *++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

void
conf_show_bmr463( void)
{
  unsigned short d0, d1, d2, d3;
  float f0, f1, f2, f3;
  int i;
  uint sts, data;
  int board_type;

  tsc_pon_read(0, &board_type);
  if(  board_type != TSC_BOARD_IFC1211)
  {
    return;
  }
  printf("   DC-DC Voltage Regulators\n");
  for( i = 0; i < 3; i++)
  {
    sts = pev791x_bmr_read( i, 0x88, &data, 2);/*0x88*/
    if( (sts < 0))
    {
      printf("      BMR#%d -> readout error [%08x]\n", i,sts);
    }
    else
    {
      d0 = (unsigned short)data;
      f0 = pev791x_bmr_conv_11bit_u( d0);
      usleep( 10000);
      sts = pev791x_bmr_read( i, 0x8b, &data, 2);
      d1 = (unsigned short)data;
      f1 = pev791x_bmr_conv_16bit_u( d1);
      usleep( 10000);
      sts = pev791x_bmr_read( i, 0x8c, &data, 2);
      d2 = (unsigned short)data;
      f2 = pev791x_bmr_conv_11bit_u( d2);
      usleep( 10000);
      sts = pev791x_bmr_read( i, 0x8d, &data, 2);/*0x8d*/
      d3 = (unsigned short)data;
      f3 = pev791x_bmr_conv_11bit_s( d3);
      usleep( 10000);
      if( i == 0) printf("      BMR#%d T2081 core\n", i);
      if( i == 1) printf("      BMR#%d Central FPGA\n", i);
      if( i == 2) printf("      BMR#%d FMC mezzanines\n", i);
      printf("        VIN                : %5.2f [%04x]\n", f0, d0);
      printf("        VOUT               : %5.2f [%04x]\n", f1, d1);
      printf("        IOUT               : %5.2f [%04x]\n", f2, d2);
      printf("        TEMP               : %5.2f [%04x]\n", f3, d3);

    }
  }
  return;
}

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * Function name : conf_show_max5970
 * Prototype     : void
 * Parameters    : none
 * Return        : none
 *----------------------------------------------------------------------------
 * Description   : display max5970 system monitoring parameters
 *
 *++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

void
conf_show_max5970( void)
{
  int i, tmp;
  float f0, f1, f2;
  uint reg, sts, data;
  uint min, max, mean;
  uint max5970 = 0x20000030;
  int board_type;

  tsc_pon_read(0, &board_type);
  if(  board_type != TSC_BOARD_IFC1211)
  {
    return;
  }

  printf("   MAX5970 Voltage Monitor\n");
  sts = tsc_i2c_read( max5970, 0, &data);
  if( (sts & TSC_I2C_CTL_EXEC_MASK) == TSC_I2C_CTL_EXEC_ERR)
  {
    printf("      -> ERROR : cannot access device registers [%08x]\n", sts);
    return;
  }
  reg = 0;
  min = 0x400;
  max = 0x0;
  mean = 0;
  for( i = 0; i < 0x1000; i++)
  {
    sts = tsc_i2c_read( max5970, reg, &data);
    tmp = (data&0xff) << 2;
    sts = tsc_i2c_read( max5970, reg+1, &data);
    tmp |= data&3;
    mean += tmp;
    if( tmp > max) max = tmp;
    if( tmp < min) min = tmp;
    //usleep(2000);
  }
  mean = mean/0x1000;
  f0 = (float)mean;
  f0 = (f0*100)/(4*1024); 
  f1 = (float)min; 
  f1 = (f1*100)/(4*1024); 
  f2 = (float)max; 
  f2 = (f2*100)/(4*1024); 
  //printf("        Current 5V0 (A)    : %.2f [%.2f - %.2f]\n", f0, f1, f2);
  /* it seems the calculated value is 50% to high ?? */
  printf("        Current 5V0 (A)    : %.2f [%.2f - %.2f]\n", f0/1.5, f1/1.5, f2/1.5);
  reg = 2;
  sts = tsc_i2c_read( max5970, reg, &data);
  tmp = (data&0xff) << 2;
  sts = tsc_i2c_read( max5970, reg+1, &data);
  tmp |= data&3;
  f0 = (float)tmp;
  f0 = (f0*16)/1024; 
  printf("        Voltage 5V0 (V)    : %.2f \n", f0);
  reg = 4;
  min = 0x400;
  max = 0x0;
  mean = 0;
  for( i = 0; i < 0x1000; i++)
  {
    sts = tsc_i2c_read( max5970, reg, &data);
    tmp = (data&0xff) << 2;
    sts = tsc_i2c_read( max5970, reg+1, &data);
    tmp |= data&3;
    mean += tmp;
    if( tmp > max) max = tmp;
    if( tmp < min) min = tmp;
    //usleep(2000);
  }
  mean = mean/0x1000;
  f0 = (float)mean;
  f0 = (f0*100)/(3*1024); 
  f1 = (float)min; 
  f1 = (f1*100)/(3*1024); 
  f2 = (float)max; 
  f2 = (f2*100)/(3*1024); 
  //printf("        Current 3V3 (A)    : %.2f [%.2f - %.2f]\n", f0, f1, f2);
  /* it seems the calculated value is 50% to high ?? */
  printf("        Current 3V3 (A)    : %.2f [%.2f - %.2f]\n", f0/1.5, f1/1.5, f2/1.5);

  reg = 6;
  sts = tsc_i2c_read( max5970, reg, &data);
  tmp = (data&0xff) << 2;
  sts = tsc_i2c_read( max5970, reg+1, &data);
  tmp |= data&3;
  f0 = (float)tmp;
  f0 = (f0*16)/1024; 
  printf("        Voltage 3V3 (V)    : %.2f \n", f0);
  return;
}

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * Function name : conf_show_device
 * Prototype     : void
 * Parameters    : none
 * Return        : none
 *----------------------------------------------------------------------------
 * Description   : display fpga device
 *
 *++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

void conf_show_device(void){
	int device = -1;

	device = tsc_get_device();
	if(device == 0){
		printf("   Current device\n");
		printf("      central\n");
	}
	else if(device == 1){
		printf("   Current device\n");
		printf("      io\n");
	}
	else{
		printf("   No device found ! \n");
	}

	return;
}

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * Function name : tsc_conf_show
 * Prototype     : void
 * Parameters    : none
 * Return        : none
 *----------------------------------------------------------------------------
 * Description   : display tsc configuration
 *
 *++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

int 
tsc_conf_show(struct cli_cmd_para *c)
{
  int i, cnt;
  int retval;

  retval = 0;
  if( !c->cnt)
  {
    conf_show_static();
    conf_show_ipcie();
    conf_show_ddr();
    conf_show_identifiers();
    conf_show_msi();
    conf_show_smon();
    //conf_show_lm95235();
    conf_show_bmr463();
    conf_show_max5970();
    conf_show_device();
    return( retval);
  }
  cnt = c->cnt;
  i = 0;
  if( cnt--)
  {
    if( !strcmp( "show", c->para[i]))
    {
      int show_set;

      show_set = 0x0;
      if( !cnt)
      {
	show_set = 0x3f;
      }
      while( cnt--)
      {
	i++;
	if( !strncmp( "all", c->para[i], 3))
	{
	  show_set |= 0x7f;
	}
	if( !strncmp( "static", c->para[i], 3))
	{
	  show_set |= 0x1;
	}
	if( !strncmp( "identifiers", c->para[i], 3))
	{
	  show_set |= 0x4;
	}
	if( !strncmp( "msi", c->para[i], 3))
	{
	  show_set |= 0x10;
	}
	if( !strncmp( "smon", c->para[i], 3))
	{
	  show_set |= 0x20;
	}
	if( !strncmp( "pciei", c->para[i], 3))
	{
	  show_set |= 0x40;
	}
	//if( !strncmp( "lm95255", c->para[i], 2))
	//{
	//  show_set |= 0x80;
	//}
	if( !strncmp( "bmr463", c->para[i], 3))
	{
	  show_set |= 0x100;
	}
	if( !strncmp( "max5970", c->para[i], 3))
	{
	  show_set |= 0x200;
	}
	if( !strncmp( "device", c->para[i], 3))
	{
	  show_set |= 0x400;
	}
	if( !strncmp( "ddr", c->para[i], 3))
	{
	  show_set |= 0x8;
	}
	if( !strncmp( "pciei", c->para[i], 3))
	{
	  show_set |= 0x40;
	}
      }
      if( show_set & 0x1) conf_show_static();
      if( show_set & 0x4) conf_show_identifiers();
      if( show_set & 0x8) conf_show_ddr();
      if( show_set & 0x10) conf_show_msi();
      if( show_set & 0x20) conf_show_smon();
      if( show_set & 0x40) conf_show_ipcie();
      //if( show_set & 0x80) conf_show_lm95235();
      if( show_set & 0x100) conf_show_bmr463();
      if( show_set & 0x200) conf_show_max5970();
      if( show_set & 0x400) conf_show_device();
    }
  }
  return( retval);
}

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * Function name : tsc_set_device
 * Prototype     : void
 * Parameters    : none
 * Return        : none
 *----------------------------------------------------------------------------
 * Description   : set device between central or io
 *++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

int tsc_set_device(struct cli_cmd_para *c){
	int cnt    = 0;
	int retval = 0;

	cnt = c->cnt;
	if(cnt--){
		if(!strcmp( "device", c->para[0])){
			if (!strncmp( "central", c->para[1], 3)){
				retval = set_device(0);
				if(retval == 0){
					printf("   -> Device : central - OK\n");
				}
				else{
					printf("   -> Device : central - Not available\n");
				}
			}
			else if(!strncmp( "io", c->para[1], 3)){
				retval = set_device(1);
				if(retval == 0){
					printf("   -> Device : io - OK\n");
				}
				else{
					printf("   -> Device : io - Not available\n");
				}
			}
			else {
				printf("Bad parameter! Type \"? set\" for help \n");
			}
		}
		else {
			printf("Bad parameter! Type \"? set\" for help \n");
		}
	}
	return(retval);
}

