#pragma once
#include "Shinobu/Core/Core.h"

#include "Shinobu/Core/Input/KeyCodes.h"
#include "Shinobu/Core/Input/MouseCodes.h"

#include <glm/vec2.hpp>
#include <memory>

namespace sh
{
    class SHINOBU_API Input
    {
    public:
        static bool IsKeyPressed(KeyCode code) { return m_instance->IsKeyPressedImpl(code); }
        static bool IsButtonPressed(ButtonCode code) { return m_instance->IsButtonPressedImpl(code); }

        static glm::vec2 GetMousePos() { return m_instance->GetMousePosImpl(); }
        static float GetMouseX() { return m_instance->GetMouseXImpl(); }
        static float GetMouseY() { return m_instance->GetMouseYImpl(); }

    private:
        virtual bool IsKeyPressedImpl(KeyCode code) = 0;
        virtual bool IsButtonPressedImpl(ButtonCode code) = 0;

        virtual glm::vec2 GetMousePosImpl() = 0;
        virtual float GetMouseXImpl() = 0;
        virtual float GetMouseYImpl() = 0;

        static std::unique_ptr<Input> Create();

        static std::unique_ptr<Input> m_instance;
    };
}