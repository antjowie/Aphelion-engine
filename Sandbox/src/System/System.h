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
//    auto view = reg.Get().view<ap::Transform, Player>();
//    for (auto& e : view)
//    {
//        auto& t = view.get<ap::Transform>(e);
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

        reg.View<ap::Transform, Player, ap::GUIDComponent>(
            [&](ap::Entity e, ap::Transform& t, Player&, ap::GUIDComponent& guid)
        {
            t = m_cam.get().transform;
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
        //auto view = reg.Get().view<ap::Transform, Sprite>();

        m_texture->Bind();

        // TODO: Move BeginScene to Layer
        ap::Renderer::BeginScene(m_cam);
               
        reg.View<ap::Transform, Sprite>(
            [&](ap::Entity e, ap::Transform& t, Sprite& s)
        {
            ap::Renderer::Submit(m_shader,m_vao,t.GetWorldMatrix());

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