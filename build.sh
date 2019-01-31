#!/usr/bin/env bash

echo "============ build ============="
rm -rf ./build
mkdir build
cd build
cmake ../ -DCMAKE_BUILD_TYPE=Debug -DCODE_COVERAGE=ON && make && make CTEST_OUTPUT_ON_FAILURE=1 test
