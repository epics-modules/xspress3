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
  ifdef BOOST_LIB
    USE_LDFLAGS += -L$(BOOST_LIB)
    USR_LDFLAGS += -lboost_unit_test_framework
  else
    xspress3EpicsTest_SYS_LIBS += boost_unit_test_framework
  endif
endif

USR_INCLUDES += -I$(BOOST_INCLUDE)
USR_INCLUDES += -I../../src

#boost_unit_test_framework_DIR=$(BOOST_LIB)
#USR_LDFLAGS += -lboost_unit_test_framework

PROD_LIBS += xspress3Epics xspress3 img_mod ADBase asyn

PROD_LIBS += $(EPICS_BASE_IOC_LIBS)

include $(TOP)/configure/RULES
