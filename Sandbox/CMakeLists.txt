cmake_minimum_required(VERSION 3.16)

project(Sandbox VERSION 1.0
                LANGUAGES CXX
)

add_executable(Sandbox
    src/Sandbox.cpp    
)

# We don't need to copy a static lib to the executable directory
if(BUILD_SHARED_LIBS)
    add_custom_target(CopyDLL 
                      ALL COMMAND ${CMAKE_COMMAND} -E copy
                      $<TARGET_FILE:Shinobu>
                      $<TARGET_FILE_DIR:Sandbox>
                      COMMENT "Copying Shinobu.dll to output dir"
    )
endif()

target_link_libraries(Sandbox PUBLIC Shinobu)