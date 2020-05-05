#include "EntryPoint.h"

int main(int argc, char** argv)
{
    sh::Log::Init();
    SH_CORE_INFO("Logger initialized");

    auto instance = sh::CreateApplication();
    instance->Run();

    return 0;
}