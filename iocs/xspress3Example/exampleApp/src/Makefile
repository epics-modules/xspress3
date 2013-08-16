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
example_DBD += ADSupport.dbd
example_DBD += NDPluginSupport.dbd
example_DBD += NDPluginAttribute.dbd

#Needed for HDF5 plugin
example_DBD += NDFileHDF5.dbd

# example_registerRecordDeviceDriver.cpp will be created
# example.dbd
example_SRCS += example_registerRecordDeviceDriver.cpp

# These two lines are needed for non-vxWorks builds, such as Linux
example_SRCS_DEFAULT += exampleMain.cpp

USR_LDFLAGS += -L$(BOOST)/lib/ -Wl,-rpath,$(BOOST)/lib

# This line says that this IOC Application depends on the
# xxx Support Module
example_LIBS += xspress3Epics

SYS_PROD_LIBS += xspress3
SYS_PROD_LIBS += Xspress3FemApi
SYS_PROD_LIBS += FemClient
SYS_PROD_LIBS += boost_thread
SYS_PROD_LIBS += boost_system
SYS_PROD_LIBS += img_mod
SYS_PROD_LIBS += rt

example_LIBS += asyn
example_LIBS += ADBase
example_LIBS += NDPlugin
example_LIBS += calc
example_LIBS += autosave
example_LIBS += devIocStats
example_LIBS += busy
example_LIBS += spectraPlugins

#Needed for HDF5 plugin
example_LIBS += NeXus
example_LIBS += hdf5
example_LIBS += sz
example_LIBS += netCDF
SYS_PROD_LIBS += png12

# We need to link this IOC Application against the EPICS Base libraries
example_LIBS += $(EPICS_BASE_IOC_LIBS)

include $(AREADETECTOR)/ADApp/commonDriverMakefile

SCRIPTS += xspress3-medm.sh xspress3-edm.sh

SYS_BOOT_FLAGS+=-MXSPRESS3=$(abspath $(XSPRESS3))
SYS_BOOT_FLAGS+=-MXSPRESS3_PREFIX=$(XSPRESS3_PREFIX)

include $(TOP)/configure/RULES

%.sh: ../%.sh
	$(MSI) $(SYS_BOOT_FLAGS) $< | sed 's/\\\$$/$$/g' > $@

