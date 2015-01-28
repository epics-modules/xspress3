#!/usr/bin/env python

# set location "GSE" or "DLS" 

from __future__ import print_function

_usage_ = "set_location.py (GSE|DLS)"

config_files = ('Makefile',
                'iocs/Makefile',
                'configure/RELEASE',
                'iocs/example4Channel/configure/CONFIG_SITE',
                'iocs/example4Channel/configure/RELEASE',
                'iocs/xspress3Example/configure/RELEASE')

import sys
import shutil

if len(sys.argv) < 2:
    print(_usage_)
    sys.exit()

location = sys.argv[1].upper()
if location not in ('DLS', 'GSE'):
    print(_usage_)
    sys.exit()

for fname in config_files:
    src = "%s.%s" % (fname, location)
    shutil.copy(src, fname)
    print("copied %s to %s." % (src, fname))

