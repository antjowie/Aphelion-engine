#include "Client.h"
#include "SceneStats.h"
#include "Component/Component.h"
#include "Component/ChunkComponent.h"
#include "System/System.h"
#include "System/ChunkSystem.h"

#include "Shinobu/Core/Application.h"
#include "Shinobu/Event/NetEvent.h"
#include "Shinobu/Net/Client.h"
#include "Shinobu/Net/ClientLayer.h"


std::unordered_map<sh::Entity, sh::Entity> ClientLayer::m_netToLocal;

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
    m_scene.RegisterSystem(InputSystem(m_camera.GetCamera()));
    m_scene.RegisterSystem(DeathSystem);
    m_scene.RegisterSystem(DrawSystem(m_camera.GetCamera()));

    m_scene.RegisterSystem(ChunkGenerateSystem);
    m_scene.RegisterSystem(ChunkRenderSystem(m_camera.GetCamera()));

    // TEMP: Spawn some nice chunks here
    auto& reg = m_scene.GetRegistry();

    for(int x = -2; x < 2; x++)
        for(int z = -2; z < 2; z++)
        {
            auto entity = reg.Create();
            auto& data = reg.Get().emplace<ChunkDataComponent>(entity);
            reg.Get().emplace<ChunkMeshComponent>(entity);
            reg.Get().emplace<ChunkModifiedComponent>(entity);

            data.pos = glm::vec3(x * chunkDimensions.x, -10.f, z * chunkDimensions.z);
        }

    m_scene.SetOnEntityCreateCb([this](sh::Entity entity)
    {
    });
    m_scene.SetOnEntityDestroyCb([this](sh::Entity entity)
    {
        m_netToLocal.erase(LocalIDToNet(entity));
    });

    m_camera.GetCamera().transform.Move(sh::Transform::GetWorldForward() * 5.f);
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
            auto newT = sh::Deserialize<sh::Transform>(p);

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
