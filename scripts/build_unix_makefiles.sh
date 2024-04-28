#!/bin/bash

if [ ! -d $(dirname "$0")/../build/unix_makefiles ]; then
    mkdir -p $(dirname "$0")/../build/unix_makefiles
fi

cd $(dirname "$0")/../build/unix_makefiles

make $1