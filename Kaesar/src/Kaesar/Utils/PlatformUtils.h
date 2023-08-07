#pragma once

#include <string>
#include <optional>

namespace Kaesar {
    class FileDialogs
    {
    public:
        // 弹出一个文件对话框供用户选择文件或指定保存路径
        static std::optional<std::string> OpenFile(const char* filter);
        static std::optional<std::string> SaveFile(const char* filter);
    };
}