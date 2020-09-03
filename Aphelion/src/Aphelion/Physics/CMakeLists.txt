cmake_minimum_required(VERSION 3.16)

add_library(
    Physics

    PhysicsAggregate.h
    PhysicsAggregate.cpp
    PhysicsBounds.h
    PhysicsBounds.cpp
    PhysicsError.h
    PhysicsFoundation.cpp
    PhysicsFoundation.h
    PhysicsGeometry.cpp
    PhysicsGeometry.h
    PhysicsGLM.h
    PhysicsLayer.h
    PhysicsMaterial.cpp
    PhysicsMaterial.h
    PhysicsQuery.cpp
    PhysicsQuery.h
    PhysicsScene.cpp
    PhysicsScene.h
    PhysicsShape.cpp
    PhysicsShape.h
    RigidBody.cpp
    RigidBody.h
)

add_library(Aphelion::Physics ALIAS Physics)

# target_precompile_headers(Physics REUSE_FROM Aphelion::Core)
get_target_property(CORE_PCH Aphelion::Core PRECOMPILE_HEADERS)
target_precompile_headers(
    Physics
    PUBLIC
    ${CORE_PCH}

    <PxPhysicsAPI.h>
)

target_include_directories(Physics PUBLIC ../../)

target_link_libraries(
    Physics
    PUBLIC
    Aphelion::Core
    #PRIVATE temp while we test physics is initialized correctly
    # PRIVATE
    PhysX::PhysXExtensions
    PhysX::PhysXCommon
    PhysX::PhysXCooking
)