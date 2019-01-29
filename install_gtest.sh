#!/usr/bin/env bash
echo "======== download gtest ========"
cd ./ext
git clone https://github.com/google/googletest.git

echo "========== test gtest =========="
cd ./googletest/googletest/make
make && ./sample1_unittest

echo "======== install gtest ========="
cd ../../ && mkdir build && cd ./build
cmake ../ && make && make install DESTDIR=./install
