# ADXspress3

An EPICS driver based on [areaDetector](https://github.com/areaDetector) for
[Quantum Detector](http://www.quantumdetectors.com) Xspress3 electronics.

# Run

Change into the `iocXspress3` directory:

    cd /path/to/ADXspress3
    cd /iocs/xspress3IOC/iocBoot/iocXspress3

Adapt `st.cmd` as necessary, and note the number of channels `${XSP3CHANS}`.
If the corresponding files, eg. `xsp3-${XSP3CHANS}ch.req`, have not been
generated, generate them now:

    ./xsp3-chan.sh ${XSP3CHANS}

Put the calibration files into the `iocXspress3/cfg-${XSP3CHANS}ch` directory,
and then run the IOC:

    ../../bin/linux-x86_64/xspress3App ./st.cmd

# Manual

See the `documentation` directory.

