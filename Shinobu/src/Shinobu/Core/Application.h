#pragma once
#include "Shinobu/Core/Core.h"
#include "Shinobu/Core/LayerStack.h"
#include "Shinobu/Core/Window.h"

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
    
    private:
        void OnEvent(Event& event);

        std::unique_ptr<Window> m_window;

        LayerStack m_layerStack;
    };

    extern std::unique_ptr<Application> CreateApplication();
} // namespace sh
