#include "Core/Base.h"
#include "Core\ProcessorInfo.h"

#ifdef PLATFORM_WINDOWS
#include "Platform/Windows/WindowsProcessorInfo.h"
#else
#include "Platform/Linux/LinuxProcessorInfo.h"
#endif

void ProcessorInfo::Display()
{
#ifdef PLATFORM_WINDOWS
    WindowsProcessorInfo::Display(); //Logical Processor
#else
    LinuxProcessorInfo::Display():
#endif
}
