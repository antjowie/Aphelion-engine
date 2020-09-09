#pragma once
#include "Aphelion/Core/Core.h"
#include "Aphelion/Core/Time.h"
#include "Aphelion/Core/Event/Event.h"

#include "Aphelion/Renderer/PerspectiveCamera.h"

namespace ap
{
    class APHELION_API PerspectiveCameraController
    {
    public:
        PerspectiveCameraController(float fovYRadians, float aspectRatio, float zNear = 0.1f, float zFar = 1000.f);
        ~PerspectiveCameraController() { Enable(false); }

        void OnUpdate(Timestep ts);
        void OnEvent(Event& e);

        // Whether to enable camera moving
        void Enable(bool enable);

        PerspectiveCamera& GetCamera() { return m_camera; }
        const PerspectiveCamera& GetCamera() const { return m_camera; }

    private:
        PerspectiveCamera m_camera;

        bool m_isRotating;
        glm::dvec2 m_oldCursorPos;
    };
}