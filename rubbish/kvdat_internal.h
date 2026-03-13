#ifndef KVDAT_INTERNAL_H
#define KVDAT_INTERNAL_H

#include <global.h>
#include <global_intdef.h>
#include <typeflags.h>
#include "kvdat.h"

enum ByteOrder {
    BYTE_ORDER_LE = 0,
    BYTE_ORDER_BE = 1,
};

enum KVDAT_FileFlags {
    KVDAT_FILE_DEFAULT           = 0,
    KVDAT_FILE_COMPRESSED        = (1u << 0),
    KVDAT_FILE_ENCRYPTED         = (1u << 1),
    KVDAT_FILE_HAS_INDEX_TABLE   = (1u << 2),
};

enum KVDAT_EntryFlags {
    ENTRY_EMPTY         = 0,
    ENTRY_VALID         = (1u << 0),
    ENTRY_DELETED       = (1u << 1),
    ENTRY_COMPRESSED    = (1u << 2),
    ENTRY_ENCRYPTED     = (1u << 3),
};

#define MAGIC_LEN       8
#define VERSION_MAJOR   1
#define VERSION_MINOR   0
#define ENDIAN          BYTE_ORDER_LE

#pragma pack(push, 1)

// FileHeader
typedef struct _dat_file_header {
    byte_t      magic[MAGIC_LEN];   // 8
    byte_t      byteorder;          // 1
    byte_t      version_major;      // 1
    byte_t      version_minor;      // 1
    dword_t     flags;              // 4
    size32_t    entrycount;         // 4
    uoff32_t    datasectionoff;     // 4
    uoff32_t    indextableoff;      // 4
    uoff32_t    eofheaderoff;       // 4
    longlong_t  timestamp;          // 8
    byte_t      reserved;           // 1
    qword_t     reserved1;
    qword_t     reserved2;
} KVDAT_FileHeader;

typedef byte_t KVDAT_DataSectionHeader[MAGIC_LEN];
//  byte_t      data_section_payload[]


// Entry
typedef struct _entry_header {
    byte_t      magic[MAGIC_LEN];   // 8
    byte_t      key_len;
} KVDAT_EntryHeader;
//  byte_t      key_payload[key_len]
typedef struct _entry_data_header {
    dword_t     flags;
    dword_t     type;
    size32_t    len;
} KVDAT_EntryDataHeader;
//  byte_t      data_payload[len]
typedef struct _entry_inmem {
    KVDAT_EntryDataHeader   header;
    byte_t                 *payload;
} KVDAT_InMemDataEntry;


typedef struct _eof_header {
    ulong_t     crc32;
    byte_t      magic[MAGIC_LEN];   // 8
} KVDAT_EOFHeader;


typedef struct _dat_idx_entry {
    dword_t     flags;
    qword_t     keyhash;
    ulong_t     entrylen;
    uoff32_t    entryoff;
} KVDAT_DatIndexEntry;


#pragma pack(pop)

typedef struct _lp {
    size32_t    len;
    void       *data;
} LPBuffer;

typedef struct _dat_file_obj {
    FILE                   *fp_dat;
    FILE                   *fp_dat_tmp;
    char                   *filename; // must be inited n freed
    char                   *filename_tmp; // must be inited n freed
//  size_t                  filename_len;
    KVDAT_FileHeader        fileheader;
    KVDAT_DatIndexEntry    *indextable;
    LPBuffer               *key_arr;
    KVDAT_InMemDataEntry   *data_entry_arr;
    size32_t                entrycap;
    int                     openmode;
    bool                    modified;
    uoff32_t                dat_file_cur;
} KVDAT_DatFileObject;



#endif