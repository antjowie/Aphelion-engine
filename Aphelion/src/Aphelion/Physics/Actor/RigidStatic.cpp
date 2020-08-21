#include "Aphelion/Physics/Actor/RigidStatic.h"
#include "Aphelion/Physics/PhysicsGLM.h"

#include "PxRigidStatic.h"

namespace ap
{
    RigidStatic::RigidStatic(PhysicsGeometry& geometry, PhysicsMaterial& material, const glm::mat4& transform)
    {
        m_handle = physx::PxCreateStatic(PxGetPhysics(),)
    }
    
    ap::RigidStatic::~RigidStatic()
    {
        m_handle->release();
    }
}