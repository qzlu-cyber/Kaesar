#pragma once

#include <string>
#include <optional>

namespace Kaesar {
    class FileDialogs
    {
    public:
        // ����һ���ļ��Ի����û�ѡ���ļ���ָ������·��
        static std::optional<std::string> OpenFile(const char* filter);
        static std::optional<std::string> SaveFile(const char* filter);
    };
}