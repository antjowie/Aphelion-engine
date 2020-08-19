cmake_minimum_required(VERSION 3.16)

##############################
# Options
set(BUILD_SHARED_LIBS "OFF" CACHE BOOL "" FORCE)
set(CMAKE_BUILD_TYPE "DEBUG" CACHE STRING "")

# Set some global variables since these have to be consistent amongst all targets
set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/output")

project(Application)

add_subdirectory(Aphelion)
add_subdirectory(Sandbox)
add_subdirectory(Demo/Physics)