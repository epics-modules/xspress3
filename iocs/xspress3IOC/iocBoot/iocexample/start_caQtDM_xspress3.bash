#!/bin/bash
export EPICS_CA_MAX_ARRAY_BYTES=30000000
source /local/DPbin/Scripts/Paths.bash
source /local/DPbin/Scripts/caQtDM_setup.bash


caQtDM -macro "P="$1":, R=det1:" xspress3_custom.ui &
