#pragma once
#include "Component/Component.h"
#include "Layer/Client.h"
#include "Primitives.h"

#include <Aphelion/Core/Application.h>
#include <Aphelion/Core/Input/Input.h>
#include <Aphelion/ECS/Scene.h>
#include <Aphelion/Core/Event/NetEvent.h>
#include <Aphelion/Renderer/Renderer.h>

static float movespeed = 1.f;

//inline void InputSystem(ap::Scene& scene)
//{
//    auto& reg = scene.GetRegistry();
//    auto view = reg.Get().view<ap::TransformComponent, Player>();
//    for (auto& e : view)
//    {
//        auto& t = view.get<ap::TransformComponent>(e);
//        
//        const auto speed = movespeed;
//        glm::vec2 offset(0.f);
//        if (ap::Input::IsKeyPressed(ap::KeyCode::Up)) offset.y += speed * ap::Time::dt;
//        if (ap::Input::IsKeyPressed(ap::KeyCode::Left)) offset.x -= speed * ap::Time::dt;
//        if (ap::Input::IsKeyPressed(ap::KeyCode::Down)) offset.y -= speed * ap::Time::dt;
//        if (ap::Input::IsKeyPressed(ap::KeyCode::Right)) offset.x += speed * ap::Time::dt;
//        t.Move(glm::vec3(offset.x,offset.y,0.f));
//
//        auto packet = serialize(t, ClientLayer::LocalIDToNet(e));
//        ap::Application::Get().OnEvent(ap::ClientSendPacketEvent(packet));
//    }
//}

class InputSystem
{
public:
    InputSystem(std::reference_wrapper<ap::PerspectiveCamera> camera)
        : m_cam(std::move(camera)) {}

    void operator() (ap::Scene& scene)
    {
        auto& reg = scene.GetRegistry();

        reg.View<ap::TransformComponent, ap::RigidBodyComponent, Player, ap::GUIDComponent>(
            [&](ap::Entity e, ap::TransformComponent& t, ap::RigidBodyComponent& rb, Player&, ap::GUIDComponent& guid)
            {
                t.t = m_cam.get().transform;
                
                if (ap::Input::IsKeyPressed(ap::KeyCode::R))
                {
                    auto ball = reg.Create();
                    auto& rb = ball.AddComponent<ap::RigidBodyComponent>();
                    rb.CreateDynamic(10.f,t.t.GetWorldMatrix());
                    rb.GetRigidBody().AddShape(ap::PhysicsShape{ ap::PhysicsGeometry::CreateSphere(1.f), ap::PhysicsMaterial(1,1,1) });
                }

                return;

                //if (!rb.GetRigidBody()) return;

                //t.t = m_cam.get().transform;
                glm::vec2 offset{ 0 };
                if (ap::Input::IsKeyPressed(ap::KeyCode::W)) offset.y -= 1.f;
                if (ap::Input::IsKeyPressed(ap::KeyCode::A)) offset.x -= 1.f;
                if (ap::Input::IsKeyPressed(ap::KeyCode::S)) offset.y += 1.f;
                if (ap::Input::IsKeyPressed(ap::KeyCode::D)) offset.x += 1.f;

                if (ap::Input::IsKeyPressed(ap::KeyCode::Space)) rb.GetRigidBody().SetLinearVelocity(glm::vec3(0,10.f,0));


                //t.t.SetRotation(m_cam.get().transform.GetRotation());
                //auto newT = t.t;
                //newT.Move(glm::vec3(offset.x, 0, offset.y));
                //
                auto rbT = rb.GetRigidBody().GetWorldTransform();
                m_cam.get().transform.SetPosition(rbT[3]);

                auto packet = ap::Serialize(t, guid);
                ap::Application::Get().OnEvent(ap::ClientSendPacketEvent(packet));
            });
    }

private:
    std::reference_wrapper<ap::PerspectiveCamera> m_cam;
};

