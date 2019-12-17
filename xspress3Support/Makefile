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
#LIB_INSTALLS_Linux += ../os/$(T_A)/libFemClient.a
LIB_INSTALLS_Linux += ../os/linux-x86_64/libxspress3.a
LIB_INSTALLS_Linux += ../os/linux-x86_64/libimg_mod.a
#LIB_INSTALLS_Linux += ../os/linux-x86_64)/libXspress3FemApi.a

#BIN_INSTALLS_Linux += ../os/linux-x86_64/imgd
#BIN_INSTALLS_Linux += ../os/linux-x86_64)/xspress3test
#BIN_INSTALLS_Linux += ../os/linux-x86_64)/xspress3.server
endif

#=============================

include $(TOP)/configure/RULES
#----------------------------------------
#  ADD RULES AFTER THIS LINE

