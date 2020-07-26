#pragma once
#include "Component/Component.h"
#include "Layer/Client.h"
#include "Primitives.h"

#include <Shinobu/Core/Application.h>
#include <Shinobu/Core/Input/Input.h>
#include <Shinobu/ECS/Scene.h>
#include <Shinobu/Event/NetEvent.h>
#include <Shinobu/Renderer/Renderer.h>

static float movespeed = 1.f;

//inline void InputSystem(sh::Scene& scene)
//{
//    auto& reg = scene.GetRegistry();
//    auto view = reg.Get().view<sh::Transform, Player>();
//    for (auto& e : view)
//    {
//        auto& t = view.get<sh::Transform>(e);
//        
//        const auto speed = movespeed;
//        glm::vec2 offset(0.f);
//        if (sh::Input::IsKeyPressed(sh::KeyCode::Up)) offset.y += speed * sh::Time::dt;
//        if (sh::Input::IsKeyPressed(sh::KeyCode::Left)) offset.x -= speed * sh::Time::dt;
//        if (sh::Input::IsKeyPressed(sh::KeyCode::Down)) offset.y -= speed * sh::Time::dt;
//        if (sh::Input::IsKeyPressed(sh::KeyCode::Right)) offset.x += speed * sh::Time::dt;
//        t.Move(glm::vec3(offset.x,offset.y,0.f));
//
//        auto packet = sh::Serialize(t, ClientLayer::LocalIDToNet(e));
//        sh::Application::Get().OnEvent(sh::ClientSendPacketEvent(packet));
//    }
//}

class InputSystem
{
public:
    InputSystem(std::reference_wrapper<sh::PerspectiveCamera> camera)
        : m_cam(std::move(camera))
    {
    }

    void operator() (sh::Scene& scene)
    {
        auto& reg = scene.GetRegistry();
        auto view = reg.Get().view<sh::Transform, Player>();

        for (auto& e : view)
        {
            auto& t = view.get<sh::Transform>(e);
            t = m_cam.get().transform;
            auto packet = sh::Serialize(t, ClientLayer::LocalIDToNet(e));
            sh::Application::Get().OnEvent(sh::ClientSendPacketEvent(packet));
        }
        sh::Renderer::EndScene();
    }

private:
    std::reference_wrapper<sh::PerspectiveCamera> m_cam;
};

inline void DeathSystem(sh::Scene& scene)
{
    auto& reg = scene.GetRegistry();
    auto view = reg.Get().view<Health>();
    for (auto& e : view)
    {
        auto& h = view.get<Health>(e);

        if(h.health <= 0)
            reg.Destroy(e);
    }
}

class DrawSystem
{
public:
    DrawSystem(std::reference_wrapper<sh::PerspectiveCamera> camera)
        : m_cam(std::move(camera))
    {
        m_shader = sh::Shader::Create("res/shader/Voxel.glsl");
        //m_texture = sh::Texture2D::Create("res/image.png");
        m_texture = sh::ArrayTexture2D::Create(2,2,"res/texture.png");

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
        auto vbo = sh::VertexBuffer::Create(
            vertices, sizeof(vertices));
        FillFaceVBOElements(vbo);

        const auto ebo = GenerateIndices<6>();

        m_vao = sh::VertexArray::Create();
        m_vao->AddVertexBuffer(vbo);
        m_vao->SetIndexBuffer(ebo);
    }

    void operator() (sh::Scene& scene)
    {
        auto& reg = scene.GetRegistry();
        auto view = reg.Get().view<sh::Transform, Sprite>();

        m_texture->Bind();

        // TODO: Move BeginScene to Layer
        sh::Renderer::BeginScene(m_cam);
        for (auto& e : view)
        {
            auto& t = view.get<sh::Transform>(e);
            //auto& sprite = view.get<Sprite>(e);
            sh::Renderer::Submit(m_shader,m_vao,t.GetWorldMatrix());

            // Tint player light blue
            //if (reg.Get().has<Player>(e)) data.color = glm::vec4(0.7f, 0.7f, 1.f, 1.f);
        }
        sh::Renderer::EndScene();
    }

private:

    std::reference_wrapper<sh::PerspectiveCamera> m_cam;
    sh::ShaderRef m_shader;
    sh::VertexArrayRef m_vao;
    sh::TextureRef m_texture;

};