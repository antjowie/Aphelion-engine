#include "Shinobu/Core/Application.h"

#include "Shinobu/ImGui/ImGuiLayer.h"

#include "Shinobu/Renderer/RendererAPI.h"
#include "Shinobu/Renderer/RenderCommand.h"

namespace sh
{
    Application* Application::m_instance = nullptr;

    Application::Application()
        : m_imguiLayer(nullptr)
        , m_isRunning(true)
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
        RenderCommand::SetClearColor(0.5f, 0.f, 0.5f, 1.f);


        while (m_isRunning)
        {
            m_imguiLayer->Begin();
            
            RenderCommand::Clear();

            for (auto layer = m_layerStack.begin(); layer != m_layerStack.end(); layer++)
            {
                (*layer)->OnEvent(LayerUpdateEvent());
                (*layer)->OnEvent(LayerGuiRenderEvent());
            }

            m_imguiLayer->End();

            // TODO: remove this. Temp render code


            m_window->OnUpdate();
        }
    }

    void Application::OnEvent(Event& event)
    {
        sh::EventDispatcher dispatcher(event);
        dispatcher.Dispatch<WindowCloseEvent>(SH_BIND_EVENT_FN(Application::OnWindowClose));
       
        // TODO: Make layertype be of type standard container to support standard container opperations (rend in this case)
        for (auto layer = m_layerStack.end() - 1; /*layer != m_layerStack.begin() - 1*/; layer--)
        {
            if(event.m_handled) return;
            (*layer)->OnEvent(event);

            // Calling -- on begin() is incorrect behavior
            if (layer == m_layerStack.begin()) return;
        }
    }
    
    void Application::OnWindowClose(WindowCloseEvent& event)
    {
        m_isRunning = false;
    }
    

} // namespace sh
