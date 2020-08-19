#pragma once
#include "Aphelion/Core/Core.h"

namespace ap
{
    class APHELION_API PhysicsDemoLayer : public Layer
    {
        virtual void OnAttach() override final {}
        virtual void OnDetach() override final {}
        virtual void OnEvent(Event& event) override final {}
        virtual void OnUpdate(Timestep ts) override final {}
        virtual void OnGuiRender() override final {}
    };
}