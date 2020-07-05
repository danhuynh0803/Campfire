#ifndef BASE_H
#define BASE_H

#include <memory>

// TODO where to put so all files have access? Maybe a default header to be added to all other header files?
//typedef uint_8t  UINT8
//typedef uint_16t UINT16
//typedef uint_32t UINT32
//typedef int_8t   INT8
//typedef int_16t  INT16
//typedef int_32t  INT32
// etc


// Platform information:
// TODO APPLE requires fallback conditions since only openGL 4.1 supported
// TODO check if win32 is supported
// TODO move stuff from resource manager into here
#ifdef _WIN32
    #ifdef _WIN64
        #define PLATFORM_WINDOWS
    #else
        #error "x86 Builds are not supported!"
    #endif

#elif defined(__APPLE__) || defined(__MACH__)
#elif defined(__linux__)
#else
#endif // Platform detection

#ifdef DEBUG
    #if defined(PLATFORM_WINDOWS)
    #endif

    #define ENABLE_ASSERTS
#endif

#ifdef ENABLE_ASSERTS
#define ASSERT(x, ...) { if (!(x)) {LOG_ERROR("Assertion Failed: {0}", __VA_ARGS__); } }

#else

#endif


#define BIT(x) (1 << x)

#define BIND_EVENT_FN(fn) std::bind(&fn, this, std::placeholders::_1)

template<typename T>
using UniquePtr = std::unique_ptr<T>;
template<typename T, typename ... Args>
constexpr std::unique_ptr<T> CreateUniquePtr(Args&& ... args)
{
    return std::make_unique<T>(std::forward<Args>(args)...);
}

template<typename T>
using SharedPtr = std::shared_ptr<T>;
template<typename T, typename ... Args>
constexpr std::shared_ptr<T> CreateSharedPtr(Args&& ... args)
{
    return std::make_shared<T>(std::forward<Args>(args)...);
}

#endif // BASE_H
