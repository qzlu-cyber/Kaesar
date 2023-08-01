#pragma once

#include "Core.h"

#include <spdlog/spdlog.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/fmt/ostr.h>

namespace Kaesar {
    class Kaesar_API Log
    {
    public:
        static void Init();

        inline static std::shared_ptr<spdlog::logger>& GetCoreLoger() { return s_CoreLogger; }
        inline static std::shared_ptr<spdlog::logger>& GetClientLoger() { return s_ClientLogger; }

    private:
        static std::shared_ptr<spdlog::logger> s_CoreLogger;
        static std::shared_ptr<spdlog::logger> s_ClientLogger;
    };
}

#define KR_CORE_TRACE(...)      ::Kaesar::Log::GetCoreLoger()->trace(__VA_ARGS__)
#define KR_CORE_INFO(...)       ::Kaesar::Log::GetCoreLoger()->info(__VA_ARGS__)
#define KR_CORE_WARN(...)       ::Kaesar::Log::GetCoreLoger()->warn(__VA_ARGS__)
#define KR_CORE_ERROR(...)      ::Kaesar::Log::GetCoreLoger()->error(__VA_ARGS__)
#define KR_CORE_CRITICAL(...)   ::Kaesar::Log::GetCoreLoger()->critical(__VA_ARGS__)

#define KR_TRACE(...)           ::Kaesar::Log::GetClientLoger()->trace(__VA_ARGS__)
#define KR_INFO(...)            ::Kaesar::Log::GetClientLoger()->info(__VA_ARGS__)
#define KR_WARN(...)            ::Kaesar::Log::GetClientLoger()->warn(__VA_ARGS__)
#define KR_ERROR(...)           ::Kaesar::Log::GetClientLoger()->error(__VA_ARGS__)
#define KR_CRITICAL(...)        ::Kaesar::Log::GetClientLoger()->critical(__VA_ARGS__)
