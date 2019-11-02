cmake_minimum_required(VERSION 3.1)

project(Shinobu VERSION 1.0
                LANGUAGES CXX
)

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

# Since we are building spdlog from scratch, we should just include it
# I'm not sure if there is a better way of doing it
add_subdirectory(thirdParty/spdlog)
target_link_libraries(Shinobu PRIVATE spdlog)