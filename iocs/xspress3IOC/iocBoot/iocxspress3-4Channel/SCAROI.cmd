# SCA, ROI and Other stuff
dbLoadRecords("xspress3_AttrReset.template", "P=$(PREFIX),R=det1:,CHAN=$(CHAN)")

#SCAs
#############

# Create an NDAttribute plugin with 8 attributes
NDAttrConfigure("$(PORT).C$(CHAN)SCA", $(QSIZE), 0, "$(PORT)", 0, 8, 0, 0, 0)
dbLoadRecords("NDAttribute.template",  "P=$(PREFIX),R=C$(CHAN)SCA:,   PORT=$(PORT).C$(CHAN)SCA, ADDR=0,TIMEOUT=1,NCHANS=$(NCHANS),NDARRAY_PORT=$(PORT)")
dbLoadRecords("NDAttributeN.template", "P=$(PREFIX),R=C$(CHAN)SCA1:,  PORT=$(PORT).C$(CHAN)SCA, ADDR=0,TIMEOUT=1,NCHANS=$(NCHANS)")
dbLoadRecords("NDAttributeN.template", "P=$(PREFIX),R=C$(CHAN)SCA2:,  PORT=$(PORT).C$(CHAN)SCA, ADDR=1,TIMEOUT=1,NCHANS=$(NCHANS)")
dbLoadRecords("NDAttributeN.template", "P=$(PREFIX),R=C$(CHAN)SCA3:,  PORT=$(PORT).C$(CHAN)SCA, ADDR=2,TIMEOUT=1,NCHANS=$(NCHANS)")
dbLoadRecords("NDAttributeN.template", "P=$(PREFIX),R=C$(CHAN)SCA4:,  PORT=$(PORT).C$(CHAN)SCA, ADDR=3,TIMEOUT=1,NCHANS=$(NCHANS)")
dbLoadRecords("NDAttributeN.template", "P=$(PREFIX),R=C$(CHAN)SCA5:,  PORT=$(PORT).C$(CHAN)SCA, ADDR=4,TIMEOUT=1,NCHANS=$(NCHANS)")
dbLoadRecords("NDAttributeN.template", "P=$(PREFIX),R=C$(CHAN)SCA6:,  PORT=$(PORT).C$(CHAN)SCA, ADDR=5,TIMEOUT=1,NCHANS=$(NCHANS)")
dbLoadRecords("NDAttributeN.template", "P=$(PREFIX),R=C$(CHAN)SCA7:,  PORT=$(PORT).C$(CHAN)SCA, ADDR=6,TIMEOUT=1,NCHANS=$(NCHANS)")
dbLoadRecords("NDAttributeN.template", "P=$(PREFIX),R=C$(CHAN)SCA8:,  PORT=$(PORT).C$(CHAN)SCA, ADDR=7,TIMEOUT=1,NCHANS=$(NCHANS)")

dbLoadRecords("xspress3ChannelSCAThreshold.template", "P=$(PREFIX),R=C$(CHAN)SCA5:,PORT=$(PORT), ADDR=$(XADDR), TIMEOUT=1, CHAN=$(CHAN), SCA=4")
dbLoadRecords("xspress3ChannelSCALimits.template",    "P=$(PREFIX),R=C$(CHAN)SCA6:,PORT=$(PORT), ADDR=$(XADDR), TIMEOUT=1, CHAN=$(CHAN), SCA=5")
dbLoadRecords("xspress3ChannelSCALimits.template",    "P=$(PREFIX),R=C$(CHAN)SCA7:,PORT=$(PORT), ADDR=$(XADDR), TIMEOUT=1, CHAN=$(CHAN), SCA=6")
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
#### Mn May-4-2016:

# Take full 2D array and turn it into single spectra
NDROIConfigure("CHAN$(CHAN)", $(QSIZE), 0, "XSP3", 0, -1, -1)
dbLoadRecords("NDROI.template", ,"P=$(PREFIX), R=ROI$(CHAN):, PORT=CHAN$(CHAN), TIMEOUT=1, ADDR=0, NDARRAY_PORT=XSP3, NDARRAY_ADDR=0, Enabled=0")

NDROIConfigure("CHANSUM$(CHAN)", $(QSIZE), 0, "PROC1", 0, -1, -1)
dbLoadRecords("NDROI.template", ,"P=$(PREFIX), R=ROISUM$(CHAN):, PORT=CHANSUM$(CHAN), TIMEOUT=1, ADDR=0, NDARRAY_PORT=PROC1, NDARRAY_ADDR=0, Enabled=0")

