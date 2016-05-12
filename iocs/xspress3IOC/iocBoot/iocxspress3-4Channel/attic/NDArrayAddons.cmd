###############
#Add all of the additional NDArray stuff (SCA, ROI, etc...)
###############

###########
# All of the these should be added to driver init function

# NDAttrConfigure(portName, queueSize, blockingCallbacks, NDArrayPort, NDArrayAddr, maxBuffers, maxMemory, priority, stackSize, maxTimeSeries, attrName)
NDAttrConfigure("$(PORT).C1_SCA0", 4096, 0, "$(PORT)", 0,  -1, -1, 0,0, 4096, "CHAN1SCA0" )

# NDAttrConfigure(portName, queueSize, blockingCallbacks, NDArrayPort, NDArrayAddr, maxBuffers, maxMemory, priority, stackSize, maxTimeSeries, attrName)
NDAttrConfigure("$(PORT).C1_SCA1", 4096, 0, "$(PORT)", 0,  -1, -1, 0,0, 4096, "CHAN1SCA1" )

# NDAttrConfigure(portName, queueSize, blockingCallbacks, NDArrayPort, NDArrayAddr, maxBuffers, maxMemory, priority, stackSize, maxTimeSeries, attrName)
NDAttrConfigure("$(PORT).C1_SCA2", 4096, 0, "$(PORT)", 0,  -1, -1, 0,0, 4096, "CHAN1SCA2" )

# NDAttrConfigure(portName, queueSize, blockingCallbacks, NDArrayPort, NDArrayAddr, maxBuffers, maxMemory, priority, stackSize, maxTimeSeries, attrName)
NDAttrConfigure("$(PORT).C1_SCA3", 4096, 0, "$(PORT)", 0,  -1, -1, 0,0, 4096, "CHAN1SCA3" )

# NDAttrConfigure(portName, queueSize, blockingCallbacks, NDArrayPort, NDArrayAddr, maxBuffers, maxMemory, priority, stackSize, maxTimeSeries, attrName)
NDAttrConfigure("$(PORT).C1_SCA4", 4096, 0, "$(PORT)", 0,  -1, -1, 0,0, 4096, "CHAN1SCA4" )

# NDAttrConfigure(portName, queueSize, blockingCallbacks, NDArrayPort, NDArrayAddr, maxBuffers, maxMemory, priority, stackSize, maxTimeSeries, attrName)
NDAttrConfigure("$(PORT).C1_SCA5", 4096, 0, "$(PORT)", 0,  -1, -1, 0,0, 4096, "CHAN1SCA5" )

# NDAttrConfigure(portName, queueSize, blockingCallbacks, NDArrayPort, NDArrayAddr, maxBuffers, maxMemory, priority, stackSize, maxTimeSeries, attrName)
NDAttrConfigure("$(PORT).C1_SCA6", 4096, 0, "$(PORT)", 0,  -1, -1, 0,0, 4096, "CHAN1SCA6" )

# NDAttrConfigure(portName, queueSize, blockingCallbacks, NDArrayPort, NDArrayAddr, maxBuffers, maxMemory, priority, stackSize, maxTimeSeries, attrName)
NDAttrConfigure("$(PORT).C1_SCA7", 4096, 0, "$(PORT)", 0,  -1, -1, 0,0, 4096, "CHAN1SCA7" )

# NDAttrConfigure(portName, queueSize, blockingCallbacks, NDArrayPort, NDArrayAddr, maxBuffers, maxMemory, priority, stackSize, maxTimeSeries, attrName)
NDAttrConfigure("$(PORT).C1_ROI0", 4096, 0, "$(PORT)", 0,  -1, -1, 0,0, 4096, "CHAN1ROI0" )

# NDAttrConfigure(portName, queueSize, blockingCallbacks, NDArrayPort, NDArrayAddr, maxBuffers, maxMemory, priority, stackSize, maxTimeSeries, attrName)
NDAttrConfigure("$(PORT).C1_ROI1", 4096, 0, "$(PORT)", 0,  -1, -1, 0,0, 4096, "CHAN1ROI1" )

# NDAttrConfigure(portName, queueSize, blockingCallbacks, NDArrayPort, NDArrayAddr, maxBuffers, maxMemory, priority, stackSize, maxTimeSeries, attrName)
NDAttrConfigure("$(PORT).C1_ROI2", 4096, 0, "$(PORT)", 0,  -1, -1, 0,0, 4096, "CHAN1ROI2" )

# NDAttrConfigure(portName, queueSize, blockingCallbacks, NDArrayPort, NDArrayAddr, maxBuffers, maxMemory, priority, stackSize, maxTimeSeries, attrName)
NDAttrConfigure("$(PORT).C1_ROI3", 4096, 0, "$(PORT)", 0,  -1, -1, 0,0, 4096, "CHAN1ROI3" )

# NDAttrConfigure(portName, queueSize, blockingCallbacks, NDArrayPort, NDArrayAddr, maxBuffers, maxMemory, priority, stackSize, maxTimeSeries, attrName)
NDAttrConfigure("$(PORT).C1_ROI4", 4096, 0, "$(PORT)", 0,  -1, -1, 0,0, 4096, "CHAN1ROI4" )

