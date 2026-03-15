#include "internal.h"

void INDEX_FILE_OBJECT_deinit(INDEX_FILE_OBJECT *_this);
int INDEX_FILE_OBJECT_init(
    INDEX_FILE_OBJECT  *_this,
    FILE               *fp,
    size32_t            entrycap,
    ulong_t             flags
) {
    if (is_null(_this)) goto INDEX_FILE_OBJECT_init_failed;
    INDEX_init_fileheader(&_this->fileheader, flags);
    _this->indextable = (INDEX_ENTRY*)malloc_checked(
        entrycap * sizeof(INDEX_ENTRY)
    );
    if (!_this->indextable) goto INDEX_FILE_OBJECT_init_failed;
    memset(_this->indextable, 0, entrycap * sizeof(INDEX_ENTRY));
    _this->key_arr = (LPBuffer*)malloc_checked(
        entrycap * sizeof(LPBuffer)
    );
    if (!_this->key_arr) goto INDEX_FILE_OBJECT_init_failed;
    memset(_this->key_arr, 0, entrycap * sizeof(LPBuffer));

    _this->idx_file_cur = indexfile_stringtableoff;
    _this->entrycap = entrycap;
    _this->fp = fp;

    return (int)entrycap;
INDEX_FILE_OBJECT_init_failed:
    INDEX_FILE_OBJECT_deinit(_this);
    return -1;
}

int INDEX_FILE_load_sections(INDEX_FILE_OBJECT *_this) {
    if (is_null(_this) || is_null(_this->indextable) || is_null(_this->key_arr)) return -1;
    fseek(_this->fp, 0, SEEK_SET);
    if (fread_checked(&_this->fileheader, sizeof(INDEX_FILE_HEADER), 1, _this->fp) < 0) {
        printerrf("failed to load fileheader\n");
        return -1;
    }
    fseek(_this->fp, _this->fileheader.footeroff, SEEK_SET);
    if (fread_checked(&_this->filefooter, sizeof(INDEX_FILE_FOOTER), 1, _this->fp) < 0) {
        printerrf("failed to load filefooter\n");
        return -1;
    }
    
}

static inline int INDEX_FILE_indextable_insert(
    INDEX_ENTRY    *indextable,
    ulong_t         idx,
    dword_t         key_hash,
    dword_t         key_len,
    uoff32_t        key_off,
    word_t          flags,
    dword_t         data_off
) {
    if (is_null(indextable)) return -1;
    indextable[idx].key_hash    = key_hash;
    indextable[idx].key_len     = key_len;
    indextable[idx].key_off     = key_off;
    indextable[idx].flags       = flags;
    indextable[idx].data_off    = data_off;
    return (int)idx;
}

int INDEX_FILE_write_entry(
    INDEX_FILE_OBJECT  *_this,
    const byte_t       *key_p,
    size32_t            key_len,
    dword_t             key_hash,
    word_t              flags,
    uoff32_t            data_off
) {
    if (
        is_null(_this) ||
        is_null(key_p)
    )
        return -1;
    ulong_t idx = _this->fileheader.entrycount;
    if (idx >= _this->entrycap) {
        printerrf("Entry capacity full\n");
        return -1;
    }
    uoff32_t key_off = _this->idx_file_cur;
    fseek(_this->fp, key_off, SEEK_SET);
    if (
        fwrite_checked(
            (void*)key_p, 1, key_len, _this->fp
        ) != key_len
    ) {
        printerrf("Failed to write key\n");
        return -1;
    }
    if (fputc('\0', _this->fp) == EOF) {
        printerrf("Failed to write NUL terminator\n");
        return -1;
    }
    if (
        INDEX_FILE_indextable_insert(
            _this->indextable,
            idx,
            key_hash,
            key_len,
            key_off,
            flags,
            data_off
        ) < 0
    ) {
        printerrf("Failed to insert indexentry\n");
        return -1;
    }
    _this->key_arr[idx].data = (void*)key_p; // This Does Not Copy
    _this->key_arr[idx].len = key_len;
    _this->idx_file_cur += (key_len + 1);
    INDEX_update_fileheader(&_this->fileheader, idx + 1, _this->idx_file_cur, _this->idx_file_cur, 0);
    return (int)_this->fileheader.entrycount++;
}

