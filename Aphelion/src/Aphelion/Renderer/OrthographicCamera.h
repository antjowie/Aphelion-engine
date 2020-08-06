#pragma once
#include "Aphelion/Core/Core.h"

#include <glm/vec3.hpp>
#include <glm/mat4x4.hpp>

namespace ap
{
    class APHELION_API OrthographicCamera
    {
    public:
        OrthographicCamera(float left, float right, float bottom, float top);

        void SetProjection(float left, float right, float bottom, float top);

        void SetPosition(const glm::vec3& pos) { m_position = pos; m_dirtyFlag = true; }
        const glm::vec3 GetPosition() const { return m_position; }

        void SetRotation(float degrees) { m_degrees = degrees;  m_dirtyFlag = true; }
        float GetRotation() { return m_degrees; }

        const glm::mat4& GetProjectionMatrix() const { CalculateMatrices(); return m_projectionMatrix; }
        const glm::mat4& GetViewMatrix() const { CalculateMatrices(); return m_viewMatrix; }
        const glm::mat4& GetViewProjectionMatrix() const { CalculateMatrices(); return m_viewProjectionMatrix; }

    private:
        void CalculateMatrices() const;

        // We calculate the matrices only upon request and cache it
        mutable bool m_dirtyFlag;
        mutable glm::mat4 m_projectionMatrix;
        mutable glm::mat4 m_viewMatrix;
        mutable glm::mat4 m_viewProjectionMatrix;

        glm::vec3 m_position;
        float m_degrees;
    };
}