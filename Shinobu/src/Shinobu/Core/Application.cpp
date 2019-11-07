#include "Shinobu/Core/Application.h"

namespace sh
{
    Application::Application()
    {
        // Some dummy layers to check if things are working
        Layer* game = new Layer("GameLayer");
        m_layerStack.PushLayer(game);
        m_layerStack.PushOverlay(new Layer("GUILayer"));
        m_layerStack.PushLayer(new Layer("MenuLayer"));
        m_layerStack.PopOverlay(game);
        m_layerStack.PopLayer(game);
    }

    void Application::Run()
    {
        while (true)
        {
            for (auto layer = m_layerStack.begin(); layer != m_layerStack.end(); layer++)
            {
                (*layer)->OnUpdate();
            }
        }
    }
} // namespace sh
