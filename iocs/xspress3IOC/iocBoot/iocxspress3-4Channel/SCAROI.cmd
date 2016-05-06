# SCA, ROI and Other stuff
dbLoadRecords("xspress3_AttrReset.template", "P=$(PREFIX),R=det1:,CHAN=$(CHAN)")

#SCAs
#############

# Create an NDAttribute plugin with 8 attributes
NDAttrConfigure("$(PORT).C$(CHAN)SCAs", $(QSIZE), 0, "$(PORT)", 0, 8, 0, 0, 0)
dbLoadRecords("NDAttribute.template",  "P=$(PREFIX),R=C$(CHAN)SCAs:,    PORT=$(PORT).C$(CHAN)SCAs,ADDR=0,TIMEOUT=1,NCHANS=$(NCHANS),NDARRAY_PORT=$(PORT)")
dbLoadRecords("NDAttributeN.template", "P=$(PREFIX),R=C$(CHAN)SCAs:1:,  PORT=$(PORT).C$(CHAN)SCAs,ADDR=0,TIMEOUT=1,NCHANS=$(NCHANS)")
dbLoadRecords("NDAttributeN.template", "P=$(PREFIX),R=C$(CHAN)SCAs:2:,  PORT=$(PORT).C$(CHAN)SCAs,ADDR=1,TIMEOUT=1,NCHANS=$(NCHANS)")
dbLoadRecords("NDAttributeN.template", "P=$(PREFIX),R=C$(CHAN)SCAs:3:,  PORT=$(PORT).C$(CHAN)SCAs,ADDR=2,TIMEOUT=1,NCHANS=$(NCHANS)")
dbLoadRecords("NDAttributeN.template", "P=$(PREFIX),R=C$(CHAN)SCAs:4:,  PORT=$(PORT).C$(CHAN)SCAs,ADDR=3,TIMEOUT=1,NCHANS=$(NCHANS)")
dbLoadRecords("NDAttributeN.template", "P=$(PREFIX),R=C$(CHAN)SCAs:5:,  PORT=$(PORT).C$(CHAN)SCAs,ADDR=4,TIMEOUT=1,NCHANS=$(NCHANS)")
dbLoadRecords("NDAttributeN.template", "P=$(PREFIX),R=C$(CHAN)SCAs:6:,  PORT=$(PORT).C$(CHAN)SCAs,ADDR=5,TIMEOUT=1,NCHANS=$(NCHANS)")
dbLoadRecords("NDAttributeN.template", "P=$(PREFIX),R=C$(CHAN)SCAs:7:,  PORT=$(PORT).C$(CHAN)SCAs,ADDR=6,TIMEOUT=1,NCHANS=$(NCHANS)")
dbLoadRecords("NDAttributeN.template", "P=$(PREFIX),R=C$(CHAN)SCAs:8:,  PORT=$(PORT).C$(CHAN)SCAs,ADDR=7,TIMEOUT=1,NCHANS=$(NCHANS)")

dbLoadRecords("xspress3ChannelSCAThreshold.template", "P=$(PREFIX),R=C$(CHAN)SCAs:5:,PORT=$(PORT), ADDR=$(XADDR), TIMEOUT=1, CHAN=$(CHAN), SCA=4")
dbLoadRecords("xspress3ChannelSCALimits.template",    "P=$(PREFIX),R=C$(CHAN)SCAs:6:,PORT=$(PORT), ADDR=$(XADDR), TIMEOUT=1, CHAN=$(CHAN), SCA=5")
dbLoadRecords("xspress3ChannelSCALimits.template",    "P=$(PREFIX),R=C$(CHAN)SCAs:7:,PORT=$(PORT), ADDR=$(XADDR), TIMEOUT=1, CHAN=$(CHAN), SCA=6")
dbLoadRecords("xspress3ChannelDTC.template",          "P=$(PREFIX),R=det1:, PORT=$(PORT),CHAN=$(CHAN), NDARRAY_PORT=$(PORT),ADDR=$(XADDR),TIMEOUT=5")

