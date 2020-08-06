#pragma once
/**
 * We supply the entry point of a program here since the user does not have to be 
 * with it
 */
#include "Aphelion/Core/Application.h"

namespace ap
{
    extern std::unique_ptr<ap::Application> CreateApplication();
}

//int main(int argc, char** argv)
//{
//    ap::Log::Init();
//    SH_CORE_INFO("Logger initialized");
//
//    auto instance = ap::CreateApplication();
//    instance->Run();
//
//    return 0;
//}