# NDAttrConfigure(portName, queueSize, blockingCallbacks, NDArrayPort, NDArrayAddr, maxBuffers, maxMemory, priority, stackSize, maxTimeSeries, attrName)
NDAttrConfigure("$(PORT).C1_ROI5", 4096, 0, "$(PORT)", 0,  -1, -1, 0,0, 4096, "CHAN1ROI5" )

# NDAttrConfigure(portName, queueSize, blockingCallbacks, NDArrayPort, NDArrayAddr, maxBuffers, maxMemory, priority, stackSize, maxTimeSeries, attrName)
NDAttrConfigure("$(PORT).C1_ROI6", 4096, 0, "$(PORT)", 0,  -1, -1, 0,0, 4096, "CHAN1ROI6" )

# NDAttrConfigure(portName, queueSize, blockingCallbacks, NDArrayPort, NDArrayAddr, maxBuffers, maxMemory, priority, stackSize, maxTimeSeries, attrName)
NDAttrConfigure("$(PORT).C1_ROI7", 4096, 0, "$(PORT)", 0,  -1, -1, 0,0, 4096, "CHAN1ROI7" )

# NDAttrConfigure(portName, queueSize, blockingCallbacks, NDArrayPort, NDArrayAddr, maxBuffers, maxMemory, priority, stackSize, maxTimeSeries, attrName)
NDAttrConfigure("$(PORT).C1_ROI8", 4096, 0, "$(PORT)", 0,  -1, -1, 0,0, 4096, "CHAN1ROI8" )

# NDAttrConfigure(portName, queueSize, blockingCallbacks, NDArrayPort, NDArrayAddr, maxBuffers, maxMemory, priority, stackSize, maxTimeSeries, attrName)
NDAttrConfigure("$(PORT).C1_ROI9", 4096, 0, "$(PORT)", 0,  -1, -1, 0,0, 4096, "CHAN1ROI9" )

# NDAttrConfigure(portName, queueSize, blockingCallbacks, NDArrayPort, NDArrayAddr, maxBuffers, maxMemory, priority, stackSize, maxTimeSeries, attrName)
NDAttrConfigure("$(PORT).C1_ROI10", 4096, 0, "$(PORT)", 0,  -1, -1, 0,0, 4096, "CHAN1ROI10" )

# NDAttrConfigure(portName, queueSize, blockingCallbacks, NDArrayPort, NDArrayAddr, maxBuffers, maxMemory, priority, stackSize, maxTimeSeries, attrName)
NDAttrConfigure("$(PORT).C1_ROI11", 4096, 0, "$(PORT)", 0,  -1, -1, 0,0, 4096, "CHAN1ROI11" )

# NDAttrConfigure(portName, queueSize, blockingCallbacks, NDArrayPort, NDArrayAddr, maxBuffers, maxMemory, priority, stackSize, maxTimeSeries, attrName)
NDAttrConfigure("$(PORT).C1_ROI12", 4096, 0, "$(PORT)", 0,  -1, -1, 0,0, 4096, "CHAN1ROI12" )

# NDAttrConfigure(portName, queueSize, blockingCallbacks, NDArrayPort, NDArrayAddr, maxBuffers, maxMemory, priority, stackSize, maxTimeSeries, attrName)
NDAttrConfigure("$(PORT).C1_ROI13", 4096, 0, "$(PORT)", 0,  -1, -1, 0,0, 4096, "CHAN1ROI13" )

# NDAttrConfigure(portName, queueSize, blockingCallbacks, NDArrayPort, NDArrayAddr, maxBuffers, maxMemory, priority, stackSize, maxTimeSeries, attrName)
NDAttrConfigure("$(PORT).C1_ROI14", 4096, 0, "$(PORT)", 0,  -1, -1, 0,0, 4096, "CHAN1ROI14" )

# NDAttrConfigure(portName, queueSize, blockingCallbacks, NDArrayPort, NDArrayAddr, maxBuffers, maxMemory, priority, stackSize, maxTimeSeries, attrName)
NDAttrConfigure("$(PORT).C1_ROI15", 4096, 0, "$(PORT)", 0,  -1, -1, 0,0, 4096, "CHAN1ROI15" )


# NDROIConfigure(portName, queueSize, blockingCallbacks, NDArrayPort, NDArrayAddr, maxBuffers, maxMemory)
NDROIConfigure("$(PORT).ROI1", 4096, 0, "$(PORT)", 0, -1, -1)

# NDROIConfigure(portName, queueSize, blockingCallbacks, NDArrayPort, NDArrayAddr, maxBuffers, maxMemory)
NDROIConfigure("$(PORT).ROISUM1", 4096, 0, "$(PORT).PROC", 0, -1, -1)

# NDStdArraysConfigure(portName, queueSize, blockingCallbacks, NDArrayPort, NDArrayAddr, maxMemory)
NDStdArraysConfigure("$(PORT).ARR1", 4096, 0, "$(PORT).ROI1", 0, -1)


# NDStdArraysConfigure(portName, queueSize, blockingCallbacks, NDArrayPort, NDArrayAddr, maxMemory)
NDStdArraysConfigure("$(PORT).ARRSUM1", 4096, 0, "$(PORT).ROISUM1", 0, -1)


