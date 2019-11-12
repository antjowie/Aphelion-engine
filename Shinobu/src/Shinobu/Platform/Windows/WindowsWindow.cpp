#include "Shinobu/Platform/Windows/WindowsWindow.h"

#include "Shinobu/Event/ApplicationEvent.h"
#include "Shinobu/Event/MouseEvent.h"
#include "Shinobu/Event/KeyEvent.h"

#include <glad/glad.h>
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
        SH_CORE_ASSERT(m_window, "GLFW window can't be created");
        
        glfwSetWindowUserPointer(m_window, &m_props);
        glfwMakeContextCurrent(m_window);
        gladLoadGLLoader(GLADloadproc(glfwGetProcAddress));

        // Set GLFW callbacks
        glfwSetWindowSizeCallback(m_window, [](GLFWwindow* window, int width, int height)
        {
            WindowProps& data = *(WindowProps*)glfwGetWindowUserPointer(window);
            data.width = width;
            data.height = height;

            WindowResizeEvent event(width, height);
            data.eventCallback(event);
        });

        glfwSetWindowCloseCallback(m_window, [](GLFWwindow* window)
        {
            WindowProps& data = *(WindowProps*)glfwGetWindowUserPointer(window);
            
            WindowCloseEvent event;
            data.eventCallback(event);
        });

        glfwSetKeyCallback(m_window, [](GLFWwindow* window, int key, int scancode, int action, int mods)
        {
            WindowProps& data = *(WindowProps*)glfwGetWindowUserPointer(window);

            switch (action)
            {
            case GLFW_PRESS:
            {
                KeyPressedEvent event(key, 0);
                data.eventCallback(event);
                break;
            }
            case GLFW_RELEASE:
            {
                KeyReleasedEvent event(key);
                data.eventCallback(event);
                break;
            }
            case GLFW_REPEAT:
            {
                KeyPressedEvent event(key, 1);
                data.eventCallback(event);
                break;
            }
            }
        });

        glfwSetCharCallback(m_window, [](GLFWwindow* window, unsigned int keycode)
        {
           WindowProps& data = *(WindowProps*)glfwGetWindowUserPointer(window);
        
           KeyTypedEvent event(keycode);
           data.eventCallback(event);
        });

        glfwSetMouseButtonCallback(m_window, [](GLFWwindow* window, int button, int action, int mods)
        {
            WindowProps& data = *(WindowProps*)glfwGetWindowUserPointer(window);

            switch (action)
            {
            case GLFW_PRESS:
            {
                MouseButtonPressedEvent event(button);
                data.eventCallback(event);
                break;
            }
            case GLFW_RELEASE:
            {
                MouseButtonReleasedEvent event(button);
                data.eventCallback(event);
                break;
            }
            }
        });

        glfwSetScrollCallback(m_window, [](GLFWwindow* window, double xOffset, double yOffset)
        {
            WindowProps& data = *(WindowProps*)glfwGetWindowUserPointer(window);

            MouseScrolledEvent event((float)xOffset, (float)yOffset);
            data.eventCallback(event);
        });

        glfwSetCursorPosCallback(m_window, [](GLFWwindow* window, double xPos, double yPos)
        {
            WindowProps& data = *(WindowProps*)glfwGetWindowUserPointer(window);

            MouseMovedEvent event((float)xPos, (float)yPos);
            data.eventCallback(event);
        });
    
        SH_CORE_INFO("Created Windows window");
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