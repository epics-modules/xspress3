#!/bin/bash

BOOST=/dls_sw/prod/tools/RHEL6-x86_64/boost/1-48-0

export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:$BOOST/prefix/lib
echo $LD_LIBRARY_PATH

exec ./bin/linux-x86_64/example bin/linux-x86_64/stexample.boot
