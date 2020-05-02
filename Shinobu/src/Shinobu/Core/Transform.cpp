#include "Shinobu/Core/Transform.h"

#include <glm/gtc/matrix_transform.hpp>

namespace sh
{
    sh::Transform::Transform()
        : m_position(0)
    {
    }

    glm::mat4 Transform::GetWorldMatrix() const
    {
        return glm::translate(glm::mat4(1),m_position);
    }
}