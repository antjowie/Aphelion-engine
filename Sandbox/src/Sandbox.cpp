#include <Shinobu/Shinobu.h>

std::unique_ptr<sh::Application> sh::CreateApplication()
{
    int val = 5;
    SH_WARN("Hello world with a random var{0}", 5);
    SH_ERROR("This is a fake crash");

    return std::make_unique<sh::Application>();
}