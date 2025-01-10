#pragma once

#include <cstdint>
#include <string>
#include <type_traits>

namespace Yo::Types {
    std::string  StringFromWString(const std::wstring& str);
    std::wstring WStringFromString(const std::string& str);
    std::string  GB2312ToUtf8(const std::string& gb2312);
    std::wstring GB2312ToUtf8(const std::wstring& gb2312);
    std::string  Utf8ToGB2312(const std::string& utf8);
    std::wstring Utf8ToGB2312(const std::wstring& utf8);
    template <class T>
    constexpr T SwapBytes(T val)
        requires(std::is_integral_v<T> || std::is_floating_point_v<T>)
    {
        T             result;
        std::uint8_t* src = reinterpret_cast<std::uint8_t*>(&val);
        std::uint8_t* dst = reinterpret_cast<std::uint8_t*>(&result);
        for (size_t i = 0; i < sizeof(T); ++i) {
            dst[i] = src[sizeof(T) - i - 1];
        }
        return result;
    }

} // namespace Yo::Types
