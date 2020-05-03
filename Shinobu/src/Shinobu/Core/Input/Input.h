#pragma once
#include "Shinobu/Core/Core.h"

#include "Shinobu/Core/Input/KeyCodes.h"
#include "Shinobu/Core/Input/MouseCodes.h"

#include <glm/vec2.hpp>
#include <memory>

namespace sh
{
    /**
     * TODO: Cursor functions take a vec2 instead of a uvec2. This should be refactored
     */
    class SHINOBU_API Input
    {
    public:
        /**
         * When cursor is disabled, the cursor will be invisible and stay centered.
         * CursorMoveEvents will still contain the new cursor position
         * 
         * TODO: This maybe should be a window specific function
         */
        static void EnableCursor(bool enable) { return m_instance->EnableCursorImpl(enable); };

        static bool IsKeyPressed(KeyCode code) { return m_instance->IsKeyPressedImpl(code); }
        static bool IsButtonPressed(ButtonCode code) { return m_instance->IsButtonPressedImpl(code); }

        static void SetCursorPos(const glm::vec2& pos) { return m_instance->SetCursorPosImpl(pos); }
        static void SetCursorX(float x) { return m_instance->SetCursorXImpl(x); }
        static void SetCursorY(float y) { return m_instance->SetCursorYImpl(y); }

        /**
         * Returns the position in screen coordinates
         */
        static glm::vec2 GetCursorPos() { return m_instance->GetCursorPosImpl(); }
        static float GetCursorX() { return m_instance->GetCursorXImpl(); }
        static float GetCursorY() { return m_instance->GetCursorYImpl(); }

    private:
        virtual void EnableCursorImpl(bool enable) = 0;

        virtual bool IsKeyPressedImpl(KeyCode code) = 0;
        virtual bool IsButtonPressedImpl(ButtonCode code) = 0;

        virtual void SetCursorPosImpl(const glm::vec2& pos) = 0;
        virtual void SetCursorXImpl(float x) = 0;
        virtual void SetCursorYImpl(float y) = 0;

        virtual glm::vec2 GetCursorPosImpl() = 0;
        virtual float GetCursorXImpl() = 0;
        virtual float GetCursorYImpl() = 0;

        static std::unique_ptr<Input> Create();

        static std::unique_ptr<Input> m_instance;
    };
}