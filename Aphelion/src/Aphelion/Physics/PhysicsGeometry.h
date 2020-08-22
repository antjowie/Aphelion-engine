#pragma once
#include "Aphelion/Core/Core.h"

#include <PxPhysicsAPI.h>

namespace ap
{
    /**
     * A wrapper around a physx geometry object
     */
    class APHELION_API PhysicsGeometry
    {
    public:
        static PhysicsGeometry CreatePlane();
        static PhysicsGeometry CreateSphere(float radius);
        static PhysicsGeometry CreateBox(const glm::vec3& halfSize);

    public:
        PhysicsGeometry(physx::PxGeometryHolder& geometry) : m_handle(geometry) {};
        physx::PxGeometryHolder& GetHandle() { return m_handle; }

        /**
         * Some geometries need a special transform. To abstract this from the user
         * every transform passed to actors goes through these transformation functions
         */
        //void ToGeomTransform(glm::mat4& transform);
        //void FromGeomTransform(glm::mat4& transform);

    private:
        physx::PxGeometryHolder m_handle;
    };
}