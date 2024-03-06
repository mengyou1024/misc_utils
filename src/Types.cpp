#include "Types.hpp"

#include <codecvt>
#include <locale>
#include <string>

namespace Yo::Types {

    std::string StringFromWString(std::wstring str) {
        std::wstring_convert<std::codecvt_utf8<wchar_t>> converter;
        return converter.to_bytes(str);
    }

    std::wstring WStringFromString(std::string str) {
        std::wstring_convert<std::codecvt_utf8<wchar_t>> converter;
        return converter.from_bytes(str);
    }
} // namespace Yo
