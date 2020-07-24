#include "Sandbox.h"
#include "Component.h"
#include "System.h"

#include <Shinobu/Net/ClientLayer.h>
#include <Shinobu/Net/ServerLayer.h>

std::unordered_map<sh::Entity, sh::Entity> ClientLayer::m_netToLocal;

void DrawSceneStats(sh::Scene& scene)
{
    auto& reg = scene.GetRegistry().Get();
    ImGui::Text("Simulation %i (%i/%i)", scene.GetSimulationCount(), scene.GetCurrentSimulationIndex(), scene.maxSimulations);
    ImGui::Text("Entities %i", reg.size());
}

std::unique_ptr<sh::Application> sh::CreateApplication()
{
    sh::Registry::RegisterComponent<Player>();
    sh::Registry::RegisterComponent<::Transform>();
    sh::Registry::RegisterComponent<Sprite>();
    sh::Registry::RegisterComponent<Health>();
    auto app = std::make_unique<sh::Application>();
    app->GetLayerStack().PushLayer(new MainMenuLayer());

    return app;
}

void MainMenuLayer::OnEvent(sh::Event& event)
{
    sh::EventDispatcher d(event);
    d.Dispatch<sh::KeyPressedEvent>([&](sh::KeyPressedEvent& e)
        {
            if (e.GetKeyCode() == sh::KeyCode::Escape)
                sh::Application::Get().Exit();
            return false;
        });
}

void MainMenuLayer::OnGuiRender()
{
    //Sleep(500);
    if (!ImGui::Begin("Options"))
        return;

    // TODO: If server or client fails this doesn't get updated
    // it should be communicated via events
    static bool client = false;
    static char serverIP[32] = "127.0.0.1";
    if (ImGui::Checkbox("Client", &client))
    {
        if (client)
        {
            m_client = new ClientLayer();
            sh::Application::Get().GetLayerStack().PushLayer(m_client);
            sh::Application::Get().OnEvent(sh::ClientConnectRequestEvent(serverIP, 25565));
        }
        else
        {
            sh::Application::Get().GetLayerStack().PopLayer(m_client);
            delete m_client;
            m_client = nullptr;
        }
    }

    ImGui::SameLine(); ImGui::Text("ip ");
    ImGui::SameLine(); ImGui::InputText("#IP", serverIP, 32);

    static bool server = false;
    if (ImGui::Checkbox("Server", &server))
    {
        if (server)
        {
            m_server = new ServerLayer();
            sh::Application::Get().GetLayerStack().PushLayer(m_server);
        }
        else
        {
            sh::Application::Get().GetLayerStack().PopLayer(m_server);
            delete m_server;
            m_server = nullptr;
        }
    }

    ImGui::End();
}

sh::Entity ClientLayer::LocalIDToNet(sh::Entity localID) 
{
    for (const auto e : m_netToLocal)
        if (e.second == localID) return e.first;
    SH_CORE_ERROR("Local ID {} can't be mapped to a network ID", localID);
}

sh::Entity ClientLayer::NetIDtoLocal(sh::Entity netID)
{
    return m_netToLocal.at(netID);
}

void ClientLayer::OnAttach()
{
    m_scene.RegisterSystem(InputSystem);
    m_scene.RegisterSystem(DeathSystem);
    m_scene.RegisterSystem(DrawSystem(m_camera.GetCamera()));

    m_scene.SetOnEntityCreateCb([this](sh::Entity entity)
    {
    });
    m_scene.SetOnEntityDestroyCb([this](sh::Entity entity)
    {
        m_netToLocal.erase(LocalIDToNet(entity));
    });
}

void ClientLayer::OnDetach()
{
    // TODO: add timeout variable to this so that we can just do 0 instead of 5
    sh::Application::Get().OnEvent(sh::ClientDisconnectRequestEvent());
    m_netToLocal.clear();
}

/**
 * This however is an issue when reconciling since the simulation runs again, 
 * so the client sends a bunch of packets. I only want to send packets at the 
 * end of the most recent simulation. 
 * To solve this, when intercepting packets. I only push them if we are not reconciling
 */
bool clientIsReconciling = false;

