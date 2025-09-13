#!/bin/bash

ROOT_DIR=$1
if [[ ! -d $ROOT_DIR ]]; then
    echo "Please provide a valid root dir."
    exit 1
fi

echo "generating fonts..."
python $ROOT_DIR/scripts/generate_fonts.py $ROOT_DIR
echo "generating shaders..."
python $ROOT_DIR/scripts/generate_shaders.py $ROOT_DIR
echo "generating lua..."
python $ROOT_DIR/scripts/generate_lua.py $ROOT_DIR
echo "done."
