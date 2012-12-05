TOP=../..

include $(TOP)/configure/CONFIG

# -------------------------------
# Build an Diamond Support Module
# -------------------------------

DATA+=xsp3.xml

#Provide header file path to Xspress3 API
USR_INCLUDES_Linux += -I$(XSPRESS3_API)/none_vme/xspress3/include/
USR_INCLUDES_Linux += -I$(XSPRESS3_API)/none_vme/xspress3/xspress3FemApi/include/
USR_INCLUDES_Linux += -I$(XSPRESS3_API)/libs/include/

LIBRARY_IOC_Linux += xspress3Epics

# xxxRecord.dbd will be installed into <top>/dbd
DBD += xspress3.dbd
DBD += xspress3Support.dbd

xspress3_DBD += base.dbd
xspress3_DBD += asyn.dbd
xspress3_DBD += ADSupport.dbd
xspress3_DBD += aCalcoutRecord.dbd

# The following are compiled and added to the support library
xspress3Epics_SRCS += xspress3Epics.cpp

# We need to link against the EPICS Base libraries
xspress3Epics_LIBS += $(EPICS_BASE_IOC_LIBS)


include $(TOP)/configure/RULES
