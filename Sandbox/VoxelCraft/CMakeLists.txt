cmake_minimum_required(VERSION 3.16)

project(VoxelCraft VERSION 1.0
                LANGUAGES CXX
)

add_executable(VoxelCraft
    src/Block/BlockLibrary.cpp
    src/Block/BlockLibrary.h
    src/Block/BlockType.cpp
    src/Block/BlockType.h
    src/Block/ChunkDataStructure.cpp
    src/Block/ChunkDataStructure.h

    src/Component/ChunkComponent.h
    src/Component/Component.h
    src/Component/ServerComponent.h

    src/Layer/Client.cpp
    src/Layer/Client.h
    src/Layer/MainMenu.h
    src/Layer/SceneStats.h
    src/Layer/Server.cpp
    src/Layer/Server.h

    src/System/ChunkSystem.cpp
    src/System/ChunkSystem.h
    src/System/PlayerSystem.h
    src/System/System.h

    src/Entry.cpp
    src/PacketBuffer.h
    src/Primitives.cpp
    src/Primitives.h
)

target_include_directories(VoxelCraft PUBLIC src)

add_custom_command(
    TARGET VoxelCraft
    POST_BUILD
    COMMAND ${CMAKE_COMMAND} 
    -E copy_directory
    ${CMAKE_CURRENT_SOURCE_DIR}/res/
    ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/res/
    COMMENT "Copying VoxelCraft resources to output dir"
)

target_link_libraries(VoxelCraft PUBLIC Aphelion::Runtime)