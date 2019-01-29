cmake_minimum_required(VERSION 3.9)
project(E_PTD_PS C CXX)
set(CMAKE_CXX_STANDARD 11)

include_directories(./include)

enable_testing()
add_subdirectory(test)
