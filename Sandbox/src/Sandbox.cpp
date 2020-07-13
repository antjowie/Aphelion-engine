#include <Shinobu/Common.h>
#include <Shinobu/Net/Client.h>
#include <Shinobu/Net/Server.h>

#include <Shinobu/ECS/ECSLayer.h>

//#include <glm/gtc/matrix_transform.hpp>
#include "Component.h"
#include "System.h"

class ExampleLayer2D : public sh::Layer
{
public:
    ExampleLayer2D() 
        : sh::Layer("Example Layer") 
        , m_camera(16.f/9.f){}

    sh::OrthographicCameraController m_camera;

    sh::Server m_server;
    sh::Client m_client;

    sh::Registry m_reg;

    void ToggleServer()
    {
        if (m_server.IsHosting()) m_server.Shutdown();
        else m_server.Host(25565);
    }

    void ToggleClient()
    {
        if (m_client.IsConnected() || m_client.IsConnecting()) m_client.Disconnect();
        else m_client.Connect("localhost", 25565);
    }

    virtual void OnAttach() override 
    {
        sh::Registry::RegisterComponent<Player>();
        sh::Registry::RegisterComponent<Transform>();
        sh::Registry::RegisterComponent<Sprite>();
        sh::Registry::RegisterComponent<SpawnEntity>();
        
        // Can't register emtpy structs which makes sense cuz theres nothing to serialize
        // Tho this also means there is no runtime information? maybe there is no info available?
        //sh::ECS::RegisterComponent<Player>();

        m_reg.RegisterSystem(SpawnSystem);
        m_reg.RegisterSystem(InputSystem);
        m_reg.RegisterSystem(DrawSystem(m_camera.GetCamera()));

        // Temp: Create entity locally. Server should send this
        //auto& reg = m_reg;
        //auto entity = reg.Create();
        //reg.Get().emplace<Transform>(entity, glm::vec2(0.f));
        //reg.Get().emplace<Sprite>(entity, "res/image.png", nullptr);
        //reg.Get().emplace<Player>(entity);

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
    }
    
    virtual void OnDetach() override 
    { 
        SH_INFO("Detached {0}", GetName()); 

        // If user runs client, we need to keep updating the server so that the server still sends disconnect packets
        if (m_client.IsConnected() && m_server.IsHosting())
        {
            auto future = m_client.Disconnect(); 
            while (future.wait_for(std::chrono::seconds(0)) != std::future_status::ready)
            {
                sh::Packet p;
                m_server.Poll(p);
            }
        }
    }

    virtual void OnEvent(sh::Event& event) override 
    {
        m_camera.OnEvent(event);

        sh::EventDispatcher d(event); 
        d.Dispatch<sh::KeyPressedEvent>([&](sh::KeyPressedEvent& e)
        {
            if (e.GetKeyCode() == sh::KeyCode::Escape)
                sh::Application::Get().Exit();
            return false;
        });
    }

