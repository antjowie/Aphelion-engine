#include "Aphelion/Renderer/OrthographicCamera.h"

#include <glm/gtc/matrix_transform.hpp>

namespace ap
{
    OrthographicCamera::OrthographicCamera(float left, float right, float bottom, float top)
        : m_dirtyFlag(true)
        , m_position(0)
        , m_degrees(0)
    {
        SetProjection(left, right, bottom, top);
    }

    void OrthographicCamera::SetProjection(float left, float right, float bottom, float top)
    {
        m_projectionMatrix = glm::ortho(left, right, bottom, top);
        m_dirtyFlag = true;
    }

    void OrthographicCamera::CalculateMatrices() const
    {
        if (!m_dirtyFlag) return;
        m_viewMatrix =
            glm::inverse(
            glm::translate(glm::mat4(1), m_position) * 
            glm::rotate(glm::mat4(1), glm::radians(m_degrees), glm::vec3(0, 0, 1)));

        m_viewProjectionMatrix = m_projectionMatrix * m_viewMatrix;
        m_dirtyFlag = false;
    }
}