#pragma once

#include <cstdint>
#include <memory>
#include <optional>
#include <utility>
#include <cstdlib>
#include <vector>

namespace Yo::algorithm::lzw {
    std::optional<std::vector<uint8_t>> lzw_compress(const uint8_t* data, std::size_t size);
    std::optional<std::vector<uint8_t>> lzw_decompress(const uint8_t* data, std::size_t size);
} // namespace algorithm
