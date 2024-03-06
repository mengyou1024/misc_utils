#pragma once

#include <cstdint>
#include <string>
#include <type_traits>

namespace Yo::Types {
    std::string  StringFromWString(std::wstring str);
    std::wstring WStringFromString(std::string str);
} // namespace Yo
