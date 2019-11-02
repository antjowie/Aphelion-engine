cmake_minimum_required(VERSION 3.1)

##############################
# Options
set(BUILD_SHARED_LIBS "ON" CACHE BOOL "")
set(CMAKE_BUILD_TYPE "DEBUG" CACHE STRING "")

project(ShinobuEngine VERSION 1.0
                      LANGUAGES CXX)

add_executable(Sandbox
    Sandbox/src/Sandbox.cpp    
)

add_library(Shinobu
    Shinobu/src/Shinobu/Core/Core.h
    Shinobu/src/Shinobu/Core/Log.h
    Shinobu/src/Shinobu/Core/Log.cpp
)

if(BUILD_SHARED_LIBS)
    target_compile_definitions(Shinobu PUBLIC SH_DYNAMIC_LINK)
    target_compile_definitions(Shinobu PRIVATE SH_BUILD_DLL)
endif()

target_include_directories(Shinobu PUBLIC
    Shinobu/src
)

target_link_libraries(Sandbox PUBLIC Shinobu)