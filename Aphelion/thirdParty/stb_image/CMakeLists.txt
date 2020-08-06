cmake_minimum_required(VERSION 3.1)

project (stb_image VERSION 1.0
              LANGUAGES C)

add_library(stb_image INTERFACE)

target_include_directories(stb_image INTERFACE include)