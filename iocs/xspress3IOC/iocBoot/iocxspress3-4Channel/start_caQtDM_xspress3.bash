#!/bin/bash
export EPICS_CA_MAX_ARRAY_BYTES=30000000
source Paths.bash
source caQtDM_setup.bash


caQtDM -attach -macro "P=dp_xsp3_1:, R=det1:" xspress3_custom.ui &
