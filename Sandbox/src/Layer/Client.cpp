#include "Client.h"
#include "SceneStats.h"
#include "Component/Component.h"
#include "Component/ChunkComponent.h"
#include "System/System.h"
#include "System/ChunkSystem.h"
#include "System/PlayerSystem.h"

#include "Aphelion/Core/Event/MouseEvent.h"
#include "Aphelion/Core/Event/KeyEvent.h"
#include "Aphelion/Core/Application.h"
#include "Aphelion/Core/Event/NetEvent.h"
#include "Aphelion/Net/Client.h"
#include "Aphelion/Net/ClientLayer.h"

void ClientLayer::OnAttach()
{
    m_scene.RegisterSystem(InputSystem(m_camera.GetCamera()));
    m_scene.RegisterSystem(DeathSystem);
    m_scene.RegisterSystem(DrawSystem(m_camera.GetCamera()));

    m_scene.RegisterSystem(ChunkHandlerSystem(m_chunks));
    //m_scene.RegisterSystem(ChunkMeshBuilderSystem);
    //m_scene.RegisterSystem(ChunkRenderSystem(m_camera.GetCamera()));

    m_scene.RegisterSystem(PlayerView(m_camera.GetCamera()));
    
    //m_camera.GetCamera().transform.SetPosition(glm::vec3(30, -5, 100));
    m_camera.GetCamera().transform.SetPosition(glm::vec3(3, 8, 16));
    //m_camera.GetCamera().transform.LookAt(glm::vec3(1, 0, 0));

#ifdef AP_DEBUG
    m_scene.SetOnEntityCreateCb([](ap::Entity entity)
    {
        //AP_INFO("Created {}", entity.GetComponent<ap::GUIDComponent>());
    });
    m_scene.SetOnEntityDestroyCb([](ap::Entity entity)
    {
        //AP_WARN("Destroyed {}", entity.GetComponent<ap::GUIDComponent>());    
    });
#endif
    //m_camera.GetCamera().transform.Move(ap::Transform::GetWorldForward() * 5.f);
    ap::Input::SetCursorPos(glm::vec2(0));
    m_camera.Enable(true);
}

void ClientLayer::OnDetach()
{
    // TODO: add timeout variable to this so that we can just do 0 instead of 5
    ap::Application::Get().OnEvent(ap::ClientDisconnectRequestEvent());
}

/**
 * This however is an issue when reconciling since the simulation runs again, 
 * so the client sends a bunch of packets. I only want to send packets at the 
 * end of the most recent simulation. 
 * To solve this, when intercepting packets. I only push them if we are not reconciling
 */
bool clientIsReconciling = false;

void ClientLayer::OnEvent(ap::Event& event)
{
    ap::EventDispatcher d(event);
    //d.Dispatch<ap::MouseButtonPressedEvent>([&](ap::MouseButtonPressedEvent& e)
    //    {
    //        if (e.GetButton() == ap::ButtonCode::Right)
    //            m_camera.Enable(true);
    //    });
    //d.Dispatch<ap::MouseButtonReleasedEvent>([&](ap::MouseButtonReleasedEvent& e)
    //    {
    //        if (e.GetButton() == ap::ButtonCode::Right)
    //            m_camera.Enable(false);
    //    });
    m_camera.OnEvent(event);

    if (d.Dispatch<ap::WindowFocusEvent>([&](ap::WindowFocusEvent& e)
        {
            m_camera.Enable(e.IsFocused());
            return false;
        }));


    if (d.Dispatch<ap::WindowIconifyEvent>([&](ap::WindowIconifyEvent& e)
        {
            m_camera.Enable(!e.IsIconified());
            return false;
        }));

    if (d.Dispatch<ap::ClientReceivePacketEvent>([&](ap::ClientReceivePacketEvent& e)
        {
            m_packets.Push(e.GetPacket(),true);
            return false;
        })) return;

    // Honestly, this may be quite a dumb way of doing it.
    // This is because the event gets modified along the way so it may get confusing where the event will pass through
    // and where not
    if (d.Dispatch<ap::ClientSendPacketEvent>([&](ap::ClientSendPacketEvent& e)
        {
            e.GetPacket().clientSimulation = m_scene.GetSimulationCount();
            return clientIsReconciling;
        })) return;
}

void ClientLayer::OnUpdate(ap::Timestep ts)
{
    auto& client = ap::NetClient::Get();
    if (!client.IsConnected()) return;

    m_camera.OnUpdate(ts);
    //m_camera.GetCamera().transform.Rotate(ap::Radians(glm::vec3(0, ts * 180.f, 0)));

    // Poll packets
    ap::Packet p;
    m_packets.Swap();
    clientIsReconciling = true;
    auto& reg = m_scene.GetRegistry();
    while(m_packets.Poll(p))
    {
        //auto netID = ap::Entity(p.entity);
        //auto match = m_netToLocal.find(netID);
        //if (match == m_netToLocal.end()) { m_netToLocal[netID] = m_scene.GetRegistry().Create(); }
        //auto local = m_netToLocal[netID];

        auto guid = p.guid;
        ap::Entity entity;
        if (!reg.Has(guid))
        {
            entity = (guid == 0 ? reg.Create() : reg.Create(guid));
        }
        else
        {
            entity = reg.Get(guid);
        }

        unsigned delta = m_scene.GetSimulationCount() - p.clientSimulation;

        //AP_TRACE("Received guid {}",entity.GetComponent<ap::GUIDComponent>());
        // We only want to reconcile player input
        if (guid == 0 || !entity.HasComponent<Player>())
        {
            m_scene.GetRegistry().HandlePacket(guid, p);
        }
        else if (
            entity.HasComponent<Player>() && p.clientSimulation != -1 && 
            !m_scene.GetRegistry(delta).HandleAndReconcilePacket(guid, p))
        {
            auto newT = ap::Deserialize<ap::Transform>(p);

            AP_WARN("Reconciliation!!!");
            // TODO: Reconciliate subsequent registries
            auto player = entity.GetComponent<ap::Transform>() = newT;
            //m_scene.GetRegistry().HandlePacket(local, p);
        }
    }
    clientIsReconciling = false;

    m_chunks.Update();
    m_chunks.Render(m_camera.GetCamera());
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
            ap::NetClientLayer::m_config.rate = 1.f / (float)tick;

#ifdef AP_DEBUG
            if (ImGui::CollapsingHeader("Inspector"))
            {
                const auto& entities = m_scene.GetRegistry().GetEntities();
                int i = 0;
                for (auto e : entities)
                {
                    const auto& name = e.GetComponent<ap::TagComponent>().tag;

                    if (ImGui::TreeNode((void*)(intptr_t)i, "%s", name.empty() ? "Entity" : name.c_str()))
                    {
                        auto& components = e.GetComponentIDs();
                        std::set<unsigned> set(components.begin(),components.end());
                        for (auto comp : set)
                            ImGui::Text("%s", ap::Registry::GetComponentData().at(comp).name.data());
                        ImGui::TreePop();
                    }
                    ++i;
                }
            }

#endif // AP_DEBUG
        }
        ImGui::SliderFloat("Movespeed", &movespeed, 0.f, 5.f);
        ImGui::End();
    }
}
