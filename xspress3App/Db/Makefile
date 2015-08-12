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
DB += xspress3ChannelMCAROI.template
DB += xspress3_highlevel.template
DB += xspress3_AttrReset.template
DB += xspress3_AttrUpdate.template
DB += xspress3_pluginControl.template
DB += xspress3_header.template
 
 include $(TOP)/configure/RULES
 #----------------------------------------
 #  ADD RULES AFTER THIS LINE
-
-install:
-	cp $(TOP)/xspress3App/Db/create_template.sh $(TOP)/db
