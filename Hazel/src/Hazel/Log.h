#pragma once

#include <spdlog/spdlog.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/fmt/ostr.h>

#include "Core.h"

namespace Hazel {
    class HAZEL_API Log
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

#define HZ_CORE_TRACE(...)      ::Hazel::Log::GetCoreLoger()->trace(__VA_ARGS__)
#define HZ_CORE_INFO(...)       ::Hazel::Log::GetCoreLoger()->info(__VA_ARGS__)
#define HZ_CORE_WARN(...)       ::Hazel::Log::GetCoreLoger()->warn(__VA_ARGS__)
#define HZ_CORE_ERROR(...)      ::Hazel::Log::GetCoreLoger()->error(__VA_ARGS__)
#define HZ_CORE_CRITICAL(...)   ::Hazel::Log::GetCoreLoger()->critical(__VA_ARGS__)

#define HZ_TRACE(...)           ::Hazel::Log::GetClientLoger()->trace(__VA_ARGS__)
#define HZ_INFO(...)            ::Hazel::Log::GetClientLoger()->info(__VA_ARGS__)
#define HZ_WARN(...)            ::Hazel::Log::GetClientLoger()->warn(__VA_ARGS__)
#define HZ_ERROR(...)           ::Hazel::Log::GetClientLoger()->error(__VA_ARGS__)
#define HZ_CRITICAL(...)        ::Hazel::Log::GetClientLoger()->critical(__VA_ARGS__)
