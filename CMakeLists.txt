cmake_minimum_required(VERSION 3.16)

##############################
# Options
set(BUILD_SHARED_LIBS "OFF" CACHE BOOL "" FORCE)
if(NOT DEFINED CMAKE_BUILD_TYPE)
    message(WARNING "No build type specified. Assuming debug...")
endif()
set(CMAKE_BUILD_TYPE "Debug" CACHE STRING "")

# Set some global variables since these have to be consistent amongst all targets

# Normally you can set this during build time. But PhysX is build during configure timeout
# so we have to set it already
set(MSVC_EXTENSION ${CMAKE_BUILD_TYPE})
if(CMAKE_BUILD_TYPE STREQUAL RelWithDebInfo)
    set(MSVC_EXTENSION "")
endif()

set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded${MSVC_EXTENSION}")

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/output")
message(STATUS "CRT: ${CMAKE_MSVC_RUNTIME_LIBRARY}")

project(Application)

add_subdirectory(Aphelion)
add_subdirectory(Sandbox)
add_subdirectory(Demo/Physics)