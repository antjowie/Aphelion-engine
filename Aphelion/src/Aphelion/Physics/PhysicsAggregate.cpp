#include "Aphelion/Physics/PhysicsAggregate.h"

namespace ap
{
    PhysicsAggregate PhysicsAggregate::Create(unsigned rigidBodyCount, bool selfCollide)
    {
        return PxGetPhysics().createAggregate(rigidBodyCount, selfCollide);
    }

    PhysicsAggregate::PhysicsAggregate()
        : m_handle(nullptr)
        , m_creator(false)
    {
    }

    PhysicsAggregate::PhysicsAggregate(physx::PxAggregate* handle)
        : m_handle(handle)
        , m_creator(true)
    {
    }

    void PhysicsAggregate::AddRigidBody(RigidBody& rb)
    {
        m_handle->addActor(*rb.GetHandle());
    }

    void PhysicsAggregate::RemoveRigidBody(RigidBody& rb)
    {
        m_handle->removeActor(*rb.GetHandle());
    }

    std::vector<RigidBody> PhysicsAggregate::GetRigidBodies()
    {
        auto count = m_handle->getNbActors();
        AP_CORE_ASSERT(false, "PhysicsAggregate::GetRigidBodies is not implemented");

        return std::vector<RigidBody>();
    }

    unsigned PhysicsAggregate::GetMaxRigidBodies() const
    {
        return m_handle->getMaxNbActors();
    }
}