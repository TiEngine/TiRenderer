﻿cmake_minimum_required(VERSION 3.10)
project(applications)

add_subdirectory(application)
if(MSVC)
  add_subdirectory(appdemo)
endif()
