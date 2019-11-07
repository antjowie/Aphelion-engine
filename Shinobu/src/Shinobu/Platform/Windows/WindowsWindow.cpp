#include "WindowsWindow.h"
#include <GLFW/glfw3.h>

namespace sh
{
    std::unique_ptr<Window> Window::Create(WindowProps props)
    {
        return std::make_unique<WindowsWindow>(props);
    }

    WindowsWindow::WindowsWindow(WindowProps& props)
        : m_props(props) 
    {
        glfwInit();
        m_window = glfwCreateWindow(m_props.width, m_props.height, m_props.title.c_str(), nullptr, nullptr);
        SH_CORE_ASSERT(m_window, "glfw window can't be created");
        if (m_window)
        {
            glfwSetWindowUserPointer(m_window, &m_props);
            SH_CORE_INFO("Created Windows window");
        }
        else
        {
            SH_CORE_ASSERT(false, "Could not create a glfw window");
        }
    }

    WindowsWindow::~WindowsWindow()
    {
        glfwTerminate();
    }

    void WindowsWindow::OnUpdate()
    {
        glfwPollEvents();
        glfwSwapBuffers(m_window);
    }

}