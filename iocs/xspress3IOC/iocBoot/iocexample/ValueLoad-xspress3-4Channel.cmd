#############
# Setup IOC with default values
#############

#Enable Array CallBacks
dbpf("$(PREFIX)det1:ArrayCallbacks","Enable")

#Configure HDF Plugin
dbpf("$(PREFIX)HDF1:FileTemplate", "%s%s%d.hdf5")
dbpf("$(PREFIX)HDF1:FileWriteMode", "Stream")
dbpf("$(PREFIX)HDF1:EnableCallbacks", "Enable")
# hdf5 output

dbpf("$(PREFIX)HDF1:Compression", "zlib")
dbpf("$(PREFIX)HDF1:ZLevel", "1")
dbpf("$(PREFIX)HDF1:AutoIncrement", "Yes")


dbpf("$(PREFIX)ROIDATA:EnableCallbacks","Enable")

dbpf("$(PREFIX)ROIDATA:EnableCallbacks","Enable")

#Set up the PROC plugin spectra summing
dbpf("$(PREFIX)Proc1:EnableFilter", "Enable")
dbpf("$(PREFIX)Proc1:FilterType", "Sum")
dbpf("$(PREFIX)Proc1:EnableCallbacks", "Enable")


# Xspress 3 configuration
dbpf("$(PREFIX)det1:CONFIG_PATH", "/home/xspress3/xspress3_settings/")
dbpf("$(PREFIX)det1:RUN_FLAGS","0")
dbpf("$(PREFIX)det1:NUM_CHANNELS","4")
dbpf("$(PREFIX)det1:NumImages", 1)
dbpf("$(PREFIX)det1:AcquireTime", 1.0)


# connect to Xspress3
dbpf("$(PREFIX)det1:CONNECT","1")
dbpf("$(PREFIX)det1:CTRL_DTC",  "Disable")
dbpf("$(PREFIX)det1:TriggerMode","Internal")

#Setup Channel 1
epicsEnvSet("CHAN",   "1")
<ValuesChan-xspress3-4Channel.cmd

dbpf("$(PREFIX)ROI$(CHAN):MinY",    "0")
dbpf("$(PREFIX)ROISUM$(CHAN):MinY", "0")

#Setup Channel 2
epicsEnvSet("CHAN",   "2")
<ValuesChan-xspress3-4Channel.cmd

dbpf("$(PREFIX)ROI$(CHAN):MinY",    "1")
dbpf("$(PREFIX)ROISUM$(CHAN):MinY", "1")

#Setup Channel 3
epicsEnvSet("CHAN",   "3")
<ValuesChan-xspress3-4Channel.cmd

dbpf("$(PREFIX)ROI$(CHAN):MinY",    "2")
dbpf("$(PREFIX)ROISUM$(CHAN):MinY", "2")

#Setup Channel 4

epicsEnvSet("CHAN",   "4")
<ValuesChan-xspress3-4Channel.cmd

dbpf("$(PREFIX)ROI$(CHAN):MinY",    "3")
dbpf("$(PREFIX)ROISUM$(CHAN):MinY", "3")


dbpf("$(PREFIX)det1:NDAttributesFile", "XSP3-xspress3-4Channel.xml")


