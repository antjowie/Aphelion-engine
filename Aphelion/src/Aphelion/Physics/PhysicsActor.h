#pragma once

// Note to future me
// Make Actor base class
// Make subclass for each actor type (rigid body, volume, trigger)
// Test api in physics demo

#include "Aphelion/Core/Core.h"

namespace ap
{
    class APHELION_API PhysicsActor
    {
        virtual void* GetHandle();
    };
}