#pragma once
#include "Aphelion/Core/Core.h"

#include <foundation/PxBounds3.h>

namespace ap
{
    /**
     * A wrapper around PxBounds3, defines an aabb
     */
    class APHELION_API PhysicsBounds
    {
    public:
        enum Axis
        {
            AxisX,
            AxisY,
            AxisZ,
        };

    public:
        PhysicsBounds() = default;
        PhysicsBounds(const physx::PxBounds3& bounds);
        PhysicsBounds(const glm::vec3& min, const glm::vec3& max);

        glm::vec3 GetCenter() const;
        float GetCenter(Axis axis) const;
        /// returns the extents, which are half of the width/height/depth.
        glm::vec3 GetExtents() const;
        float GetExtents(Axis axis) const;
        /// returns the dimensions (width/height/depth) of this axis aligned box.
        glm::vec3 GetDimensions() const;

    private:
        physx::PxBounds3 m_bounds;
    };
}