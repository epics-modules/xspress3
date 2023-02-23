.. _xraylarch: https://xraypy.github.io/xraylarch


.. _xrf_display_chapter:

Installing and using the ``xrfdisplay_epics`` application
============================================================

While the Epics display screens give access to the low-level Epics interface
for the Xspress3 driver, most Xspress3 users will use the interface for X-ray
Fluoresence (XRF) spectroscopy. The Epics display screens do not give
a very rich or high-level interface for working with the XRF spectra.
The ``Epics XRF Control`` application from `xraylarch`_ can be used for basic
interactions with detectors using the Xspress3 module, and a richer set of
tools for working with XRF spectra.

By default this will be installed with the `build_xpsress3.py` script discussed
in :ref:`install_chapter`, downloading and installing a Anaconda Python
environment in ``/home/xspress3/xraylarch``, which includes a program called
`xrfdisplay_epics` that will display XRF spectra.
