# Device initialisation
# ---------------------
dbLoadDatabase("$(TOP)/dbd/xspress3App.dbd")

# suppress "callbackRequest: cbLow ring buffer full"
callbackSetQueueSize(8000)

xspress3App_registerRecordDeviceDriver(pdbbase) 

epicsEnvSet("PORT",   "XSP3")
epicsEnvSet("QSIZE",  "128")
epicsEnvSet("XSIZE",  "$(NUM_BINS)")

# The maximum number of frames buffered in the NDPluginCircularBuff plugin
epicsEnvSet("CBUFFS", "4096")

# The search path for database files
epicsEnvSet("EPICS_DB_INCLUDE_PATH", "$(ADCORE)/db:$(ADXSPRESS3)/db:.")

##################################################
# Start Xspress3 driver

# Parameters
# portName: The Asyn port name to use
# numChannels: The max number of channels (eg. 8)
# numCards: The number of Xspress3 systems (normally 1)
# baseIP: The base address used by the Xspress3  1Gig and 10Gig interfaces (eg. "192.168.0.1")
# Max Frames: Used for XSPRESS3 API Configuration   (depends on Xspress3 firmware config).
# Max Driver Frames: Used to limit how big EPICS driver arrays are. Needs to match database MAX_FRAMES_DRIVER_RBV.
# maxSpectra The maximum size of each spectra (eg. 4096)
# maxBuffers Used by asynPortDriver (set to 0 for unlimited)
# maxMemory Used by asynPortDriver (set to 0 for unlimited)
# debug This debug flag is passed to xsp3_config in the Xspress API (0 or 1)
# simTest 0 or 1. Set to 1 to run up this driver in simulation mode. 
xspress3Config("$(PORT)", "$(NUM_CHANNELS)", "$(XSP3CARDS)", "$(XSP3ADDR)", "$(MAXFRAMES)", 16384, "$(NUM_BINS)", 0, 0, 0, 0)

#
# Create a processing plugin

NDProcessConfigure("PROC1", $(QSIZE), 0, "$(PORT)", 0, 0, 0)
dbLoadRecords("NDProcess.template",   "P=$(PREFIX),R=Proc1:,  PORT=PROC1,ADDR=0,TIMEOUT=1,NDARRAY_PORT=$(PORT)")

# Create an HDF5 file saving plugin
NDFileHDF5Configure("FileHDF1", $(QSIZE), 0, "$(PORT)", 0)
dbLoadRecords("NDFileHDF5.template",  "P=$(PREFIX),R=HDF1:,PORT=FileHDF1,ADDR=0,TIMEOUT=1,NDARRAY_PORT=$(PORT)")

# load main template
dbLoadRecords("xspress3.template","P=$(PREFIX),R=det1:,PORT=$(PORT), ADDR=0, TIMEOUT=5, MAX_SPECTRA=$(XSIZE), MAX_FRAMES=$(MAXFRAMES), HDF=$(PREFIX)HDF1:, PROC=$(PREFIX)Proc1:")

