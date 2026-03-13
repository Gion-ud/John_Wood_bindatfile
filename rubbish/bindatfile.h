#ifndef BINDATFILE_H
#define BINDATFILE_H


#include <global.h>
#include <global_intdef.h>
#include <typeflags.h>

enum ByteOrder {
    BYTE_ORDER_LE = 0,
    BYTE_ORDER_BE = 1,
};

enum FileFlags {
    FILE_DEFAULT            = 0,
    FILE_COMPRESSED         = (1u << 0),
    FILE_ENCRYPTED          = (1u << 1),
    FILE_HAS_INDEX_TABLE    = (1u << 2),
};

enum EntryFlags {
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

static const byte_t FILE_MAGIC[MAGIC_LEN] = { 0xBB, 'D', 'A', 'T', 0x03, '\r', '\n', '\0' };
static const byte_t DATA_SECTION_MAGIC[MAGIC_LEN] = { '.', 'D', 'A', 'T', 'A', '\r', '\n', '\0' };
static const byte_t ENTRY_MAGIC[MAGIC_LEN] = { '.', 'E', 'N', 'T', 'R', 'Y', '\r', '\n' };
static const byte_t EOF_MAGIC[MAGIC_LEN] = { '\0', '\n', '.', 'E', 'O', 'F', '\r', '\n' };

#define MIN_ENTRY_CAP 16


#pragma pack(push, 1)

// FileHeader
typedef struct _dat_fileheader {
    byte_t      magic[MAGIC_LEN];   // 8
    dword_t     flags;              // 4
    byte_t      version_major;      // 1
    byte_t      version_minor;      // 1
    size32_t    headersize;
    size32_t    entrycount;         // 4
    uoff32_t    indextableoff;      // 4
    uoff32_t    datasectionoff;     // 4
    uoff32_t    eofheaderoff;       // 4
    longlong_t  timestamp;          // 8
    word_t      reserved;           // 2
    dword_t     reserved1;          // 4
} DAT_FILE_HEADER;

//  typedef byte_t KVDAT_DataSectionHeader[MAGIC_LEN];
//  byte_t      data_section_payload[]

typedef struct _dat_entryheader {
    word_t      flags;
    dword_t     type;
    size32_t    len;
} DAT_ENTRY_HEADER;
//  byte_t      data_payload[len]


typedef struct _eof_header {
    ulong_t     crc32;
    byte_t      magic[MAGIC_LEN];   // 8
} DAT_EOF_HEADER;

typedef struct _dat_idx_entry {
    qword_t     keyhash;
    ulong_t     entrylen;
    uoff32_t    entryoff;
} DAT_INDEX_ENTRY;

// EndOfPack
#pragma pack(pop)

typedef struct _lp_buf {
    size32_t    len;
    void       *data;
} LPBuffer;

typedef struct _tlv_obj {
    dword_t     type;
    size32_t    len;
    void       *data;
} TLVDataBuffer;

enum KVDAT_OpenMode {
    MODE_CREATE    = 0,
    MODE_OPEN      = 1,
};

typedef struct _entry_inmem {
    DAT_ENTRY_HEADER        header;
    byte_t                 *payload;
} DAT_ENTRY_INMEM;

typedef struct _dat_file_obj {
    FILE                   *fp;
    FILE                   *fp_tmp;
    char                   *filename; // must be inited n freed
    char                   *filename_tmp; // must be inited n freed
    DAT_FILE_HEADER         fileheader;
    DAT_INDEX_ENTRY        *indextable;
    LPBuffer               *key_arr;
    DAT_ENTRY_INMEM        *entry_arr;
    uoff32_t               *entry_relative_offtable;
    size32_t                entrycap;
    bool                    modified;
    uoff32_t                dat_file_cur;
} DAT_FILE_OBJECT;


static inline uoff32_t datfile_indextableoffset = sizeof(DAT_FILE_HEADER);
static inline uoff32_t datfile_get_datasectionoff(size32_t entrycount) {
    return sizeof(DAT_FILE_HEADER) + entrycount * sizeof(DAT_INDEX_ENTRY);
}
static inline uoff32_t datfile_get_datasection_payload_off(size32_t entrycount) {
    return sizeof(DAT_FILE_HEADER) + entrycount * sizeof(DAT_INDEX_ENTRY) + sizeof(DATA_SECTION_MAGIC);
}





FILE *fopen_checked(const char *filename, const char *openmode);
FILE *fclose_checked(FILE *fp);
void *malloc_checked(size_t mem_size);
void *malloc_checked(size_t mem_size);
longlong_t fread_checked(void *buf, size_t type_size, size_t cnt, FILE *fp);
longlong_t fwrite_checked(void *buf, size_t type_size, size_t count, FILE *fp);


static inline void DAT_init_fileheader(DAT_FILE_HEADER *out_fileheader_p, dword_t flags) {
    memcpy(
        out_fileheader_p->magic,
        (byte_t*)FILE_MAGIC,
        MAGIC_LEN
    );
    out_fileheader_p->version_major     = VERSION_MAJOR;
    out_fileheader_p->version_minor     = VERSION_MINOR;
    out_fileheader_p->headersize        = sizeof(DAT_FILE_HEADER);
    out_fileheader_p->flags             = flags;
    out_fileheader_p->entrycount        = 0;                        // it will inc
    out_fileheader_p->indextableoff     = datfile_indextableoffset; // payload after datasection
    out_fileheader_p->datasectionoff    = 0;                        //
    out_fileheader_p->eofheaderoff      = 0;                        // eofoff - sizeof(EOFHeader) 
    out_fileheader_p->timestamp         = 0;                        // only written when closing
}
static inline void KVDAT_update_fileheader(
    DAT_FILE_HEADER    *fileheader_p,
    size32_t            entrycount,
    uoff32_t            datasectionoff,
    uoff32_t            eofheaderoff,
    longlong_t          time
) {
    fileheader_p->entrycount        = entrycount;       // it will inc
    fileheader_p->datasectionoff    = datasectionoff;   // it will inc
    fileheader_p->eofheaderoff      = eofheaderoff;     // eofoff - sizeof(EOFHeader) 
}

static inline void KVDAT_update_timestamp_fileheader(DAT_FILE_HEADER *fileheader_p) {
    fileheader_p->timestamp = (time_t)time(NULL); 
}

#endif