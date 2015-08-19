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
dbpf("$(PREFIX)HDF1:AutoIncrement", "Yes")


dbpf("$(PREFIX)ROIDATA:EnableCallbacks","Enable")

#Set up the PROC plugin spectra summing
dbpf("$(PREFIX)Proc1:EnableFilter", "Enable")
dbpf("$(PREFIX)Proc1:FilterType", "Sum")
dbpf("$(PREFIX)Proc1:EnableCallbacks", "Enable")


# Xspress 3 configuration
dbpf("$(PREFIX)det1:CONFIG_PATH", "/home/xspress3/xspress3.4chan.20150717/settings/")
dbpf("$(PREFIX)det1:RUN_FLAGS","0")
dbpf("$(PREFIX)det1:NUM_CHANNELS","4")




# connect to Xspress3
dbpf("$(PREFIX)det1:CONNECT","1")

# set TTL trigger
dbpf("$(PREFIX)det1:TriggerMode","Internal")

#Setup Channel1

dbpf("$(PREFIX)ROI1:EnableX", "Enable")
dbpf("$(PREFIX)ROI1:EnableY", "Enable")
dbpf("$(PREFIX)ROI1:EnableZ", "Disable")
dbpf("$(PREFIX)ROI1:MinY", "0")
dbpf("$(PREFIX)ROI1:SizeY", "1")
dbpf("$(PREFIX)ROI1:EnableCallbacks", "Enable")
dbpf("$(PREFIX)ARR1:EnableCallbacks", "Enable")
dbpf("$(PREFIX)ROIStat1:EnableCallbacks", "Enable")
epicsEnvSet("CHAN",   "1")
<defaultValuesChan.cmd

#Setup Channel2

dbpf("$(PREFIX)ROI2:EnableX", "Enable")
dbpf("$(PREFIX)ROI2:EnableY", "Enable")
dbpf("$(PREFIX)ROI2:EnableZ", "Disable")
dbpf("$(PREFIX)ROI2:MinY", "1")
dbpf("$(PREFIX)ROI2:SizeY", "1")
dbpf("$(PREFIX)ROI2:EnableCallbacks", "Enable")
dbpf("$(PREFIX)ARR2:EnableCallbacks", "Enable")
dbpf("$(PREFIX)ROIStat2:EnableCallbacks", "Enable")
epicsEnvSet("CHAN",   "2")
<defaultValuesChan.cmd

#Setup Channel3

dbpf("$(PREFIX)ROI3:EnableX", "Enable")
dbpf("$(PREFIX)ROI3:EnableY", "Enable")
dbpf("$(PREFIX)ROI3:EnableZ", "Disable")
dbpf("$(PREFIX)ROI3:MinY", "2")
dbpf("$(PREFIX)ROI3:SizeY", "1")
dbpf("$(PREFIX)ROI3:EnableCallbacks", "Enable")
dbpf("$(PREFIX)ARR3:EnableCallbacks", "Enable")
dbpf("$(PREFIX)ROIStat3:EnableCallbacks", "Enable")
epicsEnvSet("CHAN",   "3")
<defaultValuesChan.cmd

#Setup Channel4

dbpf("$(PREFIX)ROI4:EnableX", "Enable")
dbpf("$(PREFIX)ROI4:EnableY", "Enable")
dbpf("$(PREFIX)ROI4:EnableZ", "Disable")
dbpf("$(PREFIX)ROI4:MinY", "3")
dbpf("$(PREFIX)ROI4:SizeY", "1")
dbpf("$(PREFIX)ROI4:EnableCallbacks", "Enable")
dbpf("$(PREFIX)ARR4:EnableCallbacks", "Enable")
dbpf("$(PREFIX)ROIStat4:EnableCallbacks", "Enable")
epicsEnvSet("CHAN",   "4")
<defaultValuesChan.cmd

dbpf("$(PREFIX)det1:NDAttributesFile", "../../xspress3App/data/XSP3.xml")

