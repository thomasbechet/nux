#!/bin/bash

SHADERS=runtimes/vmnative/shaders
OUTPUT=runtimes/vmnative/shaders_data.h

printf "#ifndef VMN_SHADERS_DATA_H\n" > $OUTPUT
printf "#define VMN_SHADERS_DATA_H\n" >> $OUTPUT

printf "#include <nulib.h>\n" >> $OUTPUT

for file in $SHADERS/*; do
    name="shader_$(basename $file | tr '.' '_')" 
    printf "static const nu_sv_t $name = NU_SV(\n" >> $OUTPUT
    while IFS= read -r line; do
        # if [[ -z $line ]]; then
        #     continue 
        # fi
        printf "\"$line\\\n\"\n" >> $OUTPUT
    done < $file
    printf ");\n" >> $OUTPUT
done

printf "#endif\n" >> $OUTPUT
