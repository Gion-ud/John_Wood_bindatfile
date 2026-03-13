#pragma once

#include <global.h>
#include <global_intdef.h>
#include <typeflags.h>
#include <zlib.h>
#include <libc_chked.h>
#include "internal_types.h"
#include "mem_types.h"



static uoff32_t indexfile_stringtableoff = sizeof(INDEX_FILE_HEADER);

static inline void INDEX_init_fileheader(INDEX_FILE_HEADER *out_fileheader_p, dword_t flags) {
    //memcpy(&out_fileheader_p->magic, (byte_t*)FILE_MAGIC, sizeof(qword_t));
    out_fileheader_p->magic             = *(qword_t*)((byte_t*)FILE_MAGIC); // this is assignment not copy
    out_fileheader_p->version_major     = VERSION_MAJOR;
    out_fileheader_p->version_minor     = VERSION_MINOR;
    out_fileheader_p->flags             = flags;
    out_fileheader_p->headersize        = sizeof(INDEX_FILE_HEADER);
    out_fileheader_p->footersize        = sizeof(INDEX_FILE_FOOTER);
    out_fileheader_p->stringtableoff    = indexfile_stringtableoff; //
    out_fileheader_p->indextableoff     = indexfile_stringtableoff; // payload after datasection
    out_fileheader_p->footeroff         = indexfile_stringtableoff; // eofoff - sizeof(EOFHeader) 
    out_fileheader_p->entrycount        = 0;                        // it will inc
    out_fileheader_p->timestamp         = 0;                        // only written when closing
}

static inline void INDEX_update_fileheader(
    INDEX_FILE_HEADER  *fileheader_p,
    size32_t            entrycount,
    uoff32_t            indextableoff,
    uoff32_t            footeroff,
    longlong_t          timestamp
) {
    fileheader_p->entrycount        = entrycount;       // it will inc
    fileheader_p->indextableoff     = indextableoff;
    fileheader_p->footeroff         = footeroff;        // eofoff - sizeof(EOFHeader)
    fileheader_p->timestamp         = timestamp;
}


