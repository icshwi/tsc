#=========================< begin file & file header >=======================
#  References
#  
#    filename : Makefile
#    author   : JFG, XP
#    company  : IOxOS
#    creation : November 1,2016
#
#----------------------------------------------------------------------------
#  Description
#
#    This Makefile allows to build the device driver module for the TSC
#
#----------------------------------------------------------------------------
#
#  Copyright (C) IOxOS Technologies SA <ioxos@ioxos.ch>
#
#    THIS SOFTWARE IS PROVIDED BY THE AUTHOR AND CONTRIBUTORS ``AS IS'' AND
#    ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
#    IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
#    ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR OR CONTRIBUTORS BE LIABLE
#    FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
#    DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
#    OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
#    HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
#    LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
#    OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
#    SUCH DAMAGE.
# 
#    GPL license :
#    This program is free software; you can redistribute it and/or
#    modify it under the terms of the GNU General Public License
#    as published by the Free Software Foundation; either version 2
#    of the License, or (at your option) any later version.
# 
#    This program is distributed in the hope that it will be useful,
#    but WITHOUT ANY WARRANTY; without even the implied warranty of
#    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#    GNU General Public License for more details.
# 
#    You should have received a copy of the GNU General Public License
#    along with this program; if not, write to the Free Software
#    Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
# 
#=============================< end file header >============================

obj-m := tsc.o pon.o 
tsc-objs := tscdrvr.o ioctllib.o tscklib.o maplib.o irqlib.o mapmaslib.o  rdwrlib.o sflashlib.o \
	dmalib.o timerlib.o fifolib.o i2clib.o semaphorelib.o userirqlib.o
pon-objs := pondrvr.o

ifeq ($(ARCH),powerpc)
# JFG platform
#	KERNELDIR = /T2080IFC_2.0/linux/build/
# XP platform
	#KERNELDIR = /data/T2081/rootfs_2080_1.9/lib/modules/3.12.37-rt51+g43cecda/build/
	#KERNELDIR = /data/QorIQ-SDK-V2.0-20160527-yocto/build_t2080rdb-64b/tmp/work/t2080rdb_64b-fsl-linux/linux-qoriq/4.1-r0/build/
	#KERNELDIR = /data/QorIQ-SDK-V2.0-20160527-yocto/build_t2080rdb-64b/tmp/work/t2080rdb_64b-fsl-linux/linux-qoriq/4.1-r0/build/
	KERNELDIR =  /data/QorIQ-SDK-V2.0-20160527-yocto/build_ifc1211/tmp/work/ifc1211-fsl-linux/linux-qoriq/4.1-r0/build/
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
