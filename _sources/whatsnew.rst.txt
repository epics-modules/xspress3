Release Notes
======================================


The latest version of the Xspress3 Epics driver is |release|

.. _whatsnew_327_label:

Version 3.2.7 Release Notes (2023-March-02)
--------------------------------------------

New features:

- Circular buffer mode
- Software frame advance


Bug fixes and enhancements:

- `Erase` is generally must faster, as only the most recent number of frames
  taken are erased, not the full list of available frames.  Since that will
  usually be fewer than 1000 frames, and often only 1 or 10 frames, that is
  significantly faster than erasing all (12216 or 16384) frames. Erasing will
  set that number of "recent frames" to 0, so that a second `Erase` will be
  very fast.


Version 3.2.6 Release Notes (2021-Oct-27)
------------------------------------------

New features:

- Added "Erase on Start" option, which will control whether `Erase` happens for
  each `Acquire`.
