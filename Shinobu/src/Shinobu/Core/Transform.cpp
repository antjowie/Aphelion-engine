#include "Shinobu/Core/Transform.h"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/quaternion.hpp>

namespace sh
{
    sh::Transform::Transform()
        : m_position(0)
        , m_quat(glm::identity<glm::quat>())
    {
    }

    glm::mat4 Transform::GetWorldMatrix() const
    {
        const glm::mat4 rotation = glm::toMat4(m_quat);
        const glm::mat4 translation = glm::translate(glm::mat4(1), m_position);

        return translation * rotation;
    }

    void Transform::SetRotation(const glm::quat& quat)
    {
        m_quat = quat;
    }

    void Transform::SetRotation(const glm::vec3& euler)
    {
        m_quat = glm::quat(euler);
    }

    void Transform::Rotate(const glm::quat& quat)
    {
        m_quat = quat * m_quat;
    }

    void Transform::Rotate(const glm::vec3& euler)
    {
        m_quat = glm::quat(euler) * m_quat;
    }
    
    glm::vec3 Transform::GetEulerRotation() const
    {
        return glm::eulerAngles(m_quat);
    }
}