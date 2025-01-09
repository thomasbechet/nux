#!/bin/bash

function usage {
  echo "usage: $0 shaders_directory"
  exit 1
}

if [ $# -ne 1 ] || [ -z $1 ];
then
  usage 
  exit
fi

FILENAME=runtimes/vmnative/shaders_data.h

printf "#ifndef NUX_SHADERS_DATA_H\n" > $FILENAME
printf "#define NUX_SHADERS_DATA_H\n" >> $FILENAME

printf "#include <nulib.h>\n" >> $FILENAME

for file in $1/*; do
    name="shader_$(basename $file | tr '.' '_')" 
    printf "static const nu_sv_t $name = NU_SV(\n" >> $FILENAME
    while IFS= read -r line; do
        # if [[ -z $line ]]; then
        #     continue 
        # fi
        printf "\"$line\\\n\"\n" >> $FILENAME
    done < $file
    printf ");\n" >> $FILENAME
done

printf "#endif\n" >> $FILENAME

