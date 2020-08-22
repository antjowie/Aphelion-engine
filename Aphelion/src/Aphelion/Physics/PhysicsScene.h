#pragma once
#include "Aphelion/Core/Core.h"
#include "Aphelion/Physics/RigidBody.h"

namespace physx
{
    class PxScene;
}

namespace ap
{
    struct PhysicsSceneDesc
    {
        glm::vec3 gravity;
    };

    /**
     * A scene for physics simulation
     * 
     * It holds all the actors in a scene and allows us to simulate the scene
     */
    class APHELION_API PhysicsScene
    {
    public:
        PhysicsScene(const PhysicsSceneDesc& desc);
        ~PhysicsScene();

        void AddActor(RigidBody& actor);
        void Simulate(float dt);

        std::vector<RigidBody> GetActors(RigidBodyType mask) const;

    private:
        physx::PxScene* m_handle;
    };

    //////////////////////////////////////////////
    // Everything underneath is used internally //
    //////////////////////////////////////////////

    struct APHELION_API PhysicsSceneFactoryDesc
    {
        unsigned cores = 1; 
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
    public:
        static void Init(const PhysicsSceneFactoryDesc& desc);
        static void Deinit();
    };
}