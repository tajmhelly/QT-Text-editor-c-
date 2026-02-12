#!/bin/bash
export LD_PRELOAD=/lib/x86_64-linux-gnu/libc.so.6
export LD_LIBRARY_PATH=/lib/x86_64-linux-gnu:/usr/lib/x86_64-linux-gnu:/usr/lib/x86_64-linux-gnu/qt6:$LD_LIBRARY_PATH
/tmp/TextEditor/build/bin/TextEditor "$@" 2>&1
