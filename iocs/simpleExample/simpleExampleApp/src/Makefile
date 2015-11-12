TOP=../..

include $(TOP)/configure/CONFIG
#----------------------------------------
#  ADD MACRO DEFINITIONS AFTER THIS LINE
#=============================

#=============================
# Build the IOC application

PROD_IOC = simpleExample
# simpleExample.dbd will be created and installed
DBD += simpleExample.dbd

# simpleExample.dbd will be made up from these files:
simpleExample_DBD += base.dbd

# Include dbd files from all support applications:
simpleExample_DBD += asyn.dbd
simpleExample_DBD += calcSupport.dbd
simpleExample_DBD += ADSupport.dbd
simpleExample_DBD += aCalcoutRecord.dbd
simpleExample_DBD += xspress3Support.dbd

# Add all the support libraries needed by this IOC
simpleExample_LIBS += asyn
simpleExample_LIBS += calc
simpleExample_LIBS += xspress3Epics
simpleExample_LIBS += xspress3
simpleExample_LIBS += ADBase
simpleExample_LIBS += img_mod

# simpleExample_registerRecordDeviceDriver.cpp derives from simpleExample.dbd
simpleExample_SRCS += simpleExample_registerRecordDeviceDriver.cpp

# Build the main IOC entry point on workstation OSs.
simpleExample_SRCS_DEFAULT += simpleExampleMain.cpp

# Finally link to the EPICS Base libraries
simpleExample_LIBS += $(EPICS_BASE_IOC_LIBS)

#===========================

include $(TOP)/configure/RULES
#----------------------------------------
#  ADD RULES AFTER THIS LINE

