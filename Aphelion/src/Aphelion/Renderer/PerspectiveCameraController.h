#pragma once
#include "Aphelion/Core/Core.h"
#include "Aphelion/Core/Time.h"
#include "Aphelion/Event/Event.h"

#include "Aphelion/Renderer/PerspectiveCamera.h"

namespace ap
{
    class APHELION_API PerspectiveCameraController
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