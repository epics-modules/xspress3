==============================
 Xspress3 AreaDetector Driver
==============================
An AreaDetector_ driver for the `Xspress3 <http://quantumdetectors.com/xspress3/>`_ solid state detector read-out system from Quantum Detectors.

=====================
 How to build an IOC
=====================
If all you need is a driver to poke NDArrays into your AreaDetector file writer then a substitution file with an instance of the xspress3.template and an iocsh script that calls xspress3Config with the appropriate arguments will be sufficient.

=====================================
 How to build a more complicated IOC
=====================================
iocgen.py is a simple python3 script that will create substitution file and iocsh syntax to run an xspress3 driver, hdf5 file writer and other plugins for visualisation of the MCAs and scalers produced by the xspress3. It can be called, in the simplest manner, like so:
  python3 iocgen.py 4
where 4 is the number of channels in the system. This will simply write to stdout so specifying file names using the appropriate flags is probably a good idea but, you know, copy and paste works too...

.. _AreaDetector: http://cars9.uchicago.edu/software/epics/areaDetector.html
