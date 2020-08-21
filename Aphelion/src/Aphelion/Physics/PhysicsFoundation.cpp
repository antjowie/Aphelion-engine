#pragma once
#include "Aphelion/Physics/PhysicsFoundation.h"
#include "Aphelion/Physics/PhysicsScene.h"

#include <PxPhysicsAPI.h>

namespace ap
{
    using namespace physx;

    static PxFoundation* foundation = nullptr;
    static PxPhysics* physics = nullptr;
    static PxPvd* pvd = nullptr;

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

    bool PhysicsFoundation::Init(const PhysicsFoundationDesc& desc)
    {
        AP_CORE_ASSERT(!foundation, "PhysicsFoundation is already initialized");

        allocatorCb = new PxDefaultAllocator();

        desc.logCb ? errorCb = new ErrorCbWrapper(desc.logCb) : errorCb = new PxDefaultErrorCallback();
        foundation = PxCreateFoundation(PX_PHYSICS_VERSION, *allocatorCb, *errorCb);

        // Pvd support
        pvd = PxCreatePvd(*foundation);
	    PxPvdTransport* transport = PxDefaultPvdSocketTransportCreate("127.0.0.1", 5425, 10);
	    pvd->connect(*transport, PxPvdInstrumentationFlag::eALL);

        physics = PxCreatePhysics(PX_PHYSICS_VERSION, *foundation, PxTolerancesScale()/*, true, gPvd*/);

        PhysicsSceneFactoryDesc sceneDesc;
        sceneDesc.cores = desc.cores;
        PhysicsSceneFactory::Init(sceneDesc);

        return foundation && pvd && physics;
    }

    void PhysicsFoundation::Deinit()
    {    
        AP_CORE_ASSERT(foundation, "PhysicsFoundation has not been initialized");
        
        PhysicsSceneFactory::Deinit();

        physics->release();
        pvd->release();
        foundation->release();

        delete allocatorCb;
        delete errorCb;

        physics = nullptr;
        pvd = nullptr;
        foundation = nullptr;

        allocatorCb = nullptr;
        errorCb = nullptr;
    }

    //PhysicsScene PhysicsFoundation::CreateScene()
    //{
    //    
    //}

}