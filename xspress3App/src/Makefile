TOP=../..

include $(TOP)/configure/CONFIG

# -------------------------------
# Build an Diamond Support Module
# -------------------------------

LIBRARY_IOC_Linux += xspress3

# xxxRecord.dbd will be installed into <top>/dbd
DBD += xspress3.dbd
DBD += xspress3Support.dbd

xspress3_DBD += base.dbd
xspress3_DBD += asyn.dbd

# The following are compiled and added to the support library
xspress3_SRCS += xspress3.cpp

# We need to link against the EPICS Base libraries
xspress3_LIBS += asyn
xspress3_LIBS += $(EPICS_BASE_IOC_LIBS)


include $(TOP)/configure/RULES
