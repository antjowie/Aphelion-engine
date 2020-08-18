cmake_minimum_required(VERSION 3.16)

add_library(
    Physics

    PhysicsLayer.h
    PhysicsLayer.cpp
)

add_library(Aphelion::Physics ALIAS Physics)

# target_precompile_headers(Physics REUSE_FROM Aphelion::Core)
target_include_directories(Physics PUBLIC ../../)
target_link_libraries(
    Physics
    PUBLIC
    Aphelion::Core
    PRIVATE
    PhysX::PhysXCommon
)