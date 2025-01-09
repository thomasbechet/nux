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

TEMPLATES=devtools/templates/
FILENAME=devtools/project/templates_data.h

printf "#ifndef NUX_TEMPLATES_DATA_H\n" > $FILENAME
printf "#define NUX_TEMPLATES_DATA_H\n" >> $FILENAME

printf "#include <nulib.h>\n" >> $FILENAME

for lang in $TEMPLATES/*; do
    name="template_$(basename $file | tr '.' '_')" 
    printf "static struct {\n
        const nu_char_t *path;\n
        const nu_byte_t *data;\n
    } template_($lang)[] = {\n" >> $FILENAME
        { 
          .path="",
          .data=NU_NULL
        }
    printf "};\n" >> $FILENAME
    while IFS= read -r line; do
        # if [[ -z $line ]]; then
        #     continue 
        # fi
        printf "\"$line\\\n\"\n" >> $FILENAME
    done < $file
    printf ");\n" >> $FILENAME
    xxd -i Makefile | head -n -2 | tail -n +2
done

printf "#endif\n" >> $FILENAME

