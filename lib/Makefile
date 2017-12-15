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
#    This Makefile allows to build the user library for the TSC
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

INCDIRS = -I ../include 
RANLIB = ranlib

all: libtsc.a

tsculib.o:tsculib.c  ../include/tscioctl.h
	$(CC) $(INCDIRS) $(EXTRAFLAGS) -c tsculib.c
clilib.o:clilib.c 
	$(CC) $(INCDIRS) $(EXTRAFLAGS) -c clilib.c
mtca4rtmlib.o:mtca4rtmlib.c 
	$(CC) $(INCDIRS) $(EXTRAFLAGS) -c mtca4rtmlib.c
ponmboxlib.o:ponmboxlib.c 
	$(CC) $(INCDIRS) $(EXTRAFLAGS) -c ponmboxlib.c
tscextlib.o:tscextlib.c 
	$(CC) $(INCDIRS) $(EXTRAFLAGS) -c tscextlib.c
tstlib.o:tstlib.c	
	$(CC) $(INCDIRS) $(EXTRAFLAGS) -c tstlib.c
libtsc.a:tsculib.o clilib.o tscextlib.o tstlib.o ponmboxlib.o mtca4rtmlib.o
	$(AR) r libtsc.a tsculib.o clilib.o tscextlib.o tstlib.o ponmboxlib.o mtca4rtmlib.o
	$(RANLIB) libtsc.a

tsculib.c: ../include/tsculib.h
clilib.c: ../include/cli.h
tstlib.c: ../include/tstlib.h

clean:
	$(RM) *.o *.a
