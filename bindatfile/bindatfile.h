#pragma once

#include "mem_types.h"
#include <libc_chked.h>

int DAT_FILE_OBJECT_init(
    DAT_FILE_OBJECT    *_this,
    FILE               *fp,
    size32_t            entrycap,
    ulong_t             flags
);
bool DAT_FILE_load_sections(DAT_FILE_OBJECT *_this);
bool DAT_FILE_validate_integrity(
    DAT_FILE_HEADER    *this_fileheader,
    DAT_FILE_FOOTER    *this_filefooter,
    FILE               *this_file_p
);


int DAT_FILE_read_entry(
    DAT_FILE_OBJECT    *_this,
    ulong_t             idx,
    DAT_ENTRY_HEADER   *entry_header_p,
    byte_t             *entry_data
);
int DAT_FILE_write_entry(
    DAT_FILE_OBJECT    *_this,
    DAT_ENTRY_HEADER   *entry_header_p,
    byte_t             *entry_data
);
int DAT_FILE_delete_entry(DAT_FILE_OBJECT *_this, ulong_t idx);


bool DAT_FILE_OBJECT_commit(DAT_FILE_OBJECT *_this);
void DAT_FILE_OBJECT_deinit(DAT_FILE_OBJECT *_this);



// inlined functions below
static inline int DAT_get_datfile_entrycount(FILE *fp) {
    if (is_null(fp)) return -1;
    fseek(fp, offsetof(DAT_FILE_HEADER, entrycount), SEEK_SET);
    size32_t entrycount = 0;
    if (
        fread_checked(&entrycount, sizeof(size32_t), 1, fp) < 0
    )
        return -1;
    return (int)entrycount;
}