#include "Shinobu/Core/Application.h"

namespace sh
{
    Application::Application()
        : m_window(Window::Create())
    {
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
} // namespace sh
