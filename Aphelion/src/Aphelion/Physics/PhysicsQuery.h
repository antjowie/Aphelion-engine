#pragma once
#include "Aphelion/Core/Core.h"
#include "Aphelion/Physics/RigidBody.h"

namespace physx
{
    class PxRaycastHit;
}

namespace ap
{
    class RigidBody;
    class PhysicsShape;

    struct APHELION_API PhysicsRaycastHit
    {
        PhysicsRaycastHit(physx::PxRaycastHit& hit);
        PhysicsRaycastHit() = default;

        RigidBody rb;
        PhysicsShape shape;
        glm::vec3 pos{ 0 };
        glm::vec3 normal{ 0 };
        float distance{ 0 };
        bool isHit{ false };

        operator bool() const { return isHit; }
    };
}