#!/usr/bin/env bash

echo "======== install gtest ========="
cd ./ext
git clone https://github.com/google/googletest.git
cd ./googletest && mkdir build && cd ./build
cmake ../ && make && make install DESTDIR=./install
