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

INCDIRS = -I ../include 
RANLIB = ranlib

all: libtsc.a

tsculib.o:tsculib.c 
	$(CC) $(INCDIRS) $(EXTRAFLAGS) -c tsculib.c
clilib.o:clilib.c 
	$(CC) $(INCDIRS) $(EXTRAFLAGS) -c clilib.c
pev791xlib.o:pev791xlib.c 
	$(CC) $(INCDIRS) $(EXTRAFLAGS) -c pev791xlib.c
tstlib.o:tstlib.c	
	$(CC) $(INCDIRS) $(EXTRAFLAGS) -c tstlib.c
libtsc.a:tsculib.o clilib.o pev791xlib.o tstlib.o
	$(AR) r libtsc.a tsculib.o clilib.o pev791xlib.o tstlib.o
	$(RANLIB) libtsc.a

tsculib.c: ../include/tsculib.h
clilib.c: ../include/cli.h
tstlib.c: ../include/tstlib.h

clean:
	$(RM) *.o *.a
