#!/bin/bash
#
# Script to build Xspress3 from original sources
#

export http_proxy="http://wwwcache.rl.ac.uk:8080"
tar_files=/home/npr78/Downloads

declare -A directories
directories[EPICS_BASE]=/home/npr78/temp/epics/R3.14.12.3/base
directories[BOOST]=/dls_sw/prod/tools/RHEL6-x86_64/boost/1-48-0
directories[SUPPORT]=/home/npr78/temp/epics/R3.14.12.3/support

declare -A sources
sources[EPICS_BASE]=http://www.aps.anl.gov/epics/download/base/baseR3.14.12.3.tar.gz
sources[BOOST]=http://sourceforge.net/projects/boost/files/boost/1.48.0/boost_1_48_0.tar.gz
sources[ASYN]=http://www.aps.anl.gov/epics/download/modules/asyn4-21.tar.gz
sources[AUTOSAVE]=http://www.aps.anl.gov/bcda/synApps/tar/autosave_R5-1.tar.gz
sources[BUSY]=http://www.aps.anl.gov/bcda/synApps/tar/busy_R1-6.tar.gz
sources[SSCAN]=http://www.aps.anl.gov/bcda/synApps/tar/sscan_R2-9.tar.gz
sources[CALC]=http://www.aps.anl.gov/bcda/synApps/tar/calc_R3-2.tar.gz
sources[AREADETECTOR]=http://cars.uchicago.edu/software/pub/areaDetectorR1-9-1.tgz
sources[DEVIOCSTATS]=http://sourceforge.net/projects/epics/files/devIocStats/devIocStats-3.1.11.tar.gz
sources[SPECTRAPLUGINS]=http://controls.diamond.ac.uk/downloads/support/spectraPlugins/1-4-2/spectraPlugins-1-4-2.tgz
sources[XSPRESS3]=http://controls.diamond.ac.uk/downloads/support/xspress3/1-5/xspress3-1-6alpha.tgz

function download() {
    local module=$1
    local source=${sources[$module]}

    local file=$(basename $source)
    echo Checking to download $file
    [ -d $tar_files ] || mkdir -p  $tar_files
    [ -f $tar_files/$file ] || wget  -P $tar_files $source

    if [ "${directories[$module]:-undefined}" == "undefined" ] ; then
        directories[$module]=${directories[SUPPORT]}/$(tar tvzf $tar_files/$file | head -1 | awk '{print $6}' | sed 's,/,,g')
    fi

    echo Copying $module into ${directories[$module]}
    if [ ! -d  ${directories[$module]} ] ; then
        mkdir -p ${directories[$module]}
        pushd ${directories[$module]}
        tar --strip-components 1 -xzf $tar_files/$file
        popd
    fi
}

function build() {
     local directory=$1

     pushd $directory

     if [ -f configure/RELEASE ] ; then
         [ -f configure/RELEASE.orig ] || mv configure/RELEASE  configure/RELEASE.orig

         local -A sed_edit
         for key in ${!directories[@]} ; do
             sed_edit[$key]="-e  s,^\s*$key\s*=.*$,$key=${directories[$key]/#${directories[SUPPORT]}/\$(SUPPORT)},"
         done

         # Fix the SUPPORT edit to not replace its own definition
         sed_edit[SUPPORT]="-e  s,^\s*SUPPORT\s*=.*$,SUPPORT=${directories[SUPPORT]},"

         sed -r  \
             -e 's/AREA_DETECTOR/AREADETECTOR/' \
             -e 's/^SNCSEQ\s*=/#SNCSEQ=/' \
             -e 's/^WORK\s*=/#WORK=/' \
             -e 's/^IPAC\s*=/#IPAC=/' \
             ${sed_edit[@]} configure/RELEASE.orig >  configure/RELEASE
     fi

     echo Building $directory
     {
         {
             make
             echo $? >build.sta
         } 4>&1 1>&3 2>&4 |
         tee build.err
     } >build.log 3>&1
     popd
}

modules=(ASYN AUTOSAVE BUSY SSCAN CALC AREADETECTOR DEVIOCSTATS SPECTRAPLUGINS XSPRESS3) 

[ "$1"x == "-f"x ] && rm -rf ${directories[SUPPORT]}

download EPICS_BASE
build ${directories[EPICS_BASE]}

for module in ${modules[@]}; do
    download $module
done

find ${directories[AREADETECTOR]} -type f -print0 | xargs -0 sed -i.orig 's/\$(AREA_DETECTOR)/$(AREADETECTOR)/g'
sed -i.orig '/^#DB +=/a\
DB += save_restoreStatus.db' ${directories[AUTOSAVE]}/asApp/Db/Makefile

for module in ${modules[@]}; do
    build ${directories[$module]}
done

build ${directories[XSPRESS3]}/iocs/xspress3Example

