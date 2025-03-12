#!/bin/bash

ROOT_DIR=$1
if [[ ! -d $ROOT_DIR ]]; then
    echo "Please provide a valid root dir."
    exit 1
fi

echo "GENERATING FONTS HEADER"
python $ROOT_DIR/scripts/generate_fonts_header.py $ROOT_DIR
echo "GENERATING SYSCALL HEADER"
python $ROOT_DIR/scripts/generate_syscall_headers.py $ROOT_DIR
echo "GENERATING SHADERS HEADER"
bash $ROOT_DIR/scripts/generate_shaders_header.sh $ROOT_DIR
echo "GENERATING TEMPLATES HEADER"
bash $ROOT_DIR/scripts/generate_templates_header.sh $ROOT_DIR
echo "FORMATTING ALL"
bash $ROOT_DIR/scripts/format_all.sh $ROOT_DIR
