cmake_minimum_required(VERSION 3.9)
project(E_PTD_PS C CXX)
set(CMAKE_CXX_STANDARD 11)

find_package(Threads REQUIRED)

enable_testing()
set(GTEST_ROOT ./ext/googletest/build/install/usr/local/)
find_package(GTest REQUIRED)
include_directories(${GTEST_INCLUDE_DIRS})

include_directories(./include)

add_subdirectory(test)
