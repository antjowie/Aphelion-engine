#include <Shinobu/Common.h>
#include <Shinobu/Net/Client.h>
#include <Shinobu/Net/Server.h>

#include <Shinobu/ECS/Registry.h>

//#include <glm/gtc/matrix_transform.hpp>
#include "Component.h"
#include "System.h"

//void ToggleClient()
//{
//    if (m_client.IsConnected() || m_client.IsConnecting()) m_client.Disconnect();
//    else m_client.Connect("localhost", 25565);
//}

class ClientLayer : public sh::Layer
{
public:
    sh::OrthographicCameraController m_camera;

    sh::Registry m_reg;

    sh::Client m_client;
    
    ClientLayer() : m_camera(16.f/9.f){}

    virtual void OnAttach() override
    {
        m_reg.RegisterSystem(SpawnSystem);
        m_reg.RegisterSystem(InputSystem);
        m_reg.RegisterSystem(DrawSystem(m_camera.GetCamera()));
        m_client.Connect("localhost", 25565);
    }

    virtual void OnEvent(sh::Event& event) override
    { 
        m_camera.OnEvent(event); 
    }

    virtual void OnDetach() override
    {
        SH_CORE_WARN("We ignore ack because server runs on main thread so this disconnect will never be ack");
        m_client.Disconnect(0);
    }

    virtual void OnUpdate(sh::Timestep ts) override 
    {
        if (!m_client.IsConnected()) return;

        m_camera.OnUpdate(ts);
        m_reg.UpdateSystems();

        // Handle packets
        sh::Packet p;
        auto& reg = m_reg;
        while (m_client.Poll(p))
        {
            // Get the right entity and check if server entity exists in view
            // Not sure why it has to be of this type, should just be a uint32_t (at the time of writing this)
            auto netID = sh::Entity(p.entity.value);

            auto match = sh::netToLocal.find(netID);
            if (match == sh::netToLocal.end()) { sh::netToLocal[netID] = reg.Create(); }

            sh::Entity local = sh::netToLocal[netID];
            SH_TRACE("Client received type ({}) for entity (local: {} net: {})", 
                m_reg.GetComponentData().at(p.id).name,
                local,netID);

            // TODO: Add an interface for this
            m_reg.GetComponentData().at(p.id).unpack(m_reg, local, p);
        }

        // TEMP: Send owned player pos
        auto view = reg.Get().view<Transform, Player>();
        for (auto e : view)
        {
            auto t = reg.Get().get<Transform>(e);
            m_client.Submit(sh::Serialize(t, (unsigned)sh::LocalIDToNet(e)));
        }

        // Flush every second
        m_client.Flush();
        //static sh::Timer time;
        //if (time.Elapsed() > 1.f)
        //{
        //    time.Reset();
        //}
    }
};

class ServerLayer : public sh::Layer
{
public:
    sh::Server m_server;
    
    sh::OrthographicCameraController m_camera;

    sh::Registry m_reg;

    ServerLayer() : m_camera(16.f / 9.f) {}

    virtual void OnEvent(sh::Event& event) override 
    { 
        m_camera.OnEvent(event); 
    }

    virtual void OnAttach() override 
    {
        m_reg.RegisterSystem(SpawnSystem);
        //m_reg.RegisterSystem(DrawSystem(m_camera.GetCamera()));

        // using ConnectCB = std::function<void(Server&, ENetPeer* connection)>;
        m_server.SetConnectCB([&](sh::Server& server, ENetPeer* connection)
            {
                char ip[64];
                enet_address_get_host_ip(&connection->address, ip, 64);
                SH_INFO("Server opened connection with {}", ip);

                // Create the new player
                auto& reg = m_reg;
                auto entity = reg.Create();
                reg.Get().emplace<Transform>(entity, glm::vec2(0.f));
                auto& sprite = reg.Get().emplace<Sprite>(entity);
                sprite.image = "res/image.png";
                sprite.LoadTexture();

                // Submit the new player
                server.Submit(sh::Serialize(Player(), unsigned(entity)), connection);

                // Send all existing users to that player
                auto view = reg.Get().view<Transform, Sprite>();
                for (auto e : view)
                {
                    auto& t = reg.Get().get<Transform>(e);
                    auto& s = reg.Get().get<Sprite>(e);

                    SpawnEntity spawn;
                    spawn.type = SpawnEntity::Player;
                    spawn.t = t;
                    spawn.sprite = s;
                    server.Submit(sh::Serialize(spawn, unsigned(e)), connection);
                }

                // Broadcast new player
                SpawnEntity e;
                e.type = SpawnEntity::Player;
                e.t.pos = glm::vec2(0.f);
                e.sprite.image = "res/image.png";
                server.Broadcast(sh::Serialize(e, unsigned(entity)));
            });
        m_server.SetDisconnectCB([](sh::Server& server, ENetPeer* connection)
            {
                char ip[64];
                enet_address_get_host_ip(&connection->address, ip, 64);
                SH_INFO("Server closed connection with {}", ip);
            });

        m_server.Host(25565);
    }
    
