cmake_minimum_required(VERSION 3.10)
project(backend_dx12)

file(GLOB_RECURSE SRC *.h *.cpp)
add_library(backend_dx12 SHARED ${SRC} ${INC})
target_link_libraries(backend_dx12 d3dcompiler d3d12 dxgi)

source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR}/../../.. FILES ${SRC} ${INC})
