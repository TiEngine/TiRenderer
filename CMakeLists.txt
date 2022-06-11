cmake_minimum_required(VERSION 3.10)
project(TiEngine)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

include_directories(include)
file(GLOB_RECURSE INC include/*)

add_subdirectory(source)
