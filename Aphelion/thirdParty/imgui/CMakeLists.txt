cmake_minimum_required(VERSION 3.1)

project (imgui VERSION 1.0
               LANGUAGES CXX
)

add_library(imgui STATIC
    imgui.cpp
    imgui_demo.cpp
    imgui_draw.cpp
    imgui_widgets.cpp

    # examples/imgui_impl_opengl3.cpp
    # examples/imgui_impl_glfw.cpp
)

target_include_directories(imgui PUBLIC /)