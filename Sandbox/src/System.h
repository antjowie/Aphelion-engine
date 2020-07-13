#pragma once

#include "Component.h"

#include <Shinobu/ECS/Registry.h>
#include <Shinobu/Renderer/Renderer2D.h>
#include <Shinobu/Core/Input/Input.h>

void SpawnSystem(sh::Registry& reg)
{
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

void InputSystem(sh::Registry& reg)
{
    auto view = reg.Get().view<Transform, Player>();
    for (auto& e : view)
    {
        auto& t = view.get<Transform>(e);
        
        constexpr float speed = 0.5f;
        if (sh::Input::IsKeyPressed(sh::KeyCode::Up)) t.pos.y += speed * sh::Time::dt;
        if (sh::Input::IsKeyPressed(sh::KeyCode::Left)) t.pos.x -= speed * sh::Time::dt;
        if (sh::Input::IsKeyPressed(sh::KeyCode::Down)) t.pos.y -= speed * sh::Time::dt;
        if (sh::Input::IsKeyPressed(sh::KeyCode::Right)) t.pos.x += speed * sh::Time::dt;
    }
}

class DrawSystem
{
public:
    DrawSystem(sh::OrthographicCamera& ortho) : m_ortho(&ortho) {}

    void operator() (sh::Registry& reg)
    {
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

            sh::Renderer2D::Submit(data);
        }
        sh::Renderer2D::EndScene();
    }

private:
    // To gen implicit constructors
    // I think references cancel that since we don't know how to copy such thing
    sh::OrthographicCamera* m_ortho;
};

// Test stuff
// ------------------------------------------

/**
 * This system will propagate packets through the ECS
 * Although I might put it in its own layer and communicate
 * them via events. I'm not too sure yet
 *
 * Honestly, systems should NOT contain state so it seems like the most obvious solution
 * Buuut, ECS layer should then also propogate events to the ECS and I don't know yet how it will do that
 *
 * I have decided since this is a game engine that for the moment when using the Layer all packets are components.
 * Then the layer will call events for the packets. The GameLayer will listen to these. This way, the net code
 * is decoupled from the ECS code
 */
class NetworkSystem
{
public:
    void operator()(sh::Registry& reg)
    {
        sh::Packet p;
        if (m_server.IsHosting())
        {
            while (m_server.Poll(p))
            {
                auto data = sh::Deserialize<sh::ExampleData>(p);
                SH_CORE_TRACE("Server received: {}", data.message);

                // You prob don't ever wanna broadcast the ip of a connected person
                data.message = std::to_string(p.sender->address.host) + ": " + data.message;
                m_server.Broadcast(sh::Serialize(data));
            }
            m_server.Flush();
        }
        if (m_client.IsConnected())
        {
            while (m_client.Poll(p))
            {
                auto data = sh::Deserialize<sh::ExampleData>(p);
                SH_CORE_TRACE("Client received: {}", data.message);
            }
            m_client.Flush();
        }
    }

private:
    sh::Server m_server;
    sh::Client m_client;
};

void TestSystem(sh::Registry& reg)
{
    SH_TRACE("UPDATE dt: {}", sh::Time::dt);
}