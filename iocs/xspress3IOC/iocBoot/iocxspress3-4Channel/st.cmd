< envPaths.linux
errlogInit(20000)

# Device initialisation
# ---------------------



epicsEnvSet("DETECTOR", "$(PORT)")

epicsEnvSet "EPICS_CA_MAX_ARRAY_BYTES", '6000000'
epicsEnvSet "EPICS_TS_MIN_WEST", '0'

< /local/config/xrd_config.epics

dbLoadDatabase("$(TOP)/dbd/xspress3App.dbd")
xspress3App_registerRecordDeviceDriver(pdbbase) 


epicsEnvSet("DETECTOR", "xsp3")
epicsEnvSet("PREFIX", "$(SECTOR)$(DETECTOR)$(SUFFIX):")

epicsEnvSet("IOC","ioc$(SECTOR)$(DETECTOR)$(SUFFIX)")

# Alive Record
#dbLoadRecords("$(ALIVE)/aliveApp/Db/alive.db","P=$(PREFIX),RHOST=164.54.100.11")

#DevIOCStats
dbLoadRecords("$(DEVIOCSTATS)/db/iocAdminSoft.db", "IOC=$(SECTOR)$(DETECTOR)$(SUFFIX)")

# caputrecorder
#asSetFilename("$(IOCBOOT)/../accessSecurity.acf")
#dbLoadRecords("$(CAPUTRECORDER)/caputRecorderApp/Db/caputPoster.db","P=$(PREFIX),N=300")


epicsEnvSet("PREFIX", "dp_xsp3_1:")
epicsEnvSet("PORT",   "XSP3")
epicsEnvSet("QSIZE",  "128")

epicsEnvSet("XSIZE",  "4096")
# Number of xspress3 channels
epicsEnvSet("YSIZE",  "4")            

epicsEnvSet("$(PORT)CARDS", "1")

epicsEnvSet("NCHANS", "16384")

epicsEnvSet("MAXFRAMES", "16384")
# The maximum number of frames buffered in the NDPluginCircularBuff plugin
epicsEnvSet("CBUFFS", "500")
# The search path for database files
epicsEnvSet("EPICS_DB_INCLUDE_PATH", "$(ADCORE)/db")

# Loading libraries
# -----------------





callbackSetQueueSize(4000)


##################################################
# Start Xspress3 driver

# Parameters
# portName The Asyn port name to use
# numChannels The max number of channels (eg. 8)
# numCards The number of Xspress3 systems (normally 1)
# baseIP The base address used by the Xspress3 1Gig and 10Gig interfaces (eg. "192.168.0.1")
# Max Frames Used for XSPRESS3 API Configuration (depends on Xspress3 firmware config).
# Max Driver Frames Used to limit how big EPICS driver arrays are). Needs to match database MAX_FRAMES_DRIVER_RBV.
# maxSpectra The maximum size of each spectra (eg. 4096)
# maxBuffers Used by asynPortDriver (set to -1 for unlimited)
# maxMemory Used by asynPortDriver (set to -1 for unlimited)
# debug This debug flag is passed to xsp3_config in the Xspress API (0 or 1)
# simTest 0 or 1. Set to 1 to run up this driver in simulation mode. 
xspress3Config("$(PORT)", "$(YSIZE)", "$(XSP3CARDS)", "192.168.0.1", "$(NCHANS)", 16384, "$(XSIZE)", -1, -1, 0, 1 )




###########
#



dbLoadRecords("$(XSPRESS3)/db/xspress3.template","P=$(PREFIX),R=det1:,PORT=$(PORT), ADDR=0, TIMEOUT=5, MAX_SPECTRA=$(XSIZE), MAX_FRAMES=$(MAXFRAMES)")



<ROIStats.cmd

# Create a processing plugin
NDProcessConfigure("PROC1", $(MAXFRAMES), 0, "$(PORT)", 0, 0, 0)
dbLoadRecords("NDProcess.template",   "P=$(PREFIX),R=Proc1:,  PORT=PROC1,ADDR=0,TIMEOUT=1,NDARRAY_PORT=$(PORT)")

# Create an HDF5 file saving plugin
NDFileHDF5Configure("FileHDF1", $(MAXFRAMES), 0, "$(PORT)", 0)
dbLoadRecords("NDFileHDF5.template",  "P=$(PREFIX),R=HDF1:,PORT=FileHDF1,ADDR=0,TIMEOUT=1,NDARRAY_PORT=$(PORT)")

#SCAs
###########

#dbLoadRecords(  'db/example4Channel_expanded.db', "XSPRESS3_PREFIX=dp_xsp3_1")

