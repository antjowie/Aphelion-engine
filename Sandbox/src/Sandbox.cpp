#include "Sandbox.h"
#include "Component.h"
#include "System.h"

std::unordered_map<sh::Entity, sh::Entity> ClientLayer::m_netToLocal;

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
    if (!ImGui::Begin("Options"))
        return;

    // TODO: If server or client fails this doesn't get updated
    // it should be communicated via events
    static bool client = false;
    if (ImGui::Checkbox("Client", &client))
    {
        if (client)
        {
            m_client = new ClientLayer();
            sh::Application::Get().GetLayerStack().PushLayer(m_client);
        }
        else
        {
            sh::Application::Get().GetLayerStack().PopLayer(m_client);
            delete m_client;
            m_client = nullptr;
        }
    }

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
    m_reg.RegisterSystem(SpawnSystem);
    m_reg.RegisterSystem(InputSystem);
    m_reg.RegisterSystem(DrawSystem(m_camera.GetCamera()));

    sh::Application::Get().OnEvent(sh::ClientConnectRequestEvent("127.0.0.1", 25565));
}

void ClientLayer::OnDetach()
{
    sh::Application::Get().OnEvent(sh::ClientDisconnectRequestEvent());
    m_netToLocal.clear();
}

void ClientLayer::OnEvent(sh::Event& event)
{
    m_camera.OnEvent(event);

    sh::EventDispatcher d(event);

    if (d.Dispatch<sh::ClientReceivePacketEvent>([&](sh::ClientReceivePacketEvent& e)
        {
            // Get the right entity and check if server entity exists in view
            auto& p = e.GetPacket();
            auto netID = sh::Entity(p.entity.value);

            auto match = m_netToLocal.find(netID);
            if (match == m_netToLocal.end()) { m_netToLocal[netID] = m_reg.Create(); }

            sh::Entity local = m_netToLocal[netID];
            //SH_TRACE("Client received type ({}) for entity (local: {} net: {})", 
            //    m_reg.GetComponentData().at(p.id).name,
            //    local,netID);

            m_reg.HandlePacket(local, p);

            return false;
        })) return;
}

void ClientLayer::OnUpdate(sh::Timestep ts)
{
    auto& client = sh::NetClient::Get();
    if (!client.IsConnected()) return;

    m_camera.OnUpdate(ts);
    m_reg.UpdateSystems();
}

void ServerLayer::OnEvent(sh::Event& event)
{
    m_camera.OnEvent(event);

    sh::EventDispatcher e(event);

    if (e.Dispatch<sh::ServerClientConnectEvent>([&](sh::ServerClientConnectEvent& e)
        {
            auto& reg = m_reg;
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
            app.OnEvent(sh::ServerSendPacketEvent(sh::Serialize(Player(), unsigned(entity)), e.GetPeer()));

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
                app.OnEvent(sh::ServerSendPacketEvent(sh::Serialize(spawn, unsigned(ent)), e.GetPeer()));
            }

            // Broadcast new player
            SpawnEntity ent;
            ent.type = SpawnEntity::Player;
            ent.t.pos = glm::vec2(0.f);
            ent.sprite.image = "res/image.png";

            app.OnEvent(sh::ServerBroadcastPacketEvent(
                sh::Serialize(ent, unsigned(entity))));
            
            return true;
        })) return;

    if (e.Dispatch<sh::ServerClientConnectEvent>([&](sh::ServerClientConnectEvent& e)
        {
            char ip[64];
            enet_address_get_host_ip(&e.GetPeer()->address, ip, 64);
            //SH_INFO("Server closed connection with {}", ip);
            return true;
        })) return;
}

void ServerLayer::OnAttach()
{
    m_reg.RegisterSystem(SpawnSystem);
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
    m_reg.UpdateSystems();
}
