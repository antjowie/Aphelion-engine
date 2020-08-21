cmake_minimum_required(VERSION 3.16)

add_library(
    Physics

    PhysicsActor.h
    PhysicsActor.cpp
    PhysicsError.h
    PhysicsFoundation.h
    PhysicsFoundation.cpp
    PhysicsScene.h
    PhysicsScene.cpp
)

add_library(Aphelion::Physics ALIAS Physics)

# target_precompile_headers(Physics REUSE_FROM Aphelion::Core)
target_include_directories(Physics PUBLIC ../../)

# get_target_property(PXPROP PhysX::PhysXCommon BINARY_DIR)
# get_target_property(APPROP Physics BINARY_DIR)
# 
# message(WARNING "PXPROP: ${PXPROP}")
# message(WARNING "APPROP: ${APPROP}")

file(GLOB DLLS
  LIST_DIRECTORIES false
  "${CMAKE_BINARY_DIR}/physx/sdk_source_bin/*.dll"
)
# file(COPY ${MY_PUBLIC_HEADERS} DESTINATION myDestination)
# message(WARNING DIR ${CMAKE_BINARY_DIR})
# message(WARNING DLLS ${DLLS})
# add_custom_command(
#     Physics
#     POST_BUILD
# 
# )

target_link_libraries(
    Physics
    PUBLIC
    Aphelion::Core
    #PRIVATE temp while we test physics is initialized correctly
    #PRIVATE
    PhysX::PhysXExtensions
    PhysX::PhysXCommon
)