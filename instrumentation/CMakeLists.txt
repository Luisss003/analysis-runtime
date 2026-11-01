cmake_minimum_required(VERSION 3.20)

project(HeapAnalysis LANGUAGES CXX)

find_package(LLVM REQUIRED CONFIG)

if(NOT LLVM_VERSION_MAJOR EQUAL 19)
    message(FATAL_ERROR "LLVM 19 required, found ${LLVM_PACKAGE_VERSION}")
endif()

message(STATUS "Found LLVM ${LLVM_PACKAGE_VERSION}")

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include_directories(SYSTEM ${LLVM_INCLUDE_DIRS})
add_definitions(${LLVM_DEFINITIONS})

add_library(CallScan MODULE CallScan.cpp)

if(NOT LLVM_ENABLE_RTTI)
    target_compile_options(CallScan PRIVATE -fno-rtti)
endif()