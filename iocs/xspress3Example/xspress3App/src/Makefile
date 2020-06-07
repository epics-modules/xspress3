TOP=../..

include $(TOP)/configure/CONFIG

# ------------------------
# Build an IOC Application
# ------------------------

PROD_IOC_Linux = xspress3App

# xspress3App.dbd will be installed into <top>/dbd
DBD += xspress3App.dbd

# xspress3App.dbd will be created from these files
xspress3App_DBD += xspress3App.dbd
xspress3App_DBD += xspress3Support.dbd
xspress3App_DBD += asSupport.dbd
xspress3App_DBD += devIocStats.dbd
xspress3App_DBD += calcSupport.dbd
xspress3App_DBD += busyRecord.dbd
xspress3App_DBD += busySupport.dbd
xspress3App_DBD += ADSupport.dbd
xspress3App_DBD += NDPluginSupport.dbd
xspress3App_DBD += NDPluginAttribute.dbd

#Needed for HDF5 plugin
xspress3App_DBD += NDFileHDF5.dbd

# xspress3App_registerRecordDeviceDriver.cpp will be created
# xspress3App.dbd
xspress3App_SRCS += xspress3App_registerRecordDeviceDriver.cpp

# These two lines are needed for non-vxWorks builds, such as Linux
xspress3App_SRCS_DEFAULT += xspress3AppMain.cpp

USR_LDFLAGS += -L$(BOOST)/lib/ -Wl,-rpath,$(BOOST)/lib

# This line says that this IOC Application depends on the
# xxx Support Module
xspress3App_LIBS += xspress3Epics

SYS_PROD_LIBS += xspress3
SYS_PROD_LIBS += img_mod
SYS_PROD_LIBS += rt

xspress3App_LIBS += asyn
xspress3App_LIBS += ADBase
xspress3App_LIBS += NDPlugin
xspress3App_LIBS += calc
xspress3App_LIBS += autosave
xspress3App_LIBS += devIocStats
xspress3App_LIBS += busy
xspress3App_LIBS += spectraPlugins

#Needed for HDF5 plugin
xspress3App_LIBS += NeXus
xspress3App_LIBS += hdf5
xspress3App_LIBS += sz
xspress3App_LIBS += netCDF
SYS_PROD_LIBS += png12
SYS_PROD_LIBS += pthread

# We need to link this IOC Application against the EPICS Base libraries
xspress3App_LIBS += $(EPICS_BASE_IOC_LIBS)

include $(AREA_DETECTOR)/ADApp/commonDriverMakefile

SCRIPTS += xspress3-medm.sh xspress3-edm.sh

SYS_BOOT_FLAGS+=-MXSPRESS3=$(abspath $(XSPRESS3))
SYS_BOOT_FLAGS+=-MXSPRESS3_PREFIX=$(XSPRESS3_PREFIX)

include $(TOP)/configure/RULES

%.sh: ../%.sh
	$(MSI) $(SYS_BOOT_FLAGS) $< | sed 's/\\\$$/$$/g' > $@

