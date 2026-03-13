#include "tlvkvdat.h"
#include "tlvkvdat_internal.h"

static byte_t FILE_MAGIC[MAGIC_LEN] = { 0xBB, 'D', 'A', 'T', 0x03, '\r', '\n', '\0' };
static byte_t DATA_SECTION_MAGIC[MAGIC_LEN] = { '.', 'D', 'A', 'T', 'A', '\r', '\n', '\0' };
static byte_t EOF_MAGIC[MAGIC_LEN] = { '.', 'D', 'A', 'T', 'E', 'O', 'F', '\n' };

static uoff32_t index_file_indextableoff = 0;
static uoff32_t dat_file_datasectionoff = sizeof(TLVKVDAT_FileHeader) + sizeof(DATA_SECTION_MAGIC);

static inline int TLVKVDAT_init_open_dat_file(TLVKVDAT_DatFileObject *_this, const char *filepath, int openmode) {
    if (!_this || !filepath) goto TLVKVDAT_init_open_dat_file_FAILED_return;
    if (openmode == TLVKVDAT_MODE_CREATE) {
        _this->fp_dat = fopen(filepath, "wb+");
        if (!_this->fp_dat) {
            perror("fopen _this->fp_dat failed with wb+");
            goto TLVKVDAT_init_open_dat_file_FAILED_return;
        }
    } else if (openmode == TLVKVDAT_MODE_OPEN) {
        _this->fp_dat = fopen(filepath, "rb+");
        if (!_this->fp_dat) {
            perror("fopen _this->fp_dat failed with rb+");
            goto TLVKVDAT_init_open_dat_file_FAILED_return;
        }
    } else {
        printerrf("TLVKVDAT_init_open_dat_file: invalid open mode\n");
        goto TLVKVDAT_init_open_dat_file_FAILED_return;
    }
    print_dbg_msg("TLVKVDAT_init_open_dat_file: success\n");
    return openmode;
TLVKVDAT_init_open_dat_file_FAILED_return:
    return -1;
}
static inline bool TLVKVDAT_init_read_file_header(TLVKVDAT_DatFileObject *_this, int openmode) {
    // 0 on failure
    if (!_this || !_this->fp_dat || openmode != TLVKVDAT_MODE_OPEN) return false;
    fseek(_this->fp_dat, 0, SEEK_SET);
    size_t freadcnt = fread(
        &_this->fileheader,
        sizeof(TLVKVDAT_FileHeader),
        1,
        _this->fp_dat
    );
    if (freadcnt != 1) {
        printerrf("fread _this->fileheader failed\n");
        return false;
    }
    return true;
}
static inline bool TLVKVDAT_init_validate_file_magic(TLVKVDAT_DatFileObject *_this, int openmode) {
    // 0 on failure
    if (!_this || openmode != TLVKVDAT_MODE_OPEN) return false;
    if (memcmp(_this->fileheader.magic, FILE_MAGIC, MAGIC_LEN) != 0) {
        printerrf("Invalid file magic!\n");
        return false;
    }
    return true;
}
static inline bool TLVKVDAT_init_read_file_eof_header(TLVKVDAT_DatFileObject *_this, TLVKVDAT_EOFHeader *eofheader_p, int openmode) {
    // 0 on failure
    // read eof
    if (!_this || ! _this->fp_dat || openmode != TLVKVDAT_MODE_OPEN) return false;
    fseek(_this->fp_dat, -(long)sizeof(TLVKVDAT_EOFHeader), SEEK_END);
    size_t freadcnt = fread(
        eofheader_p,
        sizeof(TLVKVDAT_EOFHeader),
        1,
        _this->fp_dat
    );
    if (freadcnt != 1) {
        printerrf("fread eofheader failed\n");
        return false;
    }
    return true;
}
static inline bool TLVKVDAT_init_validate_file_eof_header(TLVKVDAT_DatFileObject *_this, TLVKVDAT_EOFHeader *eofheader_p, int openmode) {
    // 0 on failure
    // _this: checksum later
    if (!_this || openmode != TLVKVDAT_MODE_OPEN) return false;
    if (memcmp(eofheader_p->magic, EOF_MAGIC, MAGIC_LEN) != 0) {
        printerrf("Invalid eof magic!\n");
        return false;
    }
    return true;
}
static inline bool TLVKVDAT_init_validate_cap(TLVKVDAT_DatFileObject *_this, int cap, int openmode) {
    if (!_this || !cap) return false;
    if (openmode == TLVKVDAT_MODE_OPEN && _this->fileheader.entrycount_total >= cap) {
        printerrf("Not enough capacity: _this->fileheader.entrycount_total >= cap\n");
        return false;
    }
    return true;
}
static inline bool TLVKVDAT_init_alloc_indextable(TLVKVDAT_DatFileObject *_this, int cap) {
    if (!_this || !cap) return false;
    _this->indextable = (TLVKVDAT_DatIndexEntry*)calloc(cap, sizeof(TLVKVDAT_DatIndexEntry));
    if (!_this->indextable) {
        printerrf("calloc _this->indextable failed\n");
        return false;
    }
    return true;
}
static inline bool TLVKVDAT_init_load_indexfile_indextable(TLVKVDAT_DatFileObject *_this, int openmode) {
    if (!_this ||
        !_this->fp_idx ||
        !_this->indextable ||
        openmode != TLVKVDAT_MODE_OPEN
    )
        return false;
    fseek(_this->fp_idx, index_file_indextableoff, SEEK_SET);
    size_t freadcnt = fread(
        _this->indextable,
        sizeof(TLVKVDAT_DatIndexEntry),
        _this->fileheader.entrycount_total,
        _this->fp_idx
    );
    if (freadcnt != _this->fileheader.entrycount_total) {
        printerrf("fread _this->indextable failed\n");
        return false;
    }
    return true;
}
static inline ulong_t TLVKVDAT_init_load_entry_maunal(
    TLVKVDAT_DatFileObject *_this,
    int _entry_idx,
    uoff32_t _entryoff,
    int openmode
) {
    if (!_this ||
        !_this->fp_dat ||
        openmode != TLVKVDAT_MODE_OPEN
    )
        return 0;
    fseek(_this->fp_dat, _entryoff, SEEK_SET);

    size_t freadcnt = fread(
        _this->indextable,
        sizeof(TLVKVDAT_DatIndexEntry),
        _this->fileheader.entrycount_total,
        _this->fp_idx
    );
    if (freadcnt != _this->fileheader.entrycount_total) {
        printerrf("fread _this->indextable failed\n");
        return 0;
    }
    return true;
}
// static inline bool TLVKVDAT_init_build_indextable(TLVKVDAT_DatFileObject *_this, int openmode);

