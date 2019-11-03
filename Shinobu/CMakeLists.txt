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

# Download submodules if needed
find_package(Git QUIET)
if(GIT_FOUND AND EXISTS "${CMAKE_SOURCE_DIR}/.git")
# Update submodules as needed
option(GIT_SUBMODULE "Check submodules during build" ON)
if(GIT_SUBMODULE)
message(STATUS "Submodule update")
execute_process(COMMAND ${GIT_EXECUTABLE} submodule update --init --recursive
WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
RESULT_VARIABLE GIT_SUBMOD_RESULT)
if(NOT GIT_SUBMOD_RESULT EQUAL "0")
message(FATAL_ERROR "git submodule update --init failed with ${GIT_SUBMOD_RESULT}, please checkout submodules")
endif()
endif()
endif()

if(NOT EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/thirdParty/spdlog/CMakeLists.txt")
message(FATAL_ERROR "The submodules were not downloaded! GIT_SUBMODULE was turned off or failed. Please update submodules and try again.")
endif()

# Since we are building spdlog from scratch, we should just include it
# I'm not sure if there is a better way of doing it
add_subdirectory(thirdParty/spdlog)
target_link_libraries(Shinobu PUBLIC spdlog)