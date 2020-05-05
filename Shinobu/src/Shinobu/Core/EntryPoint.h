#pragma once
/**
 * We supply the entry point of a program here since the user does not have to be 
 * with it
 */
#include "Shinobu/Core/Application.h"

namespace sh
{
    extern std::unique_ptr<sh::Application> CreateApplication();
}

//int main(int argc, char** argv)
//{
//    sh::Log::Init();
//    SH_CORE_INFO("Logger initialized");
//
//    auto instance = sh::CreateApplication();
//    instance->Run();
//
//    return 0;
//}