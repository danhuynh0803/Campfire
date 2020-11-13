#include <Core/Log.h>
#include <spdlog/sinks/stdout_color_sinks.h>

std::shared_ptr<spdlog::logger> Log::coreLogger;

void Log::Init()
{
    spdlog::set_pattern("%^[%T] %n: %v%$");
    coreLogger = spdlog::stdout_color_mt("App");
    coreLogger->set_level(spdlog::level::trace);
}