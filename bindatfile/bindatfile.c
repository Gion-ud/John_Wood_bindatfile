#include "bindatfile.h"
#include "internal.h"

int DAT_FILE_OBJECT_init(
    DAT_FILE_OBJECT    *_this,
    FILE               *fp,
    size32_t            entrycap,
    ulong_t             flags
) {
    if (is_null(_this) || is_null(fp)) goto DAT_FILE_OBJECT_init_failed;
    _this->fp = fp;
    DAT_init_fileheader(&_this->fileheader, flags);
    _this->offtable = (uoff32_t*)malloc_checked(
        entrycap * sizeof(uoff32_t)
    );
    if (!_this->offtable) goto DAT_FILE_OBJECT_init_failed;

    _this->dat_file_cur = datfile_datasectionoff;
    _this->entrycap = entrycap;

    return (int)entrycap;
DAT_FILE_OBJECT_init_failed:
    DAT_FILE_OBJECT_deinit(_this);
    return -1;
}

bool DAT_FILE_load_sections(DAT_FILE_OBJECT *_this) {
    if (
        is_null(_this) || is_null(_this->fp) || is_null(_this->offtable)
    )
        return false;
    if (
        !DAT_FILE_load_fileheader(
            _this->fp,
            &_this->fileheader
        )
    )
        return false; // inheritance
    if (
        !DAT_FILE_load_filefooter(
            _this->fp,
            &_this->fileheader,
            &_this->filefooter
        )
    )
        return false;
    if (
        !DAT_FILE_load_offtable(
            _this->fp,
            &_this->fileheader,
            _this->offtable
        )
    )
        return false;
    return true;
}

ulong_t DAT_FILE_compute_crc32(
    FILE       *this_file_p,
    uoff32_t    src_off,
    uoff32_t    dest_off
) {
    if (is_null(this_file_p)) return 0;
    ulong_t f_crc32 = (ulong_t)crc32(0L, Z_NULL, 0);
    byte_t buffer[BUFFER_SIZE] = {0};
    size_t n = 0;
    fseek(this_file_p, src_off, SEEK_SET);
    for (uoff32_t i = 0; i < dest_off / BUFFER_SIZE; ++i) {
        n = fread(buffer, 1, BUFFER_SIZE, this_file_p);
        if (!n) break;
        f_crc32 = crc32(f_crc32, (byte_t*)buffer, n);
    }
    size_t rem = dest_off % BUFFER_SIZE;
    if (rem) {
        n = fread(buffer, 1, rem, this_file_p);
        if (n) {
            f_crc32 = crc32(f_crc32, (byte_t*)buffer, n);
        }
    }
    return f_crc32;
}

bool DAT_FILE_validate_integrity(
    DAT_FILE_HEADER    *this_fileheader,
    DAT_FILE_FOOTER    *this_filefooter,
    FILE               *this_file_p
) {
    if (
        is_null(this_fileheader) ||
        is_null(this_filefooter) ||
        is_null(this_file_p)
    )
        return false;

    if (
        memcmp(
            &this_fileheader->magic, (char*)FILE_MAGIC, MAGIC_LEN
        ) != 0
    ) {
        printerrf("Invalid file magic\n");
        return false;
    }

    if (
        memcmp(
            &this_filefooter->magic, (char*)EOF_MAGIC, MAGIC_LEN
        ) != 0
    ) {
        printerrf("Invalid eof magic\n");
        return false;
    }


    ulong_t crc = DAT_FILE_compute_crc32(
        this_file_p, 0, this_fileheader->footeroff
    );
    if (this_filefooter->crc32 != crc) {
        printerrf("file corrupted: crc32 mismatch\n");
        return false;
    }
    return true;
}

static inline int DAT_FILE_offsettable_insert(
    uoff32_t           *offsetable,
    ulong_t             idx,
    uoff32_t            entryoff
) {
    if (is_null(offsetable)) return -1;
    offsetable[idx] = entryoff;
    return (int)idx;
}

