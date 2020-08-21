#include "PhysicsMaterial.h"

// This is already included by precompiled header
#include <PxPhysicsAPI.h>

namespace ap
{
    PhysicsMaterial::PhysicsMaterial(float staticFriction, float dynamicFriction, float restitution)
    {
        m_handle = PxGetPhysics().createMaterial(staticFriction,dynamicFriction,restitution);
    }

    physx::PxMaterial* PhysicsMaterial::GetHandle()
    {
        return m_handle;
    }
}
