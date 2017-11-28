 
#=========================< begin file & file header >=======================
#  References
#  
#    filename : Makefile
#    author   : XP
#    company  : IOxOS
#    creation : Nov 22,2017
#    version  : 0.0.1
#
#----------------------------------------------------------------------------
#  Description
#
#    This Makefile allows to build the exmaples
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
#=============================< end file header >============================

INCDIR = ../../include 
LIBDIR = ../../lib

INC = -I $(INCDIR)
LIB = -L $(LIBDIR)

HFILES = $(INCDIR)/cli.h 

all : mapusr1 tscctl Ifc1211Pon

mapusr1: mapusr1.c
	$(CC) $(LIB) $(INC) -o mapusr1 mapusr1.c -ltsc -lrt -lm
tscctl: tscctl.c
	$(CC) $(LIB) $(INC) -o tscctl tscctl.c -ltsc -lrt -lm
Ifc1211Pon: Ifc1211Pon.c
	$(CC) $(LIB) $(INC) -o Ifc1211Pon Ifc1211Pon.c -ltsc -lrt -lm		
clean:
	$(RM) *.o *.a