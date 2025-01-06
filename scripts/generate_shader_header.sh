#!/bin/bash

function usage {
  echo "usage: $0 shader_directory"
  exit 1
}

if [ $# -ne 1 ] || [ -z $1 ];
then
  usage 
  exit
fi

printf "#ifndef NUX_SHADER_DATA_H\n"
printf "#define NUX_SHADER_DATA_H\n"

printf "#include <nulib.h>\n"

for file in $1/*; do
    name="shader_$(basename $file | tr '.' '_')" 
    printf "static const nu_sv_t $name = NU_SV(\n"
    while IFS= read -r line; do
        # if [[ -z $line ]]; then
        #     continue 
        # fi
        printf "\"$line\\\n\"\n"
    done < $file
    printf ");\n"
done

printf "#endif\n"

