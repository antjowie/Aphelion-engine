#pragma once
#include "Shinobu/Core/Core.h"
#include "Shinobu/Core/Time.h"
#include "Shinobu/Event/Event.h"

#include "Shinobu/Renderer/PerspectiveCamera.h"

namespace sh
{
    class SHINOBU_API PerspectiveCameraController
    {
    public:
        PerspectiveCameraController(float fovYRadians, float aspectRatio, float zNear = 0.1f, float zFar = 1000.f);

        void OnUpdate(Timestep ts);
        void OnEvent(Event& e);

        PerspectiveCamera& GetCamera() { return m_camera; }
        const PerspectiveCamera& GetCamera() const { return m_camera; }

    private:

        bool m_isRotating;
        PerspectiveCamera m_camera;
    };
}