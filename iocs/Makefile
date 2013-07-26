TOP = ..
include $(TOP)/configure/CONFIG

ifeq ($(EPICS_HOST_ARCH), linux-x86)
#	DIRS := $(wildcard example example_sim xspress3Exampl)
else ifeq ($(EPICS_HOST_ARCH), linux-x86_64)
#	DIRS := $(wildcard example example_sim xspress3Example)
else ifeq ($(EPICS_HOST_ARCH), win32-x86)
	DIRS :=
endif

include $(TOP)/configure/RULES_DIRS
