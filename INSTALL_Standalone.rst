
The 'build_xspress3.py' script configures and builds a standalone
Epics areaDetector environment that is "almost" ready to run for the
Xspress3 electronics.

Required Packages
===========================

The build_xspress3.py script should run to completion and create an
Xspress3 IOC that is ready to configure and run, but has been tested
only on Centos7 machines.  

Some dependencies that may need to be installed (one time only) with
your package manager:

   re2c  
   rpcgen
   readline, readline-devel
   hdf5, hdf5-devel
   libtiff, libtiff-devel
   libjpeg, libjpeg-turbo-devel
   libxml2, libxml2-devel
   bzip2-devel, bzip2-libs
   GraphicsMagick, GraphicsMagick-devel
   motif, motif-devel (for medm)

On Centos7, you should be able to install these with 

   sudo yum install re2c readline-devel hdf5-devel ...


Build Epics and Xspress3 support
=====================================

The current Xspress3 driver version is 3.2.4.

To build the latest Xspress3 epics drivers, the simplest method is to
use the xspress3_build script (python) in this folder.

Running the 'build_xspress3.py' script will create a *complete* Epics
environment starting with Epics base (7.0.3.1), areaDetector version
3.9, and recent versions of all the other needed Epics libraries
needed to run the Xspress3 epics interface.  To use this script,
create a folder for your epics tree, say /home/xspress3/epics and run
the build script from that folder:

    mkdir /home/xspress3/epics
    cd    /home/xspress3/epics
    wget  https://raw.githubusercontent.com/epics-modules/xspress3/master/build_xspress3.py
    python build_xspress3.py all


This will build all the epics modules, including the Xspress3
application in the current folder. It will also create a 'bin'
directory here that contains scripts to set your environment and run
the Xspress3 IOC:

    bin/bash_profile.sh     bash script to set environmental variables
    bin/procServ            useful utility for long-running tasks like an IOC
    bin/medm                simple, minimal Epics display manager
    bin/run_xspress3.sh     bash script to run an XSPRESS3 IOC 
    bin/start_ioc           python script to run the Xspress3 using procServ
    bin/run_xrfcontrol.py   python script to view and control the Xpsress3
    bin/run_medm.sh         bash script to launch medm for your Xspress3 


When completed and configured for your particular Xspress3 module, you
should be able to run your xspress3 with 

    /home/xspress3/epics/bin/start_ioc xspress3

and then view screens for your detector either using an Epics display
manager like medm or run the XRF Control application with

    ~> /home/xspress3/epics/bin/run_medm.sh
or
    ~> /home/xspress3/epics/bin/run_xrfcontrol.py


Setting up an IOC
====================

After building the Xspress3 application, you will need to configure
and set up an IOC to run with your Xspress3 detector electronics.
There are several example IOCs in

  xspress3/iocs/xspress3IOC/iocBoot/

Depending on the number of detector channels you have, you can probably 
start with one of
  ioc_1Channel/         
  ioc_2Channel/         
  ioc_4Channel/         
  ioc_7Channel/
  ioc_14Channel/        
  ioc_16Channel/        

or 
  iocGSECARS-4Channel/  
  iocGSECARS-7Channel/

For each of these IOC directories, the startup file `st.cmd` is the
main IOC startup file. These define the PREFIX for all the Epics PVs
to be "XSP3_NChan:" with N=1, 2, 4, 7, 14, or 16 as appropriate.  You
may wish to modify this PREFIX.  Other common configurable values can
be changed here as well.

The startup file also defines the folder used to read the xspress3
calibration files.  By default, this is set to 
    /home/xspress3/xspress3_settings/current/

You can change this value, but read the section on configuration and
calibration settings below.

Many of the settings will be set on startup from the command files in 
the xspress3/iocs/xspress3IOC/iocBoot/common folder.

In particular, the EventWidth (see the section on Deadtime below) is
explicitly set at the end of the startup file to be a value of 6.  If
you have carefully calibrated and measured the deadtimes for your
detector channels, you can set these here, or at run-time. 

You can run your IOC with the bin/run_xspress3.sh bash script. This
defines the variable

  IOCNAME=ioc_4Channel

which you may need to change to reflect the IOC and to match you
detector and electronics configuration. 

Once you have verified that this script runs and is connecting to your
Xspress3, you can use the 'start_ioc' script with 

  ~>  /home/xspress3/epics/bin/start_ioc xspress3

to run the IOC with the procServ utility for running, logging, and
reconnecting to long-running processes such as IOCs.

Important Note:
---------------

If you have changed the PREFIX of the IOC from the initially 
installed value, you may need to edit the following scripts to 
have a consistent PREFIX:

   bin/run_xspress3.sh
   bin/run_medm.sh
   bin/run_xrfcontrol.py
   bin/Xspress3.env 


