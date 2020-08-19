cmake_minimum_required(VERSION 3.16)

add_executable(
    PhysicsDemo
    
    Entry.cpp
    PhysicsDemoLayer.h
    PhysicsDemoLayer.cpp
)

target_link_libraries(
    PhysicsDemo
    PUBLIC
    Aphelion::Runtime
)