#pragma once
#include "Aphelion/Core/Core.h"
#include "PhysicsActor.h"

namespace ap
{
    /**
     * A wrapper around the PhysX actors class
     * A abstract base class that represents an actor in the world
     */
    class APHELION_API RigidDynamic : public PhysicsActor
    {
    public:
        virtual ~RigidDynamic() = default;

        virtual void* GetHandle() = 0;
        virtual PhysicsActorType GetType() const = 0;
    };
}