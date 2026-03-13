#include "hash_index.h"
#include <libc_chked.h>

int HASH_INDEX_insert(
    HASH_INDEX_OBJECT  *_this,
    hash_t              key_hash,   // key hash provided by usr
    int                 key_idx     // key idx in the kv arr
);
struct hash_table_ops ht_vtable = {
    .hash   = &fnv_1a_hash,
    .insert = &HASH_INDEX_insert,
};

int HASH_INDEX_OBJECT_init(
    HASH_INDEX_OBJECT  *_this,
    size32_t            ht_size,        // ht size
    size32_t            overflow_cap    // the actual cap of ht is ht_size + overflow_cap
) { // constructor
    if (is_null(_this)) return -1;
    if ((ht_size & (ht_size - 1)) != 0) {
        printerrf("ht_size must be power of 2\n");
        return -1;
    }
    _this->ht_size = ht_size;
    _this->ht_total_cap = ht_size + overflow_cap;
    _this->ht_used_cnt = 1; // slot 0 is reserved for NULL
    _this->ht_key_idx_arr = (HT_KEY_IDX*)malloc_checked(
        _this->ht_total_cap * sizeof(HT_KEY_IDX)
    );
    if (!_this->ht_key_idx_arr) return -1;
    memset(
        _this->ht_key_idx_arr,
        0,
        _this->ht_total_cap * sizeof(HT_KEY_IDX)
    );
    _this->ht_ops = &ht_vtable;

    return _this->ht_total_cap;
}

int HASH_INDEX_insert(
    HASH_INDEX_OBJECT  *_this,
    hash_t              key_hash,   // key hash provided by usr
    int                 key_idx     // key idx in the kv arr
) {
    if (is_null(_this)) return -1;
    if (_this->ht_used_cnt >= _this->ht_size) {
        // this doesnt use ht actual cap bc if so is done
        // hash table would degrade done  to O(n) due to collisions
        printerrf("ht full\n");
        return -1;
    }
    int h_idx = key_hash & (_this->ht_size - 1); // truncate the upper bits to get the mod

    ulong_t probe_cnt = 0;
    while (probe_cnt < _this->ht_size) {
        if (!_this->ht_key_idx_arr[h_idx] && h_idx) {
            _this->ht_key_idx_arr[h_idx] = key_idx;
            ++_this->ht_used_cnt;
            return h_idx;
        }
        ++h_idx; ++probe_cnt;
        h_idx &= (_this->ht_size - 1);
    }

    return -1;
}

void HASH_INDEX_OBJECT_deinit(HASH_INDEX_OBJECT *_this) { // destructor
    _this->ht_size = 0;
    _this->ht_total_cap = 0;
    _this->ht_used_cnt = 0;
    _this->ht_ops = NULL;
    _this->ht_key_idx_arr = free_checked(_this->ht_key_idx_arr);
    return;
}