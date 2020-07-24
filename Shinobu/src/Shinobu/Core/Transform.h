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
        Transform(
            const glm::vec3& pos   = glm::vec3(0.f), 
            const glm::vec3& euler = glm::vec3(0.f));
        
        static constexpr glm::vec3 GetWorldUp() { return glm::vec3(0, 1, 0); }
        static constexpr glm::vec3 GetWorldRight() { return glm::vec3(1, 0, 0); }
        static constexpr glm::vec3 GetWorldForward() { return glm::vec3(0, 0, 1); }

        glm::vec3 GetUp() const { return GetWorldMatrix()[1]; }
        glm::vec3 GetRight() const { return GetWorldMatrix()[0]; }
        glm::vec3 GetForward() const { return GetWorldMatrix()[2]; }

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

        friend bool operator==(const Transform&, const Transform&);
        template <typename S> friend void serialize(S& s, Transform& t);

    private:
        glm::vec3 m_position;
        glm::vec3 m_euler;
    };

    inline bool operator==(const Transform& lhs, const Transform& rhs)
    {
        return lhs.m_position == rhs.m_position && lhs.m_euler == rhs.m_euler;
    }

    template <typename S>
    void serialize(S& s, glm::vec3& v)
    {
        s.value4b(v.x);
        s.value4b(v.y);
        s.value4b(v.z);
    }

    template <typename S>
    void serialize(S& s, Transform& t)
    {
        serialize(s, t.m_position);
        serialize(s, t.m_euler);
    }

    inline glm::vec3& Degrees(glm::vec3& radians) { return radians = radians / glm::pi<float>() * 180.f; }
    inline glm::vec3& Radians(glm::vec3& degrees) { return degrees = degrees / 180.f * glm::pi<float>(); }
}