Display Files for MEDM, etc
==================================

Simple display files for medm, caqtdm, and css/boy are placed in the folders
  adls/
  uis/
  opis/

respectively. You should use the files 'xspress3_4chan.*' and
'xspress3_7chan.*' as the starting main screens.  

This build_xspress3.py script does build MEDM, but does not attempt to
build the other display managers.

Once connected, you can run medm from the bin directory (which will be
placed in your path if you source the bin/bash_profile.sh file) to
connect to your Xspress3 by providing the base PREFIX (without a
trailing ':'!!)  for the appropriate display file, as with

  ~> source /home/xspress3/epics/bin/bash_profile.sh
  ~> /home/epics/bin/run_medm.sh


Xspress3 configuration and calibration settings
=====================================================

The Xspress3 iocs need to read Xspress3 setting files as supplied by
Quantum detectors or configured using xspress3-autocalib.py.  By
default (or when you run the calibration procedure yourself), these
files will be put in

 /etc/xspress3/calibration/initial/settings/

or some similar directory under  /etc/xspress3/calibration/

The challenge is that if you change detectors or re-run the calibration, 
these files might be overwritten. 

The iocs here all point to settings files in 

  /home/xspress3/settings/current/

you should make copies of the settings in /etc/xspress3/calibration/
and place the files you want to use in this folder.


Troubleshooting Xspress3 errors
=======================================

If you are changing detectors, and especially the number of detector
elements in use, you may get some cryptic messages about the wrong
number of channels. If so, doing

   rm /dev/shm/xsp*

should remove any stale shared memory files that are used to connect
and transfer data to the xspress3.  These will be rebuilt by the
xspress3 server as needed.


Deadtime Values, Calculations, and EventWidth
=================================================

The Xspress3 driver produces MCA spectra by analyzing each X-ray
event.  Because it does processing of each pulse, there is a finite
dead time that will reduce the output countrate from the true input
countrate.  Knowing the details of these rates, you can correct (at
least partially) for this "deadtime effect".  While the Xspress3 can
report spectra that are corrected, these corrections are sometimes
suspect and so the recommended approach is to collect uncorrected
spectra and the factors need to correct the data in post-processing.

Xspress3 v3 reports a single multiplicative deadtime correction factor
for each detector element that can be used to correct the spectrum
simply by multiplying each spectrum intensity by that value.  This
factor will be 1.0 at very low count rate and increase to 1.5 or even
2.0 for very saturated spectra.  This value is held in Scaler Channel
9, {PREFIX}:C{N}SCA:9:Value_RBV for a PREFIX and Channel N , and the
"percent dead time" will be in Scaler Channel 10,
{PREFIX}:C{N}SCA:10:Value_RBV.  As will all the scalers, these values
will be saved for each frame in mapping mode, and can be collected as
a TimeSeries with any of the spectral ROIs.

The Xspress3 digitizes the signal from each detector using a 12.5 nsec
time bin (clock tick). It detects events and measures the intensity of
each.  Occassionally, it will also reset its charge (using 1 clock
tick). Each event has a finite time-width called the "EventWidth"
which will include the rise time for the charge collection and the
initial detection of the event.  Thus for any spectrum that counted for 
ClockTicks real time, the "Dead Time" will be

     DeadTime = N_Resets + N_AllEvents * EventWidth

where N_Resets is the number of reset events in the time of that
spectrum, N_AllEvents is the number of events processed (the "input
counts") and Eventwidth is the time to process each event.

The "live time" will be ClockTicks - DeadTime, and the deadtime correction
factor will be
 
     DT_Factor = ClockTicks / (ClockTicks - N_Resets - N_AllEvents*EventWidth)

The "percent deadtime" will be

     DT_Percent = 100 * (N_Resets + N_AllEvents*EventWidth)/ClockTicks

From this, the only real adjustable parameter for controlling the
deadtime correction is the EventWidth, which will be an integer value
between about 4 (50 nsec) and 12 (150 nsec), depending on the detector
type.  This value should be set individually for each detector element
in the Xspress3 calibration procedure.  But, you may want to measure
deadtime curves and tweak these values yourself.   The IOC startup scripts 
end with setting these values, as with:

  # dbpf("$(PREFIX)C1:EventWidth",    "6")

By default, these are commented out, so that the calibration values
will be used, but you can adjust these values here. 


Running the XRF Display and Control program
=================================================

In addition to the display manager screens for MEDM, caQtDM, and
CSS/BOY, there is a standalone application for viewing XRF data from
Xspress3.  This will be downloaded and installed into
/home/xspress3/xraylarch and build a small python script installed to 
bin/run_xrfcontrol.py to run the XRF Control application.


