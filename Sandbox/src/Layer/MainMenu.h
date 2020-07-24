#pragma once
#include "Client.h"
#include "Server.h"

#include "Shinobu/Core/Application.h"
#include "Shinobu/Core/Layer.h"
#include "Shinobu/Core/Input/KeyCodes.h"
#include "Shinobu/Event/KeyEvent.h"
#include "Shinobu/Event/NetEvent.h"

class MainMenuLayer : public sh::Layer
{
public:
    MainMenuLayer() : Layer("MainMenuLayer") {}
    sh::Layer* m_client = nullptr;
    sh::Layer* m_server = nullptr;

    void OnEvent(sh::Event& event) override
    {
        sh::EventDispatcher d(event);
        d.Dispatch<sh::KeyPressedEvent>([&](sh::KeyPressedEvent& e)
            {
            if (e.GetKeyCode() == sh::KeyCode::Escape)
                sh::Application::Get().Exit();
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
                sh::Application::Get().GetLayerStack().PushLayer(m_client);
                sh::Application::Get().OnEvent(sh::ClientConnectRequestEvent(serverIP, 25565));
            }
            else
            {
                sh::Application::Get().GetLayerStack().PopLayer(m_client);
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
                sh::Application::Get().GetLayerStack().PushLayer(m_server);
            }
            else
            {
                sh::Application::Get().GetLayerStack().PopLayer(m_server);
                delete m_server;
                m_server = nullptr;
            }
        }

        ImGui::End();
    }
};
