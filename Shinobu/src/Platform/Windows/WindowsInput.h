#pragma once
#include "Shinobu/Core/Input/Input.h"

namespace sh
{
    class SHINOBU_API WindowsInput : public Input
    {
    private:
        virtual bool IsKeyPressedImpl(KeyCode code) override final;
        virtual bool IsButtonPressedImpl(ButtonCode code) override final;

        virtual glm::vec2 GetMousePosImpl() override final;
        virtual float GetMouseXImpl() override final;
        virtual float GetMouseYImpl() override final;
    };
}