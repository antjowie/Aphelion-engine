#include "Aphelion/Physics/PhysicsQuery.h"
#include "Aphelion/Physics/PhysicsGLM.h"

#include <PxPhysicsAPI.h>

namespace ap
{
    PhysicsRaycastHit::PhysicsRaycastHit(physx::PxRaycastHit& hit)
        : rb(hit.actor)
        , shape(hit.shape)
        , pos(MakeVec3(hit.position))
        , normal(MakeVec3(hit.normal))
        , distance(hit.distance)
        , isHit(true)
    {
    }
}