static inline bool TLVKVDAT_init_alloc_kvarr(TLVKVDAT_DatFileObject *_this, int cap, int openmode) {
    if (!_this || !cap) return false;
    _this->kv_arr = (TLVKVDAT_KV*)calloc(cap, sizeof(TLVKVDAT_KV));
    if (!_this->kv_arr) {
        printerrf("calloc _this->kv_arr failed\n");
        return false;
    }
    return true;
}
static inline bool TLVKVDAT_init_load_kvarr(TLVKVDAT_DatFileObject *_this, int openmode) {
    if (!_this ||
        !_this->fp_dat ||
        !_this->kv_arr ||
        openmode != TLVKVDAT_MODE_OPEN
    )
        return false;
    ulong_t loaded_entrycount = 0;
    for (ulong_t i = 0; i < _this->fileheader.entrycount_total; ++i) {
        fseek(fp->dat, );
    }
    if (loaded_entrycount != _this->fileheader.entrycount_total) {
        printerrf("fread _this->key_arr failed\n");
        return false;
    }

    return true;
}



// next step: alloc mem for indextable, key_arr and val_arr;
//            deal with create instead of open

static inline int TLVKVDAT_init_open_dat_file(TLVKVDAT_DatFileObject *_this, const char *filepath, int openmode);
static inline bool TLVKVDAT_init_read_file_header(TLVKVDAT_DatFileObject *_this, int openmode);
static inline bool TLVKVDAT_init_validate_file_magic(TLVKVDAT_DatFileObject *_this, int openmode);
static inline bool TLVKVDAT_init_read_file_eof_header(TLVKVDAT_DatFileObject *_this, TLVKVDAT_EOFHeader *eofheader_p, int openmode);
static inline bool TLVKVDAT_init_validate_file_eof_header(TLVKVDAT_DatFileObject *_this, TLVKVDAT_EOFHeader *eofheader_p, int openmode);
static inline bool TLVKVDAT_init_validate_cap(TLVKVDAT_DatFileObject *_this, int cap, int openmode);
static inline bool TLVKVDAT_init_alloc_indextable(TLVKVDAT_DatFileObject *_this, int cap);
static inline bool TLVKVDAT_init_load_indextable(TLVKVDAT_DatFileObject *_this, int openmode);
static inline bool TLVKVDAT_init_alloc_kvarr(TLVKVDAT_DatFileObject *_this, int cap, int openmode);
static inline bool TLVKVDAT_init_load_kvarr(TLVKVDAT_DatFileObject *_this, int openmode);

