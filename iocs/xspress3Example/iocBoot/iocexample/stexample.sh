#!/bin/bash

BOOST=/dls_sw/work/common/boost/1.49.0

export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:$BOOST/stage/lib
echo $LD_LIBRARY_PATH

exec ./bin/linux-x86_64/example bin/linux-x86_64/stexample.boot
