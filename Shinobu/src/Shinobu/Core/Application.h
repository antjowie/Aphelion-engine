#pragma once
#include "Core.h"

namespace sh
{
    class SHINOBU_API Application
    {
    public:
        Application();
        virtual ~Application();

        void Run();
    private:
    };

    extern std::unique_ptr<Application> CreateApplication();
} // namespace sh
