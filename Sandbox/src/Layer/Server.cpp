#include "Server.h"
#include "System/System.h"
#include "SceneStats.h"
#include "Component/Component.h"
#include "Component/ServerComponent.h"

#include <Shinobu/Core/Application.h>
#include <Shinobu/Event/NetEvent.h>
#include <Shinobu/Net/Server.h>
#include <Shinobu/Net/ServerLayer.h>

#include "Component/ChunkComponent.h"
#include "System/ChunkSystem.h"

void ServerLayer::OnEvent(sh::Event& event)
{
    sh::EventDispatcher e(event);

    if (e.Dispatch<sh::ServerClientConnectEvent>([&](sh::ServerClientConnectEvent& e)
        {
            auto& reg = m_scene.GetRegistry();
            auto& app = sh::Application::Get();

            char ip[64];
            enet_address_get_host_ip(&e.GetPeer()->address, ip, 64);

            // Send all existing users to that player
            auto view = reg.Get().view<sh::Transform, Sprite, Health>();
            for (auto ent : view)
            {
                auto& t = reg.Get().get<sh::Transform>(ent);
                auto& s = reg.Get().get<Sprite>(ent);
                auto& h = reg.Get().get<Health>(ent);

                app.OnEvent(sh::ServerSendPacketEvent(sh::Serialize(t, ent, true), e.GetPeer()));
                app.OnEvent(sh::ServerSendPacketEvent(sh::Serialize(s, ent, true), e.GetPeer()));
                app.OnEvent(sh::ServerSendPacketEvent(sh::Serialize(h, ent, true), e.GetPeer()));
            }

            // Create the new player player
            auto entity = reg.Create();
            auto& t = reg.Get().emplace<sh::Transform>(entity);
            auto& s = reg.Get().emplace<Sprite>(entity);
            s.image = "res/image.png";
            s.LoadTexture();
            auto& h = reg.Get().emplace<Health>(entity);
            h.health = 1;

            app.OnEvent(sh::ServerBroadcastPacketEvent(sh::Serialize(t, entity,true)));
            app.OnEvent(sh::ServerBroadcastPacketEvent(sh::Serialize(s, entity,true)));
            app.OnEvent(sh::ServerBroadcastPacketEvent(sh::Serialize(h,entity,true)));
            // Tell the new user that they own that player
            app.OnEvent(sh::ServerSendPacketEvent( 
                sh::Serialize(Player(), entity, true), e.GetPeer()));

            m_players[e.GetPeer()] = entity;
            return true;
        })) return;

    if (e.Dispatch<sh::ServerClientDisconnectEvent>([&](sh::ServerClientDisconnectEvent& e)
        {
            char ip[64];
            enet_address_get_host_ip(&e.GetPeer()->address, ip, 64);
            SH_INFO("Server closed connection with {}", ip);

            // When player leaves we broadcast that their HP is zero
            auto entity = m_players.at(e.GetPeer());
            auto& h = m_scene.GetRegistry().Get().get<Health>(entity);
            h.health = 0;
            sh::Application::Get().OnEvent(sh::ServerBroadcastPacketEvent(sh::Serialize(h,entity)));

            m_players.erase(e.GetPeer());
            return true;
        })) return;

    if (e.Dispatch<sh::ServerSendPacketEvent>([&](sh::ServerSendPacketEvent& e)
        {
            e.GetPacket().serverSimulation = m_scene.GetSimulationCount();
            return false;
        })) return;
    if (e.Dispatch<sh::ServerBroadcastPacketEvent>([&](sh::ServerBroadcastPacketEvent& e)
        {
            e.GetPacket().serverSimulation = m_scene.GetSimulationCount();
            return false;
        })) return;

    if (e.Dispatch<sh::ServerReceivePacketEvent>([&](sh::ServerReceivePacketEvent& e)
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
    sh::Application::Get().OnEvent(sh::ServerHostRequestEvent(25565));

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
    sh::Application::Get().OnEvent(sh::ServerShutdownRequestEvent());
}

void ServerLayer::OnUpdate(sh::Timestep ts)
{
    auto& server = sh::NetServer::Get();
    if (!server.IsHosting()) return;
    
    sh::Packet p;
    m_packets.Swap();
    while (m_packets.Poll(p))
    {
        auto& reg = m_scene.GetRegistry();
        // Check if entity exists in our scene (created on server)
        if (reg.Get().valid(sh::Entity(p.entity)))
        {
            reg.HandlePacket(sh::Entity(p.entity), p);
        }
        // This means that the client created a request or send a null entity
        else
        {
            auto entity = reg.Create();
            
            reg.Get().emplace<SenderComponent>(entity, p.sender, sh::Entity(p.entity));
            reg.HandlePacket(entity, p);
        }
    }
    m_scene.Simulate(ts);

    // TODO: Should or could be handled in systems
    auto& reg = m_scene.GetRegistry().Get();
    auto view = reg.view<sh::Transform>();
    for (auto e : view)
    {
        auto p = sh::Serialize(view.get(e), e);
        sh::Application::Get().OnEvent(sh::ServerBroadcastPacketEvent(p));
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
            sh::NetServerLayer::m_config.rate = 1.f / (float)tick;
        }
        ImGui::End();
    }
}