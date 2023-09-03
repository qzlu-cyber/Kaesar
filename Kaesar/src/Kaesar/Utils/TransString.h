#pragma once

#include <string>

namespace Kaesar {
    class TransString
    {
    public:
        static std::string UTF8ToString(const std::string& utf8Data);
        static std::string StringToUTF8(const std::string& utf8Data);

        static std::string TBS(std::string& str);
    };
}