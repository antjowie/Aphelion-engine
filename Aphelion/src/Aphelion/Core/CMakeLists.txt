cmake_minimum_required(VERSION 3.16)

# The Core is code that every module in our application needs
# These include math libraries, events, the layer stack but also
# configuration data such as export macros, log macros, etc

add_library(
    Core

    Event/ApplicationEvent.h
    Event/Event.h
    Event/KeyEvent.h
    Event/MouseEvent.h
    Event/NetEvent.h

    ImGui/ImGuiBuild.cpp
    ImGui/ImGuiLayer.cpp
    ImGui/ImGuiLayer.h

    Input/Input.cpp
    Input/Input.h
    Input/KeyCodes.h
    Input/MouseCodes.h

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

    # These depend on current platform and should be their own system
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
        <glm/gtx/hash.hpp>

        Log.h
)

target_include_directories(Core PUBLIC ../../)

target_link_libraries(    
    Core
    PUBLIC
    Enet::enet          # Net event includes Enet. Need to think up another way to 
    Bitsery::bitsery    # handle event system. Same for Bitsery
    glm::glm
    spdlog::spdlog
    ImGui::imgui

    # These are needed since the Core makes a window, but they should be refactored
    # to the renderer. Although this could become a bit odd when the Core will
    # need the renderer (which is probably why the runtime application should be
    # defined in the Runtime, but Runtime will depend on all systems). 
    # ...
    # I'll need to reconsider the design one day. For now these dependencies
    # are copied over in the Renderer script
    PRIVATE
    Glad::glad
    glfw

    # Temporary. When importing the header of the physics physx sdk is required. 
    # We should add a event callback for the ecs
    PhysX::PhysXFoundation
)

# Set up target definitions
if(BUILD_SHARED_LIBS)
    target_compile_definitions(Core PUBLIC AP_DYNAMIC_LINK)
    target_compile_definitions(Core PRIVATE AP_BUILD_DLL)
endif()

if (CMAKE_BUILD_TYPE STREQUAL "Debug")
    target_compile_definitions(Core PUBLIC AP_DEBUG)
endif()