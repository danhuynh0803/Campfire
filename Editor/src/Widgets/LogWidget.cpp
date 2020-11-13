#include "Widgets/LogWidget.h"

std::shared_ptr<spdlog::logger> LogWidget::mLogger = nullptr;
std::shared_ptr<ImGuiLog> LogWidget::mImGuiLog = nullptr;

void LogWidget::Init()
{
    mImGuiLog = std::make_shared<ImGuiLog>();
    auto widget_sink = std::make_shared<widget_sink_mt>();
    mLogger = std::make_shared<spdlog::logger>("Campfire", widget_sink);
}
