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

ifeq (Linux, $(OS_CLASS))
ifeq (x86_64, $(ARCH_CLASS))
#LIB_INSTALLS_Linux += ../os/$(T_A)/libFemClient.a
LIB_INSTALLS_Linux += ../os/linux-x86_64/libxspress3.so.1.0
LIB_INSTALLS_Linux += ../os/linux-x86_64/libimg_mod.so.1.0
LIB_INSTALLS_Linux += ../os/linux-x86_64/libxspress3.so
LIB_INSTALLS_Linux += ../os/linux-x86_64/libimg_mod.so

#LIB_INSTALLS_Linux += ../os/linux-x86_64)/libXspress3FemApi.a

#BIN_INSTALLS_Linux += ../os/linux-x86_64/imgd
#BIN_INSTALLS_Linux += ../os/linux-x86_64)/xspress3test
#BIN_INSTALLS_Linux += ../os/linux-x86_64)/xspress3.server
endif
endif

#=============================

include $(TOP)/configure/RULES
#----------------------------------------
#  ADD RULES AFTER THIS LINE

