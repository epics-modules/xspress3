TOP = ../..
include $(TOP)/configure/CONFIG
ARCH = linux-x86_64
TARGETS = envPaths
TARGETS += stexample.boot
TARGETS += xspress3-ioc.sh
TARGETS += xspress3-medm.sh
TARGETS += xspress3-edm.sh
include $(TOP)/configure/RULES.ioc

SYS_BOOT_FLAGS+=-MXSPRESS3=$(abspath $(XSPRESS3))
SYS_BOOT_FLAGS+=-MXSPRESS3_PREFIX=$(XSPRESS3_PREFIX)
%.sh: %.src
	$(MSI) $(SYS_BOOT_FLAGS) $< | sed 's/\\\$$/$$/g' > $@
