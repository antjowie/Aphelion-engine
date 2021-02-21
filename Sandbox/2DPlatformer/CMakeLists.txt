cmake_minimum_required(VERSION 3.16)

project(2DPlatformer VERSION 1.0
                     LANGUAGES CXX
)

add_executable(2DPlatformer
    "src/Entry.cpp"
)

target_include_directories(2DPlatformer PUBLIC src)

# add_custom_command(
#     TARGET 2DPlatformer
#     POST_BUILD
#     COMMAND ${CMAKE_COMMAND} 
#     -E copy_directory
#     ${CMAKE_CURRENT_SOURCE_DIR}/res/
#     ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/res/
#     COMMENT "Copying 2DPlatformer resources to output dir"
# )

target_link_libraries(2DPlatformer PUBLIC
    Aphelion::Runtime
)