# NDAttrConfigure(portName, queueSize, blockingCallbacks, NDArrayPort, NDArrayAddr, maxBuffers, maxMemory, priority, stackSize, maxTimeSeries, attrName)
NDAttrConfigure("$(PORT).C2_SCA0", 4096, 0, "$(PORT)", 0,  -1, -1, 0,0, 4096, "CHAN2SCA0" )

# NDAttrConfigure(portName, queueSize, blockingCallbacks, NDArrayPort, NDArrayAddr, maxBuffers, maxMemory, priority, stackSize, maxTimeSeries, attrName)
NDAttrConfigure("$(PORT).C2_SCA1", 4096, 0, "$(PORT)", 0,  -1, -1, 0,0, 4096, "CHAN2SCA1" )

# NDAttrConfigure(portName, queueSize, blockingCallbacks, NDArrayPort, NDArrayAddr, maxBuffers, maxMemory, priority, stackSize, maxTimeSeries, attrName)
NDAttrConfigure("$(PORT).C2_SCA2", 4096, 0, "$(PORT)", 0,  -1, -1, 0,0, 4096, "CHAN2SCA2" )

# NDAttrConfigure(portName, queueSize, blockingCallbacks, NDArrayPort, NDArrayAddr, maxBuffers, maxMemory, priority, stackSize, maxTimeSeries, attrName)
NDAttrConfigure("$(PORT).C2_SCA3", 4096, 0, "$(PORT)", 0,  -1, -1, 0,0, 4096, "CHAN2SCA3" )

# NDAttrConfigure(portName, queueSize, blockingCallbacks, NDArrayPort, NDArrayAddr, maxBuffers, maxMemory, priority, stackSize, maxTimeSeries, attrName)
NDAttrConfigure("$(PORT).C2_SCA4", 4096, 0, "$(PORT)", 0,  -1, -1, 0,0, 4096, "CHAN2SCA4" )

# NDAttrConfigure(portName, queueSize, blockingCallbacks, NDArrayPort, NDArrayAddr, maxBuffers, maxMemory, priority, stackSize, maxTimeSeries, attrName)
NDAttrConfigure("$(PORT).C2_SCA5", 4096, 0, "$(PORT)", 0,  -1, -1, 0,0, 4096, "CHAN2SCA5" )

# NDAttrConfigure(portName, queueSize, blockingCallbacks, NDArrayPort, NDArrayAddr, maxBuffers, maxMemory, priority, stackSize, maxTimeSeries, attrName)
NDAttrConfigure("$(PORT).C2_SCA6", 4096, 0, "$(PORT)", 0,  -1, -1, 0,0, 4096, "CHAN2SCA6" )

# NDAttrConfigure(portName, queueSize, blockingCallbacks, NDArrayPort, NDArrayAddr, maxBuffers, maxMemory, priority, stackSize, maxTimeSeries, attrName)
NDAttrConfigure("$(PORT).C2_SCA7", 4096, 0, "$(PORT)", 0,  -1, -1, 0,0, 4096, "CHAN2SCA7" )

# NDAttrConfigure(portName, queueSize, blockingCallbacks, NDArrayPort, NDArrayAddr, maxBuffers, maxMemory, priority, stackSize, maxTimeSeries, attrName)
NDAttrConfigure("$(PORT).C2_ROI0", 4096, 0, "$(PORT)", 0,  -1, -1, 0,0, 4096, "CHAN2ROI0" )

# NDAttrConfigure(portName, queueSize, blockingCallbacks, NDArrayPort, NDArrayAddr, maxBuffers, maxMemory, priority, stackSize, maxTimeSeries, attrName)
NDAttrConfigure("$(PORT).C2_ROI1", 4096, 0, "$(PORT)", 0,  -1, -1, 0,0, 4096, "CHAN2ROI1" )

# NDAttrConfigure(portName, queueSize, blockingCallbacks, NDArrayPort, NDArrayAddr, maxBuffers, maxMemory, priority, stackSize, maxTimeSeries, attrName)
NDAttrConfigure("$(PORT).C2_ROI2", 4096, 0, "$(PORT)", 0,  -1, -1, 0,0, 4096, "CHAN2ROI2" )

# NDAttrConfigure(portName, queueSize, blockingCallbacks, NDArrayPort, NDArrayAddr, maxBuffers, maxMemory, priority, stackSize, maxTimeSeries, attrName)
NDAttrConfigure("$(PORT).C2_ROI3", 4096, 0, "$(PORT)", 0,  -1, -1, 0,0, 4096, "CHAN2ROI3" )

# NDAttrConfigure(portName, queueSize, blockingCallbacks, NDArrayPort, NDArrayAddr, maxBuffers, maxMemory, priority, stackSize, maxTimeSeries, attrName)
NDAttrConfigure("$(PORT).C2_ROI4", 4096, 0, "$(PORT)", 0,  -1, -1, 0,0, 4096, "CHAN2ROI4" )

# NDAttrConfigure(portName, queueSize, blockingCallbacks, NDArrayPort, NDArrayAddr, maxBuffers, maxMemory, priority, stackSize, maxTimeSeries, attrName)
NDAttrConfigure("$(PORT).C2_ROI5", 4096, 0, "$(PORT)", 0,  -1, -1, 0,0, 4096, "CHAN2ROI5" )

