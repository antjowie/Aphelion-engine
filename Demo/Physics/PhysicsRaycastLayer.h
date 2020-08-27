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

class APHELION_API PhysicsRaycastLayer : public ap::Layer
{
public:
    PhysicsRaycastLayer()
        : m_camera(ap::Radians(45.f), 16.f / 9.f)
        , m_scene({})
    {}

    virtual void OnAttach() override final
    {
        m_camera.GetCamera().transform.SetPosition(ap::Transform::GetWorldForward() * -10.f + ap::Transform::GetWorldUp() * 5.f);
        m_camera.GetCamera().transform.LookAt(glm::vec3(0));
        auto mat = ap::PhysicsMaterial(0.5f, 0.5f, 0.5f);

        //auto planeShape = ap::PhysicsShape(ap::PhysicsGeometry::CreatePlane());
        //auto plane = ap::RigidBody::CreateStatic(planeShape, glm::identity<glm::mat4>());
        //m_scene.AddActor(plane);

        auto shape = ap::PhysicsShape(ap::PhysicsGeometry::CreateBox(glm::vec3(0.5f)), mat);

        for (float i = -1; i < 2; i++)
        {
            const auto transform = glm::translate(glm::identity<glm::mat4>(), glm::vec3(float(i) * 1.5f, 0.f, 0.f));
            ap::RigidBody rb = ap::RigidBody::CreateStatic(shape, transform);
            m_scene.AddActor(rb);
        }
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
        m_camera.OnUpdate(ts);


        static auto cube = ap::CreateCube();
        static auto shader = ap::Shader::Create("res/shader/Texture3DFlat.glsl");
        static auto texture = ap::Texture2D::Create(1, 1);
        constexpr uint32_t color = 0xffffffff;
        texture->SetData(reinterpret_cast<const void*>(&color),sizeof(color));
        texture->Bind();

        m_scene.Simulate(ts);

        ap::Renderer::BeginScene(m_camera.GetCamera());
        for (const auto& actor : m_scene.GetActors())
        {
            auto bounds = actor.GetWorldBounds();
            ap::Renderer::Submit(shader, cube, actor.GetWorldTransform() * glm::scale(glm::mat4(1), bounds.GetDimensions()));
        }
        ap::Renderer::EndScene();
    }

    //virtual void OnGuiRender() override final;

private:
    ap::PerspectiveCameraController m_camera;
    ap::PhysicsScene m_scene;
};