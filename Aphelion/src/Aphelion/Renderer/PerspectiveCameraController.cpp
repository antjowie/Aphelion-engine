#include "Aphelion/Renderer/PerspectiveCameraController.h"

#include "Aphelion/Core/Event/MouseEvent.h"

#include "Aphelion/Core/Input/Input.h"
#include "Aphelion/Core/Input/KeyCodes.h"

#include "Aphelion/Core/Application.h"

namespace ap
{
    PerspectiveCameraController::PerspectiveCameraController(float fovYRadians, float aspectRatio, float zNear, float zFar)
        : m_camera(fovYRadians, aspectRatio,zNear,zFar)
        , m_isRotating(false)
    {
    }

    void PerspectiveCameraController::OnUpdate(Timestep ts)
    {
        glm::vec3 offset(0);
        const glm::vec3 forward(m_camera.transform.GetForward());
        const glm::vec3 right(m_camera.transform.GetRight());
        const glm::vec3 up(m_camera.transform.GetUp());

        // NOTE: I flipped the forward for the camera. This is because OpenGL by default 'looks' into the -z
        // direction. Since I reuse the transform class and don't want to modify too many things, I change 
        // the direction for the camera. This is something to keep in mind. It should not affect anything else
        // and we can continue on as we like to.
        if (Input::IsKeyPressed(KeyCode::W)) offset += -forward;
        if (Input::IsKeyPressed(KeyCode::A)) offset += -right;
        if (Input::IsKeyPressed(KeyCode::S)) offset += forward;
        if (Input::IsKeyPressed(KeyCode::D)) offset += right;
        if (Input::IsKeyPressed(KeyCode::E)) offset += up;
        if (Input::IsKeyPressed(KeyCode::Q)) offset += -up;

        if(Input::IsKeyPressed(KeyCode::LeftShift)) offset *= 10.f;

        m_camera.transform.SetPosition(m_camera.transform.GetPosition() + offset * ts.Seconds() * 5.f);
    }

    void PerspectiveCameraController::OnEvent(Event& e)
    {
        EventDispatcher d(e);
        
        static bool isRotating = false;
        static glm::vec2 prevPos(0);
        d.Dispatch<MouseButtonPressedEvent>([&](MouseButtonPressedEvent& e)
            {
                if (e.GetButton() == ButtonCode::Right)
                {
                    Input::EnableCursor(false);
                    isRotating = true;
                    prevPos = Input::GetCursorPos();
                }
                return false;
            });
        d.Dispatch<MouseButtonReleasedEvent>([&](MouseButtonReleasedEvent& e)
            {
                if (e.GetButton() == ButtonCode::Right)
                {
                    Input::EnableCursor(true);
                    isRotating = false;
                }
                return false;
            });

        if (isRotating)
        {
            d.Dispatch<MouseMovedEvent>([&](MouseMovedEvent& e)
                {
                    // TODO: Add a sensitivity variable for offset
                    /*const */auto offset = glm::vec2(e.GetX(), e.GetY()) - prevPos;
                    offset = offset / 25.f * glm::two_pi<float>();

                    prevPos = Input::GetCursorPos();
                    
                    m_camera.transform.Rotate(Radians(glm::vec3(-offset.y, -offset.x , 0)));
                    return false;
                });
        }
    }
}