#!/bin/bash

installDir=$1
appName=$(basename $installDir)

if [[ "${appName}" =~ example([0-9]+)Channel ]] ; then
    numChannels=${BASH_REMATCH[1]}

    cat <<EOF >> $installDir/configure/CONFIG_SITE
# XSPRESS3 configuration parameters
XSPRESS3_PREFIX=XSPRESS3-EXAMPLE
XSPRESS3_CONFIG:=\$(abspath \$(XSPRESS3))/xspress3_settings/${numChannels}channel
XSPRESS3_SAVE:=\$(abspath \$(TOP))/save
XSPRESS3_DATA=/home/${USER}/data
EOF

    cat <<"EOF" >> $installDir/${appName}App/src/Makefile
SCRIPTS += xspress3-medm.sh xspress3-edm.sh
SYS_BOOT_FLAGS+=-MXSPRESS3=$(abspath $(XSPRESS3))
SYS_BOOT_FLAGS+=-MXSPRESS3_PREFIX=$(XSPRESS3_PREFIX)
%.sh: ../%.sh
	$(MSI) $(SYS_BOOT_FLAGS) $< | sed 's/\\\$$/$$/g' > $@
EOF

    sed -i 's/^dbLoadRecords\(.*\)$/dbLoadRecords( \1, "XSPRESS3_PREFIX=$(XSPRESS3_PREFIX)")/'  $installDir/iocBoot/ioc${appName}/st${appName}.src

    sed -i '/include.*RULES/i \
USR_BOOT_FLAGS+=-MXSPRESS3_PREFIX=$(XSPRESS3_PREFIX)\
USR_BOOT_FLAGS+=-MXSPRESS3_CONFIG=$(abspath $(XSPRESS3_CONFIG))\
USR_BOOT_FLAGS+=-MXSPRESS3_SAVE=$(abspath $(XSPRESS3_SAVE))\
USR_BOOT_FLAGS+=-MXSPRESS3_DATA=$(abspath $(XSPRESS3_DATA))\
USR_BOOT_FLAGS+=-MXSPRESS3_ABSPATH=$(abspath $(XSPRESS3))\
' $installDir/iocBoot/ioc${appName}/Makefile

else
    echo $appName not post-edited
fi