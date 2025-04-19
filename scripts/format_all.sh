#!/bin/bash

ROOT_DIR=$1
if [[ ! -d $ROOT_DIR ]]; then
    echo "Please provide a valid root dir."
    exit 1
fi

find $ROOT_DIR/core -type f -regex '.*\(h\|c\)' -exec clang-format -i {} \;
find $ROOT_DIR/runtimes/native/cli -type f -regex '.*\(h\|c\)' -exec clang-format -i {} \;
find $ROOT_DIR/runtimes/native/sdk  -type f -regex '.*\(h\|c\)' -exec clang-format -i {} \;
find $ROOT_DIR/examples/ -type f -regex '.*\(h\|c\)^' -exec clang-format -i {} \;
find $ROOT_DIR/nuxlib/ -type f -regex '.*\(h\|c\)' -exec clang-format -i {} \;
find $ROOT_DIR/externals/nulib/ -type f -regex '.*\(h\|c\)' -exec clang-format -i {} \;
