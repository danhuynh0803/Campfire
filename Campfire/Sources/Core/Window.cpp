#include "Core/Base.h"
#include "Core/Window.h"

#ifdef PLATFORM_WINDOWS
#include "Platform/Windows/WindowsWindow.h"
#endif

std::unique_ptr<Window> Window::Create(const WindowProps& props)
{
    #ifdef PLATFORM_WINDOWS
        return CreateUniquePtr<WindowsWindow>(props);
    #else
        return nullptr;
    #endif
}
