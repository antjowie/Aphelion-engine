#pragma once
#include "Aphelion/Core/Core.h"

namespace physx
{
    class PxMaterial;
}

namespace ap
{
    /**
     * A wrapper around a PhysX material object 
     */
    class APHELION_API PhysicsMaterial
    {
    public:
        PhysicsMaterial(float staticFriction, float dynamicFriction, float restitution);
        physx::PxMaterial* GetHandle(); 

    private:
        physx::PxMaterial* m_handle;
    };
}