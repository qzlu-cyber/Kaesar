#include "krpch.h"
#include "TransString.h"

namespace Kaesar {
    std::string TransString::UTF8ToString(const std::string& utf8Data)
    {
        char* buf = new char[1024 * 60];
        snprintf(buf, static_cast<size_t>(1024) * 60, u8"%s", utf8Data.c_str());
        TCHAR* wscBuffer = new TCHAR[1024 * 10];
        MultiByteToWideChar(CP_UTF8, 0, buf, -1, wscBuffer, 1024 * 10);
        delete[] buf; // 释放之前的堆内存

        char* convertedBuf = new char[1024 * 9];
        WideCharToMultiByte(CP_ACP, 0, wscBuffer, -1, convertedBuf, 1024 * 9, NULL, NULL);
        delete[] wscBuffer; // 释放堆内存

        std::string result(convertedBuf);
        delete[] convertedBuf; // 释放堆内存

        return result;
    }

    std::string TransString::StringToUTF8(const std::string& str)
    {
        int nwLen = ::MultiByteToWideChar(CP_ACP, 0, str.c_str(), -1, NULL, 0);
        wchar_t* pwBuf = new wchar_t[nwLen + 1];
        ZeroMemory(pwBuf, nwLen * 2 + 2);
        ::MultiByteToWideChar(CP_ACP, 0, str.c_str(), str.length(), pwBuf, nwLen);
        int nLen = ::WideCharToMultiByte(CP_UTF8, 0, pwBuf, -1, NULL, NULL, NULL, NULL);
        char* pBuf = new char[nLen + 1];
        ZeroMemory(pBuf, nLen + 1);
        ::WideCharToMultiByte(CP_UTF8, 0, pwBuf, nwLen, pBuf, nLen, NULL, NULL);
        std::string retStr(pBuf);

        delete[]pwBuf;
        delete[]pBuf;

        pwBuf = NULL;
        pBuf = NULL;

        return retStr;
    }

    std::string TransString::TBS(std::string& str)
    {
        str = UTF8ToString(str);

        typedef std::codecvt_byname<wchar_t, char, std::mbstate_t> F;

        static std::wstring_convert<F>strC(new F("Chinese"));
        static std::wstring_convert<std::codecvt_utf8<wchar_t>> strCnv;
        return strCnv.to_bytes(strC.from_bytes(str));
    }
}