#include "Aphelion/Physics/Actor/RigidStatic.h"
#include "Aphelion/Physics/PhysicsGLM.h"

#include <PxRigidStatic.h>

namespace ap
{
    RigidStatic::RigidStatic(PhysicsGeometry& geometry, PhysicsMaterial& material, const glm::mat4& transform)
        : m_handle(physx::PxCreateStatic(
            PxGetPhysics(),
            physx::PxTransform(ap::MakeMat4(transform)), 
            geometry.GetHandle().any(), 
            *material.GetHandle()))
    {
    }

    RigidStatic::RigidStatic(physx::PxRigidStatic* handle)
        : m_handle(handle)
    {
    }
    
    ap::RigidStatic::~RigidStatic()
    {
        m_handle->release();
    }

    void RigidStatic::SetWorldTransform(const glm::mat4& transform)
    {
        m_handle->setGlobalPose(physx::PxTransform(MakeMat4(transform)));
    }

    const glm::mat4& RigidStatic::GetWorldTransform() const
    {
        return ap::MakeMat4(physx::PxMat44(m_handle->getGlobalPose()));
    }
}