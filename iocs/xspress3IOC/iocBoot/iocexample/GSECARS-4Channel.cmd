< envPaths
errlogInit(20000)

# Device initialisation
# ---------------------

dbLoadDatabase("$(TOP)/dbd/xspress3App.dbd")

# MN 27-June-2016
# The default callback queue in EPICS base is only 2000 bytes.
# This should suppress
#    "callbackRequest: cbLow ring buffer full"
# but I'm not really sure why this happens
callbackSetQueueSize(8000)


xspress3App_registerRecordDeviceDriver(pdbbase) 

epicsEnvSet("PREFIX", "13QX4:")
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
epicsEnvSet("EPICS_DB_INCLUDE_PATH", "$(ADCORE)/db:$(ADXSPRESS3)/db:.")


#DevIOCStats
# dbLoadRecords("$(DEVIOCSTATS)/db/iocAdminSoft.db", "IOC=$(IOC)")


##################################################
# Start Xspress3 driver

# Parameters
# portName: The Asyn port name to use
# numChannels: The max number of channels (eg. 8)
# numCards: The number of Xspress3 systems (normally 1)
# baseIP: The base address used by the Xspress3 
#         1Gig and 10Gig interfaces (eg. "192.168.0.1")
# Max Frames: Used for XSPRESS3 API Configuration 
#             (depends on Xspress3 firmware config).
# Max Driver Frames: Used to limit how big EPICS driver 
#            arrays are. Needs to match database MAX_FRAMES_DRIVER_RBV.
# maxSpectra The maximum size of each spectra (eg. 4096)
# maxBuffers Used by asynPortDriver (set to 0 for unlimited)
# maxMemory Used by asynPortDriver (set to 0 for unlimited)
# debug This debug flag is passed to xsp3_config in the Xspress API (0 or 1)
# simTest 0 or 1. Set to 1 to run up this driver in simulation mode. 
xspress3Config("$(PORT)", "$(YSIZE)", "$(XSP3CARDS)", "192.168.0.1", "$(NCHANS)", 16384, "$(XSIZE)", 0, 0, 0, 0)

############

# Create a processing plugin
NDProcessConfigure("PROC1", $(QSIZE), 0, "$(PORT)", 0, 0, 0)
dbLoadRecords("NDProcess.template",   "P=$(PREFIX),R=Proc1:,  PORT=PROC1,ADDR=0,TIMEOUT=1,NDARRAY_PORT=$(PORT)")

# Create an HDF5 file saving plugin
NDFileHDF5Configure("FileHDF1", $(QSIZE), 0, "$(PORT)", 0)
dbLoadRecords("NDFileHDF5.template",  "P=$(PREFIX),R=HDF1:,PORT=FileHDF1,ADDR=0,TIMEOUT=1,NDARRAY_PORT=$(PORT)")

dbLoadRecords("xspress3.template","P=$(PREFIX),R=det1:,PORT=$(PORT), ADDR=0, TIMEOUT=5, MAX_SPECTRA=$(XSIZE), MAX_FRAMES=$(MAXFRAMES), HDF=$(PREFIX)HDF1:, PROC=$(PREFIX)Proc1:")


#########################################
#Channel 1
epicsEnvSet("CHAN",   "1")
epicsEnvSet("XADDR",  "0")
<SCAROI-GSECARS-4Channel.cmd

#########################################
#Channel 2
epicsEnvSet("CHAN",   "2")
epicsEnvSet("XADDR",  "1")
<SCAROI-GSECARS-4Channel.cmd

#########################################
#Channel 3
epicsEnvSet("CHAN",   "3")
epicsEnvSet("XADDR",  "2")
<SCAROI-GSECARS-4Channel.cmd

#########################################
#Channel 4
epicsEnvSet("CHAN",   "4")
epicsEnvSet("XADDR",  "3")
<SCAROI-GSECARS-4Channel.cmd

# Optional: load scan records
# dbLoadRecords("$(SSCAN)/sscanApp/Db/scan.db", "P=$(PREFIX),MAXPTS1=2000,MAXPTS2=200,MAXPTS3=20,MAXPTS4=10,MAXPTSH=10")
# Optional: load sseq record for acquisition sequence
# dbLoadRecords("$(CALC)/calcApp/Db/yySseq.db", "P=$(PREFIX), S=AcquireSequence")

dbLoadRecords("xspress3Deadtime_4Channel.template",   "P=$(PREFIX)")

< AutoSave.cmd
set_pass0_restoreFile("GSECARS-4Channel-settings.sav")
set_pass1_restoreFile("GSECARS-4Channel-settings.sav")

#########################################
# ioc initialization

iocInit

< ValueLoad-GSECARS-4Channel.cmd

# save settings every thirty seconds
create_monitor_set("GSECARS-4Channel-settings.req",30,"P=$(PREFIX)")

epicsThreadSleep(5.)

# Set default event widths for deadtime correction
# note that these should be tuned for each detector:
dbpf("$(PREFIX)C1:EventWidth",    "8")
dbpf("$(PREFIX)C2:EventWidth",    "8")
dbpf("$(PREFIX)C3:EventWidth",    "7")
dbpf("$(PREFIX)C4:EventWidth",    "6")

# Xspress3 is now ready to use!
