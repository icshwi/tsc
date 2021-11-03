#=========================< begin file & file header >=======================
#  References
#  
#    filename : Makefile
#    author   : JFG, XP
#    company  : IOxOS
#    creation : Sept 14,2015
#
#----------------------------------------------------------------------------
#  Description
#
#    This Makefile allows to build the TscMon application
#
#----------------------------------------------------------------------------
#
#  Copyright Notice
#  
#    Copyright and all other rights in this document are reserved by 
#    IOxOS Technologies SA. This documents contains proprietary information    
#    and is supplied on express condition that it may not be disclosed, 
#    reproduced in whole or in part, or used for any other purpose other
#    than that for which it is supplies, without the written consent of  
#    IOxOS Technologies SA                                                        
#
#=============================< end file header >============================

INCDIR = ../../include 
LIBDIR = ../../lib

INC = -I $(INCDIR)
LIB = -L $(LIBDIR)

CFLAGS	:= -MMD -MP -Wall $(INC)

CFILES	:= TscMon.c script.c rdwr.c conf.c map.c mbox.c amc.c rtm.c dma.c 
CFILES	+= timer.c sflash.c buf.c tst.c semaphore.c fifo.c lmk.c 
CFILES	+= ddr.c i2c.c adc3110.c adc3117.c adc3112.c adc3210.c acq1430.c alias.c 
CFILES	+= tdma.c fbi1482.c rcf1450.c rsp1461.c rdt1465.c rcc1466.c gscope.c

OFILES	:= $(CFILES:.c=.o)


all : TscMon SmemCalibration

%o: %c
	$(CC) $(CFLAGS) -c -o $@ $<

-include $(CFILES:.c=.d)

TscMon: $(OFILES) $(LIBDIR)/libtsc.a $(LIBDIR)/libadc.a 
	$(CC) $(LIB) -o $@ $(OFILES) -lrt -lm -ladc -ltsc

SmemCalibration: SmemCalibration.o $(LIBDIR)/libtsc.a 
	$(CC) $(LIB) -o $@ $< -ltsc -lrt -lm

.PHONY: clean
clean:
	$(RM) *.d *.o *.a SmemCalibration TscMon

