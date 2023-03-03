Configuring your IOC
=========================

st.cmd
-------

The st.cmd file sets the default parameters for the system, this is located in:

.. code:: bash

	/home/xspress3/epics/xspress3/iocs/xspress3IOC/iocBoot/ioc_<N>Channel/st.cmd

There is a default configuration structure for most common number of channels. If there is not a suitable option you can configure your own or contact Quantum Detectors

These are configured at start of IOC and cannot be changed during operation. This file is generally cofigured once for the system and then it is used to launch the IOC.


.. warning:: 
	
	If you change any of the settings below you will need to clear the shared memory before restarting the IOC:	``rm /dev/shm/xsp*``

	



The important configurable values are explained below:

Prefix
^^^^^^^
.. code:: C

	# PREFIX
	epicsEnvSet("PREFIX", "XSP3_<N>Chan:")

Option to provide a prefix for all system PVs. Use a sensible name so that you can recognise the system that is being used. Changing this may be required if you wish to control multiple systems with the same control PC. This then avoids name conflicts.

Number of Channels
^^^^^^^^^^^^^^^^^^^
.. code:: C

	# Number of xspress3 channels
	epicsEnvSet("NUM_CHANNELS",  "<N>")  

The number of channels the IOC will configure for the system. You will require connection to this many channels and settings files for all the channels trying to connect.

.. note::

	You can configure fewer channels than your system has, but not more


Number of Cards
^^^^^^^^^^^^^^^^^
.. code:: C

	# Number of xspress3 cards
	epicsEnvSet("XSP3CARDS", "<M>")

The number of cards the IOC will connect to, for a X3X/M system this value will be the number of channels diveded by 2. For an X3 system it is divided by 8.

IP Address
^^^^^^^^^^^
.. code:: C
	
	# IP ADDR
	epicsEnvSet("XSP3ADDR",  "192.168.0.1")

Default IP address configured for connection to the Xspress 3 system. If required this can be changed. Please contact Quantum Detectors if you need to reconfigure the Xspress boxes.

Circular Buffer
^^^^^^^^^^^^^^^
.. code:: C

	# Control of Circular Buffer
	# Use CIRC_BUFFER as 1 for more than 12216 frames
	epicsEnvSet("CIRC_BUFFER", "1")

Control of circular buffer option, 1 will activate the circular buffer increasing the number of potential frames above 12216. To turn this off select 0.

Circular buffer adds a flag to data that has been read out signifying that it can be overwritten without data loss.

Maximum Frames
^^^^^^^^^^^^^^^

.. code:: C

	# Max Number of Frames for data collection
	epicsEnvSet("MAXFRAMES", "<NumFrames>")
	epicsEnvSet("MAXDRIVERFRAMES", "<NumFrames>")

Number provided to the system determining the maximum number of frames that will be possible in one scan. This value is required to allocate memory for file creation and saving.

.. note:: 

	Tested up to maximum of 500,000.

Settings Configuration
^^^^^^^^^^^^^^^^^^^^^^^

.. code:: C

	#Configure and connect to Xspress3
	dbpf("$(PREFIX)det1:CONFIG_PATH", "/home/xspress3/xspress3_settings/current/")

This is the location that the IOC looks for the settings files, this can be changed if you have a different folder structure.
