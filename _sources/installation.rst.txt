.. _build_xspress3.py: https://raw.githubusercontent.com/epics-modules/xspress3/master/build_xspress3.py
.. _xspress3 sources: https://millenia.cars.aps.anl.gov/software/xspress3/sources
.. _xraylarch: https://xraypy.github.io/xraylarch

.. _install_chapter:

Installation
=====================================

The Xpsress3 module needs to run on a Linux computer connected to the Xspress3
module.  Generally, these have used the Redhat/CentOS Linux distribution, and
run when logged into the `xspress3` user account.  The instruction methods here
may assume that distribution, but it may be possible to build and run the
Xspress3 Epics driver on other Linux distribution or other user accounts.

The installation process described here will build a complete and standalone
Epics environment under the directory `/home/xspress3/epics`. It is certainly
possible to install within an existing Epics development environment.

The `build_xspress3.py`_ python script is simplest and easiest way to install
the Xspress3 driver.  Ideally, this driver can be installed with::


    mkdir /home/xspress3/epics
    cd    /home/xspress3/epics
    wget  https://raw.githubusercontent.com/epics-modules/xspress3/master/build_xspress3.py
    python build_xspress3.py all


Required System Packages
-------------------------

The following packages (using the Redhat/Centos names) are required to build
and run the Xspress3:

   * re2c
   * readline, readline-devel
   * hdf5-devel
   * libtiff-devel
   * bzip-devel
   * libxml2-devel
   * GraphicsMagick-devel

In addition, the `telnet` package is recommended to run the `procServ` process,
and `motif-devel` is required if you intend to use the MEDM display screens.

It should be possible to install all of these with::

  sudo yum install re2c readline-devel hdf5-devel libtiff-devel \
                   bzip-devel libxml2-devel GraphicsMagick-devel \
                   maktelnet motif-devel


Required Epics Modules
-------------------------

The following Epics modules are needed to build and run the Xspress3
Input/Output Controller program, with the currently installed versions:

  * epics base 7.0.3.1
  * asyn 4-38
  * seq  2.2.6
  * ipac 2.15
  * alive R1-1-1
  * std R3-6
  * autosave R5-10
  * iocStats 3.1.16
  * busy R1-7-2
  * sscan R2-11-3
  * calc R3-7-3
  * areaDetector R3-9
  * adSupport R1-9
  * adCore R3-9
  * procServ 2.6.0
  * medm 3.0.3
  * xspress 3.2.7

The `build_xspress3.py`_ script will download these versions of these packages
from the `xspress3 sources`_ web site, hosted at
https://millenia.cars.aps.anl.gov/software/xspress3.  We expect that newer
versions of any of the above packages should also work.


The `build_xspress3.py` script
--------------------------------

The `build_xspress3.py`_ python script will

   1. check the installation of required system packages.
   2. download the required Epics modules.
   3. configure the require Epics modules, which typically involves writing
      environmental variables into a file in the ``configure`` folder for each
      of the modules.
   4. write a master `Build.sh` shell script.
   5. run the `Build.sh` shell script to build each module.  This shell script
      may be useful for debugging the build process.
   6. Download and install `xraylarch`_ for its `xrfdisplay_epics`
      application (see :ref:`xrf_display_chapter`).
