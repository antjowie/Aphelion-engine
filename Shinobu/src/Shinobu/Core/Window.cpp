#include "Shinobu/Core/Window.h"

#include "Platform/Windows/WindowsWindow.h"

namespace sh
{
    std::unique_ptr<Window> Window::Create(WindowProps props)
    {
#ifdef SH_PLATFORM_WINDOWS
        return std::make_unique<WindowsWindow>(props);
#elif
        SH_CRITICAL("Unknown platform!");
#endif // SH_PLATFORM_WINDOWS
    }
}