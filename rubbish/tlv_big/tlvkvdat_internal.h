#ifndef TLVKVDAT_INTERNAL_H
#define TLVKVDAT_INTERNAL_H

#include <global.h>
#include <global_intdef.h>
#include <typeflags.h>
#include "tlvkvdat.h"

enum ByteOrder {
    BYTE_ORDER_LE = 0,
    BYTE_ORDER_BE = 1,
};

enum TLVKVDAT_FileFlags {
    TLVKVDAT_FILE_DEFAULT           = 0,
    TLVKVDAT_FILE_COMPRESSED        = (1u << 0),
    TLVKVDAT_FILE_ENCRYPTED         = (1u << 1),
    TLVKVDAT_FILE_HAS_INDEX_TABLE   = (1u << 2),
};

enum TLVKVDAT_EntryFlags {
    ENTRY_EMPTY         = 0,
    ENTRY_VALID         = (1u << 0),
    ENTRY_DELETED       = (1u << 1),
    ENTRY_COMPRESSED    = (1u << 2),
    ENTRY_ENCRYPTED     = (1u << 3),
};

#define MAGIC_LEN       8
#define VERSION_MAJOR   1
#define VERSION_MINOR   0
#define BYTE_ORDER      BYTE_ORDER_LE

#pragma pack(push, 1)

typedef struct _dat_file_header {
    byte_t      magic[MAGIC_LEN];   // 8
    byte_t      byteorder;          // 1
    byte_t      version_major;      // 1
    byte_t      version_minor;      // 1
    dword_t     flags;              // 4
    size32_t    entrycount_total;   // 4
    size32_t    entrycount_invalid; // 4
    uoff32_t    datasectionoff;     // 4
    uoff32_t    eofheaderoff;       // 4
    size32_t    eofheadersize;      // 4
    longlong_t  timestamp;          // 8
    byte_t      reserved[5];        // 5
    qword_t     reserved1;
    qword_t     reserved2;
} TLVKVDAT_FileHeader;

typedef struct _data_section_header {
    byte_t      magic[MAGIC_LEN];   // 8
} TLVKVDAT_DataSectionHeader;
//  byte_t      data_section_payload[]


typedef struct _entry_magic {
    byte_t      magic[MAGIC_LEN];   // 8
} TLVKVDAT_EntryMagicHeader;        // Optional
/*
    TLVKVDAT_TLVDataHeader  key_header
    byte_t                  key_payload[key_header.len]
    TLVKVDAT_TLVDataHeader  val_header
    byte_t                  val_payload[val_header.len]
*/

typedef struct _tlv_header {
    dword_t     type;
    size32_t    len;
} TLVKVDAT_TLVDataHeader;
//  byte_t      payload[TLVKVDAT_TLVDataHeader::len]


typedef struct _eof_header {
    byte_t      magic[MAGIC_LEN];   // 8
    ulong_t     crc32;
} TLVKVDAT_EOFHeader;


typedef struct _dat_idx_entry {
//  ulong_t     id;
    dword_t    *flags;
    qword_t     keyhash;
    ulong_t     entrylen;
    uoff32_t    entryoff;
} TLVKVDAT_DatIndexEntry;


#pragma pack(pop)

typedef struct _kv_kair {
    TLVDataObject key;
    TLVDataObject val;
} TLVKVDAT_KV;


typedef struct _dat_file_obj {
    FILE                   *fp_dat;
    FILE                   *fp_idx;
    char                   *filename;
    TLVKVDAT_FileHeader     fileheader;
    TLVKVDAT_KV            *kv_arr;
    TLVKVDAT_DatIndexEntry *indextable;
    size32_t                entrycap;
    int                     opened;
    int                     openmode;
    int                     modified;
    uoff32_t                dat_file_cur;
} TLVKVDAT_DatFileObject;



#endif