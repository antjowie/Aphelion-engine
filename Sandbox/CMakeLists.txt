cmake_minimum_required(VERSION 3.1)

project(Sandbox VERSION 1.0
                LANGUAGES CXX
)

add_executable(Sandbox
    src/Sandbox.cpp    
)

target_link_libraries(Sandbox PUBLIC Shinobu)

# We don't need to copy a static lib to the executable directory
if(BUILD_SHARED_LIBS)
    add_custom_command(TARGET Sandbox 
                       POST_BUILD 
                       COMMAND ${CMAKE_COMMAND} -E copy
                       $<TARGET_FILE:Shinobu>
                       $<TARGET_FILE_DIR:Sandbox>
    )
endif()