#pragma once

#include <typeflags.h>
#include <global_intdef.h>

#define MAGIC_LEN 8

enum FileFlags {
    FILE_DEFAULT    = 0,
    FILE_COMPRESSED = (1u << 0),
    FILE_ENCRYPTED  = (1u << 1),
};

enum EntryFlags {
    ENTRY_DELETED       = (1u << 0),
    ENTRY_COMPRESSED    = (1u << 1),
    ENTRY_ENCRYPTED     = (1u << 2),
};


#pragma pack(push, 1)

// FileHeader
typedef struct _idx_fileheader {
    qword_t     magic;          // 8
    byte_t      version_major;  // 1
    byte_t      version_minor;  // 1
    word_t      flags;          // 2
    size32_t    headersize;     // 4
    size32_t    footersize;     // 4
    uoff32_t    stringtableoff; // 4
    uoff32_t    indextableoff;  // 4
    uoff32_t    footeroff;      // 4
    word_t      entrycount;     // 4
    longlong_t  timestamp;      // 8
} INDEX_FILE_HEADER;

typedef struct _idx_filefooter {
    ulong_t     crc32;  // 4
    qword_t     magic;  // 8
} INDEX_FILE_FOOTER;

typedef struct _idx_entry {
    dword_t     key_hash;
    size32_t    key_len;
    uoff32_t    key_off;
    uoff32_t    data_off;
    word_t      flags;
    word_t      reserved;
} INDEX_ENTRY;


//  byte_t      entry_payload[len];


/*
internal layout:
[FileHeader]
[Entries]
    [EntryHeader][payload]
    ...d
[OffsetTable[]]
[FileFooter]
*/

// EndOfPack

#pragma pack(pop)
