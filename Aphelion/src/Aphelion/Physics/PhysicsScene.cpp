#include "Aphelion/Physics/PhysicsScene.h"
#include "Aphelion/Physics/PhysicsGLM.h"

#include <PxPhysicsAPI.h>

namespace ap
{
    /// This is created in the PhysicsSceneFactory::Init method
    static physx::PxCpuDispatcher* dispatcher = nullptr;
    static float maxStep = 1.f / 60.f;
    
    PhysicsScene::PhysicsScene(const PhysicsSceneDesc& desc)
        : m_desc(desc)
        , m_handle(nullptr)
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

    PhysicsScene::~PhysicsScene()
    {
        m_handle->release();
    }

    void PhysicsScene::AddActor(RigidBody& actor)
    {
        m_handle->addActor(*actor.GetHandle());
    }

    void PhysicsScene::RemoveActor(RigidBody& actor)
    {
        m_handle->removeActor(*actor.GetHandle());
        actor = {};
    }

    void PhysicsScene::AddAggregate(PhysicsAggregate& aggregate)
    {
        m_handle->addAggregate(*aggregate.GetHandle());
    }

    void PhysicsScene::RemoveAggregate(PhysicsAggregate& aggregate)
    {
        m_handle->removeAggregate(*aggregate.GetHandle());
    }

    void PhysicsScene::Simulate(float dt)
    {
        // Steps the simulation in steps if frame takes too long
        while (dt > maxStep)
        {
            AP_CORE_WARN("Stepping for {} frames...", int(dt / maxStep));
            m_handle->simulate(maxStep);
            m_handle->fetchResults(true);
            dt -= maxStep;
        }
        m_handle->simulate(dt);
        m_handle->fetchResults(true);
    }

    PhysicsRaycastHit PhysicsScene::Raycast(const glm::vec3& origin, const glm::vec3& dir, float distance)
    {
        AP_CORE_ASSERT(distance != 0, "distance may not be zero");
        physx::PxRaycastBuffer hitCb;
        m_handle->raycast(ap::MakeVec3(origin), ap::MakeVec3(dir), distance, hitCb);

        return hitCb.hasBlock ? PhysicsRaycastHit(hitCb.block) : PhysicsRaycastHit();
    }

    std::vector<RigidBody> PhysicsScene::GetActors(RigidBodyType mask) const
    {
        physx::PxActorTypeFlags pxMask = static_cast<physx::PxActorTypeFlag::Enum>(mask);

        unsigned nbActors = m_handle->getNbActors(pxMask);
        std::vector<physx::PxRigidActor*> actors(nbActors);
        m_handle->getActors(pxMask, reinterpret_cast<physx::PxActor**>(actors.data()), nbActors);

        std::vector<RigidBody> ret;
        ret.reserve(nbActors);
        for (auto& actor : actors)
        {
            ret.push_back(RigidBody(actor));
        }

        return ret;
    }

    std::vector<PhysicsAggregate> PhysicsScene::GetAggregates() const
    {
        unsigned nbAggregates = m_handle->getNbAggregates();
        std::vector<physx::PxAggregate*> aggregates(nbAggregates);
        m_handle->getAggregates(reinterpret_cast<physx::PxAggregate**>(aggregates.data()), nbAggregates);

        std::vector<PhysicsAggregate> ret;
        ret.reserve(nbAggregates);
        for (auto& aggregate : aggregates)
        {
            ret.push_back(PhysicsAggregate(aggregate));
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
        maxStep = desc.maxStep;
    }
    
    void PhysicsSceneFactory::Deinit()
    {
        AP_CORE_ASSERT(dispatcher, "PhysicsSceneFactory has never been initialized");
        delete dispatcher;
        dispatcher = nullptr;
    }
}