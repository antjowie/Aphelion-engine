#include <Shinobu/Shinobu.h>
#include <iostream>

int main()
{
    int val = 5;
    SH_WARN("Hello world with a random var{0}", 5);
    SH_ERROR("This is a fake crash");

    std::cin.get();

    return 0;
}