cmake_minimum_required(VERSION 3.10)
project(backend)

file(GLOB SRC *.h *.cpp)
add_definitions(-DBackendModule)
add_library(backend SHARED ${SRC} ${INC})

source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR}/../.. FILES ${SRC} ${INC})

# backend implementation
add_subdirectory(cpu)
add_subdirectory(dx12)
add_subdirectory(vulkan)
