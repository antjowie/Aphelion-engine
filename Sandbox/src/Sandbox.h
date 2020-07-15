#pragma once
#include <Shinobu/Common.h>
#include <Shinobu/Net/Client.h>
#include <Shinobu/Net/Server.h>
#include <Shinobu/Event/NetEvent.h>

#include <Shinobu/ECS/Scene.h>

class ClientLayer : public sh::Layer
{
public:
    ClientLayer() : Layer("ClientLayer"), m_camera(16.f / 9.f) {}

    static sh::Entity LocalIDToNet(sh::Entity localID);
    static sh::Entity NetIDtoLocal(sh::Entity netID);

    virtual void OnAttach() override;
    virtual void OnDetach() override;
    virtual void OnEvent(sh::Event& event) override;
    virtual void OnUpdate(sh::Timestep ts) override;

private:
    sh::OrthographicCameraController m_camera;
    sh::Scene m_scene;

    // This is needed since the ID on the client does not always match with the server
    // (spawning predicted entities, not everyone has the same entities)
    // TODO: Move this into some kind of netcode interface since it couples the ECS for no reason
    // I also think that the user should not be thinking about which id context they are referring to
    // it should be handled by a server client layer. 
    // But how would it know which components contain entity IDs? Since it has to convert them
    // to server IDs. I don't know yet so for now the client is responsible for converting it themselves.
    static std::unordered_map<sh::Entity, sh::Entity> m_netToLocal;
};

class ServerLayer : public sh::Layer
{
public:
    ServerLayer() : Layer("ServerLayer"), m_camera(16.f / 9.f) {}

    virtual void OnEvent(sh::Event& event) override;
    virtual void OnAttach() override;
    virtual void OnDetach() override;
    virtual void OnUpdate(sh::Timestep ts) override final;

private:
    sh::OrthographicCameraController m_camera;
    sh::Scene m_scene;
};

class MainMenuLayer : public sh::Layer
{
public:
    MainMenuLayer() : Layer("MainMenuLayer") {}
    sh::Layer* m_client = nullptr;
    sh::Layer* m_server = nullptr;

    virtual void OnEvent(sh::Event& event) override;
    virtual void OnGuiRender() override;
};

//std::unique_ptr<sh::Application> sh::CreateApplication();