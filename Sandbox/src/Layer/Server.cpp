#include "Server.h"
#include "System/System.h"
#include "SceneStats.h"
#include "Component/Component.h"
#include "Component/ServerComponent.h"

#include <Aphelion/Core/Application.h>
#include <Aphelion/Core/Event/NetEvent.h>
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
            //auto view = reg.Get().view<ap::TransformComponent, Sprite, Health>();
            reg.View<ap::TransformComponent,Sprite,Health, ap::GUIDComponent>(
                [&](ap::Entity entity, ap::TransformComponent& t, Sprite& s, Health& h, ap::GUIDComponent& guid)
            {
                app.OnEvent(ap::ServerSendPacketEvent(ap::Serialize(t, guid), e.GetPeer()));
                app.OnEvent(ap::ServerSendPacketEvent(ap::Serialize(s, guid), e.GetPeer()));
                app.OnEvent(ap::ServerSendPacketEvent(ap::Serialize(h, guid), e.GetPeer()));
            });
            //for (auto ent : view)

            // Create the new entity for the client that just joined
            auto entity = reg.Create();

            auto& guid = entity.GetComponent<ap::GUIDComponent>().guid;
            auto& t = entity.GetComponent<ap::TransformComponent>();
            t.t.SetPosition(glm::vec3(0, 100, 0));
            auto& s = entity.AddComponent<Sprite>();
            s.image = "res/image.png";
            s.LoadTexture();
            auto& h = entity.AddComponent<Health>(1);
            //h.health = 1;

            app.OnEvent(ap::ServerBroadcastPacketEvent(ap::Serialize(t, guid)));
            app.OnEvent(ap::ServerBroadcastPacketEvent(ap::Serialize(s, guid)));
            app.OnEvent(ap::ServerBroadcastPacketEvent(ap::Serialize(h, guid)));
            // Tell the new user that they own that player
            app.OnEvent(ap::ServerSendPacketEvent(ap::Serialize(Player(), guid), e.GetPeer()));

            m_players[e.GetPeer()] = guid;
            return true;
        })) return;

    if (e.Dispatch<ap::ServerClientDisconnectEvent>([&](ap::ServerClientDisconnectEvent& e)
        {
            char ip[64];
            enet_address_get_host_ip(&e.GetPeer()->address, ip, 64);
            AP_INFO("Server closed connection with {}", ip);

            // When player leaves we broadcast that their HP is zero
            auto guid = m_players.at(e.GetPeer());
            auto entity = m_scene.GetRegistry().Get(guid);
            auto& h = entity.GetComponent<Health>();
            h.health = 0;
            ap::Application::Get().OnEvent(ap::ServerBroadcastPacketEvent(ap::Serialize(h,guid)));

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
    m_scene.RegisterSystem(InputResponseSystem);

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
        // Received a command
        if (p.guid == 0)
        {
            auto entity = reg.Create();

            entity.AddComponent<SenderComponent>(p.sender);
            reg.HandlePacket(entity.GetComponent<ap::GUIDComponent>().guid, p);
        }
        else 
        {
            reg.HandlePacket(p.guid, p);
        }
    }
    m_scene.Simulate(ts);

    // TODO: Should or could be handled in systems
    auto& reg = m_scene.GetRegistry();
    //auto view = reg.view<ap::Transform>();
    reg.View<ap::TransformComponent, ap::GUIDComponent>(
        [](ap::Entity e, ap::TransformComponent& t, ap::GUIDComponent& guid)
    {
        auto p = ap::Serialize(t, guid);
        ap::Application::Get().OnEvent(ap::ServerBroadcastPacketEvent(p));
    });
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