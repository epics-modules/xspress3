Python Interpreter
===================

Example HDF5 Reading
--------------------

The below is a basic python interpreter for the HDF5 file which creates a simple CSV from the frame data of a basic scan. By changing the file name in the function call to a completed scan you can get data from your file.

This is not optimised and is intended to show how many of the parameters can be accessed from the file. 

.. code:: python

	# -*- coding: utf-8 -*-
	"""
	Created on Fri Dec 16 15:25:30 2022

	@author: Tom
	"""

	import h5py


	def headerDict (chan):
	    timePV = "CHAN{}SCA0".format(chan)
	    DTPV = "CHAN{}DTPercent".format(chan)
	    ICRPV = "CHAN{}SCA3".format(chan)
	    OCRPV = "CHAN{}SCA4".format(chan)
	    chanDict = {
	            "time" : timePV,
	            "DTpercent":DTPV,
	            "ICR":ICRPV,
	            "OCR":OCRPV
	            }
	    return chanDict

	def headerWrite (df,paramData, param, numChan, frame):
	    df.write("%s," % param)
	    for chan in range(1,numChan):
	        df.write("%.2f" % (paramData[channels[chan][param]][frame]))
	        if (chan==numChan-1):
	            df.write("\n")
	        else:
	            df.write(",")
	            

	def getData(data, chan, frame):
	    header = {
	            'chan':chan,
	            'frame':frame
	            }
	    scaler = data['data']['data'][frame][chan]
	    params={}
	    NDAttr = headerDict(chan)
	    for attr in NDAttr:
	        val=NDAttr[attr]
	        params[attr]=(data['instrument']['NDAttributes'][val][frame])
	    allData = {
	            'header':header,
	            'scaler':scaler,
	            'params':params
	                }
	    return allData
	    

	def writeFile(rawdata, frame, channels):
	    header=['']*8
	    MCA=['']*4096
	    for c in range(1,channels):

	        data=getData(rawdata, c, frame)
	        j=0
	        frame = data['header']['frame']
	        chan = data['header']['chan']
	        
	    
	        params = data['params']

	        scaler = data['scaler']
	        
	        header[j]=("Frame, %i\n" % frame)
	        j=j+1
	        if chan == 1:
	            header[j]+=str("Channel,")
	        header[j]+=str("%i" % chan)
	        if chan == channels-1:
	            header[j]+=("\n")
	        else:
	             header[j]+=(",") 
	        j=j+1
	        
	    
	        
	        for param in params:
	            val = params[param]
	            if chan == 1:
	                header[j]=("%s," % param)
	            header[j]+=("%f" % val)
	            if chan == channels-1:
	                header[j]+=("\n")
	            else:
	                header[j]+=(",")
	            j=j+1

	        for i in range(len(scaler)):
	            if chan == 1:
	                MCA[i]=("%d," % (i*10))
	            MCA[i]+=("%d" % scaler[i])
	            if chan == channels-1:
	                MCA[i]+=("\n" % scaler[i])
	            else: 
	                MCA[i]+=(",")

	            
	        df=open("Frame{frame}.csv".format(frame=frame), "+w") 

	        for h in header:
	            df.write(h)
	        for m in MCA:
	            df.write(m)
	    
	        df.close()
	    
	# Gets basic data for hdf5 file
	f = h5py.File("<fileName>.hdf5",'r')

	rawdata = f['entry']
	    

	channels=8
	for frame in range(10):
	    writeFile(rawdata, frame, channels)

