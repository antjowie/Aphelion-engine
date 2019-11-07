#include "WindowsWindow.h"


namespace sh
{
    std::unique_ptr<Window> Window::Create(WindowProps props)
    {
        return std::make_unique<WindowsWindow>(props);
    }

    WindowsWindow::WindowsWindow(WindowProps& props)
        : m_props(props) 
    {
        SH_CORE_INFO("Created Windows window");
    }

    void WindowsWindow::OnUpdate()
    {
    }

}