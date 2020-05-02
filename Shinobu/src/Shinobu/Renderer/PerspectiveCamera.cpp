#include "Shinobu/Renderer/PerspectiveCamera.h"

#include <glm/gtc/matrix_transform.hpp>

namespace sh
{
    PerspectiveCamera::PerspectiveCamera(float fovYRadians, float aspectRatio, float zNear, float zFar)
        : m_dirtyFlag(true)
        , m_position(0)
        , m_fovY(fovYRadians)
        , m_aspectRatio(aspectRatio)
        , m_near(zNear)
        , m_far(zFar)
    {
        SetProjection(fovYRadians, aspectRatio, zNear, zFar);
    }

    void PerspectiveCamera::SetProjection(float fovYRadians, float aspectRatio, float zNear, float zFar)
    {
        m_fovY = fovYRadians;
        m_aspectRatio = aspectRatio;
        m_near = zNear;
        m_far = zFar;

        m_dirtyFlag = true;
    }

    void PerspectiveCamera::CalculateMatrices() const
    {
        glm::quat quat;
        
        if (!m_dirtyFlag) return;

        m_projectionMatrix = glm::perspective(m_fovY, m_aspectRatio, m_near, m_far);

        m_viewMatrix = glm::inverse(transform.GetWorldMatrix());

        m_viewProjectionMatrix = m_projectionMatrix * m_viewMatrix;
        m_dirtyFlag = false;
    }
}