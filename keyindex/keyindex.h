#pragma once

#include "mem_types.h"

int INDEX_get_entrycount(FILE *fp);
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
    qword_t             key_hash
);
int INDEX_FILE_delete_entry(INDEX_FILE_OBJECT *_this, ulong_t idx);
int INDEX_FILE_get_key(
    INDEX_FILE_OBJECT  *_this,
    ulong_t             idx,
    LPBuffer           *out_key_p
);
int INDEX_FILE_get_idx(INDEX_FILE_OBJECT *_this, LPBuffer *key_p);
bool INDEX_FILE_OBJECT_commit(INDEX_FILE_OBJECT *_this);
void INDEX_FILE_OBJECT_deinit(INDEX_FILE_OBJECT *_this);