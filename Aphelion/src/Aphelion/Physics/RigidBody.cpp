#include "Aphelion/Physics/RigidBody.h"
#include "Aphelion/Physics/PhysicsGLM.h"

namespace ap
{
    RigidBody RigidBody::CreateStatic(PhysicsGeometry& geometry, PhysicsMaterial& material, const glm::mat4& transform)
    {
        return RigidBody(
            physx::PxCreateStatic(
                PxGetPhysics(),
                physx::PxTransform(ap::MakeMat4(transform)),
                geometry.GetHandle().any(),
                *material.GetHandle()));
    }
    
    RigidBody RigidBody::CreateDynamic(PhysicsGeometry& geometry, PhysicsMaterial& material, float density, const glm::mat4& transform)
    {
        return RigidBody(
            physx::PxCreateDynamic(
                PxGetPhysics(),
                physx::PxTransform(ap::MakeMat4(transform)),
                geometry.GetHandle().any(),
                *material.GetHandle(),
                density));
    }

    RigidBody::RigidBody(physx::PxRigidActor* actor)
        : m_handle(actor)
        , m_rb(nullptr)
    {
        AP_CORE_ASSERT(m_handle, "Handle is not valid. Failed to create actor");
        switch (m_handle->getConcreteType())
        {
        case physx::PxConcreteType::eRIGID_STATIC:
            m_type = RigidBodyType::RigidStaticType;
            break;
        case physx::PxConcreteType::eRIGID_DYNAMIC:
            m_type = RigidBodyType::RigidDynamicType;
            m_rb = reinterpret_cast<physx::PxRigidBody*>(m_handle);
            break;
        }
    }
    
    void RigidBody::SetWorldTransform(const glm::mat4& transform)
    {
        m_handle->setGlobalPose(physx::PxTransform(ap::MakeMat4(transform)));
    }
    glm::mat4 RigidBody::GetWorldTransform() const
    {
        return ap::MakeMat4(physx::PxMat44(m_handle->getGlobalPose()));
    }
    
    void RigidBody::SetAngularDamping(float damping)
    {
        AP_CORE_ASSERT(m_rb, "Can't add forces to a static actor. Use a dynamic rigid body instead");
        m_rb->setAngularDamping(damping);
    }

    void RigidBody::SetLinearVelocity(const glm::vec3& velocity)
    {
        AP_CORE_ASSERT(m_rb, "Can't add forces to a static actor. Use a dynamic rigid body instead");
        m_rb->setLinearVelocity(MakeVec3(velocity));
    }

    physx::PxRigidActor* RigidBody::GetHandle()
    {
        return m_handle;
    }
    
    RigidBodyType RigidBody::GetType() const
    {
        return m_type;
    }
}