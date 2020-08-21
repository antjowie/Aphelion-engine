cmake_minimum_required(VERSION 3.16)

################################
# Set up this project
project(
    Aphelion 
    VERSION 1.0
    LANGUAGES CXX
)

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
set(BUILD_TESTING "OFF" CACHE BOOL "" FORCE)
set(CMAKE_FOLDER External)

add_subdirectory(thirdParty/bitsery)
add_subdirectory(thirdParty/enet)
add_subdirectory(thirdParty/entt)
add_subdirectory(thirdParty/Glad)
set(GLFW_BUILD_DOCS OFF)
set(GLFW_INSTALL OFF)
add_subdirectory(thirdParty/glfw)
add_subdirectory(thirdParty/glm)
add_subdirectory(thirdParty/imgui)
set(SPDLOG_ENABLE_PCH ON)
include(../CMake/PhysXBuild.cmake)
BuildPhysX(${CMAKE_CURRENT_SOURCE_DIR}/thirdParty/PhysX OFF)
find_package(PhysX REQUIRED)
add_subdirectory(thirdParty/spdlog)
add_subdirectory(thirdParty/stb_image)

set(BUILD_SHARED_LIBS ${BUILD_SHARED_LIBS_OLD} CACHE BOOL "" FORCE)

################################
# Configure main project

set(CMAKE_FOLDER Aphelion)

add_subdirectory(src/Aphelion/Core)
add_subdirectory(src/Aphelion/Physics)

add_library(
    Runtime

    src/Aphelion/ECS/Component.h
    src/Aphelion/ECS/Entity.h
    src/Aphelion/ECS/Registry.cpp
    src/Aphelion/ECS/Registry.h
    src/Aphelion/ECS/Scene.cpp
    src/Aphelion/ECS/Scene.h
    
    src/Aphelion/ImGui/ImGuiBuild.cpp
    src/Aphelion/ImGui/ImGuiLayer.cpp
    src/Aphelion/ImGui/ImGuiLayer.h

    src/Aphelion/Net/Client.cpp
    src/Aphelion/Net/Client.h
    src/Aphelion/Net/ClientLayer.cpp
    src/Aphelion/Net/ClientLayer.h
    src/Aphelion/Net/Packet.cpp
    src/Aphelion/Net/Packet.h
    src/Aphelion/Net/Protocol.h
    src/Aphelion/Net/Server.cpp
    src/Aphelion/Net/Server.h
    src/Aphelion/Net/ServerLayer.cpp
    src/Aphelion/Net/ServerLayer.h

    src/Aphelion/Renderer/GraphicsContext.cpp
    src/Aphelion/Renderer/GraphicsContext.h
    src/Aphelion/Renderer/OrthographicCamera.cpp
    src/Aphelion/Renderer/OrthographicCamera.h
    src/Aphelion/Renderer/OrthographicCameraController.cpp
    src/Aphelion/Renderer/OrthographicCameraController.h
    src/Aphelion/Renderer/PerspectiveCamera.cpp
    src/Aphelion/Renderer/PerspectiveCamera.h
    src/Aphelion/Renderer/PerspectiveCameraController.cpp
    src/Aphelion/Renderer/PerspectiveCameraController.h
    src/Aphelion/Renderer/Primitive.cpp
    src/Aphelion/Renderer/Primitive.h
    src/Aphelion/Renderer/RenderCommand.cpp
    src/Aphelion/Renderer/RenderCommand.h
    src/Aphelion/Renderer/Renderer.cpp
    src/Aphelion/Renderer/Renderer.h
    src/Aphelion/Renderer/Renderer2D.cpp
    src/Aphelion/Renderer/Renderer2D.h
    src/Aphelion/Renderer/RendererAPI.cpp
    src/Aphelion/Renderer/RendererAPI.h
    src/Aphelion/Renderer/Shader.cpp
    src/Aphelion/Renderer/Shader.h
    src/Aphelion/Renderer/Texture.cpp
    src/Aphelion/Renderer/Texture.h
    src/Aphelion/Renderer/VertexArray.cpp
    src/Aphelion/Renderer/VertexArray.h
    src/Aphelion/Renderer/VertexBuffer.cpp
    src/Aphelion/Renderer/VertexBuffer.h

    # TODO: These depend on current platform so we should move them in the future
    src/Platform/OpenGL/OpenGLRendererAPI.cpp
    src/Platform/OpenGL/OpenGLRendererAPI.h
    src/Platform/OpenGL/OpenGLContext.cpp
    src/Platform/OpenGL/OpenGLContext.h
    src/Platform/OpenGL/OpenGLShader.cpp
    src/Platform/OpenGL/OpenGLShader.h
    src/Platform/OpenGL/OpenGLTexture.cpp
    src/Platform/OpenGL/OpenGLTexture.h
    src/Platform/OpenGL/OpenGLVertexArray.cpp
    src/Platform/OpenGL/OpenGLVertexArray.h
    src/Platform/OpenGL/OpenGLVertexBuffer.cpp
    src/Platform/OpenGL/OpenGLVertexBuffer.h
)

add_library(Aphelion::Runtime ALIAS Runtime)
# For some reason this causes an error where we need to use /FS
# target_precompile_headers(Runtime REUSE_FROM Aphelion::Core)
# https://gitlab.kitware.com/cmake/cmake/-/issues/19659
# Here is something mentioned about pdb copying (our issue)
get_target_property(CORE_PCH Aphelion::Core PRECOMPILE_HEADERS)
target_precompile_headers(
    Physics 
    PUBLIC
    ${CORE_PCH}
)

target_include_directories(Runtime PUBLIC src)

add_custom_command(
    TARGET Runtime
    POST_BUILD
    COMMAND ${CMAKE_COMMAND} 
    -E copy_directory
    ${CMAKE_CURRENT_SOURCE_DIR}/res/
    ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/res/
    COMMENT "Copying Aphelion resources to output dir"
)

target_link_libraries(
    Runtime
    PUBLIC 
    Aphelion::Core
    Aphelion::Physics
    Enet::enet
    EnTT::EnTT
    Glad::glad
    glfw
    ImGui::imgui
    stb_image::stb_image
)
