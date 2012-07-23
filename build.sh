#!/bin/bash

# Create a fresh 'build' directory tree
if [ -e build ]; then rm -rf build; fi
mkdir build
mkdir build/core
mkdir build/tests

# Build each file
# Do we want to use -m32 -std=c99 ?
gcc -c src/core/string.c -o build/core/string.o
gcc -c src/core/utils.c -o build/core/utils.o

# Build tests
gcc -c tests/test_string.c -o build/tests/test_string.o -Isrc -Ilib
gcc build/tests/test_string.o build/core/string.o build/core/utils.o -o bin/run_tests
