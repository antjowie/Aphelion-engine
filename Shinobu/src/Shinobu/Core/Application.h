#pragma once
#include "Shinobu/Core/Core.h"
#include "Shinobu/Core/LayerStack.h"
#include "Shinobu/Core/Window.h"

#include "Shinobu/ImGui/ImGuiLayer.h"

namespace sh
{
    /**
     * Application is the engine instance. It is responsible for communicating events between layers
     * and initializing systems. 
     */
    class SHINOBU_API Application
    {
    public:
        Application();
        virtual ~Application() = default;

        void Run();

        inline Window& GetWindow() { return *m_window; }
        static inline Application& Get() { return *m_instance; } 

    private:
        void OnEvent(Event& event);

        std::unique_ptr<Window> m_window;
        LayerStack m_layerStack;
        ImGuiLayer* m_imguiLayer;

        static Application* m_instance;
    };

    extern std::unique_ptr<Application> CreateApplication();
} // namespace sh
