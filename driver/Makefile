#=========================< begin file & file header >=======================
#  References
#  
#    filename : Makefile
#    author   : JFG, XP
#               Jeong Han Lee
#    company  : IOxOS
#    creation : November 1,2016
#    modified : Thursday, October 17 20:05:35 CEST 2019
#----------------------------------------------------------------------------
#  Description
#
#    This Makefile allows to build the device driver module for the TSC
#
#----------------------------------------------------------------------------
#
#  Copyright (C) IOxOS Technologies SA <ioxos@ioxos.ch>
#  Copyright (C) 2019  European Spallation Source ERIC
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


ifneq ($(KERNELRELEASE),)

obj-m := tsc.o pon.o 
tsc-y := tscdrvr.o ioctllib.o tscklib.o maplib.o irqlib.o mapmaslib.o  rdwrlib.o sflashlib.o \
	    dmalib.o timerlib.o fifolib.o i2clib.o semaphorelib.o userirqlib.o
pon-y := pondrvr.o

ccflags-y := -I$(src)/../include
else

KERNELDIR ?= /lib/modules/$(shell uname -r)/build
PWD := $(shell pwd)

all: modules

modules modules_install clean:
	$(MAKE) -C $(KERNELDIR) M=$(PWD) $@

.PHONY: all modules modules_install clean

endif


