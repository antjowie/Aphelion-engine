#include "Aphelion/Physics/PhysicsScene.h"
#include "Aphelion/Physics/PhysicsGLM.h"

#include <PxPhysicsAPI.h>

namespace ap
{
    /// This is created in the PhysicsSceneFactory::Init method
    static physx::PxCpuDispatcher* dispatcher = nullptr;
    
    PhysicsScene::PhysicsScene(const PhysicsSceneDesc& desc)
    {
    	physx::PxSceneDesc sceneDesc(PxGetPhysics().getTolerancesScale());
	    sceneDesc.gravity = ap::MakeVec3(desc.gravity);
	    sceneDesc.cpuDispatcher = dispatcher;
	    sceneDesc.filterShader = physx::PxDefaultSimulationFilterShader;

        m_handle = PxGetPhysics().createScene(sceneDesc);
        auto* pvdClient = m_handle->getScenePvdClient();
	    if (pvdClient)
	    {
		    pvdClient->setScenePvdFlag(physx::PxPvdSceneFlag::eTRANSMIT_CONSTRAINTS, true);
		    pvdClient->setScenePvdFlag(physx::PxPvdSceneFlag::eTRANSMIT_CONTACTS, true);
		    pvdClient->setScenePvdFlag(physx::PxPvdSceneFlag::eTRANSMIT_SCENEQUERIES, true);
	    }
    }

    void PhysicsScene::AddActor(PhysicsActor& actor)
    {
        m_handle->addActor(*actor.GetHandle());
    }

    void PhysicsScene::Simulate(float dt)
    {
        m_handle->simulate(dt);
        m_handle->fetchResults(true);
    }

    std::vector<PhysicsActor*> PhysicsScene::GetActors(PhysicsActorType mask) const
    {
        physx::PxActorTypeFlags pxMask = static_cast<physx::PxActorTypeFlag::Enum>(mask);
        return {};
	 //   unsigned nbActors = m_handle->getNbActors(pxMask);
		//std::vector<PxRigidActor*> actors(nbActors);
		//scene->getActors(PxActorTypeFlag::eRIGID_DYNAMIC | PxActorTypeFlag::eRIGID_STATIC, reinterpret_cast<PxActor**>(&actors[0]), nbActors);
    }

    //////////////////////////////////////////////
    // Everything underneath is used internally //
    //////////////////////////////////////////////

    void PhysicsSceneFactory::Init(const PhysicsSceneFactoryDesc& desc)
    {
        AP_CORE_ASSERT(!dispatcher, "PhysicsSceneFactory is already initialized");
        dispatcher = physx::PxDefaultCpuDispatcherCreate(desc.cores);
    }
    
    void PhysicsSceneFactory::Deinit()
    {
        AP_CORE_ASSERT(dispatcher, "PhysicsSceneFactory has never been initialized");
        delete dispatcher;
        dispatcher = nullptr;
    }
}