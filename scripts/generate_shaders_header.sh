#!/bin/bash

ROOT_DIR=$1
if [[ ! -d $ROOT_DIR ]]; then
    echo "Please provide a valid root dir."
    exit 1
fi

SHADERS=$ROOT_DIR/runtimes/native/shaders
OUTPUT=$ROOT_DIR/runtimes/native/shaders_data.c.inc

printf "#include <nulib/nulib.h>\n" >> $OUTPUT

for file in $SHADERS/*; do
    name="shader_$(basename $file | tr '.' '_')" 
    printf "static const nu_sv_t $name = NU_SV(\n" >> $OUTPUT
    while IFS= read -r line; do
        # if [[ -z $line ]]; then
        #     continue 
        # fi
        #printf "\"$line\\\n\"\n" >> $OUTPUT
        printf "\"%s\\\n\"\n" "$line" >> $OUTPUT
    done < $file
    printf ");\n" >> $OUTPUT
done
