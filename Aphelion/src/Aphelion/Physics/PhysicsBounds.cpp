#include "Aphelion/Physics/PhysicsBounds.h"
#include "Aphelion/Physics/PhysicsGLM.h"

namespace ap
{
    PhysicsBounds::PhysicsBounds(const physx::PxBounds3& bounds)
        : m_bounds(bounds)
    {
    }

    PhysicsBounds::PhysicsBounds(const glm::vec3& min, const glm::vec3& max)
        : m_bounds(MakeVec3(min),MakeVec3(max))
    {
    }
    
    glm::vec3 PhysicsBounds::GetCenter() const
    {
        return MakeVec3(m_bounds.getCenter());
    }

    float PhysicsBounds::GetCenter(Axis axis) const
    {
        return m_bounds.getCenter(axis);
    }

    glm::vec3 PhysicsBounds::GetExtents() const
    {
        return MakeVec3(m_bounds.getExtents());
    }

    float PhysicsBounds::GetExtents(Axis axis) const
    {
        return m_bounds.getExtents(axis);
    }

    glm::vec3 PhysicsBounds::GetDimensions() const
    {
        return MakeVec3(m_bounds.getDimensions());
    }
}
