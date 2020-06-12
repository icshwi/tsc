#
#  Copyright (c) 2019  European Spallation Source ERIC
#
#  The program is free software: you can redistribute
#  it and/or modify it under the terms of the GNU General Public License
#  as published by the Free Software Foundation, either version 2 of the
#  License, or any newer version.
#
#  This program is distributed in the hope that it will be useful, but WITHOUT
#  ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
#  FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
#  more details.
#
#  You should have received a copy of the GNU General Public License along with
#  this program. If not, see https://www.gnu.org/licenses/gpl-2.0.txt
#
# 
# Author  : Jeong Han Lee
# email   : jeonghan.lee@gmail.com
# Date    : Wednesday, November 27 12:01:40 CET 2019
# version : 0.0.2
#

TOP:=$(CURDIR)

ifeq ($(DESTDIR),)
    DESTDIR := $(TOP)/bin
endif

APPINC:=$(TOP)/include
APPLIB:=$(TOP)/lib
TSCMON:=$(TOP)/src/TscMon
TSCTST:=$(TOP)/src/TscTst

VPATH = $(APPINC) $(APPLIB) $(TSCMON) $(TSCTST)

CPPFLAGS += -I $(APPINC)
CPPFLAGS += -I $(TSCMON)
CPPFLAGS += -I $(TSCTST)

TSCLIB_SRCS := tsculib.c clilib.c tscextlib.c tstlib.c ponmboxlib.c \
	       	mtca4amclib.c mtca4rtmlib.c pca9539lib.c i2ceepromlib.c \
            fbi1482lib.c rcf1450lib.c rsp1461lib.c rdt1465lib.c rcc1466lib.c

ADCLIB_SRCS := adclib.c adc3112lib.c fscope3112lib.c adc3110lib.c gscopelib.c \
               gscope3110lib.c adc3117lib.c adc3210lib.c

TSCMON_SRCS := TscMon.c script.c rdwr.c conf.c map.c mbox.c amc.c rtm.c dma.c 
TSCMON_SRCS += timer.c sflash.c buf.c tst.c semaphore.c fifo.c lmk.c 
TSCMON_SRCS += ddr.c i2c.c adc3110.c adc3117.c adc3112.c acq1430.c alias.c 
TSCMON_SRCS += tdma.c fbi1482.c rcf1450.c rsp1461.c rdt1465.c rcc1466.c gscope.c

SMEMCA_SRCS :=SmemCalibration.c
#TSCTST_SRCS := $(wildcard $(TSCTST)/*.c)


TSCLIB_OBJS :=$(addsuffix .o,$(basename $(TSCLIB_SRCS)))
ADCLIB_OBJS :=$(addsuffix .o,$(basename $(ADCLIB_SRCS)))
TSCMON_OBJS :=$(addsuffix .o,$(basename $(TSCMON_SRCS)))
SMEMCA_OBJS :=$(addsuffix .o,$(basename $(SRC_SMEMCAL)))
TSCTST_OBJS :=$(addsuffix .o,$(basename $(TSCTST_SRCS)))

TSBLIB:= libtsc.so
ADCLIB:= libadc.so

CFLAGS := -Wall
LDFLAGS :=-Wall -lrt -lm

BINS := TscMon SmemCalibration 

build: $(BINS) 

TscMon: $(TSCMON_OBJS) $(ADCLIB_OBJS) $(TSCLIB_OBJS)
	$(LINK.c) $^ $(LOADLIBES) $(LDLIBS) -o $@

SmemCalibration: $(SMEMCA_OBJS) $(TSCMON_OBJS) $(ADCLIB_OBJS) $(TSCLIB_OBJS)
	$(LINK.c) $^ $(LOADLIBES) $(LDLIBS) -o $@

TscTst: $(TSCTST_OBJS) $(TSCMON_OBJS)
	$(LINK.c) $^ $(LOADLIBES) $(LDLIBS) -o $@

$(TSBLIB): $(TSCLIB_OBJS) 
	$(CXX) -shared -fPIC $(LDFLAGS) -o $@ $^

$(ADCLIB): $(ADCLIB_OBJS) $(TSCLIB_OBJS) 
	$(CXX) -shared -fPIC $(LDFLAGS) -o $@ $^

.c.o :
	$(COMPILE.c) $(OUTPUT_OPTION) $<

install: $(BINS) 
	install -d $(DESTDIR)/
	install -m 755 $^ $(DESTDIR)/

.PHONY: distclean clean build install

clean:
	@$(RM) $(SMEMCA_OBJS) $(TSCMON_OBJS) $(ADCLIB_OBJS) $(TSCLIB_OBJS)

distclean: clean
	@$(RM) $(BINS)
	@$(RM) -r $(DESTDIR)

PRINT.%:
	@echo $* = $($*)
	@echo $*\'s origin is $(origin $*)
