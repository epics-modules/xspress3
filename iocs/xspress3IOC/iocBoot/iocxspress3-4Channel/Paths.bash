#!/bin/bash
export SUPPORT=/local/DPbin/epics/epics_2015-05-31/synApps_5_8/support/

#export EPICS support locations to envorment variables 
output=`perl -s release.pl -form=bash ${SUPPORT}/configure/RELEASE`
eval $output

#export AreaDetector locations to envorment variables 
output=`perl -s release.pl -form=bash ${AREA_DETECTOR}/configure/RELEASE_PATHS.local`
eval $output

output=`perl -s release.pl -form=bash ${AREA_DETECTOR}/configure/RELEASE_PATHS.local.${EPICS_HOST_ARCH}`
eval $output


