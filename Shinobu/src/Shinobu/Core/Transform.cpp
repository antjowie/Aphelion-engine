#include "Shinobu/Core/Transform.h"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/quaternion.hpp>

namespace sh
{
    sh::Transform::Transform()
        : m_position(0)
        , m_euler(0)
    {
    }

    void Transform::LookAt(const glm::vec3& target)
    {
        m_euler = glm::eulerAngles(glm::quatLookAt(glm::normalize(target - m_position), glm::vec3(0, 1, 0)));
    }

    glm::mat4 Transform::GetWorldMatrix() const
    {
        const glm::mat4 rotation = glm::toMat4(glm::quat(m_euler));
        const glm::mat4 translation = glm::translate(glm::mat4(1), m_position);

        return translation * rotation;
    }

    void Transform::SetRotation(const glm::quat& quat)
    {
        m_euler = glm::eulerAngles(quat);
    }

    void Transform::SetRotation(const glm::vec3& euler)
    {
        m_euler = euler;
    }

    void Transform::Rotate(const glm::quat& quat)
    {
        m_euler += glm::eulerAngles(quat);
    }

    void Transform::Rotate(const glm::vec3& euler)
    {
        m_euler = m_euler;
    }
    
    const glm::vec3& Transform::GetEulerRotation() const
    {
        return m_euler;
    }
}