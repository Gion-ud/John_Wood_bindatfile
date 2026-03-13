#include "kvdat.h"
#include "kvdat_internal.h"

static const byte_t FILE_MAGIC[MAGIC_LEN] = { 0xBB, 'D', 'A', 'T', 0x03, '\r', '\n', '\0' };
static const byte_t DATA_SECTION_MAGIC[MAGIC_LEN] = { '.', 'D', 'A', 'T', 'A', '\r', '\n', '\0' };
static const byte_t ENTRY_MAGIC[MAGIC_LEN] = { 'D', 'A', 'T', 'E', 'N', 'T', 'R', 'Y' };
static const byte_t EOF_MAGIC[MAGIC_LEN] = { '.', 'D', 'A', 'T', 'E', 'O', 'F', '\n' };

#define MIN_ENTRY_CAP 16

#define is_null(p) _is_null((p), #p)

static inline bool _is_null(const void *p, const char *p_sym) {
    if (!p) {
        print_err_msg("%s: NULL ptr\n", p_sym);
        return true;
    }
    return false;
}

static const uoff32_t datfile_datasectionoff = sizeof(KVDAT_FileHeader);
static const uoff32_t datfile_datasection_payload_off = datfile_datasectionoff + MAGIC_LEN;


static inline uoff32_t entry_key_len_off(uoff32_t entryoff) {
    return entryoff + offsetof(KVDAT_EntryHeader, key_len);
}
static inline uoff32_t entry_key_data_off(uoff32_t entryoff) {
    return entryoff + sizeof(KVDAT_EntryHeader);
}
static inline uoff32_t entry_index_to_off(int idx, KVDAT_DatIndexEntry *indextable) {
    return indextable[idx].entryoff;
}
static inline bool getentrykey(FILE *fp_dat, uoff32_t entryoff, LPBuffer *out_key_p) {
    if (is_null(fp_dat)) return false;
    if (is_null(out_key_p)) return false;
    fseek(fp_dat, entry_key_len_off(entryoff), SEEK_SET);
    size_t freadcnt = fread(
        &out_key_p->len,
        sizeof(out_key_p->len),
        1,
        fp_dat
    );
    if (freadcnt != 1) {
        printerrf("fread out_key_p->len failed");
        return false;
    }
    out_key_p->data = malloc(out_key_p->len);
    if (!out_key_p->data) {
        printerrf("out_key_p->data failed");
        return false;
    }
    freadcnt = fread(
        out_key_p->data,
        1,
        out_key_p->len,
        fp_dat
    );
    if (freadcnt != out_key_p->len) {
        printerrf("fread out_key_p->data failed");
        return false;
    }
    return true;
}

static inline bool is_valid_minentrycap(ulong_t entrycap) {
    if (entrycap < MIN_ENTRY_CAP) {
        printerrf("entrycap must be at least %d\n", MIN_ENTRY_CAP);
        return false;
    }
    return true;
}

