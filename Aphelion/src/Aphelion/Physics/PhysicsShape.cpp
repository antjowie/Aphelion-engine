#include "Aphelion/Physics/PhysicsShape.h"
#include "Aphelion/Physics/RigidBody.h"
#include "Aphelion/Physics/PhysicsGLM.h"

namespace ap
{
    PhysicsShape::PhysicsShape(PhysicsGeometry& geometry, PhysicsMaterial& material, const glm::mat4& offset)
       : m_handle(PxGetPhysics().createShape(geometry.GetHandle().any(), *material.GetHandle()))
    {
    }

    PhysicsShape::PhysicsShape(physx::PxShape* shape)
        : m_handle(shape)
    {
    }

    void PhysicsShape::SetLocalTransform(const glm::mat4& transform)
    {
        m_handle->setLocalPose(physx::PxTransform(ap::MakeMat4(transform)));
    }

    glm::mat4 PhysicsShape::GetLocalTransform() const
    {
        return ap::MakeMat4(m_handle->getLocalPose());
    }
    
    PhysicsGeometry PhysicsShape::GetGeometry() const
    {
        return m_handle->getGeometry();
    }
    PhysicsMaterial PhysicsShape::GetMaterial() const
    {
        bool hasLogged = false;
        if (!hasLogged)
        {
            hasLogged = true;
            AP_CORE_WARN("PhysicsMaterial::GetMaterial only returns the first material... silencing message");
        }

        physx::PxMaterial* material = nullptr;
        m_handle->getMaterials(&material, 1, 0);
        return PhysicsMaterial(material);
    }
    
    RigidBody PhysicsShape::GetRigidBody() const
    {
        return m_handle->getActor();
    }

    physx::PxShape* PhysicsShape::GetHandle()
    {
        return m_handle;
    }
}