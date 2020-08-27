#pragma once
#include "Aphelion/Core/Core.h"
#include "Aphelion/Core/Layer.h"
//#include "Aphelion/Core/Event/Event.h"

#include "Aphelion/Core/Application.h"
#include "Aphelion/Core/Input/KeyCodes.h"
#include "Aphelion/Core/Event/KeyEvent.h"

#include "Aphelion/Renderer/Primitive.h"
#include "Aphelion/Renderer/Renderer.h"
#include "Aphelion/Renderer/Texture.h"

#include "Aphelion/Physics/PhysicsFoundation.h"
#include "Aphelion/Physics/PhysicsShape.h"
#include "Aphelion/Physics/PhysicsScene.h"

#include "Aphelion/Renderer/PerspectiveCameraController.h"

class APHELION_API PhysicsDemoLayer : public ap::Layer
{
public:
    PhysicsDemoLayer();

    virtual void OnAttach() override final
    {
        m_scene = std::make_unique<ap::PhysicsScene>();

    }

    virtual void OnDetach() override final
    {

    }

    virtual void OnEvent(ap::Event& event) override final
    {
        m_camera.OnEvent(event);
        ap::EventDispatcher d(event);
        d.Dispatch<ap::KeyPressedEvent>(
            [&](ap::KeyPressedEvent& e)
            {
                if (e.GetKeyCode() == ap::KeyCode::Escape) ap::Application::Get().Exit();

                return false;
            });
    }

    virtual void OnUpdate(ap::Timestep ts) override final
    {

    }

    //virtual void OnGuiRender() override final;

private:
    ap::PerspectiveCameraController m_camera;

    std::unique_ptr<ap::PhysicsScene> m_scene;
};