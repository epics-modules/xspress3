#!/bin/bash
#
# Script to build Xspress3 from original sources
#

export http_proxy="http://wwwcache.rl.ac.uk:8080"
tar_files=/home/$USER/Downloads

declare -A directories
directories[EPICS_BASE]=/home/$USER/temp/epics/R3.14.12.3/base
directories[BOOST]=/dls_sw/prod/tools/RHEL6-x86_64/boost/1-48-0
directories[SUPPORT]=/home/$USER/temp/epics/R3.14.12.3/support

declare -A sources
sources[EPICS_BASE]=http://www.aps.anl.gov/epics/download/base/baseR3.14.12.3.tar.gz
sources[EXTENSIONS_TOP]=http://www.aps.anl.gov/epics/download/extensions/extensionsTop_20120904.tar.gz
sources[MSI]=http://www.aps.anl.gov/epics/download/extensions/msi1-6.tar.gz
sources[MEDM]=http://www.aps.anl.gov/epics/download/extensions/medm3_1_7.tar.gz
sources[EDM]='http://ics-web.sns.ornl.gov/edm/info/getAttachment.php?attachId=448&name=edm-1-12-89.tgz'
sources[BOOST]=http://sourceforge.net/projects/boost/files/boost/1.48.0/boost_1_48_0.tar.gz
sources[ASYN]=http://www.aps.anl.gov/epics/download/modules/asyn4-21.tar.gz
sources[AUTOSAVE]=http://www.aps.anl.gov/bcda/synApps/tar/autosave_R5-1.tar.gz
sources[BUSY]=http://www.aps.anl.gov/bcda/synApps/tar/busy_R1-6.tar.gz
sources[SSCAN]=http://www.aps.anl.gov/bcda/synApps/tar/sscan_R2-9.tar.gz
sources[CALC]=http://www.aps.anl.gov/bcda/synApps/tar/calc_R3-2.tar.gz
sources[AREA_DETECTOR]=http://cars.uchicago.edu/software/pub/areaDetectorR1-9-1.tgz
sources[DEVIOCSTATS]=http://sourceforge.net/projects/epics/files/devIocStats/devIocStats-3.1.11.tar.gz
sources[SPECTRAPLUGINS]=http://controls.diamond.ac.uk/downloads/support/spectraPlugins/1-6/spectraPlugins-1-6.tgz
sources[XSPRESS3]=http://controls.diamond.ac.uk/downloads/support/xspress3/1-6/xspress3-1-6.tgz

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

    if [ ! -d  ${directories[$module]} ] ; then
        echo Copying $module into ${directories[$module]}
        mkdir -p ${directories[$module]}
        pushd ${directories[$module]} > /dev/null
        tar --strip-components 1 -xzf $tar_files/$file
        popd > /dev/null
    fi
}

function build() {
     local directory=$1
     local force=${2:-false}

     pushd $directory > /dev/null

     if [ -f configure/RELEASE ] ; then
         [ -f configure/RELEASE.orig ] || mv configure/RELEASE  configure/RELEASE.orig

         local -A sed_edit
         for key in ${!directories[@]} ; do
             sed_edit[$key]="-e  s,^\s*$key\s*=.*$,$key=${directories[$key]/#${directories[SUPPORT]}/\$(SUPPORT)},"
         done

         # Fix the SUPPORT edit to not replace its own definition
         sed_edit[SUPPORT]="-e  s,^\s*SUPPORT\s*=.*$,SUPPORT=${directories[SUPPORT]},"

         sed -r  \
             -e 's/AREADETECTOR/AREA_DETECTOR/' \
             -e 's/^SNCSEQ\s*=/#SNCSEQ=/' \
             -e 's/^WORK\s*=/#WORK=/' \
             -e 's/^IPAC\s*=/#IPAC=/' \
             ${sed_edit[@]} configure/RELEASE.orig >  configure/RELEASE
     fi

     echo -n "Building $directory......"
     {
         {
             $force && make clean uninstall
             make
             echo $? >build.sta
         } 4>&1 1>&3 2>&4 |
         tee build.err
     } >build.log 3>&1

     (( $(cat build.sta) != 0 )) && echo "[ERROR]" || echo "[OK]"
     popd > /dev/null
}

function Usage {
    echo "Usage: $0 [-hf] [-m module] [-e epics_path] [-b boost_path] [-s support_path] [-t tar_path]"
    echo
    echo " h : print this help message"
    echo " f : force rebuild by running make clean first"
    echo " m : Specify a specific module (or modules) to rebuild (use with caution because of dependencies)"
    echo " e : Path to location where EPICS base is to be built (default ${directories[EPICS_BASE]})."
    echo " b : Path to location where BOOST is installed (default ${directories[BOOST]})."
    echo " s : Path to location where EPICS support modules are to be built (default ${directories[SUPPORT]})."
    echo " t : Path to where downloaded tar files are to be stored (default $tar_files)."
    exit 1
}

force=false
while getopts 'hfm:e:b:s:t:' option; do
    case $option in
        h) Usage;;
        f) force=true;;
        m) build_modules=($build_modules $OPTARG);;
    esac
done

modules=(ASYN AUTOSAVE BUSY SSCAN CALC AREA_DETECTOR DEVIOCSTATS SPECTRAPLUGINS XSPRESS3)

download EPICS_BASE
if [ "${build_modules:-undefined}" == "undefined" ] ; then 
    build ${directories[EPICS_BASE]} $force
fi

for module in ${modules[@]}; do
    download $module
done

# Patch Autosave to install databases
[ -f ${directories[AUTOSAVE]}/asApp/Db/Makefile.orig ] || sed -i.orig '/^#DB +=/a\
DB += save_restoreStatus.db' ${directories[AUTOSAVE]}/asApp/Db/Makefile

build_modules=${build_modules:-${modules[@]}}
echo Building the following support modules: ${build_modules[@]}
for module in ${build_modules[@]}; do
    if [ "${directories[$module]}x" == "x" ] ; then
        echo "Don't know anything about module $module"
    else
        build ${directories[$module]} $force
    fi
done

build ${directories[XSPRESS3]}/iocs/xspress3Example $force
