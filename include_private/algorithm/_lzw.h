/*
    Variable-length code LZW compressor and decompressor for fixed-memory decoding.
    Copyright (c) 2020-2022, Eddy L O Jansson. Licensed under The MIT License.

    See https://github.com/eloj/lzw-eddy
*/
#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>
#include <stdint.h>
#if defined(_MSC_VER)
    #include <BaseTsd.h>
typedef SSIZE_T ssize_t;
#else
    #include <sys/types.h> // for ssize_t
#endif

#define LZW_EDDY_MAJOR_VERSION 1
#define LZW_EDDY_MINOR_VERSION 0
#define LZW_EDDY_PATCH_VERSION 0
#define LZW_EDDY_VERSION       "1.0.0"

#define LZW_MIN_CODE_WIDTH     9
// 9 to 16-bit codes should all work, but 12 is the default for a reason.
// Going beyond 16-bit codes would require code changes. More isn't better either.
#ifndef LZW_MAX_CODE_WIDTH
    #define LZW_MAX_CODE_WIDTH 12
#endif
#define LZW_MAX_CODES (1UL << LZW_MAX_CODE_WIDTH)

enum lzw_errors {
    LZW_NOERROR               = 0,
    LZW_DESTINATION_TOO_SMALL = -1,
    LZW_INVALID_CODE_STREAM   = -2,
    LZW_STRING_TABLE_FULL     = -3,
};

// This type must be large enough for SYMBOL_BITS + LZW_MAX_CODE_WIDTH*2 bits.
#if LZW_MAX_CODE_WIDTH > 12
typedef uint64_t lzw_node;
#else
typedef uint32_t lzw_node;
#endif
typedef uint32_t bitres_t;
typedef uint16_t code_t;

struct lzw_string_table {
    uint32_t code_width;
    code_t   next_code;
    code_t   prev_code;
    lzw_node node[LZW_MAX_CODES]; // 16K at 12-bit codes.
};

struct lzw_state {
    struct lzw_string_table tree;

    // If we ever need more of these, change to a flag-word.
    bool was_init;
    bool must_reset;

    size_t rptr;
    size_t wptr;
    // Bit reservoir, need room for LZW_MAX_CODE_WIDTH*2-1 bits.
    bitres_t bitres;
    uint32_t bitres_len;

    // Tracks the longest prefix used, which is equal to the minimum output buffer required for decompression.
    size_t longest_prefix;
    // Restrict the longest_prefix to this -- optimize for decode buffer size.
    size_t longest_prefix_allowed;
};

// Translate error code to message.
const char *lzw_strerror(enum lzw_errors errnum);

/*
    Decompress `slen` bytes from `src` into `dest` of size `dlen`.

    Returns the number of bytes decompressed into `dest`.
    Once all input has been consumed, 0 is returned.
    On error, a negative integer is returned.

    Neither `src` nor `dest` may be NULL.

    `state`should be zero-initialized.

    `dlen` should be at least 4096 bytes, unless the input is known to
    require less.

    `LZWD_DESTINATION_TOO_SMALL` will be returned if the output buffer is too small, in which case
    you'd have to restart from the beginning with a larger `dest`.

    All that said, even a file consisting of 80K zeros requires only 400 bytes,
    so we're being very conservative here. A 'normal' file may need only 128 bytes or so.
*/
ssize_t lzw_decompress(struct lzw_state *state, uint8_t *src, size_t slen, uint8_t *dest, size_t dlen);

/*
    Compress `slen` bytes from `src` into `dest` of size `dlen`.

    Returns the number of bytes compressed into `dest`.
    Once all input has been consumed, 0 is returned.
    On error, a negative integer is returned.

    Neither `src` nor `dest` may be NULL.

    `state`should be zero-initialized.
*/
ssize_t lzw_compress(struct lzw_state *state, uint8_t *src, size_t slen, uint8_t *dest, size_t dlen);

#ifdef __cplusplus
}
#endif