    virtual void OnUpdate(sh::Timestep ts) override final
    {
        m_camera.OnUpdate(ts);
        m_reg.UpdateSystems();

        sh::Packet p;
        if (m_server.IsHosting())
        {
            auto& reg = m_reg;
         
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
                
                //SH_TRACE("Client received type ({}) for entity {}",
                    //m_reg.GetComponentData().at(p.id).name,
                    //netID);

                // TODO: Add an interface for this
                m_reg.GetComponentData().at(p.id).unpack(m_reg, netID, p);

                continue;

                /**
                 * TODO:
                 * This ugly centralized switch is something that I want to prevent.
                 * To do that, I'll take some ideas from the Overwatch and Unity lecture that I've looked at
                 * Essentially, I'm thinking of a system with 2 components. For example:
                 * PredictTransform and Transform
                 * Transform is authorative and is where the ECS will forward it's values to
                 * while the client is free to modify the PredictTransform component. This
                 * allows us to easily distinguish between the two. More importantly, it allows
                 * the ECS to just update values directly instead of the user having to handle it
                 */
                switch (p.id)
                {
                case entt::type_info<sh::ExampleData>::id():
                {
                    //auto reg = m_reg;
                    auto data = sh::Deserialize<sh::ExampleData>(p);

                    // You prob don't ever wanna broadcast the ip of a connected person
                    data.message = std::to_string(p.sender->address.host) + ": " + data.message;
                    
                    //SH_TRACE("Server received ({}): {}", p.id.value, data.message);
                    m_server.Broadcast(p);
                }
                break;

                case entt::type_info<Foo>::id():
                {
                    auto data = sh::Deserialize<Foo>(p);
                }
                break;

                case entt::type_info<Bar>::id():
                {
                    auto data = sh::Deserialize<Bar>(p);
                }
                break;
                }

            }

            // Broadcast all the positions that the server has
            // TEMP: Send all player pos, this should prob happen in a system
            auto view = reg.Get().view<Transform>();
            for (auto e : view)
            {
                auto t = reg.Get().get<Transform>(e);
                m_server.Broadcast(sh::Serialize(t, (unsigned)e));
            }

            m_server.Flush();
        }
        if (m_client.IsConnected())
        {
            auto& reg = m_reg;
            while (m_client.Poll(p))
            {
                // Get the right entity and check if server entity exists in view
                // Not sure why it has to be of this type, should just be a uint32_t (at the time of writing this)
                auto netID = sh::Entity(p.entity.value);
                                
                auto match = sh::netToLocal.find(netID);
                if (match == sh::netToLocal.end()) { sh::netToLocal[netID] = reg.Create(); }

                sh::Entity local = sh::netToLocal[netID];
                //SH_TRACE("Client received type ({}) for entity (local: {} net: {})", 
                    //m_reg.GetComponentData().at(p.id).name,
                    //local,netID);

                // TODO: Add an interface for this
                m_reg.GetComponentData().at(p.id).unpack(m_reg, local, p);
            }

            // TEMP: Send owned player pos
            auto view = reg.Get().view<Transform, Player>();
            for (auto e : view)
            {
                auto t = reg.Get().get<Transform>(e);
                m_client.Submit(sh::Serialize(t,(unsigned)sh::LocalIDToNet(e)));
            }


            static sh::Timer time;
            if (time.Elapsed() > 1.f)
            {
                m_client.Flush();
                time.Reset();
            }
        }
    }

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

                ImGui::InputText("message",msg, 128);
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
};