int TLVKVDAT_init(TLVKVDAT_DatFileObject *_this, const char *filepath, int cap, int openmode) {
    if (!_this || !filepath || !cap) goto TLVKVDAT_init_failed_return;
    memset(_this, 0, sizeof(TLVKVDAT_DatFileObject));

    int ret = TLVKVDAT_init_open_dat_file(_this, filepath, openmode);
    if (ret < 0) {
        printerrf("TLVKVDAT_init_open_dat_file failed\n");
        goto TLVKVDAT_init_failed_return;
    }

    if (openmode == TLVKVDAT_MODE_OPEN) {
    TLVKVDAT_open_dat_file:
        if (!TLVKVDAT_init_read_file_header(_this, openmode)) {
            printerrf("TLVKVDAT_init_read_file_header failed\n");
            goto TLVKVDAT_fp_dat_fclose;
        }
        if (!TLVKVDAT_init_validate_file_magic(_this, openmode)) {
            printerrf("TLVKVDAT_init_validate_file_magic failed\n");
            goto TLVKVDAT_fp_dat_fclose;
        }
        TLVKVDAT_EOFHeader eofheader = {0};
        if (!TLVKVDAT_init_read_file_eof_header(_this, &eofheader, openmode)) {
            printerrf("TLVKVDAT_init_read_file_eof_header failed\n");
            goto TLVKVDAT_fp_dat_fclose;
        }
        if (!TLVKVDAT_init_validate_file_eof_header(_this, &eofheader, openmode)) {
            printerrf("TLVKVDAT_init_validate_file_eof_header failed\n");
            goto TLVKVDAT_fp_dat_fclose;
        }
        if (!TLVKVDAT_init_validate_cap(_this, cap, openmode)) {
            printerrf(
                "TLVKVDAT_init_validate_cap failed\n"
                "Default entry capacity to _this->fileheader.entrycount_total + 32"
            );
            cap = _this->fileheader.entrycount_total + 32;
        }
    TLVKVDAT_load_dat_indextable:
        if (!TLVKVDAT_init_alloc_indextable(_this, cap)) {
            printerrf("TLVKVDAT_init_alloc_indextable failed\n");
            goto TLVKVDAT_indextable_free;
        }
        if (!TLVKVDAT_init_load_indexfile_indextable(_this, cap)) {
            printerrf("TLVKVDAT_init_load_indextable failed\n");
            goto TLVKVDAT_indextable_free;
        }



    } else if (openmode == TLVKVDAT_MODE_CREATE) {
    TLVKVDAT_create_dat_file:
        // Do Something
    }

    return cap;

TLVKVDAT_init_failed_cleanup:
    TLVKVDAT_kvarr_free:
        if (_this->indextable) {
            free(_this->indextable);
            _this->indextable = NULL;
        }
    TLVKVDAT_indextable_free:
        if (_this->indextable) {
            free(_this->indextable);
            _this->indextable = NULL;
        }
    TLVKVDAT_fp_dat_fclose:
        if (_this->fp_dat) {
            fclose(_this->fp_dat);
            _this->fp_dat = NULL;
        }
TLVKVDAT_init_failed_return:
    return TLVKVDAT_INIT_FAILED;
}