static inline FILE *KVDAT_open_datfile(const char *filepath, int openmode) {
    if (is_null(filepath)) return NULL;

    FILE *fp_dat = NULL;
    if (openmode == MODE_CREATE) {
        fp_dat = fopen(filepath, "wb+");
        if (!fp_dat) {
            printerrf("fopen fp_dat failed with wb+: %s\n", strerror(errno));
            return NULL;
        }
    } else if (openmode == MODE_OPEN) {
        fp_dat = fopen(filepath, "rb+");
        if (!fp_dat) {
            printerrf("fopen fp_dat failed with rb+: %s\n", strerror(errno));
            return NULL;
        }
    } else {
        printerrf("KVDAT_open_dat_file: invalid open mode\n");
        return NULL;
    }
    return fp_dat;
}
static inline bool KVDAT_parse_fileheader(FILE *fp_dat, KVDAT_FileHeader *out_fileheader_p) {
    if (is_null(fp_dat)) return false;
    if (is_null(out_fileheader_p)) return false;
    fseek(fp_dat, 0, SEEK_SET);
    size_t freadcnt = fread(
        out_fileheader_p,
        sizeof(KVDAT_FileHeader),
        1,
        fp_dat
    );
    if (freadcnt != 1) {
        printerrf("fread out_fileheader_p failed\n");
        return false;
    }
    if (memcmp(out_fileheader_p->magic, FILE_MAGIC, MAGIC_LEN) != 0) {
        printerrf("memcmp: Invalid out_fileheader_p->magic\n");
        return false;
    }
    return true;
}
static inline bool KVDAT_parse_eofheader(FILE *fp_dat, KVDAT_EOFHeader *out_eofheader_p) {
    // 0 on failure
    // read eof
    if (is_null(fp_dat)) return false;
    if (is_null(out_eofheader_p)) return false;

    fseek(fp_dat, -(long)sizeof(KVDAT_EOFHeader), SEEK_END);
    size_t freadcnt = fread(
        out_eofheader_p,
        sizeof(KVDAT_EOFHeader),
        1,
        fp_dat
    );
    if (freadcnt != 1) {
        printerrf("fread out_eofheader_p failed\n");
        return false;
    }
    if (memcmp(out_eofheader_p->magic, EOF_MAGIC, MAGIC_LEN) != 0) {
        printerrf("memcmp: Invalid out_eofheader_p->magic\n");
        return false;
    }
    return true;
}
static inline bool KVDAT_entrycap_is_valid(const KVDAT_FileHeader *fileheader_p, int entrycap) {
    if (is_null(fileheader_p)) return false;
    if (!is_valid_minentrycap(entrycap)) return false;
    if (entrycap < fileheader_p->entrycount) {
        printerrf(
            "Not enough capacity: entrycap < fileheader_p->entrycount\n"
            "fileheader.entrycount=%u\n",
            fileheader_p->entrycount
        );
        return false;
    }
    return true;
}
static inline KVDAT_DatIndexEntry *KVDAT_alloc_indextable(int entrycap) {
    if (!is_valid_minentrycap(entrycap)) return NULL;
    KVDAT_DatIndexEntry *indextable = (KVDAT_DatIndexEntry*)malloc(entrycap * sizeof(KVDAT_DatIndexEntry));
    if (!indextable) {
        printerrf("malloc _this->indextable failed\n");
        return NULL;
    }
    memset(indextable, 0, entrycap * sizeof(KVDAT_DatIndexEntry));
    return indextable;
}
static inline ulong_t KVDAT_load_indextable(
    FILE                   *fp_dat,
    KVDAT_FileHeader       *fileheader_p,
    KVDAT_DatIndexEntry    *indextable,
    int                     entrycap
) {
    if (!is_valid_minentrycap(entrycap)) return false;
    if (is_null(fp_dat)) return false;
    if (is_null(fileheader_p)) return false;
    if (is_null(indextable)) return false;

    fseek(fp_dat, fileheader_p->indextableoff, SEEK_SET);
    size_t freadcnt = fread(
        indextable,
        sizeof(KVDAT_DatIndexEntry),
        fileheader_p->entrycount,
        fp_dat
    );
    if (freadcnt != fileheader_p->entrycount) {
        printerrf("fread indextable failed\n");
        return false;
    }
    return freadcnt;
}
static inline LPBuffer *KVDAT_alloc_keyarr(int entrycap) {
    if (!is_valid_minentrycap(entrycap)) return false;
    LPBuffer *key_arr = (LPBuffer*)malloc(entrycap * sizeof(LPBuffer));
    if (!key_arr) {
        printerrf("malloc _this->kv_arr failed\n");
        return NULL;
    }
    memset(key_arr, 0, entrycap * sizeof(LPBuffer));
    return key_arr;
}
static inline KVDAT_InMemDataEntry *KVDAT_alloc_datentryarr(int entrycap) {
    if (!is_valid_minentrycap(entrycap)) return false;
    KVDAT_InMemDataEntry *data_entry_arr = (KVDAT_InMemDataEntry*)malloc(entrycap * sizeof(KVDAT_InMemDataEntry));
    if (!data_entry_arr) {
        printerrf("malloc dat_entry_arr failed");
        return NULL;
    }
    memset(data_entry_arr, 0, entrycap * sizeof(KVDAT_InMemDataEntry));
    return data_entry_arr;
}

