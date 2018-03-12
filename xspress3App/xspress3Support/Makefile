TOP=../..
include $(TOP)/configure/CONFIG
#----------------------------------------
#  ADD MACRO DEFINITIONS AFTER THIS LINE
#=============================

# Note, the files in this directory were manually copied from the Andor SDK directories 
# for Linux and Windows.  The soft links for Linux were manually created.

# The extension on ATMCD32D.h was changed from .H to .h because the EPICS build system does
# not recognize .H
INC += datamod.h
INC += xspress3.h
INC += xspress3_fem_api.h
INC += xspress3_dma_protocol.h
INC += os9types.h
INC += xspress3_data_mod.h

LIB_INSTALLS_Linux += ../os/$(T_A)/libxspress3.a
LIB_INSTALLS_Linux += ../os/$(T_A)/libimg_mod.a


#=============================

include $(TOP)/configure/RULES
#----------------------------------------
#  ADD RULES AFTER THIS LINE

