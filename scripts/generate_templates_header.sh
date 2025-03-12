#!/bin/bash

ROOT_DIR=$1
if [[ ! -d $ROOT_DIR ]]; then
    echo "Please provide a valid root dir."
    exit 1
fi

TEMPLATES=$ROOT_DIR/runtimes/native/data/templates
OUTPUT=$ROOT_DIR/runtimes/native/sdk/templates_data.c.inc

printf "#include <nulib/nulib.h>\n" > $OUTPUT
printf "typedef struct
{const nu_char_t *path; const nu_byte_t *data; nu_size_t size; } sdk_template_file_t;\n" >> $OUTPUT

shopt -s dotglob # Iter hidden files

for TEMPLATE in $TEMPLATES/*; do
    LANG=$(basename $TEMPLATE)
    echo "language: $LANG ($TEMPLATE)"
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
