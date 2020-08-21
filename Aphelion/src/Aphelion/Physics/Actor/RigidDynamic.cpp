#include "Aphelion/Physics/Actor/RigidDynamic.h"
#include "Aphelion/Physics/PhysicsGLM.h"

#include <PxRigidStatic.h>

namespace ap
{
    RigidDynamic::RigidDynamic(PhysicsGeometry& geometry, PhysicsMaterial& material, float density, const glm::mat4& transform)
        : m_handle(physx::PxCreateDynamic(
            PxGetPhysics(),
            physx::PxTransform(ap::MakeMat4(transform)), 
            geometry.GetHandle(), 
            *material.GetHandle(),
            density))
    {
    }
    
    RigidDynamic::RigidDynamic(physx::PxRigidDynamic* handle)
        : m_handle(handle)
    {
    }
    
    void RigidDynamic::SetWorldTransform(const glm::mat4& transform)
    {
        m_handle->setGlobalPose(physx::PxTransform(MakeMat4(transform)));
    }

    const glm::mat4& RigidDynamic::GetWorldTransform() const
    {
        return ap::MakeMat4(physx::PxMat44(m_handle->getGlobalPose()));
    }

    void RigidDynamic::SetAngularDamping(float damping)
    {
        m_handle->setAngularDamping(damping);
    }
    
    void RigidDynamic::SetLinearVelocity(const glm::vec3& velocity)
    {
        m_handle->setLinearVelocity(MakeVec3(velocity));
    }
}