static inline ulong_t KVDAT_load_keyarr(
    FILE                   *fp_dat,
    KVDAT_FileHeader       *fileheader_p,
    KVDAT_DatIndexEntry    *indextable,
    LPBuffer               *key_arr,
    int                     entrycap
) {
    if (
        !is_valid_minentrycap(entrycap) ||
        is_null(fp_dat) ||
        is_null(fileheader_p) ||
        is_null(indextable) ||
        is_null(key_arr)
    )
        return false;

    uoff32_t entryoff = 0;
    ulong_t i = 0;
    for (i = 0; i < fileheader_p->entrycount; ++i) {
        entryoff = entry_index_to_off(i, indextable);
        fseek(fp_dat, entryoff, SEEK_SET);
        if (!getentrykey(fp_dat, entryoff, &key_arr[i])) {
            printerrf("getentrykey failed at index %u", i);
            return false;
        }
    }
    return i;
}

static inline FILE *KVDAT_dup_datfile(
    FILE       *fp_dat,
    const char *filename,
    const char *filename_new
) {
    // file size is limited to 2 gb since i use 32 bit offsets
    if (is_null(fp_dat) || is_null(filename)) return NULL;
    size_t filename_len = strlen(filename);

    if (memcmp(filename, filename_new, filename_len) == 0) {
        printerrf("filename and filename_new cannot be identical\n");
        return NULL;
    }

    FILE *fp_tmp = fopen(filename_new, "wb+");
    if (!fp_tmp) {
        perror("fopen filename_new failed\n");
        return NULL;
    }
    fseek(fp_dat, 0, SEEK_END);
    long filesize = ftell(fp_dat);
    fseek(fp_dat, 0, SEEK_SET);

    byte_t file_buffer[BUFFER_SIZE] = {0};
    size_t bytes_read = 0;
    size_t bytes_written = 0;
    while (LOOP_RUNNING) {
        bytes_read = fread(file_buffer, 1, BUFFER_SIZE, fp_dat);
        bytes_written = fwrite(file_buffer, 1, bytes_read, fp_tmp);
        if (bytes_written != bytes_read) {
            printerrf("fwrite file_buffer failed\n");
            fclose(fp_tmp);
            return NULL;
        }
        if (bytes_read != BUFFER_SIZE) {
            if (feof(fp_dat)) {
                printerrf("fread fp_dat: EOF reached\n");
            } else if (ferror(fp_dat)) {
                printerrf("fread fp_dat: I/O error\n");
                fclose(fp_tmp);
                return NULL;
            }
            break;
        }
    }

    return fp_tmp;
}
static inline void KVDAT_create_init_fileheader(KVDAT_FileHeader *out_fileheader_p, dword_t flags) {
    memcpy(
        out_fileheader_p->magic,
        (byte_t*)FILE_MAGIC,
        MAGIC_LEN
    );
    out_fileheader_p->byteorder         = BYTE_ORDER_LE;            // little endian
    out_fileheader_p->version_major     = VERSION_MAJOR;
    out_fileheader_p->version_minor     = VERSION_MINOR;
    out_fileheader_p->flags             = flags;
    out_fileheader_p->entrycount        = 0;                        // it will inc
    out_fileheader_p->datasectionoff    = datfile_datasectionoff;
    out_fileheader_p->indextableoff     = datfile_datasectionoff;   // payload after datasection
    out_fileheader_p->eofheaderoff      = datfile_datasectionoff;   // eofoff - sizeof(EOFHeader) 
    out_fileheader_p->timestamp         = 0;                        // only written when closing
}
static inline void KVDAT_update_fileheader(
    KVDAT_FileHeader   *fileheader_p,
    size32_t            entrycount,
    uoff32_t            indextableoff,
    uoff32_t            eofheaderoff
) {
    fileheader_p->entrycount    = entrycount;       // it will inc
    fileheader_p->indextableoff = indextableoff;    // payload after datasection
    fileheader_p->eofheaderoff  = eofheaderoff;     // eofoff - sizeof(EOFHeader) 
}

