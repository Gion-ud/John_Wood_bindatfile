#pragma once

#include "mem_types.h"
#include <hash_index/hash_func.h>
#include <libc_chked.h>

hash32_t (*hash)(const byte_t *key_p, size32_t key_len) = &fnv_1a_hash32;

static inline int INDEX_FILE_get_entrycount(FILE *fp) {
    if (is_null(fp)) return -1;
    fseek(fp, offsetof(INDEX_FILE_HEADER, entrycount), SEEK_SET);
    word_t entrycount = 0;
    if (
        fread_checked(&entrycount, sizeof(word_t), 1, fp) < 0
    )
        return -1;
    return (int)entrycount;
}

int INDEX_FILE_OBJECT_init(
    INDEX_FILE_OBJECT  *_this,
    FILE               *fp,
    size32_t            entrycap, 
    ulong_t             flags
);
int INDEX_FILE_write_entry(
    INDEX_FILE_OBJECT  *_this,
    const byte_t       *key_p,
    size32_t            key_len,
    dword_t             key_hash,
    word_t              flags,
    uoff32_t            data_off
);
int INDEX_FILE_delete_entry(INDEX_FILE_OBJECT *_this, ulong_t idx);
int INDEX_FILE_get_key(
    INDEX_FILE_OBJECT  *_this,
    ulong_t             idx,
    byte_t             *out_key_p,
    size32_t           *out_key_len_p
);
//int INDEX_FILE_get_idx(INDEX_FILE_OBJECT *_this, LPBuffer *key_p);
bool INDEX_FILE_OBJECT_commit(INDEX_FILE_OBJECT *_this);
void INDEX_FILE_OBJECT_deinit(INDEX_FILE_OBJECT *_this);