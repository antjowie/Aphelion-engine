#pragma once
#include "Aphelion/Core/Core.h"
#include "Aphelion/Physics/PhysicsGeometry.h"
#include "Aphelion/Physics/PhysicsMaterial.h"

namespace physx
{
    class PxShape;
}

namespace ap
{
    class RigidBody;

    /**
     * A wrapper around the PhysX shape
     */
    class APHELION_API PhysicsShape
    {
    public:
        PhysicsShape(PhysicsGeometry& geometry, PhysicsMaterial& material, const glm::mat4& offset = glm::identity<glm::mat4>());
        PhysicsShape(physx::PxShape* shape);
        PhysicsShape();
        ~PhysicsShape();

        void SetLocalTransform(const glm::mat4& transform);
        glm::mat4 GetLocalTransform() const;

        PhysicsGeometry GetGeometry() const;
        PhysicsMaterial GetMaterial() const;
        RigidBody GetRigidBody() const;
        physx::PxShape* GetHandle();
        
    private:
        physx::PxShape* m_handle;
        bool m_creator;
    };
}