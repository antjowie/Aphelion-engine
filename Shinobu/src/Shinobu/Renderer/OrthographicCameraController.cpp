#include "Shinobu/Renderer/OrthographicCameraController.h"

#include "Shinobu/Event/ApplicationEvent.h"
#include "Shinobu/Event/MouseEvent.h"

#include "Shinobu/Core/Application.h"
#include "Shinobu/Core/Input/Input.h"

#include <GLFW/glfw3.h>

namespace sh
{
    OrthographicCameraController::OrthographicCameraController(float aspectRatio, float degrees)
        : m_aspectRatio(aspectRatio)
        , m_zoom(1)
        , m_camera(-m_aspectRatio * m_zoom, m_aspectRatio* m_zoom, -m_zoom, m_zoom)
    {
    }

    void OrthographicCameraController::OnUpdate(Timestep ts)
    {
        glm::vec2 offset(0);
        constexpr glm::vec2 right(1,0);
        constexpr glm::vec2 up(0, 1);

        if (Input::IsKeyPressed(KeyCode::W)) offset += up;
        if (Input::IsKeyPressed(KeyCode::A)) offset += -right;
        if (Input::IsKeyPressed(KeyCode::S)) offset += -up;
        if (Input::IsKeyPressed(KeyCode::D)) offset += right;

        m_camera.SetPosition(m_camera.GetPosition() + glm::vec3(offset,0.f) * ts.Seconds());

        float degrees = 0.f;
        constexpr float degOffset = 90.f;
        if (Input::IsKeyPressed(KeyCode::Q)) degrees += degOffset;
        if (Input::IsKeyPressed(KeyCode::E)) degrees -= degOffset;

        m_camera.SetRotation(m_camera.GetRotation() + degrees * ts);
    }

    void OrthographicCameraController::OnEvent(Event& e)
    {
        EventDispatcher d(e);

        if (d.Dispatch<WindowResizeEvent>([&](WindowResizeEvent& e)
            {
                m_aspectRatio = (float)e.GetWidth() / (float)e.GetHeight();
                UpdateProjection();
                return false;
            })) return;

        if (d.Dispatch<MouseScrolledEvent>([&](MouseScrolledEvent& e)
            {
                float zoom = GetZoom() - e.GetYOffset();
                SetZoom(glm::max(0.5f,zoom));
                return false;
            })) return;

    }

    void OrthographicCameraController::UpdateProjection()
    {
        m_camera.SetProjection(-m_aspectRatio * m_zoom, m_aspectRatio * m_zoom, -m_zoom, m_zoom);
    }

}