# NDAttrConfigure(portName, queueSize, blockingCallbacks, NDArrayPort, NDArrayAddr, maxBuffers, maxMemory, priority, stackSize, maxTimeSeries, attrName)
NDAttrConfigure("$(PORT).C2_ROI6", 4096, 0, "$(PORT)", 0,  -1, -1, 0,0, 4096, "CHAN2ROI6" )

# NDAttrConfigure(portName, queueSize, blockingCallbacks, NDArrayPort, NDArrayAddr, maxBuffers, maxMemory, priority, stackSize, maxTimeSeries, attrName)
NDAttrConfigure("$(PORT).C2_ROI7", 4096, 0, "$(PORT)", 0,  -1, -1, 0,0, 4096, "CHAN2ROI7" )

# NDAttrConfigure(portName, queueSize, blockingCallbacks, NDArrayPort, NDArrayAddr, maxBuffers, maxMemory, priority, stackSize, maxTimeSeries, attrName)
NDAttrConfigure("$(PORT).C2_ROI8", 4096, 0, "$(PORT)", 0,  -1, -1, 0,0, 4096, "CHAN2ROI8" )

# NDAttrConfigure(portName, queueSize, blockingCallbacks, NDArrayPort, NDArrayAddr, maxBuffers, maxMemory, priority, stackSize, maxTimeSeries, attrName)
NDAttrConfigure("$(PORT).C2_ROI9", 4096, 0, "$(PORT)", 0,  -1, -1, 0,0, 4096, "CHAN2ROI9" )

# NDAttrConfigure(portName, queueSize, blockingCallbacks, NDArrayPort, NDArrayAddr, maxBuffers, maxMemory, priority, stackSize, maxTimeSeries, attrName)
NDAttrConfigure("$(PORT).C2_ROI10", 4096, 0, "$(PORT)", 0,  -1, -1, 0,0, 4096, "CHAN2ROI10" )

# NDAttrConfigure(portName, queueSize, blockingCallbacks, NDArrayPort, NDArrayAddr, maxBuffers, maxMemory, priority, stackSize, maxTimeSeries, attrName)
NDAttrConfigure("$(PORT).C2_ROI11", 4096, 0, "$(PORT)", 0,  -1, -1, 0,0, 4096, "CHAN2ROI11" )

# NDAttrConfigure(portName, queueSize, blockingCallbacks, NDArrayPort, NDArrayAddr, maxBuffers, maxMemory, priority, stackSize, maxTimeSeries, attrName)
NDAttrConfigure("$(PORT).C2_ROI12", 4096, 0, "$(PORT)", 0,  -1, -1, 0,0, 4096, "CHAN2ROI12" )

# NDAttrConfigure(portName, queueSize, blockingCallbacks, NDArrayPort, NDArrayAddr, maxBuffers, maxMemory, priority, stackSize, maxTimeSeries, attrName)
NDAttrConfigure("$(PORT).C2_ROI13", 4096, 0, "$(PORT)", 0,  -1, -1, 0,0, 4096, "CHAN2ROI13" )

# NDAttrConfigure(portName, queueSize, blockingCallbacks, NDArrayPort, NDArrayAddr, maxBuffers, maxMemory, priority, stackSize, maxTimeSeries, attrName)
NDAttrConfigure("$(PORT).C2_ROI14", 4096, 0, "$(PORT)", 0,  -1, -1, 0,0, 4096, "CHAN2ROI14" )

# NDAttrConfigure(portName, queueSize, blockingCallbacks, NDArrayPort, NDArrayAddr, maxBuffers, maxMemory, priority, stackSize, maxTimeSeries, attrName)
NDAttrConfigure("$(PORT).C2_ROI15", 4096, 0, "$(PORT)", 0,  -1, -1, 0,0, 4096, "CHAN2ROI15" )


# NDROIConfigure(portName, queueSize, blockingCallbacks, NDArrayPort, NDArrayAddr, maxBuffers, maxMemory)
NDROIConfigure("$(PORT).ROI2", 4096, 0, "$(PORT)", 0, -1, -1)

# NDROIConfigure(portName, queueSize, blockingCallbacks, NDArrayPort, NDArrayAddr, maxBuffers, maxMemory)
NDROIConfigure("$(PORT).ROISUM2", 4096, 0, "$(PORT).PROC", 0, -1, -1)

# NDStdArraysConfigure(portName, queueSize, blockingCallbacks, NDArrayPort, NDArrayAddr, maxMemory)
NDStdArraysConfigure("$(PORT).ARR2", 4096, 0, "$(PORT).ROI2", 0, -1)

# NDStdArraysConfigure(portName, queueSize, blockingCallbacks, NDArrayPort, NDArrayAddr, maxMemory)
NDStdArraysConfigure("$(PORT).ARRSUM2", 4096, 0, "$(PORT).ROISUM2", 0, -1)

# NDAttrConfigure(portName, queueSize, blockingCallbacks, NDArrayPort, NDArrayAddr, maxBuffers, maxMemory, priority, stackSize, maxTimeSeries, attrName)
NDAttrConfigure("$(PORT).C3_SCA0", 4096, 0, "$(PORT)", 0,  -1, -1, 0,0, 4096, "CHAN3SCA0" )

