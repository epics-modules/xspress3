TOP = ../../..
include $(TOP)/configure/CONFIG

DATA += $(patsubst ../%, %, $(wildcard ../*.adl))
DATA += $(patsubst ../%, %, $(wildcard ../*.gif))
DATA += $(patsubst ../%, %, $(wildcard ../*.png))
#DATA += <name>.edl for "edl" files created with velocity scripts

include $(TOP)/configure/RULES
