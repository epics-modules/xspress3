< envPaths.linux
errlogInit(20000)

# Device initialisation
# ---------------------



epicsEnvSet "EPICS_CA_MAX_ARRAY_BYTES", '6000000'
epicsEnvSet "EPICS_TS_MIN_WEST", '0'


dbLoadDatabase("$(TOP)/dbd/xspress3App.dbd")
xspress3App_registerRecordDeviceDriver(pdbbase) 




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
epicsEnvSet("EPICS_DB_INCLUDE_PATH", "$(ADCORE)/db:$(XSPRESS3)/db:.")


<AutoSave.cmd

epicsEnvSet("IOC","xspress3IOC")


#DevIOCStats
dbLoadRecords("$(DEVIOCSTATS)/db/iocAdminSoft.db", "IOC=$(IOC)")


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

# Create a processing plugin
NDProcessConfigure("PROC1", $(QSIZE), 0, "$(PORT)", 0, 0, 0)
dbLoadRecords("NDProcess.template",   "P=$(PREFIX),R=Proc1:,  PORT=PROC1,ADDR=0,TIMEOUT=1,NDARRAY_PORT=$(PORT)")

# Create an HDF5 file saving plugin
NDFileHDF5Configure("FileHDF1", $(QSIZE), 0, "$(PORT)", 0)
dbLoadRecords("NDFileHDF5.template",  "P=$(PREFIX),R=HDF1:,PORT=FileHDF1,ADDR=0,TIMEOUT=1,NDARRAY_PORT=$(PORT)")

dbLoadRecords("xspress3.template","P=$(PREFIX),R=det1:,PORT=$(PORT), ADDR=0, TIMEOUT=5, MAX_SPECTRA=$(XSIZE), MAX_FRAMES=$(MAXFRAMES), HDF=$(PREFIX)HDF1:, PROC=$(PREFIX)Proc1:")



#########################################
#Channel 1
#########################################
epicsEnvSet("CHAN",   "1")
dbLoadRecords("xspress3_AttrReset.template", "P=$(PREFIX),R=det1:,CHAN=$(CHAN)")

<SCAROI.cmd
dbLoadRecords("xspress3ChannelSCAThreshold.template", "P=$(PREFIX),R=C$(CHAN)SCAs:5:,PORT=$(PORT),ADDR=0,TIMEOUT=1,CHAN=$(CHAN), SCA=4")
dbLoadRecords("xspress3ChannelSCALimits.template", "P=$(PREFIX),R=C$(CHAN)SCAs:6:,PORT=$(PORT), ADDR=0, TIMEOUT=1, CHAN=$(CHAN), SCA=5")
dbLoadRecords("xspress3ChannelSCALimits.template", "P=$(PREFIX),R=C$(CHAN)SCAs:7:,PORT=$(PORT), ADDR=0, TIMEOUT=1, CHAN=$(CHAN), SCA=6")
dbLoadRecords("xspress3ChannelDTC.template", "P=$(PREFIX),R=det1:,PORT=$(PORT),CHAN=$(CHAN), NDARRAY_PORT=$(PORT),ADDR=0,TIMEOUT=5")

#########################################
#Channel 2
#########################################
epicsEnvSet("CHAN",   "2")
dbLoadRecords("xspress3_AttrReset.template", "P=$(PREFIX),R=det1:,CHAN=$(CHAN)")

<SCAROI.cmd
dbLoadRecords("xspress3ChannelSCAThreshold.template", "P=$(PREFIX),R=C$(CHAN)SCAs:5:,PORT=$(PORT),ADDR=1,TIMEOUT=1,CHAN=$(CHAN), SCA=4")
dbLoadRecords("xspress3ChannelSCALimits.template", "P=$(PREFIX),R=C$(CHAN)SCAs:6:,PORT=$(PORT), ADDR=1, TIMEOUT=1, CHAN=$(CHAN), SCA=5")
dbLoadRecords("xspress3ChannelSCALimits.template", "P=$(PREFIX),R=C$(CHAN)SCAs:7:,PORT=$(PORT), ADDR=1, TIMEOUT=1, CHAN=$(CHAN), SCA=6")
dbLoadRecords("xspress3ChannelDTC.template", "P=$(PREFIX),R=det1:,PORT=$(PORT),CHAN=$(CHAN), NDARRAY_PORT=$(PORT),ADDR=1,TIMEOUT=5")

#########################################
#Channel 3
#########################################
epicsEnvSet("CHAN",   "3")
dbLoadRecords("xspress3_AttrReset.template", "P=$(PREFIX),R=det1:,CHAN=$(CHAN)")

<SCAROI.cmd
dbLoadRecords("xspress3ChannelSCAThreshold.template", "P=$(PREFIX),R=C$(CHAN)SCAs:5:,PORT=$(PORT),ADDR=2,TIMEOUT=1,CHAN=$(CHAN), SCA=4")
dbLoadRecords("xspress3ChannelSCALimits.template", "P=$(PREFIX),R=C$(CHAN)SCAs:6:,PORT=$(PORT), ADDR=2, TIMEOUT=1, CHAN=$(CHAN), SCA=5")
dbLoadRecords("xspress3ChannelSCALimits.template", "P=$(PREFIX),R=C$(CHAN)SCAs:7:,PORT=$(PORT), ADDR=2, TIMEOUT=1, CHAN=$(CHAN), SCA=6")
dbLoadRecords("xspress3ChannelDTC.template", "P=$(PREFIX),R=det1:,PORT=$(PORT),CHAN=$(CHAN), NDARRAY_PORT=$(PORT),ADDR=2,TIMEOUT=5")

#########################################
#Channel 4
#########################################
epicsEnvSet("CHAN",   "4")
dbLoadRecords("xspress3_AttrReset.template", "P=$(PREFIX),R=det1:,CHAN=$(CHAN)")

<SCAROI.cmd
dbLoadRecords("xspress3ChannelSCAThreshold.template", "P=$(PREFIX),R=C$(CHAN)SCAs:5:,PORT=$(PORT),ADDR=3,TIMEOUT=1,CHAN=$(CHAN), SCA=4")
dbLoadRecords("xspress3ChannelSCALimits.template", "P=$(PREFIX),R=C$(CHAN)SCAs:6:,PORT=$(PORT), ADDR=3, TIMEOUT=1, CHAN=$(CHAN), SCA=5")
dbLoadRecords("xspress3ChannelSCALimits.template", "P=$(PREFIX),R=C$(CHAN)SCAs:7:,PORT=$(PORT), ADDR=3, TIMEOUT=1, CHAN=$(CHAN), SCA=6")
dbLoadRecords("xspress3ChannelDTC.template", "P=$(PREFIX),R=det1:,PORT=$(PORT),CHAN=$(CHAN), NDARRAY_PORT=$(PORT),ADDR=3,TIMEOUT=5")



# Optional: load scan records
#dbLoadRecords("$(SSCAN)/sscanApp/Db/scan.db", "P=$(PREFIX),MAXPTS1=2000,MAXPTS2=200,MAXPTS3=20,MAXPTS4=10,MAXPTSH=10")

# Optional: load sseq record for acquisition sequence
dbLoadRecords("$(CALC)/calcApp/Db/yySseq.db", "P=$(PREFIX), S=AcquireSequence")



#########################################
# Final ioc initialization
# ------------------------

iocInit

<defaultValueLoad.cmd

# save settings every thirty seconds
create_monitor_set("auto_settings.req",30,"P=$(PREFIX)")


