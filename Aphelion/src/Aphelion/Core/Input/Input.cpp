#include "Aphelion/Core/Input/Input.h"

#include "Platform/Windows/WindowsInput.h"

namespace ap
{
    std::unique_ptr<Input> Input::m_instance = Input::Create();

    std::unique_ptr<Input> Input::Create()
    {
#ifdef AP_PLATFORM_WINDOWS
        return std::make_unique<WindowsInput>();
#elif
#error "Platform not supported"
#endif // AP_PLATFORM_WINDOWS
    }
}