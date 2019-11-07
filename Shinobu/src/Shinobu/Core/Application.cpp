#include "Shinobu/Core/Application.h"

namespace sh
{
    Application::Application()
        : m_window(Window::Create())
    {
        m_window->SetEventCallback(SH_BIND_EVENT_FN(Application::OnEvent));
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
        SH_CORE_INFO(event);
    }
} // namespace sh
