#!../../bin/linux-x86_64/xspress3App

< envPaths
errlogInit(20000)

# PREFIX
#epicsEnvSet("PREFIX", "XSP3_8Chan:")
epicsEnvSet("PREFIX", "XF:06BM-ES{Xsp:1}:")

# Number of xspress3 channels
epicsEnvSet("NUM_CHANNELS",  "8")            

# Number of xspress3 cards and IP ADDR
# Newer xspress3 units come with multiple dual channel cards 
epicsEnvSet("XSP3CARDS", "4")
epicsEnvSet("XSP3ADDR",  "192.168.0.1")

# Max Number of Frames for data collection
epicsEnvSet("MAXFRAMES", "16384")

# Number of Energy bins
epicsEnvSet("NUM_BINS",  "4096")

< DefineXSP3Driver.cmd

###############################
# DEFINE CHANNELS
#Channel 1
epicsEnvSet("CHAN",   "1")
epicsEnvSet("CHM1",   "0")
< DefineSCAROI.cmd

#Channel 2
epicsEnvSet("CHAN",   "2")
epicsEnvSet("CHM1",   "1")
< DefineSCAROI.cmd

#Channel 3
epicsEnvSet("CHAN",   "3")
epicsEnvSet("CHM1",   "2")
< DefineSCAROI.cmd

#Channel 4
epicsEnvSet("CHAN",   "4")
epicsEnvSet("CHM1",   "3")
< DefineSCAROI.cmd

#Channel 5
epicsEnvSet("CHAN",   "5")
epicsEnvSet("CHM1",   "4")
< DefineSCAROI.cmd

#Channel 6
epicsEnvSet("CHAN",   "6")
epicsEnvSet("CHM1",   "5")
< DefineSCAROI.cmd

#Channel 7
epicsEnvSet("CHAN",   "7")
epicsEnvSet("CHM1",   "6")
< DefineSCAROI.cmd

#Channel 8
epicsEnvSet("CHAN",   "8")
epicsEnvSet("CHM1",   "7")
< DefineSCAROI.cmd

###############################

dbLoadRecords("xspress3Deadtime_8Channel.template",   "P=$(PREFIX)")

< AutoSave.cmd

###############################
# start IOC
iocInit

# setup startup values

#Configure and connect to Xspress3
dbpf("$(PREFIX)det1:CONFIG_PATH", "/home/xspress3/xspress3_settings/current/")

< SetMainValues.cmd
dbpf("$(PREFIX)det1:NDAttributesFile", "XSP3-8Channel.xml")

###############################
# SET UP CHANNELS
#Channel 1
epicsEnvSet("CHAN",   "1")
epicsEnvSet("CHM1",   "0")
< SetChannelValues.cmd

#Channel 2
epicsEnvSet("CHAN",   "2")
epicsEnvSet("CHM1",   "1")
< SetChannelValues.cmd

#Channel 3
epicsEnvSet("CHAN",   "3")
epicsEnvSet("CHM1",   "2")
< SetChannelValues.cmd

#Channel 4
epicsEnvSet("CHAN",   "4")
epicsEnvSet("CHM1",   "3")
< SetChannelValues.cmd

#Channel 5
epicsEnvSet("CHAN",   "5")
epicsEnvSet("CHM1",   "4")
< SetChannelValues.cmd

#Channel 6
epicsEnvSet("CHAN",   "6")
epicsEnvSet("CHM1",   "5")
< SetChannelValues.cmd

#Channel 7
epicsEnvSet("CHAN",   "7")
epicsEnvSet("CHM1",   "6")
< SetChannelValues.cmd

#Channel 8
epicsEnvSet("CHAN",   "8")
epicsEnvSet("CHM1",   "7")
< SetChannelValues.cmd

###############################

# save settings every thirty seconds
create_monitor_set("8Channel_settings.req",30,"P=$(PREFIX)")

epicsThreadSleep(5.)

# Set default event widths for deadtime correction
# note: these may be tuned for each detector:
# dbpf("$(PREFIX)C1:EventWidth",    "6")
# dbpf("$(PREFIX)C2:EventWidth",    "6")
# dbpf("$(PREFIX)C3:EventWidth",    "6")
# dbpf("$(PREFIX)C4:EventWidth",    "6")
# dbpf("$(PREFIX)C5:EventWidth",    "6")
# dbpf("$(PREFIX)C6:EventWidth",    "6")
# dbpf("$(PREFIX)C7:EventWidth",    "6")
# dbpf("$(PREFIX)C8:EventWidth",    "6")

# Xspress3 is now ready to use!
