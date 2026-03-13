#ifndef HASH_INDEX_H
#define HASH_INDEX_H

#include "hash_func.h"
#include "libc_chked.h"

typedef struct _ht_object HASH_INDEX_OBJECT;
struct hash_table_ops {
    hash_t  (*hash)(const byte_t *src, size_t len);
    int     (*insert)(HASH_INDEX_OBJECT *_this, hash_t key_hash, int key_idx);
//  int     (*get_key_idx)(HASH_INDEX_OBJECT *_this, const byte_t *key, size_t key_len);
};

typedef long_t HT_KEY_IDX; // kv are always paired


typedef struct _ht_object {
    HT_KEY_IDX *ht_key_idx_arr;
    size32_t    ht_size;
    size32_t    ht_total_cap;
    size32_t    ht_used_cnt;

    struct hash_table_ops *ht_ops;
} HASH_INDEX_OBJECT;

//extern struct hash_table_ops ht_vtable;
int HASH_INDEX_insert(
    HASH_INDEX_OBJECT  *_this,
    hash_t              key_hash,   // key hash provided by usr
    int                 key_idx     // key idx in the kv arr
);

int HASH_INDEX_OBJECT_init(
    HASH_INDEX_OBJECT  *_this,
    size32_t            ht_size,
    size32_t            overflow_cap
);
void HASH_INDEX_OBJECT_deinit(HASH_INDEX_OBJECT *_this);


#endif