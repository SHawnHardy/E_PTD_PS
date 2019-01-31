#!/usr/bin/env bash

# ===== Copyright =====
# @source https://github.com/codecov/example-cpp11-cmake/blob/master/.travis.yml
# @original author
#  - RokKos - RokKos
#  - Rolf Eike Beer - DerDakon
# @copyright MIT License
# have modified by SHawnHardy

cd build

echo TRAVIS_COMPILER : ${TRAVIS_COMPILER}
echo TRAVIS_OS_NAME : ${TRAVIS_OS_NAME}

if [ "$TRAVIS_COMPILER" == "clang" -a "$TRAVIS_OS_NAME" == "linux" ];then
    echo '#!/bin/bash
        exec llvm-cov gcov "$@"' > llvm-gcov.sh
    chmod +x ./llvm-gcov.sh
    lcov --directory . --base-directory . --gcov-tool ./llvm-gcov.sh --capture --output-file coverage.info
else
    # capture coverage info
    lcov --directory . --capture --output-file coverage.info
fi

# filter out system
lcov --remove coverage.info '/usr/*' '/usr/lib/*' --output-file coverage.info
# debug info
lcov --list coverage.info
