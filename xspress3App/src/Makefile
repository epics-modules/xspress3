TOP=../..

include $(TOP)/configure/CONFIG

# -------------------------------
# Build an Diamond Support Module
# -------------------------------

DATA+=xsp3.xml

LIBRARY_IOC_Linux += xspress3Epics

DBD += xspress3Support.dbd

#xspress3_DBD += base.dbd
#xspress3_DBD += asyn.dbd
#xspress3_DBD += ADSupport.dbd
#xspress3_DBD += aCalcoutRecord.dbd
#DIRS += plugins

# The following are compiled and added to the support library
xspress3Epics_SRCS += xspress3Epics.cpp
xspress3Epics_SRCS += xsp3Api.cpp
xspress3Epics_SRCS += xsp3Detector.cpp
xspress3Epics_SRCS += xsp3Simulator.cpp
xspress3Epics_SRCS += xsp3SimElement.cpp
xspress3Epics_SRCS += xsp3TimeRegister.cpp

# We need to link against the EPICS Base libraries
#xspress3Epics_LIBS += $(EPICS_BASE_IOC_LIBS)



include $(ADCORE)/ADApp/commonLibraryMakefile

include $(TOP)/configure/RULES
