#pragma once

#include <global.h>
#include <global_intdef.h>
#include <typeflags.h>
#include <zlib.h>
#include "libc_chked.h"
#include "internal_types.h"
#include "mem_types.h"



static uoff32_t datfile_datasectionoff = sizeof(DAT_FILE_HEADER);


static inline void DAT_init_fileheader(DAT_FILE_HEADER *out_fileheader_p, dword_t flags) {
    //memcpy(&out_fileheader_p->magic, (byte_t*)FILE_MAGIC, sizeof(qword_t));
    out_fileheader_p->magic             = *(qword_t*)((byte_t*)FILE_MAGIC); // this is assignment not copy
    out_fileheader_p->version_major     = VERSION_MAJOR;
    out_fileheader_p->version_minor     = VERSION_MINOR;
    out_fileheader_p->flags             = flags;
    out_fileheader_p->headersize        = sizeof(DAT_FILE_HEADER);
    out_fileheader_p->footersize        = sizeof(DAT_FILE_FOOTER);
    out_fileheader_p->entrycount        = 0;                        // it will inc
    out_fileheader_p->offtableoff       = datfile_datasectionoff;   // payload after datasection
    out_fileheader_p->datasectionoff    = datfile_datasectionoff;   //
    out_fileheader_p->footeroff         = datfile_datasectionoff;   // eofoff - sizeof(EOFHeader) 
    out_fileheader_p->timestamp         = 0;                        // only written when closing
}

static inline void DAT_update_fileheader(
    DAT_FILE_HEADER    *fileheader_p,
    size32_t            entrycount,
    uoff32_t            offtableoff,
    uoff32_t            datasectionoff,
    uoff32_t            footeroff,
    longlong_t          timestamp
) {
    fileheader_p->entrycount        = entrycount;       // it will inc
    fileheader_p->offtableoff       = offtableoff;
    fileheader_p->datasectionoff    = datasectionoff;
    fileheader_p->footeroff         = footeroff;        // eofoff - sizeof(EOFHeader)
    fileheader_p->timestamp         = timestamp;
}

static inline bool DAT_FILE_load_fileheader(
    FILE               *datfile_p,
    DAT_FILE_HEADER    *out_fileheader_p
) {
    if (is_null(datfile_p) || is_null(out_fileheader_p))
        return false;

    fseek(datfile_p, 0, SEEK_SET);
    if (
        fread_checked(
            out_fileheader_p, sizeof(DAT_FILE_HEADER), 1, datfile_p
        ) < 0
    )
        return false;

    return true;
}


static inline uoff32_t *DAT_FILE_load_offtable(
    FILE               *datfile_p,
    DAT_FILE_HEADER    *fileheader_p,
    uoff32_t           *out_offtable
) {
    if (
        is_null(datfile_p) ||
        is_null(out_offtable) ||
        is_null(fileheader_p)
    )
        return NULL;

    fseek(datfile_p, fileheader_p->offtableoff, SEEK_SET);
    if (
        fread_checked(
            out_offtable, sizeof(uoff32_t), fileheader_p->entrycount, datfile_p
        ) < 0
    )
        return NULL;

    return out_offtable;
}

static inline bool DAT_FILE_load_filefooter(
    FILE               *datfile_p,
    DAT_FILE_HEADER    *fileheader_p,
    DAT_FILE_FOOTER    *out_filefooter_p
) {
    if (
        is_null(datfile_p) ||
        is_null(fileheader_p) ||
        is_null(out_filefooter_p)
    )
        return false;

    fseek(datfile_p, fileheader_p->footeroff, SEEK_SET);
    if (
        fread_checked(
            out_filefooter_p, sizeof(DAT_FILE_FOOTER), 1, datfile_p
        ) < 0
    )
        return false;

    return true;
}


