#pragma once
#include "Aphelion/Core/Core.h"

namespace ap
{
    struct PhysicsSceneDesc
    {

    };
    /**
     * This system will be initialized by the PhysicsFoundation system 
     * 
     * Scene has it's own factory for the following reason:
     * I could add a ConfigureScene function to the foundation
     * but these two classes shouldn't rely on each other
     * So to keep the interface consistent. I gave the scene their own 
     * factory. This is because we need to construct a 
     * CpuDispatcher that is shared amongst all scenes. 
     *
     * This could be made in the foundation, 
     * but that would mean that scene would be created via the foundation
     * which would make the interface inconsistent.
     */
    class APHELION_API PhysicsSceneFactory
    {

    };

    /**
     * A scene for physics simulation
     * 
     * It holds all the actors in a scene and allows us to simulate the scene
     */
    class APHELION_API PhysicsScene
    {
    public:

    };
}