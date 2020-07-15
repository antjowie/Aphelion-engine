#pragma once
#include "Shinobu/Core/Core.h"
#include "Shinobu/Core/LayerStack.h"
#include "Shinobu/Core/Window.h"

#include "Shinobu/Event/ApplicationEvent.h"

#include "Shinobu/ImGui/ImGuiLayer.h"

#include <memory>

namespace sh
{
    /**
     * Application is the engine instance. It is responsible for communicating events between layers
     * and initializing systems. 
     */
    class SHINOBU_API Application
    {
    public:
        Application(WindowProps props = {});
        virtual ~Application();

        void Run();

        inline LayerStack& GetLayerStack() { return m_layerStack; }
        inline Window& GetWindow() { return *m_window; }
        static inline Application& Get() { return *m_instance; } 

        inline void Exit() { m_isRunning = false; }

        void OnEvent(Event& event);
    private:
        bool OnWindowClose(WindowCloseEvent& event);

        std::unique_ptr<Window> m_window;
        LayerStack m_layerStack;
        ImGuiLayer* m_imguiLayer;

        bool m_isRunning;

        static Application* m_instance;
    };
} // namespace sh
