#include "Aphelion/Core/Window.h"

#include "Platform/Windows/WindowsWindow.h"

namespace ap
{
    std::unique_ptr<Window> Window::Create(WindowProps props)
    {
#ifdef AP_PLATFORM_WINDOWS
        return std::make_unique<WindowsWindow>(props);
#elif
        AP_CRITICAL("Unknown platform!");
#endif // AP_PLATFORM_WINDOWS
    }
}