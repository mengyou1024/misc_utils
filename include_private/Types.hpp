#pragma once

#include <cstdint>
#include <string>
#include <type_traits>

namespace Yo::Types {
    std::string  StringFromWString(const std::wstring& str);
    std::wstring WStringFromString(const std::string& str);
    std::string  GB2312ToUtf8(const std::string& gb2312);
    std::string  Utf8ToGB2312(const std::string& utf8);
    template <class T>
    constexpr T SwapBytes(T val)
        requires std::is_integral_v<T>
    {
        if constexpr (sizeof(T) == 1) {
            return val;
        } else if constexpr (sizeof(T) == 2) {
            return static_cast<T>(((val & 0xff) << 8) | ((val & 0xff00) >> 8));
        } else if constexpr (sizeof(T) == 4) {
            return static_cast<T>(((val & 0xff) << 24) | ((val & 0xff00) << 8) | ((val & 0xff0000) >> 8) | (val & 0xff000000) >> 24);
        } else if constexpr (sizeof(T) == 8) {
            return static_cast<T>(((val & 0xff) << 56) | ((val & 0xff00) << 40) | (val & 0xff0000) << 24 | ((val & 0xff000000) << 8) | ((val & 0xff00000000) >> 8) | (val & 0xff0000000000) >> 24);
        } else {
            static_assert(false, "Unsupported type");
        }
    }
} // namespace Yo::Types
