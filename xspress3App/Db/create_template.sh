#!/bin/sh

# Generate a template with the specified number of channels and ROIs.
# The number of channels and ROIs must be specified on the commandline.

XSPRESS3PATH=.
BINS=4096
NREQUIREDOPTS=2
nopts=0
#echo "Before opts"
while getopts ":n: r: p:" opt; do
    case $opt in
        n)
            NCHANS=$OPTARG
            nopts=$((nopts + 1))
            ;;
        r)
            NROISPERCHAN=$OPTARG
            nopts=$((nopts + 1))
            ;;
        p)
            XSPRESS3PATH=$OPTARG
            ;;
        b)
            BINS=$OPTARG
            ;;
        \?)
            echo "Invalid option: -$OPTARG" >&2
            exit 1
            ;;
        :)
            echo "Option -$OPTARG requires an argument" >&2
            exit 1
            ;;
    esac
done
#echo "After opts" $nopts "$XSPRESS3PATH" >&2

if [ $nopts -ne $NREQUIREDOPTS ]; then
    echo "Required argument missing" >&2
    exit 1
fi

set -u

# Insert header
if [ -a "$XSPRESS3PATH/db/xspress3_header.template" ]; then
    cat "$XSPRESS3PATH/db/xspress3_header.template"
    env printf '\nsubstitute "TYPE=Float64, FTVL=DOUBLE, NELEMENTS=%d"' $BINS
    echo
    NROI=0
    for channel in $(seq 1 $NCHANS); do
        env printf '\nsubstitute "R=:ROI%d:, PORT=XSP3.ROICHAN, ROI=%d"\ninclude "NDROIStatN.template"\n' $channel $NROI
        env printf 'substitute "PORT=XSP3.STD1, ADDR=%d"\ninclude "NDStdArrays.template"\n' $NROI
        NROI=$((NROI + 1))
        for roi in $(seq 1 $NROISPERCHAN); do
            env printf '\nsubstitute "PORT=XSP3.ROICHAN, R=:C%d_ROI, ROI=%d"\ninclude "NDROIStatN.template"\n' $channel $NROI
            NROI=$((NROI + 1))
        done
    done
fi

