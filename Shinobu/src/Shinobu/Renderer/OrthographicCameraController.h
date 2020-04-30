#pragma once
#include "Shinobu/Core/Core.h"

#include "Shinobu/Renderer/OrthographicCamera.h"

namespace sh
{
    class SHINOBU_API OrthographicCameraController
    {
    public:
        OrthographicCameraController(float aspectRatio, float degrees = 0);

        void OnUpdate();
        void OnEvent();

        OrthographicCamera& GetCamera() { return m_camera; }
        const OrthographicCamera& GetCamera() const { return m_camera; }

    private:
        float m_aspectRatio;
        float m_zoom;

        OrthographicCamera m_camera;
    };
}