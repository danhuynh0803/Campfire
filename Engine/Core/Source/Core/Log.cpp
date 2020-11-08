#include <Core/Log.h>
/*

std::shared_ptr<spdlog::logger> Log::coreLogger;
std::shared_ptr<spdlog::logger> Log::clientLogger;
std::shared_ptr<LogWidget> Log::logWidget;

void Log::Init()
{
    spdlog::set_pattern("%^[%T] %n: %v%$");
    auto widget_sink = std::make_shared<widget_sink_mt>();
    coreLogger = std::make_shared<spdlog::logger>("Campfire", widget_sink);
    coreLogger->set_level(spdlog::level::trace);

    logWidget = std::make_shared<LogWidget>();
    // TODO split up logs between api and client later
    //clientLogger = spdlog::stdout_color_mt("App");
    //clientLogger->set_level(spdlog::level::trace);
}
*/
