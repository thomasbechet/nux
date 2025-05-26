#!/bin/bash

ROOT_DIR=$1
if [[ ! -d $ROOT_DIR ]]; then
    echo "Please provide a valid root dir."
    exit 1
fi

SLANG=$ROOT_DIR/core/shaders/main.slang
OUTPUT=$ROOT_DIR/runtimes/native/shaders/main

slangc $SLANG -profile glsl_450 -target spirv -o $OUTPUT.vert.spv -entry vertexMain 
slangc $SLANG -profile glsl_450 -target spirv -o $OUTPUT.frag.spv -entry fragmentMain
slangc $SLANG -profile glsl_450 -target glsl -o $OUTPUT.vert.glsl -entry vertexMain 
slangc $SLANG -profile glsl_450 -target glsl -o $OUTPUT.frag.glsl -entry fragmentMain
