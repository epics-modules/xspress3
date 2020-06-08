TOP=../..
include $(TOP)/configure/CONFIG
#----------------------------------------
#  ADD MACRO DEFINITIONS AFTER THIS LINE

#======== DETECTOR LIBRARY ==============

# The following gets rid of the -fno-implicit-templates flag on vxWorks, 
# so we get automatic template instantiation.
# This is what we want for miscellaneous/asynPortDriver.cpp
ifeq (vxWorks,$(findstring vxWorks, $(T_A)))
CODE_CXXFLAGS=
endif

ifeq (linux-x86_64, $(findstring linux-x86_64, $(T_A)))
LIBRARY_IOC_Linux += xspress3Epics
xspress3Epics_SRCS += xspress3Epics.cpp
xspress3Epics_SRCS += xsp3Api.cpp
xspress3Epics_SRCS += xsp3Detector.cpp
xspress3Epics_SRCS += xsp3Simulator.cpp
xspress3Epics_SRCS += xsp3SimElement.cpp
xspress3Epics_SRCS += xsp3TimeRegister.cpp
endif

DBD += xspress3Support.dbd

include $(ADCORE)/ADApp/commonLibraryMakefile

#=============================

include $(TOP)/configure/RULES
#----------------------------------------
#  ADD RULES AFTER THIS LINE