# NDAttrConfigure(portName, queueSize, blockingCallbacks, NDArrayPort, NDArrayAddr, maxBuffers, maxMemory, priority, stackSize, maxTimeSeries, attrName)
NDAttrConfigure("$(PORT).C3_SCA1", 4096, 0, "$(PORT)", 0,  -1, -1, 0,0, 4096, "CHAN3SCA1" )

# NDAttrConfigure(portName, queueSize, blockingCallbacks, NDArrayPort, NDArrayAddr, maxBuffers, maxMemory, priority, stackSize, maxTimeSeries, attrName)
NDAttrConfigure("$(PORT).C3_SCA2", 4096, 0, "$(PORT)", 0,  -1, -1, 0,0, 4096, "CHAN3SCA2" )

# NDAttrConfigure(portName, queueSize, blockingCallbacks, NDArrayPort, NDArrayAddr, maxBuffers, maxMemory, priority, stackSize, maxTimeSeries, attrName)
NDAttrConfigure("$(PORT).C3_SCA3", 4096, 0, "$(PORT)", 0,  -1, -1, 0,0, 4096, "CHAN3SCA3" )

# NDAttrConfigure(portName, queueSize, blockingCallbacks, NDArrayPort, NDArrayAddr, maxBuffers, maxMemory, priority, stackSize, maxTimeSeries, attrName)
NDAttrConfigure("$(PORT).C3_SCA4", 4096, 0, "$(PORT)", 0,  -1, -1, 0,0, 4096, "CHAN3SCA4" )

# NDAttrConfigure(portName, queueSize, blockingCallbacks, NDArrayPort, NDArrayAddr, maxBuffers, maxMemory, priority, stackSize, maxTimeSeries, attrName)
NDAttrConfigure("$(PORT).C3_SCA5", 4096, 0, "$(PORT)", 0,  -1, -1, 0,0, 4096, "CHAN3SCA5" )

# NDAttrConfigure(portName, queueSize, blockingCallbacks, NDArrayPort, NDArrayAddr, maxBuffers, maxMemory, priority, stackSize, maxTimeSeries, attrName)
NDAttrConfigure("$(PORT).C3_SCA6", 4096, 0, "$(PORT)", 0,  -1, -1, 0,0, 4096, "CHAN3SCA6" )

# NDAttrConfigure(portName, queueSize, blockingCallbacks, NDArrayPort, NDArrayAddr, maxBuffers, maxMemory, priority, stackSize, maxTimeSeries, attrName)
NDAttrConfigure("$(PORT).C3_SCA7", 4096, 0, "$(PORT)", 0,  -1, -1, 0,0, 4096, "CHAN3SCA7" )

# NDAttrConfigure(portName, queueSize, blockingCallbacks, NDArrayPort, NDArrayAddr, maxBuffers, maxMemory, priority, stackSize, maxTimeSeries, attrName)
NDAttrConfigure("$(PORT).C3_ROI0", 4096, 0, "$(PORT)", 0,  -1, -1, 0,0, 4096, "CHAN3ROI0" )

# NDAttrConfigure(portName, queueSize, blockingCallbacks, NDArrayPort, NDArrayAddr, maxBuffers, maxMemory, priority, stackSize, maxTimeSeries, attrName)
NDAttrConfigure("$(PORT).C3_ROI1", 4096, 0, "$(PORT)", 0,  -1, -1, 0,0, 4096, "CHAN3ROI1" )

# NDAttrConfigure(portName, queueSize, blockingCallbacks, NDArrayPort, NDArrayAddr, maxBuffers, maxMemory, priority, stackSize, maxTimeSeries, attrName)
NDAttrConfigure("$(PORT).C3_ROI2", 4096, 0, "$(PORT)", 0,  -1, -1, 0,0, 4096, "CHAN3ROI2" )

# NDAttrConfigure(portName, queueSize, blockingCallbacks, NDArrayPort, NDArrayAddr, maxBuffers, maxMemory, priority, stackSize, maxTimeSeries, attrName)
NDAttrConfigure("$(PORT).C3_ROI3", 4096, 0, "$(PORT)", 0,  -1, -1, 0,0, 4096, "CHAN3ROI3" )

# NDAttrConfigure(portName, queueSize, blockingCallbacks, NDArrayPort, NDArrayAddr, maxBuffers, maxMemory, priority, stackSize, maxTimeSeries, attrName)
NDAttrConfigure("$(PORT).C3_ROI4", 4096, 0, "$(PORT)", 0,  -1, -1, 0,0, 4096, "CHAN3ROI4" )

# NDAttrConfigure(portName, queueSize, blockingCallbacks, NDArrayPort, NDArrayAddr, maxBuffers, maxMemory, priority, stackSize, maxTimeSeries, attrName)
NDAttrConfigure("$(PORT).C3_ROI5", 4096, 0, "$(PORT)", 0,  -1, -1, 0,0, 4096, "CHAN3ROI5" )

# NDAttrConfigure(portName, queueSize, blockingCallbacks, NDArrayPort, NDArrayAddr, maxBuffers, maxMemory, priority, stackSize, maxTimeSeries, attrName)
NDAttrConfigure("$(PORT).C3_ROI6", 4096, 0, "$(PORT)", 0,  -1, -1, 0,0, 4096, "CHAN3ROI6" )

