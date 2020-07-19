#pragma once

#include "Component.h"
#include "Sandbox.h"

#include <Shinobu/ECS/Scene.h>
#include <Shinobu/Renderer/Renderer2D.h>
#include <Shinobu/Core/Input/Input.h>


void SpawnSystem(sh::Scene& scene)
{
    auto& reg = scene.GetRegistry();
    auto view = reg.Get().view<SpawnEntity>();
    for (auto& e : view)
    {
        auto& c = view.get<SpawnEntity>(e);

        switch (c.type)
        {
        case SpawnEntity::Type::Player:
            SH_INFO("Spawned a player");
            reg.Get().emplace_or_replace<Transform>(e) = c.t;
            reg.Get().emplace_or_replace<Sprite>(e) = c.sprite;
            break;
        }

    }
    reg.Get().clear<SpawnEntity>();
}

void InputSystem(sh::Scene& scene)
{
    auto& reg = scene.GetRegistry();
    auto view = reg.Get().view<Transform, Player>();
    for (auto& e : view)
    {
        auto& t = view.get<Transform>(e);
        
        constexpr float speed = 0.5f;
        Transform newT = t;
        if (sh::Input::IsKeyPressed(sh::KeyCode::Up)) newT.pos.y += speed * sh::Time::dt;
        if (sh::Input::IsKeyPressed(sh::KeyCode::Left)) newT.pos.x -= speed * sh::Time::dt;
        if (sh::Input::IsKeyPressed(sh::KeyCode::Down)) newT.pos.y -= speed * sh::Time::dt;
        if (sh::Input::IsKeyPressed(sh::KeyCode::Right)) newT.pos.x += speed * sh::Time::dt;

        //t = newT;
        auto packet = sh::Serialize(newT, ClientLayer::LocalIDToNet(e));
        sh::Application::Get().OnEvent(sh::ClientSendPacketEvent(packet));
    }
}

class DrawSystem
{
public:
    DrawSystem(sh::OrthographicCamera& ortho) : m_ortho(&ortho) {}

    void operator() (sh::Scene& scene)
    {
        auto& reg = scene.GetRegistry();
        auto view = reg.Get().view<Transform, Sprite>();

        // TODO: Maybe make the engine begin and end scene
        // Since systems should not have functions (as a temp fix
        // the ECS now uses move operator instead of making a copy)
        sh::Renderer2D::BeginScene(*m_ortho);
        for (auto& e : view)
        {
            auto& t = view.get<Transform>(e);
            auto& sprite = view.get<Sprite>(e);


            sh::Render2DData data;
            data.pos = t.pos;
            data.texture = sprite.tex;
            data.size = glm::vec2(0.25f);

            // Tint player light blue
            if (reg.Get().has<Player>(e)) data.color = glm::vec4(0.7f, 0.7f, 1.f, 1.f);

            SH_TRACE("X:{}",t.pos.x);
            sh::Renderer2D::Submit(data);
        }
        sh::Renderer2D::EndScene();
    }

private:
    // To gen implicit constructors
    // I think references cancel that since we don't know how to copy such thing
    sh::OrthographicCamera* m_ortho;
};