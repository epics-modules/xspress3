#!/bin/bash
#
# Script to build Xspress3 from original sources
#

# If you need a http proxy, please edit the following, otherwise comment it out:
export http_proxy="http://wwwcache.rl.ac.uk:8080"

# Edit to set the location to store tar files
tar_files=/home/$USER/Downloads

# Edit the following declarations to locations where BOOST is installed, and
# where you want the downloaded software installed
declare -A directories
directories[BOOST]=/home/$USER/software/boost
directories[EPICS_BASE]=/home/$USER/software/epics/R3.14.12.3/base
directories[SUPPORT]=/home/$USER/software/epics/R3.14.12.3/support
directories[EXTENSIONS_TOP]=/home/$USER/software/epics/R3.14.12.3/extensions

###############################################################################
#
#     Normally, you wont have to edit code below this line
#
###############################################################################

export EPICS_HOST_ARCH=linux-x86_64

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
sources[XSPRESS3]=http://controls.diamond.ac.uk/downloads/support/xspress3/1-9/xspress3-1-9.tgz

directories[MSI]=${directories[EXTENSIONS_TOP]}/src/msi
directories[MEDM]=${directories[EXTENSIONS_TOP]}/src/medm
directories[EDM]=${directories[EXTENSIONS_TOP]}/src/edm

function download() {
    local module=$1
    local source=${sources[$module]}

    local file=$(basename ${source/*=/})
    echo Checking to download $file
    [ -d $tar_files ] || mkdir -p  $tar_files
    [ -f $tar_files/$file ] || wget  -O $tar_files/$file $source

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
             make $DEBUG_OPTS $PROFILE_OPTS
             echo $? >build.sta
         } 4>&1 1>&3 2>&4 |
         tee build.err
     } >build.log 3>&1

     (( $(cat build.sta) != 0 )) && echo "[ERROR]" || echo "[OK]"
     popd > /dev/null
}

function build_boost() {
     local directory=${directories[BOOST]}
     pushd $directory > /dev/null

     echo -n "Building $directory......"
     {
         {
             ./bootstrap.sh --prefix=$PWD \
                 --with-libraries=serialization,test,exception,iostreams,program_options,thread,timer
             echo $? >build.sta

             if (( $(cat build.sta) == 0 )) ; then
                 ./b2 install
                 echo $? >build.sta
             fi
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
    echo " m : Specify a specific module (or modules) to rebuild 
               (use with caution because of dependencies)"
    echo " e : Path to location where EPICS base is to be built
               (default ${directories[EPICS_BASE]})."
    echo " b : Path to location where BOOST is installed
               (default ${directories[BOOST]})."
    echo " s : Path to location where EPICS support modules are to be built
               (default ${directories[SUPPORT]})."
    echo " t : Path to where downloaded tar files are to be stored
               (default $tar_files)."
    echo " d : Include debug symbols in compilation"
    echo " p : Include profiling code in compilation"
    exit 1
}

force=false
while getopts 'pdhfm:e:b:s:t:' option; do
    case $option in
        h) Usage;;
        f) force=true;;
        m) build_modules=($build_modules $OPTARG);;
        e) directories[EPICS_BASE]=$OPTARG;;
        b) directories[BOOST]=$OPTARG;;
        s) directories[SUPPORT]=$OPTARG;;
        t) tar_files=$OPTARG;;
        d) DEBUG_OPTS="OPT_CXXFLAGS_YES+=-g OPT_CFLAGS_YES+=-g" ;;
        p) PROFILE_OPTS="OPT_CXXFLAGS_YES+=-pg OPT_CFLAGS_YES+=-pg DEBUG_LDFLAGS+=-pg" ;;
        *) Usage;;  
    esac
done

modules=(EXTENSIONS_TOP MSI EDM MEDM ASYN AUTOSAVE BUSY SSCAN CALC AREA_DETECTOR DEVIOCSTATS SPECTRAPLUGINS XSPRESS3)

# Download and build BOOST
download BOOST
build_boost

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

# Patch NDAttributeList to be case sensitive for speed
[ -f ${directories[AREA_DETECTOR]}/ADApp/ADSrc/NDArray.cpp.orig ] ||
    sed -i.orig '/done/s/epicsStrCaseCmp/strcmp/' ${directories[AREA_DETECTOR]}/ADApp/ADSrc/NDArray.cpp

# devIocStats needs the Extensions bin directory to be on the path since it needs msi.
export PATH=$PATH:${directories[EXTENSIONS_TOP]}/bin/$EPICS_HOST_ARCH

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


echo "
Build complete. Please edit the following file to create Xspress 3 defaults for your site:

${directories[XSPRESS3]}/iocs/xspress3Example/configure/CONFIG_SITE

then do the following:

cd ${directories[XSPRESS3]}/iocs/xspress3Example
make clean
make

Please also add the following lines to your ~/.bash_profile:

PATH=${directories[EPICS_BASE]}/bin/${EPICS_HOST_ARCH}:\$PATH
PATH=${directories[EXTENSIONS_TOP]}/bin/${EPICS_HOST_ARCH}:\$PATH
PATH=${directories[XSPRESS3]}/bin/${EPICS_HOST_ARCH}:\$PATH
PATH=${directories[XSPRESS3]}/iocs/xspress3Example/bin/${EPICS_HOST_ARCH}:\$PATH

The following commands are then available:
xspress3-ioc.sh  : runs the xspress3 ioc
xspress3-edm.sh  : runs the edm screens (beware fonts issues)
xspress3-medm.sh : runs the medm screens
xspress3.server  : runs xspress3 command line interface
imgd             : runs xspress3 spectra viewer
"
