#pragma once
#include "Shinobu/Core/Core.h"

namespace sh
{
    class SHINOBU_API Application
    {
    public:
        Application();
        virtual ~Application() = default;

        void Run();
    private:
    };

    extern std::unique_ptr<Application> CreateApplication();
} // namespace sh
