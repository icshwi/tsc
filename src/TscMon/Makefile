#=========================< begin file & file header >=======================
#  References
#  
#    filename : Makefile
#    author   : JFG
#    company  : IOxOS
#    creation : Sept 14,2015
#    version  : 0.0.1
#
#----------------------------------------------------------------------------
#  Description
#
#    This Makefile allows to build the user library for the ALTHEA7910
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
#  $Log : $
#  
#=============================< end file header >============================

INCDIR = ../../include 
LIBDIR = ../../lib

INC = -I $(INCDIR)
LIB = -L $(LIBDIR)

OFILES = TscMon.o script.o rdwr.o conf.o map.o dma.o timer.o sflash.o buf.o tst.o semaphore.o fifo.o lmk.o i2c.o adc3110.o adc3117.o
HFILES = $(INCDIR)/cli.h 

all : TscMon

conf.o: conf.c cmdlist.h Makefile
	$(CC) $(FLAGS) $(INC) -c conf.c

map.o: map.c cmdlist.h Makefile
	$(CC) $(FLAGS) $(INC) -c map.c

rdwr.o: rdwr.c cmdlist.h Makefile
	$(CC) $(FLAGS) $(INC) -c rdwr.c

fifo.o: fifo.c cmdlist.h Makefile
	$(CC) $(FLAGS) $(INC) -c fifo.c

script.o: script.c cmdlist.h Makefile
	$(CC) $(FLAGS) $(INC) -c script.c

dma.o: dma.c cmdlist.h Makefile
	$(CC) $(FLAGS) $(INC) -c dma.c

timer.o: timer.c cmdlist.h Makefile
	$(CC) $(FLAGS) $(INC) -c timer.c

semaphore.o: semaphore.c cmdlist.h Makefile
	$(CC) $(FLAGS) $(INC) -c semaphore.c

sflash.o: sflash.c cmdlist.h Makefile
	$(CC) $(FLAGS) $(INC) -c sflash.c

buf.o: buf.c cmdlist.h Makefile
	$(CC) $(FLAGS) $(INC) -c buf.c

tst.o: tst.c cmdlist.h Makefile
	$(CC) $(FLAGS) $(INC) -c tst.c

lmk.o: lmk.c cmdlist.h Makefile
	$(CC) $(FLAGS) $(INC) -c lmk.c

i2c.o: i2c.c cmdlist.h Makefile
	$(CC) $(FLAGS) $(INC) -c i2c.c

adc3110.o: adc3110.c cmdlist.h Makefile
	$(CC) $(FLAGS) $(INC) -c adc3110.c

adc3117.o: adc3117.c cmdlist.h Makefile
	$(CC) $(FLAGS) $(INC) -c adc3117.c

TscMon.o: TscMon.c cmdlist.h Makefile
	$(CC) $(FLAGS) $(INC) -c TscMon.c

TscMon:$(OFILES)  $(OLIB) $(LIBDIR)/libtsc.a
	$(CC) $(LIB) -o TscMon $(OFILES) -ltsc -lrt -lm
clean:
	$(RM) *.o *.a
