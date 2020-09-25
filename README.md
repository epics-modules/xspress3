# XSPRESS3
An EPICS driver based on [areaDetector](https://github.com/areaDetector) for [Quantum Detector](http://www.quantumdetectors.com) Xspress3 electronics.


# Installation

The simplest installation process will build a complete epics environment:

   mkdir /home/xspress3/epics
   cd    /home/xspress3/epics
   wget  https://github.com/epics-modules/xspress3/blob/master/build_xspress3.py
   python build_xspress3.py all


# Run

The above installation will build all the epics modules, including the
Xspress3 application in the current folder. It will also create a
'bin' directory in your /home/xspress3/epics' folder that contains
scripts to set your environment and run the Xspress3 IOC:

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

    /home/xspress3/epics/bin/run_medm.sh

or
    /home/xspress3/epics/bin/run_xrfcontrol.py


