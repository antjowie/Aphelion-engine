#pragma once
#include "Client.h"
#include "Server.h"

#include "Aphelion/Core/Application.h"
#include "Aphelion/Core/Layer.h"
#include "Aphelion/Core/Input/KeyCodes.h"
#include "Aphelion/Event/KeyEvent.h"
#include "Aphelion/Event/NetEvent.h"

class MainMenuLayer : public ap::Layer
{
public:
    MainMenuLayer() : Layer("MainMenuLayer") {}
    ap::Layer* m_client = nullptr;
    ap::Layer* m_server = nullptr;

    void OnEvent(ap::Event& event) override
    {
        ap::EventDispatcher d(event);
        d.Dispatch<ap::KeyPressedEvent>([&](ap::KeyPressedEvent& e)
            {
            if (e.GetKeyCode() == ap::KeyCode::Escape)
                ap::Application::Get().Exit();
            return false;
            });
    }

    void OnGuiRender() override
    {
        //Sleep(500);
        if (!ImGui::Begin("Options"))
            return;

        // TODO: If server or client fails this doesn't get updated
        // it should be communicated via events
        static bool client = false;
        static char serverIP[32] = "127.0.0.1";
        if (ImGui::Checkbox("Client", &client))
        {
            if (client)
            {
                m_client = new ClientLayer();
                ap::Application::Get().GetLayerStack().PushLayer(m_client);
                ap::Application::Get().OnEvent(ap::ClientConnectRequestEvent(serverIP, 25565));
            }
            else
            {
                ap::Application::Get().GetLayerStack().PopLayer(m_client);
                delete m_client;
                m_client = nullptr;
            }
        }

        ImGui::SameLine(); ImGui::Text("ip ");
        ImGui::SameLine(); ImGui::InputText("#IP", serverIP, 32);

        static bool server = false;
        if (ImGui::Checkbox("Server", &server))
        {
            if (server)
            {
                m_server = new ServerLayer();
                ap::Application::Get().GetLayerStack().PushLayer(m_server);
            }
            else
            {
                ap::Application::Get().GetLayerStack().PopLayer(m_server);
                delete m_server;
                m_server = nullptr;
            }
        }

        ImGui::End();
    }
};
