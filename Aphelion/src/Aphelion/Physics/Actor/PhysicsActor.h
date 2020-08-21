#pragma once
#include "Aphelion/Core/Core.h"

namespace physx
{
    class PxActor;
}

namespace ap
{
    enum class APHELION_API PhysicsActorType
    {
        RigidStatic,
        RigidDynamic,
    };

    /**
     * A wrapper around the PhysX actors class
     * A abstract base class that represents an actor in the world
     */
    class APHELION_API PhysicsActor
    {
    public:
        virtual ~PhysicsActor() = default;

        virtual void SetWorldTransform(const glm::mat4& transform) = 0;
        virtual const glm::mat4& GetWorldTransform() const = 0;

        virtual physx::PxActor* GetHandle() = 0;
        virtual PhysicsActorType GetType() const = 0;
    };
}