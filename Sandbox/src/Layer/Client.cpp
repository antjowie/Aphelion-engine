#include "Client.h"
#include "SceneStats.h"
#include "Component/Component.h"
#include "Component/ChunkComponent.h"
#include "System/System.h"
#include "System/ChunkSystem.h"

#include "Aphelion/Core/Application.h"
#include "Aphelion/Core/Event/NetEvent.h"
#include "Aphelion/Net/Client.h"
#include "Aphelion/Net/ClientLayer.h"

void ClientLayer::OnAttach()
{
    m_scene.RegisterSystem(InputSystem(m_camera.GetCamera()));
    m_scene.RegisterSystem(DeathSystem);
    m_scene.RegisterSystem(DrawSystem(m_camera.GetCamera()));

    m_scene.RegisterSystem(ChunkStrategySystem);
    m_scene.RegisterSystem(ChunkMeshBuilderSystem);
    m_scene.RegisterSystem(ChunkRenderSystem(m_camera.GetCamera()));

    m_camera.GetCamera().transform.SetPosition(glm::vec3(30, -5, 100));
    //m_camera.GetCamera().transform.LookAt(glm::vec3(1, 0, 0));

    // TEMP: Spawn some nice chunks here
    auto& reg = m_scene.GetRegistry();
    for(int x = -2; x < 5; x++)
        for(int z = -2; z < 2; z++)
        {
            auto entity = reg.Create();
            auto& data = entity.AddComponent<ChunkSpawnComponent>();
            
            data.pos = glm::vec3(x * chunkDimensions.x, -40.f, z * chunkDimensions.z);
        }    

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

    m_camera.GetCamera().transform.Move(ap::Transform::GetWorldForward() * 5.f);
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
    m_camera.OnEvent(event);

    ap::EventDispatcher d(event);

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
        }
        ImGui::SliderFloat("Movespeed", &movespeed, 0.f, 5.f);
        ImGui::End();
    }
}
