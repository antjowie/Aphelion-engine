#pragma once
#include "PhysicsSystem.h"

#include <PxPhysicsAPI.h>

namespace ap
{
    using namespace physx;

    static PxFoundation* foundation = nullptr;
    static PxPhysics* physics = nullptr;

    static PxAllocatorCallback* allocatorCb;
    static PxErrorCallback* errorCb;

    class ErrorCbWrapper : public PxErrorCallback
    {
    public:
        ErrorCbWrapper(const PhysicsErrorLogCb& cb) : m_cb(cb) {}

        virtual void reportError(PxErrorCode::Enum code, const char* message, const char* file, int line) override final
        {
            m_cb(static_cast<PhysicsErrorCode>(code), message, file, line);
        }

    private:
        PhysicsErrorLogCb m_cb;
    };

    bool PhysicsSystem::Init(const PhysicsSystemDesc& desc)
    {
        desc.logCb ? errorCb = new ErrorCbWrapper(desc.logCb) : errorCb = new PxDefaultErrorCallback();
        foundation = PxCreateFoundation(PX_PHYSICS_VERSION, *allocatorCb, *errorCb);

        // Pvd support
        //gPvd = PxCreatePvd(*gFoundation);
        //PxPvdTransport* transport = PxDefaultPvdSocketTransportCreate(PVD_HOST, 5425, 10);
        //gPvd->connect(*transport, PxPvdInstrumentationFlag::eALL);

        physics = PxCreatePhysics(PX_PHYSICS_VERSION, *foundation, PxTolerancesScale()/*, true, gPvd*/);
        return foundation && physics;
    }

    void PhysicsSystem::Shutdown()
    {    
        physics->release();
        foundation->release();
    }

    //PhysicsScene PhysicsSystem::CreateScene()
    //{
    //    
    //}

}