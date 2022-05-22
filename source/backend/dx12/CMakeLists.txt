cmake_minimum_required(VERSION 3.10)
project(backend_dx12)

file(GLOB_RECURSE SRC *.h *.cpp)
add_library(backend_dx12 SHARED ${SRC} ${INC})
target_link_libraries(backend_dx12 d3dcompiler d3d12 dxgi dxguid)

add_custom_target(unzip_directx_headers ALL)
add_custom_command(TARGET unzip_directx_headers PRE_BUILD
  COMMENT "Unpacking DirectXHeaders."
  DEPENDS DirectX-Headers-1.4.9.zip
  WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/../../../3rd
    COMMAND ${CMAKE_COMMAND} -E remove_directory DirectXHeaders
    COMMAND ${CMAKE_COMMAND} -E tar xzf DirectX-Headers-1.4.9.zip
    COMMAND ${CMAKE_COMMAND} -E rename DirectX-Headers-1.4.9 DirectXHeaders
  VERBATIM)
add_dependencies(backend_dx12 unzip_directx_headers)

target_include_directories(backend_dx12 PRIVATE
  ${CMAKE_CURRENT_SOURCE_DIR}/../../../3rd/DirectXHeaders/include)

source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR}/../../.. FILES ${SRC} ${INC})
