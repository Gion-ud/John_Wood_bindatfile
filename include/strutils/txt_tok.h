#pragma once

#include "global.h"

typedef struct tok_meta tokmeta_t;
struct tok_meta {
    ulong_t tok_len;
    ulong_t tok_off;
};

ulong_t CntTok(const char *src, ulong_t src_len, const char delim);
tokmeta_t *MarkTokPosMeta(const char* src, ulong_t src_len, ulong_t tok_cnt, const char delim);
char *CpyTokSrc(const char* src, ulong_t src_len, tokmeta_t tok_info);
ulong_t lstrip(char **pLine, ulong_t len);
ulong_t rstrip(char *const *pLine, ulong_t len);