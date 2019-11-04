#pragma once

#include "Shinobu/Core/Log.h"
#include "Shinobu/Core/Application.h"

int main(int argc, char** argv)
{
    sh::Log::Init();
    SH_CORE_INFO("Entrypoint is valid!");

    auto instance = sh::CreateApplication();
    instance->Run();

    return 0;
}