#pragma once

#include "Shinobu/Core/Log.h"

namespace sh
{
    extern void* CreateApplication();
}

int main(int argc, char** argv)
{
    sh::Log::Init();
    SH_CORE_INFO("Entrypoint is valid!");

    sh::CreateApplication();
}