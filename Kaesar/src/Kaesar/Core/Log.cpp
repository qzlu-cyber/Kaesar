#include "krpch.h"
#include "Log.h"

namespace Kaesar {
    std::shared_ptr<spdlog::logger> Log::s_CoreLogger;
    std::shared_ptr<spdlog::logger> Log::s_ClientLogger;

    void Log::Init() {
        // ���ü�¼��Ϣ�ĸ�ʽ
        spdlog::set_pattern("%^[%T] %n: %v%$"); // ʱ��� ��־���ࣨCore or Client�� ����

        s_CoreLogger = spdlog::stdout_color_mt("Kaesar");
        s_CoreLogger->set_level(spdlog::level::trace);

        s_ClientLogger = spdlog::stderr_color_mt("APP");
        s_ClientLogger->set_level(spdlog::level::trace);
    }
}