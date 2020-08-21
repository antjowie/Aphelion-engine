cmake_minimum_required(VERSION 3.16)

add_library(
    Physics

    Actor/PhysicsActor.h
    
    Actor/RigidDynamic.cpp
    Actor/RigidDynamic.h
    Actor/RigidStatic.cpp
    Actor/RigidStatic.h

    PhysicsError.h
    PhysicsFoundation.cpp
    PhysicsFoundation.h
    PhysicsGeometry.cpp
    PhysicsGeometry.h
    PhysicsGLM.h
    PhysicsMaterial.cpp
    PhysicsMaterial.h
    PhysicsScene.cpp
    PhysicsScene.h
)

add_library(Aphelion::Physics ALIAS Physics)

# target_precompile_headers(Physics REUSE_FROM Aphelion::Core)
target_include_directories(Physics PUBLIC ../../)

target_link_libraries(
    Physics
    PUBLIC
    Aphelion::Core
    #PRIVATE temp while we test physics is initialized correctly
    # PRIVATE
    PhysX::PhysXExtensions
    PhysX::PhysXCommon
)