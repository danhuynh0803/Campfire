#ifndef LOG_H
#define LOG_H

#include <memory>
#include "spdlog/spdlog.h"

class Log
{
public:
    static void Init();

    inline static std::shared_ptr<spdlog::logger>& GetCoreLogger() { return coreLogger; };
    inline static std::shared_ptr<spdlog::logger>& GetClientLogger() { return clientLogger; };

    // Display data into Imgui's log
    static void Imgui(...);

private:
    static std::shared_ptr<spdlog::logger> coreLogger;
    static std::shared_ptr<spdlog::logger> clientLogger;
};

// Log macros
#define LOG_IMGUI(...)    Log::Imgui(__VA_ARGS__)
#define LOG_TRACE(...)    Log::GetCoreLogger()->trace(__VA_ARGS__)
#define LOG_INFO(...)     Log::GetCoreLogger()->info(__VA_ARGS__)
#define LOG_WARN(...)     Log::GetCoreLogger()->warn(__VA_ARGS__)
#define LOG_ERROR(...)    Log::GetCoreLogger()->error(__VA_ARGS__)
#define LOG_CRITICAL(...) Log::GetCoreLogger()->critical(__VA_ARGS__)

#endif // LOG_H
