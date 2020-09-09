#pragma once
#include "Aphelion/Core/Core.h"

#include <glm/vec3.hpp>
#include <glm/mat4x4.hpp>
#include <glm/gtc/quaternion.hpp>

/// Terrible dependency that should not exist
/// Serialization should be moved to its own system
#include <bitsery/bitsery.h>

namespace ap
{
    /**
     * All rotations are done in radians
     */
    class APHELION_API Transform
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

        bool operator==(const Transform& rhs) const { return m_position == rhs.m_position && m_euler == rhs.m_euler; }

        /// TODO: Find a way to access these for serialize purposes
        glm::vec3 m_position;
        glm::vec3 m_euler;
    private:
    };

    template <typename T> T Degrees(const T& radians) { return radians / glm::pi<float>() * 180.f; }
    template <typename T> T Radians(const T& degrees) { return degrees / 180.f * glm::pi<float>(); }
    template <typename T> T& Degrees(T& radians) { return radians = Degrees(radians)}
    template <typename T> T& Radians(T& degrees) { return degrees = Radians(degrees)}
}

namespace bitsery
{

/// TODO: Move these to a serialize system and figure out if we can wrap it
template <typename S, typename T>
void serialize(S& s, glm::vec<3,T>& v)
{
    s.value4b(v.x);
    s.value4b(v.y);
    s.value4b(v.z);
}

template <typename S>
void serialize(S& s, ap::Transform& t)
{
    serialize(s, t.m_position);
    serialize(s, t.m_euler);
}
}