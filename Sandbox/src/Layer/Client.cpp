#include "Client.h"
#include "SceneStats.h"
#include "Component/Component.h"
#include "Component/ChunkComponent.h"
#include "System/System.h"
#include "System/ChunkSystem.h"

#include "Aphelion/Core/Application.h"
#include "Aphelion/Event/NetEvent.h"
#include "Aphelion/Net/Client.h"
#include "Aphelion/Net/ClientLayer.h"


std::unordered_map<ap::Entity, ap::Entity> ClientLayer::m_netToLocal;

ap::Entity ClientLayer::LocalIDToNet(ap::Entity localID) 
{
    for (const auto e : m_netToLocal)
        if (e.second == localID) return e.first;
    AP_CORE_ERROR("Local ID {} can't be mapped to a network ID", localID);
}

ap::Entity ClientLayer::NetIDtoLocal(ap::Entity netID)
{
    return m_netToLocal.at(netID);
}

void ClientLayer::OnAttach()
{
    m_scene.RegisterSystem(InputSystem(m_camera.GetCamera()));
    m_scene.RegisterSystem(DeathSystem);
    m_scene.RegisterSystem(DrawSystem(m_camera.GetCamera()));

    m_scene.RegisterSystem(ChunkStrategySystem);
    m_scene.RegisterSystem(ChunkMeshBuilderSystem);
    m_scene.RegisterSystem(ChunkRenderSystem(m_camera.GetCamera()));

    // TEMP: Spawn some nice chunks here
    auto& reg = m_scene.GetRegistry();
    for(int x = -2; x < 5; x++)
        for(int z = -2; z < 2; z++)
        {
            auto entity = reg.Create();
            auto& data = reg.Get().emplace<ChunkSpawnComponent>(entity);
            
            data.pos = glm::vec3(x * chunkDimensions.x, -40.f, z * chunkDimensions.z);
        }    

    m_scene.SetOnEntityCreateCb([this](ap::Entity entity)
    {
    });
    m_scene.SetOnEntityDestroyCb([this](ap::Entity entity)
    {
        m_netToLocal.erase(LocalIDToNet(entity));
    });

    m_camera.GetCamera().transform.Move(ap::Transform::GetWorldForward() * 5.f);
}

void ClientLayer::OnDetach()
{
    // TODO: add timeout variable to this so that we can just do 0 instead of 5
    ap::Application::Get().OnEvent(ap::ClientDisconnectRequestEvent());
    m_netToLocal.clear();
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
    
    // Poll packets
    ap::Packet p;
    m_packets.Swap();
    clientIsReconciling = true;
    while(m_packets.Poll(p))
    {
        auto netID = ap::Entity(p.entity);
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
            auto newT = ap::Deserialize<ap::Transform>(p);

            AP_WARN("Reconciliation!!!");
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
            ap::NetClientLayer::m_config.rate = 1.f / (float)tick;
        }
        ImGui::SliderFloat("Movespeed", &movespeed, 0.f, 5.f);
        ImGui::End();
    }
}
