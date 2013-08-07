#!/bin/bash

epics=$(awk -F= '/^[:space:]*EPICS_BASE[:space:]*=/{print $2}' configure/RELEASE)

for dir in configure iocs/xspress3Example/configure ; do
    cp $epics/configure/*.Dls $dir
    cp $epics/bin/$EPICS_HOST_ARCH/convertDlsRelease.pl $dir

    sed -i '/include.*Dls[:space:]*$/s,(CONFIG),(TOP)/configure,' $dir/CONFIG $dir/RULES
    sed -i '/convertDlsRelease/s,(EPICS_BASE_HOST_BIN),(TOP)/configure,' $dir/CONFIG.Dls
    sed -i 's/AREADETECTOR/AREA_DETECTOR/g' $dir/RELEASE
done

sed -i '/^dbpf/s/"Yes")/"Enabled")/' iocs/xspress3Example/iocBoot/iocexample/stexample.src