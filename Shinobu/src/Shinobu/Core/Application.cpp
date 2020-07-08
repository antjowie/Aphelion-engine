#include "Shinobu/Core/Application.h"
#include "Shinobu/Core/Time.h"

#include "Shinobu/ImGui/ImGuiLayer.h"
#include "Shinobu/Renderer/Renderer.h"
#include "Shinobu/Renderer/RenderCommand.h"

#include <enet/enet.h>

namespace sh
{
    Application* Application::m_instance = nullptr;

    Application::Application(WindowProps props)
        : m_imguiLayer(nullptr)
        , m_isRunning(true)
    {
        SH_CORE_ASSERT(!m_instance,"Can not create multiple Applications");
        m_instance = this;

        int status = enet_initialize();
        if (status == 0) SH_CORE_TRACE("Enet initialized");
        else SH_CORE_ERROR("Enet failed to initialize with error code {}", status);

        m_window = Window::Create(props);
        m_window->SetEventCallback(SH_BIND_EVENT_FN(Application::OnEvent));
        m_window->SetVSync(true);

        Renderer::Init();
        RenderCommand::SetClearColor(0.5f, 0.f, 0.5f, 1.f);
        
        m_imguiLayer = new ImGuiLayer();
        m_layerStack.PushOverlay(m_imguiLayer);
    }

    Application::~Application()
    {
        enet_deinitialize();

        m_instance = nullptr;
    }

    void Application::Run()
    {
        sh::Time time;
        while (m_isRunning)
        {
            const Timestep step(time.Elapsed());
            time.Reset();

            for (auto layer = m_layerStack.begin(); layer != m_layerStack.end(); layer++)
            {
                (*layer)->OnUpdate(step);
            }

            m_imguiLayer->Begin();
            for (auto layer = m_layerStack.begin(); layer != m_layerStack.end(); layer++)
            {
                (*layer)->OnGuiRender();
            }
            m_imguiLayer->End();

            m_window->OnUpdate();
            RenderCommand::Clear();
        }
    }

    void Application::OnEvent(Event& event)
    {
        sh::EventDispatcher dispatcher(event);
        dispatcher.Dispatch<WindowCloseEvent>(SH_BIND_EVENT_FN(Application::OnWindowClose));
        dispatcher.Dispatch<WindowResizeEvent>([](WindowResizeEvent& e)
        {
            Renderer::OnWindowResize(e.GetWidth(),e.GetHeight());
            return false;
        });

        // TODO: Make layertype be of type standard container to support standard container opperations (rend in this case)
        for (auto layer = m_layerStack.end() - 1; /*layer != m_layerStack.begin() - 1*/; layer--)
        {
            (*layer)->OnEvent(event);
            if(event.handled) return;

            // Calling -- on begin() is incorrect behavior
            if (layer == m_layerStack.begin()) return;
        }
    }
    
    bool Application::OnWindowClose(WindowCloseEvent& event)
    {
        m_isRunning = false;
        return false;
    }
    

} // namespace sh
