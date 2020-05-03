#pragma once
#include "Shinobu/Core/Core.h"

#include <glm/vec3.hpp>
#include <glm/mat4x4.hpp>
#include <glm/gtc/quaternion.hpp>

namespace sh
{
    /**
     * All rotations are done in radians
     */
    class SHINOBU_API Transform
    {
    public:
        Transform();
        
        static constexpr glm::vec3 GetWorldUp() { return glm::vec3(0, 1, 0); }
        static constexpr glm::vec3 GetWorldRight() { return glm::vec3(1, 0, 0); }
        static constexpr glm::vec3 GetWorldForward() { return glm::vec3(0, 0, 1); }

        glm::vec3 GetUp() { return GetWorldMatrix()[1]; }
        glm::vec3 GetRight() { return GetWorldMatrix()[0]; }
        glm::vec3 GetForward() { return GetWorldMatrix()[2]; }

        void LookAt(const glm::vec3& target);
        void LookTowards(const glm::vec3& direction);

        void SetPosition(const glm::vec3& pos) { m_position = pos; }
        void Move(const glm::vec3& offset) { m_position += offset; }
        const glm::vec3& GetPosition() const { return m_position; }

        void SetRotation(const glm::quat& quat);
        void SetRotation(const glm::vec3& euler);
        void Rotate(const glm::quat& quat);
        void Rotate(const glm::vec3& euler);
        const glm::vec3& GetEulerRotation() const;

        glm::mat4 GetWorldMatrix() const;

    private:
        glm::vec3 m_position;
        glm::vec3 m_euler;
    };

    inline glm::vec3& Degrees(glm::vec3& radians) { return radians = radians / glm::pi<float>() * 180.f; }
    inline glm::vec3& Radians(glm::vec3& degrees) { return degrees = degrees / 180.f * glm::pi<float>(); }
}