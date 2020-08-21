#pragma once
#include "Aphelion/Core/Core.h"
#include "Aphelion/Physics/Actor/PhysicsActor.h"
#include "Aphelion/Physics/PhysicsGeometry.h"
#include "Aphelion/Physics/PhysicsMaterial.h"

namespace physx 
{ 
    class PxRigidDynamic; 
}

namespace ap
{
    /**
     * A wrapper around the PhysX actors class
     * A abstract base class that represents an actor in the world
     */
    class APHELION_API RigidDynamic : public PhysicsActor
    {
    public:
        RigidDynamic(PhysicsGeometry& geometry, PhysicsMaterial& material, float density, const glm::mat4& transform);
        RigidDynamic(physx::PxRigidDynamic* handle);
        virtual ~RigidDynamic() = default;

        virtual physx::PxRigidDynamic* GetHandle() { return m_handle; }
        virtual PhysicsActorType GetType() const { return PhysicsActorType::RigidDynamicType; }

        virtual void SetWorldTransform(const glm::mat4& transform) override final;
        virtual const glm::mat4& GetWorldTransform() const override final;

        void SetAngularDamping(float damping);
        void SetLinearVelocity(const glm::vec3& velocity);

    private:
        physx::PxRigidDynamic* m_handle;
    };
}