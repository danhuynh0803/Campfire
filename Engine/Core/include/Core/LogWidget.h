#pragma once

#include <iostream>
#include <spdlog/spdlog.h>
#include <spdlog/sinks/ostream_sink.h>
#include <spdlog/sinks/base_sink.h>

#include "Core/ImGuiLog.h"

class LogWidget
{
public:
    static void Init();
    inline static void Draw(bool* isOpen) { mImGuiLog->Draw(isOpen); }
    inline static std::shared_ptr<ImGuiLog> GetImGuiLog() { return mImGuiLog; }
    inline static std::shared_ptr<spdlog::logger> GetLogger() { return mLogger; }
private:
    static std::shared_ptr<spdlog::logger> mLogger;
    static std::shared_ptr<ImGuiLog> mImGuiLog;
};

//=============================================================================
template<typename Mutex>
class widget_sink : public spdlog::sinks::base_sink <Mutex>
{
protected:
    void sink_it_(const spdlog::details::log_msg& msg) override
    {
        spdlog::memory_buf_t formatted;
        spdlog::sinks::base_sink<Mutex>::formatter_-> format(msg, formatted);
        LogWidget::GetImGuiLog()->AddLog(fmt::to_string(formatted).c_str());
    }

    void flush_() override
    {
        std::cout << std::flush;
    }
};

#include "spdlog/details/null_mutex.h"
#include <mutex>
using widget_sink_mt = widget_sink<std::mutex>;
using widget_sink_st = widget_sink<spdlog::details::null_mutex>;

#define LOG_TRACE(...)     { LogWidget::GetLogger()->trace(__VA_ARGS__);    }
#define LOG_INFO(...)      { LogWidget::GetLogger()->info(__VA_ARGS__);     }
#define LOG_WARN(...)      { LogWidget::GetLogger()->warn(__VA_ARGS__);     }
#define LOG_ERROR(...)     { LogWidget::GetLogger()->error(__VA_ARGS__);    }
#define LOG_CRITICAL(...)  { LogWidget::GetLogger()->critical(__VA_ARGS__); }
