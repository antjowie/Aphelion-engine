#pragma once
#include "Aphelion/Core/Core.h"
#include "PhysicsError.h"
#include "PhysicsScene.h"

#include <functional>

#include <PxPhysicsAPI.h>

namespace ap
{
    struct PhysicsFoundationDesc
    {
        PhysicsErrorLogCb logCb = nullptr;
    };
    
    /**
     * The core of the physics system.
     * NOTE: Calling any physics functions before this system is initialized
     * will result into undefined behavior
     * 
     * It initialized the core systems that PhysX uses such as the foundation module, 
     * pvd module, etc. 
     *
     */
    class APHELION_API PhysicsFoundation
    {
    public:
        static bool Init(const PhysicsFoundationDesc& desc);
        static void Shutdown();

    private:
        bool m_isInitialized;
    };
}