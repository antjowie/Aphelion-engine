#pragma once
/**
 * We supply the entry point of a program here since the user does not have to be 
 * with it
 */

#include "Shinobu/Core/Log.h"

int main(int argc, char** argv)
{
    sh::Log::Init();
    SH_CORE_INFO("Logger initialized");

    auto instance = sh::CreateApplication();
    instance->Run();

    return 0;
}