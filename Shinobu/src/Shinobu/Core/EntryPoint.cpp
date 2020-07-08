#include "EntryPoint.h"

int main(int argc, char** argv)
{
    // Initialize systems
    sh::Log::Init();
    SH_CORE_TRACE("Logger initialized");

    // Execute engine
    auto instance = sh::CreateApplication();
    instance->Run();

    return 0;
}