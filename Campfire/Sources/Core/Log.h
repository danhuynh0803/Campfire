#ifndef LOG_H
#define LOG_H

#include <memory>
#include <sstream>
#include <spdlog/spdlog.h>
#include <spdlog/sinks/ostream_sink.h>

#include "Editor/Widgets/LogWidget.h"

#include "spdlog/sinks/base_sink.h"
#include <iostream>

template<typename Mutex>
class widget_sink : public spdlog::sinks::base_sink <Mutex>
{
protected:
    void sink_it_(const spdlog::details::log_msg& msg) override
    {

        // log_msg is a struct containing the log entry info like level, timestamp, thread id etc.
        // msg.raw contains pre formatted log

        // If needed (very likely but not mandatory), the sink formats the message before sending it to its final destination:
        spdlog::memory_buf_t formatted;
        base_sink<Mutex>::formatter_->format(msg, formatted);
        Log::GetWidget()->AddLog(fmt::to_string(formatted).c_str());
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

class Log
{
public:
    static void Init();

    inline static std::shared_ptr<spdlog::logger>& GetCoreLogger() { return coreLogger; };
    inline static std::shared_ptr<spdlog::logger>& GetClientLogger() { return clientLogger; };
    inline static std::shared_ptr<LogWidget>& GetWidget() { return logWidget; };
    inline static void ShowLog(bool* isOpen) { logWidget->Draw(isOpen); }

private:
    static std::shared_ptr<spdlog::logger> coreLogger;
    static std::shared_ptr<spdlog::logger> clientLogger;
    static std::shared_ptr<LogWidget> logWidget;
};

#define LOG_TRACE(...)     { Log::GetCoreLogger()->trace(__VA_ARGS__);    }
#define LOG_INFO(...)      { Log::GetCoreLogger()->info(__VA_ARGS__);     }
#define LOG_WARN(...)      { Log::GetCoreLogger()->warn(__VA_ARGS__);     }
#define LOG_ERROR(...)     { Log::GetCoreLogger()->error(__VA_ARGS__);    }
#define LOG_CRITICAL(...)  { Log::GetCoreLogger()->critical(__VA_ARGS__); }

#endif // LOG_H
