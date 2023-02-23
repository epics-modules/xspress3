Setting up and Running an Epics Input/Output Controller
=========================================================


Running the Epics IOC
=======================

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


After building you can configure an Xspress3 IOC, starting with one of
the defaults in the
    /home/xspress3/epics/xspress3/iocs/xspress3IOC/iocBoot

directory. Note that if you change the prefix or the number of
detector elements used, you may need to edit several of the files in
the bin/ directory to match your configuration.

Once properly configured, you should be able to run your xspress3 in a
long-running procServ process with

    /home/xspress3/epics/bin/start_ioc xspress3

and then view screens for your detector either using an Epics display
manager like medm, caqtdm, or css/boy.  Display screens for these
display managers can be found in the folders

    /home/xspress3/epics/adls   screen files for medm
    /home/xspress3/epics/uis    screen files for caqtm
    /home/xspress3/epics/opis   screen files for css/boy

For example, you will be able to run MEDM for your Xspress3 with

    /home/xspress3/epics/bin/run_medm.sh

You will also be able to run a dedicated XRF Control application with

    /home/xspress3/epics/bin/run_xrfcontrol.py
