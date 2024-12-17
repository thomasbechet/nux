#!/bin/bash

function usage {
  echo "usage: $0 dir"
  exit 1
}

if [ $# -ne 1 ] || [ -z $1 ];
then
  usage 
  exit
fi

DIR=$1

if [ ! -d $DIR ];
then
    echo "$DIR is not a directory"
    exit 1
fi

SDK=$NUX_SDK

# if [ -z $SDK ]; 
# then
#     echo "NUX_SDK is not defined"
#     exit 1
# fi

set -e

PROJECT=$(basename $1)
MAIN=$PROJECT.c

clang --target=wasm32 -nostdlib -O0 -Wl,--strip-all -Wl,--export=start -z stack-size=8192 -Wl,--initial-memory=65536 -Wl,--no-entry -Wl,--export-all -Wl,--allow-undefined -o $PROJECT.wasm $DIR/$MAIN