static inline void KVDAT_update_timestamp_fileheader(KVDAT_FileHeader *fileheader_p) {
    fileheader_p->timestamp = (time_t)time(NULL); 
}


int KVDAT_init(
    KVDAT_DatFileObject    *_this,
    const char             *filename,
    int                     entrycap,
    int                     openmode
) {
    if (
        is_null(_this) ||
        is_null(filename) ||
        !is_valid_minentrycap(entrycap)
    ) 
        goto KVDAT_init_failed_return;

    memset(_this, 0, sizeof(KVDAT_DatFileObject));

KVDAT_init_alloc_filename_str:
    size_t filename_len = strlen(filename);
    _this->filename = (char*)malloc(filename_len + 1);
    if (!_this->filename) {
        printerrf("malloc _this->filename failed\n");
        goto NOT_KVDAT_init_alloc_filename_str;
    }
    memcpy(_this->filename, filename, filename_len);
    _this->filename[filename_len] = '\0';

KVDAT_init_open_datfile:
    _this->fp_dat = KVDAT_open_datfile(filename, openmode);
    if (!_this->fp_dat) {
        printerrf("KVDAT_open_datfile failed\n");
        goto NOT_KVDAT_init_alloc_filename_str;
    }

    if (openmode == MODE_OPEN) {
    KVDAT_init_OPEN_open_datfile:
        if (!KVDAT_parse_fileheader(_this->fp_dat, &_this->fileheader)) {
            printerrf("KVDAT_parse_fileheader failed\n");
            goto NOT_KVDAT_init_open_datfile;
        }
        KVDAT_EOFHeader eofheader = {0};
        if (!KVDAT_parse_eofheader(_this->fp_dat, &eofheader)) {
            printerrf("KVDAT_parse_eofheader failed\n");
            goto NOT_KVDAT_init_open_datfile;
        }
        if (!KVDAT_entrycap_is_valid(&_this->fileheader, entrycap)) {
            printerrf("Invalid entrycap\n");
            goto NOT_KVDAT_init_open_datfile;
        }
    KVDAT_init_OPEN_load_indextable:
        _this->indextable = KVDAT_alloc_indextable(entrycap);
        if (!_this->indextable) {
            printerrf("KVDAT_alloc_indextable failed\n");
            goto NOT_KVDAT_init_load_indextable;
        }
        if (
            !KVDAT_load_indextable(
                _this->fp_dat,
                &_this->fileheader,
                _this->indextable,
                entrycap
            )
        ) {
            printerrf("KVDAT_load_indextable failed\n");
            goto NOT_KVDAT_init_load_indextable;
        }
    KVDAT_init_OPEN_load_keyarr:
        _this->key_arr = KVDAT_alloc_keyarr(entrycap);
        if (!_this->key_arr) {
            printerrf("KVDAT_alloc_keyarr failed\n");
            goto NOT_KVDAT_init_load_keyarr;
        }
        if (
            !KVDAT_load_keyarr(
                _this->fp_dat,
                &_this->fileheader,
                _this->indextable,
                _this->key_arr,
                entrycap
            )
        ) {
            printerrf("KVDAT_load_keyarr failed\n");
            goto NOT_KVDAT_init_load_keyarr;
        }
        _this->modified = false;
        _this->dat_file_cur = _this->fileheader.indextableoff;
    KVDAT_init_OPEN_dup_datfile:
        _this->filename_tmp = (char*)malloc(strlen(filename) + 17);
        if (!_this->filename_tmp) {
            printerrf("malloc _this->filename_tmp failed\n");
            goto NOT_KVDAT_init_load_keyarr;
        }
        memcpy(_this->filename_tmp, filename, filename_len);
        memcpy(_this->filename_tmp + filename_len, ".backup", 8);
        _this->fp_dat_tmp = KVDAT_dup_datfile(
            _this->fp_dat, filename, _this->filename_tmp
        );
        if (!_this->fp_dat_tmp) {
            printerrf("KVDAT_dup_datfile _this->fp_dat failed");
            goto NOT_KVDAT_init_dup_datfile;
        }

    } else if (openmode == MODE_CREATE) {
    KVDAT_init_CREATE_open_datfile:
        if (!is_valid_minentrycap(entrycap)) {
            printerrf("Invalid entrycap\n");
            goto NOT_KVDAT_init_open_datfile;
        }
    KVDAT_init_CREATE_alloc_indextable:
        _this->indextable = KVDAT_alloc_indextable(entrycap);
        if (!_this->indextable) {
            printerrf("KVDAT_alloc_indextable failed\n");
            goto NOT_KVDAT_init_load_indextable;
        }
    KVDAT_init_CREATE_alloc_keyarr:
        _this->key_arr = KVDAT_alloc_keyarr(entrycap);
        if (!_this->key_arr) {
            printerrf("KVDAT_alloc_keyarr failed\n");
            goto NOT_KVDAT_init_load_keyarr;
        }
        KVDAT_create_init_fileheader(&_this->fileheader, KVDAT_FILE_DEFAULT);
        _this->modified = true;
        fseek(_this->fp_dat, datfile_datasectionoff, SEEK_SET);
        _this->dat_file_cur = datfile_datasectionoff + fwrite(
            (byte_t*)DATA_SECTION_MAGIC,
            1,
            MAGIC_LEN,
            _this->fp_dat
        );
        if (_this->dat_file_cur == datfile_datasectionoff) {
            printerrf("fwrite DATA_SECTION_MAGIC failed\n");
        }
        KVDAT_update_fileheader(
            &_this->fileheader,
            0,
            _this->dat_file_cur,
            _this->dat_file_cur
        );
        // check fwrite
    }

    print_dbg_msg("SUCCESS\n");
    _this->entrycap = entrycap;
    _this->openmode = openmode;
    return entrycap;

KVDAT_init_failed_cleanup:
    NOT_KVDAT_init_dup_datfile:
    if (_this->fp_dat_tmp) {
        fclose(_this->fp_dat_tmp);
        _this->fp_dat_tmp = NULL;
    }
    if (_this->filename_tmp) {
        free(_this->filename_tmp);
        _this->filename_tmp = NULL;
    }
    NOT_KVDAT_init_load_keyarr:
    if (_this->key_arr) {
        free(_this->key_arr);
        _this->key_arr = NULL;
    }
    NOT_KVDAT_init_load_indextable:
    if (_this->indextable) {
        free(_this->indextable);
        _this->indextable = NULL;
    }
    NOT_KVDAT_init_open_datfile:
    if (_this->fp_dat) {
        fclose(_this->fp_dat);
        _this->fp_dat = NULL;
    }
    NOT_KVDAT_init_alloc_filename_str:
    if (_this->filename) {
        free(_this->filename);
        _this->filename = NULL;
    }

KVDAT_init_failed_return:
    return KVDAT_INIT_FAILED;
}