# NDAttrConfigure(portName, queueSize, blockingCallbacks, NDArrayPort, NDArrayAddr, maxBuffers, maxMemory, priority, stackSize, maxTimeSeries, attrName)
NDAttrConfigure("$(PORT).C3_ROI7", 4096, 0, "$(PORT)", 0,  -1, -1, 0,0, 4096, "CHAN3ROI7" )

# NDAttrConfigure(portName, queueSize, blockingCallbacks, NDArrayPort, NDArrayAddr, maxBuffers, maxMemory, priority, stackSize, maxTimeSeries, attrName)
NDAttrConfigure("$(PORT).C3_ROI8", 4096, 0, "$(PORT)", 0,  -1, -1, 0,0, 4096, "CHAN3ROI8" )

# NDAttrConfigure(portName, queueSize, blockingCallbacks, NDArrayPort, NDArrayAddr, maxBuffers, maxMemory, priority, stackSize, maxTimeSeries, attrName)
NDAttrConfigure("$(PORT).C3_ROI9", 4096, 0, "$(PORT)", 0,  -1, -1, 0,0, 4096, "CHAN3ROI9" )

# NDAttrConfigure(portName, queueSize, blockingCallbacks, NDArrayPort, NDArrayAddr, maxBuffers, maxMemory, priority, stackSize, maxTimeSeries, attrName)
NDAttrConfigure("$(PORT).C3_ROI10", 4096, 0, "$(PORT)", 0,  -1, -1, 0,0, 4096, "CHAN3ROI10" )

# NDAttrConfigure(portName, queueSize, blockingCallbacks, NDArrayPort, NDArrayAddr, maxBuffers, maxMemory, priority, stackSize, maxTimeSeries, attrName)
NDAttrConfigure("$(PORT).C3_ROI11", 4096, 0, "$(PORT)", 0,  -1, -1, 0,0, 4096, "CHAN3ROI11" )

# NDAttrConfigure(portName, queueSize, blockingCallbacks, NDArrayPort, NDArrayAddr, maxBuffers, maxMemory, priority, stackSize, maxTimeSeries, attrName)
NDAttrConfigure("$(PORT).C3_ROI12", 4096, 0, "$(PORT)", 0,  -1, -1, 0,0, 4096, "CHAN3ROI12" )

# NDAttrConfigure(portName, queueSize, blockingCallbacks, NDArrayPort, NDArrayAddr, maxBuffers, maxMemory, priority, stackSize, maxTimeSeries, attrName)
NDAttrConfigure("$(PORT).C3_ROI13", 4096, 0, "$(PORT)", 0,  -1, -1, 0,0, 4096, "CHAN3ROI13" )

# NDAttrConfigure(portName, queueSize, blockingCallbacks, NDArrayPort, NDArrayAddr, maxBuffers, maxMemory, priority, stackSize, maxTimeSeries, attrName)
NDAttrConfigure("$(PORT).C3_ROI14", 4096, 0, "$(PORT)", 0,  -1, -1, 0,0, 4096, "CHAN3ROI14" )

# NDAttrConfigure(portName, queueSize, blockingCallbacks, NDArrayPort, NDArrayAddr, maxBuffers, maxMemory, priority, stackSize, maxTimeSeries, attrName)
NDAttrConfigure("$(PORT).C3_ROI15", 4096, 0, "$(PORT)", 0,  -1, -1, 0,0, 4096, "CHAN3ROI15" )

# NDROIConfigure(portName, queueSize, blockingCallbacks, NDArrayPort, NDArrayAddr, maxBuffers, maxMemory)
NDROIConfigure("$(PORT).ROI3", 4096, 0, "$(PORT)", 0, -1, -1)

# NDROIConfigure(portName, queueSize, blockingCallbacks, NDArrayPort, NDArrayAddr, maxBuffers, maxMemory)
NDROIConfigure("$(PORT).ROISUM3", 4096, 0, "$(PORT).PROC", 0, -1, -1)

# NDStdArraysConfigure(portName, queueSize, blockingCallbacks, NDArrayPort, NDArrayAddr, maxMemory)
NDStdArraysConfigure("$(PORT).ARR3", 4096, 0, "$(PORT).ROI3", 0, -1)

# NDStdArraysConfigure(portName, queueSize, blockingCallbacks, NDArrayPort, NDArrayAddr, maxMemory)
NDStdArraysConfigure("$(PORT).ARRSUM3", 4096, 0, "$(PORT).ROISUM3", 0, -1)

# NDAttrConfigure(portName, queueSize, blockingCallbacks, NDArrayPort, NDArrayAddr, maxBuffers, maxMemory, priority, stackSize, maxTimeSeries, attrName)
NDAttrConfigure("$(PORT).C4_SCA0", 4096, 0, "$(PORT)", 0,  -1, -1, 0,0, 4096, "CHAN4SCA0" )

# NDAttrConfigure(portName, queueSize, blockingCallbacks, NDArrayPort, NDArrayAddr, maxBuffers, maxMemory, priority, stackSize, maxTimeSeries, attrName)
NDAttrConfigure("$(PORT).C4_SCA1", 4096, 0, "$(PORT)", 0,  -1, -1, 0,0, 4096, "CHAN4SCA1" )

