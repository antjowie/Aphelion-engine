cmake_minimum_required(VERSION 3.1)

project(Shinobu VERSION 1.0
                LANGUAGES CXX
)

# add_subdirectory(thirdParty/spdlog)

add_library(Shinobu
    src/Shinobu/Core/Core.h
    src/Shinobu/Core/Log.h
    src/Shinobu/Core/Log.cpp
)

if(BUILD_SHARED_LIBS)
    target_compile_definitions(Shinobu PUBLIC SH_DYNAMIC_LINK)
    target_compile_definitions(Shinobu PRIVATE SH_BUILD_DLL)
endif()

target_include_directories(Shinobu PUBLIC
    src
)