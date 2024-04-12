#include "lzw.hpp"

#include "_lzw.h"

#include <stdexcept>

namespace Yo::algorithm::lzw {
    std::optional<std::vector<uint8_t>> lzw_compress(const uint8_t *data, std::size_t size) {
        std::vector<uint8_t> ret;
        lzw_state            state           = {};
        ssize_t              compressed_size = 0;
        do {
            if (ret.size() == 0) {
                ret.resize(1024 * 10);
            } else {
                ret.resize(ret.size() * 2);
            }
            compressed_size = ::lzw_compress(&state, const_cast<uint8_t *>(data), size, ret.data(), ret.size());
            if (compressed_size < 0) {
                throw std::runtime_error(lzw_strerror((lzw_errors)compressed_size));
            }
        } while (lzw_errors(compressed_size) == LZW_DESTINATION_TOO_SMALL);
        ret.resize(compressed_size);
        return ret;
    }

    std::optional<std::vector<uint8_t>> lzw_decompress(const uint8_t *data, std::size_t size) {
        std::vector<uint8_t> ret;
        lzw_state            state           = {};
        ssize_t              compressed_size = 0;
        do {
            if (ret.size() == 0) {
                ret.resize(1024 * 10);
            } else {
                ret.resize(ret.size() * 2);
            }
            compressed_size = ::lzw_decompress(&state, const_cast<uint8_t *>(data), size, ret.data(), ret.size());
            if (compressed_size < 0) {
                throw std::runtime_error(lzw_strerror((lzw_errors)compressed_size));
            }
        } while (lzw_errors(compressed_size) == LZW_DESTINATION_TOO_SMALL);
        ret.resize(compressed_size);
        return ret;
    }

} // namespace Yo::algorithm::lzw
