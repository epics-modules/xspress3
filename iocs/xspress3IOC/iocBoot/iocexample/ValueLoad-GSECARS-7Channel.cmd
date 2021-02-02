#############
# Setup IOC with default values
#############

#Enable Array Callbacks, set Attributes file
dbpf("$(PREFIX)det1:ArrayCallbacks","Enable")
dbpf("$(PREFIX)det1:NDAttributesFile", "XSP3-GSECARS-7Channel.xml")

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

#Configure and connect to Xspress3
dbpf("$(PREFIX)det1:CONFIG_PATH", "/home/xspress3/xspress3_settings/current/")
dbpf("$(PREFIX)det1:RUN_FLAGS","0")
dbpf("$(PREFIX)det1:NUM_CHANNELS","7")
dbpf("$(PREFIX)det1:NumImages", 1000)
dbpf("$(PREFIX)det1:AcquireTime", 0.25)
dbpf("$(PREFIX)det1:CONNECT","1")
dbpf("$(PREFIX)det1:CTRL_DTC",  "Disable")
dbpf("$(PREFIX)det1:TriggerMode","Internal")

#Setup Channel 1
epicsEnvSet("CHAN",   "1")
<ValuesChan-GSECARS-7Channel.cmd

dbpf("$(PREFIX)ROI$(CHAN):MinY",    "0")
dbpf("$(PREFIX)ROISUM$(CHAN):MinY", "0")

#Setup Channel 2
epicsEnvSet("CHAN",   "2")
<ValuesChan-GSECARS-7Channel.cmd

dbpf("$(PREFIX)ROI$(CHAN):MinY",    "1")
dbpf("$(PREFIX)ROISUM$(CHAN):MinY", "1")

#Setup Channel 3
epicsEnvSet("CHAN",   "3")
<ValuesChan-GSECARS-7Channel.cmd

dbpf("$(PREFIX)ROI$(CHAN):MinY",    "2")
dbpf("$(PREFIX)ROISUM$(CHAN):MinY", "2")

#Setup Channel 4
epicsEnvSet("CHAN",   "4")
<ValuesChan-GSECARS-7Channel.cmd

dbpf("$(PREFIX)ROI$(CHAN):MinY",    "3")
dbpf("$(PREFIX)ROISUM$(CHAN):MinY", "3")


#Setup Channel 5
epicsEnvSet("CHAN",   "5")
<ValuesChan-GSECARS-7Channel.cmd

dbpf("$(PREFIX)ROI$(CHAN):MinY",    "4")
dbpf("$(PREFIX)ROISUM$(CHAN):MinY", "4")



#Setup Channel 6
epicsEnvSet("CHAN",   "6")
<ValuesChan-GSECARS-7Channel.cmd

dbpf("$(PREFIX)ROI$(CHAN):MinY",    "5")
dbpf("$(PREFIX)ROISUM$(CHAN):MinY", "5")



#Setup Channel 7
epicsEnvSet("CHAN",   "7")
<ValuesChan-GSECARS-7Channel.cmd

dbpf("$(PREFIX)ROI$(CHAN):MinY",    "6")
dbpf("$(PREFIX)ROISUM$(CHAN):MinY", "6")

