#include "Aphelion/Physics/PhysicsScene.h"
#include "Aphelion/Physics/PhysicsGLM.h"
#include "Aphelion/Physics/Actor/RigidStatic.h"
#include "Aphelion/Physics/Actor/RigidDynamic.h"

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

    std::vector<std::unique_ptr<PhysicsActor>> PhysicsScene::GetActors(PhysicsActorType mask) const
    {
        physx::PxActorTypeFlags pxMask = static_cast<physx::PxActorTypeFlag::Enum>(mask);
        
	    unsigned nbActors = m_handle->getNbActors(pxMask);
		std::vector<physx::PxRigidActor*> actors(nbActors);
		m_handle->getActors(pxMask, reinterpret_cast<physx::PxActor**>(&actors[0]), nbActors);

		std::vector<std::unique_ptr<PhysicsActor>> ret(nbActors);
        for(auto& actor : actors)
        {
            switch (actor->getType())
            {
            case physx::PxActorType::Enum::eRIGID_STATIC:
                ret.push_back(std::make_unique<RigidStatic>(static_cast<physx::PxRigidStatic*>(actor)));
                break;

            case physx::PxActorType::Enum::eRIGID_DYNAMIC:
                ret.push_back(std::make_unique<RigidDynamic>(static_cast<physx::PxRigidDynamic*>(actor)));
                break;

            default:
                AP_CORE_ERROR("Physics actor type is not handled");
                break;
            }
        }

        return ret;
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