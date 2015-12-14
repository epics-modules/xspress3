TOP=../..
include $(TOP)/configure/CONFIG
#----------------------------------------
#  ADD MACRO DEFINITIONS AFTER THIS LINE
#=============================

# The plugin-test executable with the actual unittests depend on the boost 
# unittest framework so we can only build it if boost has been configured
ifdef BOOST 
  PROD_IOC_Linux += xspress3EpicsTest
  xspress3EpicsTest_SRCS += xspress3EpicsTest.cpp
  xspress3EpicsTest_SRCS += testXspress3.cpp
  USR_INCLUDES += -D__UNIT_TEST__
  USR_INCLUDES += -I../../src
  ifdef BOOST_LIBS
    USR_LDFLAGS += -L$(BOOST_LIBS)
    USR_LDFLAGS += -lboost_unit_test_framework
  else
    xspress3EpicsTest_SYS_LIBS += boost_unit_test_framework
  endif
endif

PROD_LIBS += xspress3Epics xspress3 img_mod ADBase asyn

PROD_LIBS += $(EPICS_BASE_IOC_LIBS)

include $(TOP)/configure/RULES
