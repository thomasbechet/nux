#!/bin/bash

ROOT_DIR=$1
if [[ ! -d $ROOT_DIR ]]; then
    echo "Please provide a valid root dir."
    exit 1
fi

find $ROOT_DIR/runtimes/ -type f -regex '.*\(h\|c\)' -exec clang-format -i {} \;
find $ROOT_DIR/examples/ -type f -regex '.*\(h\|c\)^' -exec clang-format -i {} \;
find $ROOT_DIR/cli/ -type f -regex '.*\(h\|c\)' -exec clang-format -i {} \;
