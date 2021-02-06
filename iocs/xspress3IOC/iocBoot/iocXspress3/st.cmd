< envPaths
errlogInit(20000)

dbLoadDatabase "$(TOP)/dbd/xspress3App.dbd"
xspress3App_registerRecordDeviceDriver(pdbbase)

epicsEnvSet("PREFIX", "13XSP3:")
epicsEnvSet("XSP3ADDR", "192.168.0.1")
epicsEnvSet("XSP3CARDS", "2")
epicsEnvSet("XSP3CHANS", "4")
epicsEnvSet("MAXFRAMES", "16384")
epicsEnvSet("NUMBINS", "4096")
epicsEnvSet("QSIZE", "256")
epicsEnvSet("PORT", "XSP3")
epicsEnvSet("EPICS_DB_INCLUDE_PATH", "$(ADCORE)/db:$(ADXSPRESS3)/db")

xspress3Config("$(PORT)", "$(XSP3CHANS)", "$(XSP3CARDS)", "$(XSP3ADDR)", "$(MAXFRAMES)", "$(MAXFRAMES)", "$(NUMBINS)", 0, 0, 0, 0)
dbLoadRecords("xspress3.template", "P=$(PREFIX),R=cam1:,PORT=$(PORT),ADDR=0,TIMEOUT=1")
NDFileHDF5Configure("FileHDF1", $(QSIZE), 0, "$(PORT)", 0)
dbLoadRecords("NDFileHDF5.template", "P=$(PREFIX),R=HDF1:,PORT=FileHDF1,ADDR=0,TIMEOUT=1,XMLSIZE=2048,NDARRAY_PORT=$(PORT)")

set_requestfile_path("./")
set_requestfile_path("$(ADCORE)/ADApp/Db")
set_requestfile_path("$(ADXSPRESS3)/xspress3App/Db")
set_savefile_path("./autosave")
set_pass0_restoreFile("auto_settings.sav")
set_pass1_restoreFile("auto_settings.sav")
save_restoreSet_status_prefix("$(PREFIX)")
dbLoadRecords("$(AUTOSAVE)/asApp/Db/save_restoreStatus.db", "P=$(PREFIX)")

cd "$(TOP)/iocBoot/$(IOC)"
dbLoadTemplate("./xsp3-$(XSP3CHANS)ch.substitutions", "P_=$(PREFIX),PORT_=$(PORT)")
iocshLoad("xsp3-$(XSP3CHANS)ch-batch.cmd", "TASK=preinit")
iocInit()
#create_monitor_set("xsp3-$(XSP3CHANS)ch.req", 30, "P=$(PREFIX)")
iocshLoad("xsp3-$(XSP3CHANS)ch-batch.cmd", "TASK=postinit")
dbpf("$(PREFIX)cam1:CONFIG_PATH", "cfg-$(XSP3CHANS)ch")
dbpf("$(PREFIX)cam1:ArrayCallbacks", 1)
dbpf("$(PREFIX)cam1:CONNECT", 1)

