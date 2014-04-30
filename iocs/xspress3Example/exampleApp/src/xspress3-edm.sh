#!/bin/bash

prefix=$(XSPRESS3_PREFIX)
EPICS_CA_SERVER_PORT=5064
EPICS_CA_REPEATER_PORT=5065

declare -A locations
locations[XSPRESS3]=$(XSPRESS3)
locations[AREA_DETECTOR]=$(AREA_DETECTOR)
locations[DEVIOCSTATS]=$(DEVIOCSTATS)
locations[ASYN]=$(ASYN)
locations[SPECTRAPLUGINS]=$(SPECTRAPLUGINS)

# Default edl display locations in source tree
declare -A edls
edls[XSPRESS3]=xspress3App/opi/edl/xspress3.edl
edls[AREA_DETECTOR]=ADApp/op/edl/ADBase.edl
edls[DEVIOCSTATS]=iocAdmin/srcDisplay/ioc_stats_soft.edl
edls[ASYN]=opi/edm/asynRecord.edl
edls[SPECTRAPLUGINS]=spectraPluginsApp/opi/edl/NDAttribute.edl

# The $ escapes the MSI macro substitution
for module in "\${!locations[@]}" ; do
    if [ -f \${locations[\$module]}/data/\$(basename \${edls[\$module]}) ] ; then
        EDMDATAFILES=\$EDMDATAFILES:\${locations[\$module]}/data/
    else
        EDMDATAFILES=\$EDMDATAFILES:\${locations[\$module]}/\$(dirname \${edls[\$module]})
    fi
done
export EDMDATAFILES

edm -x -m "P=\$prefix" -port \$EPICS_CA_SERVER_PORT xspress3.edl &
edm -x -m "ioc=\$prefix" -port \$EPICS_CA_SERVER_PORT ioc_stats_soft.edl &
echo \$EDMDATAFILES