#########################################
#Channel 1
#########################################
epicsEnvSet("CHAN",   "1")
#dbLoadRecords("$(XSPRESS3)/db/xspress3_pluginControl.template", "P=$(PREFIX),R=det1:,CHAN=$(CHAN)")
#dbLoadRecords("$(XSPRESS3)/db/xspress3_AttrReset.template", "P=$(PREFIX),R=det1:,CHAN=$(CHAN)")
#dbLoadRecords("$(XSPRESS3)/db/xspress3_AttrUpdate.template", "P=$(PREFIX),R=det1:,CHAN=$(CHAN)")

<SCAROI.cmd
dbLoadRecords("$(XSPRESS3)/db/xspress3ChannelDTC.template", "P=$(PREFIX),R=det1:,PORT=$(PORT),CHAN=$(CHAN), NDARRAY_PORT=$(PORT),ADDR=0,TIMEOUT=5")

#########################################
#Channel 2
#########################################
epicsEnvSet("CHAN",   "2")
#dbLoadRecords("$(XSPRESS3)/db/xspress3_pluginControl.template", "P=$(PREFIX),R=det1:,CHAN=$(CHAN)")
#dbLoadRecords("$(XSPRESS3)/db/xspress3_AttrReset.template", "P=$(PREFIX),R=det1:,CHAN=$(CHAN)")
#dbLoadRecords("$(XSPRESS3)/db/xspress3_AttrUpdate.template", "P=$(PREFIX),R=det1:,CHAN=$(CHAN)")

<SCAROI.cmd
dbLoadRecords("$(XSPRESS3)/db/xspress3ChannelDTC.template", "P=$(PREFIX),R=det1:,PORT=$(PORT),CHAN=$(CHAN), NDARRAY_PORT=$(PORT),ADDR=0,TIMEOUT=5")

#########################################
#Channel 3
#########################################
epicsEnvSet("CHAN",   "3")
#dbLoadRecords("$(XSPRESS3)/db/xspress3_pluginControl.template", "P=$(PREFIX),R=det1:,CHAN=$(CHAN)")
#dbLoadRecords("$(XSPRESS3)/db/xspress3_AttrReset.template", "P=$(PREFIX),R=det1:,CHAN=$(CHAN)")
#dbLoadRecords("$(XSPRESS3)/db/xspress3_AttrUpdate.template", "P=$(PREFIX),R=det1:,CHAN=$(CHAN)")
<SCAROI.cmd
dbLoadRecords("$(XSPRESS3)/db/xspress3ChannelDTC.template", "P=$(PREFIX),R=det1:,PORT=$(PORT),CHAN=$(CHAN), NDARRAY_PORT=$(PORT),ADDR=0,TIMEOUT=5")

#########################################
#Channel 4
#########################################
epicsEnvSet("CHAN",   "4")
#dbLoadRecords("$(XSPRESS3)/db/xspress3_pluginControl.template", "P=$(PREFIX),R=det1:,CHAN=$(CHAN)")
#dbLoadRecords("$(XSPRESS3)/db/xspress3_AttrReset.template", "P=$(PREFIX),R=det1:,CHAN=$(CHAN)")
#dbLoadRecords("$(XSPRESS3)/db/xspress3_AttrUpdate.template", "P=$(PREFIX),R=det1:,CHAN=$(CHAN)")
<SCAROI.cmd
dbLoadRecords("$(XSPRESS3)/db/xspress3ChannelDTC.template", "P=$(PREFIX),R=det1:,PORT=$(PORT),CHAN=$(CHAN), NDARRAY_PORT=$(PORT),ADDR=0,TIMEOUT=5")







dbLoadRecords("$(XSPRESS3)/db/xspress3_highlevel.template","P=$(PREFIX),R=det1:,HDF=$(PREFIX)HDF5:, PROC=$(PREFIX)PROC:")

# Create a standard arrays plugin
NDStdArraysConfigure("Image1", 5, 0, "$(PORT)", 0, 0)

dbLoadRecords("$(ADCORE)/db/NDStdArrays.template", "P=$(PREFIX),R=image1:,PORT=Image1,ADDR=0,TIMEOUT=1,NDARRAY_PORT=$(PORT),TYPE=Float64,FTVL=DOUBLE,NELEMENTS=16384")

#<AutoSave.cmd
#	Beamlilne custom autosave and plugin add-ons
< /local/DPbin/epics/Custom_AD_Plugins/ADPluginAddOns.cmd

#########################################
# Final ioc initialization
# ------------------------

iocInit

<defaultValueLoad.cmd


#	Beamlilne custom autosave and plugin add-ons
< /local/DPbin/epics/Custom_AD_Plugins/ADPluginAddOns-AS.cmd

