#pragma once
#include "Aphelion/Physics/PhysicsFoundation.h"
#include "Aphelion/Physics/PhysicsScene.h"

//#include <PxPhysicsAPI.h>

namespace ap
{
    using namespace physx;

    static PxFoundation* foundation = nullptr;
    static PxPhysics* physics = nullptr;
    static PxCooking* cooking = nullptr;
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
        AP_CORE_ASSERT(foundation, "PxCreateFoundation failed!");

        // Pvd support
        pvd = PxCreatePvd(*foundation);
        AP_CORE_ASSERT(pvd, "PxCreatePvd failed!");
        PxPvdTransport* transport = PxDefaultPvdSocketTransportCreate("127.0.0.1", 5425, 10);
	    pvd->connect(*transport, PxPvdInstrumentationFlag::eALL);

        physics = PxCreatePhysics(PX_PHYSICS_VERSION, *foundation, PxTolerancesScale(), true, pvd);
        AP_CORE_ASSERT(physics, "PxCreatePhysics failed!");

        PhysicsSceneFactoryDesc sceneDesc;
        sceneDesc.cores = desc.cores;
        PhysicsSceneFactory::Init(sceneDesc);

        cooking = PxCreateCooking(PX_PHYSICS_VERSION, *foundation, physics->getTolerancesScale());
        AP_CORE_ASSERT(cooking,"PxCreateCooking failed!");

        return foundation && pvd && physics && cooking;
    }

    void PhysicsFoundation::Deinit()
    {    
        AP_CORE_ASSERT(foundation, "PhysicsFoundation has not been initialized");
        
        PhysicsSceneFactory::Deinit();

        physics->release();
        pvd->release();
        foundation->release();
        cooking->release();

        delete allocatorCb;
        delete errorCb;

        physics = nullptr;
        pvd = nullptr;
        foundation = nullptr;
        cooking = nullptr;

        allocatorCb = nullptr;
        errorCb = nullptr;
    }
}