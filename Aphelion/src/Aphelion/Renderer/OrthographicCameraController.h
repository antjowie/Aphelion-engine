#pragma once
#include "Aphelion/Core/Core.h"
#include "Aphelion/Core/Time.h"
#include "Aphelion/Event/Event.h"

#include "Aphelion/Renderer/OrthographicCamera.h"

namespace ap
{
    class APHELION_API OrthographicCameraController
    {
    public:
        OrthographicCameraController(float aspectRatio, float degrees = 0);

        void OnUpdate(Timestep ts);
        void OnEvent(Event& e);

        OrthographicCamera& GetCamera() { return m_camera; }
        const OrthographicCamera& GetCamera() const { return m_camera; }

        void SetZoom(float zoom) { m_zoom = zoom; UpdateProjection(); }
        float GetZoom() const { return m_zoom; }

    private:
        void UpdateProjection();

        float m_aspectRatio;
        float m_zoom;

        OrthographicCamera m_camera;
    };
}