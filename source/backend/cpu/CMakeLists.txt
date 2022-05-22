cmake_minimum_required(VERSION 3.10)
project(backend_cpu)

file(GLOB_RECURSE SRC *.h *.cpp)
add_library(backend_cpu SHARED ${SRC} ${INC})

source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR}/../../.. FILES ${SRC} ${INC})
