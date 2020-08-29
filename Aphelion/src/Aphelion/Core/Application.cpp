#include "Aphelion/Core/Application.h"
#include "Aphelion/Core/Time.h"
#include "Aphelion/Core/ImGui/ImGuiLayer.h"

#include "Aphelion/Renderer/Renderer.h"
#include "Aphelion/Renderer/RenderCommand.h"

#include "Aphelion/Net/ClientLayer.h"
#include "Aphelion/Net/ServerLayer.h"

#include "Aphelion/ECS/Component.h"
#include "Aphelion/Physics/PhysicsLayer.h"


#include <enet/enet.h>

namespace ap
{
    Application* Application::m_instance = nullptr;

    Application::Application(WindowProps props)
        : m_imguiLayer(nullptr)
        , m_isRunning(true)
    {
        AP_CORE_ASSERT(!m_instance,"Can not create multiple Applications");
        m_instance = this;


        // Window
        m_window = Window::Create(props);
        m_window->SetEventCallback(AP_BIND_FN(Application::OnEvent));
        m_window->SetVSync(true);

        // Renderer
        Renderer::Init();
        RenderCommand::SetClearColor(0.5f, 0.f, 0.5f, 1.f);
        
        // GUI
        m_imguiLayer = new ImGuiLayer();
        m_layerStack.PushOverlay(m_imguiLayer);

        // ECS
        RegisterECSComponents();

        // Physics
        m_layerStack.PushLayer(new PhysicsLayer());

        // Net
        int status = enet_initialize();
        if (status == 0) AP_CORE_TRACE("Enet initialized");
        else AP_CORE_ERROR("ENet failed to initialize with error code {}", status);

        auto clientLayer = new NetClientLayer();
        clientLayer->SetEventCB(AP_BIND_FN(Application::OnEvent));
        m_layerStack.PushLayer(clientLayer);

        auto serverLayer = new NetServerLayer();
        serverLayer->SetEventCB(AP_BIND_FN(Application::OnEvent));
        m_layerStack.PushLayer(serverLayer);
    }

    Application::~Application()
    {
        m_layerStack.Clear();

        enet_deinitialize();
        AP_CORE_TRACE("ENet destroyed");

        Renderer::Deinit();

        m_instance = nullptr;
    }

    void Application::Run()
    {
        ap::Timer time;
        while (m_isRunning)
        {
            const Timestep step(time.Elapsed());
            time.Reset();
            Time::dt = step;
            Time::frameCount++;

            // If the step is longer than a second, assume that system got stuck and discard current frame
            if (step > 1.f)
                continue;

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
        ap::EventDispatcher dispatcher(event);
        dispatcher.Dispatch<WindowCloseEvent>(AP_BIND_FN(Application::OnWindowClose));
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
    

} // namespace ap