# Create StdArray for Visualization 
NDStdArraysConfigure("MCA$(CHAN)", 5, 0, "CHAN$(CHAN)", 0, 0)
dbLoadRecords("$(ADCORE)/db/NDStdArrays.template", "P=$(PREFIX),R=MCA$(CHAN):,PORT=MCA$(CHAN),ADDR=0,TIMEOUT=1,NDARRAY_PORT=CHAN$(CHAN),TYPE=Float64,FTVL=DOUBLE,NELEMENTS=$(XSIZE)")

NDStdArraysConfigure("MCASUM$(CHAN)", 5, 0, "CHANSUM$(CHAN)", 0, 0)
dbLoadRecords("$(ADCORE)/db/NDStdArrays.template", "P=$(PREFIX),R=MCASUM$(CHAN):,PORT=MCASUM$(CHAN),ADDR=0,TIMEOUT=1,NDARRAY_PORT=CHANSUM$(CHAN),TYPE=Float64,FTVL=DOUBLE,NELEMENTS=$(XSIZE)")


# New ROI Stats 
NDROIStatConfigure("ROISTAT$(CHAN)", $(QSIZE), 0, "CHAN$(CHAN)", 0, 32, 0, 0)
dbLoadRecords("NDROIStat.template",   "P=$(PREFIX),R=MCA$(CHAN)ROI: ,PORT=ROISTAT$(CHAN),ADDR=0,TIMEOUT=1, NDARRAY_PORT=CHAN$(CHAN),NCHANS=$(NCHANS)")
dbLoadRecords("NDROIStatN.template",  "P=$(PREFIX),R=MCA$(CHAN)ROI:1:,PORT=ROISTAT$(CHAN),ADDR=0,TIMEOUT=1,NCHANS=$(NCHANS)")
dbLoadRecords("NDROIStatN.template",  "P=$(PREFIX),R=MCA$(CHAN)ROI:2:,PORT=ROISTAT$(CHAN),ADDR=1,TIMEOUT=1,NCHANS=$(NCHANS)")
dbLoadRecords("NDROIStatN.template",  "P=$(PREFIX),R=MCA$(CHAN)ROI:3:,PORT=ROISTAT$(CHAN),ADDR=2,TIMEOUT=1,NCHANS=$(NCHANS)")
dbLoadRecords("NDROIStatN.template",  "P=$(PREFIX),R=MCA$(CHAN)ROI:4:,PORT=ROISTAT$(CHAN),ADDR=3,TIMEOUT=1,NCHANS=$(NCHANS)")
dbLoadRecords("NDROIStatN.template",  "P=$(PREFIX),R=MCA$(CHAN)ROI:5:,PORT=ROISTAT$(CHAN),ADDR=4,TIMEOUT=1,NCHANS=$(NCHANS)")
dbLoadRecords("NDROIStatN.template",  "P=$(PREFIX),R=MCA$(CHAN)ROI:6:,PORT=ROISTAT$(CHAN),ADDR=5,TIMEOUT=1,NCHANS=$(NCHANS)")
dbLoadRecords("NDROIStatN.template",  "P=$(PREFIX),R=MCA$(CHAN)ROI:7:,PORT=ROISTAT$(CHAN),ADDR=6,TIMEOUT=1,NCHANS=$(NCHANS)")
dbLoadRecords("NDROIStatN.template",  "P=$(PREFIX),R=MCA$(CHAN)ROI:8:,PORT=ROISTAT$(CHAN),ADDR=7,TIMEOUT=1,NCHANS=$(NCHANS)")
dbLoadRecords("NDROIStatN.template",  "P=$(PREFIX),R=MCA$(CHAN)ROI:9:,PORT=ROISTAT$(CHAN),ADDR=8,TIMEOUT=1,NCHANS=$(NCHANS)")
dbLoadRecords("NDROIStatN.template",  "P=$(PREFIX),R=MCA$(CHAN)ROI:10:,PORT=ROISTAT$(CHAN),ADDR=9,TIMEOUT=1,NCHANS=$(NCHANS)")
dbLoadRecords("NDROIStatN.template",  "P=$(PREFIX),R=MCA$(CHAN)ROI:11:,PORT=ROISTAT$(CHAN),ADDR=10,TIMEOUT=1,NCHANS=$(NCHANS)")
dbLoadRecords("NDROIStatN.template",  "P=$(PREFIX),R=MCA$(CHAN)ROI:12:,PORT=ROISTAT$(CHAN),ADDR=11,TIMEOUT=1,NCHANS=$(NCHANS)")
dbLoadRecords("NDROIStatN.template",  "P=$(PREFIX),R=MCA$(CHAN)ROI:13:,PORT=ROISTAT$(CHAN),ADDR=12,TIMEOUT=1,NCHANS=$(NCHANS)")
dbLoadRecords("NDROIStatN.template",  "P=$(PREFIX),R=MCA$(CHAN)ROI:14:,PORT=ROISTAT$(CHAN),ADDR=13,TIMEOUT=1,NCHANS=$(NCHANS)")
dbLoadRecords("NDROIStatN.template",  "P=$(PREFIX),R=MCA$(CHAN)ROI:15:,PORT=ROISTAT$(CHAN),ADDR=14,TIMEOUT=1,NCHANS=$(NCHANS)")
dbLoadRecords("NDROIStatN.template",  "P=$(PREFIX),R=MCA$(CHAN)ROI:16:,PORT=ROISTAT$(CHAN),ADDR=15,TIMEOUT=1,NCHANS=$(NCHANS)")
dbLoadRecords("NDROIStatN.template",  "P=$(PREFIX),R=MCA$(CHAN)ROI:17:,PORT=ROISTAT$(CHAN),ADDR=16,TIMEOUT=1,NCHANS=$(NCHANS)")
dbLoadRecords("NDROIStatN.template",  "P=$(PREFIX),R=MCA$(CHAN)ROI:18:,PORT=ROISTAT$(CHAN),ADDR=17,TIMEOUT=1,NCHANS=$(NCHANS)")
dbLoadRecords("NDROIStatN.template",  "P=$(PREFIX),R=MCA$(CHAN)ROI:19:,PORT=ROISTAT$(CHAN),ADDR=18,TIMEOUT=1,NCHANS=$(NCHANS)")
dbLoadRecords("NDROIStatN.template",  "P=$(PREFIX),R=MCA$(CHAN)ROI:20:,PORT=ROISTAT$(CHAN),ADDR=19,TIMEOUT=1,NCHANS=$(NCHANS)")
dbLoadRecords("NDROIStatN.template",  "P=$(PREFIX),R=MCA$(CHAN)ROI:21:,PORT=ROISTAT$(CHAN),ADDR=20,TIMEOUT=1,NCHANS=$(NCHANS)")
dbLoadRecords("NDROIStatN.template",  "P=$(PREFIX),R=MCA$(CHAN)ROI:22:,PORT=ROISTAT$(CHAN),ADDR=21,TIMEOUT=1,NCHANS=$(NCHANS)")
dbLoadRecords("NDROIStatN.template",  "P=$(PREFIX),R=MCA$(CHAN)ROI:23:,PORT=ROISTAT$(CHAN),ADDR=22,TIMEOUT=1,NCHANS=$(NCHANS)")
dbLoadRecords("NDROIStatN.template",  "P=$(PREFIX),R=MCA$(CHAN)ROI:24:,PORT=ROISTAT$(CHAN),ADDR=23,TIMEOUT=1,NCHANS=$(NCHANS)")
dbLoadRecords("NDROIStatN.template",  "P=$(PREFIX),R=MCA$(CHAN)ROI:25:,PORT=ROISTAT$(CHAN),ADDR=24,TIMEOUT=1,NCHANS=$(NCHANS)")
dbLoadRecords("NDROIStatN.template",  "P=$(PREFIX),R=MCA$(CHAN)ROI:26:,PORT=ROISTAT$(CHAN),ADDR=25,TIMEOUT=1,NCHANS=$(NCHANS)")
dbLoadRecords("NDROIStatN.template",  "P=$(PREFIX),R=MCA$(CHAN)ROI:27:,PORT=ROISTAT$(CHAN),ADDR=26,TIMEOUT=1,NCHANS=$(NCHANS)")
dbLoadRecords("NDROIStatN.template",  "P=$(PREFIX),R=MCA$(CHAN)ROI:28:,PORT=ROISTAT$(CHAN),ADDR=27,TIMEOUT=1,NCHANS=$(NCHANS)")
dbLoadRecords("NDROIStatN.template",  "P=$(PREFIX),R=MCA$(CHAN)ROI:29:,PORT=ROISTAT$(CHAN),ADDR=28,TIMEOUT=1,NCHANS=$(NCHANS)")
dbLoadRecords("NDROIStatN.template",  "P=$(PREFIX),R=MCA$(CHAN)ROI:30:,PORT=ROISTAT$(CHAN),ADDR=29,TIMEOUT=1,NCHANS=$(NCHANS)")
dbLoadRecords("NDROIStatN.template",  "P=$(PREFIX),R=MCA$(CHAN)ROI:31:,PORT=ROISTAT$(CHAN),ADDR=30,TIMEOUT=1,NCHANS=$(NCHANS)")
dbLoadRecords("NDROIStatN.template",  "P=$(PREFIX),R=MCA$(CHAN)ROI:32:,PORT=ROISTAT$(CHAN),ADDR=31,TIMEOUT=1,NCHANS=$(NCHANS)")




