TOP = ../..
include $(TOP)/configure/CONFIG

SCRIPTS+=stexample.boot
SCRIPTS+=stexample.sh

include $(TOP)/configure/RULES

stexample.sh: ../stexample.sh
	$(MSI) $(SYS_BOOT_FLAGS) $< > $@
