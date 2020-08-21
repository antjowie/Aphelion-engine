#include "Aphelion/Physics/Actor/RigidDynamic.h"
#include "Aphelion/Physics/PhysicsGLM.h"

#include <PxRigidStatic.h>

namespace ap
{
    const glm::mat4& RigidDynamic::GetWorldTransform() const
    {
        return ap::MakeMat4(physx::PxMat44(m_handle->getGlobalPose()));
    }
}