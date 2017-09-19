#=========================< begin file & file header >=======================
#  References
#  
#    filename : Makefile
#    author   : JFG
#    company  : IOxOS
#    creation : june 30,2008
#    version  : 0.0.1
#
#----------------------------------------------------------------------------
#  Description
#
#    This Makefile allows to build the PEV1100 test launcher AltTst
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
LIBDIR = ../../lib
INCDIR = ../../include

OLIB = $(LIBDIR)/libtsc.a
OFILES = tst_0x.o tst_1x.o tst_2x.o tst_3x.o tst_4x.o tst_5x.o
HFILES = $(INCDIR)/cli.h $(INCDIR)/tsctst.h $(INCDIR)/tstlib.h

all : TscTst

INC = -I $(INCDIR)
LIB = -L $(LIBDIR)

TscTst.o: TscTst.c $(HFILES) tstlist.h
	$(CC) -Wall $(FLAGS) $(INC) -c TscTst.c

tst_0x.o: tst_0x.c $(HFILES)
	$(CC) -Wall $(FLAGS) $(INC) -c tst_0x.c

tst_1x.o: tst_1x.c $(HFILES)
	$(CC) -Wall $(FLAGS) $(INC) -c tst_1x.c

tst_2x.o: tst_2x.c $(HFILES)
	$(CC) -Wall $(FLAGS) $(INC) -c tst_2x.c

tst_3x.o: tst_3x.c $(HFILES)
	$(CC) -Wall $(FLAGS) $(INC) -c tst_3x.c

tst_4x.o: tst_4x.c $(HFILES)
	$(CC) -Wall $(FLAGS) $(INC) -c tst_4x.c
	
tst_5x.o: tst_5x.c $(HFILES)
	$(CC) -Wall $(FLAGS) $(INC) -c tst_5x.c

TscTst: TscTst.o $(OFILES)  $(OLIB)
	$(CC) -Wall $(LIB) -o TscTst TscTst.o  $(OFILES) -ltsc -lrt

clean:
	$(RM) *.o TscTst
