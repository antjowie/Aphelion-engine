#pragma once
#include "Aphelion/Core/Core.h"
#include "Aphelion/Core/Layer.h"


class APHELION_API PhysicsDemoLayer : public ap::Layer
{
    virtual void OnAttach() override final;
    virtual void OnDetach() override final;
    //virtual void OnEvent(Event& event) override final;
    virtual void OnUpdate(ap::Timestep ts) override final;
    //virtual void OnGuiRender() override final;
};