int INDEX_FILE_delete_entry(INDEX_FILE_OBJECT *_this, ulong_t idx) {
    if (is_null(_this)) return false;
    if (idx >= _this->entrycap) {
        printerrf("dat entry capacity full\n");
        return -1;
    }
    _this->indextable[idx].key_off = 0;
    _this->indextable[idx].data_off = 0;
    _this->indextable[idx].flags |= ENTRY_DELETED;
    return (int)idx;
}

int INDEX_FILE_get_key(
    INDEX_FILE_OBJECT  *_this,
    ulong_t             idx,
    byte_t             *out_key_p,
    size32_t           *out_key_len_p
) {
    if (is_null(_this) || is_null(out_key_p) || is_null(out_key_p) || is_null(out_key_len_p))
        return -1;
    if (idx > _this->entrycap) {
        printerrf("idx out of bound\n");
        return -1;
    }
    if (!_this->indextable[idx].key_off || (_this->indextable[idx].flags | ENTRY_DELETED)) {
        printerrf("Entry[%u] is not valid\n", idx);
        return -1;
    }
    uoff32_t key_off = _this->indextable[idx].key_off;
    *out_key_len_p = _this->indextable[idx].key_len;
    fseek(_this->fp, key_off, SEEK_SET);
    if (
        fread_checked(
            out_key_p, 1, *out_key_len_p, _this->fp
        ) < 0
    )
        return -1;

    return (int)idx;
}

/*
int INDEX_FILE_get_idx(
    INDEX_FILE_OBJECT  *_this,
    LPBuffer           *key_p
) {
    if (is_null(_this) || is_null(key_p))
        return -1;

}
*/


bool INDEX_FILE_OBJECT_commit(INDEX_FILE_OBJECT *_this) {
    if (is_null(_this)) return -1;
    size32_t entrycount = _this->fileheader.entrycount;
    uoff32_t indextableoff = (_this->idx_file_cur + PADDING_ALIGN_SIZE - 1) &~ (PADDING_ALIGN_SIZE - 1);
    printerrf("this.idx_file_cur=%u\n", _this->idx_file_cur);

    uoff32_t filefooteroff = indextableoff + _this->fileheader.entrycount * sizeof(INDEX_ENTRY);

    fseek(_this->fp, indextableoff, SEEK_SET);
    if (
        fwrite_checked(
            _this->indextable, sizeof(INDEX_ENTRY), entrycount, _this->fp
        ) < 0
    )
        return false;

    INDEX_update_fileheader(
        &_this->fileheader,
        entrycount,
        indextableoff,
        filefooteroff,
        time(NULL)
    );
    fseek(_this->fp, 0, SEEK_SET);
    if (
        fwrite_checked(
            &_this->fileheader, sizeof(INDEX_FILE_HEADER), 1, _this->fp
        ) < 0
    )
        return false;

    //memcpy(&filefooter.magic, (byte_t*)EOF_MAGIC, sizeof(qword_t));
    _this->filefooter.magic = *(qword_t*)((byte_t*)EOF_MAGIC);
    
    fseek(_this->fp, filefooteroff, SEEK_SET);
    if (
        fwrite_checked(
            &_this->filefooter, sizeof(INDEX_FILE_FOOTER), 1, _this->fp
        ) < 0
    )
        return false;

    return true;
}

void INDEX_FILE_OBJECT_deinit(INDEX_FILE_OBJECT *_this) {
    if (_this->indextable) {
        _this->indextable = free_checked(_this->indextable);
    }
    if (_this->key_arr) {
        _this->key_arr = free_checked(_this->key_arr);
    }
    memset(_this, 0, sizeof(INDEX_FILE_OBJECT));
}