TOP=../..
include $(TOP)/configure/CONFIG
#----------------------------------------
#  ADD MACRO DEFINITIONS AFTER THIS LINE

#----------------------------------------------------
# In a Diamond Support Module, install
# templates like this
#
DB += xspress3.template
DB += xspress3Channel.template
DB += xspress3ChannelSCALimits.template
DB += xspress3ChannelSCAThreshold.template
DB += xspress3_highlevel.template
DB += xspress3_NDROIStatN.template
DB += xspress3_updateFlag.template
DB += xspress3_AvailableFrame.template

include $(TOP)/configure/RULES
#----------------------------------------
#  ADD RULES AFTER THIS LINE

install:
	cp $(TOP)/xspress3App/Db/create_template.sh $(TOP)/db
