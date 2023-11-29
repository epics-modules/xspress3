TOP=../..

include $(TOP)/configure/CONFIG

# -------------------------------
# Build an Diamond Support Module
# -------------------------------

DATA+=xsp3.xml

ifeq (Linux, $(OS_CLASS))
ifeq (x86_64, $(ARCH_CLASS))
  LIBRARY_IOC_Linux += xspress3Epics
endif
endif

DBD += xspress3Support.dbd


# The following are compiled and added to the support library
xspress3Epics_SRCS += xspress3Epics.cpp
xspress3Epics_SRCS += xsp3Api.cpp
xspress3Epics_SRCS += xsp3Detector.cpp
xspress3Epics_SRCS += xsp3Simulator.cpp
xspress3Epics_SRCS += xsp3SimElement.cpp
xspress3Epics_SRCS += xsp3TimeRegister.cpp



include $(ADCORE)/ADApp/commonLibraryMakefile

include $(TOP)/configure/RULES
