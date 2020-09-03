#pragma once
#include "Aphelion/Core/Core.h"
#include "Aphelion/Physics/RigidBody.h"

//#include <vector>

namespace physx
{
    class PxAggregate;
}

namespace ap
{
    /**
     * An aggregate can keep a collection of actors/rigid bodies. This is usefull for if you want to create
     * an actor with a lot of shapes, or have a bunch of actors make up one big actor.
     */
    class APHELION_API PhysicsAggregate
    {
    public:
        static PhysicsAggregate Create(unsigned rigidBodyCount, bool selfCollide);

    public:
        PhysicsAggregate();
        PhysicsAggregate(physx::PxAggregate* handle);

        /// You can't add a rigid body that is already added to a scene
        void AddRigidBody(RigidBody& rb);
        void RemoveRigidBody(RigidBody& rb);
        std::vector<RigidBody> GetRigidBodies();
        unsigned GetMaxRigidBodies() const;

        physx::PxAggregate* GetHandle() { return m_handle; }
        bool Valid() const { return m_handle; }
        operator bool() const { return Valid(); }

    private:
        physx::PxAggregate* m_handle;

        bool m_creator; 
    };
}