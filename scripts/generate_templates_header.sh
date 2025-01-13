#!/bin/bash

TEMPLATES=sdk/templates
OUTPUT=sdk/templates_data.h

printf "#ifndef SDK_TEMPLATES_DATA_H\n" > $OUTPUT
printf "#define SDK_TEMPLATES_DATA_H\n" >> $OUTPUT

printf "#include <nulib.h>\n" >> $OUTPUT
printf "typedef struct
{const nu_char_t *path; const nu_byte_t *data; nu_size_t size; } sdk_template_file_t;\n" >> $OUTPUT

shopt -s dotglob # Iter hidden files

for TEMPLATE in $TEMPLATES/*; do
    LANG=$(basename $TEMPLATE)
    echo $LANG
    echo $TEMPLATE
    for FILE in $TEMPLATE/* $TEMPLATE/**/*; do
        echo $FILE
        if [ ! -f $FILE ]; then
            continue
        fi
        FILENAME=$(basename $FILE | tr '.' '_')
        printf "static const nu_byte_t template_${LANG}_${FILENAME}_data[] = {\n" >> $OUTPUT
        printf "$(xxd -i $FILE | head -n -2 | tail -n +2)" >> $OUTPUT
        printf "};\n" >> $OUTPUT
    done

    printf "static sdk_template_file_t template_${LANG}_files[] = {\n" >> $OUTPUT
    for FILE in $TEMPLATE/* $TEMPLATE/**/*; do
        if [ ! -f $FILE ]; then
            continue
        fi
        FILENAME=$(basename $FILE | tr '.' '_')
        SIZE=${#TEMPLATE}
        FILEPATH=$(echo $FILE | cut -c $(($SIZE + 2))-)
        FILEDATA="template_${LANG}_${FILENAME}_data"
        printf "{ .path=\"$FILEPATH\", .data=$FILEDATA, .size=sizeof($FILEDATA) },\n" >> $OUTPUT
    done
    printf "{ .path=NU_NULL, .data=NU_NULL, .size=0 }\n" >> $OUTPUT
    printf "};\n" >> $OUTPUT
done

printf "#endif\n" >> $OUTPUT