    virtual void OnUpdate(sh::Timestep ts) override final
    {
        if (!m_server.IsHosting()) return;

        m_camera.OnUpdate(ts);
        m_reg.UpdateSystems();

        if (m_server.IsHosting())
        {
            auto& reg = m_reg;
         
            sh::Packet p;
            while (m_server.Poll(p))
            {
                // TODO: Normally you would handle the input here but for now no verifiction
                // TODO: If the client is hosting the server as well they will both interact with one
                // world since the ECS is static. This must be refactored because otherwise
                // the client will render everything on the server view
                // which in our game (minecraft clone) should not be the case
                
                // Get the right entity and check if server entity exists in view
                // Not sure why it has to be of this type, should just be a uint32_t (at the time of writing this)
                // In server case, netID is already the correct ID
                auto netID = sh::Entity(p.entity.value);
                
                SH_TRACE("Server received type ({}) for entity {}",
                    m_reg.GetComponentData().at(p.id).name,
                    netID);

                // TODO: Add an interface for this
                m_reg.GetComponentData().at(p.id).unpack(m_reg, netID, p);
            }

            // Broadcast all the positions that the server has
            // TEMP: Send all player pos, this should prob happen in a system
            auto view = reg.Get().view<Transform>();
            for (auto e : view)
            {
                auto& t = reg.Get().get<Transform>(e);
                m_server.Broadcast(sh::Serialize(t, (unsigned)e));
            }

            m_server.Flush();
        }
    }
};

class MainMenuLayer : public sh::Layer
{
public:
    sh::Layer* m_client = nullptr;
    sh::Layer* m_server = nullptr;

    virtual void OnEvent(sh::Event& event) override
    {
        sh::EventDispatcher d(event);
        d.Dispatch<sh::KeyPressedEvent>([&](sh::KeyPressedEvent& e)
        {
            if (e.GetKeyCode() == sh::KeyCode::Escape)
                sh::Application::Get().Exit();
            return false;
        });
    }

    virtual void OnGuiRender() override
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

    /*
    virtual void OnGuiRender() override final
    {
        if (!ImGui::Begin("Network"))
        {
            // Early out if the window is collapsed, as an optimization.
            ImGui::End();
            return;
        }
        {
            bool hosting = m_server.IsHosting();
            bool connected = m_client.IsConnected() | m_client.IsConnecting();

            if (ImGui::Checkbox("Server", &hosting)) ToggleServer();
            if (ImGui::Checkbox("Client", &connected))
            {
                ToggleClient();
            }
            if (m_client.IsConnected())
            {
                static char msg[128] = "";

                ImGui::InputText("message", msg, 128);
                if (ImGui::Button("send"))
                {
                    sh::ExampleData data;
                    data.message = msg;
                    auto p = sh::Serialize(data, entt::type_info<sh::ExampleData>::id());
                    SH_TRACE("Client send ({}): {}", p.id.value, data.message);
                    m_client.Submit(p);
                }
                if (ImGui::Button("Foo"))
                {
                    Foo data;
                    //data.message = msg;
                    auto id = entt::type_info<Foo>::id();
                    auto p = sh::Serialize(data, id);
                    SH_TRACE("Client send (Foo)");
                    m_client.Submit(p);
                }
                if (ImGui::Button("Bar"))
                {
                    Bar data;
                    //data.message = msg;
                    auto id = entt::type_info<Bar>::id();
                    auto p = sh::Serialize(data, id);
                    SH_TRACE("Client send (Bar)");
                    m_client.Submit(p);
                }

            }
        }
        ImGui::End();
    }
    */
};


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