#!/bin/bash

FONT=runtimes/native/fonts/PublicPixel.ttf
OUTPUT=runtimes/native/fonts_data.h

python scripts/convert_font.py $FONT > $OUTPUT
