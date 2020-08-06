#include "EntryPoint.h"

int main(int argc, char** argv)
{
    // Initialize systems
    ap::Log::Init();
    AP_CORE_TRACE("Logger initialized");

    // Execute engine
    auto instance = ap::CreateApplication();
    instance->Run();

    return 0;
}