int DAT_FILE_write_entry(
    DAT_FILE_OBJECT    *_this,
    DAT_ENTRY_HEADER   *entry_header_p,
    byte_t             *entry_data
) {
    if (
        is_null(_this) ||
        is_null(entry_header_p) ||
        is_null(entry_data)
    )
        return -1;
    ulong_t idx = _this->fileheader.entrycount;
    if (idx >= _this->entrycap) {
        printerrf("dat entry capacity full\n");
        return -1;
    }
    uoff32_t entryoff = _this->dat_file_cur;

    fseek(_this->fp, entryoff, SEEK_SET);
    if (
        fwrite_checked(
            entry_header_p, sizeof(DAT_ENTRY_HEADER), 1, _this->fp
        ) != 1
    ) {
        printerrf("Failed to write entryheader\n");
        return -1;
    }
    if (
        fwrite_checked(
            entry_data, 1, entry_header_p->len, _this->fp
        ) != entry_header_p->len
    ) {
        printerrf("Failed to write data\n");
        return -1;
    }
    if (
        DAT_FILE_offsettable_insert(
            _this->offtable,
            idx,
            entryoff
        ) < 0
    ) {
        printerrf("Failed to insert indexentry\n");
        return -1;
    }

    _this->dat_file_cur += (sizeof(DAT_ENTRY_HEADER) + entry_header_p->len);
    return (int)_this->fileheader.entrycount++;
}

int DAT_FILE_delete_entry(DAT_FILE_OBJECT *_this, ulong_t idx) {
    if (is_null(_this)) return false;
    if (idx >= _this->entrycap) {
        printerrf("dat entry capacity full\n");
        return -1;
    }
    _this->offtable[idx] = 0;
    return (int)idx;
}

int DAT_FILE_read_entry(
    DAT_FILE_OBJECT    *_this,
    ulong_t             idx,
    DAT_ENTRY_HEADER   *entry_header_p,
    byte_t             *entry_data
) {
    if (
        is_null(_this) ||
        is_null(entry_header_p) ||
        is_null(entry_data)
    )
        return -1;

    if (idx > _this->entrycap) {
        printerrf("idx out of bound\n");
        return -1;
    }
    if (!_this->offtable[idx]) {
        printerrf("Entry[%u] is not valid\n", idx);
        return -1;
    }
    uoff32_t entryoff = _this->offtable[idx];
    fseek(_this->fp, entryoff, SEEK_SET);
    if (
        fread_checked(
            entry_header_p, sizeof(DAT_ENTRY_HEADER), 1, _this->fp
        ) < 0
    )
        return -1;

    if (
        fread_checked(
            entry_data, 1, entry_header_p->len, _this->fp
        ) < 0
    )
        return -1;

    return (int)idx;
}


bool DAT_FILE_OBJECT_commit(DAT_FILE_OBJECT *_this) {
    if (is_null(_this)) return -1;
    size32_t entrycount = _this->fileheader.entrycount;
    uoff32_t indextableoff = (_this->dat_file_cur + 3) &~3;
    //printerrf("this.dat_file_cur=%u\n", indextableoff);
    uoff32_t filefooteroff = indextableoff + entrycount * sizeof(uoff32_t);

    fseek(_this->fp, indextableoff, SEEK_SET);
    if (
        fwrite_checked(
            _this->offtable, sizeof(uoff32_t), entrycount, _this->fp
        ) < 0
    )
        return false;

    DAT_update_fileheader(
        &_this->fileheader,
        entrycount,
        indextableoff,
        _this->fileheader.datasectionoff,
        filefooteroff,
        time(NULL)
    );
    fseek(_this->fp, 0, SEEK_SET);
    if (
        fwrite_checked(
            &_this->fileheader, sizeof(DAT_FILE_HEADER), 1, _this->fp
        ) < 0
    )
        return false;

    //memcpy(&filefooter.magic, (byte_t*)EOF_MAGIC, sizeof(qword_t));
    _this->filefooter.magic = *(qword_t*)((byte_t*)EOF_MAGIC);
    _this->filefooter.crc32 = DAT_FILE_compute_crc32(_this->fp, 0, _this->fileheader.footeroff);

    fseek(_this->fp, filefooteroff, SEEK_SET);
    if (
        fwrite_checked(
            &_this->filefooter, sizeof(DAT_FILE_FOOTER), 1, _this->fp
        ) < 0
    )
        return false;

    return true;
}

void DAT_FILE_OBJECT_deinit(DAT_FILE_OBJECT *_this) {
    // file must be closed manually

    if (_this->offtable) {
        _this->offtable = free_checked(_this->offtable);
        _this->offtable = NULL;
    }
    memset(&_this->fileheader, 0, sizeof(DAT_FILE_HEADER));
    memset(&_this->filefooter, 0, sizeof(DAT_FILE_FOOTER));
    _this->entrycap = 0;
    _this->dat_file_cur = 0;
}