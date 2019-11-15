cmake_minimum_required(VERSION 3.16)

################################
# Set up this project
project(Shinobu VERSION 1.0
                LANGUAGES CXX
)

add_library(Shinobu
    src/Shinobu/Core/Application.h
    src/Shinobu/Core/Application.cpp
    src/Shinobu/Core/Core.h
    src/Shinobu/Core/EntryPoint.h
    src/Shinobu/Core/Layer.h
    src/Shinobu/Core/LayerStack.h
    src/Shinobu/Core/LayerStack.cpp
    src/Shinobu/Core/Log.h
    src/Shinobu/Core/Log.cpp
    src/Shinobu/Core/Window.h

    src/Shinobu/Event/ApplicationEvent.h
    src/Shinobu/Event/Event.h
    src/Shinobu/Event/KeyEvent.h
    src/Shinobu/Event/LayerEvent.h
    src/Shinobu/Event/MouseEvent.h

    src/Shinobu/ImGui/ImGuiBuild.cpp
    src/Shinobu/ImGui/ImGuiLayer.h
    src/Shinobu/ImGui/ImGuiLayer.cpp

    # These depend on current platform so we should move them in the future
    src/Shinobu/Platform/Windows/WindowsWindow.h
    src/Shinobu/Platform/Windows/WindowsWindow.cpp
)

target_precompile_headers(Shinobu 
    PUBLIC
        <iostream>
        <memory>
        <utility>
        <algorithm>
        <functional>
        <string>
        <sstream>
        <array>
        <vector>
        <unordered_map>
        <unordered_set>

        src/Shinobu/Core/Log.h
)

target_include_directories(Shinobu PUBLIC
    src
)

# Set up target definitions
if(BUILD_SHARED_LIBS)
    target_compile_definitions(Shinobu PUBLIC SH_DYNAMIC_LINK)
    target_compile_definitions(Shinobu PRIVATE SH_BUILD_DLL)
endif()

################################
# Download submodules if needed
find_package(Git QUIET)
if(GIT_FOUND AND EXISTS "${CMAKE_SOURCE_DIR}/.git")
    # Update submodules as needed
    option(GIT_SUBMODULE "Check submodules during build" ON)
    if(GIT_SUBMODULE)
        message(STATUS "Submodule update")
        execute_process(COMMAND 
                        ${GIT_EXECUTABLE} submodule update --init --recursive
                        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
                        RESULT_VARIABLE GIT_SUBMOD_RESULT)
        if(NOT GIT_SUBMOD_RESULT EQUAL "0")
            message(FATAL_ERROR "git submodule update --init failed with ${GIT_SUBMOD_RESULT}, please checkout submodules")
        else()
            message(STATUS "Got correct submodules, turning off automatic pull")
            set(GIT_SUBMODULE "OFF" CACHE BOOL "" FORCE)
        endif()
    endif()
endif()

# Verify if submodules are installed
if(NOT EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/thirdParty/spdlog/CMakeLists.txt")
    message(FATAL_ERROR "The submodules were not downloaded! GIT_SUBMODULE was turned off or failed. Please update submodules and try again.")
endif()

################################
# Build thirdparty libraries
# 
# Since we are building from source, we should just include it
# I'm not sure if there is a better way of doing it
set(BUILD_SHARED_LIBS_OLD ${BUILD_SHARED_LIBS})
set(BUILD_SHARED_LIBS "OFF" CACHE BOOL "" FORCE)

add_subdirectory(thirdParty/spdlog)
set(GLFW_BUILD_DOCS "OFF" CACHE BOOL "")
add_subdirectory(thirdParty/glfw)
add_subdirectory(thirdParty/imgui)
add_subdirectory(thirdParty/Glad)

set(BUILD_SHARED_LIBS ${BUILD_SHARED_LIBS_OLD} CACHE BOOL "" FORCE)
  
target_link_libraries(Shinobu PUBLIC spdlog glfw imgui Glad)