#include <Shinobu/Common.h>

std::unique_ptr<sh::Application> sh::CreateApplication()
{
    return std::make_unique<sh::Application>();
}