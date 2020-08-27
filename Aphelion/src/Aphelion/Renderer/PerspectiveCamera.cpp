#include "Aphelion/Renderer/PerspectiveCamera.h"

#include <glm/gtc/matrix_transform.hpp>

namespace ap
{
    PerspectiveCamera::PerspectiveCamera(float fovY, float aspectRatio, float zNear, float zFar)
        : m_dirtyFlag(true)
        , m_fovY(fovY)
        , m_aspectRatio(aspectRatio)
        , m_near(zNear)
        , m_far(zFar)
    {
        AP_CORE_ASSERT(fovY != 0, "FOV can't be zero");
        AP_CORE_ASSERT(aspectRatio != 0, "Aspect ratio can't be zero");
        SetProjection(fovY, aspectRatio, zNear, zFar);
    }

    void PerspectiveCamera::SetProjection(float fovY, float aspectRatio, float zNear, float zFar)
    {
        m_fovY = fovY;
        m_aspectRatio = aspectRatio;
        m_near = zNear;
        m_far = zFar;

        m_dirtyFlag = true;
    }

    void PerspectiveCamera::CalculateMatrices() const
    {   
        static Transform oldT = transform;

        if (!m_dirtyFlag && transform == oldT) return;

        m_projectionMatrix = glm::perspective(m_fovY, m_aspectRatio, m_near, m_far);

        m_viewMatrix = glm::inverse(transform.GetWorldMatrix());

        m_viewProjectionMatrix = m_projectionMatrix * m_viewMatrix;
        m_dirtyFlag = false;
        oldT = transform;
    }
}