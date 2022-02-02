cmake_minimum_required(VERSION 3.10)
project(appdemo)

file(GLOB_RECURSE SRC *.h *.cpp)
add_executable(appdemo ${SRC} ${INC})
#add_executable(appdemo WIN32 ${SRC} ${INC})
target_link_libraries(appdemo application backend)
#target_link_libraries(appdemo application backend renderer)

source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR}/../../.. FILES ${SRC} ${INC})
