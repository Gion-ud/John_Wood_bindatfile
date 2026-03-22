#pragma once

#include <zlib.h>
#include "libc_chked.h"

static inline ulong_t compute_file_section_crc32(
    FILE       *this_file_p,
    uoff32_t    src_off,
    uoff32_t    dest_off
) {
    if (is_null(this_file_p)) return 0;
    ulong_t f_crc32 = (ulong_t)crc32(0L, Z_NULL, 0);
    byte_t buffer[BUFFER_SIZE] = {0};
    size_t n = 0;
    fseek(this_file_p, src_off, SEEK_SET);
    for (uoff32_t i = 0; i < dest_off / BUFFER_SIZE; ++i) {
        n = fread(buffer, 1, BUFFER_SIZE, this_file_p);
        if (!n) break;
        f_crc32 = crc32(f_crc32, (byte_t*)buffer, n);
    }
    size_t rem = dest_off % BUFFER_SIZE;
    if (rem) {
        n = fread(buffer, 1, rem, this_file_p);
        if (n) {
            f_crc32 = crc32(f_crc32, (byte_t*)buffer, n);
        }
    }
    return f_crc32;
}

static inline ulong_t compute_mem_crc32(void *buf, ulong_t buf_len) {
    if (is_null(buf)) return 0;
    ulong_t ulcrc32 = (ulong_t)crc32(0L, Z_NULL, 0);
    ulcrc32 = crc32(ulcrc32, buf, buf_len);
    return ulcrc32;
}