cmake_minimum_required(VERSION 3.16)

project(Sandbox VERSION 1.0
                LANGUAGES CXX
)

add_executable(Sandbox
    src/Block/BlockLibrary.cpp
    src/Block/BlockLibrary.h
    src/Block/BlockType.cpp
    src/Block/BlockType.h

    src/Component/ChunkComponent.h
    src/Component/Component.h

    src/Layer/Client.cpp
    src/Layer/Client.h
    src/Layer/MainMenu.h
    src/Layer/SceneStats.h
    src/Layer/Server.cpp
    src/Layer/Server.h

    src/System/ChunkSystem.cpp
    src/System/ChunkSystem.h
    src/System/System.h

    src/Entry.cpp
    src/PacketBuffer.h
    src/Primitives.cpp
    src/Primitives.h
 
  "src/Component/ServerComponent.h")

target_include_directories(Sandbox PUBLIC src)

# We don't need to copy a static lib to the executable directory
# if(BUILD_SHARED_LIBS)
#     add_custom_target(CopyDLL 
#                         ALL COMMAND ${CMAKE_COMMAND} -E copy
#                         $<TARGET_FILE:Aphelion::Runtime>
#                         $<TARGET_FILE_DIR:Sandbox>
#                         COMMENT "Copying Aphelion.dll to output dir"
#     )
# endif()
# 
# if (CMAKE_BUILD_TYPE STREQUAL "Debug")
#     add_custom_target(CopyPDB
#                     ALL COMMAND ${CMAKE_COMMAND} -E copy
#                     $<TARGET_FILE_DIR:Aphelion::Runtime>/Runtime.pdb 
#                     $<TARGET_FILE_DIR:Sandbox>
#                     COMMENT "Copying Aphelion.pdb to output dir"
#     )
# endif ()

if (EXISTS ${CMAKE_SOURCE_DIR}/Aphelion/res)
    add_custom_target(CopyGameAssets
                    ALL COMMAND ${CMAKE_COMMAND} -E copy_directory
                    ${CMAKE_SOURCE_DIR}/Aphelion/res 
                    $<TARGET_FILE_DIR:Sandbox>/res
                    COMMENT "Copying Aphelion resources to output dir"
    )
endif()

if (EXISTS ${CMAKE_SOURCE_DIR}/Sandbox/res)
    add_custom_target(CopyEngineAssets
                    ALL COMMAND ${CMAKE_COMMAND} -E copy_directory
                    ${CMAKE_SOURCE_DIR}/Sandbox/res 
                    $<TARGET_FILE_DIR:Sandbox>/res
                    COMMENT "Copying Sandbox resources to output dir"
    )
endif()

target_link_libraries(Sandbox PUBLIC Aphelion::Runtime)