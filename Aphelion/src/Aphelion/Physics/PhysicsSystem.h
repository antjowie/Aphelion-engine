#pragma once
#include "Aphelion/Core/Core.h"
#include "PhysicsError.h"
#include "PhysicsScene.h"

#include <functional>
namespace ap
{
    struct PhysicsSystemDesc
    {
        PhysicsErrorLogCb logCb = nullptr;
    };
    
    /**
     * A static system that sets up the physics systems
     */
    class APHELION_API PhysicsSystem
    {
    public:
        static bool Init(const PhysicsSystemDesc& desc);
        static void Shutdown();

        static PhysicsScene CreateScene();

    private:
        bool m_isInitialized;
    };
}