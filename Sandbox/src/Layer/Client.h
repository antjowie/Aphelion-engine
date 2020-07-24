#pragma once
#include "Shinobu/Core/Layer.h"
#include "Shinobu/ECS/Scene.h"
#include "Shinobu/Renderer/PerspectiveCameraController.h"

#include "PacketBuffer.h"

class ClientLayer : public sh::Layer
{
public:
    ClientLayer() : Layer("ClientLayer"), m_camera(glm::radians(45.f),16.f/9.f) {}

    static sh::Entity LocalIDToNet(sh::Entity localID);
    static sh::Entity NetIDtoLocal(sh::Entity netID);

    virtual void OnAttach() override;
    virtual void OnDetach() override;
    virtual void OnEvent(sh::Event& event) override;
    virtual void OnUpdate(sh::Timestep ts) override;

    virtual void OnGuiRender() override;

private:

    sh::PerspectiveCameraController m_camera;
    sh::Scene m_scene;
    PacketBuffer m_packets;

    static std::unordered_map<sh::Entity, sh::Entity> m_netToLocal;
};
