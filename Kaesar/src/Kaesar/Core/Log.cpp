#include "krpch.h"
#include "Log.h"

namespace Kaesar {
    std::shared_ptr<spdlog::logger> Log::s_CoreLogger;
    std::shared_ptr<spdlog::logger> Log::s_ClientLogger;

    void Log::Init() {
        // 设置记录信息的格式
        spdlog::set_pattern("%^[%T] %n: %v%$"); // 时间戳 日志种类（Core or Client） 内容

        s_CoreLogger = spdlog::stdout_color_mt("Kaesar");
        s_CoreLogger->set_level(spdlog::level::trace);

        s_ClientLogger = spdlog::stderr_color_mt("APP");
        s_ClientLogger->set_level(spdlog::level::trace);
    }
}