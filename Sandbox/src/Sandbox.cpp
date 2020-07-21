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
    sh::Registry::RegisterComponent<SpawnEntity>();
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
    m_scene.RegisterSystem(SpawnSystem);
    m_scene.RegisterSystem(InputSystem);
    m_scene.RegisterSystem(DrawSystem(m_camera.GetCamera()));
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
            //SH_TRACE("PUSHED {}", sh::Registry::GetComponentData().at(e.GetPacket().id).name);

            m_packets.Push(e.GetPacket(),true);

            return false;
        })) return;

    // Honestly, this may be quite a dumb way of doing it.
    // This is because the event gets modified along the way so it may get confusing where the event will pass through
    // and where not
    if (d.Dispatch<sh::ClientSendPacketEvent>([&](sh::ClientSendPacketEvent& e)
        {
            e.GetPacket().clientSimulation = m_scene.GetSimulationCount();

            if (!clientIsReconciling)
            {
                sh::Packet p;
                e.GetPacket().serializeFn(p);
                SH_TRACE("CLIENT Sending {} pos {:.2f}", e.GetPacket().clientSimulation, sh::Deserialize<Transform>(p).pos.x);
            }

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
    //SH_INFO("Polling packets");

    m_packets.Swap();
    clientIsReconciling = true;
    while(m_packets.Poll(p))
    {
        auto p2{ p };
        if(entt::type_info<Transform>::id() == p.id)
        SH_TRACE("CLIENT Retrieved {} pos {:.2f}", p.clientSimulation, sh::Deserialize<Transform>(p2).pos.x);

        //SH_TRACE(sh::Registry::GetComponentData().at(p.id).name);
        // Get the right entity and check if server entity exists in current reg
        auto netID = sh::Entity(p.entity);

        auto match = m_netToLocal.find(netID);
        if (match == m_netToLocal.end()) { m_netToLocal[netID] = m_scene.GetRegistry().Create(); }

        sh::Entity local = m_netToLocal[netID];
        //SH_TRACE("Client received type ({}) for entity (local: {} net: {})", 
        //    m_reg.GetComponentData().at(p.id).name,
        //    local,netID);

        /*
            Why do I subtract by 1?
            Update loop

            Handle packets
            Copy current values to next simulation
            Increate current simulation counter
            Simulate the simulation
            Send packets

            The problem here is that when you get a packet the simulation is already on in the future.
            See this example:
            at n = 2 we send a packet.
            now we are at n = 3.
            The server is so fast that we receive the packet already so we handle it.
            The packet has n = 2 so to calculate the delta we do 3 - 2.
            We get a rollback of 1 simulation.

            The problem is that when we reconcile the packets we have already advance the simulation count.
            
            When we handle these packets. We apply them to the previous simulation.
        */

        unsigned delta = m_scene.GetSimulationCount() - p.clientSimulation;

        Transform oldT;
        //if (p.clientSimulation != -1 && p.id == entt::type_info<Transform>::id())
        //{
        //    auto& reg = m_scene.GetRegistry(delta).Get();
        //    auto val = reg.try_get<Transform>(local);
        //    if (val) oldT = *val;
        //}
        // We only want to reconcile player input
        if (p.isCommand || !m_scene.GetRegistry().Get().has<Player>(local))
        {
            m_scene.GetRegistry().HandlePacket(local, p);
        }
        else if (m_scene.GetRegistry().Get().has<Player>(local) && p.clientSimulation != -1 && m_scene.GetRegistry(delta).HandleAndReconcilePacket(local, p))
        {
            auto newT = sh::Deserialize<Transform>(p);

            SH_WARN("Reconciliation!!! Curr {} Delta {} Local {:.2f} Server {:.2f}", m_scene.GetSimulationCount(), delta, oldT.pos.x,newT.pos.x);
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

            // TODO: Create a join component that is handled by the ECS
            // Create the new player
            auto entity = reg.Create();
            reg.Get().emplace<Transform>(entity, glm::vec2(0.f));
            auto& sprite = reg.Get().emplace<Sprite>(entity);
            sprite.image = "res/image.png";
            sprite.LoadTexture();

            // Submit the new player
            app.OnEvent(sh::ServerSendPacketEvent(sh::Serialize(Player(), entity, true), e.GetPeer()));

            // Send all existing users to that player
            auto view = reg.Get().view<Transform, Sprite>();
            for (auto ent : view)
            {
                auto& t = reg.Get().get<Transform>(ent);
                auto& s = reg.Get().get<Sprite>(ent);

                SpawnEntity spawn;
                spawn.type = SpawnEntity::Player;
                spawn.t = t;
                spawn.sprite = s;
                app.OnEvent(sh::ServerSendPacketEvent(sh::Serialize(spawn, ent, true), e.GetPeer()));
            }

            // Broadcast new player
            SpawnEntity ent;
            ent.type = SpawnEntity::Player;
            ent.t.pos = glm::vec2(0.f);
            ent.sprite.image = "res/image.png";

            app.OnEvent(sh::ServerBroadcastPacketEvent(
                sh::Serialize(ent, entity,true)));
            
            return true;
        })) return;

    if (e.Dispatch<sh::ServerClientConnectEvent>([&](sh::ServerClientConnectEvent& e)
        {
            char ip[64];
            enet_address_get_host_ip(&e.GetPeer()->address, ip, 64);
            //SH_INFO("Server closed connection with {}", ip);
            return true;
        })) return;

    if (e.Dispatch<sh::ServerSendPacketEvent>([&](sh::ServerSendPacketEvent& e)
        {
            e.GetPacket().serverSimulation = m_scene.GetSimulationCount();
            return false;
        })) return;
    if (e.Dispatch<sh::ServerBroadcastPacketEvent>([&](sh::ServerBroadcastPacketEvent& e)
        {
            //sh::Packet p;
            //e.GetPacket().serializeFn(p);
            //if(entt::type_info<Transform>::id() == e.GetPacket().id && e.GetPacket().clientSimulation != 0)
            //    SH_TRACE("SERVER Send {} pos {}", e.GetPacket().clientSimulation, (int)sh::Deserialize<Transform>(p).pos.x);
            e.GetPacket().serverSimulation = m_scene.GetSimulationCount();
            return false;
        })) return;

    if (e.Dispatch<sh::ServerReceivePacketEvent>([&](sh::ServerReceivePacketEvent& e)
        {
            // TODO: Handle the input
            m_packets.Push(e.GetPacket(),false);
            //m_scene.GetRegistry().HandlePacket(sh::Entity(e.GetPacket().entity), e.GetPacket());
            return true;
        })) return;

}

void ServerLayer::OnAttach()
{
    m_scene.RegisterSystem(SpawnSystem);
    //m_reg.RegisterSystem(DrawSystem(m_camera.GetCamera()));

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
        auto p2{ p };
        SH_TRACE("SERVER Retrieved {} pos {:.2f}", p.clientSimulation, sh::Deserialize<Transform>(p2).pos.x);

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

        //SH_TRACE("Server X:{}", t.pos.x);
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