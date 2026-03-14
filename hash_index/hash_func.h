#include <global.h>

typedef ulonglong_t hash_t;
typedef ulong_t hash32_t;


static inline hash_t fnv_1a_hash(const byte_t* key, size_t len) {
#define FNV_OFFSET_BASIS 14695981039346656037ULL
#define FNV_PRIME        1099511628211ULL
    hash_t h = FNV_OFFSET_BASIS;
    size_t i = 0;
    while (i < len) {
        h ^= key[i++];
        h *= FNV_PRIME;
    }
    return h;
#undef FNV_OFFSET_BASIS
#undef FNV_PRIME
}

static inline hash32_t fnv_1a_hash32(const byte_t* key, size32_t len) {
#define FNV_OFFSET_BASIS 0x811C9DC5
#define FNV_PRIME        0x01000193
    hash_t h = FNV_OFFSET_BASIS;
    ulong_t i = 0;
    while (i < len) {
        h ^= key[i++];
        h *= FNV_PRIME;
    }
    return h;
#undef FNV_OFFSET_BASIS
#undef FNV_PRIME
}
