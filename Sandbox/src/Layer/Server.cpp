#include "Server.h"
#include "System/System.h"
#include "SceneStats.h"
#include "Component/Component.h"
#include "Component/ServerComponent.h"

#include <Aphelion/Core/Application.h>
#include <Aphelion/Event/NetEvent.h>
#include <Aphelion/Net/Server.h>
#include <Aphelion/Net/ServerLayer.h>

#include "Component/ChunkComponent.h"
#include "System/ChunkSystem.h"

void ServerLayer::OnEvent(ap::Event& event)
{
    ap::EventDispatcher e(event);

    if (e.Dispatch<ap::ServerClientConnectEvent>([&](ap::ServerClientConnectEvent& e)
        {
            auto& reg = m_scene.GetRegistry();
            auto& app = ap::Application::Get();

            char ip[64];
            enet_address_get_host_ip(&e.GetPeer()->address, ip, 64);

            // Send all existing users to that player
            auto view = reg.Get().view<ap::Transform, Sprite, Health>();
            for (auto ent : view)
            {
                auto& t = reg.Get().get<ap::Transform>(ent);
                auto& s = reg.Get().get<Sprite>(ent);
                auto& h = reg.Get().get<Health>(ent);

                app.OnEvent(ap::ServerSendPacketEvent(ap::Serialize(t, ent, true), e.GetPeer()));
                app.OnEvent(ap::ServerSendPacketEvent(ap::Serialize(s, ent, true), e.GetPeer()));
                app.OnEvent(ap::ServerSendPacketEvent(ap::Serialize(h, ent, true), e.GetPeer()));
            }

            // Create the new player player
            auto entity = reg.Create();
            auto& t = reg.Get().emplace<ap::Transform>(entity);
            auto& s = reg.Get().emplace<Sprite>(entity);
            s.image = "res/image.png";
            s.LoadTexture();
            auto& h = reg.Get().emplace<Health>(entity);
            h.health = 1;

            app.OnEvent(ap::ServerBroadcastPacketEvent(ap::Serialize(t, entity,true)));
            app.OnEvent(ap::ServerBroadcastPacketEvent(ap::Serialize(s, entity,true)));
            app.OnEvent(ap::ServerBroadcastPacketEvent(ap::Serialize(h,entity,true)));
            // Tell the new user that they own that player
            app.OnEvent(ap::ServerSendPacketEvent( 
                ap::Serialize(Player(), entity, true), e.GetPeer()));

            m_players[e.GetPeer()] = entity;
            return true;
        })) return;

    if (e.Dispatch<ap::ServerClientDisconnectEvent>([&](ap::ServerClientDisconnectEvent& e)
        {
            char ip[64];
            enet_address_get_host_ip(&e.GetPeer()->address, ip, 64);
            SH_INFO("Server closed connection with {}", ip);

            // When player leaves we broadcast that their HP is zero
            auto entity = m_players.at(e.GetPeer());
            auto& h = m_scene.GetRegistry().Get().get<Health>(entity);
            h.health = 0;
            ap::Application::Get().OnEvent(ap::ServerBroadcastPacketEvent(ap::Serialize(h,entity)));

            m_players.erase(e.GetPeer());
            return true;
        })) return;

    if (e.Dispatch<ap::ServerSendPacketEvent>([&](ap::ServerSendPacketEvent& e)
        {
            e.GetPacket().serverSimulation = m_scene.GetSimulationCount();
            return false;
        })) return;
    if (e.Dispatch<ap::ServerBroadcastPacketEvent>([&](ap::ServerBroadcastPacketEvent& e)
        {
            e.GetPacket().serverSimulation = m_scene.GetSimulationCount();
            return false;
        })) return;

    if (e.Dispatch<ap::ServerReceivePacketEvent>([&](ap::ServerReceivePacketEvent& e)
        {
            // TODO: Handle the input
            m_packets.Push(e.GetPacket(),false);
            return true;
        })) return;

}

void ServerLayer::OnAttach()
{
    //m_reg.RegisterSystem(DrawSystem(m_camera.GetCamera()));
    m_scene.RegisterSystem(DeathSystem);
    ap::Application::Get().OnEvent(ap::ServerHostRequestEvent(25565));

    m_scene.RegisterSystem(ChunkRequestResponseSystem);

    //// TEMP: Spawn some nice chunks here
    //auto& reg = m_scene.GetRegistry();

    //for (int x = 0; x < 2; x++)
    //    {
    //        auto entity = reg.Create();
    //        auto& data = reg.Get().emplace<ChunkDataComponent>(entity);

    //        data.pos = glm::vec3(x * chunkDimensions.x, -40.f, x * chunkDimensions.z);
    //    }
}

void ServerLayer::OnDetach()
{
    ap::Application::Get().OnEvent(ap::ServerShutdownRequestEvent());
}

void ServerLayer::OnUpdate(ap::Timestep ts)
{
    auto& server = ap::NetServer::Get();
    if (!server.IsHosting()) return;
    
    ap::Packet p;
    m_packets.Swap();
    while (m_packets.Poll(p))
    {
        auto& reg = m_scene.GetRegistry();
        // Check if entity exists in our scene (created on server)
        if (reg.Get().valid(ap::Entity(p.entity)))
        {
            reg.HandlePacket(ap::Entity(p.entity), p);
        }
        // This means that the client created a request or send a null entity
        else
        {
            auto entity = reg.Create();
            
            reg.Get().emplace<SenderComponent>(entity, p.sender, ap::Entity(p.entity));
            reg.HandlePacket(entity, p);
        }
    }
    m_scene.Simulate(ts);

    // TODO: Should or could be handled in systems
    auto& reg = m_scene.GetRegistry().Get();
    auto view = reg.view<ap::Transform>();
    for (auto e : view)
    {
        auto p = ap::Serialize(view.get(e), e);
        ap::Application::Get().OnEvent(ap::ServerBroadcastPacketEvent(p));
    }
}

void ServerLayer::OnGuiRender()
{
    if (ImGui::Begin("Stats"))
    {
        if (ImGui::CollapsingHeader("Server"))
        {
            DrawSceneStats(m_scene);

            static int tick = 60;
            ImGui::SliderInt("Tickrate", &tick, 1, 200);
            ap::NetServerLayer::m_config.rate = 1.f / (float)tick;
        }
        ImGui::End();
    }
}