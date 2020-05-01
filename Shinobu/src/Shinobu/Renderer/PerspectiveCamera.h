#pragma once
#include "Shinobu/Core/Core.h"

#include <glm/vec3.hpp>
#include <glm/mat4x4.hpp>
#include <glm/gtc/quaternion.hpp>

namespace sh
{
    class SHINOBU_API PerspectiveCamera
    {
    public:
        PerspectiveCamera(float fovYRadians, float aspectRatio, float zNear= 0.1f, float zFar = 1000.f);

        void SetProjection(float fovYRadians, float aspectRatio, float zNear, float zFar);

        void SetPosition(const glm::vec3& pos) { m_position = pos; m_dirtyFlag = true; }
        void SetFOV(float fovYRadians) { m_fovY = fovYRadians; m_dirtyFlag = true; }
        void SetAspectRatio(float aspectRatio) { m_aspectRatio = aspectRatio; m_dirtyFlag = true; }
        void SetNear(float zNear) { m_near = zNear; m_dirtyFlag = true; }
        void SetFar(float zFar) { m_far= zFar; m_dirtyFlag = true; }
        
        const glm::vec3 GetPosition() const { return m_position; }
        float GetFOV() const { return m_fovY; }
        float GetAspectRatio() const { return m_aspectRatio; }
        float GetNear() const { return m_near; }
        float GetFar() const { return m_far; }

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

        glm::quat m_quat;
        glm::vec3 m_position;
        float m_fovY;
        float m_aspectRatio;
        float m_near;
        float m_far;
    };
}