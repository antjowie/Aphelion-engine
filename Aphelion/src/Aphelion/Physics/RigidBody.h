#pragma once
#include "Aphelion/Core/Core.h"
#include "Aphelion/Physics/PhysicsShape.h"
#include "Aphelion/Physics/PhysicsBounds.h"

namespace physx
{
    class PxRigidBody;
}

namespace ap
{
    enum RigidBodyType
    {
        RigidStaticType = Bit(0),
        RigidDynamicType = Bit(1),

        AllMask
    };

    /**
     * A wrapper around the PhysX actors class
     * 
     * Actors are described as rigid bodies. This is since every actor has a rigid body
     */
    class APHELION_API RigidBody
    {
    public:
        static RigidBody CreateStatic(PhysicsShape& shape, const glm::mat4& transform);
        static RigidBody CreateStatic(const glm::mat4& transform);
        static RigidBody CreateDynamic(PhysicsShape& shape, float density, const glm::mat4& transform);
        static RigidBody CreateDynamic(float density, const glm::mat4& transform);

    public:
        RigidBody(physx::PxRigidActor* actor, bool creator = false);
        RigidBody();

        void SetWorldTransform(const glm::mat4& transform);
        glm::mat4 GetWorldTransform() const;
        PhysicsBounds GetWorldBounds() const;

        void AddShape(PhysicsShape& shape);
        void RemoveShape(PhysicsShape& shape);
        std::vector<PhysicsShape> GetShapes() const;
        bool IsSleeping() const;

        /**
         * The following functions only apply on dynamic rigid bodies.
         * Will be ignored if trying to apply on static rigid bodies.
         */
        void SetAngularDamping(float damping);
        void SetLinearVelocity(const glm::vec3& velocity);

        /**
         * Handles and type identification
         */
        physx::PxRigidActor* GetHandle();
        RigidBodyType GetType() const;

        void SetUserData(void* data);
        void* GetUserData();

        bool Valid() const { return m_handle; }
        operator bool() const { return Valid(); }

    private:
        physx::PxRigidActor* m_handle;
        
        /**
         * This handle is only set if the handle type is a dynamic rb
         * It is used to save myself from having to reinterpret cast the whole time
         */
        physx::PxRigidBody* m_rb;
        RigidBodyType m_type;
    };
}