void ClientLayer::OnEvent(sh::Event& event)
{
    m_camera.OnEvent(event);

    sh::EventDispatcher d(event);

    if (d.Dispatch<sh::ClientReceivePacketEvent>([&](sh::ClientReceivePacketEvent& e)
        {
            m_packets.Push(e.GetPacket(),true);
            return false;
        })) return;

    // Honestly, this may be quite a dumb way of doing it.
    // This is because the event gets modified along the way so it may get confusing where the event will pass through
    // and where not
    if (d.Dispatch<sh::ClientSendPacketEvent>([&](sh::ClientSendPacketEvent& e)
        {
            e.GetPacket().clientSimulation = m_scene.GetSimulationCount();
            return clientIsReconciling;
        })) return;
}

void ClientLayer::OnUpdate(sh::Timestep ts)
{
    auto& client = sh::NetClient::Get();
    if (!client.IsConnected()) return;

    m_camera.OnUpdate(ts);
    
    // Poll packets
    sh::Packet p;
    m_packets.Swap();
    clientIsReconciling = true;
    while(m_packets.Poll(p))
    {
        auto netID = sh::Entity(p.entity);
        auto match = m_netToLocal.find(netID);
        if (match == m_netToLocal.end()) { m_netToLocal[netID] = m_scene.GetRegistry().Create(); }
        auto local = m_netToLocal[netID];

        unsigned delta = m_scene.GetSimulationCount() - p.clientSimulation;

        // We only want to reconcile player input
        if (p.isCommand || !m_scene.GetRegistry().Get().has<Player>(local))
        {
            m_scene.GetRegistry().HandlePacket(local, p);
        }
        else if (m_scene.GetRegistry().Get().has<Player>(local) && 
            p.clientSimulation != -1 && 
            m_scene.GetRegistry(delta).HandleAndReconcilePacket(local, p))
        {
            auto newT = sh::Deserialize<Transform>(p);

            SH_WARN("Reconciliation!!!");
            // TODO: Reconciliate subsequent registries
            m_scene.GetRegistry().HandlePacket(local, p);
        }
    }
    clientIsReconciling = false;

    m_scene.Simulate(ts);
}

void ClientLayer::OnGuiRender()
{
    if (ImGui::Begin("Stats"))
    {
        if(ImGui::CollapsingHeader("Client"))
        {
            DrawSceneStats(m_scene);

            static int tick = 60;
            ImGui::SliderInt("Tickrate", &tick, 1, 200);
            sh::NetClientLayer::m_config.rate = 1.f / (float)tick;
        }
        ImGui::SliderFloat("Movespeed", &movespeed, 0.f, 5.f);
        ImGui::End();
    }
}

void ServerLayer::OnEvent(sh::Event& event)
{
    m_camera.OnEvent(event);

    sh::EventDispatcher e(event);

    if (e.Dispatch<sh::ServerClientConnectEvent>([&](sh::ServerClientConnectEvent& e)
        {
            auto& reg = m_scene.GetRegistry();
            auto& app = sh::Application::Get();

            char ip[64];
            enet_address_get_host_ip(&e.GetPeer()->address, ip, 64);

            // Send all existing users to that player
            auto view = reg.Get().view<Transform, Sprite, Health>();
            for (auto ent : view)
            {
                auto& t = reg.Get().get<Transform>(ent);
                auto& s = reg.Get().get<Sprite>(ent);
                auto& h = reg.Get().get<Health>(ent);

                app.OnEvent(sh::ServerSendPacketEvent(sh::Serialize(t, ent, true), e.GetPeer()));
                app.OnEvent(sh::ServerSendPacketEvent(sh::Serialize(s, ent, true), e.GetPeer()));
                app.OnEvent(sh::ServerSendPacketEvent(sh::Serialize(h, ent, true), e.GetPeer()));
            }

            // Create the new player player
            auto entity = reg.Create();
            auto& t = reg.Get().emplace<Transform>(entity, glm::vec2(0.f));
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
}

void ServerLayer::OnDetach()
{
    sh::Application::Get().OnEvent(sh::ServerShutdownRequestEvent());
}

void ServerLayer::OnUpdate(sh::Timestep ts)
{
    auto& server = sh::NetServer::Get();
    if (!server.IsHosting()) return;

    m_camera.OnUpdate(ts);
    
    sh::Packet p;
    m_packets.Swap();
    while (m_packets.Poll(p))
    {
        m_scene.GetRegistry().HandlePacket(sh::Entity(p.entity), p);
    }
    m_scene.Simulate(ts);

    // TODO: Should or could be handled in systems
    auto& reg = m_scene.GetRegistry().Get();
    auto view = reg.view<Transform>();
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