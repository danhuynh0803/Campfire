#include <Core/Log.h>
#include <spdlog/sinks/stdout_color_sinks.h>

std::shared_ptr<spdlog::logger> Log::coreLogger;
std::shared_ptr<spdlog::logger> Log::clientLogger;
std::shared_ptr<LogWidget> Log::logWidget;

void Log::Init()
{
    // Core outputs to console
    spdlog::set_pattern("%^[%T] %n: %v%$");
    coreLogger = spdlog::stdout_color_mt("App");
    coreLogger->set_level(spdlog::level::trace);

    // Client outputs to editor log
    auto widget_sink = std::make_shared<widget_sink_mt>();
    clientLogger = std::make_shared<spdlog::logger>("Campfire", widget_sink);
    clientLogger->set_level(spdlog::level::trace);
    logWidget = std::make_shared<LogWidget>();
}
