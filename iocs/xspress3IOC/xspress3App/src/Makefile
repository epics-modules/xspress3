TOP=../..
include $(TOP)/configure/CONFIG
#----------------------------------------
#  ADD MACRO DEFINITIONS AFTER THIS LINE

PROD_NAME = xspress3App
ifeq (linux-x86_64, $(findstring linux-x86_64, $(T_A)))
  PROD_IOC_Linux  += $(PROD_NAME)
endif

# <name>.dbd will be created from <name>Include.dbd
DBD += $(PROD_NAME).dbd
$(PROD_NAME)_DBD += xspress3Support.dbd

# <name>_registerRecordDeviceDriver.cpp will be created from <name>.dbd
$(PROD_NAME)_SRCS += $(PROD_NAME)_registerRecordDeviceDriver.cpp $(PROD_NAME)Main.cpp

#PROD_LIBS += spectraPlugins
PROD_LIBS += xspress3Epics
PROD_LIBS += xspress3
PROD_LIBS += img_mod
PROD_SYS_LIBS += pthread rt

include $(ADCORE)/ADApp/commonDriverMakefile

#=============================

include $(TOP)/configure/RULES
#----------------------------------------
#  ADD RULES AFTER THIS LINE

