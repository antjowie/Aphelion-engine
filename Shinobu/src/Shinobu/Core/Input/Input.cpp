#include "Shinobu/Core/Input/Input.h"

#include "Platform/Windows/WindowsInput.h"

namespace sh
{
    std::unique_ptr<Input> Input::m_instance = Input::Create();

    std::unique_ptr<Input> Input::Create()
    {
#ifdef SH_PLATFORM_WINDOWS
        return std::make_unique<WindowsInput>();
#elif
#error "Platform not supported"
#endif // SH_PLATFORM_WINDOWS
    }
}