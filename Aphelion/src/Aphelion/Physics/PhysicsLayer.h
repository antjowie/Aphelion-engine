#pragma once
#include "Aphelion/Core/Core.h"
#include "Aphelion/Core/Layer.h"

namespace ap
{
    class APHELION_API PhysicsLayer : public Layer
    {
        //virtual ~Layer() = default;

        virtual void OnAttach() override final;
        virtual void OnDetach() override final;
        virtual void OnEvent(Event& event) override final;
        virtual void OnUpdate(Timestep ts) override final;
        virtual void OnGuiRender() override final;
    };
}