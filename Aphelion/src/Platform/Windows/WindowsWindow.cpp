#include "Platform/Windows/WindowsWindow.h"

#include "Aphelion/Core/Event/ApplicationEvent.h"
#include "Aphelion/Core/Event/MouseEvent.h"
#include "Aphelion/Core/Event/KeyEvent.h"

#include <GLFW/glfw3.h>

namespace ap
{
    WindowsWindow::WindowsWindow(WindowProps& props)
        : m_props(props) 
    {
        AP_CORE_VERIFY(glfwInit(), "GLFW failed to initialize");

        m_window = glfwCreateWindow(m_props.width, m_props.height, m_props.title.c_str(), nullptr, nullptr);

        AP_CORE_ASSERT(m_window, "GLFW window can't be created");
        glfwSetWindowUserPointer(m_window, &m_props);

        m_context = GraphicsContext::Create(static_cast<void*>(m_window));
        m_context->Init();

        // Set GLFW callbacks
        glfwSetErrorCallback([](int code, const char* message)
        {
           AP_CORE_ERROR("GLFW Error ({0}): {1}", code, message); 
        });
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

        glfwSetWindowPosCallback(m_window, [](GLFWwindow* window, int xpos, int ypos)
            {
                WindowProps& data = *(WindowProps*)glfwGetWindowUserPointer(window);

                WindowMoveEvent event(xpos, ypos);
                data.eventCallback(event);
            }
        );

        glfwSetWindowIconifyCallback(m_window, [](GLFWwindow* window, int iconify)
            {
                WindowProps& data = *(WindowProps*)glfwGetWindowUserPointer(window);

                WindowIconifyEvent event(iconify);
                data.eventCallback(event);
            }
        );

        glfwSetWindowFocusCallback(m_window, [](GLFWwindow* window, int focused)
            {
                WindowProps& data = *(WindowProps*)glfwGetWindowUserPointer(window);

                WindowFocusEvent event(focused);
                data.eventCallback(event);

            }
        );

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

        AP_CORE_INFO("Created Windows window");
    }

    WindowsWindow::~WindowsWindow()
    {
        glfwTerminate();
    }

    void WindowsWindow::OnUpdate()
    {
        glfwPollEvents();
        m_context->SwapBuffers();
    }

    void WindowsWindow::SetVSync(bool enable)
    {
        glfwSwapInterval(enable ? 1 : 0);
        AP_CORE_TRACE("VSync set to {0}", enable ? "Enabled" : "Disabled");
    }
}