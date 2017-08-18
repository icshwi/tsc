#=========================< begin file & file header >=======================
#  References
#  
#    filename : Makefile
#    author   : JFG, XP
#    company  : IOxOS
#    creation : November 1,2016
#    version  : 0.0.1
#
#----------------------------------------------------------------------------
#  Description
#
#    This Makefile allows to build the device driver module for the TSC
#
#----------------------------------------------------------------------------
#  Copyright Notice
#  
#    Copyright and all other rights in this document are reserved by 
#    IOxOS Technologies SA. This documents contains proprietary information    
#    and is supplied on express condition that it may not be disclosed, 
#    reproduced in whole or in part, or used for any other purpose other
#    than that for which it is supplies, without the written consent of  
#    IOxOS Technologies SA                                                        
#
#----------------------------------------------------------------------------
#  Change History
#  
#=============================< end file header >============================

obj-m := tsc.o pon.o 
tsc-objs := tscdrvr.o ioctllib.o tscklib.o maplib.o irqlib.o mapmaslib.o mapslvlib.o rdwrlib.o sflashlib.o \
	dmalib.o timerlib.o fifolib.o i2clib.o semaphorelib.o
pon-objs := pondrvr.o

ifeq ($(ARCH),powerpc)
# JFG platform
    KERNELDIR = /T2080IFC_2.0/linux/build/
# XP platform
	#KERNELDIR = /data/T2081/rootfs_2080_1.9/lib/modules/3.12.37-rt51+g43cecda/build/
	#KERNELDIR = /data/QorIQ-SDK-V2.0-20160527-yocto/build_t2080rdb-64b/tmp/work/t2080rdb_64b-fsl-linux/linux-qoriq/4.1-r0/build/
	#KERNELDIR = /data/QorIQ-SDK-V2.0-20160527-yocto/build_t2080rdb-64b/tmp/work/t2080rdb_64b-fsl-linux/linux-qoriq/4.1-r0/build/
#	KERNELDIR =  /data/QorIQ-SDK-V2.0-20160527-yocto/build_ifc1211/tmp/work/ifc1211-fsl-linux/linux-qoriq/4.1-r0/build/
else
	KERNELDIR ?= /lib/modules/$(shell uname -r)/build
endif
EXTRA_CFLAGS := $(ADD_CFLAGS)
PWD := $(shell pwd)
default:
ifeq ($(ARCH),powerpc)
	$(MAKE) -I . ARCH=$(ARCH) CROSS_COMPILE=$(CROSS_COMPILE)  -C $(KERNELDIR) M=$(PWD) modules
else
	$(MAKE) -I . -C $(KERNELDIR) M=$(PWD) modules
endif
clean:
	$(MAKE) -C $(KERNELDIR) M=$(PWD) clean
	@rm -f Module.symvers
	$(RM) *.o *.ko *.mod.c
