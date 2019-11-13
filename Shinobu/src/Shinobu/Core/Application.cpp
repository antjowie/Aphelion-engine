#include "Shinobu/Core/Application.h"
#include "Shinobu/ImGui/ImGuiLayer.h"

namespace sh
{
    Application::Application()
        : m_window(Window::Create())
    {
        m_window->SetEventCallback(SH_BIND_EVENT_FN(Application::OnEvent));
        m_layerStack.PushOverlay(new ImGuiLayer());
    }

    void Application::Run()
    {
        while (true)
        {
            m_window->OnUpdate();
            for (auto layer = m_layerStack.begin(); layer != m_layerStack.end(); layer++)
            {
                (*layer)->OnUpdate();
            }
        }
    }

    void Application::OnEvent(Event& event)
    {
        SH_CORE_TRACE(event);
    }
} // namespace sh
