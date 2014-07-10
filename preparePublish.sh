#!/bin/bash

epics=$(awk -F= '/^[:space:]*EPICS_BASE[:space:]*=/{print $2}' configure/RELEASE)

for dir in configure iocs/*/configure; do
    if [ "$1" == "-r" ]; then
        rm -f $dir/*.Dls $dir/convertDlsRelease.pl
        sed -i '/include.*Dls[:space:]*$/s,(TOP)/configure,(CONFIG),' $dir/CONFIG $dir/RULES
    else
        cp $epics/configure/*.Dls $dir
        cp $epics/bin/$EPICS_HOST_ARCH/convertDlsRelease.pl $dir

        sed -i '/include.*Dls[:space:]*$/s,(CONFIG),(TOP)/configure,' $dir/CONFIG $dir/RULES
        sed -i '/convertDlsRelease/s,(EPICS_BASE_HOST_BIN),(TOP)/configure,' $dir/CONFIG.Dls
    fi
done

for file in configure/RELEASE iocs/*/configure/RELEASE \
            xspress3App/Db/*.template iocs/*/*App/Db/*.substitutions \
            iocs/*/*App/src/[^O]*; do    
    if [ "$1" == "-r" ]; then
        sed -i 's/AREA_DETECTOR/AREADETECTOR/g' $file
    else
        sed -i 's/AREADETECTOR/AREA_DETECTOR/g' $file
    fi
done

for file in iocs/*/iocBoot/ioc*/st*.src; do
    if [ "$1" == "-r" ]; then
        sed -i '/^dbpf/s/"Enable")/"Yes")/' $file
    else
        sed -i '/^dbpf/s/"Yes")/"Enable")/' $file
    fi
done

