cmake_minimum_required(VERSION 3.16)

##############################
# Options
set(BUILD_SHARED_LIBS "ON" CACHE BOOL "")
set(CMAKE_BUILD_TYPE "DEBUG" CACHE STRING "")

project(Application)

add_subdirectory(Shinobu)
add_subdirectory(Sandbox)