#ROIs
###########

#### MN May-4-2016 was:
# # Take full 2D array and turn it into single spectra
# NDROIConfigure("$(PORT).CHAN$(CHAN)", $(QSIZE), 0, "PROC1", 0, -1, -1)
# dbLoadRecords("NDROI.template", ,"P=$(PREFIX), R=ROI$(CHAN):, PORT=$(PORT).CHAN$(CHAN), TIMEOUT=1, ADDR=0, NDARRAY_PORT=PROC1, NDARRAY_ADDR=0, Enabled=0")
#
# # Create StdArray for Visualization 
# NDStdArraysConfigure("$(PORT).ARR$(CHAN)", 5, 0, "$(PORT).CHAN$(CHAN)", 0, 0)
# dbLoadRecords("$(ADCORE)/db/NDStdArrays.template", "P=$(PREFIX),R=ARR$(CHAN):,PORT=$(PORT).ARR$(CHAN),ADDR=0,TIMEOUT=1,NDARRAY_PORT=$(PORT).CHAN$(CHAN),TYPE=Float64,FTVL=DOUBLE,NELEMENTS=$(XSIZE)")
#### Mn May-4-2016 try:

# Take full 2D array and turn it into single spectra
NDROIConfigure("$(PORT).CHAN$(CHAN)", $(QSIZE), 0, "XSP3", 0, -1, -1)
dbLoadRecords("NDROI.template", ,"P=$(PREFIX), R=ROI$(CHAN):, PORT=$(PORT).CHAN$(CHAN), TIMEOUT=1, ADDR=0, NDARRAY_PORT=XSP3, NDARRAY_ADDR=0, Enabled=0")

NDROIConfigure("$(PORT).CHANSUM$(CHAN)", $(QSIZE), 0, "PROC1", 0, -1, -1)
dbLoadRecords("NDROI.template", ,"P=$(PREFIX), R=ROISUM$(CHAN):, PORT=$(PORT).CHANSUM$(CHAN), TIMEOUT=1, ADDR=0, NDARRAY_PORT=PROC1, NDARRAY_ADDR=0, Enabled=0")

# Create StdArray for Visualization 
NDStdArraysConfigure("$(PORT).ARR$(CHAN)", 5, 0, "$(PORT).CHAN$(CHAN)", 0, 0)
dbLoadRecords("$(ADCORE)/db/NDStdArrays.template", "P=$(PREFIX),R=ARR$(CHAN):,PORT=$(PORT).ARR$(CHAN),ADDR=0,TIMEOUT=1,NDARRAY_PORT=$(PORT).CHAN$(CHAN),TYPE=Float64,FTVL=DOUBLE,NELEMENTS=$(XSIZE)")

NDStdArraysConfigure("$(PORT).ARRSUM$(CHAN)", 5, 0, "$(PORT).CHANSUM$(CHAN)", 0, 0)
dbLoadRecords("$(ADCORE)/db/NDStdArrays.template", "P=$(PREFIX),R=ARRSUM$(CHAN):,PORT=$(PORT).ARRSUM$(CHAN),ADDR=0,TIMEOUT=1,NDARRAY_PORT=$(PORT).CHANSUM$(CHAN),TYPE=Float64,FTVL=DOUBLE,NELEMENTS=$(XSIZE)")


