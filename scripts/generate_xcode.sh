#!/bin/sh

if [ ! -d $(dirname "$0")/../build/xcode ]; then
    mkdir -p $(dirname "$0")/../build/xcode
fi

cmake -G "Xcode" -S $(dirname "$0")/.. -B $(dirname "$0")/../build/xcode