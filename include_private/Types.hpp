#pragma once

#include <cstdint>
#include <string>
#include <type_traits>

namespace Yo::Types {
    std::string  StringFromWString(std::wstring str);
    std::wstring WStringFromString(std::string str);
    std::string  GB2312ToUtf8(std::string gb2312);
    std::string  Utf8ToGB2312(std::string utf8);
} // namespace Yo::Types