# NDAttrConfigure(portName, queueSize, blockingCallbacks, NDArrayPort, NDArrayAddr, maxBuffers, maxMemory, priority, stackSize, maxTimeSeries, attrName)
NDAttrConfigure("$(PORT).C4_SCA2", 4096, 0, "$(PORT)", 0,  -1, -1, 0,0, 4096, "CHAN4SCA2" )

# NDAttrConfigure(portName, queueSize, blockingCallbacks, NDArrayPort, NDArrayAddr, maxBuffers, maxMemory, priority, stackSize, maxTimeSeries, attrName)
NDAttrConfigure("$(PORT).C4_SCA3", 4096, 0, "$(PORT)", 0,  -1, -1, 0,0, 4096, "CHAN4SCA3" )

# NDAttrConfigure(portName, queueSize, blockingCallbacks, NDArrayPort, NDArrayAddr, maxBuffers, maxMemory, priority, stackSize, maxTimeSeries, attrName)
NDAttrConfigure("$(PORT).C4_SCA4", 4096, 0, "$(PORT)", 0,  -1, -1, 0,0, 4096, "CHAN4SCA4" )

# NDAttrConfigure(portName, queueSize, blockingCallbacks, NDArrayPort, NDArrayAddr, maxBuffers, maxMemory, priority, stackSize, maxTimeSeries, attrName)
NDAttrConfigure("$(PORT).C4_SCA5", 4096, 0, "$(PORT)", 0,  -1, -1, 0,0, 4096, "CHAN4SCA5" )

# NDAttrConfigure(portName, queueSize, blockingCallbacks, NDArrayPort, NDArrayAddr, maxBuffers, maxMemory, priority, stackSize, maxTimeSeries, attrName)
NDAttrConfigure("$(PORT).C4_SCA6", 4096, 0, "$(PORT)", 0,  -1, -1, 0,0, 4096, "CHAN4SCA6" )

# NDAttrConfigure(portName, queueSize, blockingCallbacks, NDArrayPort, NDArrayAddr, maxBuffers, maxMemory, priority, stackSize, maxTimeSeries, attrName)
NDAttrConfigure("$(PORT).C4_SCA7", 4096, 0, "$(PORT)", 0,  -1, -1, 0,0, 4096, "CHAN4SCA7" )

# NDAttrConfigure(portName, queueSize, blockingCallbacks, NDArrayPort, NDArrayAddr, maxBuffers, maxMemory, priority, stackSize, maxTimeSeries, attrName)
NDAttrConfigure("$(PORT).C4_ROI0", 4096, 0, "$(PORT)", 0,  -1, -1, 0,0, 4096, "CHAN4ROI0" )

# NDAttrConfigure(portName, queueSize, blockingCallbacks, NDArrayPort, NDArrayAddr, maxBuffers, maxMemory, priority, stackSize, maxTimeSeries, attrName)
NDAttrConfigure("$(PORT).C4_ROI1", 4096, 0, "$(PORT)", 0,  -1, -1, 0,0, 4096, "CHAN4ROI1" )

# NDAttrConfigure(portName, queueSize, blockingCallbacks, NDArrayPort, NDArrayAddr, maxBuffers, maxMemory, priority, stackSize, maxTimeSeries, attrName)
NDAttrConfigure("$(PORT).C4_ROI2", 4096, 0, "$(PORT)", 0,  -1, -1, 0,0, 4096, "CHAN4ROI2" )

# NDAttrConfigure(portName, queueSize, blockingCallbacks, NDArrayPort, NDArrayAddr, maxBuffers, maxMemory, priority, stackSize, maxTimeSeries, attrName)
NDAttrConfigure("$(PORT).C4_ROI3", 4096, 0, "$(PORT)", 0,  -1, -1, 0,0, 4096, "CHAN4ROI3" )

# NDAttrConfigure(portName, queueSize, blockingCallbacks, NDArrayPort, NDArrayAddr, maxBuffers, maxMemory, priority, stackSize, maxTimeSeries, attrName)
NDAttrConfigure("$(PORT).C4_ROI4", 4096, 0, "$(PORT)", 0,  -1, -1, 0,0, 4096, "CHAN4ROI4" )

# NDAttrConfigure(portName, queueSize, blockingCallbacks, NDArrayPort, NDArrayAddr, maxBuffers, maxMemory, priority, stackSize, maxTimeSeries, attrName)
NDAttrConfigure("$(PORT).C4_ROI5", 4096, 0, "$(PORT)", 0,  -1, -1, 0,0, 4096, "CHAN4ROI5" )

# NDAttrConfigure(portName, queueSize, blockingCallbacks, NDArrayPort, NDArrayAddr, maxBuffers, maxMemory, priority, stackSize, maxTimeSeries, attrName)
NDAttrConfigure("$(PORT).C4_ROI6", 4096, 0, "$(PORT)", 0,  -1, -1, 0,0, 4096, "CHAN4ROI6" )

