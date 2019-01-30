cmake_minimum_required(VERSION 3.9)
project(E_PTD_PS C CXX)
set(CMAKE_CXX_STANDARD 11)

include_directories(./include)

option(CODE_COVERAGE "This is a option for code coverage" ON)

if(CODE_COVERAGE STREQUAL "ON")
    set(CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/CMakeModules)
    include(CodeCoverage)
    APPEND_COVERAGE_COMPILER_FLAGS()
endif()

enable_testing()
add_subdirectory(test)
