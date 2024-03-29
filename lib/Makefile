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

CFLAGS          := -fPIC -MP -MMD -Wall $(INCDIRS) $(EXTRAFLAGS)

SRCS_LIB_TSC  := tsculib.c clilib.c tscextlib.c tstlib.c ponmboxlib.c \
                 mtca4amclib.c mtca4rtmlib.c ltc2489lib.c pca9539lib.c i2ceepromlib.c \
                 fbi1482lib.c rcf1450lib.c rsp1461lib.c rdt1465lib.c rcc1466lib.c dacwgenlib.c \
                 fmclib.c

SRCS_LIB_ADC  := adclib.c ads42lb69lib.c dac38j84lib.c gscopelib.c gscope3110lib.c fscope3112lib.c scope1430lib.c \
                 daq1430lib.c adc3110lib.c adc3112lib.c adc3117lib.c adc3210lib.c zdclib.c lmklib.c xilinxlib.c

OBJS_LIB_TSC	:= $(SRCS_LIB_TSC:.c=.o)
OBJS_LIB_ADC	:= $(SRCS_LIB_ADC:.c=.o)

all: libtsc.a libadc.a

libtsc.a: $(OBJS_LIB_TSC)
	$(AR) r $@ $^
	$(RANLIB) $@

libadc.a: $(OBJS_LIB_ADC)
	$(AR) r $@ $^
	$(RANLIB) $@

%o: %c
	$(CC) $(CFLAGS) -o $@ -c $<

-include $(SRCS_LIB_TSC:.c=.d)
-include $(SRCS_LIB_ADC:.c=.d)

clean:
	$(RM) *.o *.d *.a
