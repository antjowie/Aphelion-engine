#include "PhysicsMaterial.h"

// This is already included by precompiled header
#include <PxPhysicsAPI.h>

namespace ap
{
    PhysicsMaterial::PhysicsMaterial(float staticFriction, float dynamicFriction, float restitution)
        : m_handle(PxGetPhysics().createMaterial(staticFriction, dynamicFriction, restitution))
        , m_creator(true)
    {
    }

    PhysicsMaterial::PhysicsMaterial(physx::PxMaterial* handle)
        : m_handle(handle)
        , m_creator(false)
    {
    }

    PhysicsMaterial::~PhysicsMaterial()
    {
        if(m_creator) m_handle->release();
    }

    physx::PxMaterial* PhysicsMaterial::GetHandle()
    {
        return m_handle;
    }
}
