#include "Shinobu/Core/Application.h"
#include "Shinobu/ImGui/ImGuiLayer.h"

// Temporary, this class should not know about GLFW
#include <glad/glad.h>
#include <GLFW/glfw3.h>

namespace sh
{
    Application* Application::m_instance = nullptr;

    Application::Application()
        : m_imguiLayer(nullptr)
    {
        SH_CORE_ASSERT(!m_instance,"Can not create multiple Applications");
        m_instance = this;

        m_window = Window::Create();
        m_window->SetEventCallback(SH_BIND_EVENT_FN(Application::OnEvent));
        
        m_imguiLayer = new ImGuiLayer();
        m_layerStack.PushOverlay(m_imguiLayer);
    }

    void Application::Run()
    {
        while (true)
        {
            m_imguiLayer->Begin();
            
            // Temporary. This should be inside of a renderer class
            int display_w, display_h;
            glfwGetFramebufferSize((GLFWwindow*)m_window->GetNativeWindow(), &display_w, &display_h);
            glViewport(0, 0, display_w, display_h);
            glClearColor(0.5f, 0.f, 0.5f, 1.f);
            glClear(GL_COLOR_BUFFER_BIT);
            
            for (auto layer = m_layerStack.begin(); layer != m_layerStack.end(); layer++)
            {
                (*layer)->OnUpdate();
            }
            m_imguiLayer->End();
            m_window->OnUpdate();
        }
    }

    void Application::OnEvent(Event& event)
    {
        SH_CORE_TRACE(event);
    }
} // namespace sh
