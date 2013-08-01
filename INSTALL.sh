#!/bin/bash
#
# Script to build Xspress3 from original sources
#

export http_proxy="http://wwwcache.rl.ac.uk:8080"
tar_files=/home/npr78/Downloads

declare -A directories
directories["EPICS_BASE"]=/scratch/npr78/temp/epics/base
directories["BOOST"]=/dls_sw/prod/tools/RHEL6-x86_64/boost/1-48-0
directories["SUPPORT"]=/scratch/npr78/temp/epics/R3.14.12.3/test

declare -A sources
sources["ASYN"]=http://www.aps.anl.gov/epics/download/modules/asyn4-21.tar.gz
sources["AUTOSAVE"]=http://www.aps.anl.gov/bcda/synApps/tar/autosave_R5-1.tar.gz
sources["BUSY"]=http://www.aps.anl.gov/bcda/synApps/tar/busy_R1-6.tar.gz
sources["SSCAN"]=http://www.aps.anl.gov/bcda/synApps/tar/sscan_R2-9.tar.gz
sources["CALC"]=http://www.aps.anl.gov/bcda/synApps/tar/calc_R3-2.tar.gz
sources["AREADETECTOR"]=http://cars.uchicago.edu/software/pub/areaDetectorR1-9-1.tgz
sources["DEVIOCSTATS"]=http://sourceforge.net/projects/epics/files/devIocStats/devIocStats-3.1.11.tar.gz
sources["SPECTRAPLUGINS"]=http://controls.diamond.ac.uk/downloads/support/spectraPlugins/1-4-2/spectraPlugins-1-4-2.tgz
sources["XSPRESS3"]=http://controls.diamond.ac.uk/downloads/support/xspress3/1-5/xspress3-1-5.tgz

function download() {
    local module=$1
    local source=${sources[$module]}

    local file=$(basename $source)
    echo Checking to download $file
    [ -d $tar_files ] || mkdir -p  $tar_files
    [ -f $tar_files/$file ] || wget  -P $tar_files $source

    [ -d ${directories["SUPPORT"]} ] ||  mkdir -p  ${directories["SUPPORT"]}
    pushd ${directories["SUPPORT"]}

    directories["$module"]='$(SUPPORT)/'$(tar tvzf $tar_files/$file | head -1 | awk '{print $6}' | sed 's,/,,g')
    [ -d $(basename ${directories["$module"]}) ] || tar xvzf $tar_files/$file
    popd
}

function build() {
     local directory=$1

     pushd $directory

     [ -f configure/RELEASE.orig ] || mv configure/RELEASE  configure/RELEASE.orig

     local -A sed_edit
     for key in ${!directories[@]} ; do
         sed_edit["$key"]="-e  s,^\s*$key\s*=.*$,$key=${directories[$key]},"
     done

     sed -r ${sed_edit[@]} \
         -e 's/AREA_DETECTOR/AREADETECTOR/' \
         -e 's/^SNCSEQ\s*=/#SNCSEQ=/' \
         -e 's/^WORK\s*=/#WORK=/' \
         -e 's/^IPAC\s*=/#IPAC=/' configure/RELEASE.orig >  configure/RELEASE

     echo Building $directory
     make
     popd
}

modules=(ASYN AUTOSAVE BUSY SSCAN CALC AREADETECTOR DEVIOCSTATS SPECTRAPLUGINS XSPRESS3) 

[ "$1"x == "-f"x ] && rm -rf ${directories["SUPPORT"]}

for module in ${modules[@]}; do
    download $module
done

for module in ${modules[@]}; do
    build ${directories["SUPPORT"]}/$(basename ${directories["$module"]})
done

build ${directories["SUPPORT"]}/$(basename ${directories["XSPRESS3"]})/