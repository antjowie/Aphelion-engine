cmake_minimum_required(VERSION 3.16)

add_library(
    Core

    Input/KeyCodes.h
    Input/MouseCodes.h
    Input/Input.h
    Input/Input.cpp

    Application.cpp
    Application.h
    EntryPoint.h
    Core.h
    Layer.h
    EntryPoint.cpp
    LayerStack.cpp
    LayerStack.h
    Log.cpp
    Log.h
    PRNG.h
    Time.cpp
    Time.h
    Transform.cpp
    Transform.h
    Window.cpp
    Window.h

    # TODO: These bottom dependencies will need to be refactored
    ../Event/ApplicationEvent.h
    ../Event/Event.h
    ../Event/KeyEvent.h
    ../Event/MouseEvent.h
    ../Event/NetEvent.h

    # These depend on current platform so we should move them in the future
    ../../Platform/Windows/WindowsWindow.cpp
    ../../Platform/Windows/WindowsWindow.h
    ../../Platform/Windows/WindowsInput.cpp
    ../../Platform/Windows/WindowsInput.h

)
add_library(Aphelion::Core ALIAS Core)

target_precompile_headers(
    Core
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

        <glm/vec3.hpp>
        <glm/mat4x4.hpp>
        <glm/gtc/quaternion.hpp>
        <glm/gtc/matrix_transform.hpp>
        <glm/gtc/matrix_transform.hpp>
        <glm/gtc/type_ptr.hpp>
        <glm/gtx/quaternion.hpp>
        
        Log.h
)

target_include_directories(Core PUBLIC ../../)

target_link_libraries(    
    Core
    PUBLIC
    Glad::glad
    Enet::enet
    glfw
    Bitsery::bitsery
    glm::glm
    spdlog::spdlog
    ImGui::imgui
)

# Set up target definitions
if(BUILD_SHARED_LIBS)
    target_compile_definitions(Core PUBLIC AP_DYNAMIC_LINK)
    target_compile_definitions(Core PRIVATE AP_BUILD_DLL)
endif()

if (CMAKE_BUILD_TYPE STREQUAL "Debug")
    target_compile_definitions(Core PUBLIC AP_DEBUG)
endif()