int KVDAT_insert_key(KVDAT_DatFileObject *_this, LPBuffer *key, int idx) {
    if (is_null(_this) || is_null(key)) return -1;
    _this->key_arr[idx].len = key->len;
    _this->key_arr[idx].data = malloc(key->len);
    if (!_this->key_arr[idx].data) {
        printerrf("malloc _this->key_arr[%d].data failed\n", idx);
        return -1;
    }
    memcpy(_this->key_arr[idx].data, key->data, key->len);
    return idx;
}

int KVDAT_insert_entry(
    KVDAT_DatFileObject *_this,
    LPBuffer            *key,
    TLVDataObject       *val
) {


}


static inline int KVDAT_write_indextable(
    FILE                   *fp_dat,
    KVDAT_FileHeader       *fileheader_p,
    KVDAT_DatIndexEntry    *indextable
) {
    if (
        is_null(fp_dat) ||
        is_null(fileheader_p) ||
        is_null(indextable)
    )
        return -1;
    fseek(fp_dat, fileheader_p->indextableoff, SEEK_SET);
    size_t written = fwrite(
        indextable,
        sizeof(KVDAT_DatIndexEntry),
        fileheader_p->entrycount,
        fp_dat
    );
    if (written != fileheader_p->entrycount) {
        printerrf("fwrite indextable failed\n");
        return -1;
    }
    return (int)written;
}
static inline int KVDAT_write_eofheader(FILE *fp_dat, KVDAT_FileHeader *fileheader_p) {
    if (is_null(fp_dat) || is_null(fileheader_p)) return -1;
    fseek(fp_dat, fileheader_p->eofheaderoff, SEEK_SET);
    KVDAT_EOFHeader eofheader = {0};
    memcpy(eofheader.magic, EOF_MAGIC, MAGIC_LEN);
    eofheader.crc32;    // reserved for future
    size_t written = fwrite(
        &eofheader,
        sizeof(KVDAT_EOFHeader),
        1,
        fp_dat
    );
    if (written != 1) {
        printerrf("fwrite fileheader_p failed\n");
        return -1;
    }
    return (int)written;
}
static inline int KVDAT_write_fileheader(FILE *fp_dat, KVDAT_FileHeader *fileheader_p) {
    if (is_null(fp_dat) || is_null(fileheader_p))
        return -1;
    fseek(fp_dat, 0, SEEK_SET);
    KVDAT_update_timestamp_fileheader(fileheader_p);
    size_t written = fwrite(
        fileheader_p,
        sizeof(KVDAT_FileHeader),
        1,
        fp_dat
    );
    if (written != 1) {
        printerrf("fwrite fileheader_p failed\n");
        return -1;
    }
    return (int)written;
}
bool KVDAT_commit(KVDAT_DatFileObject *_this) {
    if (is_null(_this)) return -1;
    if (!_this->modified) {
        printerrf("datfile unmodified!\n");
        return false;
    }
    int ret = KVDAT_write_indextable(
        _this->fp_dat,
        &_this->fileheader,
        _this->indextable
    );
    if (ret < 0) {
        printerrf("KVDAT_write_indextable failed\n");
        return false;
    }
    ret = KVDAT_write_eofheader(_this->fp_dat, &_this->fileheader);
    if (ret < 0) {
        printerrf("KVDAT_write_eofheader failed\n");
        return false;
    }
    ret = KVDAT_write_fileheader(_this->fp_dat, &_this->fileheader);
    if (ret < 0) {
        printerrf("KVDAT_write_fileheader failed\n");
        return false;
    }

    return true;
}

