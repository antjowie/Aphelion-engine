#pragma once
#include "Aphelion/Core/Core.h"
#include "Aphelion/Core/Layer.h"
//#include "Aphelion/Core/Event/Event.h"

#include "Aphelion/Renderer/PerspectiveCameraController.h"

class APHELION_API PhysicsDemoLayer : public ap::Layer
{
public:
    PhysicsDemoLayer();

    virtual void OnAttach() override final;
    virtual void OnDetach() override final;
    virtual void OnEvent(ap::Event& event) override final;
    virtual void OnUpdate(ap::Timestep ts) override final;
    //virtual void OnGuiRender() override final;

private:
    ap::PerspectiveCameraController m_camera;
};
