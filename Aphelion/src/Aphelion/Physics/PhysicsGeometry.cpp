#include "PhysicsGeometry.h"

namespace ap
{
    //void PhysicsGeometry::ToGeomTransform(glm::mat4& transform)
    //{
    //}

    //void PhysicsGeometry::FromGeomTransform(glm::mat4& transform)
    //{
    //}

    PhysicsGeometry PhysicsGeometry::CreatePlane()
    {
        //auto obj = PhysicsGeometry{ physx::PxPlaneGeometry() };
        //obj.m_isPlane = true;
        //return obj;
        return { physx::PxPlaneGeometry() };
    }

    PhysicsGeometry PhysicsGeometry::CreateSphere(float radius)
    {
        return { physx::PxSphereGeometry(radius) };
    }

    PhysicsGeometry PhysicsGeometry::CreateBox(const glm::vec3& halfSize)
    {
        // https://gameworksdocs.nvidia.com/PhysX/4.0/documentation/PhysXGuide/Manual/Geometry.html#boxes
        // TODO: Verify if x should be y in our case (to stay consistent with coordinate systems)
        return { physx::PxBoxGeometry(halfSize.x,halfSize.y,halfSize.z) };
    }
}