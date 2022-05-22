cmake_minimum_required(VERSION 3.10)
project(application)

file(GLOB_RECURSE SRC *.h *.cpp)
add_library(application STATIC ${SRC} ${INC})

source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR}/../../.. FILES ${SRC} ${INC})
