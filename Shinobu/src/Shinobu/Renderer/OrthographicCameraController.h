#pragma once
#include "Shinobu/Core/Core.h"
#include "Shinobu/Core/Timestep.h"
#include "Shinobu/Event/Event.h"

#include "Shinobu/Renderer/OrthographicCamera.h"

namespace sh
{
    class SHINOBU_API OrthographicCameraController
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