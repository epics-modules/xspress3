TOP=../..

include $(TOP)/configure/CONFIG

# ------------------------
# Build an IOC Application
# ------------------------

PROD_IOC_Linux = example

# example.dbd will be installed into <top>/dbd
DBD += example.dbd

# example.dbd will be created from these files
example_DBD += xspress3.dbd
example_DBD += xspress3Support.dbd
example_DBD += asSupport.dbd
example_DBD += devIocStats.dbd
example_DBD += calcSupport.dbd
example_DBD += busyRecord.dbd
example_DBD += busySupport.dbd

# example_registerRecordDeviceDriver.cpp will be created
# example.dbd
example_SRCS += example_registerRecordDeviceDriver.cpp

# These two lines are needed for non-vxWorks builds, such as Linux
example_SRCS_DEFAULT += exampleMain.cpp
example_SRCS_vxWorks += -nil-

# The following adds object code from base/src/vxWorks
example_OBJS_vxWorks += $(EPICS_BASE_BIN)/vxComLibrary

# This line says that this IOC Application depends on the
# xxx Support Module
example_LIBS += xspress3Epics
example_LIBS += asyn
example_LIBS += ADBase
example_LIBS += NDPlugin
example_LIBS += calc
example_LIBS += autosave
example_LIBS += devIocStats
example_LIBS += busy

# We need to link this IOC Application against the EPICS Base libraries
example_LIBS += $(EPICS_BASE_IOC_LIBS)

DATA += exampleGUI

include $(TOP)/configure/RULES
