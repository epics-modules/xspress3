.. xspress3epics documentation master file

.. _EPICS Controls System:  https://epics-controls.org/
.. _Quantum Detectors:      https://quantumdetectors.com/
.. _Xspress3:               https://quantumdetectors.com/products/xspress3/
.. _epics-modules:          https://github.com/epics-modules
.. _xspress 3 github page:   https://github.com/epics-modules/xspress3
.. _areaDetector:           https://areadetector.github.io/master/index.html

.. _Diamond Light Source:   https://www.diamond.ac.uk/
.. _Advanced Photon Source: https://www.aps.anl.gov/
.. _Xspress3 API Documentation: https://epics-modules.github.io/xspress3/xspress3api


Xspress3 Epics Module
==================================

Xspress3Epics provides support for `Quantum Detectors`_ `Xspress3`_ series of
pulse-counting readout electronics for the `EPICS Controls System`_. The
Xspress 3 electronics are used with X-ray fluorescence detectors, and provide
high performace in throughput and energy resolution.  Both single- and
multi-element detector systems are supported.  The EPICS driver for Xspress 3 is
build on top of EPICS `areaDetector`_ version 3, and supports all variations of
Xspress 3, including the "mini" and "X" series.

The driver was originally developed at `Diamond Light Source`_, but has been
developed within the `epics-modules`_ for many years.  Most of the recent
development of the driver has been a collaboration between scientists and
engineers working at the `Advanced Photon Source`_ and `Quantum Detectors`_, and done
at `xspress 3 github page`_.

Documentation for the low-level API to the Xspress3 system are at `Xspress3 API Documentation`_.


Table of Contents
-----------------------

.. toctree::
   :maxdepth: 2

   installation
   whatsnew
   ioc_index
   display_screens
   counting_modes
   xrfdisplay_epics
   usefulPVs
   file_structure
