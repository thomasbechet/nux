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

if [ -z $SDK ]; 
then
    echo "NUX_SDK is not defined"
    exit 1
fi

set -e

PROJECT=$(basename $1)
MAIN=$PROJECT.c

clang --target=wasm32 --no-standard-libraries -O3 -Wl,--strip-all -Wl,--export-all -Wl,--no-entry -o $PROJECT.wasm $DIR/$MAIN
