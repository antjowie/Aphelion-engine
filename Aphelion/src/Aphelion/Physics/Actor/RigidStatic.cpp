#include "Aphelion/Physics/Actor/RigidStatic.h"
#include "Aphelion/Physics/PhysicsGLM.h"

#include <PxRigidStatic.h>

namespace ap
{
    RigidStatic::RigidStatic(PhysicsGeometry& geometry, PhysicsMaterial& material, const glm::mat4& transform)
    {
        m_handle = physx::PxCreateStatic(
            PxGetPhysics(),
            physx::PxTransform(ap::MakeMat4(transform)), 
            geometry.GetHandle(), 
            *material.GetHandle());
    }
    
    ap::RigidStatic::~RigidStatic()
    {
        m_handle->release();
    }

    const glm::mat4& RigidStatic::GetWorldTransform() const
    {
        return ap::MakeMat4(physx::PxMat44(m_handle->getGlobalPose()));
    }
}