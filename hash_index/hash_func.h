#include <global.h>

typedef ulonglong_t hash_t;

static inline hash_t fnv_1a_hash(const byte_t* key, size_t len) {
#define FNV_OFFSET_BASIS 14695981039346656037ULL
#define FNV_PRIME        1099511628211ULL
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