/*
class ExampleLayer3D : public sh::Layer
{
public:
    ExampleLayer3D()
        : sh::Layer("Example Layer")
        , fov(glm::radians(45.f))
        , m_camera(fov, 16.f/9.f)
    {}
    
    float fov;

    sh::PerspectiveCameraController m_camera;

    bool m_lookAtCube = false;
    sh::Transform m_transform;
    std::shared_ptr<sh::VertexArray> m_cube;
    std::shared_ptr<sh::Shader> m_shader;

    virtual void OnAttach() override
    {
        ImGui::SetCurrentContext(sh::ImGuiLayer::GetContext());

        // Create a cube
        {
            //https://developer.mozilla.org/en-US/docs/Web/API/WebGL_API/Tutorial/Creating_3D_objects_using_WebGL
            constexpr float positions[] {
            // Front face
                -1.0, -1.0, 1.0,
                1.0, -1.0, 1.0,
                1.0, 1.0, 1.0,
                -1.0, 1.0, 1.0,

                // Back face
                -1.0, -1.0, -1.0,
                -1.0, 1.0, -1.0,
                1.0, 1.0, -1.0,
                1.0, -1.0, -1.0,

                // Top face
                -1.0, 1.0, -1.0,
                -1.0, 1.0, 1.0,
                1.0, 1.0, 1.0,
                1.0, 1.0, -1.0,

                // Bottom face
                -1.0, -1.0, -1.0,
                1.0, -1.0, -1.0,
                1.0, -1.0, 1.0,
                -1.0, -1.0, 1.0,

                // Right face
                1.0, -1.0, -1.0,
                1.0, 1.0, -1.0,
                1.0, 1.0, 1.0,
                1.0, -1.0, 1.0,

                // Left face
                -1.0, -1.0, -1.0,
                -1.0, -1.0, 1.0,
                -1.0, 1.0, 1.0,
                -1.0, 1.0, -1.0,
        };
            constexpr uint32_t indices[] {
                0, 1, 2, 0, 2, 3,    // front
                4, 5, 6, 4, 6, 7,    // back
                8, 9, 10, 8, 10, 11,   // top
                12, 13, 14, 12, 14, 15,   // bottom
                16, 17, 18, 16, 18, 19,   // right
                20, 21, 22, 20, 22, 23,   // left
        };

            m_shader = sh::Shader::Create("res/shaders/Texture3D.glsl");

            auto vBuffer = sh::VertexBuffer::Create(positions, sizeof(positions));
            vBuffer->AddElement(sh::BufferElement(sh::ShaderDataType::Float3, "aPos"));

            auto iBuffer = sh::IndexBuffer::Create(indices, sizeof(indices) / sizeof(indices[0]));

            m_cube = sh::VertexArray::Create();
            m_cube->AddVertexBuffer(vBuffer);
            m_cube->SetIndexBuffer(iBuffer);
        }
        m_transform.SetPosition(glm::vec3(0, 0, 10));

        m_camera.GetCamera().transform.LookTowards(sh::Transform::GetWorldForward());
    }

    virtual void OnDetach() override { SH_INFO("Detached {0}", GetName()); }

    virtual void OnEvent(sh::Event& event) override
    {
        m_camera.OnEvent(event);

        sh::EventDispatcher d(event);
        d.Dispatch<sh::KeyPressedEvent>([&](sh::KeyPressedEvent& e)
        {
            if (e.GetKeyCode() == sh::KeyCode::Escape)
                sh::Application::Get().Exit();
            return false;
        });
    }

    virtual void OnUpdate(sh::Timestep ts) override final
    {
        m_camera.OnUpdate(ts);
        static float et = 0.f;
        et += ts;

        // Update camera
        const float rotX = glm::sin(glm::radians(et * 180.f));
        const float rotY = glm::cos(glm::radians(et * 90.f));
        const float rotZ = glm::sin(glm::cos(glm::radians(et * 180.f)));
        const float xOffset = glm::cos(glm::radians(et * 180.f));
        const float yOffset = glm::sin(glm::radians(et * 180.f));
        const glm::vec3 offset = glm::vec3(xOffset, yOffset, 0.f) * 3.f; 
        auto transform =
            glm::translate(glm::mat4(1), offset+ sh::Transform::GetWorldForward() * 10.f) *
            glm::rotate(glm::mat4(1), rotX, glm::vec3(1, 0, 0)) *
            glm::rotate(glm::mat4(1), rotY, glm::vec3(0, 1, 0)) *
            glm::rotate(glm::mat4(1), rotZ, glm::vec3(0, 0, 1))
            ;

        m_transform.SetPosition(transform[3]);

        if(m_lookAtCube)
            m_camera.GetCamera().transform.LookAt(m_transform.GetPosition());

        sh::Renderer::BeginScene(m_camera.GetCamera());
        sh::Renderer::Submit(m_shader, m_cube, m_transform.GetWorldMatrix());
        sh::Renderer::EndScene();
    }

    virtual void OnGuiRender() override final
    {
        if (!ImGui::Begin("Camera"))
        {
            ImGui::End();
            return;
        }

        ImGui::SliderAngle("fov", &fov, 0, 180);
        m_camera.GetCamera().SetFOV(fov);

        glm::vec3 pos(m_camera.GetCamera().transform.GetPosition());
        ImGui::DragFloat3("position", &pos.x);
        
        {
            glm::vec3 t(m_camera.GetCamera().transform.GetEulerRotation());
            ImGui::DragFloat3("rotation", &sh::Degrees(t).x);
            m_camera.GetCamera().transform.SetRotation(sh::Radians(t));
        }

        {
            glm::vec3 t(m_transform.GetEulerRotation());
            ImGui::DragFloat3("cube rotation", &sh::Degrees(t).x);
            m_transform.SetRotation(sh::Radians(t));
        }

        if (ImGui::Button("reset")) m_transform.SetRotation(glm::vec3(0));
        if (ImGui::Checkbox("look at cube", &m_lookAtCube)); // Yea we don't do anything here cuz we check in update

        ImGui::End();
    }
};
*/

std::unique_ptr<sh::Application> sh::CreateApplication()
{
    auto app = std::make_unique<sh::Application>();
    app->GetLayerStack().PushLayer(new ECSLayer);
    app->GetLayerStack().PushLayer(new ExampleLayer2D);

    return app;
}