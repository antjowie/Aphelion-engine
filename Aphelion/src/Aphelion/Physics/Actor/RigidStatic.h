#pragma once
#include "Aphelion/Core/Core.h"
#include "Aphelion/Physics/Actor/PhysicsActor.h"
#include "Aphelion/Physics/PhysicsGeometry.h"
#include "Aphelion/Physics/PhysicsMaterial.h"

namespace physx 
{ 
    class PxRigidStatic; 
}

namespace ap
{
    /**
     * A wrapper around the PhysX actors class
     * A abstract base class that represents an actor in the world
     */
    class APHELION_API RigidStatic : public PhysicsActor
    {
    public:
        RigidStatic(PhysicsGeometry& geometry, PhysicsMaterial& material, const glm::mat4& transform);
        virtual ~RigidStatic();

        virtual physx::PxRigidStatic* GetHandle() override final { return m_handle; }
        virtual PhysicsActorType GetType() const override final { return PhysicsActorType::RigidStatic; }
        
        //virtual void SetWorldTransform(const glm::mat4& transform) override final;
        virtual const glm::mat4& GetWorldTransform() const override final;

    private:
        physx::PxRigidStatic* m_handle;
    };
}