#!/bin/bash

export EPICS_CA_MAX_ARRAY_BYTES=3000000
export EPICS_DISPLAY_PATH=/local/DPbin/epics/epics_2015-08-31/synApps_5_8/support/adls/:./:$EPICS_DISPLAY_PATH

medm -x -macro "P=dp_xsp3_1:, R=det1:" xspress3_custom.adl &

