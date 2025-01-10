#include "Types.hpp"

#include <Windows.h>
#include <codecvt>
#include <locale>
#include <string>

namespace Yo::Types {

    std::string StringFromWString(const std::wstring& str) {
        std::wstring_convert<std::codecvt_utf8<wchar_t>> converter;
        return converter.to_bytes(str);
    }

    std::wstring WStringFromString(const std::string& str) {
        std::wstring_convert<std::codecvt_utf8<wchar_t>> converter;
        return converter.from_bytes(str);
    }

    // GB2312到UTF-8的转换
    int GB2312ToUtf8(const char* gb2312, char* utf8) {
        int      len  = MultiByteToWideChar(CP_ACP, 0, gb2312, -1, NULL, 0);
        wchar_t* wstr = new wchar_t[len + 1];
        memset(wstr, 0, static_cast<size_t>(len + 1));
        MultiByteToWideChar(CP_ACP, 0, gb2312, -1, wstr, len);
        len = WideCharToMultiByte(CP_UTF8, 0, wstr, -1, NULL, 0, NULL, NULL);
        WideCharToMultiByte(CP_UTF8, 0, wstr, -1, utf8, len, NULL, NULL);
        if (wstr)
            delete[] wstr;
        return len;
    }

    // UTF-8到GB2312的转换
    int Utf8ToGB2312(const char* utf8, char* gb2312) {
        int      len  = MultiByteToWideChar(CP_UTF8, 0, utf8, -1, NULL, 0);
        wchar_t* wstr = new wchar_t[len + 1];
        memset(wstr, 0, static_cast<size_t>(len + 1));
        MultiByteToWideChar(CP_UTF8, 0, utf8, -1, wstr, len);
        len = WideCharToMultiByte(CP_ACP, 0, wstr, -1, NULL, 0, NULL, NULL);
        WideCharToMultiByte(CP_ACP, 0, wstr, -1, gb2312, len, NULL, NULL);
        if (wstr)
            delete[] wstr;
        return len;
    }

    std::string GB2312ToUtf8(const std::string& gb2312) {
        char buf[1024] = {};
        int  len       = GB2312ToUtf8(gb2312.c_str(), buf);
        return std::string(buf, len - 1);
    }

    std::wstring GB2312ToUtf8(const std::wstring& gb2312) {
        char buf[1024] = {};
        int  len       = GB2312ToUtf8(StringFromWString(gb2312).c_str(), buf);
        return WStringFromString(std::string(buf, len - 1));
    }

    std::string Utf8ToGB2312(const std::string& utf8) {
        char buf[1024] = {};
        int  len       = Utf8ToGB2312(utf8.c_str(), buf);
        return std::string(buf, len - 1);
    }

    std::wstring Utf8ToGB2312(const std::wstring& utf8) {
        char buf[1024] = {};
        int  len       = Utf8ToGB2312(StringFromWString(utf8).c_str(), buf);
        return WStringFromString(std::string(buf, len - 1));
    }

} // namespace Yo::Types
