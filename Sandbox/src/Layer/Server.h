#pragma once
#include "Shinobu/Core/Layer.h"
#include "PacketBuffer.h"

#include "Shinobu/ECS/Scene.h"

class ServerLayer : public sh::Layer
{
public:
    ServerLayer() : Layer("ServerLayer") {}

    virtual void OnEvent(sh::Event& event) override;
    virtual void OnAttach() override;
    virtual void OnDetach() override;
    virtual void OnUpdate(sh::Timestep ts) override final;

    virtual void OnGuiRender() override final;

private:
    sh::Scene m_scene;
    PacketBuffer m_packets;

    std::unordered_map<ENetPeer*, sh::Entity> m_players;
};
