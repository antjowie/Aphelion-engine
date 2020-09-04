#pragma once
#include "Aphelion/Core/Core.h"
#include "Aphelion/Physics/PhysicsError.h"
#include "Aphelion/Physics/PhysicsScene.h"

#include <functional>

namespace physx
{
    class APHELION_API PxCooking;
}

namespace ap
{
    struct PhysicsFoundationDesc
    {
        PhysicsErrorLogCb logCb = nullptr;
        unsigned cores = 1;
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
        static void Deinit();

        static physx::PxCooking& GetCooking();

    private:
        bool m_isInitialized;
    };
}