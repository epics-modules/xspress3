#!/bin/bash

export PATH=$PATH:/APSshare/caqtdm/bin/

# caQtDM Stuff
export QT_PLUGIN_PATH=/APSshare/caqtdm/lib:/APSshare/caqtdm/plugins
# ADCore
export CAQTDM_DISPLAY_PATH=${ADCORE}/ADApp/op/ui/:${ADCORE}/ADApp/op/ui/autoconvert/:$CAQTDM_DISPLAY_PATH

# Do the rest of Area Detectors
#ADAndor
export CAQTDM_DISPLAY_PATH=${ADANDOR}/andorApp/op/ui:${ADANDOR}/andorApp/op/ui/autoconvert/:$CAQTDM_DISPLAY_PATH
#ADAndor3
export CAQTDM_DISPLAY_PATH=${ADANDOR3}/andor3App/op/ui:${ADANDOR3}/andor3App/op/ui/autoconvert/:$CAQTDM_DISPLAY_PATH
#ADMar345
export CAQTDM_DISPLAY_PATH=${ADMAR345}/mar345App/op/ui/:${ADMAR345}/mar345App/op/ui/autoconvert/:$CAQTDM_DISPLAY_PATH
#ADMarCCD
export CAQTDM_DISPLAY_PATH=${ADMARCCD}/marCCDApp/op/ui/:${ADMARCCD}/marCCDApp/op/ui/autoconvert/:$CAQTDM_DISPLAY_PATH
#ADMythen
export CAQTDM_DISPLAY_PATH=${ADMYTHEN}/mythenApp/op/ui/:${ADMYTHEN}/mythenApp/op/ui/autoconvert/:$CAQTDM_DISPLAY_PATH
#ADPilatus
export CAQTDM_DISPLAY_PATH=${ADPILATUS}/pilatusApp/op/ui/:${ADPILATUS}/pilatusApp/op/ui/autoconvert/:$CAQTDM_DISPLAY_PATH
#ADPixirad
export CAQTDM_DISPLAY_PATH=${ADPIXIRAD}/pixiradApp/op/ui/:${ADPIXIRAD}/pixiradApp/op/ui/autoconvert/:$CAQTDM_DISPLAY_PATH
#ADProsilica
export CAQTDM_DISPLAY_PATH=${ADPROSILICA}/prosilicaApp/op/ui/:${ADPROSILICA}/prosilicaApp/op/ui/autoconvert/:$CAQTDM_DISPLAY_PATH

# asyn
export CAQTDM_DISPLAY_PATH=${ASYN}/opi/caqtdm/:$CAQTDM_DISPLAY_PATH

# Autosave
export CAQTDM_DISPLAY_PATH=${AUTOSAVE}/asApp/op/ui/:$CAQTDM_DISPLAY_PATH

# Busy
export CAQTDM_DISPLAY_PATH=${BUSY}/busyApp/op/ui/:$CAQTDM_DISPLAY_PATH

# Calc
export CAQTDM_DISPLAY_PATH=${CALC}/calcApp/op/ui/:$CAQTDM_DISPLAY_PATH

# DXP
export CAQTDM_DISPLAY_PATH=${DXP}/dxpApp/op/ui/:$CAQTDM_DISPLAY_PATH

# MCA
export CAQTDM_DISPLAY_PATH=${MCA}/mcaApp/op/ui/:$CAQTDM_DISPLAY_PATH

# Motor
export CAQTDM_DISPLAY_PATH=${MOTOR}/motorApp/op/ui/:$CAQTDM_DISPLAY_PATH

# SoftGlue
export CAQTDM_DISPLAY_PATH=${SOFTGLUE}/softGlueApp/op/ui/:$CAQTDM_DISPLAY_PATH

# sscan
export CAQTDM_DISPLAY_PATH=${SSCAN}/sscanApp/op/ui/:$CAQTDM_DISPLAY_PATH

# Xspress3 Directory
export CAQTDM_DISPLAY_PATH=${ADXSPRESS3}/xspress3App/opi/ui/:$CAQTDM_DISPLAY_PATH