# NDAttrConfigure(portName, queueSize, blockingCallbacks, NDArrayPort, NDArrayAddr, maxBuffers, maxMemory, priority, stackSize, maxTimeSeries, attrName)
NDAttrConfigure("$(PORT).C4_ROI7", 4096, 0, "$(PORT)", 0,  -1, -1, 0,0, 4096, "CHAN4ROI7" )

# NDAttrConfigure(portName, queueSize, blockingCallbacks, NDArrayPort, NDArrayAddr, maxBuffers, maxMemory, priority, stackSize, maxTimeSeries, attrName)
NDAttrConfigure("$(PORT).C4_ROI8", 4096, 0, "$(PORT)", 0,  -1, -1, 0,0, 4096, "CHAN4ROI8" )

# NDAttrConfigure(portName, queueSize, blockingCallbacks, NDArrayPort, NDArrayAddr, maxBuffers, maxMemory, priority, stackSize, maxTimeSeries, attrName)
NDAttrConfigure("$(PORT).C4_ROI9", 4096, 0, "$(PORT)", 0,  -1, -1, 0,0, 4096, "CHAN4ROI9" )

# NDAttrConfigure(portName, queueSize, blockingCallbacks, NDArrayPort, NDArrayAddr, maxBuffers, maxMemory, priority, stackSize, maxTimeSeries, attrName)
NDAttrConfigure("$(PORT).C4_ROI10", 4096, 0, "$(PORT)", 0,  -1, -1, 0,0, 4096, "CHAN4ROI10" )

# NDAttrConfigure(portName, queueSize, blockingCallbacks, NDArrayPort, NDArrayAddr, maxBuffers, maxMemory, priority, stackSize, maxTimeSeries, attrName)
NDAttrConfigure("$(PORT).C4_ROI11", 4096, 0, "$(PORT)", 0,  -1, -1, 0,0, 4096, "CHAN4ROI11" )

# NDAttrConfigure(portName, queueSize, blockingCallbacks, NDArrayPort, NDArrayAddr, maxBuffers, maxMemory, priority, stackSize, maxTimeSeries, attrName)
NDAttrConfigure("$(PORT).C4_ROI12", 4096, 0, "$(PORT)", 0,  -1, -1, 0,0, 4096, "CHAN4ROI12" )

# NDAttrConfigure(portName, queueSize, blockingCallbacks, NDArrayPort, NDArrayAddr, maxBuffers, maxMemory, priority, stackSize, maxTimeSeries, attrName)
NDAttrConfigure("$(PORT).C4_ROI13", 4096, 0, "$(PORT)", 0,  -1, -1, 0,0, 4096, "CHAN4ROI13" )

# NDAttrConfigure(portName, queueSize, blockingCallbacks, NDArrayPort, NDArrayAddr, maxBuffers, maxMemory, priority, stackSize, maxTimeSeries, attrName)
NDAttrConfigure("$(PORT).C4_ROI14", 4096, 0, "$(PORT)", 0,  -1, -1, 0,0, 4096, "CHAN4ROI14" )

# NDAttrConfigure(portName, queueSize, blockingCallbacks, NDArrayPort, NDArrayAddr, maxBuffers, maxMemory, priority, stackSize, maxTimeSeries, attrName)
NDAttrConfigure("$(PORT).C4_ROI15", 4096, 0, "$(PORT)", 0,  -1, -1, 0,0, 4096, "CHAN4ROI15" )

# NDAttrConfigure(portName, queueSize, blockingCallbacks, NDArrayPort, NDArrayAddr, maxBuffers, maxMemory, priority, stackSize, maxTimeSeries, attrName)
NDAttrConfigure("$(PORT).C4_ROI16", 4096, 0, "$(PORT)", 0,  -1, -1, 0,0, 4096, "CHAN4ROI16" )

# NDROIConfigure(portName, queueSize, blockingCallbacks, NDArrayPort, NDArrayAddr, maxBuffers, maxMemory)
NDROIConfigure("$(PORT).ROI4", 4096, 0, "$(PORT)", 0, -1, -1)

# NDROIConfigure(portName, queueSize, blockingCallbacks, NDArrayPort, NDArrayAddr, maxBuffers, maxMemory)
NDROIConfigure("$(PORT).ROISUM4", 4096, 0, "$(PORT).PROC", 0, -1, -1)

# NDStdArraysConfigure(portName, queueSize, blockingCallbacks, NDArrayPort, NDArrayAddr, maxMemory)
NDStdArraysConfigure("$(PORT).ARR4", 4096, 0, "$(PORT).ROI4", 0, -1)

# NDStdArraysConfigure(portName, queueSize, blockingCallbacks, NDArrayPort, NDArrayAddr, maxMemory)
NDStdArraysConfigure("$(PORT).ARRSUM4", 4096, 0, "$(PORT).ROISUM4", 0, -1)

# NDProcessConfigure(portName, queueSize, blockingCallbacks, NDArrayPort, NDArrayAddr, maxBuffers, maxMemory)
NDProcessConfigure("$(PORT).PROC", 16, 0, "$(PORT)", 0, 50, 0)

# NDROIConfigure(portName, queueSize, blockingCallbacks, NDArrayPort, NDArrayAddr, maxBuffers, maxMemory)
NDROIConfigure("$(PORT).ROIDATA", 16, 0, "$(PORT)", 0, 50, 0)
