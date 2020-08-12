#pragma once
#include "Aphelion/Core/Layer.h"
#include "PacketBuffer.h"

#include "Aphelion/ECS/Scene.h"

class ServerLayer : public ap::Layer
{
public:
    ServerLayer() : Layer("ServerLayer") {}

    virtual void OnEvent(ap::Event& event) override;
    virtual void OnAttach() override;
    virtual void OnDetach() override;
    virtual void OnUpdate(ap::Timestep ts) override final;

    virtual void OnGuiRender() override final;

private:
    ap::Scene m_scene;
    PacketBuffer m_packets;

    // Map of player guid
    std::unordered_map<ENetPeer*, unsigned> m_players;
};
