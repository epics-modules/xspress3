#############
# Setup IOC with default values
#############

#Enable Array Callbacks, set Attributes file
dbpf("$(PREFIX)det1:ArrayCallbacks","Enable")
dbpf("$(PREFIX)det1:NDAttributesFile", "XSP3.xml")

#Configure HDF Plugin
dbpf("$(PREFIX)HDF1:FileTemplate", "%s%s%d.hdf5")
dbpf("$(PREFIX)HDF1:FileWriteMode", "Stream")
dbpf("$(PREFIX)HDF1:EnableCallbacks", "Enable")
dbpf("$(PREFIX)HDF1:Compression", "zlib")
dbpf("$(PREFIX)HDF1:ZLevel", "1")
dbpf("$(PREFIX)HDF1:AutoIncrement", "Yes")

#Configure PROC plugin spectra summing
dbpf("$(PREFIX)Proc1:EnableFilter", "Enable")
dbpf("$(PREFIX)Proc1:FilterType", "Sum")
dbpf("$(PREFIX)Proc1:EnableCallbacks", "Enable")

# Configure Xspress3:
# The "CONFIG_PATH" value points to the calibration folder with files:
#     card0_clocks.dat, chan1_reion0.dat, etc
# dbpf("$(PREFIX)det1:CONFIG_PATH", "/etc/xspress3/calibration/initial")
dbpf("$(PREFIX)det1:CONFIG_PATH", "$(SUPPORT)/../xspress3_settings/current/")

dbpf("$(PREFIX)det1:RUN_FLAGS","0")
dbpf("$(PREFIX)det1:NUM_CHANNELS","7")
dbpf("$(PREFIX)det1:NumImages", 16000)
dbpf("$(PREFIX)det1:AcquireTime", 0.25)
dbpf("$(PREFIX)det1:CONNECT","1")
dbpf("$(PREFIX)det1:CTRL_DTC",  "Disable")
dbpf("$(PREFIX)det1:TriggerMode","Internal")
dbpf("$(PREFIX)det1:EraseOnStart","No")

#Setup Channel 1
epicsEnvSet("CHAN",   "1")
<defaultValuesChan.cmd

dbpf("$(PREFIX)ROI$(CHAN):MinY",    "0")
dbpf("$(PREFIX)ROISUM$(CHAN):MinY", "0")

#Setup Channel 2
epicsEnvSet("CHAN",   "2")
<defaultValuesChan.cmd

dbpf("$(PREFIX)ROI$(CHAN):MinY",    "1")
dbpf("$(PREFIX)ROISUM$(CHAN):MinY", "1")

#Setup Channel 3
epicsEnvSet("CHAN",   "3")
<defaultValuesChan.cmd

dbpf("$(PREFIX)ROI$(CHAN):MinY",    "2")
dbpf("$(PREFIX)ROISUM$(CHAN):MinY", "2")

#Setup Channel 4
epicsEnvSet("CHAN",   "4")
<defaultValuesChan.cmd

dbpf("$(PREFIX)ROI$(CHAN):MinY",    "3")
dbpf("$(PREFIX)ROISUM$(CHAN):MinY", "3")


#Setup Channel 5
epicsEnvSet("CHAN",   "5")
<defaultValuesChan.cmd

dbpf("$(PREFIX)ROI$(CHAN):MinY",    "4")
dbpf("$(PREFIX)ROISUM$(CHAN):MinY", "4")



#Setup Channel 6
epicsEnvSet("CHAN",   "6")
<defaultValuesChan.cmd

dbpf("$(PREFIX)ROI$(CHAN):MinY",    "5")
dbpf("$(PREFIX)ROISUM$(CHAN):MinY", "5")



#Setup Channel 7
epicsEnvSet("CHAN",   "7")
<defaultValuesChan.cmd

dbpf("$(PREFIX)ROI$(CHAN):MinY",    "6")
dbpf("$(PREFIX)ROISUM$(CHAN):MinY", "6")

