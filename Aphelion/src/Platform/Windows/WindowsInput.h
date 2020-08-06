#pragma once
#include "Aphelion/Core/Input/Input.h"

namespace ap
{
    class APHELION_API WindowsInput : public Input
    {
    private:
        virtual void EnableCursorImpl(bool enable) override final;

        virtual bool IsKeyPressedImpl(KeyCode code) override final;
        virtual bool IsButtonPressedImpl(ButtonCode code) override final;

        virtual void SetCursorPosImpl(const glm::vec2& pos) override final;
        virtual void SetCursorXImpl(float x) override final;
        virtual void SetCursorYImpl(float y) override final;

        virtual glm::vec2 GetCursorPosImpl() override final;
        virtual float GetCursorXImpl() override final;
        virtual float GetCursorYImpl() override final;
    };
}