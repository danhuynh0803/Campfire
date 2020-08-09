#include "Core/Base.h"
#include "Core/Window.h"

#ifdef PLATFORM_WINDOWS
#include "Platform/Windows/WindowsWindow.h"
#elif PLATFORM_LINUX
#include "Platform/Linux/LinuxWindow.h"
#endif

std::unique_ptr<Window> Window::Create(const WindowProps& props)
{
    //#ifdef PLATFORM_WINDOWS
    //    return CreateUniquePtr<WindowsWindow>(props);
    //#elif PLATFORM_LINUX
    //    return CreateUniquePtr<LinuxWindow>(props);
    //#endif

    #ifdef PLATFORM_WINDOWS
        return CreateUniquePtr<WindowsWindow>(props);
    #else
        return CreateUniquePtr<LinuxWindow>(props);
    #endif
}