#pragma once
#include "Aphelion/Core/Layer.h"
#include "Aphelion/ECS/Scene.h"
#include "Aphelion/Renderer/PerspectiveCameraController.h"

#include "PacketBuffer.h"

class ClientLayer : public ap::Layer
{
public:
    ClientLayer() : Layer("ClientLayer"), m_camera(glm::radians(45.f),16.f/9.f) {}

    //static ap::EntityID LocalIDToNet(ap::EntityID localID);
    //static ap::EntityID NetIDtoLocal(ap::EntityID netID);

    virtual void OnAttach() override;
    virtual void OnDetach() override;
    virtual void OnEvent(ap::Event& event) override;
    virtual void OnUpdate(ap::Timestep ts) override;

    virtual void OnGuiRender() override;

private:

    ap::PerspectiveCameraController m_camera;
    ap::Scene m_scene;
    PacketBuffer m_packets;

    //static std::unordered_map<ap::EntityID, ap::EntityID> m_netToLocal;
};