inline void DeathSystem(ap::Scene& scene)
{
    auto& reg = scene.GetRegistry();

    reg.View<Health>([&](ap::Entity e, Health& h)
        {
            // TODO: Very bad but only players have health so if they don't have a phyiscs component we add that here
            if (!e.HasComponent<ap::RigidBodyComponent>())
            {
                e.GetComponent<ap::TagComponent>().tag = "Player";
                auto& rb = e.AddComponent<ap::RigidBodyComponent>();
                auto& t = e.GetComponent<ap::TransformComponent>();

                rb.CreateDynamic(1.f, t.t.GetWorldMatrix());

                auto hb = ap::PhysicsGeometry::CreateBox(glm::vec3(0.3f, 0.8f, 0.3f));
                auto mat = ap::PhysicsMaterial(0.5f, 0.5f, 0.5f);
                rb.GetRigidBody().AddShape(ap::PhysicsShape(hb, mat));

                // TODO: Add axis locking to interface
                reinterpret_cast<physx::PxRigidDynamic*>(rb.GetRigidBody().GetHandle())
                    ->setRigidDynamicLockFlags(
                        physx::PxRigidDynamicLockFlag::eLOCK_LINEAR_Z | 
                        physx::PxRigidDynamicLockFlag::eLOCK_ANGULAR_X | 
                        physx::PxRigidDynamicLockFlag::eLOCK_ANGULAR_Z);
            }

            if(h.health <= 0)
                reg.Destroy(e);
        });
}

class DrawSystem
{
public:
    DrawSystem(std::reference_wrapper<ap::PerspectiveCamera> camera)
        : m_cam(std::move(camera))
    {
        m_shader = ap::Shader::Create("res/shader/Voxel.glsl");
        static const auto lightDir = glm::normalize(glm::vec3(0.1f, -1.f, -0.4f));
        m_shader->Bind();
        m_shader->SetVec3("aLightDir", glm::value_ptr(lightDir));
        m_shader->SetFloat("aAmbient", 0.5f);
        //m_texture = ap::Texture2D::Create("res/image.png");
        m_texture = ap::ArrayTexture2D::Create(2,2,"res/texture.png");

        // Create a block
        float vertices[faceAttributeCount * 4 * 6];
        unsigned offset = 0;
        for(int i = 0; i < 6; i++)
        {
            auto data = GenerateFaceVertices(FaceDir(i),0,0,0,i % 4);
            memcpy_s(
                &vertices[offset],
                faceAttributeCount * 4 * sizeof(float),
                data.data(),
                faceAttributeCount * 4 * sizeof(float));
            offset += faceAttributeCount * 4;
        }
        auto vbo = ap::VertexBuffer::Create(
            vertices, sizeof(vertices));
        FillFaceVBOElements(vbo);

        const auto ebo = GenerateIndices<6>();

        m_vao = ap::VertexArray::Create();
        m_vao->AddVertexBuffer(vbo);
        m_vao->SetIndexBuffer(ebo);
    }

    void operator() (ap::Scene& scene)
    {
        auto& reg = scene.GetRegistry();
        //auto view = reg.Get().view<ap::TransformComponent, Sprite>();

        m_texture->Bind();
            //m_shader->Bind();

        // TODO: Move BeginScene to Layer
        ap::Renderer::BeginScene(m_cam);
               
        reg.View<ap::TransformComponent, Sprite>(
            [&](ap::Entity e, ap::TransformComponent& t, Sprite& s)
        {
            ap::Renderer::Submit(m_shader,m_vao,t.t.GetWorldMatrix());

            // Tint player light blue
            //if (reg.Get().has<Player>(e)) data.color = glm::vec4(0.7f, 0.7f, 1.f, 1.f);
        });
        ap::Renderer::EndScene();
    }

private:
    std::reference_wrapper<ap::PerspectiveCamera> m_cam;
    ap::ShaderRef m_shader;
    ap::VertexArrayRef m_vao;
    ap::TextureRef m_texture;
};