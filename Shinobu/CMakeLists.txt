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
    src/Shinobu/Core/Timestep.h
    src/Shinobu/Core/Window.h
    src/Shinobu/Core/Window.cpp

    src/Shinobu/Core/Input/KeyCodes.h
    src/Shinobu/Core/Input/MouseCodes.h

    src/Shinobu/Event/ApplicationEvent.h
    src/Shinobu/Event/Event.h
    src/Shinobu/Event/KeyEvent.h
    src/Shinobu/Event/MouseEvent.h

    src/Shinobu/ImGui/ImGuiBuild.cpp
    src/Shinobu/ImGui/ImGuiLayer.h
    src/Shinobu/ImGui/ImGuiLayer.cpp

    src/Shinobu/Renderer/GraphicsContext.h
    src/Shinobu/Renderer/GraphicsContext.cpp
    src/Shinobu/Renderer/RenderCommand.h
    src/Shinobu/Renderer/RenderCommand.cpp
    src/Shinobu/Renderer/Renderer.h
    src/Shinobu/Renderer/Renderer.cpp
    src/Shinobu/Renderer/Renderer2D.h
    src/Shinobu/Renderer/Renderer2D.cpp
    src/Shinobu/Renderer/RendererAPI.h
    src/Shinobu/Renderer/RendererAPI.cpp
    src/Shinobu/Renderer/Shader.h
    src/Shinobu/Renderer/Shader.cpp
    src/Shinobu/Renderer/Texture.h
    src/Shinobu/Renderer/Texture.cpp
    src/Shinobu/Renderer/VertexArray.h
    src/Shinobu/Renderer/VertexArray.cpp
    src/Shinobu/Renderer/VertexBuffer.h
    src/Shinobu/Renderer/VertexBuffer.cpp

    # These depend on current platform so we should move them in the future
    src/Platform/Windows/WindowsWindow.h
    src/Platform/Windows/WindowsWindow.cpp

    src/Platform/OpenGL/OpenGLRendererAPI.h
    src/Platform/OpenGL/OpenGLRendererAPI.cpp
    src/Platform/OpenGL/OpenGLContext.h
    src/Platform/OpenGL/OpenGLContext.cpp
    src/Platform/OpenGL/OpenGLShader.h
    src/Platform/OpenGL/OpenGLShader.cpp
    src/Platform/OpenGL/OpenGLTexture.h
    src/Platform/OpenGL/OpenGLTexture.cpp
    src/Platform/OpenGL/OpenGLVertexArray.h
    src/Platform/OpenGL/OpenGLVertexArray.cpp
    src/Platform/OpenGL/OpenGLVertexBuffer.h
    src/Platform/OpenGL/OpenGLVertexBuffer.cpp

    src/Shinobu/Renderer/OrthographicCamera.h
    src/Shinobu/Renderer/OrthographicCamera.cpp
    src/Shinobu/Renderer/OrthographicCameraController.h
    src/Shinobu/Renderer/OrthographicCameraController.cpp
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
        <type_traits>

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

if (CMAKE_BUILD_TYPE STREQUAL "Debug")
    target_compile_definitions(Shinobu PUBLIC SH_DEBUG)
endif ()

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
add_subdirectory(thirdParty/stb_image)
add_subdirectory(thirdParty/glm)

set(BUILD_SHARED_LIBS ${BUILD_SHARED_LIBS_OLD} CACHE BOOL "" FORCE)
  
target_link_libraries(Shinobu PUBLIC glm spdlog glfw imgui Glad stb_image)