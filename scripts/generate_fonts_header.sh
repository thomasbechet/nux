#!/bin/bash

ROOT_DIR=$1
if [[ ! -d $ROOT_DIR ]]; then
    echo "Please provide a valid root dir."
    exit 1
fi

FONT=$ROOT_DIR/runtimes/native/fonts/PublicPixel.ttf
OUTPUT=$ROOT_DIR/runtimes/native/fonts_data.h

python $ROOT_DIR/scripts/convert_font.py $FONT > $OUTPUT
