.. _Channel Access: https://epics.anl.gov/base/R3-14/12-docs/CAref.html

Useful PVs
============

There are many configurable variables that can be used during the scan, the most common ones are described below.

The system will be configured with a prefix and a suffix this is set up in the :ref:`st.cmd` file.

The prefix by default will be of the form "XSP3_NCHAN". The suffix will be of the form "det1". These both can be changed during configuration.

The process variables are of the for (P):(R):<Process Variable> and can be controlled with `Channel Access`_ (CA) commands.

Most of the PVs will have a readback value which you can 'get' without influencing the value these will have the same name with '_RBV' affixed to the end. 


Scanning
----------

Start
^^^^^^
(P):(R):Acquire
	Starts the scan by setting it to 1 and stop by setting to 0.

(P):(R):DetectorState_RBV
	Contains information about the acquisition state of the system. Idle, Acquire, Aborted.	

Example:

.. code:: bash

	caput XSP3_8Chan:det1:Acquire 1
	caget XSP3_8Chan:det1:DetectorState_RBV
	caput XSP3_8Chan:det1:Acquire 0
	caget XSP3_8Chan:det1:DetectorState_RBV 

This code will start the scan, the readback value would then inform us that the scan has started by being in the state 'acquire'. Then the scanning is stopped, before the number of frames specified is reached therefore the state would be read back as 'aborted'. If the system has completed the acquisition then it would be in the state 'idle'

Number of frames
^^^^^^^^^^^^^^^^^

(P):(R):NumImages
	Specifies the number of frames to acquire for this acquisition. Integer values between 1 and MAXFRAMES.

Example:

.. code:: bash

	caput XSP3_8Chan:det1:NumImages 1000
	
Sets the number of frames to be 1000

Frame Time
^^^^^^^^^^^

(P):(R):AcquireTime
	Sets the frame time if using the internal time frame generator. This is used in certain :ref:`couting modes <Counting Modes for the Xspress 3>`. The time is a floating number in seconds for the desired frame time. 

Example:

.. code:: bash

	caput XSP3_8Chan:det1:AcquireTime 0.05
	
Sets the dwell time of the frame to be 50 ms.

File saving
^^^^^^^^^^^^

.. note::
	Before file saving can be turned on an acquisition will need to be performed to allow the system to know the size of the histograms.

(P):HDF1:FilePath
	Location that the file will be saved to, this should be provided as a absolute path and has to exist as it will not be created by the system.

.. code:: bash

	caput -S XSP3_8Chan:HDF1:FilePath /home/xspress3/data/
	
Sets the file saving location to be in the folder '/home/xspress3/data'

.. note::
	You are required to have the '-S' option to put the string as a an array of characters so it can be inpepreted by the CA commands 

(P):HDF1:FileName
	File name prefeix for the saving of files. Files will be numbered after this prefix.


.. code:: bash

	caput -S XSP3_8Chan:HDF1:FileName DemoScan

.. note::
	You are required to have the '-S' option to put the string as a an array of characters so it can be inpepreted by the CA commands 

(P):HDF1:FileNumber
	Starting number to be suffix onto the specified file name. Integer.

.. code:: bash

	caput XSP3_8Chan:HDF1:FileNumber 0

Starts the numbering for the file from 0. When a new file is set to capture this will increment automatically if ``XSP3_8Chan:HDF1:AutoIncrement`` is set to 1. If you do not want this behaviour you can set this auto increment to 0 however this will overwrite your file if you start a new capture.

(P):HDF1:Capture
	Variable to control the file saving. If set to 1 then the next acquisition will be saved as specified by the file parameters. If set to 0 then the acquisition will not be saved

.. code:: bash

	caput XSP3_8Chan:HDF1:Capture 1

Configures the system to save the next acquisition.



Configuration
--------------

Some PVs are usful to set before you start any scans these are usually set to help with integration with the wider setup of your capturing system.

Trigger Mode
^^^^^^^^^^^^^

Depending on the :ref:`trigger methods <Counting Modes for the Xspress 3>` you will need to set the corresponding mode to correctly advance your frame. 

(P):(R):TriggerMode
	PV to select the mode required for your frame advancing. Ecamples are shown below for the common methods.

.. code:: bash

	caput XSP3_8Chan:det1:TriggerMode 1

Internal trigger mode, frame advance will happen automatically after the system is ready once the acquisition is started. The frame time will be specified in the :ref:`frame time <Frame Time>` PV.


.. code:: bash

	caput XSP3_8Chan:det1:TriggerMode 3

TTL Veto Only mode, frame advance comes from TTL signal going high and frame stopped when the signal goes low. When acquisition is started the system will get ready and go into the state 'acquire' it is then ready to recieve hardware triggers.

.. code:: bash

	caput XSP3_8Chan:det1:TriggerMode 0

Software Trigger with variable acquisition time

The frame is advanced by setting ``XSP3_8Chan:det1:SoftTrigger`` to 1 and stopped after this is set to 0.

.. code:: bash

	caput XSP3_8Chan:det1:TriggerMode 7

Software + Internal with acquisition time set by :ref:`frame time <Frame Time>` PV.

The frame is advanced by setting ``XSP3_8Chan:det1:SoftTrigger`` to 1 and stopped after the dwell time is reached. Setting the softTrigger PV to 0 will ready the system to recieve the next trigger.

Invert Veto
^^^^^^^^^^^^
If you are using a :ref:`hardware trigger method<Hardware Triggers>` then you can choose to invert the frame start signal so that it will start when the TTL signal goes low instead of high. This is useful if you do not want to reconfigure your hardware triggering system.

(P):(R):INVERT_VETO
	Set to 1 to invert the Veto, 0 to leave as default.


Erase on Start
^^^^^^^^^^^^^^^
When starting a scan one of the steps clears data from a frame so that it is empty ready to start counting the new frame, in some circumstances, this is not required.

(P):(R):EraseOnStart
	Setting EraseOnStart to 0 you will significantly reduce the time taken for the system to be ready to start acquiring frames.

Example:
.. code:: bash
	
	caput XSP3_8Chan:det1:EraseOnStart 0