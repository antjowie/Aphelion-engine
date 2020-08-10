cmake_minimum_required(VERSION 3.1)

project (Glad VERSION 1.0
              LANGUAGES C)

add_library(Glad 
    include/glad/glad.h
    include/KHR/khrplatform.h
    src/glad.c
)

add_library(Glad::glad ALIAS Glad)

target_include_directories(Glad PUBLIC include)