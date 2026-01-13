#!/bin/sh

gcc -O2 -std=c2x -o calc *.c -lm && strip calc
if test "$?" = "0"
then
    CMD=./calc ./regression-test.sh
fi