void KVDAT_deinit(KVDAT_DatFileObject *_this) {
    if (is_null(_this)) return;

NOT_KVDAT_init_load_keyarr:
    if (_this->key_arr) {
        free(_this->key_arr);
        _this->key_arr = NULL;
    }
NOT_KVDAT_init_load_indextable:
    if (_this->indextable) {
        free(_this->indextable);
        _this->indextable = NULL;
    }
NOT_KVDAT_init_dup_datfile:
    if (_this->fp_dat_tmp) {
        free(_this->fp_dat_tmp);
        _this->fp_dat_tmp = NULL;
    }
NOT_KVDAT_init_open_datfile:
    if (_this->fp_dat) {
        fclose(_this->fp_dat);
        _this->fp_dat = NULL;
    }
    if (_this->openmode == MODE_OPEN) {
        if (remove(_this->filename_tmp) != 0) {
            perror("remove(_this->filename_tmp)");
            goto NOT_KVDAT_init_alloc_filenames;
        }
    }

NOT_KVDAT_init_alloc_filenames:
    if (_this->filename_tmp) {
        free(_this->filename_tmp);
        _this->filename_tmp = NULL;
    }
    if (_this->filename) {
        free(_this->filename);
        _this->filename = NULL;
    }

    memset(_this, 0, sizeof(KVDAT_DatFileObject));
    return;
}
