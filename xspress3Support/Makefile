TOP=..
include $(TOP)/configure/CONFIG
#----------------------------------------
#  ADD MACRO DEFINITIONS AFTER THIS LINE
#=============================


INC += datamod.h
INC += xspress3.h
INC += xspress3_fem_api.h
INC += xspress3_dma_protocol.h
INC += os9types.h
INC += xspress3_data_mod.h

ifeq (linux-x86_64, $(findstring linux-x86_64, $(T_A)))
LIB_INSTALLS_Linux += ../os/linux-x86_64/libxspress3.a
LIB_INSTALLS_Linux += ../os/linux-x86_64/libimg_mod.a
endif

#=============================

include $(TOP)/configure/RULES
#----------------------------------------
#  ADD RULES AFTER THIS LINE