# New ROI Stats 
NDROIStatConfigure("ROISTAT$(CHAN)", $(QSIZE), 0, "$(PORT).CHAN$(CHAN)", 0, 32, 0, 0)
dbLoadRecords("NDROIStat.template",   "P=$(PREFIX),R=ROIStat$(CHAN):  ,PORT=ROISTAT$(CHAN),ADDR=0,TIMEOUT=1,NDARRAY_PORT=$(PORT),NCHANS=$(NCHANS)")
dbLoadRecords("NDROIStatN.template",  "P=$(PREFIX),R=ROIStat$(CHAN):1:,PORT=ROISTAT$(CHAN),ADDR=0,TIMEOUT=1,NCHANS=$(NCHANS)")
dbLoadRecords("NDROIStatN.template",  "P=$(PREFIX),R=ROIStat$(CHAN):2:,PORT=ROISTAT$(CHAN),ADDR=1,TIMEOUT=1,NCHANS=$(NCHANS)")
dbLoadRecords("NDROIStatN.template",  "P=$(PREFIX),R=ROIStat$(CHAN):3:,PORT=ROISTAT$(CHAN),ADDR=2,TIMEOUT=1,NCHANS=$(NCHANS)")
dbLoadRecords("NDROIStatN.template",  "P=$(PREFIX),R=ROIStat$(CHAN):4:,PORT=ROISTAT$(CHAN),ADDR=3,TIMEOUT=1,NCHANS=$(NCHANS)")
dbLoadRecords("NDROIStatN.template",  "P=$(PREFIX),R=ROIStat$(CHAN):5:,PORT=ROISTAT$(CHAN),ADDR=4,TIMEOUT=1,NCHANS=$(NCHANS)")
dbLoadRecords("NDROIStatN.template",  "P=$(PREFIX),R=ROIStat$(CHAN):6:,PORT=ROISTAT$(CHAN),ADDR=5,TIMEOUT=1,NCHANS=$(NCHANS)")
dbLoadRecords("NDROIStatN.template",  "P=$(PREFIX),R=ROIStat$(CHAN):7:,PORT=ROISTAT$(CHAN),ADDR=6,TIMEOUT=1,NCHANS=$(NCHANS)")
dbLoadRecords("NDROIStatN.template",  "P=$(PREFIX),R=ROIStat$(CHAN):8:,PORT=ROISTAT$(CHAN),ADDR=7,TIMEOUT=1,NCHANS=$(NCHANS)")
dbLoadRecords("NDROIStatN.template",  "P=$(PREFIX),R=ROIStat$(CHAN):9:,PORT=ROISTAT$(CHAN),ADDR=8,TIMEOUT=1,NCHANS=$(NCHANS)")
dbLoadRecords("NDROIStatN.template",  "P=$(PREFIX),R=ROIStat$(CHAN):10:,PORT=ROISTAT$(CHAN),ADDR=9,TIMEOUT=1,NCHANS=$(NCHANS)")
dbLoadRecords("NDROIStatN.template",  "P=$(PREFIX),R=ROIStat$(CHAN):11:,PORT=ROISTAT$(CHAN),ADDR=10,TIMEOUT=1,NCHANS=$(NCHANS)")
dbLoadRecords("NDROIStatN.template",  "P=$(PREFIX),R=ROIStat$(CHAN):12:,PORT=ROISTAT$(CHAN),ADDR=11,TIMEOUT=1,NCHANS=$(NCHANS)")
dbLoadRecords("NDROIStatN.template",  "P=$(PREFIX),R=ROIStat$(CHAN):13:,PORT=ROISTAT$(CHAN),ADDR=12,TIMEOUT=1,NCHANS=$(NCHANS)")
dbLoadRecords("NDROIStatN.template",  "P=$(PREFIX),R=ROIStat$(CHAN):14:,PORT=ROISTAT$(CHAN),ADDR=13,TIMEOUT=1,NCHANS=$(NCHANS)")
dbLoadRecords("NDROIStatN.template",  "P=$(PREFIX),R=ROIStat$(CHAN):15:,PORT=ROISTAT$(CHAN),ADDR=14,TIMEOUT=1,NCHANS=$(NCHANS)")
dbLoadRecords("NDROIStatN.template",  "P=$(PREFIX),R=ROIStat$(CHAN):16:,PORT=ROISTAT$(CHAN),ADDR=15,TIMEOUT=1,NCHANS=$(NCHANS)")
dbLoadRecords("NDROIStatN.template",  "P=$(PREFIX),R=ROIStat$(CHAN):17:,PORT=ROISTAT$(CHAN),ADDR=16,TIMEOUT=1,NCHANS=$(NCHANS)")
dbLoadRecords("NDROIStatN.template",  "P=$(PREFIX),R=ROIStat$(CHAN):18:,PORT=ROISTAT$(CHAN),ADDR=17,TIMEOUT=1,NCHANS=$(NCHANS)")
dbLoadRecords("NDROIStatN.template",  "P=$(PREFIX),R=ROIStat$(CHAN):19:,PORT=ROISTAT$(CHAN),ADDR=18,TIMEOUT=1,NCHANS=$(NCHANS)")
dbLoadRecords("NDROIStatN.template",  "P=$(PREFIX),R=ROIStat$(CHAN):20:,PORT=ROISTAT$(CHAN),ADDR=19,TIMEOUT=1,NCHANS=$(NCHANS)")
dbLoadRecords("NDROIStatN.template",  "P=$(PREFIX),R=ROIStat$(CHAN):21:,PORT=ROISTAT$(CHAN),ADDR=20,TIMEOUT=1,NCHANS=$(NCHANS)")
dbLoadRecords("NDROIStatN.template",  "P=$(PREFIX),R=ROIStat$(CHAN):22:,PORT=ROISTAT$(CHAN),ADDR=21,TIMEOUT=1,NCHANS=$(NCHANS)")
dbLoadRecords("NDROIStatN.template",  "P=$(PREFIX),R=ROIStat$(CHAN):23:,PORT=ROISTAT$(CHAN),ADDR=22,TIMEOUT=1,NCHANS=$(NCHANS)")
dbLoadRecords("NDROIStatN.template",  "P=$(PREFIX),R=ROIStat$(CHAN):24:,PORT=ROISTAT$(CHAN),ADDR=23,TIMEOUT=1,NCHANS=$(NCHANS)")
dbLoadRecords("NDROIStatN.template",  "P=$(PREFIX),R=ROIStat$(CHAN):25:,PORT=ROISTAT$(CHAN),ADDR=24,TIMEOUT=1,NCHANS=$(NCHANS)")
dbLoadRecords("NDROIStatN.template",  "P=$(PREFIX),R=ROIStat$(CHAN):26:,PORT=ROISTAT$(CHAN),ADDR=25,TIMEOUT=1,NCHANS=$(NCHANS)")
dbLoadRecords("NDROIStatN.template",  "P=$(PREFIX),R=ROIStat$(CHAN):27:,PORT=ROISTAT$(CHAN),ADDR=26,TIMEOUT=1,NCHANS=$(NCHANS)")
dbLoadRecords("NDROIStatN.template",  "P=$(PREFIX),R=ROIStat$(CHAN):28:,PORT=ROISTAT$(CHAN),ADDR=27,TIMEOUT=1,NCHANS=$(NCHANS)")
dbLoadRecords("NDROIStatN.template",  "P=$(PREFIX),R=ROIStat$(CHAN):29:,PORT=ROISTAT$(CHAN),ADDR=28,TIMEOUT=1,NCHANS=$(NCHANS)")
dbLoadRecords("NDROIStatN.template",  "P=$(PREFIX),R=ROIStat$(CHAN):30:,PORT=ROISTAT$(CHAN),ADDR=29,TIMEOUT=1,NCHANS=$(NCHANS)")
dbLoadRecords("NDROIStatN.template",  "P=$(PREFIX),R=ROIStat$(CHAN):31:,PORT=ROISTAT$(CHAN),ADDR=30,TIMEOUT=1,NCHANS=$(NCHANS)")
dbLoadRecords("NDROIStatN.template",  "P=$(PREFIX),R=ROIStat$(CHAN):32:,PORT=ROISTAT$(CHAN),ADDR=31,TIMEOUT=1,NCHANS=$(NCHANS)")




