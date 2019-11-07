#pragma once
#include "Shinobu/Core/Core.h"
#include "Shinobu/Core/LayerStack.h"

namespace sh
{
    class SHINOBU_API Application
    {
    public:
        Application();
        virtual ~Application() = default;

        void Run();
    
    private:

        LayerStack m_layerStack;
    };

    extern std::unique_ptr<Application> CreateApplication();
} // namespace sh
